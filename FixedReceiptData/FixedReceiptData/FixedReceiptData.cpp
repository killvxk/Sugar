// FixedReceiptData.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <map>
#include <set>
#include "AmountDataFixed.h"
#include "DateDataFixed.h"
#include "NumberDataFixed.h"

int main()
{
	std::cout << "Start Program!=================>" << std::endl;
	std::cout << std::endl;

	std::string validatedPath = "D:\\1000\\validated\\";
	std::string resultPath = "D:\\1000\\result\\";

	//AmountDataFixed amountDataFixed(validatedPath, resultPath);
	//amountDataFixed.StartFixed();

	//DateDataFixed dateDataFixed(validatedPath, resultPath);
	//dateDataFixed.StartFixed();

	//NumberDataFixed numberDataFixed(validatedPath, resultPath);
	//numberDataFixed.StartFixed();
	
	std::cout << std::endl;
	std::cout << "<=================End Program!" << std::endl;

	return 0;
}

