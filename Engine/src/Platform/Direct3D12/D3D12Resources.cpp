
#include "D3D12Resources.h"
#include "D3D12Renderer.h"
#include "D3D12Macros.h"

namespace Engine::Graphics::D3D12
{
#pragma region Descriptor Heap
	// D3D12 API를 활용해 Descriptor Heap을 생성하는 초기화 클래스.
	bool Descriptor_Heap::Initialize(uint32 capacity, bool is_shader_visible)
	{
		// 여러 thread를 사용하는 경우, 리소스를 로드하거나 생성하는 작업이 여러 thread에서 일어나게 됨.
		// 리소스를 생성하고, 이를 파이프라인에 바인딩 하기 위해 descriptor를 만드는 과정이 여러 thread에서 일어나고, 
		// 이에 따라 동일한 descriptor_heap 클래스 인스턴스가 여러 thread에 의해 접근될 수 있음. 이 때 일어나는 race condition을 막기 위해 lock을 걸어줌. 
		std::lock_guard lock{ _mutex };

		// heap type에 따라 heap의 최대 수용 크기를 체크함.
		assert(capacity && capacity < D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2);
		assert(!(_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER && capacity > D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE));

		// heap tyep에 따라 shader visible 여부가 결정됨. DSV, RTV descriptor를 위한 heap의 경우, GPU가 아닌 CPU 메모리에 할당됨.
		if (_type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV ||
			_type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		{
			is_shader_visible = false;
		}

		// 이전에 heap을 생성해 주었다면, 해제 하고 다시 만들어 줄 수 있도록 함.
		Release();

		ID3D12Device* const device{ D3D12_Renderer::Get_Device()};
		assert(device);

		// descriptor heap 생성.
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Flags = is_shader_visible  // shader visible 여부.
			? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
			: D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		desc.NumDescriptors = capacity; // heap에 할당 가능한 최대 descriptor들의 개수.
		desc.Type = _type;              // descriptor heap 타입.
		desc.NodeMask = 0;              // 단일 어댑터(GPU 하나)의 경우 0으로 설정. GPU 여러 개 있는 경우 힙을 만들어줄 GPU를 선택하기 위한 옵션 값.

		HRESULT hr{ S_OK };
		DXCHECK(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_heap)));
		if (FAILED(hr)) return false;

		// descriptor 할당을 위한 변수들을 초기화.
		_free_handles = std::move(std::make_unique<uint32[]>(capacity)); // heap에 descriptor를 할당 할 수 있는 slot의 인덱스.
		_capacity = capacity;
		_size = 0;

		// 사용할 수 있는 slot 인덱스를 넣어줌.
		for (uint32 i{ 0 }; i < capacity; ++i)
			_free_handles[i] = i;
		// descriptor를 해제 하기 위해 예약해 두는 인덱스 배열 또한 초기에는 빈 상태여야 함. 애초에 할당한 descriptor가 없기 때문.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
			assert(_deferred_free_indices[i].empty());

		// heap 타입에 대해 할당 가능한 descriptor의 크기를 얻어옴.
		// descriptor 할당 시 리턴 해줄 descriptor handle의 offset 주소 값을 계산할 때 사용함.
		_descriptor_size = device->GetDescriptorHandleIncrementSize(_type);

		// descriptor heap 시작 주소에 대한 포인터를 얻어옴.
		// heap에 descriptor를 할당 할 때, 이 시작 주소에 offset 값을 더한 위치의 주소에 descriptor들이 할당 되게 됨.
		_cpu_start = _heap->GetCPUDescriptorHandleForHeapStart();
		_gpu_start = is_shader_visible ? _heap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };

		return true;
	}

	// 할당한 descriptor들을 해제 하는 클래스. 사실상 삭제하려는 descriptor의 offset 주소를 다른 descriptor로 덮어쓸 수 있도록 사용가능한 슬롯으로 설정해줌.
	void Descriptor_Heap::Process_Deferred_Free(uint32 frame_idx)
	{
		std::lock_guard lock{ _mutex };
		assert(frame_idx < frame_buffer_count);

		// 인자로 주어진 프레임 인덱스에 대한 해제 자원들의 descriptor 인덱스가 담긴 벡터를 참조.
		std::vector<uint32>& indices{ _deferred_free_indices[frame_idx] };
		if (!indices.empty())
		{
			for (auto index : indices)
			{
				--_size;						// heap에 할당한 descriptor의 개수를 하나 줄여줌.
				_free_handles[_size] = index;   // 기존에 descriptor를 할당한 위치를 다른 descriptor로 덮어쓰기 위해 사용 가능한 주소 인덱스로 열어줌.
			}
			indices.clear();
		}
	}

	void Descriptor_Heap::Release()
	{
		assert(!_size);
		D3D12_Renderer::Get_Instance()->Deferred_Release(_heap);
		_heap = nullptr;
	}

	// heap에 descriptor를 할당 하기 위해 빈 slot 위치(heap 시작 주소에서 시작하는 offset 주소)가 담긴 Descriptor_Handle 구조체를 리턴해주는 함수.
	Descriptor_Handle Descriptor_Heap::Allocate_Descriptor()
	{
		std::lock_guard lock{ _mutex };
		assert(_heap);               // descriptor 할당에 앞서 descriptor heap을 생성 했는지 먼저 체크.
		assert(_size < _capacity);   // 현재 heap에 할당한 descriptor의 개수가 heap의 최대 할당 크기를 넘어서지 않는지 체크.

		// descriptor 할당을 위해 heap에서 사용 가능한 offset 주소를 계산함.
		const uint32 index{ _free_handles[_size] };       // 사용 가능한 slot의 인덱스.
		const uint32 offset{ index * _descriptor_size };  // slot 인덱스 값에 descriptor 크기를 곱해 heap의 시작 주소에서 시작하는 offset 주소를 계산함.
		++_size;

		// shader visible 여부에 따라 CPU, GPU 핸들의 주소 값을 채워줌.
		Descriptor_Handle handle;
		handle.cpu.ptr = _cpu_start.ptr + offset;
		if (Is_shader_visible())
		{
			handle.gpu.ptr = _gpu_start.ptr + offset;
		}

		// 추후 디버깅을 위해 디버깅 전용 변수 값을 채워줌.
		DEBUG_OP(handle.container = this);
		DEBUG_OP(handle.index = index);
		return handle;
	}

	// 할당한 descriptor를 해제하기 위한 함수. 이 함수를 호출 했다고 바로 해제 되는 것이 아니라, GPU 작업이 완료 되어 더 이상 descriptor를 참조하지 않는 경우 삭제가 진행 됨.
	void Descriptor_Heap::Free_Descriptor(Descriptor_Handle& handle)
	{
		if (!handle.Is_Valid()) return; // decriptor를 해제 하기 전에 앞서 handle에 들어 있는 주소가 유효한지 체크.

		std::lock_guard lock{ _mutex };

		assert(_heap && _size);					  // heap이 존재 하는지 체크 && heap에 한 개 이상의 descriptor를 할당 했는지 체크.
		assert(handle.container == this);		  // 해제하고자 하는 descriptor가 할당된 heap이 이 heap이 맞는지 확인.
		assert(handle.cpu.ptr >= _cpu_start.ptr); // heap의 cpu쪽 할당 메모리 시작 주소가 해제 하고자 하는 descriptor의 주소 보다 작은지 확인.
		assert((handle.cpu.ptr - _cpu_start.ptr) % _descriptor_size == 0); // 해제 하고자 하는 descriptor의 크기가 이 heap에서 정의한 descriptor 크기가 맞는지 확인.
		assert(handle.index < _capacity);         // 해제 하고자 하는 descriptor의 인덱스가 heap의 최대 할당 크기 보다 작은지 확인.
		
		// 해제 하고자 하는 descriptor의 index를 계산.
		const uint32 index{ (uint32)(handle.cpu.ptr - _cpu_start.ptr) / _descriptor_size };
		assert(handle.index == index);            // 해제 하고자 하는 index가 정확한지 확인.

		// heap에서 삭제할 decriptor의 인덱스를 캐싱. 실제 삭제 작업은 descriptor에 대한 참조가 없을 때 이루어짐.
		const uint32 frame_index{ D3D12_Renderer::Get_Current_Frame_Index()};
		_deferred_free_indices[frame_index].push_back(index);          // 현재 프레임에서 제거해야 할 descriptor가 위치한 slot의 인덱스를 캐싱.
		D3D12_Renderer::Get_Instance()->Set_Deferred_Release_Flag();   // 현재 프레임에 제거해야 할 자원이 있음을 체크 표시 해줌.
	}
#pragma endregion 
}