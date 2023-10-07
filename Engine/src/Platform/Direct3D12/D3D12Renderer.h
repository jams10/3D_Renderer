#pragma once

#include "Graphics/Renderer.h"
#include "D3D12Headers.h"
#include "D3D12Command.h"

namespace Engine::Graphics::D3D12
{
	using Microsoft::WRL::ComPtr;

	class D3D12_Renderer : public Renderer
	{
	public:
		D3D12_Renderer();

		DISABLE_COPY_AND_MOVE(D3D12_Renderer)

		virtual bool Initialize() override;
		virtual void Shutdown() override;
		virtual void Render() override;

	private:
		bool Failed_Initialize();
		IDXGIAdapter4* Determine_MainAdapter();
		D3D_FEATURE_LEVEL Get_Maximum_Feature_Level(IDXGIAdapter4* adapter);

	private:
		ID3D12Device8* _main_device{ nullptr };
		IDXGIFactory7* _dxgi_factory{ nullptr };
		D3D12_Command  _gfx_command;

		D3D_FEATURE_LEVEL _minimum_feature_level{ D3D_FEATURE_LEVEL_11_0 }; // 렌더러가 지원할 가장 낮은 feature level.
	};
}