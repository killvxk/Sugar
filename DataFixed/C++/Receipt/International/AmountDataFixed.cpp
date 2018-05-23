#include <iostream>
#include <sstream>
#include <set>
#include <regex>

#include "AmountDataFixed.h"
#include "../Utils.h"
#include "../Double.h"

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

		size_t dotIndex = data.rfind(u'.');
		if (dotIndex != std::string::npos && (data.length() - data.rfind(u'.')) == 3)
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

		size_t dotIndex = data.rfind(u'.');
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
			Double d_subtotal(subtotal), d_tip(tip), d_total(total);
			if (Equal(d_total, d_subtotal + d_tip)) {
				return;
			}
		}

		if (CheckNumber(tiprate_tip) && CheckNumber(tiprate_total)) {
			Double d_tip = tiprate_tip, d_total = tiprate_total;
			if (CheckNumber(subtotal)) {
				Double d_subtotal =subtotal, d_tip = tiprate_tip, d_total = tiprate_total;
				if (Equal(d_total, d_subtotal + d_tip)) {
					total = tiprate_total;
					tip = tiprate_tip;
					return;
				}
			}
			else {
				Double d_subtotal = d_total - d_tip;
				std::string fixed = d_subtotal.ToString();
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

		if (CheckNumber(subtotal)) {
			Double d_subtotal = Double(subtotal);
			if (tip.empty() || !CheckNumber(tip)) {
				tip = "0.00";
				total = d_subtotal.ToString();
			}
			else if (CheckNumber(tip)) {
				total = (d_subtotal + Double(tip)).ToString();
			}
		}
		else if (CheckNumber(total)) {
			Double d_total = Double(total);
			if (tip.empty() || !CheckNumber(tip)) {
				tip = "0.00";
				subtotal = Double(d_total).ToString();
			}
			else if (CheckNumber(tip)) {
				subtotal = (Double(d_total) - Double(tip)).ToString();
			}
		}

		return;
	}

	bool AmountDataFixed::FixedDataBySubTotal(std::string &subtotal, std::string &tip, std::string &total) {
		Double d_subtotal = subtotal;
		bool tipTooMuch = false;

		if (CheckNumber(tip)) {
			Double d_tip = tip;
			tipTooMuch = d_tip > d_subtotal * Double("0.4");
			if (!tipTooMuch) {
				Double d_total = d_subtotal + d_tip;
				std::string fixed = d_total.ToString();
				if (d_total >= d_subtotal && (total.length() == 0 || !CheckNumber(total) || Similarity(fixed, total) > 0.5)) {
					total = fixed;
					if (!CheckData(tip))
						FixedAmountData(tip);
					return true;
				}
			}
		}

		if (CheckNumber(total)) {
			Double d_total = total;
			Double d_tip = d_total - d_subtotal;
			std::string fixed = d_tip.ToString();
			if (d_tip >= Double("0.00") && (tip.length() == 0 || !CheckNumber(tip) || tipTooMuch || Similarity(fixed, tip) > 0.5)) {
				tip = fixed;
				if (!CheckData(total))
					FixedAmountData(total);
				return true;
			}
		}

		return false;
	}

	bool AmountDataFixed::FixedDataByTotal(std::string &subtotal, std::string &tip, std::string &total) {
		Double d_total = total;
		bool tipTooMuch = false;

		if (CheckNumber(tip)) {
			Double d_tip = tip;
			tipTooMuch = d_tip > d_total * Double("0.4");
			if (!tipTooMuch) {
				Double d_subtotal = d_total - d_tip;
				std::string fixed = d_subtotal.ToString();
				if (subtotal.length() == 0 || !CheckNumber(subtotal) || Similarity(fixed, subtotal) > 0.5) {
					subtotal = fixed;
					if (!CheckData(tip)) {
						FixedAmountData(tip);
					}
					return true;
				}
			}
		}

		return false;
	}

	void AmountDataFixed::NormalizeData(std::string &data) {
		if (data.length() == 0) return;

		static std::vector<std::string> patterns = {" ", "$", "\\"};
		for (const auto &pattern : patterns) {
			StringReplace(data, pattern, "");
		}
	}

	AmountDataMildFixed::AmountDataMildFixed(const std::string &InValidatedPath, const std::string &InResultPath)
		: AmountDataFixed(InValidatedPath, InResultPath) {

	}

	AmountDataMildFixed::~AmountDataMildFixed() {

	}

	void AmountDataMildFixed::FixedAmountData(std::string &subtotal, std::string &tip, std::string &total, const std::string &tiprate_tip, const std::string &tiprate_total) {
		FormatData(subtotal);
		FormatData(tip);
		FormatData(total);

		if (CheckNumber(subtotal) && CheckNumber(tip) && CheckNumber(total)) {
			Double d_subtotal(subtotal), d_tip(tip), d_total(total);
			if (Equal(d_total, d_subtotal + d_tip)) {
				return;
			}

			if (tip.length()) {
				Double d_tip = d_total - d_subtotal;
				std::string fixed = d_tip.ToString();
				if (MissDot(tip)) {
					Trim(tip, '0');
					if (Similarity(fixed, tip) == 1 && fixed.length() - tip.length() == 1) {
						tip = fixed;
						return;
					}
				}
				else if (MissInteger(tip)) {
					if (Similarity(fixed, Double(tip).ToString()) == 1) {
						tip = fixed;
						return;
					}
				}
			}

			if (MissDot(total)) {
				Double d_total = d_subtotal + d_tip;
				std::string fixed = d_total.ToString();
				Trim(total, '0');
				if (Similarity(fixed, total) == 1 && fixed.length() - total.length() == 1) {
					total = fixed;
					return;
				}
			}
		}
	}

	void AmountDataMildFixed::FormatData(std::string &data) {
		size_t dotIndex = data.rfind(u'.');
		if (dotIndex != std::string::npos) {
			if ((data.length() - dotIndex) > 2) {
				data = data.substr(0, dotIndex + 3);
			}
		}
	}

	bool AmountDataMildFixed::MissDot(const std::string &data) {
		size_t dotIndex = data.rfind(u'.');
		return dotIndex == std::string::npos;
	}

	bool AmountDataMildFixed::MissInteger(const std::string &data) {
		size_t dotIndex = data.rfind(u'.');
		if (dotIndex != std::string::npos && dotIndex == 0) {
			return true;
		}

		return false;
	}

	void AmountDataMildFixed::Trim(std::string &data, char ch) {
		while (data.length() && data[data.length() - 1] == ch) data.pop_back();
	}
}