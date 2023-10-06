#pragma once

extern Engine::Application* Engine::Create_Application();

/*
* ���α׷��� ������.
* SandBox ������Ʈ���� CreateApplication() �Լ��� ���� �߱� ������ �������� �������� �Űܿ� �� ����.
*/
int main(int argc, char** argv)
{
	std::cout << "Started Engine...\n";
	auto app = Engine::Create_Application();
	app->Run();
	delete app;
}