
#pragma once

#include "Platform/Win32/WindowsHeaders.h"
#include <iostream>

// �ڵ� ���� : https://onecoke.tistory.com/entry/C-%EC%BD%98%EC%86%94-%EA%B8%80%EC%9E%90-%EB%B0%8F-%EB%B0%B0%EA%B2%BD%EC%83%89-%EB%B3%80%EA%B2%BD%ED%95%98%EA%B8%B0

#ifdef _DEBUG
enum Color
{
	BLACK,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHTGRAY,
	DARKGRAY,
	LIGHTBLUE,
	LIGHTGREEN,
	LIGHTCYAN,
	LIGHTRED,
	LIGHTMAGENTA,
	YELLOW,
	WHITE
};
#endif

// ��� ���Ͽ��� �����ع����� SandBox ������Ʈ���� include�� ���, �Լ� �ߺ� ���ǰ� �ǹǷ� ���� �δ� ���� ���־���.
extern void Set_Text_Color(int foreground, int background);

#ifdef _DEBUG
#define LOG_ERROR(x) Set_Text_Color(Color::RED, Color::BLACK); std::cout << x << '\n'; Set_Text_Color(Color::WHITE, Color::BLACK); 
#define LOG_WARN(x) Set_Text_Color(Color::YELLOW, Color::BLACK);  std::cout << x << '\n'; Set_Text_Color(Color::WHITE, Color::BLACK); 
#define LOG_EVENT(x) Set_Text_Color(Color::LIGHTBLUE, Color::BLACK);  std::cout << x << '\n'; Set_Text_Color(Color::WHITE, Color::BLACK); 
#define LOG_LOG(x) Set_Text_Color(Color::WHITE, Color::BLACK); std::cout << x << '\n';
#else
#define LOG_ERROR(x) 
#define LOG_WARN(x) 
#define LOG_EVENT(x) 
#define LOG_LOG(x)
#endif

