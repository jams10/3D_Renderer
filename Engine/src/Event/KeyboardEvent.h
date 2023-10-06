
#pragma once

#include "Event.h"

/*
 *	키보드 입력 관련 이벤트.
 */

namespace Engine
{

	// 모든 키보드 이벤트들의 추상 클래스. 이를 구현해 구체적인 키보드 이벤트를 만들어줌.
	class Key_Event : public Event
	{
	public:
		inline int GetKeyCode() const { return _keyCode; }

		EVENT_CLASS_CATEGORY(Event_Category_Keyboard | Event_Category_Input)
	protected:
		Key_Event(int keycode)
			: _keyCode(keycode) {}

		int _keyCode; // 누르거나 뗀 키
	};

	// 키를 눌렀을 때 발생 시킬 이벤트.
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
		int _repeatCount; // 반복해서 해당 키를 누른 횟수를 저장.
	};

	// 키를 떼었을 때 발생 시킬 이벤트.
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