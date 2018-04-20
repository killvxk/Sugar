#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include <rapidjson\document.h>
#include <vector>
#include <algorithm>

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

inline std::string UTF8_To_string(const std::string & str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}