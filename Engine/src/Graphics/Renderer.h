#pragma once

#include "Common/CommonHeaders.h"

namespace Engine::Graphics
{
	// 사용할 렌더링 API를 나타내는 enum. 나중에 다른 렌더링 API를 사용해 렌더러를 구현할 수 있도록 함.
	enum class GraphicsPlatform : uint32
	{
		Direct3D12 = 0,
	};

	class Renderer
	{
	public:
		Renderer() {}
		virtual ~Renderer() {}
		DISABLE_COPY_AND_MOVE(Renderer)

		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void Render() = 0;

		static Renderer* Create(GraphicsPlatform platform);
	};


}