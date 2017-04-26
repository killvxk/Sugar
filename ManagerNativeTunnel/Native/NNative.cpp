#include "NNative.h"
#include <iostream>

NNative::NNative() {

}

NNative::~NNative() {

}

bool NNative::helloWorld(Parameter *parameter) {
	std::cout << "hello world" << std::endl;
    parameter->callback(L"OK");
	return true;
}