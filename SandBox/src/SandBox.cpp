
#pragma comment(lib, "Engine.lib")

#include "Engine.h"

class Sandbox : public Engine::Application // ���� �� Application Ŭ������ �����.
{
public:
	Sandbox()
	{

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
