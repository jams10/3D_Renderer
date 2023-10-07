#pragma once

// 복사 생성자, 복사 대입 연산자를 막아주는 매크로.
#ifndef DISABLE_COPY
#define DISABLE_COPY(T)                    \
	explicit T(const T&) = delete;         \
	T& operator=(const T&) = delete;       
#endif

// 이동 생성자, 이동 대입 연산자를 막아주는 매크로.
#ifndef DISABLE_MOVE
#define DISABLE_MOVE(T)                     \
	explicit T(const T&&) = delete;         \
	T& operator=(const T&&) = delete;       
#endif

#ifndef DISABLE_COPY_AND_MOVE
#define DISABLE_COPY_AND_MOVE(T) DISABLE_COPY(T) DISABLE_MOVE(T)
#endif