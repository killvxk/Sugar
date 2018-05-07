#include <iostream>
#include <sstream>
#include <set>
#include <regex>

#include "../Utils.h"
#include "AmountDataFixed.h"

namespace International
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

		std::string validated_subtotal, validated_tip, validated_tiprate, validated_total;
		std::string result_subtotal, result_tip, result_tiprate, result_total;

		if (!ParseData(InValidatedDocument, validated_subtotal, validated_tip, validated_tiprate, validated_total)
			|| !ParseData(InResultDocument, result_subtotal, result_tip, result_tiprate, result_total)
			|| !CheckData(validated_subtotal)
			|| !CheckData(validated_tip)
			|| !CheckData(validated_total)) {
			std::cout << " Data Error" << std::endl;
			return;
		}

		if (!Equal(validated_subtotal, result_subtotal) || !Equal(validated_tip, result_tip) || !Equal(validated_total, result_total)) {
			ErrorCount++;
		}
		else {
			std::cout << "Validated Equal To Result" << std::endl;
			return;
		}

		std::cout << result_subtotal << ", " << result_tip << ", " << result_total << std::endl;
		std::cout << (result_tiprate.length() == 0 ? "(Empty)" : result_tiprate) << std::endl;
		std::cout << " Fixed To " << std::endl;

		NormalizeData(result_subtotal);
		NormalizeData(result_tip);
		NormalizeData(result_total);

		std::string tiprate_tip, tiprate_total;
		FixedTipRate(result_tiprate, tiprate_tip, tiprate_total);
		FixedAmountData(result_subtotal, result_tip, result_total, tiprate_tip, tiprate_total);

		if (!CheckData(result_subtotal))
			FixedAmountData(result_subtotal);
		if (!CheckData(result_tip))
			FixedAmountData(result_tip);
		if (!CheckData(result_total))
			FixedAmountData(result_total);

		std::cout << result_subtotal << ", " << result_tip << ", " << result_total << std::endl;
		std::cout << (result_tiprate.length() == 0 ? "(Empty)" : result_tiprate) << std::endl;

		if (Equal(validated_subtotal, result_subtotal) && Equal(validated_tip, result_tip) && Equal(validated_total, result_total)) {
			FixedCount++;
			std::cout << "Fixed Success!" << std::endl;
		}
		else {
			std::cout << "Validated " << validated_subtotal << ", " << validated_tip << ", " << validated_total << std::endl;
			std::cout << "Fixed Falied!" << std::endl;
		}
	}

	void AmountDataFixed::AfterFixed() {
		std::cout << "Error Count " << ErrorCount << ", Fixed Count " << FixedCount << ", Fixed Error Count " << FixedErrorCount << std::endl;

		std::cout << std::endl;
		std::cout << "<=================End Fixed Amount Data" << std::endl;
	}

	bool AmountDataFixed::ParseData(const std::shared_ptr<rapidjson::Document> &jsonDocument, std::string &subtotal, std::string &tip, std::string &tip_rate, std::string &total) {
		if (!jsonDocument || !jsonDocument->IsObject() || !jsonDocument->HasMember("regions"))
			return false;

		rapidjson::Value &regions = jsonDocument->operator[]("regions");

		bool has_payment = false, has_tip = false, has_tiprate = false, has_total = false;
		for (auto &region : regions.GetArray()) {
			if (!region.HasMember("cls")
				|| !region.HasMember("result")) {
				continue;
			}

			switch (region["cls"].GetInt())
			{
			case 3: {
				subtotal = region["result"].GetArray()[0].GetString();
				has_payment = subtotal.size();
				break;
			}
			case 5: {
				tip = region["result"].GetArray()[0].GetString();
				has_tip = tip.size();
				break;
			}
			case 6: {
				total = region["result"].GetArray()[0].GetString();
				has_total = total.size();
				break;
			}
			case 11: {
				tip_rate = region["result"].GetArray()[0].GetString();
				has_tiprate = tip_rate.size();
				break;
			}
			default:
				break;
			}
		}

		return has_payment && (has_tip || has_total);
	}

	bool AmountDataFixed::CheckData(const std::string &data)
	{
		if (!CheckNumber(data)) {
			return false;
		}

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

		return true;
	}

	void AmountDataFixed::FixedTipRate(std::string &tiprate, std::string &tip, std::string &total) {
		if (tiprate.size() == 0) return;

		std::vector<std::string> tokens;
		std::regex re("%|\\$|,|:|-|\\s+");
		std::sregex_token_iterator token_iterator(tiprate.begin(), tiprate.end(), re, -1);
		std::sregex_token_iterator end;
		while (token_iterator != end) {
			if (CheckNumber(*token_iterator)) {
				tokens.push_back(*token_iterator);
			}
			token_iterator++;
		}

		if (tokens.size() == 3) {
			std::ostringstream stringStream;
			int rate = atoi(tokens[0].c_str());
			if (rate > 100) {
				tokens[0] = std::to_string(rate % 100);
			}
			stringStream << tokens[0] << "% tip = $" << tokens[1] << ", Total = $" << tokens[2];
			tiprate = stringStream.str();
			tip = tokens[1];
			total = tokens[2];
		}
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

	void AmountDataFixed::FixedAmountData(std::string &subtotal, std::string &tip, std::string &total, const std::string &tiprate_tip, const std::string &tiprate_total) {
		if (CheckNumber(subtotal) && CheckNumber(tip) && CheckNumber(total)) {
			double d_subtotal = std::stod(subtotal), d_tip = std::stod(tip), d_total = std::stod(total);
			if (Equal(d_total, d_subtotal + d_tip)) {
				return;
			}
		}

		if (CheckNumber(tiprate_tip) && CheckNumber(tiprate_total)) {
			double d_tip = std::stod(tiprate_tip), d_total = std::stod(tiprate_total);
			if (CheckNumber(subtotal)) {
				double d_subtotal = std::stod(subtotal), d_tip = std::stod(tiprate_tip), d_total = std::stod(tiprate_total);
				if (Equal(d_total, d_subtotal + d_tip)) {
					total = tiprate_total;
					tip = tiprate_tip;
					return;
				}
			}
			else {
				double d_subtotal = d_total - d_tip;
				std::string fixed = DoubleToString(d_subtotal);
				if (subtotal.length() == 0 || !CheckNumber(subtotal) || Similarity(fixed, subtotal) > 0.7) {
					subtotal = fixed;
					total = tiprate_total;
					tip = tiprate_tip;
					return;
				}
			}
		}


		if (CheckData(subtotal) && FixedDataBySubTotal(subtotal, tip, total)) {
			return;
		}

		if (CheckData(total) && FixedDataByTotal(subtotal, tip, total)) {
			return;
		}

		return;
	}

	bool AmountDataFixed::FixedDataBySubTotal(std::string &subtotal, std::string &tip, std::string &total) {
		double d_subtotal = std::stod(subtotal);

		if (CheckNumber(tip)) {
			double d_tip = std::stod(tip);
			double d_total = d_subtotal + d_tip;
			std::string fixed = DoubleToString(d_total);
			if (total.length() == 0 || !CheckNumber(total) ||Similarity(fixed, total) > 0.7) {
				total = fixed;
				if (!CheckData(tip))
					FixedAmountData(tip);
				return true;
			}
		}

		if (CheckNumber(total)) {
			double d_total = std::stod(total);
			double d_tip = d_total - d_subtotal;
			std::string fixed = DoubleToString(d_tip);
			if (tip.length() == 0 || !CheckNumber(tip) || Similarity(fixed, tip) > 0.7) {
				tip = fixed;
				if (!CheckData(total))
					FixedAmountData(total);
				return true;
			}
		}

		return false;
	}

	bool AmountDataFixed::FixedDataByTotal(std::string &subtotal, std::string &tip, std::string &total) {
		double d_total = std::stod(total);

		if (CheckNumber(tip)) {
			double d_tip = std::stod(tip);
			double d_subtotal = d_total - d_tip;
			std::string fixed = DoubleToString(d_subtotal);
			if (subtotal.length() == 0 || !CheckNumber(subtotal) || Similarity(fixed, subtotal) > 0.7) {
				subtotal = fixed;
				if (!CheckData(tip)) {
					FixedAmountData(tip);
				}
				return true;
			}
		}

		if (CheckNumber(subtotal)) {
			double d_subtotal = std::stod(subtotal);
			double d_tip = d_total - d_subtotal;
			std::string fixed = DoubleToString(d_tip);
			if (tip.length() == 0 || !CheckNumber(tip) || Similarity(fixed, tip) > 0.7) {
				tip = fixed;
				if (!CheckData(subtotal)) {
					FixedAmountData(subtotal);
				}
				return true;
			}
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

	void AmountDataFixed::NormalizeData(std::string &data) {
		if (data.length() == 0) return;

		static std::vector<std::string> patterns = {" ", "$", "\\"};
		for (const auto &pattern : patterns) {
			StringReplace(data, pattern, "");
		}
		/*std::vector<std::string> invalidch;
		static std::set<char> numberPatterns = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' };
		for (int index = 0; index < data.size(); ) {
			if (numberPatterns.count(data[index]) == 0) {
				data.erase(data.begin() + index);
			}
			else {
				index++;
			}
		}*/
	}
}