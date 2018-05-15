#include <iostream>
#include <set>

#include "../Utils.h"
#include "AmountDataFixed.h"

namespace Domestic
{
	AmountDataFixed::AmountDataFixed(const std::string &InValidatedPath, const std::string &InResultPath)
		: DataFixed(InValidatedPath, InResultPath) {

	}

	AmountDataFixed::~AmountDataFixed() {

	}

	void AmountDataFixed::BeforeFixed() {
		std::cout << "Start Fixed Amount Data=================>" << std::endl;
		std::cout << std::endl;
	}

	void AmountDataFixed::FixedData(std::shared_ptr<rapidjson::Document> &InValidatedDocument,
		std::shared_ptr<rapidjson::Document> InResultDocument) {

		std::string validated_before_tax, validated_tax, validated_after_tax;
		std::string result_before_tax, result_tax, result_after_tax;

		if (!ParseData(InValidatedDocument, validated_before_tax, validated_tax, validated_after_tax)
			|| !ParseData(InResultDocument, result_before_tax, result_tax, result_after_tax)
			|| !CheckData(validated_before_tax)
			|| !CheckData(validated_tax)
			|| !CheckData(validated_after_tax)) {
			std::cout << " Data Error" << std::endl;
			return;
		}

		if (!Equal(validated_before_tax, result_before_tax) || !Equal(validated_tax, result_tax) || !Equal(validated_after_tax, result_after_tax)) {
			ErrorCount++;
		}
		else {
			std::cout << "Validated Equal To Result" << std::endl;
			return;
		}

		std::string origin_result_before_tax = result_before_tax;

		std::cout << "Validated Not Equal To Result" << std::endl;
		std::cout << result_before_tax << ", " << result_tax << ", " << result_after_tax << " Fixed To " << std::endl;

		FixedAmountData(result_before_tax, result_tax, result_after_tax);

		std::cout << result_before_tax << ", " << result_tax << ", " << result_after_tax << std::endl;

		if (Equal(validated_before_tax, result_before_tax) && Equal(validated_tax, result_tax) && Equal(validated_after_tax, result_after_tax)) {
			FixedCount++;
			std::cout << "Fixed Success!" << std::endl;
		}
		else {
			std::cout << "Validated " << validated_before_tax << ", " << validated_tax << ", " << validated_after_tax << std::endl;
			if (Equal(origin_result_before_tax, validated_before_tax) && !Equal(result_before_tax, validated_before_tax)) {
				FixedErrorCount++;
				std::cout << "Fixed origin before tax " << origin_result_before_tax << " to " << result_before_tax << std::endl;
			}
			std::cout << "Fixed Falied!" << std::endl;
		}
	}

	void AmountDataFixed::AfterFixed() {
		std::cout << "Error Count " << ErrorCount << ", Fixed Count " << FixedCount << ", Fixed Error Count " << FixedErrorCount << std::endl;

		std::cout << std::endl;
		std::cout << "<=================End Fixed Amount Data" << std::endl;
	}

	bool AmountDataFixed::ParseData(const std::shared_ptr<rapidjson::Document> &jsonDocument, std::string &before_tax, std::string &tax, std::string &after_tax) {
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

	bool AmountDataFixed::CheckData(const std::string &data)
	{
		size_t dotIndex = data.rfind('.');
		if (dotIndex != std::string::npos && (data.length() - data.rfind('.')) == 3)
			return true;

		return false;
	}

	bool AmountDataFixed::CheckNumber(const std::string &data) {
		if (data.length() == 0) return false;

		static std::set<char> numberPatterns = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' };
		for (auto ch : data) {
			if (numberPatterns.count(ch) == 0) {
				return false;
			}
		}

		if (std::count(data.begin(), data.end(), '.') > 1)
			return false;

		return true;
	}

	void AmountDataFixed::FixedAmountData(std::string &data) {
		if (data.length() == 0) return;

		size_t dotIndex = data.rfind('.');
		if (dotIndex != std::string::npos) {
			if ((data.length() - dotIndex) > 2) {
				data = data.substr(0, dotIndex + 3);
			}
			else if ((data.length() - dotIndex) == 2) {
				data.append("0");
			}
			else if ((data.length() - dotIndex) == 1) {
				data.append("00");
			}
		}
		else {
			data.append(".00");
		}
	}

	void AmountDataFixed::FixedAmountData(std::string &before_tax, std::string &tax, std::string &after_tax) {
		double d_before_tax = std::stod(before_tax), d_tax = std::stod(tax), d_after_tax = std::stod(after_tax);

		if (Equal(d_after_tax, d_before_tax + d_tax)) {
			if (!CheckData(before_tax)) {
				FixedAmountData(before_tax);
			}
			if (!CheckData(tax)) {
				FixedAmountData(tax);
			}
			if (!CheckData(after_tax)) {
				FixedAmountData(after_tax);
			}
			return;
		}

		if (CheckData(after_tax) && FixedDataByAfterTax(before_tax, tax, d_after_tax)) {
			return;
		}

		if (CheckData(tax) && FixedDataByTax(before_tax, after_tax, d_tax)) {
			return;
		}

		if (CheckData(before_tax) && FixedDataByBeforeTax(tax, after_tax, d_before_tax)) {
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
					float fixed_similarity = SimilarityRate(fixed, before_tax);
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
		if (CheckData(before_tax)) {
			bool valiedTax = false;
			double d_before_tax = std::stod(before_tax);
			for (auto rate : tax_rate) {
				if (Equal(Round(d_before_tax * rate), d_tax)) {
					valiedTax = true;
					break;
				}
			}

			if (valiedTax) {
				double fixed_after_tax = d_before_tax + d_tax;
				std::string fixed = DoubleToString(fixed_after_tax);
				if (SimilarityRate(fixed, after_tax) > 0.7) {
					after_tax = fixed;
					return true;
				}
			}
		}

		float similarity = 0.0f;
		std::string fixed_before_tax;
		std::string fixed_after_tax;
		for (auto rate : tax_rate) {
			for (double error = -0.09; error < 0.1; error += 0.01) {
				double temp_before_tax = (d_tax / rate) + error;
				std::string fixed = DoubleToString(temp_before_tax);
				float fixed_similarity = SimilarityRate(fixed, before_tax);
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
			std::string fixed = DoubleToString(d_before_tax + temp_tax);
			float fixed_similarity = SimilarityRate(fixed, after_tax);
			fixed_similarity += SimilarityRate(DoubleToString(temp_tax), tax);
			
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
				std::vector<std::set<char> > patterns = { { '8', '6', '0'},{ '1', '7' },{ '3', '8' },{'0', '4'} };
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
}