#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include <rapidjson\document.h>
#include <vector>
#include <algorithm>

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

inline void StringReplace(std::string &strBase, std::string strSrc, std::string strDes)
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

inline std::shared_ptr<rapidjson::Document> ReadFromFile(const std::string &jsonPath)
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
	size_t n = fread(&buffer[0], 1, size, fp);
	fclose(fp);
	StringReplace(buffer, "\r\n", "");
	jsonDocument->Parse(&buffer[0]);
	return jsonDocument;
}

template <class Type>
inline bool Equal(Type value1, Type value2) { return value1 == value2; }

template <>
inline bool Equal(double value1, double value2) {
	static constexpr double epsilon = std::numeric_limits<double>::epsilon();
	return std::abs(value1 - value2) < epsilon;
}

#ifdef min
#undef min
#endif // min

#ifdef max
#undef max
#endif // max

inline float Similarity(const std::string &str1, const std::string &str2) {

	size_t len1 = str1.length();
	size_t len2 = str2.length();
	std::vector<std::vector<size_t> > diff((len1 + 1), std::vector<size_t>(len2 + 1));
	for (size_t index = 0; index <= len1; index++) {
		diff[index][0] = index;
	}
	for (size_t index = 0; index <= len2; index++) {
		diff[0][index] = index;
	}

	size_t temp;
	for (size_t i = 1; i <= len1; i++) {
		for (size_t j = 1; j <= len2; j++) {
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
