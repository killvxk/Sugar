#pragma once
#include "DataFixed.h"

class CheckCodeDataFixed : public DataFixed
{
public:
	CheckCodeDataFixed(const std::string &InValidatedPath, const std::string &InResultPath);
	~CheckCodeDataFixed();

private:
	void BeforeFixed() override;
	void FixedData(std::shared_ptr<rapidjson::Document> &InValidatedDocument,
		std::shared_ptr<rapidjson::Document> InResultDocument) override;
	void AfterFixed() override;

	bool ParseData(const std::shared_ptr<rapidjson::Document> &InJsonDocument, std::vector<std::string> &CheckCodeVector);
	bool CheckData(const std::string &date);

	std::string FixedData(std::vector<std::string> &CheckDataVector);

private:
	const int NumberCount = 20;

	int ErrorCount = 0;
	int FixedCount = 0;
	int FixedErrorCount = 0;
};

