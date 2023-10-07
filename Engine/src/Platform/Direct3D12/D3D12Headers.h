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
}