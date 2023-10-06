
#pragma once

#include "Platform/Win32/WindowsHeaders.h"
#include <iostream>

// 코드 참고 : https://onecoke.tistory.com/entry/C-%EC%BD%98%EC%86%94-%EA%B8%80%EC%9E%90-%EB%B0%8F-%EB%B0%B0%EA%B2%BD%EC%83%89-%EB%B3%80%EA%B2%BD%ED%95%98%EA%B8%B0

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

// 헤더 파일에서 구현해버리면 SandBox 프로젝트에서 include할 경우, 함수 중복 정의가 되므로 구현 부는 따로 빼주었음.
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

