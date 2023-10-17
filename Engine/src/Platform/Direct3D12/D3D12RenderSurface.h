#pragma once

#include "Common/CommonHeaders.h"
#include "D3D12Headers.h"
#include "D3D12Resources.h"
#include "Window/Window.h"

namespace Engine::Graphics::D3D12
{
	// 씬이 렌더링 되는 back buffer를 갖는 스왑 체인과 뷰 포트, scissor rect등을 래핑한 클래스.
	class D3D12_RenderSurface
	{
	public:
		explicit D3D12_RenderSurface(Window* wnd)
			:_window(wnd)
		{
			assert(_window != nullptr);
		}
		~D3D12_RenderSurface()
		{
			_swap_chain = nullptr;
		}

		// 보통 8비트의 rgb 형식을 사용해 렌더링 하지만, hdr 모니터같이 hdr 형식을 표현할 수 있는 경우, 최종 렌더링을 어떤 형식으로 할지 인자로 받아줄 수 있도록 함.
		void Create_Swap_Chain(IDXGIFactory7* factory, ID3D12CommandQueue* cmd_queue, DXGI_FORMAT format);
		void Present() const;
		void Resize();
		void Release();

		constexpr uint32 Get_Width() const { return (uint32)_viewport.Width; }
		constexpr uint32 Get_Height() const { return (uint32)_viewport.Height; }
		constexpr ID3D12Resource* const Get_Current_Back_Buffer() const { return _render_target_data[_current_bb_index].resource; }
		constexpr D3D12_CPU_DESCRIPTOR_HANDLE Get_Current_RTV() const { return _render_target_data[_current_bb_index].rtv.cpu; }
		constexpr const D3D12_VIEWPORT& Get_Viewport() const { return _viewport; }
		constexpr const D3D12_RECT& Get_Scissor_Rect() const { return _scissor_rect; }

	private:
		void Finalize();

	private:

		struct Render_Target_Data
		{
			ID3D12Resource*		resource{ nullptr };
			Descriptor_Handle	rtv{};
		};

		Engine::Window*     _window;
		Render_Target_Data	_render_target_data[frame_buffer_count]{}; // 각 back buffer에 대한 rtv들.
		IDXGISwapChain4*	_swap_chain{ nullptr };
		mutable uint32		_current_bb_index{ 0 }; // 현재 back buffer 인덱스. const 함수인 present에서 값을 바꾸므로 mutable로 설정.
		D3D12_VIEWPORT		_viewport{};
		D3D12_RECT	        _scissor_rect{};
	};
}