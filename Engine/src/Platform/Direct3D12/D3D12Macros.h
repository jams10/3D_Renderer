#pragma once

// D3D API ȣ���� ���� ������ Ȯ���ϱ� ���� �뵵�� assert ��ũ��.
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
// COM �������̽� ��ü�� �츮�� �ĺ��� �� �ִ� �̸����� �ٲٰ�, ��ü ���� �� Visual Studio�� output â�� �����.
#define NAME_D3D12_OBJECT(obj, name) obj->SetName(name); OutputDebugString(L"::D3D12 Object Created : "); OutputDebugString(name); OutputDebugString(L"\n");
// COM �������̽� ��ü�� �츮�� �ĺ��� �� �ִ� �̸����� �ٲٰ�, ��ü ���� �� Visual Studio�� output â�� �����.
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