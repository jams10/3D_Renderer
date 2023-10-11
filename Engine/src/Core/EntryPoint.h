#pragma once

#define _CRTDBG_MAP_ALLOC 
#include <crtdbg.h>
#include <stdlib.h>

extern Engine::Application* Engine::Create_Application();

/*
* ���α׷��� ������.
* SandBox ������Ʈ���� CreateApplication() �Լ��� ���� �߱� ������ �������� �������� �Űܿ� �� ����.
*/
int main(int argc, char** argv)
{
#if _DEBUG
	// �޸� ���� üũ.
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