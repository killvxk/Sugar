#include <iostream>
#include <map>

#include "../Utils.h"
#include "DateDataFixed.h"

namespace Domestic
{
	DateDataFixed::DateDataFixed(const std::string &InValidatedPath, const std::string &InResultPath)
		: DataFixed(InValidatedPath, InResultPath) {
	}

	DateDataFixed::~DateDataFixed()
	{
	}

	void DateDataFixed::BeforeFixed() {
		std::cout << "Start Fixed Date Data=================>" << std::endl;
		std::cout << std::endl;
	}

	void DateDataFixed::FixedData(std::shared_ptr<rapidjson::Document> &InValidatedDocument,
		std::shared_ptr<rapidjson::Document> InResultDocument) {
		std::vector<std::string> ValidatedDateVector;
		std::vector<std::string> ResultDateVector;

		if (!ParseData(InValidatedDocument, ValidatedDateVector)
			|| !ParseData(InResultDocument, ResultDateVector)
			|| !CheckData(ValidatedDateVector[0])) {
			std::cout << "Validated Data Error" << std::endl;
			return;
		}

		if (!Equal(ResultDateVector[0], ValidatedDateVector[0])) {

		}
		else {
			std::cout << "Validated Equal To Result" << std::endl;
			return;
		}

		ErrorCount++;
		std::string origin_result_date = ResultDateVector[0];

		std::cout << "Validated Not Equal To Result" << std::endl;
		std::cout << ResultDateVector[0] << " Fixed To " << std::endl;

		std::string result_date;
		for (auto date : ResultDateVector) {
			result_date = date;
			if (FixedDate(result_date, ResultDateVector)) {
				break;
			}
		}

		std::cout << result_date << std::endl;

		if (Equal(ValidatedDateVector[0], result_date)) {
			FixedCount++;
			std::cout << "Fixed Success!" << std::endl;
		}
		else {
			std::cout << "Validated " << ValidatedDateVector[0] << std::endl;
			if (Equal(ValidatedDateVector[0], origin_result_date)) {
				FixedErrorCount++;
				std::cout << "Fixed origin before tax " << origin_result_date << " to " << result_date << std::endl;
			}

			//std::cout << "Result List" << std::endl;
			//for (auto date : ResultDateVector) {
			//	std::cout << date << std::endl;
			//}

			std::cout << "Fixed Falied!" << std::endl;
		}
	}

	void DateDataFixed::AfterFixed() {
		std::cout << "Total Count " << ErrorCount << ", Fixed Count " << FixedCount << ", Fixed Error Count " << FixedErrorCount << std::endl;

		std::cout << std::endl;
		std::cout << "<=================End Fixed Date Data" << std::endl;
	}

	bool DateDataFixed::ParseData(const std::shared_ptr<rapidjson::Document> &jsonDocument, std::vector<std::string> &FVectorDate) {
		if (!jsonDocument || !jsonDocument->IsObject() || !jsonDocument->HasMember("regions"))
			return false;

		rapidjson::Value &regions = jsonDocument->operator[]("regions");

		for (auto &region : regions.GetArray()) {
			if (!region.HasMember("cls")
				|| !region.HasMember("ref_result")
				|| !region.HasMember("result")) {
				continue;
			}

			switch (region["cls"].GetInt())
			{
			case 3: {
				for (auto &date : region["result"].GetArray()) {
					if (strlen(date.GetString()) > 0)
						FVectorDate.push_back(UTF8_To_string(date.GetString()));
				}

				for (auto &date : region["ref_result"].GetArray()) {
					if (strlen(date.GetString()) > 0)
						FVectorDate.push_back(UTF8_To_string(date.GetString()));
				}

				break;
			}
			default:
				break;
			}
		}

		return FVectorDate.size();
	}

#define LAST_YEAR 2018 // fetch lasy year
	std::vector<std::string> patterns = { "年", "月", "日" };

	bool DateDataFixed::CheckData(const std::string &date) {
		size_t indexOfYear = date.find(patterns[0]);
		if (indexOfYear == std::string::npos || indexOfYear != 4) {
			return false;
		}

		int year = std::atoi(date.substr(0, indexOfYear).c_str());
		if (year < 2000 || year > LAST_YEAR) {
			return false;
		}

		size_t indexOfMonth = date.find(patterns[1], indexOfYear + 1);
		if (indexOfMonth == std::string::npos || indexOfMonth != 8) {
			return false;
		}

		int month = std::atoi(date.substr(indexOfYear + patterns[0].length(), indexOfMonth - indexOfYear - patterns[0].length()).c_str());
		if (month < 1 || month > 12) {
			return false;
		}

		size_t indexOfDay = date.find(patterns[2], indexOfMonth + 1);
		if (indexOfDay == std::string::npos || indexOfDay != 12) {
			return false;
		}

		int day = std::atoi(date.substr(indexOfMonth + patterns[1].length(), indexOfDay - indexOfMonth - patterns[1].length()).c_str());
		if (day < 1 || day > 31) {
			return false;
		}

		return true;
	}

	bool DateDataFixed::FixedDate(std::string &date, std::vector<std::string> &DateVector) {
		size_t indexOfYear = date.find(patterns[0]);
		if (indexOfYear == std::string::npos) {
			std::vector<std::string> errorPatters = { { "0." } };
			for (auto error : errorPatters) {
				size_t indexOfError = date.find(error);
				if (indexOfError != std::string::npos) {
					date.replace(indexOfError, error.length(), "年");
					indexOfYear = date.find(patterns[0]);
					break;
				}
			}
		}

		indexOfYear = date.find(patterns[0]);
		if (indexOfYear == std::string::npos) {
			if (date.length() > 4) {
				date.insert(4, patterns[0]);
				indexOfYear = 4;
			}
			else {
				return false;
			}
		}
		else if (indexOfYear > 4) {
			while (date[0] != '2') {
				date = date.substr(1);
				indexOfYear;;
			}
			if (indexOfYear != 4) {
				return false;
			}

		}

		{
			std::map<std::string, int> yearMap;
			for (auto date : DateVector) {
				std::string year;
				if (CheckData(date)) {
					size_t indexOfYear = date.find(patterns[0]);
					if (indexOfYear == std::string::npos || indexOfYear != 4) {
						return 0;
					}

					year = date.substr(0, indexOfYear);
				}
				else if (date.length() > 4) {
					year = date.substr(0, 4);
				}
				else {
					continue;
				}

				if (yearMap.count(year)) {
					yearMap[year]++;
				}
				else {
					yearMap[year] = 1;
				}
			}

			std::vector<std::pair<std::string, int>> yearVector;
			for (auto year : yearMap) {
				yearVector.push_back(year);
			}
			std::sort(yearVector.begin(), yearVector.end(), [](const std::pair<std::string, int> &first,
				const std::pair<std::string, int> &second) -> bool {
				int iFirstYear = atoi(first.first.c_str());
				int iSecondYear = atoi(second.first.c_str());
				return first.second > second.second || (first.second == second.second && iFirstYear > iSecondYear);
			});

			for (auto year : yearVector) {
				int iYear = atoi(year.first.c_str());
				if (iYear < 2000 || iYear > LAST_YEAR) {
					continue;
				}

				date.replace(0, indexOfYear, year.first);
				break;
			}
		}

		size_t indexOfMonth = date.find(patterns[1], indexOfYear + 1);
		if (indexOfMonth != std::string::npos) {
			std::string month = date.substr(indexOfYear + patterns[0].length(), indexOfMonth - indexOfYear - patterns[0].length());
			std::map<std::string, std::string> monthMappings = { { "13", "10" } };
			if (monthMappings.count(month)) {
				date.replace(indexOfYear + patterns[0].length(), month.length(), monthMappings[month]);
			}

			if (month.length() == 2)
			{
				int iMonth = atoi(month.c_str());
				if (iMonth < 1) {
					return false;
				}
				if (iMonth > 12 && iMonth < 20) {
					if (month[1] == '7') {
						month[1] = '1';
					}

					date.replace(indexOfYear + patterns[0].length(), indexOfMonth - indexOfYear - patterns[0].length(), month);
				}
			}
			else if (month.length() == 1) {
				month = '0' + month;
				date.replace(indexOfYear + patterns[0].length(), indexOfMonth - indexOfYear - patterns[0].length(), month);
				indexOfMonth += 1;
			}
		}
		else if ((date.length() - indexOfYear - patterns[0].length()) > 2) {
			switch (date[indexOfYear + patterns[0].length() + 2]) {
			case '4':
			case '8': {
				date.replace(indexOfYear + patterns[0].length() + 2, 1, patterns[1]);
				indexOfMonth = date.find(patterns[1], indexOfYear + 1);
				break;
			}
			default: {
				date.insert(date.length() - indexOfYear - patterns[0].length() - 1, patterns[1]);
				indexOfMonth = date.find(patterns[1], indexOfYear + 1);
			}
			}
		}

		size_t indexOfDay = date.find(patterns[2], indexOfMonth + 1);
		if (indexOfDay == std::string::npos) {
			date.append(patterns[2]);
			indexOfDay = date.find(patterns[2], indexOfMonth + 1);
		}

		std::string day = date.substr(indexOfMonth + patterns[1].length(), indexOfDay - indexOfMonth - patterns[1].length());

		{
			bool needReplace = false;
			std::map<std::string, std::string> dayMappings = { { "月", "4" },{ ".", "5" } };
			for (auto dayMapping : dayMappings) {
				size_t indexOfError = day.find(dayMapping.first);
				if (indexOfError != std::string::npos) {
					day.replace(indexOfError, dayMapping.first.length(), dayMapping.second);
					needReplace = true;
				}
			}
			if (needReplace) {
				date.replace(indexOfMonth + patterns[1].length(), indexOfDay - indexOfMonth - patterns[1].length(), day);
			}
		}

		if (day.length() == 2)
		{
			std::map<std::string, std::string> dayMappings = { { "6", "0" } };
			int iDay = atoi(day.c_str());
			if (iDay > 31 && iDay < 40) {
				if (day[1] == '6') {
					day[1] = '0';
				}

				date.replace(indexOfMonth + patterns[1].length(), indexOfDay - indexOfMonth - patterns[1].length(), day);
			}
		}
		else if (day.length() == 1) {
			day = '0' + day;
			date.replace(indexOfMonth + patterns[1].length(), indexOfDay - indexOfMonth - patterns[1].length(), day);
		}
		else {
			day = day.substr(0, 2);
			date.replace(indexOfMonth + patterns[1].length(), indexOfDay - indexOfMonth - patterns[1].length(), day);
		}

		return CheckData(date);
	}
}