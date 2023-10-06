
#include "Application.h"
#include "Platform/Win32/WindowsWindow.h"
#include "Utils/CustomLog.h"

namespace Engine
{
	Application* Application::_instance = nullptr;

	Application::Application()
	{
		assert(_instance == nullptr);

		// ������ �޽��� �߻� �� �̺�Ʈ�� ����� �Ѱ��� �� ó������ �ݹ� �Լ��� ���ڷ� ���� �Ѱ� ������ ����.
		// ������ ���� �Ŀ� �ݹ� �Լ��� ���ε� ������, ������ ���� �� WM_SIZE �޽����� �ٷ� �߻��ϰ� �Ǵµ� 
		// �� ���������� �ݹ� �Լ��� ���ε� �Ǿ� ���� �ʱ� ������ �� �Լ� ��ü�� ȣ���ϱ� ������ bad_function_call ���� �߻�.
		// ����, ������ Ŭ���� ��ü ���� ������ �ݹ� �Լ��� ���� ���ε� ����.
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

	// �̺�Ʈ�� ó���ϴ� �Լ�.
	void Application::Process_Event(Event& event)
	{
		// ���� �̺�Ʈ ��ü�� ���ڷ� �Ͽ� dispatcher ��ü ����.
		Event_Dispatcher dispatcher(event);

		// Dispatch() �Լ� ���ο��� ���ø� ������ �̺�Ʈ Ŭ���� Ÿ�԰� dispatcher ��ü ������ �߰���, �ڽ��� ��� �ִ� event ��ü�� Ÿ���� ���� �ݹ� �Լ��� ������.
		// ���� ���� �̺�Ʈ�� WindowCloseEvent�� ���, OnWindowClose �Լ��� ȣ���ϰ�, ResizeEvent�� ��� OnWindowResize �Լ��� ȣ��ǰ� ��.
		dispatcher.Dispatch<Window_Close_Event>(BIND_EVENT_CALLBACK(&Application::On_Window_Close));
		dispatcher.Dispatch<Window_Resize_Event>(BIND_EVENT_CALLBACK(&Application::On_Window_Resize));

		// �׽�Ʈ�� �α�.
		if (event.Get_Event_Type() == Event_Type::Window_Resize)
		{
			LOG_EVENT(event);
		}
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
		return true;
	}
}