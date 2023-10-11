#pragma once

#include "D3D12Headers.h"

namespace Engine::Graphics::D3D12
{
	/*
	* �� �������� ������ �ϱ� ���� �ʿ��� ����� �����ϰ� ����ȭ �ϱ� ���� ���Ǵ� ����ü.
	* ���������� ����� �����ϱ� ���� ��� �Ҵ��ڿ� CPU<->GPU�� ����ȭ�� ���� GPU �۾��� ���� �� ���� ����ϴ� ����ȭ �Լ��� ������ ����.
	*/
	struct Command_Frame
	{
		ID3D12CommandAllocator* _cmd_allocator{ nullptr };  // ��� ����Ʈ�� ���� ��ɵ��� ������ �޸𸮸� �����ϴ� ��� �Ҵ���.
		uint64                  _fence_value{ 0 };          // �����ӿ� �ش��ϴ� fence ��. �� ���� ���� GPU �۾��� �������� Ȯ��.

	// GPU�� �۾��� �Ϸ��� �� ���� ����ϴ� �۾��� �̺�Ʈ�� �̿��ؼ� ó����.
	// GPU�� �츮�� ���� ����� �Ϸ� �ߴ��� Ȯ���ϰ� ��� �ϴ� ����ȭ �۾��� ó���ϴ� �Լ�.
		void Wait(HANDLE fence_event, ID3D12Fence1* fence)
		{
			assert(fence && fence_event);

			// �Ϸ�� fence value�� Ư�� �������� fence value���� �۴ٸ�, ���� Ư�� �����ӿ��� ���� ����� �Ϸ����� ���ߴٴ� ��.
			// �츮�� Signal() �Լ��� ���� ��� ��� ����Ʈ���� ����ǰ� �� �������� ���ڷ� �Ѱ��� fence_value�� 1 ���� ��Ű���� �߱� ������,
			// ��� ��ɵ��� �Ϸ�ǰ� ���� fence_value�� 1 �����ϰ� ��.
			if (fence->GetCompletedValue() < _fence_value)
			{
				// �۾��� �Ϸ�� �� fence_event�� ���� �Ϸ� ���θ� ���� ���� �� �ֵ��� ��.
				DXCHECK(fence->SetEventOnCompletion(_fence_value, fence_event));
				// �۾��� �Ϸ�Ǿ� event�� ��� �� ���� ���.
				WaitForSingleObject(fence_event, INFINITE);
			}
		}

		void Release()
		{
			::Release(_cmd_allocator);
			_fence_value = 0;
		}
	};

	/*
	* D3D12�� Ŀ�ǵ� ����Ʈ, Ŀ�ǵ� ť�� �ϳ��� �����ϱ� ���� Ŭ����.
	*/
	class D3D12_Command
	{
	public:
		D3D12_Command() = default;
		~D3D12_Command();
		DISABLE_COPY_AND_MOVE(D3D12_Command)

		explicit D3D12_Command(ID3D12Device8* const device, D3D12_COMMAND_LIST_TYPE type);
		void Begin_Frame();
		void End_Frame();
		void Release();

		constexpr ID3D12CommandQueue* const Command_Queue() const { return _cmd_queue; }
		constexpr ID3D12GraphicsCommandList6* const Command_List() const { return _cmd_list; }
		constexpr uint32 Frame_Index() const { return _frame_index; }

	private:
		void Flush();

	private:
		ID3D12CommandQueue*				_cmd_queue{ nullptr };           // Ŀ�ǵ� ����Ʈ�� ������ Ŀ�ǵ� ť.
		ID3D12GraphicsCommandList6*		_cmd_list{ nullptr };            // ��ɵ��� �־��� Ŀ�ǵ� ����Ʈ.
		ID3D12Fence1*					_fence{ nullptr };               // CPU<->GPU�� ����ȭ�� ����� fence.
		uint64							_fence_value{ 0 };               // CPU<->GPU�� ����ȭ�� ����� fence ī��Ʈ ��.
		Command_Frame					_cmd_frames[frame_buffer_count]; // ����� ������ ������ �迭.
		HANDLE							_fence_event{ nullptr };		 // CPU<->GPU�� ����ȭ�� ����� �̺�Ʈ �ڵ�.
		uint32							_frame_index{ 0 };				 // ���� �������� �ε���.
	};
}