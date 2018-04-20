#pragma once
#include "DataFixed.h"

class AmountDataFixed : public DataFixed
{
public:
	AmountDataFixed(const std::string &InValidatedPath, const std::string &InResultPath);
	~AmountDataFixed();

private:
	void BeforeFixed() override;
	void FixedData(std::shared_ptr<rapidjson::Document> &InValidatedDocument,
		std::shared_ptr<rapidjson::Document> InResultDocument) override;
	void AfterFixed() override;

	bool ParseTax(const std::shared_ptr<rapidjson::Document> &jsonDocument, std::string &before_tax, std::string &tax, std::string &after_tax);
	
	bool CheckAmountData(const std::string &data);
	void FixedAmountData(std::string &data);

	void FixedAmountData(std::string &before_tax, std::string &tax, std::string &after_tax);

	bool FixedDataByAfterTax(std::string &before_tax, std::string &tax, double d_after_tax);
	bool FixedDataByTax(std::string &before_tax, std::string &after_tax, double d_tax);
	bool FixedDataByBeforeTax(std::string &tax, std::string &after_tax, double d_before_tax);

	bool CheckBeforeTaxFixedResult(const std::string &before_tax, const std::string &fixed_before_tax, double similarity);

	std::string DoubleToString(double invalue);
	double Round(double value);

private:
	int errorCount = 0;
	int fixedCount = 0;
	int fixedErrorCount = 0;
};

