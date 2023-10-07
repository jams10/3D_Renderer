#pragma once

// ���� ������, ���� ���� �����ڸ� �����ִ� ��ũ��.
#ifndef DISABLE_COPY
#define DISABLE_COPY(T)                    \
	explicit T(const T&) = delete;         \
	T& operator=(const T&) = delete;       
#endif

// �̵� ������, �̵� ���� �����ڸ� �����ִ� ��ũ��.
#ifndef DISABLE_MOVE
#define DISABLE_MOVE(T)                     \
	explicit T(const T&&) = delete;         \
	T& operator=(const T&&) = delete;       
#endif

#ifndef DISABLE_COPY_AND_MOVE
#define DISABLE_COPY_AND_MOVE(T) DISABLE_COPY(T) DISABLE_MOVE(T)
#endif