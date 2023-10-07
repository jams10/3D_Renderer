
#include "Renderer.h"
#include "Platform/Direct3D12/D3D12Renderer.h"
#include "Utils/CustomLog.h"

namespace Engine::Graphics
{
	Renderer* Renderer::Create(GraphicsPlatform platform)
	{
		switch (platform)
		{
		case GraphicsPlatform::Direct3D12:
			return new D3D12::D3D12_Renderer();
		}

		LOG_ERROR("Platform not supported.")

		return nullptr;
	}
}