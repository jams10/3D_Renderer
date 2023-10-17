#pragma once

#include "Common/CommonHeaders.h"
#include "Window/Window.h"

namespace Engine::Graphics
{
	// ����� ������ API�� ��Ÿ���� enum. ���߿� �ٸ� ������ API�� ����� �������� ������ �� �ֵ��� ��.
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

		virtual bool Initialize(Window* wnd) = 0;
		virtual void Shutdown() = 0;
		virtual void Render() = 0;

		static Renderer* Create(GraphicsPlatform platform);
	};


}