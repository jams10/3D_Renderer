
#pragma once

#include "Common/CommonHeaders.h"
#include "EventMacros.h"

namespace Engine
{
	// �̺�Ʈ Ÿ���� �����ϴ� enum class.
	// �� ���� �̺�Ʈ Ŭ������ �̸��� enum�� ���� ��� EVENT_CLASS_TYPE() ��ũ�θ� ���� ���� �̺�Ʈ Ŭ������ Ÿ���� �޾ƿ� �� ����.
	enum class Event_Type
	{
		None = 0,
		Window_Close, Window_Resize, Window_Focus, Window_Lost_Focus, Window_Moved,
		App_Update, App_Render,
		Key_Pressed, Key_Released,
		Mouse_Button_Pressed, Mouse_Button_Released, Mouse_Moved, Mouse_Scrolled, Mouse_WheelUp, Mouse_WheelDown
	};

	// �̺�Ʈ�� ���͸� �ϱ� ���� �̺�Ʈ ī�װ� enum. Ư�� �̺�Ʈ�� �������� ���� �� ���.
	// ���� ���� �� ���� ���͸� �� �� �ֵ��� bit ���� ���.(bitflag)
	enum Event_Category
	{
		None = 0,
		Event_Category_Application = BIT(0),
		Event_Category_Input = BIT(1),
		Event_Category_Keyboard = BIT(2),
		Event_Category_Mouse = BIT(3),
		Event_Category_MouseButton = BIT(4)
	};
	// Event Ŭ������ ����ϴ� ��ü���� �̺�Ʈ Ŭ�������� �Լ� �κ��� ���ϰ� �ۼ��� �� �ֵ��� ��ũ�� �ۼ�.
	// #  : �޾ƿ� ��ũ�� ������ ���ڿ��� �ٲ���.
	// ## : �� ���� ��ū�� �ٿ���.
	// �̺�Ʈ Ÿ�Ը� �������� ���� Ŭ���� �ν��Ͻ��� �ʿ� ���� ������ static �Լ��� �������.
	// ���� �Լ��� GetEventType() �Լ��� �̿�, �������� ���� Event Ŭ���� ���� �ϳ��� ��� �̺�Ʈ �ν��Ͻ��� �̺�Ʈ Ÿ���� ������ �� �ֵ��� ��.
#define EVENT_CLASS_TYPE(type) static Event_Type Get_Static_Type() { return Event_Type::##type; }\
							   virtual Event_Type Get_Event_Type() const override { return Get_Static_Type(); }\
							   virtual const char* Get_Name() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int Get_Category_Flags() const override { return category; }

	/*
		<Event>
		�̺�Ʈ�� ������ Ŭ����. Event Ŭ������ ��� �޾� �� ���� �̺�Ʈ���� ��Ÿ���� Ŭ������ ������.
	*/
	class Event
	{
		// EventDispatcher Ŭ�������� ���� �����ϵ��� ������.
		friend class Event_Dispatcher;
	public:
		virtual Event_Type Get_Event_Type() const = 0;
		virtual const char* Get_Name() const = 0;
		virtual int Get_Category_Flags() const = 0;
		virtual std::string To_String() const { return Get_Name(); }

		// �̺�Ʈ�� Ư�� ī�װ��� ���ϴ��� üũ�ϴ� �Լ�.
		inline bool Is_In_Category(Event_Category category)
		{
			return Get_Category_Flags() & category;
		}

	public:
		// �̺�Ʈ�� ó���Ǿ����� �˷��ִ� ����.
		// ���� ���̾�� ���� ���̾�� �̺�Ʈ�� �� �̻� �����ϰ� ���� �ʴٸ�, �� ���� true�� ����� ���� ���̾ �̺�Ʈ�� �������� �ʵ��� ��.
		bool handled = false;

	};

	/*
		<Event_Dispatcher>
		�� �̺�Ʈ Ÿ�Կ� �ش��ϴ� �̺�Ʈ�� �ݹ� �Լ��� �����ϴ� ������ �ϴ� Ŭ����.
	*/
	class Event_Dispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>; // ���� Ÿ���� bool, ���ڰ� ���ø� ���� Ÿ���� �Լ� ��ü.
	public:
		Event_Dispatcher(Event& event)
			: _event(event)
		{
		}

		template<typename T>
		bool Dispatch(EventFn<T> event_callback_Func)
		{
			// m_Event�� Ÿ�԰� T::GetStaticType()�� Ÿ���� ��ġ�ϴ��� �˻�. 
			// ��, EventDispatcher�� ��� �ִ� �̺�Ʈ Ÿ�԰� Dispatch() ȣ��� �Բ� �Ѱ��� ���ø� Ÿ���� ��ġ�ϴ��� �˻�.
			if (_event.Get_Event_Type() == T::Get_Static_Type())
			{
				// m_Event ��ü�� T Ÿ������ ĳ�����ؼ� callback �Լ��� ���ڷ� ����, callback �Լ��� ȣ��.
				// ���� Ÿ������ �̺�Ʈ ó�� ���θ� bool Ÿ������ �޾Ƽ� m_Event�� ����.
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