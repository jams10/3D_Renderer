
#include "D3D12Resources.h"
#include "D3D12Renderer.h"
#include "D3D12Macros.h"

namespace Engine::Graphics::D3D12
{
#pragma region Descriptor Heap
	// D3D12 API�� Ȱ���� Descriptor Heap�� �����ϴ� �ʱ�ȭ Ŭ����.
	bool Descriptor_Heap::Initialize(uint32 capacity, bool is_shader_visible)
	{
		// ���� thread�� ����ϴ� ���, ���ҽ��� �ε��ϰų� �����ϴ� �۾��� ���� thread���� �Ͼ�� ��.
		// ���ҽ��� �����ϰ�, �̸� ���������ο� ���ε� �ϱ� ���� descriptor�� ����� ������ ���� thread���� �Ͼ��, 
		// �̿� ���� ������ descriptor_heap Ŭ���� �ν��Ͻ��� ���� thread�� ���� ���ٵ� �� ����. �� �� �Ͼ�� race condition�� ���� ���� lock�� �ɾ���. 
		std::lock_guard lock{ _mutex };

		// heap type�� ���� heap�� �ִ� ���� ũ�⸦ üũ��.
		assert(capacity && capacity < D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2);
		assert(!(_type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER && capacity > D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE));

		// heap tyep�� ���� shader visible ���ΰ� ������. DSV, RTV descriptor�� ���� heap�� ���, GPU�� �ƴ� CPU �޸𸮿� �Ҵ��.
		if (_type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV ||
			_type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		{
			is_shader_visible = false;
		}

		// ������ heap�� ������ �־��ٸ�, ���� �ϰ� �ٽ� ����� �� �� �ֵ��� ��.
		Release();

		ID3D12Device* const device{ D3D12_Renderer::Get_Device()};
		assert(device);

		// descriptor heap ����.
		D3D12_DESCRIPTOR_HEAP_DESC desc{};
		desc.Flags = is_shader_visible  // shader visible ����.
			? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
			: D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		desc.NumDescriptors = capacity; // heap�� �Ҵ� ������ �ִ� descriptor���� ����.
		desc.Type = _type;              // descriptor heap Ÿ��.
		desc.NodeMask = 0;              // ���� �����(GPU �ϳ�)�� ��� 0���� ����. GPU ���� �� �ִ� ��� ���� ������� GPU�� �����ϱ� ���� �ɼ� ��.

		HRESULT hr{ S_OK };
		DXCHECK(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_heap)));
		if (FAILED(hr)) return false;

		// descriptor �Ҵ��� ���� �������� �ʱ�ȭ.
		_free_handles = std::move(std::make_unique<uint32[]>(capacity)); // heap�� descriptor�� �Ҵ� �� �� �ִ� slot�� �ε���.
		_capacity = capacity;
		_size = 0;

		// ����� �� �ִ� slot �ε����� �־���.
		for (uint32 i{ 0 }; i < capacity; ++i)
			_free_handles[i] = i;
		// descriptor�� ���� �ϱ� ���� ������ �δ� �ε��� �迭 ���� �ʱ⿡�� �� ���¿��� ��. ���ʿ� �Ҵ��� descriptor�� ���� ����.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
			assert(_deferred_free_indices[i].empty());

		// heap Ÿ�Կ� ���� �Ҵ� ������ descriptor�� ũ�⸦ ����.
		// descriptor �Ҵ� �� ���� ���� descriptor handle�� offset �ּ� ���� ����� �� �����.
		_descriptor_size = device->GetDescriptorHandleIncrementSize(_type);

		// descriptor heap ���� �ּҿ� ���� �����͸� ����.
		// heap�� descriptor�� �Ҵ� �� ��, �� ���� �ּҿ� offset ���� ���� ��ġ�� �ּҿ� descriptor���� �Ҵ� �ǰ� ��.
		_cpu_start = _heap->GetCPUDescriptorHandleForHeapStart();
		_gpu_start = is_shader_visible ? _heap->GetGPUDescriptorHandleForHeapStart() : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };

		return true;
	}

	// �Ҵ��� descriptor���� ���� �ϴ� Ŭ����. ��ǻ� �����Ϸ��� descriptor�� offset �ּҸ� �ٸ� descriptor�� ��� �� �ֵ��� ��밡���� �������� ��������.
	void Descriptor_Heap::Process_Deferred_Free(uint32 frame_idx)
	{
		std::lock_guard lock{ _mutex };
		assert(frame_idx < frame_buffer_count);

		// ���ڷ� �־��� ������ �ε����� ���� ���� �ڿ����� descriptor �ε����� ��� ���͸� ����.
		std::vector<uint32>& indices{ _deferred_free_indices[frame_idx] };
		if (!indices.empty())
		{
			for (auto index : indices)
			{
				--_size;						// heap�� �Ҵ��� descriptor�� ������ �ϳ� �ٿ���.
				_free_handles[_size] = index;   // ������ descriptor�� �Ҵ��� ��ġ�� �ٸ� descriptor�� ����� ���� ��� ������ �ּ� �ε����� ������.
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

	// heap�� descriptor�� �Ҵ� �ϱ� ���� �� slot ��ġ(heap ���� �ּҿ��� �����ϴ� offset �ּ�)�� ��� Descriptor_Handle ����ü�� �������ִ� �Լ�.
	Descriptor_Handle Descriptor_Heap::Allocate_Descriptor()
	{
		std::lock_guard lock{ _mutex };
		assert(_heap);               // descriptor �Ҵ翡 �ռ� descriptor heap�� ���� �ߴ��� ���� üũ.
		assert(_size < _capacity);   // ���� heap�� �Ҵ��� descriptor�� ������ heap�� �ִ� �Ҵ� ũ�⸦ �Ѿ�� �ʴ��� üũ.

		// descriptor �Ҵ��� ���� heap���� ��� ������ offset �ּҸ� �����.
		const uint32 index{ _free_handles[_size] };       // ��� ������ slot�� �ε���.
		const uint32 offset{ index * _descriptor_size };  // slot �ε��� ���� descriptor ũ�⸦ ���� heap�� ���� �ּҿ��� �����ϴ� offset �ּҸ� �����.
		++_size;

		// shader visible ���ο� ���� CPU, GPU �ڵ��� �ּ� ���� ä����.
		Descriptor_Handle handle;
		handle.cpu.ptr = _cpu_start.ptr + offset;
		if (Is_shader_visible())
		{
			handle.gpu.ptr = _gpu_start.ptr + offset;
		}

		// ���� ������� ���� ����� ���� ���� ���� ä����.
		DEBUG_OP(handle.container = this);
		DEBUG_OP(handle.index = index);
		return handle;
	}

	// �Ҵ��� descriptor�� �����ϱ� ���� �Լ�. �� �Լ��� ȣ�� �ߴٰ� �ٷ� ���� �Ǵ� ���� �ƴ϶�, GPU �۾��� �Ϸ� �Ǿ� �� �̻� descriptor�� �������� �ʴ� ��� ������ ���� ��.
	void Descriptor_Heap::Free_Descriptor(Descriptor_Handle& handle)
	{
		if (!handle.Is_Valid()) return; // decriptor�� ���� �ϱ� ���� �ռ� handle�� ��� �ִ� �ּҰ� ��ȿ���� üũ.

		std::lock_guard lock{ _mutex };

		assert(_heap && _size);					  // heap�� ���� �ϴ��� üũ && heap�� �� �� �̻��� descriptor�� �Ҵ� �ߴ��� üũ.
		assert(handle.container == this);		  // �����ϰ��� �ϴ� descriptor�� �Ҵ�� heap�� �� heap�� �´��� Ȯ��.
		assert(handle.cpu.ptr >= _cpu_start.ptr); // heap�� cpu�� �Ҵ� �޸� ���� �ּҰ� ���� �ϰ��� �ϴ� descriptor�� �ּ� ���� ������ Ȯ��.
		assert((handle.cpu.ptr - _cpu_start.ptr) % _descriptor_size == 0); // ���� �ϰ��� �ϴ� descriptor�� ũ�Ⱑ �� heap���� ������ descriptor ũ�Ⱑ �´��� Ȯ��.
		assert(handle.index < _capacity);         // ���� �ϰ��� �ϴ� descriptor�� �ε����� heap�� �ִ� �Ҵ� ũ�� ���� ������ Ȯ��.
		
		// ���� �ϰ��� �ϴ� descriptor�� index�� ���.
		const uint32 index{ (uint32)(handle.cpu.ptr - _cpu_start.ptr) / _descriptor_size };
		assert(handle.index == index);            // ���� �ϰ��� �ϴ� index�� ��Ȯ���� Ȯ��.

		// heap���� ������ decriptor�� �ε����� ĳ��. ���� ���� �۾��� descriptor�� ���� ������ ���� �� �̷����.
		const uint32 frame_index{ D3D12_Renderer::Get_Current_Frame_Index()};
		_deferred_free_indices[frame_index].push_back(index);          // ���� �����ӿ��� �����ؾ� �� descriptor�� ��ġ�� slot�� �ε����� ĳ��.
		D3D12_Renderer::Get_Instance()->Set_Deferred_Release_Flag();   // ���� �����ӿ� �����ؾ� �� �ڿ��� ������ üũ ǥ�� ����.
	}
#pragma endregion 
}