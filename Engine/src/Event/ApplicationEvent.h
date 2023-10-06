
#pragma once

#include "Event.h"

/*
 *	Window 관련 이벤트. Application의 업데이트 렌더링 이벤트.
 */

namespace Engine
{
	// 윈도우 크기 조절 시 발생 시킬 이벤트.
	class Window_Resize_Event : public Event
	{
	public:
		Window_Resize_Event(unsigned int width, unsigned int height)
			: _width(width), _height(height) {}

		inline unsigned int Get_Width() const { return _width; }
		inline unsigned int Get_Height() const { return _height; }

		std::string To_String() const override
		{
			std::stringstream ss;
			ss << "Window Resize Event: " << _width << ", " << _height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(Window_Resize)
		EVENT_CLASS_CATEGORY(Event_Category_Application)
	private:
		unsigned int _width, _height;
	};

	// 윈도우를 껐을 때 발생 시킬 이벤트.
	class Window_Close_Event : public Event
	{
	public:
		Window_Close_Event() {}

		EVENT_CLASS_TYPE(Window_Close)
		EVENT_CLASS_CATEGORY(Event_Category_Application)
	};

	// Application 업데이트 작업 시 발생 시킬 이벤트.
	class App_Update_Event : public Event
	{
	public:
		App_Update_Event() {}

		EVENT_CLASS_TYPE(App_Update)
		EVENT_CLASS_CATEGORY(Event_Category_Application)
	};

	// Application 렌더링 작업 시 발생 시킬 이벤트.
	class App_Render_Event : public Event
	{
	public:
		App_Render_Event() {}

		EVENT_CLASS_TYPE(App_Render)
		EVENT_CLASS_CATEGORY(Event_Category_Application)
	};
}