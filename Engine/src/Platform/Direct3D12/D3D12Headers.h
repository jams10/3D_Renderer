#pragma once

#include "Common/CommonHeaders.h"
#include "D3D12Macros.h"

#include <dxgi1_6.h>  // 직접적으로 Direct3D12와 상관 없는 작업들을 위한 라이브러리. ex) 스왑 체인 생성, adapter 찾기 등.
#include <d3d12.h>    // 실질적인 Direct3D12 API 함수들이 담겨 있음.
#include <wrl.h>      // ComPtr 사용을 위함. d3d12와는 별개의 라이브러리.

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

namespace Engine::Graphics::D3D12
{
	constexpr uint32 frame_buffer_count{ 3 };
	constexpr DXGI_FORMAT render_target_format{ DXGI_FORMAT_R8G8B8A8_UNORM_SRGB }; // 렌더 타겟 최종 포맷.
	constexpr D3D_FEATURE_LEVEL minimum_feature_level{ D3D_FEATURE_LEVEL_11_0 }; // 렌더러가 지원할 가장 낮은 feature level.
}

// COM 인터페이스 객체를 Release 하는 함수.
template<typename T>
constexpr void Release(T*& resource)
{
	if (resource)
	{
		resource->Release();
		resource = nullptr;
	}
}