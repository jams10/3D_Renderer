#pragma once

#include "Common/CommonHeaders.h"
#include "D3D12Headers.h"

namespace Engine::Graphics::D3D12
{

	class Descriptor_Heap; // 타입을 사용하기 위해 전방선언.

	/*
	* Descriptor Heap에 할당한 Descriptor에 대한 핸들을 나타내는 구조체.
	* 내부에 CPU, GPU Descriptor에 대한 핸들을 들고 있음.
	*/
	struct Descriptor_Handle
	{
		// descriptor heap에 할당한 descriptor에 대한 핸들. heap 상에서의 descriptor의 위치 주소(offset 주소).
		D3D12_CPU_DESCRIPTOR_HANDLE	cpu{}; // CPU 메모리에 할당된 descriptor에 대한 핸들, 주소.
		D3D12_GPU_DESCRIPTOR_HANDLE	gpu{}; // GPU 메모리에 할당된 descriptor에 대한 핸들, 주소.

		// HANDLE 구조체 내부의 ptr 변수 값은 unsigned long long 타입을 재정의한 타입의 변수임. 따라서 유효성 체크를 nullptr가 아닌 0과 비교해 주소 값이 0인지 체크.
		constexpr bool Is_Valid() const { return cpu.ptr != 0; }
		constexpr bool Is_Shader_Visible() const { return gpu.ptr != 0; } // shader-visible descriptor의 경우 GPU 쪽에 할당 하기 때문에 gpu 핸들을 체크.

		// 디버깅에 사용될 변수들.
#ifdef _DEBUG
	private:
		friend class Descriptor_Heap;              // Descriptor_Heap 클래스 쪽에서 private 변수에 접근 가능하도록 함.
		Descriptor_Heap* container{ nullptr };     // 이 descriptor가 할당된 heap에 대한 포인터.
		int32			 index{ -1 };              // heap에서 이 descriptor의 인덱스. descriptor 핸들을 public으로 열어 주었기 때문에 잘못된 값을 덮어쓰는 경우를 체크하기 위함.
#endif
	};

	/*
	* Descriptor Heap을 할당하고 관리하는 클래스.
	*/
	class Descriptor_Heap
	{
	public:
		explicit Descriptor_Heap(D3D12_DESCRIPTOR_HEAP_TYPE type) : _type{ type } {};
		DISABLE_COPY_AND_MOVE(Descriptor_Heap)
		~Descriptor_Heap() { assert(!_heap); }

		bool Initialize(uint32 capacity, bool is_shader_visible);
		void Process_Deferred_Free(uint32 frame_idx);
		void Release();

		// descriptor를 할당하고 해제하는 함수들.
		[[nodiscard]] Descriptor_Handle Allocate_Descriptor(); // descriptor 할당 하고 핸들을 리턴함. [[nodiscard]]의 경우 리턴 값이 버려질 경우 경고를 발생 시킴. 함수 호출 리턴 값을 호출 부에서 받아주어야 함.
		void Free_Descriptor(Descriptor_Handle& handle);

		// getters
		constexpr D3D12_DESCRIPTOR_HEAP_TYPE Type() const { return _type; }
		constexpr D3D12_CPU_DESCRIPTOR_HANDLE Cpu_Start() const { return _cpu_start; }
		constexpr D3D12_GPU_DESCRIPTOR_HANDLE Gpu_Start() const { return _gpu_start; }
		constexpr ID3D12DescriptorHeap* const Heap() const { return _heap; }
		constexpr uint32 Capacity() const { return _capacity; }
		constexpr uint32 Size() const { return _size; }
		constexpr uint32 Descriptor_Size() const { return _descriptor_size; }
		constexpr bool Is_shader_visible() const { return _gpu_start.ptr != 0; }

	private:
		ID3D12DescriptorHeap*				_heap;                      // 실제로 API를 통해 생성한 heap COM 객체에 대한 포인터.
		const D3D12_DESCRIPTOR_HEAP_TYPE	_type{};					// descriptor heap의 유형. descriptor 할당 시 유형에 맞는 heap에 할당 해 주어야 함.
		D3D12_CPU_DESCRIPTOR_HANDLE			_cpu_start{};				// CPU에 의해 접근 가능한 base address.
		D3D12_GPU_DESCRIPTOR_HANDLE			_gpu_start{};				// GPU에 의해 접근 가능한 base address.
		std::unique_ptr<uint32[]>           _free_handles{};			// heap에 있는 slot에 대한 인덱스. slot은 heap에 descriptor를 할당할 수 있는 공간을 의미.
		std::vector<uint32>                 _deferred_free_indices[frame_buffer_count]{}; // 해제 해야 하는 자원의 descriptor에 대한 인덱스.
		std::mutex						    _mutex{};					// 자원 할당, 해제 시 race condition을 방지하기 위한 mutex.
		uint32								_capacity{ 0 };				// heap의 전체 크기. 최대 할당 가능한 decriptor의 개수.
		uint32								_size{ 0 };					// 할당된 descriptor 갯수.
		uint32								_descriptor_size{};			// 이 heap에 할당할 수 있는 descriptor의 크기.
	};
}