
#pragma once

#include "Event.h"

/*
 *	���콺 �Է� ���� �̺�Ʈ.
 */

namespace Engine
{
	// ���콺 �̵� �� �߻� ��ų �̺�Ʈ.
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

	// ���콺 �� ��ũ�� �� �߻� ��ų �̺�Ʈ���� �⺻ Ŭ����.
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

	// ���콺 ���� ���� �÷��� �� �߻� ��ų �̺�Ʈ.
	class Mouse_WheelUp_Event : public Mouse_Scrolled_Event
	{
	public:
		Mouse_WheelUp_Event(float xOffset, float yOffset)
			: Mouse_Scrolled_Event(xOffset, yOffset) {}

		EVENT_CLASS_TYPE(Mouse_WheelUp)
	};

	// ���콺 ���� �Ʒ��� ������ �� �߻� ��ų �̺�Ʈ.
	class Mouse_WheelDown_Event : public Mouse_Scrolled_Event
	{
	public:
		Mouse_WheelDown_Event(float xOffset, float yOffset)
			: Mouse_Scrolled_Event(xOffset, yOffset) {}

		EVENT_CLASS_TYPE(Mouse_WheelDown)
	};

	// ���콺 ��ư �̺�Ʈ���� �߻� Ŭ����. �̸� ������ ��ü���� ��ư �̺�Ʈ�� �������.
	class Mouse_Button_Event : public Event
	{
	public:
		inline int GetMouseButton() const { return _button; }

		EVENT_CLASS_CATEGORY(Event_Category_Mouse | Event_Category_Input)
	protected:
		Mouse_Button_Event(int button)
			: _button(button) {}

		int _button; // Ŭ���� ��ư.
	};

	// ���콺 ��ư�� ������ �� �߻� ��ų �̺�Ʈ.
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

	// ���콺 ��ư�� ������ �� �߻� ��ų �̺�Ʈ.
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