#pragma once
#include <vector>
#include "DataFixed.h"

class DateDataFixed : public DataFixed
{
public:
	DateDataFixed(const std::string &InValidatedPath, const std::string &InResultPath);
	~DateDataFixed();

private:
	void BeforeFixed() override;
	void FixedData(std::shared_ptr<rapidjson::Document> &InValidatedDocument,
		std::shared_ptr<rapidjson::Document> InResultDocument) override;
	void AfterFixed() override;

	bool ParseDate(const std::shared_ptr<rapidjson::Document> &jsonDocument, std::vector<std::string> &FVectorDate);

	bool CheckDate(const std::string &date);
	bool FixedDate(std::string &date, std::vector<std::string> &DateVector);

private:
	int totalCount = 0;
	int fixedCount = 0;
	int fixedErrorCount = 0;
};

