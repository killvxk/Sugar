#pragma once
#include <memory>
#include <rapidjson\document.h>
#include <vector>

class DateDataFixed
{
public:
	DateDataFixed(const std::string &validatedPath, const std::string &resultPath);
	~DateDataFixed();

	void StartFixed();

private:
	bool ParaseDate(const std::shared_ptr<rapidjson::Document> &jsonDocument, std::vector<std::string> &FVectorDate);

	bool CheckDate(const std::string &date);
	bool FixedDate(std::string &date, std::vector<std::string> &DateVector);

private:
	const std::string ValidatedPath;
	const std::string ResultPath;
};

