
#include "D3D12Command.h"

namespace Engine::Graphics::D3D12
{
	D3D12_Command::~D3D12_Command()
	{
	}

	D3D12_Command::D3D12_Command(ID3D12Device8* const device, D3D12_COMMAND_LIST_TYPE type)
	{
		HRESULT hr{ S_OK };
		D3D12_COMMAND_QUEUE_DESC desc{};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Type = type;

		// 1. 커맨드 큐 생성.
		DXCHECK(hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_cmd_queue)));
		if (FAILED(hr)) goto _error;

		// device 생성시 해 주었던 것과 마찬가지로 COM 객체의 이름을 변경하여 디버깅 할 때 쉽게 식별할 수 있도록 함.
		NAME_D3D12_OBJECT(_cmd_queue,
			type == D3D12_COMMAND_LIST_TYPE_DIRECT ?
			L"Graphics Command Queue" :
			type == D3D12_COMMAND_LIST_TYPE_COMPUTE ?
			L"Compute Command Queue" : L"Command Queue");

		// 2. 커맨드 할당자 생성.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			Command_Frame& frame{ _cmd_frames[i] };
			DXCHECK(hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(&frame._cmd_allocator)));
			if (FAILED(hr)) goto _error;

			NAME_D3D12_OBJECT_INDEXED(frame._cmd_allocator, i,
				type == D3D12_COMMAND_LIST_TYPE_DIRECT ?
				L"Graphics Command Allocator" :
				type == D3D12_COMMAND_LIST_TYPE_COMPUTE ?
				L"Compute Command Allocator" : L"Command Allocator");
		}

		// 3. 커맨드 리스트 생성.
		DXCHECK(hr = device->CreateCommandList(0, type, _cmd_frames[0]._cmd_allocator, nullptr, IID_PPV_ARGS(&_cmd_list)));
		if (FAILED(hr)) goto _error;
		// 커맨드 리스트 생성 후 커맨드 리스트에 명령을 담기 위해 Reset()을 호출하기 위해서는 먼저 커맨드 리스트가 Close()를 통해 닫혀 있어야 함.
		DXCHECK(hr = _cmd_list->Close());
		if (FAILED(hr)) goto _error;

		NAME_D3D12_OBJECT(_cmd_list,
			type == D3D12_COMMAND_LIST_TYPE_DIRECT ?
			L"Graphics Command List" :
			type == D3D12_COMMAND_LIST_TYPE_COMPUTE ?
			L"Compute Command List" : L"Command List");

		// 4. Fence와 Fence Event 생성.
		DXCHECK(hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
		if (FAILED(hr)) goto _error;
		NAME_D3D12_OBJECT(_fence, L"D3D12 Fence");

		_fence_event = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS); // windows event를 생성해줌.
		assert(_fence_event);

		return;

	_error:
		Release();
	}

	// 프레임 시작 시 처리할 작업을 수행하는 함수.
	// 이전 프레임에 던진 명령들이 수행될 때 까지 대기하고, 시그널 되면 새 명령을 기록하기 위해 커맨드 리스트 초기화.
	void D3D12_Command::Begin_Frame()
	{
		Command_Frame& frame{ _cmd_frames[_frame_index] };
		// 커맨드 리스트에 새 명령을 기록하기 전, GPU가 작업을 완료할 때 까지 대기.
		frame.Wait(_fence_event, _fence);

		// 커맨드 할당자를 reset하는 것은 이전에 우리가 커맨드 리스트에 명령을 기록하기 위해 사용했던 메모리를 해제해 주는 것임.
		// 커맨드 리스트를 reset하는 것은 커맨드 리스트에 새 명령을 기록하기 위해 커맨드 리스트를 다시 열여주는 것임.
		DXCHECK(frame._cmd_allocator->Reset());
		DXCHECK(_cmd_list->Reset(frame._cmd_allocator, nullptr));
	}

	// 프레임이 끝날 때 처리할 작업을 수행하는 함수.
	// 커맨드 리스트를 큐에 제출하고, 커맨드 리스트에 넣은 명령들의 완료 여부를 통지 받기 위해 Signal() 함수 호출.
	void D3D12_Command::End_Frame()
	{
		// 명령 기록이 끝났으면 커맨드 리스트를 닫아줘야 함.
		DXCHECK(_cmd_list->Close());

		// 커맨드 리스트 목록을 만들고, 커맨드 큐에 제출해서 GPU에서 명령들이 수행될 수 있도록 함.
		// 지금은 싱글 스레드에서 명령들을 기록하고 있기 때문에 커맨드 리스트가 하나임.
		ID3D12CommandList* const cmd_lists[]{ _cmd_list };
		_cmd_queue->ExecuteCommandLists(_countof(cmd_lists), &cmd_lists[0]);

		// 동기화를 위한 fence 카운트 값 설정 및 Signal() 함수 호출.
		uint64& fence_value{ _fence_value }; // 프레임마다 계속 증가하는 fence value.
		++fence_value;
		Command_Frame& frame{ _cmd_frames[_frame_index] };
		frame._fence_value = fence_value;
		_cmd_queue->Signal(_fence, _fence_value);

		// 한 프레임에 대한 명령 기록 작업이 끝나면 다음 프레임에 대한 명령 기록 작업을 수행할 수 있도록 현재 프레임 버퍼 인덱스를 증가 시킴.
		_frame_index = (_frame_index + 1) % frame_buffer_count;
	}

	// 모든 프레임 버퍼를 비워주는 함수.
	void D3D12_Command::Flush()
	{
		// 모든 프레임 버퍼의 커맨드 리스트 작업이 끝날 때 까지 대기.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			_cmd_frames[i].Wait(_fence_event, _fence);
		}
		_frame_index = 0;
	}

	void D3D12_Command::Release()
	{
		// 1. 모든 커맨드 리스트의 작업이 끝났는지 체크. 끝날 때까지 대기. & fence 인터페이스 객체 해제.
		Flush();
		::Release(_fence);
		_fence_value = 0;

		// 2. 동기화 작업을 위해 사용한 event 해제.
		CloseHandle(_fence_event);
		_fence_event = nullptr;

		// 3. 커맨드 큐와 리스트 인터페이스 객체 해제.
		::Release(_cmd_queue);
		::Release(_cmd_list);

		// 4. 커맨드 할당자 해제.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			_cmd_frames[i].Release();
		}
	}
}