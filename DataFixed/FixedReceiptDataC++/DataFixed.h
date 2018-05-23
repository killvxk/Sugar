#pragma once
#include <memory>
#include <rapidjson\document.h>

class DataFixed
{
public:
	DataFixed(const std::string &InValidatedPath, const std::string &InResultPath);
	virtual ~DataFixed();

	void StartFixed();

private:
	virtual void BeforeFixed() { }
	virtual void FixedData(std::shared_ptr<rapidjson::Document> &InValidatedDocument,
		std::shared_ptr<rapidjson::Document> InResultDocument) = 0;
	virtual void AfterFixed() { }

	std::shared_ptr<rapidjson::Document> ReadFromFile(const std::string &InJsonPath);

private:
	const std::string ValidatedPath;
	const std::string ResultPath;
};

