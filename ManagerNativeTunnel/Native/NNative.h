#pragma once
#include "INNative.h"

class NATIVEAPI NNative : public INNative
{
public:
	NNative();
	~NNative();

	bool helloWorld(Parameter *parameter) override;
};

