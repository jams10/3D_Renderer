#pragma once

#include "Common/CommonHeaders.h"
#include "Event/ApplicationEvent.h"
#include "Utils/GameTimer.h"
#include "Layer/LayerStack.h"

namespace Engine
{
	class Window;

	/*
	* GameLoop를 수행하는 클래스. SandBox 프로젝트 쪽에서 이 클래스를 상속 받아 Loop에서 실행 시킬 로직을 구현함.
	*/
	class Application
	{
	public:
		Application();
		virtual ~Application(); // SandBox 프로젝트의 Application에서 이 함수를 상속 받기 떄문에 기본 클래스인 이 클래스의 소멸자를 가상 함수로 선언해줌.

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

	// SandBox 프로젝트에서 Application 클래스를 상속 받고 이 함수를 구현해줌.
	Application* Create_Application();
}