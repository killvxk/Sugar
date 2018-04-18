// FixedReceiptData.cpp : Defines the entry point for the console application.
//


#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <Windows.h>
#include <algorithm>
#include <set>
#include <rapidjson\document.h>

#ifdef min
#undef min
#endif // min

#ifdef max
#undef max
#endif // max

void StringReplace(std::string &strBase, std::string strSrc, std::string strDes)
{
	std::string::size_type pos = 0;
	std::string::size_type srcLen = strSrc.size();
	std::string::size_type desLen = strDes.size();
	pos = strBase.find(strSrc, pos);
	while ((pos != std::string::npos))
	{
		strBase.replace(pos, srcLen, strDes);
		pos = strBase.find(strSrc, (pos + desLen));
	}
}

std::shared_ptr<rapidjson::Document> ReadFromFile(const std::string &jsonPath)
{
	std::shared_ptr<rapidjson::Document> jsonDocument = std::make_shared<rapidjson::Document>();

	FILE* fp = nullptr;
	fopen_s(&fp, jsonPath.c_str(), "rb");
	if (!fp)
	{
		return jsonDocument;
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	std::string buffer(size, 0);
	fseek(fp, 0, SEEK_SET);
	long n = fread(&buffer[0], 1, size, fp);
	fclose(fp);
	StringReplace(buffer, "\r\n", "");
	jsonDocument->Parse(&buffer[0]);
	return jsonDocument;
}

bool ParseTax(std::shared_ptr<rapidjson::Document> jsonDocument, std::string &before_tax, std::string &tax, std::string &after_tax) {
	if (!jsonDocument || !jsonDocument->IsObject() || !jsonDocument->HasMember("regions"))
		return false;

	rapidjson::Value &regions = jsonDocument->operator[]("regions");

	bool has_before_tax = false, has_tax = false, has_after_tax = false;
	for (auto &region : regions.GetArray()) {
		if (!region.HasMember("cls")
			|| !region.HasMember("ref_result")
			|| !region.HasMember("result")) {
			continue;
		}

		switch (region["cls"].GetInt())
		{
		case 4: {
			before_tax = region["result"].GetArray()[0].GetString();
			has_before_tax = before_tax.size();
			break;
		}
		case 8: {
			tax = region["result"].GetArray()[0].GetString();
			has_tax = tax.size();
			break;
		}
		case 9: {
			after_tax = region["result"].GetArray()[0].GetString();
			has_after_tax = after_tax.size();
			break;
		}
		default:
			break;
		}
	}

	return has_before_tax && has_tax && has_after_tax;
}

template <class Type>
bool Equal(Type value1, Type value2) { return value1 == value2; }

template <>
bool Equal(double value1, double value2) {
	static constexpr double epsilon = std::numeric_limits<double>::epsilon();
	return std::abs(value1 - value2) < epsilon;
}

float Similarity(const std::string &str1, const std::string &str2) {

	int len1 = str1.length();
	int len2 = str2.length();
	std::vector<std::vector<int> > diff((len1 + 1), std::vector<int>(len2 + 1));
	for (int index = 0; index <= len1; index++) {
		diff[index][0] = index;
	}
	for (int index = 0; index <= len2; index++) {
		diff[0][index] = index;
	}

	int temp;
	for (int i = 1; i <= len1; i++) {
		for (int j = 1; j <= len2; j++) {
			if (str1[i - 1] == str2[j - 1]) {
				temp = 0;
			}
			else {
				temp = 1;
			}

			diff[i][j] = std::min(std::min(diff[i - 1][j - 1] + temp, diff[i][j - 1] + 1), diff[i - 1][j] + 1);
		}
	}

	return 1 - (float)diff[len1][len2] / std::max(len1, len2);
}

std::string DoubleToString(double invalue)
{
	std::string value = std::to_string(int(invalue * 100));
	if (value.length() >= 3) {
		value.insert(value.end() - 2, '.');
	}
	else {
		value = std::to_string(invalue);
		size_t dotIndex = value.find('.');
		if (dotIndex != std::string::npos) {
			return value.substr(0, dotIndex + 3);
		}
	}
	return value;
}

bool CheckData(const std::string &data)
{
	size_t dotIndex = data.rfind('.');
	if (dotIndex != std::string::npos && (data.length() - data.rfind('.')) == 3)
		return true;

	return false;
}

double Round(double value)
{
	int ivalue = value * 1000;
	if (ivalue % 10 > 4)
	{
		return value + 0.01;
	}

	return value;
}

bool FixedDataByAfterTax(std::string &before_tax, std::string &tax, double d_after_tax) {
	double tax_rate[] = { 0.03, 0.05, 0.06, 0.11, 0.17 };
	float similarity = 0.0f;
	std::string fixed_before_tax;
	std::string fixed_tax;
	for (auto rate : tax_rate) {
		for (double error = -0.09; error < 0.1; error += 0.01) {
			double temp_before_tax = (d_after_tax / (1.0 + rate)) + error;
			double temp_tax = Round(temp_before_tax * rate);
			if (Equal((int(temp_before_tax * 100) % 100 + int(temp_tax * 100) % 100) % 100, int(d_after_tax * 100) % 100)) {
				std::string fixed = DoubleToString(temp_before_tax);
				float fixed_similarity = Similarity(fixed, before_tax);
				if (fixed_similarity > similarity) {
					fixed_before_tax = fixed;
					fixed_tax = DoubleToString(temp_tax);
					similarity = fixed_similarity;
				}
			}
		}
	}

	size_t length = fixed_before_tax.length();
	if (length && similarity > 0.5) {
		if (length == before_tax.length()) {
			std::vector<std::set<char> > pattens = { { '8', '6', '0', '5', '9', '4' },{ '1', '4', '7' },{ '3', '8' },{ '3', '7' } };
			for (int index = 0; index < length; ++index) {
				if (fixed_before_tax[index] != before_tax[index]) {
					bool match = false;
					for (auto patten : pattens) {
						if (patten.count(fixed_before_tax[index]) && patten.count(before_tax[index])) {
							match = true;
							break;
						}
					}
					if (!match) {
						return false;;
					}
				}
			}
		}
		else {
			if (similarity < 0.6 && (std::max(length, before_tax.length()) * similarity) > 3.0) {
				return false;
			}
		}


		before_tax = fixed_before_tax;
		tax = fixed_tax;
		return true;
	}

	return false;
}

void FixedTaxData(std::string &before_tax, std::string &tax, std::string &after_tax) {
	double d_before_tax = std::stod(before_tax), d_tax = std::stod(tax), d_after_tax = std::stod(after_tax);

	if (Equal(d_after_tax, d_before_tax + d_tax))
		return;

	if (CheckData(after_tax) && FixedDataByAfterTax(before_tax, tax, d_after_tax)) {
		return;
	}

	double tax_rate[] = { 0.03, 0.05, 0.06, 0.11, 0.17 };
	if (CheckData(tax)) {
		float similarity = 0.0f;
		std::string fixed_before_tax;
		std::string fixed_tax;
		for (auto rate : tax_rate) {
			for (double error = -0.09; error < 0.1; error += 0.01) {
				double temp_before_tax = (d_tax / rate) + error;
				std::string fixed = DoubleToString(temp_before_tax);
				float fixed_similarity = Similarity(fixed, before_tax);
				if (fixed_similarity > similarity) {
					fixed_before_tax = fixed;
					//fixed_tax = DoubleToString(temp_tax);
					similarity = fixed_similarity;
				}
			}
		}
	}
}

template <class Vistor>
void VisitFolder(const std::string &lpPath, Vistor vistor)
{
	assert(lpPath.length());

	std::string szFind;
	WIN32_FIND_DATAA FindFileData;

	szFind = lpPath;
	szFind.append("*.*");

	HANDLE hFind = ::FindFirstFileA(szFind.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind) return;

	while (true)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				std::string szFolder(lpPath);
				szFolder.append(FindFileData.cFileName).append("\\");
				VisitFolder(szFolder, vistor);
			}
		}
		else
		{
			vistor(lpPath, FindFileData.cFileName);
		}
		if (!FindNextFileA(hFind, &FindFileData))    break;
	}
	FindClose(hFind);
}

int main()
{
	int errorCount = 0;
	int fixedCount = 0;
	int fixedErrorCount = 0;

	std::cout << "Start Program!" << std::endl;

	std::string validatedPath = "D:\\1000\\validated\\";
	std::string resultPath = "D:\\1000\\result\\";
	auto vistor = [&](const std::string &filePath, const std::string &fileName) {
		std::shared_ptr<rapidjson::Document> validatedDocument = ReadFromFile(validatedPath + fileName);
		std::shared_ptr<rapidjson::Document> resultDocument = ReadFromFile(resultPath + fileName);

		std::string validated_before_tax, validated_tax, validated_after_tax;
		std::string result_before_tax, result_tax, result_after_tax;

		if (!ParseTax(validatedDocument, validated_before_tax, validated_tax, validated_after_tax)
			|| !ParseTax(resultDocument, result_before_tax, result_tax, result_after_tax)
			|| !CheckData(validated_before_tax)
			|| !CheckData(validated_tax)
			|| !CheckData(validated_after_tax)) {
			std::cout << fileName << " Data Error" << std::endl;
			std::cout << std::endl;
			return;
		}

		if (!Equal(validated_before_tax, result_before_tax) || !Equal(validated_tax, result_tax) || !Equal(validated_after_tax, result_after_tax)) {
			errorCount++;
		}
		else {
			return;
		}

		std::string origin_result_before_tax = result_before_tax;

		std::cout << fileName << " Validated Not Equal To Result" << std::endl;
		std::cout << result_before_tax << ", " << result_tax << ", " << result_after_tax << " Fixed To " << std::endl;

		FixedTaxData(result_before_tax, result_tax, result_after_tax);

		std::cout << result_before_tax << ", " << result_tax << ", " << result_after_tax << std::endl;

		if (Equal(validated_before_tax, result_before_tax) && Equal(validated_tax, result_tax) && Equal(validated_after_tax, result_after_tax)) {
			fixedCount++;
			std::cout << "Fixed Success!" << std::endl;
		}
		else {
			std::cout << "Validated " << validated_before_tax << ", " << validated_tax << ", " << validated_after_tax << std::endl;
			if (Equal(origin_result_before_tax, validated_before_tax) && !Equal(result_before_tax, validated_before_tax)) {
				fixedErrorCount++;
				std::cout << "Fixed origin before tax " << origin_result_before_tax << " to " << result_before_tax << std::endl;
			}
			std::cout << "Fixed Falied!" << std::endl;
		}

		std::cout << std::endl;
	};

	vistor(validatedPath, "10100_110.jpg.json");
	VisitFolder(validatedPath, vistor);

	std::cout << "Error Count " << errorCount << ", Fixed Count " << fixedCount << ", Fixed Error Count " << fixedErrorCount << std::endl;
	std::cout << "End Program!" << std::endl;

	return 0;
}

