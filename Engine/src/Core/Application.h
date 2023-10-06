#pragma once

namespace Engine
{
	/*
	* GameLoop를 수행하는 클래스. SandBox 프로젝트 쪽에서 이 클래스를 상속 받아 Loop에서 실행 시킬 로직을 구현함.
	*/
	class Application
	{
	public:
		Application();
		virtual ~Application(); // SandBox 프로젝트의 Application에서 이 함수를 상속 받기 떄문에 기본 클래스인 이 클래스의 소멸자를 가상 함수로 선언해줌.

		void Run();
	};

	// SandBox 프로젝트에서 Application 클래스를 상속 받고 이 함수를 구현해줌.
	Application* CreateApplication();
}