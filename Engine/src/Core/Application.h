#pragma once

#include "Common/CommonHeaders.h"
#include "Event/ApplicationEvent.h"

namespace Engine
{
	class Window;

	/*
	* GameLoop�� �����ϴ� Ŭ����. SandBox ������Ʈ �ʿ��� �� Ŭ������ ��� �޾� Loop���� ���� ��ų ������ ������.
	*/
	class Application
	{
	public:
		Application();
		virtual ~Application(); // SandBox ������Ʈ�� Application���� �� �Լ��� ��� �ޱ� ������ �⺻ Ŭ������ �� Ŭ������ �Ҹ��ڸ� ���� �Լ��� ��������.

		void Run();

		void Process_Event(Event& event);

	private:
		bool On_Window_Close(Window_Close_Event event);
		bool On_Window_Resize(Window_Resize_Event event);

	private:
		std::unique_ptr<Window> _wnd;
		bool _is_running = true;

		static Application* _instance;
	};

	// SandBox ������Ʈ���� Application Ŭ������ ��� �ް� �� �Լ��� ��������.
	Application* Create_Application();
}