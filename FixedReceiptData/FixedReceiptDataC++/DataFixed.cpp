#include <iostream>
#include <string>
#include <memory>
#include <Windows.h>
#include <rapidjson\document.h>

#include "DataFixed.h"
#include "Utils.h"

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

DataFixed::DataFixed(const std::string &InValidatedPath, const std::string &InResultPath)
	: ValidatedPath(InValidatedPath)
	, ResultPath(InResultPath) {
}

DataFixed::~DataFixed() {

}

void DataFixed::StartFixed() {
	BeforeFixed();

	auto vistor = [&](const std::string &InFilePath, const std::string &InFileName) {
		std::cout << "Start Handler " << InFileName << std::endl;

		std::shared_ptr<rapidjson::Document> ValidatedDocument = ReadFromFile(ValidatedPath + InFileName);
		std::shared_ptr<rapidjson::Document> ResultDocument = ReadFromFile(ResultPath + InFileName);

		FixedData(ValidatedDocument, ResultDocument);

		std::cout << "End Handler " << InFileName << std::endl << std::endl;
	};

	vistor(ValidatedPath, "10101_290.jpg.json");
	VisitFolder(ValidatedPath, vistor);

	AfterFixed();
}

std::shared_ptr<rapidjson::Document> DataFixed::ReadFromFile(const std::string &InJsonPath)
{
	std::shared_ptr<rapidjson::Document> jsonDocument = std::make_shared<rapidjson::Document>();

	FILE* fp = nullptr;
	fopen_s(&fp, InJsonPath.c_str(), "rb");
	if (!fp)
	{
		return jsonDocument;
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	std::string buffer(size, 0);
	fseek(fp, 0, SEEK_SET);
	size_t n = fread(&buffer[0], 1, size, fp);
	fclose(fp);
	StringReplace(buffer, "\r\n", "");
	jsonDocument->Parse(&buffer[0]);
	return jsonDocument;
}