
#pragma once

#include "Common/CommonHeaders.h"
#include "Event/Event.h"

namespace Engine
{
	using std::wstring;
	// 윈도우의 기본적인 정보(타이틀, 너비, 높이)가 담긴 구조체.
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
	*   윈도우를 추상화 한 추상 클래스(인터페이스 클래스).
	*   플랫폼 별로 이를 구체화 하여 실제 윈도우 클래스를 만들어줌.
	*/
	class Window
	{
	public:
		using Event_Callback_Fn = std::function<void(Event&)>; // 리턴 타입이 void, 인자로 Event 참조 타입을 받아주는 함수 객체.

		virtual ~Window() {}

		virtual void On_Update() = 0;

		virtual uint16 Get_Width() const = 0;
		virtual uint16 Get_Height() const = 0;

		virtual void Set_Event_Callback(const Event_Callback_Fn& callback) = 0;
		virtual void Set_VSync(bool enabled) = 0;
		virtual bool Is_VSync() const = 0;
		virtual void* Get_Native_Window() const = 0;

		// 플랫폼 별로 다르게 구현할 Window를 생성하는 함수.
		static Window* Create(const Event_Callback_Fn& callback, const Window_Info& infos = Window_Info());
	};
}