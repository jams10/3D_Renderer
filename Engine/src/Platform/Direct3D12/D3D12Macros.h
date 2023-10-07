#pragma once

// D3D API 호출이 성공 했음을 확인하기 위한 용도의 assert 매크로.
#ifdef _DEBUG
#ifndef DXCHECK
#define DXCHECK(x)								\
if(FAILED(x)) {									\
	char lineNumber[32];						\
	sprintf_s(lineNumber, "%u", __LINE__);	    \
	OutputDebugStringA("Error in: ");			\
	OutputDebugStringA(__FILE__);				\
	OutputDebugStringA("\nLine: ");				\
	OutputDebugStringA(lineNumber);				\
	OutputDebugStringA("\n");					\
	OutputDebugStringA(#x);						\
	OutputDebugStringA("\n");					\
	__debugbreak();                             \
}							
#endif // DXCHECK
#else
#ifndef DXCHECK
#define DXCHECK(x) x
#endif // DXCHECK
#endif // _DEBUG

#ifdef _DEBUG
// COM 인터페이스 객체를 우리가 식별할 수 있는 이름으로 바꾸고, 객체 생성 시 Visual Studio의 output 창에 출력함.
#define NAME_D3D12_OBJECT(obj, name) obj->SetName(name); OutputDebugString(L"::D3D12 Object Created : "); OutputDebugString(name); OutputDebugString(L"\n");
// COM 인터페이스 객체를 우리가 식별할 수 있는 이름으로 바꾸고, 객체 생성 시 Visual Studio의 output 창에 출력함.
#define NAME_D3D12_OBJECT_INDEXED(obj, idx, name)           \
{                                                           \
	wchar_t full_name[128];									\
	if (swprintf_s(full_name, L"%s[%u]", name, idx) > 0) {	\
		obj->SetName(full_name);							\
		OutputDebugString(L"::D3D12 Object Created : ");	\
		OutputDebugString(full_name); 						\
		OutputDebugString(L"\n");							\
	}                                                       \
}
#else
#define NAME_D3D12_OBJECT(x, name)
#define NAME_D3D12_OBJECT_INDEXED(x, idx, name)
#endif // _DEBUG