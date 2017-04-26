#include "MNative.h"
#include "../Native/NNative.h"

MNative::MNative()
{
	fObject = new NNative();
}

MNative::~MNative()
{
	delete fObject;
}

bool MNative::helloWorld(System::IntPtr parameter)
{
	return fObject->helloWorld((Parameter *)parameter.ToPointer());
}
