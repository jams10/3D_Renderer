#pragma once

#include "Common/CommonHeaders.h"
#include "D3D12Headers.h"
#include "D3D12Resources.h"
#include "Window/Window.h"

namespace Engine::Graphics::D3D12
{
	// ���� ������ �Ǵ� back buffer�� ���� ���� ü�ΰ� �� ��Ʈ, scissor rect���� ������ Ŭ����.
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

		// ���� 8��Ʈ�� rgb ������ ����� ������ ������, hdr ����Ͱ��� hdr ������ ǥ���� �� �ִ� ���, ���� �������� � �������� ���� ���ڷ� �޾��� �� �ֵ��� ��.
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
		Render_Target_Data	_render_target_data[frame_buffer_count]{}; // �� back buffer�� ���� rtv��.
		IDXGISwapChain4*	_swap_chain{ nullptr };
		mutable uint32		_current_bb_index{ 0 }; // ���� back buffer �ε���. const �Լ��� present���� ���� �ٲٹǷ� mutable�� ����.
		D3D12_VIEWPORT		_viewport{};
		D3D12_RECT	        _scissor_rect{};
	};
}