#pragma once
#include "../DataFixed.h"

namespace International
{
	class AmountDataFixed : public DataFixed
	{
	public:
		AmountDataFixed(const std::string &InValidatedPath, const std::string &InResultPath);
		~AmountDataFixed();

	protected:
		void BeforeFixed() override;
		void FixedData(std::shared_ptr<rapidjson::Document> &InValidatedDocument,
			std::shared_ptr<rapidjson::Document> InResultDocument) override;
		void AfterFixed() override;

		bool ParseData(const std::shared_ptr<rapidjson::Document> &jsonDocument, std::string &subtotal, std::string &tip, std::string &tip_rate, std::string &total);
		bool CheckData(const std::string &data);
		bool CheckNumber(const std::string &data);

		void FixedTipRate(std::string &tiprate, std::string &tip, std::string &total);
		void FixedAmountData(std::string &data);
		virtual void FixedAmountData(std::string &subtotal, std::string &tip, std::string &total, const std::string &tiprate_tip, const std::string &tiprate_total);
		bool FixedDataBySubTotal(std::string &subtotal, std::string &tip, std::string &total);
		bool FixedDataByTotal(std::string &subtotal, std::string &tip, std::string &total);;

		void NormalizeData(std::string &data);

	private:
		int ErrorCount = 0;
		int FixedCount = 0;
		int FixedErrorCount = 0;
	};

	class AmountDataMildFixed : public AmountDataFixed
	{
	public:
		AmountDataMildFixed(const std::string &InValidatedPath, const std::string &InResultPath);
		~AmountDataMildFixed();

	private:
		void FixedAmountData(std::string &subtotal, std::string &tip, std::string &total, const std::string &tiprate_tip, const std::string &tiprate_total) override;
		void FormatData(std::string &data);
		bool MissDot(const std::string &data);
		bool MissInteger(const std::string &data);
		void Trim(std::string &data, char ch);
	};
}