#pragma once

#include <assert.h>
#include <string>
#include <sstream>
#include <functional>
#include <memory>

#include "CommonMacros.h"
#include "Types.h"

// COM 인터페이스 객체를 Release 하는 함수.
template<typename T>
constexpr void Release(T*& resource)
{
	if (resource)
	{
		resource->Release();
		resource = nullptr;
	}
}