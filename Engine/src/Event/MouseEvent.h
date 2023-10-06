
#pragma once

#include "Event.h"

/*
 *	마우스 입력 관련 이벤트.
 */

namespace Engine
{
	// 마우스 이동 시 발생 시킬 이벤트.
	class Mouse_Moved_Event : public Event
	{
	public:
		Mouse_Moved_Event(float x, float y)
			: _mouseX(x), _mouseY(y) {}

		inline float GetX() const { return _mouseX; }
		inline float GetY() const { return _mouseY; }

		std::string To_String() const override
		{
			std::stringstream ss;
			ss << "Mouse Moved Event: " << _mouseX << ", " << _mouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(Mouse_Moved)
		EVENT_CLASS_CATEGORY(Event_Category_Mouse | Event_Category_Input)
	private:
		float _mouseX, _mouseY;
	};

	// 마우스 휠 스크롤 시 발생 시킬 이벤트들의 기본 클래스.
	class Mouse_Scrolled_Event : public Event
	{
	public:
		Mouse_Scrolled_Event(float xOffset, float yOffset)
			: _xOffset(xOffset), _yOffset(yOffset) {}

		inline float Get_XOffset() const { return _xOffset; }
		inline float Get_YOffset() const { return _yOffset; }

		std::string To_String() const override
		{
			std::stringstream ss;
			ss << "Mouse Scrolled Event: " << Get_XOffset() << ", " << Get_YOffset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(Mouse_Scrolled)
		EVENT_CLASS_CATEGORY(Event_Category_Mouse | Event_Category_Input)
	private:
		float _xOffset, _yOffset;
	};

	// 마우스 휠을 위로 올렸을 때 발생 시킬 이벤트.
	class Mouse_WheelUp_Event : public Mouse_Scrolled_Event
	{
	public:
		Mouse_WheelUp_Event(float xOffset, float yOffset)
			: Mouse_Scrolled_Event(xOffset, yOffset) {}

		EVENT_CLASS_TYPE(Mouse_WheelUp)
	};

	// 마우스 휠을 아래로 내렸을 때 발생 시킬 이벤트.
	class Mouse_WheelDown_Event : public Mouse_Scrolled_Event
	{
	public:
		Mouse_WheelDown_Event(float xOffset, float yOffset)
			: Mouse_Scrolled_Event(xOffset, yOffset) {}

		EVENT_CLASS_TYPE(Mouse_WheelDown)
	};

	// 마우스 버튼 이벤트들의 추상 클래스. 이를 구현해 구체적인 버튼 이벤트를 만들어줌.
	class Mouse_Button_Event : public Event
	{
	public:
		inline int GetMouseButton() const { return _button; }

		EVENT_CLASS_CATEGORY(Event_Category_Mouse | Event_Category_Input)
	protected:
		Mouse_Button_Event(int button)
			: _button(button) {}

		int _button; // 클릭한 버튼.
	};

	// 마우스 버튼을 눌렀을 때 발생 시킬 이벤트.
	class Mouse_Button_Pressed_Event : public Mouse_Button_Event
	{
	public:
		Mouse_Button_Pressed_Event(int button)
			: Mouse_Button_Event(button) {}

		std::string To_String() const override
		{
			std::stringstream ss;
			ss << "Mouse Button Pressed Event: " << _button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(Mouse_Button_Pressed)
	};

	// 마우스 버튼을 떼었을 때 발생 시킬 이벤트.
	class Mouse_Button_Released_Event : public Mouse_Button_Event
	{
	public:
		Mouse_Button_Released_Event(int button)
			: Mouse_Button_Event(button) {}

		std::string To_String() const override
		{
			std::stringstream ss;
			ss << "Mouse Button Released Event: " << _button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(Mouse_Button_Released)
	};

}