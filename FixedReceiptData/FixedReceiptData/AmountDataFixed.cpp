#include <iostream>
#include <set>

#include "Utils.h"
#include "AmountDataFixed.h"

AmountDataFixed::AmountDataFixed(const std::string &validatedPath, const std::string &resultPath)
	: ValidatedPath(validatedPath)
	, ResultPath(resultPath) {

}

AmountDataFixed::~AmountDataFixed() {

}

void AmountDataFixed::StartFixed() {
	std::cout << "Start Fixed Amount Data=================>" << std::endl;
	std::cout << std::endl;

	int errorCount = 0;
	int fixedCount = 0;
	int fixedErrorCount = 0;
	auto vistor = [&](const std::string &filePath, const std::string &fileName) {
		std::shared_ptr<rapidjson::Document> validatedDocument = ReadFromFile(ValidatedPath + fileName);
		std::shared_ptr<rapidjson::Document> resultDocument = ReadFromFile(ResultPath + fileName);

		std::string validated_before_tax, validated_tax, validated_after_tax;
		std::string result_before_tax, result_tax, result_after_tax;

		if (!ParseTax(validatedDocument, validated_before_tax, validated_tax, validated_after_tax)
			|| !ParseTax(resultDocument, result_before_tax, result_tax, result_after_tax)
			|| !CheckAmountData(validated_before_tax)
			|| !CheckAmountData(validated_tax)
			|| !CheckAmountData(validated_after_tax)) {
			std::cout << fileName << " Data Error" << std::endl;
			std::cout << std::endl;
			return;
		}

		if (!Equal(validated_before_tax, result_before_tax) || !Equal(validated_tax, result_tax) || !Equal(validated_after_tax, result_after_tax)) {
			errorCount++;
		}
		else {
			return;
		}

		std::string origin_result_before_tax = result_before_tax;

		std::cout << fileName << " Validated Not Equal To Result" << std::endl;
		std::cout << result_before_tax << ", " << result_tax << ", " << result_after_tax << " Fixed To " << std::endl;

		FixedAmountData(result_before_tax, result_tax, result_after_tax);

		std::cout << result_before_tax << ", " << result_tax << ", " << result_after_tax << std::endl;

		if (Equal(validated_before_tax, result_before_tax) && Equal(validated_tax, result_tax) && Equal(validated_after_tax, result_after_tax)) {
			fixedCount++;
			std::cout << "Fixed Success!" << std::endl;
		}
		else {
			std::cout << "Validated " << validated_before_tax << ", " << validated_tax << ", " << validated_after_tax << std::endl;
			if (Equal(origin_result_before_tax, validated_before_tax) && !Equal(result_before_tax, validated_before_tax)) {
				fixedErrorCount++;
				std::cout << "Fixed origin before tax " << origin_result_before_tax << " to " << result_before_tax << std::endl;
			}
			std::cout << "Fixed Falied!" << std::endl;
		}

		std::cout << std::endl;
	};

	//vistor(ValidatedPath, "10101_562.jpg.json");
	VisitFolder(ValidatedPath, vistor);

	std::cout << "Error Count " << errorCount << ", Fixed Count " << fixedCount << ", Fixed Error Count " << fixedErrorCount << std::endl;

	std::cout << std::endl;
	std::cout << "<=================End Fixed Amount Data" << std::endl;
}

bool AmountDataFixed::ParseTax(const std::shared_ptr<rapidjson::Document> &jsonDocument, std::string &before_tax, std::string &tax, std::string &after_tax) {
	if (!jsonDocument || !jsonDocument->IsObject() || !jsonDocument->HasMember("regions"))
		return false;

	rapidjson::Value &regions = jsonDocument->operator[]("regions");

	bool has_before_tax = false, has_tax = false, has_after_tax = false;
	for (auto &region : regions.GetArray()) {
		if (!region.HasMember("cls")
			|| !region.HasMember("ref_result")
			|| !region.HasMember("result")) {
			continue;
		}

		switch (region["cls"].GetInt())
		{
		case 4: {
			before_tax = region["result"].GetArray()[0].GetString();
			has_before_tax = before_tax.size();
			break;
		}
		case 8: {
			tax = region["result"].GetArray()[0].GetString();
			has_tax = tax.size();
			break;
		}
		case 9: {
			after_tax = region["result"].GetArray()[0].GetString();
			has_after_tax = after_tax.size();
			break;
		}
		default:
			break;
		}
	}

	return has_before_tax && has_tax && has_after_tax;
}

bool AmountDataFixed::CheckAmountData(const std::string &data)
{
	size_t dotIndex = data.rfind('.');
	if (dotIndex != std::string::npos && (data.length() - data.rfind('.')) == 3)
		return true;

	return false;
}

void AmountDataFixed::FixedAmountData(std::string &data) {
	size_t dotIndex = data.rfind('.');
	if (dotIndex != std::string::npos && (data.length() - dotIndex) > 2) {
		data = data.substr(0, dotIndex + 3);
	}
}

void AmountDataFixed::FixedAmountData(std::string &before_tax, std::string &tax, std::string &after_tax) {
	double d_before_tax = std::stod(before_tax), d_tax = std::stod(tax), d_after_tax = std::stod(after_tax);

	if (Equal(d_after_tax, d_before_tax + d_tax)) {
		if (!CheckAmountData(before_tax)) {
			FixedAmountData(before_tax);
		}
		if (!CheckAmountData(tax)) {
			FixedAmountData(tax);
		}
		if (!CheckAmountData(after_tax)) {
			FixedAmountData(after_tax);
		}
		return;
	}

	if (CheckAmountData(after_tax) && FixedDataByAfterTax(before_tax, tax, d_after_tax)) {
		return;
	}

	if (CheckAmountData(tax) && FixedDataByTax(before_tax, after_tax, d_tax)) {
		return;
	}

	if (CheckAmountData(before_tax) && FixedDataByBeforeTax(tax, after_tax, d_before_tax)) {
		return;
	}
}

double tax_rate[] = { 0.03, 0.05, 0.06, 0.11, 0.17 };

bool AmountDataFixed::FixedDataByAfterTax(std::string &before_tax, std::string &tax, double d_after_tax) {
	float similarity = 0.0f;
	std::string fixed_before_tax;
	std::string fixed_tax;
	for (auto rate : tax_rate) {
		for (double error = -0.09; error < 0.1; error += 0.01) {
			double temp_before_tax = (d_after_tax / (1.0 + rate)) + error;
			double temp_tax = Round(temp_before_tax * rate);
			if (Equal((int(temp_before_tax * 100) % 100 + int(temp_tax * 100) % 100) % 100, int(d_after_tax * 100) % 100)) {
				std::string fixed = DoubleToString(temp_before_tax);
				float fixed_similarity = Similarity(fixed, before_tax);
				if (!(fixed_similarity < similarity)) {
					fixed_before_tax = fixed;
					fixed_tax = DoubleToString(temp_tax);
					if (tax == fixed_tax) {
						before_tax = fixed_before_tax;
						return true;
					}
					similarity = fixed_similarity;
				}
			}
		}
	}

	if (CheckBeforeTaxFixedResult(before_tax, fixed_before_tax, similarity)) {
		before_tax = fixed_before_tax;
		tax = fixed_tax;
		return true;
	}

	return false;
}

bool AmountDataFixed::FixedDataByTax(std::string &before_tax, std::string &after_tax, double d_tax) {
	if (CheckAmountData(before_tax)) {
		double d_before_tax = std::stod(before_tax);
		double fixed_after_tax = d_before_tax + d_tax;
		std::string fixed = DoubleToString(fixed_after_tax);
		if (Similarity(fixed, after_tax) > 0.7) {
			after_tax = fixed;
			return true;
		}
	}

	float similarity = 0.0f;
	std::string fixed_before_tax;
	std::string fixed_after_tax;
	for (auto rate : tax_rate) {
		for (double error = -0.09; error < 0.1; error += 0.01) {
			double temp_before_tax = (d_tax / rate) + error;
			std::string fixed = DoubleToString(temp_before_tax);
			float fixed_similarity = Similarity(fixed, before_tax);
			if (fixed_similarity > similarity) {
				fixed_before_tax = fixed;
				fixed_after_tax = DoubleToString(temp_before_tax + Round(temp_before_tax * rate));
				similarity = fixed_similarity;
			}
		}
	}

	if (CheckBeforeTaxFixedResult(before_tax, fixed_before_tax, similarity)) {
		before_tax = fixed_before_tax;
		after_tax = fixed_after_tax;
		return true;
	}

	return false;
}

bool AmountDataFixed::FixedDataByBeforeTax(std::string &tax, std::string &after_tax, double d_before_tax) {
	float similarity = 0.0f;
	double fixed_tax;
	for (auto rate : tax_rate) {
		double temp_tax = Round(d_before_tax * rate);
		std::string fixed = DoubleToString(d_before_tax + Round(d_before_tax * rate));
		float fixed_similarity = Similarity(fixed, after_tax);
		if (fixed_similarity > similarity) {
			fixed_tax = temp_tax;
			similarity = fixed_similarity;
		}
	}

	std::string fixed = DoubleToString(fixed_tax);
	tax = fixed;
	fixed = DoubleToString(d_before_tax + fixed_tax);
	after_tax = fixed;
	return true;
}

bool AmountDataFixed::CheckBeforeTaxFixedResult(const std::string &before_tax, const std::string &fixed_before_tax, double similarity) {
	size_t length = fixed_before_tax.length();
	if (length && similarity > 0.4999) {
		if (length == before_tax.length()) {
			std::vector<std::set<char> > patterns = { { '8', '6', '0', '5', '9', '4' },{ '1', '4', '7' },{ '3', '8' },{ '3', '7' } };
			for (int index = 0; index < length; ++index) {
				if (fixed_before_tax[index] != before_tax[index]) {
					bool match = false;
					for (auto pattern : patterns) {
						if (pattern.count(fixed_before_tax[index]) && pattern.count(before_tax[index])) {
							match = true;
							break;
						}
					}
					if (!match) {
						return false;
					}
				}
			}
		}
		else {
			if (similarity < 0.6 && (std::max(length, before_tax.length()) * similarity) > 3.0) {
				return false;
			}
		}

		return true;
	}

	return false;
}


std::string AmountDataFixed::DoubleToString(double invalue)
{
	std::string value = std::to_string(int(invalue * 100));
	if (value.length() >= 3) {
		value.insert(value.end() - 2, '.');
	}
	else {
		value = std::to_string(invalue);
		size_t dotIndex = value.find('.');
		if (dotIndex != std::string::npos) {
			return value.substr(0, dotIndex + 3);
		}
	}
	return value;
}

double AmountDataFixed::Round(double value)
{
	int ivalue = int(value * 1000);
	if (ivalue % 10 > 4)
	{
		return value + 0.01;
	}

	return value;
}