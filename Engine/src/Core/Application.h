#pragma once

namespace Engine
{
	/*
	* GameLoop�� �����ϴ� Ŭ����. SandBox ������Ʈ �ʿ��� �� Ŭ������ ��� �޾� Loop���� ���� ��ų ������ ������.
	*/
	class Application
	{
	public:
		Application();
		virtual ~Application(); // SandBox ������Ʈ�� Application���� �� �Լ��� ��� �ޱ� ������ �⺻ Ŭ������ �� Ŭ������ �Ҹ��ڸ� ���� �Լ��� ��������.

		void Run();
	};

	// SandBox ������Ʈ���� Application Ŭ������ ��� �ް� �� �Լ��� ��������.
	Application* CreateApplication();
}