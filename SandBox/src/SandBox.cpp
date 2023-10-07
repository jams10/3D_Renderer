
#pragma comment(lib, "Engine.lib")

#include "Engine.h"

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer() {}

	virtual void On_Update(float dt) override
	{
		//std::cout << "Update\n";
	}

	virtual void On_Event(Engine::Event& event) override
	{
		std::cout << event.To_String() << '\n';
	}
};

class Sandbox : public Engine::Application // 엔진 쪽 Application 클래스를 상속함.
{
public:
	Sandbox()
	{
		Push_Layer(new ExampleLayer());
	}
	~Sandbox()
	{

	}
};

// 엔진 쪽에 선언된 Application을 상속 받아서 실제로 Application을 생성하도록 함수를 구현함.
// 이 함수는 Engine 프로젝트의 main 함수에서 호출하기 때문에 진입점을 Engine 프로젝트 쪽으로 옮겨주는 셈이 됨.
Engine::Application* Engine::Create_Application()
{
	return new Sandbox;
}
