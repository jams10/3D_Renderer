
#include "Application.h"
#include "Platform/Win32/WindowsWindow.h"
#include "Utils/CustomLog.h"

namespace Engine
{
	Application* Application::_instance = nullptr;

	Application::Application()
	{
		assert(_instance == nullptr);

		// 윈도우 메시지 발생 시 이벤트를 만들어 넘겼을 때 처리해줄 콜백 함수를 인자로 같이 넘겨 윈도우 생성.
		// 원래는 생성 후에 콜백 함수를 바인딩 했으나, 윈도우 생성 시 WM_SIZE 메시지가 바로 발생하게 되는데 
		// 이 시점에서는 콜백 함수가 바인딩 되어 있지 않기 때문에 빈 함수 객체를 호출하기 때문에 bad_function_call 오류 발생.
		// 따라서, 윈도우 클래스 객체 생성 시점에 콜백 함수도 같이 바인딩 해줌.
		_wnd = std::unique_ptr<Window>(Window::Create(BIND_EVENT_CALLBACK(&Application::Process_Event)));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (_is_running)
		{
			_wnd->On_Update();
		}
	}

	// 이벤트를 처리하는 함수.
	void Application::Process_Event(Event& event)
	{
		// 들어온 이벤트 객체를 인자로 하여 dispatcher 객체 생성.
		Event_Dispatcher dispatcher(event);

		// Dispatch() 함수 내부에서 템플릿 인자인 이벤트 클래스 타입과 dispatcher 객체 생성시 추가된, 자신이 들고 있는 event 객체의 타입을 비교해 콜백 함수를 실행함.
		// 따라서 들어온 이벤트가 WindowCloseEvent인 경우, OnWindowClose 함수를 호출하고, ResizeEvent인 경우 OnWindowResize 함수가 호출되게 됨.
		dispatcher.Dispatch<Window_Close_Event>(BIND_EVENT_CALLBACK(&Application::On_Window_Close));
		dispatcher.Dispatch<Window_Resize_Event>(BIND_EVENT_CALLBACK(&Application::On_Window_Resize));

		// 테스트용 로그.
		if (event.Get_Event_Type() == Event_Type::Window_Resize)
		{
			LOG_EVENT(event);
		}
	}

	// WindowClose 이벤트 처리 함수.
	bool Application::On_Window_Close(Window_Close_Event event)
	{
		_is_running = false;
		return true; // 하위 레이어에 이벤트를 전파하지 않도록 true 리턴.
	}

	// WindowResize 이벤트 처리 함수.
	bool Application::On_Window_Resize(Window_Resize_Event event)
	{
		return true;
	}
}