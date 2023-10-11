#pragma once

#include "D3D12Headers.h"

namespace Engine::Graphics::D3D12
{
	/*
	* 한 프레임을 렌더링 하기 위해 필요한 명령을 저장하고 동기화 하기 위해 사용되는 구조체.
	* 내부적으로 명령을 저장하기 위한 명령 할당자와 CPU<->GPU간 동기화를 위해 GPU 작업이 끝날 때 까지 대기하는 동기화 함수를 가지고 있음.
	*/
	struct Command_Frame
	{
		ID3D12CommandAllocator* _cmd_allocator{ nullptr };  // 명령 리스트에 들어가는 명령들을 저장할 메모리를 관리하는 명령 할당자.
		uint64                  _fence_value{ 0 };          // 프레임에 해당하는 fence 값. 이 값을 통해 GPU 작업이 끝났는지 확인.

	// GPU가 작업을 완료할 때 까지 대기하는 작업을 이벤트를 이용해서 처리함.
	// GPU가 우리가 보낸 명령을 완료 했는지 확인하고 대기 하는 동기화 작업을 처리하는 함수.
		void Wait(HANDLE fence_event, ID3D12Fence1* fence)
		{
			assert(fence && fence_event);

			// 완료된 fence value가 특정 프레임의 fence value보다 작다면, 아직 특정 프레임에서 보낸 명령을 완료하지 못했다는 뜻.
			// 우리가 Signal() 함수를 통해 모든 명령 리스트들이 수행되고난 맨 마지막에 인자로 넘겨준 fence_value를 1 증가 시키도록 했기 때문에,
			// 모든 명령들이 완료되고 나면 fence_value가 1 증가하게 됨.
			if (fence->GetCompletedValue() < _fence_value)
			{
				// 작업이 완료될 때 fence_event를 통해 완료 여부를 통지 받을 수 있도록 함.
				DXCHECK(fence->SetEventOnCompletion(_fence_value, fence_event));
				// 작업이 완료되어 event가 깨어날 때 까지 대기.
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
	* D3D12의 커맨드 리스트, 커맨드 큐를 하나로 관리하기 위한 클래스.
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
		ID3D12CommandQueue*				_cmd_queue{ nullptr };           // 커맨드 리스트를 제출할 커맨드 큐.
		ID3D12GraphicsCommandList6*		_cmd_list{ nullptr };            // 명령들을 넣어줄 커맨드 리스트.
		ID3D12Fence1*					_fence{ nullptr };               // CPU<->GPU간 동기화에 사용할 fence.
		uint64							_fence_value{ 0 };               // CPU<->GPU간 동기화에 사용할 fence 카운트 값.
		Command_Frame					_cmd_frames[frame_buffer_count]; // 명령을 저장할 프레임 배열.
		HANDLE							_fence_event{ nullptr };		 // CPU<->GPU간 동기화에 사용할 이벤트 핸들.
		uint32							_frame_index{ 0 };				 // 현재 프레임의 인덱스.
	};
}