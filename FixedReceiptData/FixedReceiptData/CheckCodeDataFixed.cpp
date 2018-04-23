#include <iostream>
#include <vector>
#include <set>

#include "Utils.h"
#include "CheckCodeDataFixed.h"


CheckCodeDataFixed::CheckCodeDataFixed(const std::string &InValidatedPath, const std::string &InResultPath)
	: DataFixed(InValidatedPath, InResultPath) {

}

CheckCodeDataFixed::~CheckCodeDataFixed() {

}

void CheckCodeDataFixed::BeforeFixed() {
	std::cout << "Start Fixed Amount Data=================>" << std::endl;
	std::cout << std::endl;
}

void CheckCodeDataFixed::FixedData(std::shared_ptr<rapidjson::Document> &InValidatedDocument,
	std::shared_ptr<rapidjson::Document> InResultDocument) {
	std::vector<std::string> ValidatedCheckCodeVector;
	std::vector<std::string> ResultCheckCodeFirstVector;

	if (!ParseData(InValidatedDocument, ValidatedCheckCodeVector)
		|| !ParseData(InResultDocument, ResultCheckCodeFirstVector)
		|| !CheckData(ValidatedCheckCodeVector[0])) {
		std::cout << "Data Error" << std::endl;
		if (ValidatedCheckCodeVector.size() && ValidatedCheckCodeVector[0].size() != 0) {
			std::cout << "Data Error" << std::endl;
		}
		return;
	}
	else {
		std::cout << "Validated Equal To Result" << std::endl;
		return;
	}

	std::string result_date;

	if (Equal(ValidatedCheckCodeVector[0], result_date)) {
		FixedCount++;
		std::cout << "Fixed Success!" << std::endl;
	}
	else {
		std::cout << "Validated " << ValidatedCheckCodeVector[0] << std::endl;
		//if (Equal(ValidatedCheckCodeVector[0], origin_result_number)) {
		//	FixedErrorCount++;
		//	std::cout << "Fixed origin before tax " << origin_result_number << " to " << result_date << std::endl;
		//}

		std::cout << "First Result List" << std::endl;
		for (auto date : ResultCheckCodeFirstVector) {
			std::cout << date << std::endl;
		}

		std::cout << "Fixed Falied!" << std::endl;
	}
}

void CheckCodeDataFixed::AfterFixed() {
	std::cout << "Error Count " << ErrorCount << ", Fixed Count " << FixedCount << ", Fixed Error Count " << FixedErrorCount << std::endl;

	std::cout << std::endl;
	std::cout << "<=================End Fixed Amount Data" << std::endl;
}

bool CheckCodeDataFixed::ParseData(const std::shared_ptr<rapidjson::Document> &InJsonDocument, std::vector<std::string> &CheckCodeVector) {
	if (!InJsonDocument || !InJsonDocument->IsObject() || !InJsonDocument->HasMember("regions"))
		return false;

	rapidjson::Value &regions = InJsonDocument->operator[]("regions");

	bool has_checkcode = false;
	for (auto &region : regions.GetArray()) {
		if (!region.HasMember("cls")
			|| !region.HasMember("ref_result")
			|| !region.HasMember("result")) {
			continue;
		}

		switch (region["cls"].GetInt())
		{
		case 5: {
			for (auto &date : region["result"].GetArray()) {
				if (strlen(date.GetString()) > 0)
					CheckCodeVector.push_back(date.GetString());
			}

			for (auto &date : region["ref_result"].GetArray()) {
				if (strlen(date.GetString()) > 0)
					CheckCodeVector.push_back(date.GetString());
			}

			has_checkcode = CheckCodeVector.size();
			break;
		}
		default:
			break;
		}
	}

	return has_checkcode;
}

bool CheckCodeDataFixed::CheckData(const std::string &date) {
	//if (date.length() != NumberCount) {
	//	return false;
	//}

	static std::set<char> checkcodePatterns = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' ' };
	for (auto ch : date) {
		if (checkcodePatterns.count(ch) == 0) {
			return false;
		}
	}

	return true;
}