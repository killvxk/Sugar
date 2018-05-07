#pragma once
#include "../DataFixed.h"

namespace Domestic
{
	class NumberDataFixed : public DataFixed
	{
	public:
		NumberDataFixed(const std::string &InValidatedPath, const std::string &InResultPath);
		virtual ~NumberDataFixed();

	private:
		void BeforeFixed() override;
		void FixedData(std::shared_ptr<rapidjson::Document> &InValidatedDocument,
			std::shared_ptr<rapidjson::Document> InResultDocument) override;
		void AfterFixed() override;

		bool ParseData(const std::shared_ptr<rapidjson::Document> &InJsonDocument, std::vector<std::string> &NumberFirstVector, std::vector<std::string> &NumberSecondVector);
		bool CheckData(const std::string &data);

		std::string FixedNumber(std::vector<std::string> &NumberFirstVector, std::vector<std::string> &NumberSecondVector);

	private:
		const int NumberCount = 8;

		int ErrorCount = 0;
		int FixedCount = 0;
		int FixedErrorCount = 0;
	};
}