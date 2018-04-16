// PutText.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <opencv2\opencv.hpp>

void GetStringSize(HDC hDC, const wchar_t* str, int* w, int* h)
{
	SIZE size;
	GetTextExtentPoint32(hDC, str, wcslen(str), &size);
	if (w != 0) *w = size.cx;
	if (h != 0) *h = size.cy;
}

cv::Mat putText(const cv::Mat &bgImage, const wchar_t* str, cv::Point org, cv::Scalar color, int fontSize, const char* faceName, bool italic = false, bool underline = false)
{
	CV_Assert(bgImage.data != 0 && (bgImage.channels() == 1 || bgImage.channels() == 3));

	cv::Mat outputImage;
	bgImage.copyTo(outputImage);

	int x, y, r, b;
	if (org.x > bgImage.cols || org.y > bgImage.rows) return outputImage;
	x = org.x < 0 ? -org.x : 0;
	y = org.y < 0 ? -org.y : 0;

	LOGFONTA lf;
	lf.lfHeight = -fontSize;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 5;
	lf.lfItalic = italic;
	lf.lfUnderline = underline;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = 0;
	strcpy_s(lf.lfFaceName, faceName);

	HFONT hf = CreateFontIndirectA(&lf);
	HDC hDC = CreateCompatibleDC(0);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hf);

	int strBaseW = 0, strBaseH = 0;
	GetStringSize(hDC, str, &strBaseW, &strBaseH);

	if (org.x + strBaseW < 0 || org.y + strBaseH < 0)
	{
		SelectObject(hDC, hOldFont);
		DeleteObject(hf);
		DeleteObject(hDC);
		return outputImage;
	}

	r = org.x + strBaseW > bgImage.cols ? bgImage.cols - org.x - 1 : strBaseW - 1;
	b = org.y + strBaseH > bgImage.rows ? bgImage.rows - org.y - 1 : strBaseH - 1;
	org.x = org.x < 0 ? 0 : org.x;
	org.y = org.y < 0 ? 0 : org.y;

	BITMAPINFO bmp = { 0 };
	BITMAPINFOHEADER& bih = bmp.bmiHeader;
	int strDrawLineStep = strBaseW * 3 % 4 == 0 ? strBaseW * 3 : (strBaseW * 3 + 4 - ((strBaseW * 3) % 4));

	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = strBaseW;
	bih.biHeight = strBaseH;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = strBaseH * strDrawLineStep;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	void* pDibData = 0;
	HBITMAP hBmp = CreateDIBSection(hDC, &bmp, DIB_RGB_COLORS, &pDibData, 0, 0);

	CV_Assert(pDibData != 0);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBmp);

	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, 0);

	TextOut(hDC, 0, 0, str, wcslen(str));

	uchar* bgImageData = (uchar*)outputImage.data;
	int bgImageStep = outputImage.step / sizeof(bgImageData[0]);
	unsigned char* pImg = (unsigned char*)outputImage.data + org.x * outputImage.channels() + org.y * bgImageStep;
	unsigned char* pStr = (unsigned char*)pDibData + x * 3;
	for (int tty = y; tty <= b; ++tty)
	{
		unsigned char* subImg = pImg + (tty - y) * bgImageStep;
		unsigned char* subStr = pStr + (strBaseH - tty - 1) * strDrawLineStep;
		for (int ttx = x; ttx <= r; ++ttx)
		{
			for (int n = 0; n < outputImage.channels(); ++n) {
				double vtxt = subStr[n] / 255.0;
				int cvv = vtxt * color.val[n] + (1 - vtxt) * subImg[n];
				subImg[n] = cvv > 255 ? 255 : (cvv < 0 ? 0 : cvv);
			}

			subStr += 3;
			subImg += outputImage.channels();
		}
	}

	SelectObject(hDC, hOldBmp);
	SelectObject(hDC, hOldFont);
	DeleteObject(hf);
	DeleteObject(hBmp);
	DeleteDC(hDC);

	return outputImage;
}

template <class Vistor>
void VisitFolder(const std::wstring &lpPath, Vistor vistor)
{
	assert(lpPath.length());

	std::wstring szFind;
	WIN32_FIND_DATA FindFileData;

	szFind = lpPath;
	szFind.append(L"*.*");

	HANDLE hFind = ::FindFirstFile(szFind.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind) return;

	while (true)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				std::wstring szFolder(lpPath);
				szFolder.append(FindFileData.cFileName).append(L"\\");
				VisitFolder(szFolder, vistor);
			}
		}
		else
		{
			vistor(lpPath, FindFileData.cFileName);
		}
		if (!FindNextFile(hFind, &FindFileData))    break;
	}
	FindClose(hFind);
}

cv::Mat ReadImage(const wchar_t* filename)
{
	FILE* fp = nullptr;
	_wfopen_s(&fp, filename, L"wb");

	if (!fp)
	{
		return cv::Mat::zeros(1, 1, CV_8U);
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	std::vector<char> buffer(size, 0);
	fseek(fp, 0, SEEK_SET);
	long n = fread(&buffer[0], 1, size, fp);
	cv::_InputArray arr(&buffer[0], size);
	cv::Mat img = cv::imdecode(arr, CV_LOAD_IMAGE_COLOR);
	fclose(fp);
	return img;
}

void WriteImage(const wchar_t *filename, const cv::Mat &mat) {
	FILE* fp = nullptr;
	_wfopen_s(&fp, filename, L"wb");
	if (!fp) {
		return;
	}

	std::vector<uchar> buf(mat.total() * 2);
	if (cv::imencode(".jpg", mat, buf))
	{
		fwrite(buf.data(), sizeof(uchar), buf.size(), fp);
	}

	fclose(fp);
}

std::wstring ReadFont(const wchar_t* filename)
{
	FILE* fp = nullptr;
	_wfopen_s(&fp, filename, L"wb");

	if (!fp)
	{
		return L"宋体";
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	std::vector<char> buffer(size + 2, 0);
	fseek(fp, 0, SEEK_SET);
	long n = fread(&buffer[0], 1, size, fp);
	fclose(fp);
	std::wstring result((wchar_t *)&buffer[0]);
	return result;
}

void ReabgImageations(const wchar_t* filename, std::vector<std::wstring> &stations)
{
	stations.clear();

	FILE* fp = nullptr;
	_wfopen_s(&fp, filename, L"wb");

	if (!fp)
	{
		return;
	}
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	std::vector<char> buffer(size + 2, 0);
	fseek(fp, 0, SEEK_SET);
	long n = fread(&buffer[0], 1, size, fp);
	fclose(fp);
	std::wstring result((wchar_t *)&buffer[0]);

	wchar_t split[] = L"、";
	wchar_t *token = nullptr;
	wchar_t *nexttoken = nullptr;
	token = wcstok_s(&result[0], split, &nexttoken);
	while (token)
	{
		stations.push_back(token);
		token = wcstok_s(NULL, split, &nexttoken);
	}
}

int main()
{
	std::wstring outputPath = L"C:\\Users\\User\\Desktop\\station\\";

	std::vector<std::wstring> stations;
	ReabgImageations((outputPath + L"stations.txt").c_str(), stations);

	std::vector<std::wstring> bgFileList;
	//std::vector<std::wstring> fontFileList;
	auto vistor = [&](const std::wstring &filePath, const std::wstring &fileName) {
		if (wcsstr(fileName.c_str(), L"-2.jpg") != nullptr)
		{
			bgFileList.push_back(filePath + fileName);
		}
		//else if (wcsstr(fileName.c_str(), L"-3.txt") != nullptr)
		//{
		//	fontFileList.push_back(filePath + fileName);
		//}
	};

	VisitFolder(outputPath, vistor);

	int dataCount = bgFileList.size();
	for (int index = 0; index < dataCount; ++index) {
		cv::Mat bgImage = ReadImage(bgFileList[index].c_str());
		if (bgImage.rows && bgImage.cols)
		{
			wchar_t indexBuffer[MAX_PATH] = { 0 };
			swprintf_s(indexBuffer, L"Output\\%d\\", index);
			CreateDirectory((outputPath + indexBuffer).c_str(), 0);
			int nameIndex = 0;
			for (auto station : stations) {
				wchar_t nameIndexBuffer[MAX_PATH] = { 0 };
				swprintf_s(nameIndexBuffer, L"%d", nameIndex++);
				WriteImage((outputPath + indexBuffer + nameIndexBuffer + station + L".jpg").c_str(),
					putText(bgImage, station.c_str(), cv::Point(0, 0), cv::Scalar(0, 0, 0), bgImage.rows, "黑体", false, false));
			}
		}
	}

	int waitKey;
	std::cin >> waitKey;
	return 0;
}