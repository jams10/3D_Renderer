#pragma once

#include "Common/CommonHeaders.h"
#include "D3D12Headers.h"

namespace Engine::Graphics::D3D12
{

	class Descriptor_Heap; // Ÿ���� ����ϱ� ���� ���漱��.

	/*
	* Descriptor Heap�� �Ҵ��� Descriptor�� ���� �ڵ��� ��Ÿ���� ����ü.
	* ���ο� CPU, GPU Descriptor�� ���� �ڵ��� ��� ����.
	*/
	struct Descriptor_Handle
	{
		// descriptor heap�� �Ҵ��� descriptor�� ���� �ڵ�. heap �󿡼��� descriptor�� ��ġ �ּ�(offset �ּ�).
		D3D12_CPU_DESCRIPTOR_HANDLE	cpu{}; // CPU �޸𸮿� �Ҵ�� descriptor�� ���� �ڵ�, �ּ�.
		D3D12_GPU_DESCRIPTOR_HANDLE	gpu{}; // GPU �޸𸮿� �Ҵ�� descriptor�� ���� �ڵ�, �ּ�.

		// HANDLE ����ü ������ ptr ���� ���� unsigned long long Ÿ���� �������� Ÿ���� ������. ���� ��ȿ�� üũ�� nullptr�� �ƴ� 0�� ���� �ּ� ���� 0���� üũ.
		constexpr bool Is_Valid() const { return cpu.ptr != 0; }
		constexpr bool Is_Shader_Visible() const { return gpu.ptr != 0; } // shader-visible descriptor�� ��� GPU �ʿ� �Ҵ� �ϱ� ������ gpu �ڵ��� üũ.

		// ����뿡 ���� ������.
#ifdef _DEBUG
	private:
		friend class Descriptor_Heap;              // Descriptor_Heap Ŭ���� �ʿ��� private ������ ���� �����ϵ��� ��.
		Descriptor_Heap* container{ nullptr };     // �� descriptor�� �Ҵ�� heap�� ���� ������.
		int32			 index{ -1 };              // heap���� �� descriptor�� �ε���. descriptor �ڵ��� public���� ���� �־��� ������ �߸��� ���� ����� ��츦 üũ�ϱ� ����.
#endif
	};

	/*
	* Descriptor Heap�� �Ҵ��ϰ� �����ϴ� Ŭ����.
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

		// descriptor�� �Ҵ��ϰ� �����ϴ� �Լ���.
		[[nodiscard]] Descriptor_Handle Allocate_Descriptor(); // descriptor �Ҵ� �ϰ� �ڵ��� ������. [[nodiscard]]�� ��� ���� ���� ������ ��� ��� �߻� ��Ŵ. �Լ� ȣ�� ���� ���� ȣ�� �ο��� �޾��־�� ��.
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
		ID3D12DescriptorHeap*				_heap;                      // ������ API�� ���� ������ heap COM ��ü�� ���� ������.
		const D3D12_DESCRIPTOR_HEAP_TYPE	_type{};					// descriptor heap�� ����. descriptor �Ҵ� �� ������ �´� heap�� �Ҵ� �� �־�� ��.
		D3D12_CPU_DESCRIPTOR_HANDLE			_cpu_start{};				// CPU�� ���� ���� ������ base address.
		D3D12_GPU_DESCRIPTOR_HANDLE			_gpu_start{};				// GPU�� ���� ���� ������ base address.
		std::unique_ptr<uint32[]>           _free_handles{};			// heap�� �ִ� slot�� ���� �ε���. slot�� heap�� descriptor�� �Ҵ��� �� �ִ� ������ �ǹ�.
		std::vector<uint32>                 _deferred_free_indices[frame_buffer_count]{}; // ���� �ؾ� �ϴ� �ڿ��� descriptor�� ���� �ε���.
		std::mutex						    _mutex{};					// �ڿ� �Ҵ�, ���� �� race condition�� �����ϱ� ���� mutex.
		uint32								_capacity{ 0 };				// heap�� ��ü ũ��. �ִ� �Ҵ� ������ decriptor�� ����.
		uint32								_size{ 0 };					// �Ҵ�� descriptor ����.
		uint32								_descriptor_size{};			// �� heap�� �Ҵ��� �� �ִ� descriptor�� ũ��.
	};
}