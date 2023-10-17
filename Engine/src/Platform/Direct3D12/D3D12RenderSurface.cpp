
#include "D3D12RenderSurface.h"
#include "D3D12Renderer.h"
#include "D3D12Macros.h"

namespace Engine::Graphics::D3D12
{
	namespace
	{
		// SRGB ������ non-SRGB �������� ��ȯ. 
		constexpr DXGI_FORMAT To_Non_Srgb(DXGI_FORMAT format)
		{
			if (format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB) return DXGI_FORMAT_R8G8B8A8_UNORM;
			else return format;
		}
	}

	// ���� ü���� �����ϴ� �Լ�.
	void D3D12_RenderSurface::Create_Swap_Chain(IDXGIFactory7* factory, ID3D12CommandQueue* cmd_queue, DXGI_FORMAT format)
	{
		assert(factory && cmd_queue);
		Release(); // ������ ���� ü���� ���� �ߴٸ� �̸� ������.

		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.BufferCount = frame_buffer_count; // ���� ü���� ���� backbuffer ����. �츮�� 3���� ������ ���ۿ� ���� ����� �����ϰ� �����Ƿ�, 3���� ������ �־���.(��� �� �� �̻��̱⸸ �ϸ� �������.) D3D12���� ����ü���� �ּ� 3�� �̻��� ���۰� �ʿ���.
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.Flags = 0;
		desc.Format = To_Non_Srgb(format);     // ���� ü���� back buffer�� srgb �������� ������� �� ����. ���� srgb ������ ���ڷ� ������, �̸� srgb�� �ƴ� �������� ��ȯ�� �־�� ��.
		desc.Height = _window->Get_Height();
		desc.Width = _window->Get_Width();
		// �츮�� �� surface�� ���� ���� Ÿ������ ����� ����. ��, ���������� �ϼ��� �̹����� �� backbuffer �ؽ��Ŀ� �������� ����. 
		// ���� anti-aliasing�̳� super sampling�� ����� �ʿ䰡 ����.
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		// ������ ������¡ �� �� �þ���� stretch�� ����.
		desc.Scaling = DXGI_SCALING_STRETCH;
		// d3d12�� flip_sequential�� flip_discard �� ���� ������. �츮�� back buffer�� �ٸ� back buffer�� �Է����� ������� �ʱ� ������ flip_discard�� �����. ��, ���ҵǸ� front ���� ������.
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Stereo = false; // 3D ���׷��� �������� �����ϴ��� ����.

		IDXGISwapChain1* swap_chain;
		HWND hwnd{ (HWND)_window->Get_Native_Window() };
		// ���� ü���� ������ �������� �ڵ��� ���ڷ� �޾���.
		DXCHECK(factory->CreateSwapChainForHwnd(cmd_queue, hwnd, &desc, nullptr, nullptr, &swap_chain));
		// ���� ü���� ����ڰ� alt + enter Ű�� ������ �� â�� ��ü ȭ������ ������ִ� ����� ������ ����. 
		// �츮�� �ִ�ȭ�� ��� ���� â���� ��ü ȭ���� ó���ϰ� �ֱ� ������ �� ����� ����.
		DXCHECK(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
		// CreateSwapChainForHwnd �Լ��� IDXGISwapChain1 Ÿ���� ���ڷ� ������, �츮�� �ֽ� ������ ���� ü���� ����ϱ� ���� ������ ���� ����.
		DXCHECK(swap_chain->QueryInterface(IID_PPV_ARGS(&_swap_chain)));

		::Release(swap_chain); // �ֽ� ���� ���� ü���� ����� ���̹Ƿ� ������ ������ IDXGISwapChain1 ��ü�� ����.

		_current_bb_index = _swap_chain->GetCurrentBackBufferIndex();

		// swap chain�� �ִ� back buffer�鿡 ���� RTV�� �����ϱ� ���� render target view descriptor heap�� slot�� �Ҵ���.
		// Allocate_Descriptor() �Լ������� descriptor�� �Ҵ�� heap �󿡼��� offset �ּҸ� �޾ƿ� �� ����.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			_render_target_data[i].rtv = D3D12_Renderer::Get_Rtv_Desc_Heap().Allocate_Descriptor();
		}

		Finalize();
	}

	// ���� �������� back buffer�� front buffer�� ��ü�ؼ� ȭ�鿡 �����ִ� �Լ�.
	void D3D12_RenderSurface::Present() const
	{
		assert(_swap_chain);
		DXCHECK(_swap_chain->Present(0, 0)); // present �Լ��� ���� back buffer �ε����� ���� ��Ŵ.
		_current_bb_index = _swap_chain->GetCurrentBackBufferIndex(); // ���� back buffer �ε����� ������ back buffer �ε����� ����.
	}

	void D3D12_RenderSurface::Resize()
	{

	}

	void D3D12_RenderSurface::Release()
	{
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			Render_Target_Data& data{ _render_target_data[i] };
			::Release(data.resource);    // resource COM �������̽� ��ü release.
			D3D12_Renderer::Get_Rtv_Desc_Heap().Free_Descriptor(data.rtv); // descriptor_handle�� ���ڷ� ����, heap���� �ش� descriptor�� release(). ���������δ� ���� �Ҵ� ������ ������� ������.
		}

		::Release(_swap_chain); // ���� ü�� ��ü release. ���� ü���� �����ϴ� ���ҽ����� ������ ���� release �� �ڿ� release �ؾ� ���� ü���� release ��.
	}

	// back buffer�� ���� RTV�� ����, ����Ʈ�� scissor rect�� window ũ��� ������.
	void D3D12_RenderSurface::Finalize()
	{
		// back buffer�鿡 ���� rtv ����.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			Render_Target_Data& data{ _render_target_data[i] };
			assert(!data.resource);
			DXCHECK(_swap_chain->GetBuffer(i, IID_PPV_ARGS(&data.resource))); // back buffer�� ���� ������ ����.

			D3D12_RENDER_TARGET_VIEW_DESC desc{};
			desc.Format = render_target_format; // ���������� ����Ϳ� ���̴� ����� ���, comma corrected�� srgb ������ �����.
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			// ���� Ÿ�� �� ����. ���ڷ� resource(���⼭�� back buffer �ؽ���), rtv ������, cpu descriptor handle�� �޾���. 
			D3D12_Renderer::Get_Device()->CreateRenderTargetView(data.resource, &desc, data.rtv.cpu);

			// ��ũ�θ� �̿��ؼ� device ��ü�� �̸��� �ٿ���.
			NAME_D3D12_OBJECT_INDEXED(data.resource, i, L"Swapchain Back Buffer");
		}

		// �� ���� �������� ��� ������ ������� �����ؾ� ��. ���� ü���� desc ����ü�� ���ͼ� back buffer ũ�⸦ Ȯ��.
		DXGI_SWAP_CHAIN_DESC desc{};
		DXCHECK(_swap_chain->GetDesc(&desc));
		const uint32 width{ desc.BufferDesc.Width };
		const uint32 height{ desc.BufferDesc.Height };
		assert(_window->Get_Width() == width && _window->Get_Height() == height);

		// ����Ʈ�� scissor rect ����.
		_viewport.TopLeftX = 0.f;
		_viewport.TopLeftY = 0.f;
		_viewport.Width = (float)width;
		_viewport.Height = (float)height;
		_viewport.MinDepth = 0.f;
		_viewport.MaxDepth = 1.0f;

		_scissor_rect = { 0,0,(int32)width, (int32)height };
	}
}