
#pragma once

#include "Common/CommonHeaders.h"
#include "EventMacros.h"

namespace Engine
{
	// 이벤트 타입을 정의하는 enum class.
	// 각 개별 이벤트 클래스의 이름과 enum을 맞춰 줘야 EVENT_CLASS_TYPE() 매크로를 통해 개별 이벤트 클래스의 타입을 받아올 수 있음.
	enum class Event_Type
	{
		None = 0,
		Window_Close, Window_Resize, Window_Focus, Window_Lost_Focus, Window_Moved,
		App_Update, App_Render,
		Key_Pressed, Key_Released,
		Mouse_Button_Pressed, Mouse_Button_Released, Mouse_Moved, Mouse_Scrolled, Mouse_WheelUp, Mouse_WheelDown
	};

	// 이벤트를 필터링 하기 위한 이벤트 카테고리 enum. 특정 이벤트만 가져오고 싶을 때 사용.
	// 여러 개를 한 번에 필터링 할 수 있도록 bit 연산 사용.(bitflag)
	enum Event_Category
	{
		None = 0,
		Event_Category_Application = BIT(0),
		Event_Category_Input = BIT(1),
		Event_Category_Keyboard = BIT(2),
		Event_Category_Mouse = BIT(3),
		Event_Category_MouseButton = BIT(4)
	};
	// Event 클래스를 상속하는 구체적인 이벤트 클래스들의 함수 부분을 편하게 작성할 수 있도록 매크로 작성.
	// #  : 받아온 매크로 변수를 문자열로 바꿔줌.
	// ## : 두 개의 토큰을 붙여줌.
	// 이벤트 타입만 가져오는 것은 클래스 인스턴스가 필요 없기 때문에 static 함수로 만들어줌.
	// 가상 함수인 GetEventType() 함수를 이용, 다형성을 통해 Event 클래스 참조 하나로 모든 이벤트 인스턴스의 이벤트 타입을 가져올 수 있도록 함.
#define EVENT_CLASS_TYPE(type) static Event_Type Get_Static_Type() { return Event_Type::##type; }\
							   virtual Event_Type Get_Event_Type() const override { return Get_Static_Type(); }\
							   virtual const char* Get_Name() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int Get_Category_Flags() const override { return category; }

	/*
		<Event>
		이벤트를 구현한 클래스. Event 클래스를 상속 받아 각 개별 이벤트들을 나타내는 클래스를 구현함.
	*/
	class Event
	{
		// EventDispatcher 클래스에서 접근 가능하도록 열어줌.
		friend class Event_Dispatcher;
	public:
		virtual Event_Type Get_Event_Type() const = 0;
		virtual const char* Get_Name() const = 0;
		virtual int Get_Category_Flags() const = 0;
		virtual std::string To_String() const { return Get_Name(); }

		// 이벤트가 특정 카테고리에 속하는지 체크하는 함수.
		inline bool Is_In_Category(Event_Category category)
		{
			return Get_Category_Flags() & category;
		}

	public:
		// 이벤트가 처리되었는지 알려주는 변수.
		// 상위 레이어에서 하위 레이어로 이벤트를 더 이상 전파하고 싶지 않다면, 이 값을 true로 만들어 하위 레이어에 이벤트가 도달하지 않도록 함.
		bool handled = false;

	};

	/*
		<Event_Dispatcher>
		각 이벤트 타입에 해당하는 이벤트를 콜백 함수에 전달하는 역할을 하는 클래스.
	*/
	class Event_Dispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>; // 리턴 타입이 bool, 인자가 템플릿 참조 타입인 함수 객체.
	public:
		Event_Dispatcher(Event& event)
			: _event(event)
		{
		}

		template<typename T>
		bool Dispatch(EventFn<T> event_callback_Func)
		{
			// m_Event의 타입과 T::GetStaticType()의 타입이 일치하는지 검사. 
			// 즉, EventDispatcher가 들고 있는 이벤트 타입과 Dispatch() 호출과 함께 넘겨준 템플릿 타입이 일치하는지 검사.
			if (_event.Get_Event_Type() == T::Get_Static_Type())
			{
				// m_Event 객체를 T 타입으로 캐스팅해서 callback 함수의 인자로 전달, callback 함수를 호출.
				// 리턴 타입으로 이벤트 처리 여부를 bool 타입으로 받아서 m_Event에 저장.
				_event.handled = event_callback_Func(*(T*)&_event);
				return true;
			}
			return false;
		}
	private:
		Event& _event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.To_String();
	}
}