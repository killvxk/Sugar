#pragma once
#include <memory>
#include <rapidjson\document.h>

class AmountDataFixed
{
public:
	AmountDataFixed(const std::string &validatedPath, const std::string &resultPath);
	~AmountDataFixed();

	void StartFixed();

private:
	bool ParseTax(std::shared_ptr<rapidjson::Document> jsonDocument, std::string &before_tax, std::string &tax, std::string &after_tax);
	
	bool CheckAmountData(const std::string &data);
	void FixedAmountData(std::string &data);

	void FixedAmountData(std::string &before_tax, std::string &tax, std::string &after_tax);

	bool FixedDataByAfterTax(std::string &before_tax, std::string &tax, double d_after_tax);
	bool FixedDataByTax(std::string &before_tax, std::string &after_tax, double d_tax);
	bool FixedDataByBeforeTax(std::string &tax, std::string &after_tax, double d_before_tax);

	bool CheckBeforeTaxFixedResult(const std::string &before_tax, const std::string &fixed_before_tax, double similarity);

private:
	const std::string ValidatedPath;
	const std::string ResultPath;
};

