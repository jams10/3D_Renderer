
#pragma comment(lib, "Engine.lib")

#include "Engine.h"

class Sandbox : public Engine::Application // 엔진 쪽 Application 클래스를 상속함.
{
public:
	Sandbox()
	{

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
