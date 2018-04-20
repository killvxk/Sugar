#include <iostream>
#include <vector>
#include <set>

#include "NumberDataFixed.h"
#include "Utils.h"

NumberDataFixed::NumberDataFixed(const std::string &InValidatedPath, const std::string &InResultPath)
	: DataFixed(InValidatedPath, InResultPath) {
}

NumberDataFixed::~NumberDataFixed()
{
}

void NumberDataFixed::BeforeFixed() {
	std::cout << "Start Fixed Number Data=================>" << std::endl;
	std::cout << std::endl;
}

void NumberDataFixed::FixedData(std::shared_ptr<rapidjson::Document> &InValidatedDocument,
	std::shared_ptr<rapidjson::Document> InResultDocument) {
	std::vector<std::string> ValidatedNumberFirstVector;
	std::vector<std::string> ValidatedNumberSecondVector;
	std::vector<std::string> ResultNumberFirstVector;
	std::vector<std::string> ResultNumberSecondVector;

	if (!ParseData(InValidatedDocument, ValidatedNumberFirstVector, ValidatedNumberSecondVector)
		|| !ParseData(InResultDocument, ResultNumberFirstVector, ResultNumberSecondVector)
		|| !CheckNumber(ValidatedNumberFirstVector[0])) {
		std::cout << "Data Error" << std::endl;
		return;
	}

	if (!Equal(ResultNumberFirstVector[0], ValidatedNumberFirstVector[0])) {
		ErrorCount++;
	}
	else {
		std::cout << "Validated Equal To Result" << std::endl;
		return;
	}

	std::string origin_result_number = ResultNumberFirstVector[0];

	std::cout << " Validated Not Equal To Result" << std::endl;
	std::cout << ResultNumberFirstVector[0] << " Fixed To " << std::endl;

	std::string result_date = this->FixedNumber(ResultNumberFirstVector, ResultNumberSecondVector);

	std::cout << result_date << std::endl;

	if (Equal(ValidatedNumberFirstVector[0], result_date)) {
		FixedCount++;
		std::cout << "Fixed Success!" << std::endl;
	}
	else {
		std::cout << "Validated " << ValidatedNumberFirstVector[0] << std::endl;
		if (Equal(ValidatedNumberFirstVector[0], origin_result_number)) {
			FixedErrorCount++;
			std::cout << "Fixed origin before tax " << origin_result_number << " to " << result_date << std::endl;
		}

		std::cout << "First Result List" << std::endl;
		for (auto date : ResultNumberFirstVector) {
			std::cout << date << std::endl;
		}
		std::cout << "Second Result List" << std::endl;
		for (auto date : ResultNumberSecondVector) {
			std::cout << date << std::endl;
		}

		std::cout << "Fixed Falied!" << std::endl;
	}
}

void NumberDataFixed::AfterFixed() {
	std::cout << "Error Count " << ErrorCount << ", Fixed Count " << FixedCount << ", Fixed Error Count " << FixedErrorCount << std::endl;

	std::cout << std::endl;
	std::cout << "<=================End Fixed Amount Data" << std::endl;
}

bool NumberDataFixed::ParseData(const std::shared_ptr<rapidjson::Document> &InJsonDocument, std::vector<std::string> &NumberFirstVector, std::vector<std::string> &NumberSecondVector) {
	if (!InJsonDocument || !InJsonDocument->IsObject() || !InJsonDocument->HasMember("regions"))
		return false;

	rapidjson::Value &regions = InJsonDocument->operator[]("regions");

	bool has_first_number = false, has_second_number = false;
	for (auto &region : regions.GetArray()) {
		if (!region.HasMember("cls")
			|| !region.HasMember("ref_result")
			|| !region.HasMember("result")) {
			continue;
		}

		switch (region["cls"].GetInt())
		{
		case 2: {
			for (auto &date : region["result"].GetArray()) {
				if (strlen(date.GetString()) > 0)
					NumberFirstVector.push_back(date.GetString());
			}

			for (auto &date : region["ref_result"].GetArray()) {
				if (strlen(date.GetString()) > 0)
					NumberFirstVector.push_back(date.GetString());
			}

			has_first_number = NumberFirstVector.size();
			break;
		}
		case 7: {
			for (auto &date : region["result"].GetArray()) {
				if (strlen(date.GetString()) > 0)
					NumberSecondVector.push_back(date.GetString());
			}

			for (auto &date : region["ref_result"].GetArray()) {
				if (strlen(date.GetString()) > 0)
					NumberSecondVector.push_back(date.GetString());
			}

			has_second_number = NumberSecondVector.size();
			break;
		}
		default:
			break;
		}
	}

	return has_first_number && has_second_number;
}

bool NumberDataFixed::CheckNumber(const std::string &date) {
	if (date.length() != NumberCount) {
		return false;
	}

	static std::set<char> numberPatterns = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', };
	for (auto ch : date) {
		if (numberPatterns.count(ch) == 0) {
			return false;
		}
	}

	return true;
}

std::string NumberDataFixed::FixedNumber(std::vector<std::string> &NumberFirstVector, std::vector<std::string> &NumberSecondVector) {
	std::sort(NumberSecondVector.begin(), NumberSecondVector.end(), [this](const std::string &first, const std::string &second) {
		return std::abs((int)first.length() - NumberCount) < std::abs((int)second.length() - NumberCount);
	});

	if (CheckNumber(NumberSecondVector[0])) {
		std::sort(NumberFirstVector.begin(), NumberFirstVector.end(), [this, &NumberSecondVector](const std::string &first, const std::string &second) {
			//if (first.length() == second.length()) {
			//	return Similarity(first.substr(0, 2), NumberSecondVector[0].substr(0, 2)) > Similarity(second.substr(0, 2), NumberSecondVector[0].substr(0, 2));
			//}

			return std::abs((int)first.length() - NumberCount) < std::abs((int)second.length() - NumberCount);;
		});
	}
	else {
		std::sort(NumberFirstVector.begin(), NumberFirstVector.end(), [this](const std::string &first, const std::string &second) {
			return std::abs((int)first.length() - NumberCount) < std::abs((int)second.length() - NumberCount);
		});
	}

	if (NumberFirstVector[0] == NumberSecondVector[0] && CheckNumber(NumberFirstVector[0])) {
		return NumberFirstVector[0];
	}

	for (auto number : NumberFirstVector) {
		if (CheckNumber(number)) {
			return number;
		}
	}

	for (auto number : NumberSecondVector) {
		if (CheckNumber(number)) {
			return number;
		}
	}

	return "";
}