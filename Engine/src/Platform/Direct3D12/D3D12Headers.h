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
	constexpr DXGI_FORMAT render_target_format{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB }; // ���� Ÿ�� ���� ����.
	constexpr D3D_FEATURE_LEVEL minimum_feature_level{ D3D_FEATURE_LEVEL_11_0 }; // �������� ������ ���� ���� feature level.
}

// COM �������̽� ��ü�� Release �ϴ� �Լ�.
template<typename T>
constexpr void Release(T*& resource)
{
	if (resource)
	{
		resource->Release();
		resource = nullptr;
	}
}