#pragma once

#define _CRTDBG_MAP_ALLOC 
#include <crtdbg.h>
#include <stdlib.h>

extern Engine::Application* Engine::Create_Application();

/*
* 프로그램의 진입점.
* SandBox 프로젝트에서 CreateApplication() 함수를 구현 했기 때문에 진입점을 이쪽으로 옮겨올 수 있음.
*/
int main(int argc, char** argv)
{
#if _DEBUG
	// 메모리 누수 체크.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	{
		std::cout << "Started Engine...\n";
		auto app = Engine::Create_Application();
		app->Run();
		delete app;
	}

	_CrtDumpMemoryLeaks();
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

	return 0;
}