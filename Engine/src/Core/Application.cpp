
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

		// ������ �޽��� �߻� �� �̺�Ʈ�� ����� �Ѱ��� �� ó������ �ݹ� �Լ��� ���ڷ� ���� �Ѱ� ������ ����.
		// ������ ���� �Ŀ� �ݹ� �Լ��� ���ε� ������, ������ ���� �� WM_SIZE �޽����� �ٷ� �߻��ϰ� �Ǵµ� 
		// �� ���������� �ݹ� �Լ��� ���ε� �Ǿ� ���� �ʱ� ������ �� �Լ� ��ü�� ȣ���ϱ� ������ bad_function_call ���� �߻�.
		// ����, ������ Ŭ���� ��ü ���� ������ �ݹ� �Լ��� ���� ���ε� ����.
		_wnd = std::unique_ptr<Window>(Window::Create(BIND_EVENT_CALLBACK(&Application::Process_Event)));

		_gfx = std::unique_ptr<Renderer>(Renderer::Create(Graphics::GraphicsPlatform::Direct3D12));
		_gfx->Initialize();
	}

	Application::~Application()
	{
		_gfx->Shutdown();

		// entry point �ν��Ͻ� ���� ���ŵ�.
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

				for (Layer* layer : _layer_stack) // ���̾���� update ȣ��.
					layer->On_Update(_timer.Delta_Time());
			}

			_gfx->Render();

			_wnd->On_Update();
		}
	}

	// �̺�Ʈ�� ó���ϴ� �Լ�.
	void Application::Process_Event(Event& event)
	{
		// ���� �̺�Ʈ ��ü�� ���ڷ� �Ͽ� dispatcher ��ü ����.
		Event_Dispatcher dispatcher(event);

		// Dispatch() �Լ� ���ο��� ���ø� ������ �̺�Ʈ Ŭ���� Ÿ�԰� dispatcher ��ü ������ �߰���, �ڽ��� ��� �ִ� event ��ü�� Ÿ���� ���� �ݹ� �Լ��� ������.
		// ���� ���� �̺�Ʈ�� WindowCloseEvent�� ���, OnWindowClose �Լ��� ȣ���ϰ�, ResizeEvent�� ��� OnWindowResize �Լ��� ȣ��ǰ� ��.
		dispatcher.Dispatch<Window_Close_Event>(BIND_EVENT_CALLBACK(&Application::On_Window_Close));
		dispatcher.Dispatch<Window_Resize_Event>(BIND_EVENT_CALLBACK(&Application::On_Window_Resize));
		dispatcher.Dispatch<Window_Active_Event>(BIND_EVENT_CALLBACK(&Application::On_Window_Active));

		// ���� ���̾���� �̺�Ʈ�� �޵��� ��. �̺�Ʈ�� ���� ���̾ ���� ���̾�� �̺�Ʈ�� �������� �ʱ� ���� Handled ���� true�� �ٲٸ� 
		// �ش� �̺�Ʈ�� �� �̻� ó������ �ʰ� ��������.
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

	// WindowClose �̺�Ʈ ó�� �Լ�.
	bool Application::On_Window_Close(Window_Close_Event event)
	{
		_is_running = false;
		return true; // ���� ���̾ �̺�Ʈ�� �������� �ʵ��� true ����.
	}

	// WindowResize �̺�Ʈ ó�� �Լ�.
	bool Application::On_Window_Resize(Window_Resize_Event event)
	{
		if (event.Get_Width() == 0 || event.Get_Height() == 0)
		{
			_is_window_minimized = true;
			return false; // ���� ���̾ �̺�Ʈ ����.
		}
		_is_window_minimized = false;
		// TODO : ������ ȭ�� ������¡ �۾�.

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