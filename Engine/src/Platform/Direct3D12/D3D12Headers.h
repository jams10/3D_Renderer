#pragma once

#include "Common/CommonHeaders.h"
#include "D3D12Macros.h"

#include <dxgi1_6.h>  // ���������� Direct3D12�� ��� ���� �۾����� ���� ���̺귯��. ex) ���� ü�� ����, adapter ã�� ��.
#include <d3d12.h>    // �������� Direct3D12 API �Լ����� ��� ����.
#include <wrl.h>      // ComPtr ����� ����. d3d12�ʹ� ������ ���̺귯��.

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

namespace Engine::Graphics::D3D12
{
	constexpr uint32 frame_buffer_count{ 3 };
}