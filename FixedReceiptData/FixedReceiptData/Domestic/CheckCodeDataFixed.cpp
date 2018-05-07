#include <iostream>
#include <vector>
#include <set>

#include "../Utils.h"
#include "CheckCodeDataFixed.h"

namespace Domestic
{
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
		std::vector<std::string> ResultCheckCodeVector;

		if (!ParseData(InValidatedDocument, ValidatedCheckCodeVector)
			|| !ParseData(InResultDocument, ResultCheckCodeVector)
			|| !CheckData(ValidatedCheckCodeVector[0])) {
			std::cout << "Data Error" << std::endl;
			return;
		}

		if (!Equal(ValidatedCheckCodeVector[0], ResultCheckCodeVector[0])) {
			ErrorCount++;
		}
		else {
			std::cout << "Validated Equal To Result" << std::endl;
			return;
		}
	

		std::string result_date = FixedData(ResultCheckCodeVector);

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
			for (auto date : ResultCheckCodeVector) {
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
				for (auto &data : region["result"].GetArray()) {
					if (strlen(data.GetString()) > 0)
						CheckCodeVector.push_back(data.GetString());
				}

				for (auto &data : region["ref_result"].GetArray()) {
					if (strlen(data.GetString()) > 0)
						CheckCodeVector.push_back(data.GetString());
				}

				has_checkcode = CheckCodeVector.size();
				break;
			}
			default:
				break;
			}
		}

		for (auto &checkCode : CheckCodeVector) {
			StringReplace(checkCode, " ", "");
		}

		return has_checkcode;
	}

	bool CheckCodeDataFixed::CheckData(const std::string &date) {
		if (date.length() != NumberCount) {
			return false;
		}

		static std::set<char> checkcodePatterns = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
		for (auto ch : date) {
			if (checkcodePatterns.count(ch) == 0) {
				return false;
			}
		}

		return true;
	}

	std::string CheckCodeDataFixed::FixedData(std::vector<std::string> &CheckDataVector) {
		std::sort(CheckDataVector.begin(), CheckDataVector.end(), [this](const std::string &first, const std::string &second) {
			return std::abs((int)first.length() - NumberCount) < std::abs((int)second.length() - NumberCount);
		});

		for (auto number : CheckDataVector) {
			if (CheckData(number)) {
				return number;
			}
		}

		return "";
	}
}