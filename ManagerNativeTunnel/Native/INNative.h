#pragma once

#include "NConfig.h"

typedef bool(__stdcall* NativeCallback) (const wchar_t* parameter);

struct Parameter
{
    wchar_t parameter[512];
    NativeCallback callback;
};

class NATIVEAPI INNative
{
public:
	virtual bool helloWorld(Parameter *parameter) = 0;
};