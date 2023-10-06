
#pragma once

#include "Common/CommonHeaders.h"
#include "Event/Event.h"

namespace Engine
{
	using std::wstring;
	// �������� �⺻���� ����(Ÿ��Ʋ, �ʺ�, ����)�� ��� ����ü.
	struct Window_Info
	{
		wstring title;
		uint16 width;
		uint16 height;

		Window_Info(wstring title_in = L"Renderer", uint16 width_in = 1280, uint16 height_in = 720)
			: title(title_in), width(width_in), height(height_in)
		{}
	};

	/*
	*   <Window>
	*   �����츦 �߻�ȭ �� �߻� Ŭ����(�������̽� Ŭ����).
	*   �÷��� ���� �̸� ��üȭ �Ͽ� ���� ������ Ŭ������ �������.
	*/
	class Window
	{
	public:
		using Event_Callback_Fn = std::function<void(Event&)>; // ���� Ÿ���� void, ���ڷ� Event ���� Ÿ���� �޾��ִ� �Լ� ��ü.

		virtual ~Window() {}

		virtual void On_Update() = 0;

		virtual uint16 Get_Width() const = 0;
		virtual uint16 Get_Height() const = 0;

		virtual void Set_Event_Callback(const Event_Callback_Fn& callback) = 0;
		virtual void Set_VSync(bool enabled) = 0;
		virtual bool Is_VSync() const = 0;
		virtual void* Get_Native_Window() const = 0;

		// �÷��� ���� �ٸ��� ������ Window�� �����ϴ� �Լ�.
		static Window* Create(const Event_Callback_Fn& callback, const Window_Info& infos = Window_Info());
	};
}