#pragma once
#include "../DataFixed.h"

namespace International
{
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

		bool ParseData(const std::shared_ptr<rapidjson::Document> &jsonDocument, std::string &subtotal, std::string &tip, std::string &tip_rate, std::string &total);
		bool CheckData(const std::string &data);
		bool CheckNumber(const std::string &data);

		void FixedTipRate(std::string &tiprate, std::string &tip, std::string &total);
		void FixedAmountData(std::string &data);
		void FixedAmountData(std::string &subtotal, std::string &tip, std::string &total, const std::string &tiprate_tip, const std::string &tiprate_total);
		bool FixedDataBySubTotal(std::string &subtotal, std::string &tip, std::string &total);
		bool FixedDataByTotal(std::string &subtotal, std::string &tip, std::string &total);;

		std::string DoubleToString(double invalue);
		double Round(double value);
		void NormalizeData(std::string &data);

	private:
		int ErrorCount = 0;
		int FixedCount = 0;
		int FixedErrorCount = 0;
	};
}