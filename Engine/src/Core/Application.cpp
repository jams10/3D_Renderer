
#include "Application.h"
#include "Platform/Win32/WindowsWindow.h"
#include "Utils/CustomLog.h"

namespace Engine
{
	Application* Application::_instance = nullptr;

	Application::Application()
	{
		assert(_instance == nullptr);
		_instance = this;

		// 윈도우 메시지 발생 시 이벤트를 만들어 넘겼을 때 처리해줄 콜백 함수를 인자로 같이 넘겨 윈도우 생성.
		// 원래는 생성 후에 콜백 함수를 바인딩 했으나, 윈도우 생성 시 WM_SIZE 메시지가 바로 발생하게 되는데 
		// 이 시점에서는 콜백 함수가 바인딩 되어 있지 않기 때문에 빈 함수 객체를 호출하기 때문에 bad_function_call 오류 발생.
		// 따라서, 윈도우 클래스 객체 생성 시점에 콜백 함수도 같이 바인딩 해줌.
		_wnd = std::unique_ptr<Window>(Window::Create(BIND_EVENT_CALLBACK(&Application::Process_Event)));

		_gfx = std::unique_ptr<Renderer>(Renderer::Create(Graphics::GraphicsPlatform::Direct3D12));
		_gfx->Initialize();
	}

	Application::~Application()
	{
		_gfx->Shutdown();

		// entry point 인스턴스 에서 제거됨.
		_instance = nullptr;
	}

	void Application::Run()
	{
		_timer.Reset();

		while (_is_running)
		{
			if (_is_window_minimized == false && _is_window_active == true)
			{
				_timer.Tick();

				for (Layer* layer : _layer_stack) // 레이어들의 update 호출.
					layer->On_Update(_timer.Delta_Time());
			}

			_gfx->Render();

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
		dispatcher.Dispatch<Window_Active_Event>(BIND_EVENT_CALLBACK(&Application::On_Window_Active));

		// 상위 레이어부터 이벤트를 받도록 함. 이벤트를 받은 레이어가 하위 레이어로 이벤트를 전파하지 않기 위해 Handled 값을 true로 바꾸면 
		// 해당 이벤트를 더 이상 처리하지 않고 빠져나감.
		for (auto it = _layer_stack.end(); it != _layer_stack.begin(); )
		{
			(*--it)->On_Event(event);
			if (event.handled)
				break;
		}
	}

	void Application::Push_Layer(Layer* layer)
	{
		_layer_stack.Push_Layer(layer);
	}

	void Application::Pop_Layer(Layer* layer)
	{
		_layer_stack.Pop_Layer(layer);
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
		if (event.Get_Width() == 0 || event.Get_Height() == 0)
		{
			_is_window_minimized = true;
			return false; // 하위 레이어에 이벤트 전파.
		}
		_is_window_minimized = false;
		// TODO : 렌더링 화면 리사이징 작업.

		return false;
	}

	bool Application::On_Window_Active(Window_Active_Event event)
	{
		_is_window_active = event.Get_Is_Active();

		if (_is_window_active == true)
			_timer.Start();
		else _timer.Stop();

		return true;
	}
}