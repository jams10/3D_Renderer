#pragma once

extern Engine::Application* Engine::CreateApplication();

/*
* ���α׷��� ������.
* SandBox ������Ʈ���� CreateApplication() �Լ��� ���� �߱� ������ �������� �������� �Űܿ� �� ����.
*/
int main(int argc, char** argv)
{
	std::cout << "Started Engine...\n";
	auto app = Engine::CreateApplication();
	app->Run();
	delete app;
}