#pragma once
#include <vector>
#include "../DataFixed.h"

namespace Domestic
{
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

		bool ParseData(const std::shared_ptr<rapidjson::Document> &jsonDocument, std::vector<std::string> &FVectorDate);
		bool CheckData(const std::string &date);

		bool FixedDate(std::string &date, std::vector<std::string> &DateVector);

	private:
		int ErrorCount = 0;
		int FixedCount = 0;
		int FixedErrorCount = 0;
	};
}
