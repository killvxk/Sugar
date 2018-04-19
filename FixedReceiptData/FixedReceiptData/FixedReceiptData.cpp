// FixedReceiptData.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "AmountDataFixed.h"

int main()
{
	std::cout << "Start Program!=================>" << std::endl;
	std::cout << std::endl;

	std::string validatedPath = "D:\\1000\\validated\\";
	std::string resultPath = "D:\\1000\\result\\";

	AmountDataFixed amountDataFixed(validatedPath, resultPath);
	amountDataFixed.StartFixed();
	
	std::cout << std::endl;
	std::cout << "<=================End Program!" << std::endl;

	return 0;
}

