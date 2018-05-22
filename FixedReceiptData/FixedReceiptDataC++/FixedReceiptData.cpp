// FixedReceiptData.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <map>
#include <set>
#include "Domestic/AmountDataFixed.h"
#include "Domestic/DateDataFixed.h"
#include "Domestic/NumberDataFixed.h"
#include "Domestic/CheckCodeDataFixed.h"
#include "International/AmountDataFixed.h"
#include "Double.h"

int main()
{
	Double a(".00"), b("0.5");
	std::string c = (a * b).ToString();
	std::cout << "Start Program!=================>" << std::endl;
	std::cout << std::endl;

	std::string validatedPath = "C:\\Users\\User\\Desktop\\receipt\\International\\validated\\";
	std::string resultPath = "C:\\Users\\User\\Desktop\\receipt\\International\\result\\";

	//Domestic::AmountDataFixed dataFixed(validatedPath, resultPath);
	//Domestic::DateDataFixed dataFixed(validatedPath, resultPath);
	//Domestic::NumberDataFixed dataFixed(validatedPath, resultPath);
	//Domestic::CheckCodeDataFixed dataFixed(validatedPath, resultPath);
	International::AmountDataMildFixed dataFixed(validatedPath, resultPath);

	dataFixed.StartFixed();

	std::cout << std::endl;
	std::cout << "<=================End Program!" << std::endl;

	return 0;
}

