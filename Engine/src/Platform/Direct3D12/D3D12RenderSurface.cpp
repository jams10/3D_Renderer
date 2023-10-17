
#include "D3D12RenderSurface.h"
#include "D3D12Renderer.h"
#include "D3D12Macros.h"

namespace Engine::Graphics::D3D12
{
	namespace
	{
		// SRGB 형식을 non-SRGB 형식으로 변환. 
		constexpr DXGI_FORMAT To_Non_Srgb(DXGI_FORMAT format)
		{
			if (format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB) return DXGI_FORMAT_R8G8B8A8_UNORM;
			else return format;
		}
	}

	// 스왑 체인을 생성하는 함수.
	void D3D12_RenderSurface::Create_Swap_Chain(IDXGIFactory7* factory, ID3D12CommandQueue* cmd_queue, DXGI_FORMAT format)
	{
		assert(factory && cmd_queue);
		Release(); // 이전에 스왑 체인을 생성 했다면 이를 날려줌.

		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.BufferCount = frame_buffer_count; // 스왑 체인이 가질 backbuffer 갯수. 우리는 3개의 프레임 버퍼에 대한 명령을 저장하고 있으므로, 3개로 설정해 주었음.(사실 두 개 이상이기만 하면 상관없음.) D3D12에서 스왑체인은 최소 3개 이상의 버퍼가 필요함.
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.Flags = 0;
		desc.Format = To_Non_Srgb(format);     // 스왑 체인의 back buffer는 srgb 형식으로 만들어줄 수 없음. 따라서 srgb 형식이 인자로 들어오면, 이를 srgb가 아닌 형식으로 변환해 주어야 함.
		desc.Height = _window->Get_Height();
		desc.Width = _window->Get_Width();
		// 우리는 이 surface를 최종 렌더 타겟으로 사용할 것임. 즉, 최종적으로 완성된 이미지를 이 backbuffer 텍스쳐에 복사해줄 것임. 
		// 따라서 anti-aliasing이나 super sampling을 사용할 필요가 없음.
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		// 윈도우 리사이징 할 때 늘어나도록 stretch로 설정.
		desc.Scaling = DXGI_SCALING_STRETCH;
		// d3d12는 flip_sequential과 flip_discard 두 개만 지원함. 우리는 back buffer를 다른 back buffer의 입력으로 사용하지 않기 때문에 flip_discard를 사용함. 즉, 스왑되면 front 버퍼 버려줌.
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Stereo = false; // 3D 스테레오 렌더링을 지원하는지 여부.

		IDXGISwapChain1* swap_chain;
		HWND hwnd{ (HWND)_window->Get_Native_Window() };
		// 스왑 체인이 부착될 윈도우의 핸들을 인자로 받아줌.
		DXCHECK(factory->CreateSwapChainForHwnd(cmd_queue, hwnd, &desc, nullptr, nullptr, &swap_chain));
		// 스왑 체인은 사용자가 alt + enter 키를 눌렀을 때 창을 전체 화면으로 만들어주는 기능을 가지고 있음. 
		// 우리는 최대화된 경계 없는 창으로 전체 화면을 처리하고 있기 때문에 이 기능을 꺼줌.
		DXCHECK(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
		// CreateSwapChainForHwnd 함수가 IDXGISwapChain1 타입을 인자로 받지만, 우리는 최신 버전의 스왑 체인을 사용하기 위해 쿼리를 통해 얻어옴.
		DXCHECK(swap_chain->QueryInterface(IID_PPV_ARGS(&_swap_chain)));

		::Release(swap_chain); // 최신 버전 스왑 체인을 사용할 것이므로 기존에 생성한 IDXGISwapChain1 객체는 해제.

		_current_bb_index = _swap_chain->GetCurrentBackBufferIndex();

		// swap chain에 있는 back buffer들에 대해 RTV를 생성하기 위해 render target view descriptor heap에 slot을 할당함.
		// Allocate_Descriptor() 함수에서는 descriptor가 할당될 heap 상에서의 offset 주소를 받아올 수 있음.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			_render_target_data[i].rtv = D3D12_Renderer::Get_Rtv_Desc_Heap().Allocate_Descriptor();
		}

		Finalize();
	}

	// 씬이 렌더링된 back buffer를 front buffer와 교체해서 화면에 보여주는 함수.
	void D3D12_RenderSurface::Present() const
	{
		assert(_swap_chain);
		DXCHECK(_swap_chain->Present(0, 0)); // present 함수는 현재 back buffer 인덱스를 증가 시킴.
		_current_bb_index = _swap_chain->GetCurrentBackBufferIndex(); // 현재 back buffer 인덱스를 증가된 back buffer 인덱스로 갱신.
	}

	void D3D12_RenderSurface::Resize()
	{

	}

	void D3D12_RenderSurface::Release()
	{
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			Render_Target_Data& data{ _render_target_data[i] };
			::Release(data.resource);    // resource COM 인터페이스 객체 release.
			D3D12_Renderer::Get_Rtv_Desc_Heap().Free_Descriptor(data.rtv); // descriptor_handle을 인자로 전달, heap에서 해당 descriptor를 release(). 내부적으로는 기존 할당 공간을 덮어쓰도록 열어줌.
		}

		::Release(_swap_chain); // 스왑 체인 객체 release. 스왑 체인을 참조하는 리소스들을 위에서 먼저 release 한 뒤에 release 해야 스왑 체인이 release 됨.
	}

	// back buffer에 대한 RTV를 생성, 뷰포트와 scissor rect를 window 크기로 설정함.
	void D3D12_RenderSurface::Finalize()
	{
		// back buffer들에 대해 rtv 생성.
		for (uint32 i{ 0 }; i < frame_buffer_count; ++i)
		{
			Render_Target_Data& data{ _render_target_data[i] };
			assert(!data.resource);
			DXCHECK(_swap_chain->GetBuffer(i, IID_PPV_ARGS(&data.resource))); // back buffer에 대한 포인터 얻어옴.

			D3D12_RENDER_TARGET_VIEW_DESC desc{};
			desc.Format = render_target_format; // 최종적으로 모니터에 보이는 결과의 경우, comma corrected된 srgb 형식을 사용함.
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
			// 렌더 타겟 뷰 생성. 인자로 resource(여기서는 back buffer 텍스쳐), rtv 서술자, cpu descriptor handle을 받아줌. 
			D3D12_Renderer::Get_Device()->CreateRenderTargetView(data.resource, &desc, data.rtv.cpu);

			// 매크로를 이용해서 device 객체에 이름을 붙여줌.
			NAME_D3D12_OBJECT_INDEXED(data.resource, i, L"Swapchain Back Buffer");
		}

		// 백 버퍼 사이즈의 경우 윈도우 사이즈와 동일해야 함. 스왑 체인의 desc 구조체를 얻어와서 back buffer 크기를 확인.
		DXGI_SWAP_CHAIN_DESC desc{};
		DXCHECK(_swap_chain->GetDesc(&desc));
		const uint32 width{ desc.BufferDesc.Width };
		const uint32 height{ desc.BufferDesc.Height };
		assert(_window->Get_Width() == width && _window->Get_Height() == height);

		// 뷰포트와 scissor rect 설정.
		_viewport.TopLeftX = 0.f;
		_viewport.TopLeftY = 0.f;
		_viewport.Width = (float)width;
		_viewport.Height = (float)height;
		_viewport.MinDepth = 0.f;
		_viewport.MaxDepth = 1.0f;

		_scissor_rect = { 0,0,(int32)width, (int32)height };
	}
}