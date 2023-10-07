
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

class Sandbox : public Engine::Application // ���� �� Application Ŭ������ �����.
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

// ���� �ʿ� ����� Application�� ��� �޾Ƽ� ������ Application�� �����ϵ��� �Լ��� ������.
// �� �Լ��� Engine ������Ʈ�� main �Լ����� ȣ���ϱ� ������ �������� Engine ������Ʈ ������ �Ű��ִ� ���� ��.
Engine::Application* Engine::Create_Application()
{
	return new Sandbox;
}
