#pragma once

extern Engine::Application* Engine::CreateApplication();

/*
* 프로그램의 진입점.
* SandBox 프로젝트에서 CreateApplication() 함수를 구현 했기 때문에 진입점을 이쪽으로 옮겨올 수 있음.
*/
int main(int argc, char** argv)
{
	std::cout << "Started Engine...\n";
	auto app = Engine::CreateApplication();
	app->Run();
	delete app;
}