#pragma once
#include <Windows.h>

#ifdef NATIVE_IMPLEMENTATION
#define NATIVEAPI __declspec(dllexport)
#else
#define NATIVEAPI __declspec(dllimport)
#endif