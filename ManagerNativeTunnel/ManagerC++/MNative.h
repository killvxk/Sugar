#pragma once
using namespace Interface;

#include "../Native/INNative.h"

public ref class MNative : IManager
{
public:
	MNative();
	~MNative();

	virtual bool helloWorld();

private:
	INNative *fObject;
};

