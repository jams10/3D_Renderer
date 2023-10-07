#pragma once

#include <assert.h>
#include <string>
#include <sstream>
#include <functional>
#include <memory>

#include "CommonMacros.h"
#include "Types.h"

// COM �������̽� ��ü�� Release �ϴ� �Լ�.
template<typename T>
constexpr void Release(T*& resource)
{
	if (resource)
	{
		resource->Release();
		resource = nullptr;
	}
}