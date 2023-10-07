#pragma once

#include "Common/CommonHeaders.h"
#include "Event/ApplicationEvent.h"
#include "Utils/GameTimer.h"
#include "Layer/LayerStack.h"

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

		void Push_Layer(Layer* layer);
		void Pop_Layer(Layer* layer);

	private:
		bool On_Window_Close(Window_Close_Event event);
		bool On_Window_Resize(Window_Resize_Event event);
		bool On_Window_Active(Window_Active_Event event);

	private:
		bool _is_running = true;
		bool _is_window_minimized = false;
		bool _is_window_active = true;

		std::unique_ptr<Window> _wnd;
		Game_Timer _timer;
		Layer_Stack _layer_stack;

		static Application* _instance;
	};

	// SandBox ������Ʈ���� Application Ŭ������ ��� �ް� �� �Լ��� ��������.
	Application* Create_Application();
}