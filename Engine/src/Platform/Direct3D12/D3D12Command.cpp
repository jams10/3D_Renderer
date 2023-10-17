
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

		// 1. Ŀ�ǵ� ť ����.
		DXCHECK(hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_cmd_queue)));
		if (FAILED(hr)) goto _error;

		// device ������ �� �־��� �Ͱ� ���������� COM ��ü�� �̸��� �����Ͽ� ����� �� �� ���� �ĺ��� �� �ֵ��� ��.
		NAME_D3D12_OBJECT(_cmd_queue,
			type == D3D12_COMMAND_LIST_TYPE_DIRECT ?
			L"Graphics Command Queue" :
			type == D3D12_COMMAND_LIST_TYPE_COMPUTE ?
			L"Compute Command Queue" : L"Command Queue");

		// 2. Ŀ�ǵ� �Ҵ��� ����.
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

		// 3. Ŀ�ǵ� ����Ʈ ����.
		DXCHECK(hr = device->CreateCommandList(0, type, _cmd_frames[0]._cmd_allocator, nullptr, IID_PPV_ARGS(&_cmd_list)));
		if (FAILED(hr)) goto _error;
		// Ŀ�ǵ� ����Ʈ ���� �� Ŀ�ǵ� ����Ʈ�� ����� ��� ���� Reset()�� ȣ���ϱ� ���ؼ��� ���� Ŀ�ǵ� ����Ʈ�� Close()�� ���� ���� �־�� ��.
		DXCHECK(hr = _cmd_list->Close());
		if (FAILED(hr)) goto _error;

		NAME_D3D12_OBJECT(_cmd_list,
			type == D3D12_COMMAND_LIST_TYPE_DIRECT ?
			L"Graphics Command List" :
			type == D3D12_COMMAND_LIST_TYPE_COMPUTE ?
			L"Compute Command List" : L"Command List");

		// 4. Fence�� Fence Event ����.
		DXCHECK(hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
		if (FAILED(hr)) goto _error;
		NAME_D3D12_OBJECT(_fence, L"D3D12 Fence");

		_fence_event = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS); // windows event�� ��������.
		assert(_fence_event);

		return;

	_error:
		Release();
	}

	// ������ ���� �� ó���� �۾��� �����ϴ� �Լ�.
	// ���� �����ӿ� ���� ��ɵ��� ����� �� ���� ����ϰ�, �ñ׳� �Ǹ� �� ����� ����ϱ� ���� Ŀ�ǵ� ����Ʈ �ʱ�ȭ.
	void D3D12_Command::Begin_Frame()
	{
		Command_Frame& frame{ _cmd_frames[_frame_index] };
		// Ŀ�ǵ� ����Ʈ�� �� ����� ����ϱ� ��, GPU�� �۾��� �Ϸ��� �� ���� ���.
		frame.Wait(_fence_event, _fence);

		// Ŀ�ǵ� �Ҵ��ڸ� reset�ϴ� ���� ������ �츮�� Ŀ�ǵ� ����Ʈ�� ����� ����ϱ� ���� ����ߴ� �޸𸮸� ������ �ִ� ����.
		// Ŀ�ǵ� ����Ʈ�� reset�ϴ� ���� Ŀ�ǵ� ����Ʈ�� �� ����� ����ϱ� ���� Ŀ�ǵ� ����Ʈ�� �ٽ� �����ִ� ����.
		DXCHECK(frame._cmd_allocator->Reset());
		DXCHECK(_cmd_list->Reset(frame._cmd_allocator, nullptr));
	}

	// �������� ���� �� ó���� �۾��� �����ϴ� �Լ�.
	// Ŀ�ǵ� ����Ʈ�� ť�� �����ϰ�, Ŀ�ǵ� ����Ʈ�� ���� ��ɵ��� �Ϸ� ���θ� ���� �ޱ� ���� Signal() �Լ� ȣ��.
	void D3D12_Command::End_Frame()
	{
		// ��� ����� �������� Ŀ�ǵ� ����Ʈ�� �ݾ���� ��.
		DXCHECK(_cmd_list->Close());

		// Ŀ�ǵ� ����Ʈ ����� �����, Ŀ�ǵ� ť�� �����ؼ� GPU���� ��ɵ��� ����� �� �ֵ��� ��.
		// ������ �̱� �����忡�� ��ɵ��� ����ϰ� �ֱ� ������ Ŀ�ǵ� ����Ʈ�� �ϳ���.
		ID3D12CommandList* const cmd_lists[]{ _cmd_list };
		_cmd_queue->ExecuteCommandLists(_countof(cmd_lists), &cmd_lists[0]);

		// ����ȭ�� ���� fence ī��Ʈ �� ���� �� Signal() �Լ� ȣ��.
		uint64& fence_value{ _fence_value }; // �����Ӹ��� ��� �����ϴ� fence value.
		++fence_value;
		Command_Frame& frame{ _cmd_frames[_frame_index] };
		frame._fence_value = fence_value;
		_cmd_queue->Signal(_fence, _fence_value);

		// �� �����ӿ� ���� ��� ��� �۾��� ������ ���� �����ӿ� ���� ��� ��� �۾��� ������ �� �ֵ��� ���� ������ ���� �ε����� ���� ��Ŵ.
		_frame_index = (_frame_index + 1) % frame_buffer_count;
	}

	// ��� ������ ���۸� ����ִ� �Լ�.
	void D3D12_Command::Flush()
	{
		// ��� ������ ������ Ŀ�ǵ� ����Ʈ �۾��� ���� �� ���� ���.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			_cmd_frames[i].Wait(_fence_event, _fence);
		}
		_frame_index = 0;
	}

	void D3D12_Command::Release()
	{
		// 1. ��� Ŀ�ǵ� ����Ʈ�� �۾��� �������� üũ. ���� ������ ���. & fence �������̽� ��ü ����.
		Flush();
		::Release(_fence);
		_fence_value = 0;

		// 2. ����ȭ �۾��� ���� ����� event ����.
		CloseHandle(_fence_event);
		_fence_event = nullptr;

		// 3. Ŀ�ǵ� ť�� ����Ʈ �������̽� ��ü ����.
		::Release(_cmd_queue);
		::Release(_cmd_list);

		// 4. Ŀ�ǵ� �Ҵ��� ����.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			_cmd_frames[i].Release();
		}
	}
}