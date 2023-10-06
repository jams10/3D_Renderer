
#pragma once

#include "Event.h"

/*
 *	Ű���� �Է� ���� �̺�Ʈ.
 */

namespace Engine
{

	// ��� Ű���� �̺�Ʈ���� �߻� Ŭ����. �̸� ������ ��ü���� Ű���� �̺�Ʈ�� �������.
	class Key_Event : public Event
	{
	public:
		inline int GetKeyCode() const { return _keyCode; }

		EVENT_CLASS_CATEGORY(Event_Category_Keyboard | Event_Category_Input)
	protected:
		Key_Event(int keycode)
			: _keyCode(keycode) {}

		int _keyCode; // �����ų� �� Ű
	};

	// Ű�� ������ �� �߻� ��ų �̺�Ʈ.
	class Key_Pressed_Event : public Key_Event
	{
	public:
		Key_Pressed_Event(int keycode, int repeatCount)
			: Key_Event(keycode), _repeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return _repeatCount; }

		std::string To_String() const override
		{
			std::stringstream ss;
			ss << "Key Pressed Event: " << _keyCode << " (" << _repeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(Key_Pressed)
	private:
		int _repeatCount; // �ݺ��ؼ� �ش� Ű�� ���� Ƚ���� ����.
	};

	// Ű�� ������ �� �߻� ��ų �̺�Ʈ.
	class Key_Released_Event : public Key_Event
	{
	public:
		Key_Released_Event(int keycode)
			: Key_Event(keycode) {}

		std::string To_String() const override
		{
			std::stringstream ss;
			ss << "Key Released Event: " << _keyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(Key_Released)
	};
}