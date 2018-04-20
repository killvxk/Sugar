// FixedReceiptData.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <map>
#include "AmountDataFixed.h"
#include "DateDataFixed.h"

#include "Utils.h"

#define LAST_YEAR 2018 // fetch lasy year

int main()
{
	std::cout << "Start Program!=================>" << std::endl;
	std::cout << std::endl;

	std::string validatedPath = "D:\\1000\\validated\\";
	std::string resultPath = "D:\\1000\\result\\";

	//AmountDataFixed amountDataFixed(validatedPath, resultPath);
	//amountDataFixed.StartFixed();

	DateDataFixed dateDataFixed(validatedPath, resultPath);
	dateDataFixed.StartFixed();
	
	std::cout << std::endl;
	std::cout << "<=================End Program!" << std::endl;

	return 0;
}

