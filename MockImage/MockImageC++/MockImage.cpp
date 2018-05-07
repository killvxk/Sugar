#include <Windows.h>
#include <opencv2\opencv.hpp>

double random() {
	return rand() / (RAND_MAX + 1.0);
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

cv::Mat BlendTwoImage(const cv::Mat &imageBG, const cv::Mat &imageFG) {
	cv::Mat result;
	imageBG.copyTo(result);

	int offsetX = random() * (imageBG.cols * 0.8 + imageFG.cols) - imageFG.cols;
	int offsetY = random() * (imageBG.rows * 0.8 + imageFG.rows) - imageFG.rows;
	for (int height = 0; height < imageFG.rows; ++height) {
		if ((height + offsetY) >= imageBG.rows || (height + offsetY) < 0)
			continue;
		for (int width = 0; width < imageFG.cols; ++width) {
			if ((width + offsetX) >= imageBG.cols || (width + offsetX) < 0) {
				continue;
			}

			cv::Vec4b pixel = imageFG.at<cv::Vec4b>(height, width);
			if (pixel[3] != 0) {
				float alpha = pixel[3] / 255.0 * 0.5;
				cv::Vec3b origin = imageBG.at<cv::Vec3b>(height + offsetY, width + offsetX);
				origin[0] = int(origin[0] * (1 - alpha) + pixel[0] * alpha);
				origin[1] = int(origin[1] * (1 - alpha) + pixel[1] * alpha);
				origin[2] = int(origin[2] * (1 - alpha) + pixel[2] * alpha);
				result.at<cv::Vec3b>(height + offsetY, width + offsetX) = origin;
			}
		}
	}

	return result;
}

template <class T>
inline T
clamp(T a, T l, T h)
{
	return (a < l) ? l : ((a > h) ? h : a);
}

cv::Mat BlurImage(const cv::Mat &imageBG, int size = 10) {
	cv::Mat result;

	auto randomInt = [&](int min, int max) {
		int value = random() * size + 1;
		while ((value % 2) == 0) {
			value = random() * size + 1;
		}
		value = clamp(value, min, max);
		return value % 2 == 0 ? value - 1 : value;
	};
	cv::Size kSize = { randomInt(3, std::min(imageBG.size[0] / 2, imageBG.size[1] / 2)), randomInt(3, std::min(imageBG.size[0] / 2, imageBG.size[1] / 2)) };
	cv::GaussianBlur(imageBG, result, kSize, 0);

	return result;
}

cv::Mat DrawLine(const cv::Mat &imageBG) {
	cv::Mat result;
	imageBG.copyTo(result);

	cv::Point start = { int(random() * imageBG.cols * 0.5), int(random() * imageBG.rows * 0.5) };
	cv::Point end = { int(random() * imageBG.cols * 0.5 + imageBG.cols * 0.5), int(random() * imageBG.rows * 0.5 + imageBG.rows * 0.5) };
	if (int(random() * 2)) {
		start.x = 0;
	}
	else {
		start.y = 0;
	}

	if (int(random() * 2)) {
		end.x = imageBG.cols;
	}
	else {
		end.y = imageBG.rows;
	}

	cv::line(result, start, end, cv::Scalar(117, 133, 156), 2);

	//cv::imshow("result", result);
	//cv::waitKey();

	return result;
}

cv::Mat DrawPoly(const cv::Mat &imageBG) {
	cv::Mat result;
	imageBG.copyTo(result);

	int npt[] = { 4 };
	const cv::Point* ppt[1];
	if (int(random() * 2))
	{
		cv::Point poly[4] = { { 0, 0, },{ imageBG.cols, 0 },{ imageBG.cols, int(random() * imageBG.rows * 0.25 + imageBG.rows * 0.25) },{ 0, int(random() * imageBG.rows * 0.5) } };
		ppt[0] = { poly };
	}
	else {
		cv::Point poly[4] = { { 0, imageBG.rows, },{ imageBG.cols, imageBG.rows },{ imageBG.cols, int(random() * imageBG.rows * 0.25 + imageBG.rows * 0.5) },{ 0, int(random() * imageBG.rows * 0.25 + imageBG.rows * 0.75) } };
		ppt[0] = { poly };
	}

	cv::fillPoly(result, ppt, npt, 1, cv::Scalar(0, 0, 0));

	//cv::imshow("result", result);
	//cv::waitKey();

	return result;
}

#ifdef DrawText
	#undef DrawText
#endif // DrawText


cv::Mat DrawText(const cv::Mat &imageBG, std::string &text, cv::Point org, cv::Scalar color, int fontSize, const char* faceName) {
	cv::Mat result;
	imageBG.copyTo(result);

	int x, y, r, b;
	if (org.x > result.cols || org.y > result.rows) return result;
	x = org.x < 0 ? -org.x : 0;
	y = org.y < 0 ? -org.y : 0;

	LOGFONTA lf;
	lf.lfHeight = -fontSize;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 5;
	lf.lfItalic = false;
	lf.lfUnderline = false;
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

	auto GetStringSize = [](HDC hDC, const char* str, int* width, int* height)
	{
		SIZE size;
		GetTextExtentPoint32A(hDC, str, strlen(str), &size);
		if (width != 0) *width = size.cx;
		if (height != 0) *height = size.cy;
	};

	int strBaseW = 0, strBaseH = 0;
	do {
		GetStringSize(hDC, text.c_str(), &strBaseW, &strBaseH);
		if (strBaseW <= result.cols)
			break;
		text.pop_back();
	} while (true);

	if (org.x + strBaseW < 0 || org.y + strBaseH < 0)
	{
		SelectObject(hDC, hOldFont);
		DeleteObject(hf);
		DeleteObject(hDC);
		return result;
	}

	r = org.x + strBaseW > result.cols ? result.cols - org.x - 1 : strBaseW - 1;
	b = org.y + strBaseH > result.rows ? result.rows - org.y - 1 : strBaseH - 1;
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

	TextOutA(hDC, 0, 0, text.c_str(), text.length());

	cv::Mat textImage(strBaseH, strBaseW, CV_8UC3, pDibData);
	cv::Mat blurImage = BlurImage(textImage, 5);
	uchar* bgImageData = (uchar*)result.data;
	int bgImageStep = result.step / sizeof(bgImageData[0]);
	unsigned char* pImg = (unsigned char*)result.data + org.x * result.channels() + org.y * bgImageStep;
	unsigned char* pStr = (unsigned char*)blurImage.data + x * 3;
	for (int tty = y; tty <= b; ++tty)
	{
		unsigned char* subImg = pImg + (tty - y) * bgImageStep;
		unsigned char* subStr = pStr + (strBaseH - tty - 1) * strDrawLineStep;
		for (int ttx = x; ttx <= r; ++ttx)
		{
			for (int n = 0; n < result.channels(); ++n) {
				double vtxt = subStr[n] / 255.0;
				int cvv = vtxt * color.val[n] + (1 - vtxt) * subImg[n];
				subImg[n] = cvv > 255 ? 255 : (cvv < 0 ? 0 : cvv);
			}

			subStr += 3;
			subImg += result.channels();
		}
	}

	SelectObject(hDC, hOldBmp);
	SelectObject(hDC, hOldFont);
	DeleteObject(hf);
	DeleteObject(hBmp);
	DeleteDC(hDC);

	return result;
}

int main()
{
	std::cout << "Start Program" << std::endl;

	srand((int)time(0));

	std::string outputFolder = "C:/Users/User/Desktop/";
	std::string chaptersFolder = "C:/Users/User/Desktop/chapters/";
	std::vector<std::string> chapters = { "1.png", "2.png", "3.png",  "4.png",  "5.png",  "6.png", };
	std::vector<std::string> fileList;
	auto vistor = [&](const std::string &filePath, const std::string &fileName) {
		if (strstr(fileName.c_str(), ".jpg") == nullptr) {
			return;
		}

		fileList.push_back(filePath + fileName);
	};

	enum EMock
	{
		BLEND,
		BLUR,
		DRAWLINE,
		DRAWPOLY,
		DRAWTEXT,
		MIX,
		MAX
	};

	int index = 0;
	auto mockData = [&](EMock eMock, const std::string &InFilePath) {
		std::size_t slashIndex = InFilePath.rfind('/');
		if (slashIndex == std::string::npos)
			return;

		std::string fileName = InFilePath.substr(slashIndex + 1);
		std::size_t dotIndex = fileName.rfind('.');
		if (dotIndex == std::string::npos)
			return;

		std::string filePath = InFilePath.substr(0, slashIndex + 1);
		std::string name = fileName.substr(0, dotIndex);
		std::string imageBGPath = filePath + fileName;
		std::string imageInfo = filePath + name + ".txt";

		index++;

		std::cout << "Start Handler " << name << "-" << index << std::endl;

		cv::Mat Result;
		cv::Mat imageBG = cv::imread(imageBGPath);
		switch (eMock)
		{
		case BLEND: {
			cv::Mat imageFG = cv::imread(chaptersFolder + chapters[int(random() * 6)], -1);
			Result = BlendTwoImage(imageBG, imageFG);
			break;
		}
		case BLUR: {
			Result = BlurImage(imageBG);
			break;
		}
		case DRAWLINE: {
			Result = DrawLine(imageBG);
			break;
		}
		case DRAWPOLY: {
			Result = DrawPoly(imageBG);
			break;
		}
		case MIX: {
			imageBG.copyTo(Result);

			if (int(random() * 2)) {
				cv::Mat imageFG = cv::imread(chaptersFolder + chapters[int(random() * 6)], -1);
				Result = BlendTwoImage(Result, imageFG);
			}

			if (int(random() * 2)) {
				Result = DrawLine(Result);
			}

			if (int(random() * 2)) {
				Result = DrawPoly(Result);
			}

			if (int(random() * 2)) {
				Result = BlurImage(Result);
			}

			break;
		}
		case DRAWTEXT: {
			static std::vector<char> chars;
			if (chars.size() == 0) {
				for (int index = 0; index < 26; ++index) {
					chars.push_back('A' + index);
				}

				for (int index = 0; index < 26; ++index) {
					chars.push_back('a' + index);
				}

				chars.push_back('\'');
				chars.push_back(' ');
			}

			std::string text(9, 0);
			text[0] = chars[(chars.size() - 1) * random()];
			for (int index = 1; index < 9; ++index) {
				text[index] = chars[chars.size() * random()];
			}

			Result = DrawText(imageBG, text, cv::Point(0, 0), cv::Scalar(0, 0, 0), imageBG.rows, "Myriad Pro");
			name = text;
			break;
		}
		default:
			throw std::exception("");
		}

		char ResultPath[MAX_PATH] = { 0 };
		sprintf_s(ResultPath, "%soutput/%s-%d.jpg", outputFolder.c_str(), name.c_str(), index);
		cv::imwrite(ResultPath, Result);
		char InfoPath[MAX_PATH] = { 0 };
		sprintf_s(InfoPath, "%soutput/%s-%d.txt", outputFolder.c_str(), name.c_str(), index);
		CopyFileA(imageInfo.c_str(), InfoPath, FALSE);
	};

	auto generate = [&](EMock eMock, int maxCount, float rate) {
		int count = (int)(maxCount * rate);

		for (int index = 0; index < count; ++index) {
			mockData(eMock, fileList[int(random() * fileList.size())]);
		}
	};

	//VisitFolder(outputFolder + "train/", vistor);
	fileList.push_back("C:/Users/User/Desktop/bg.png");

	//generate(MIX, fileList.size(), 2.0);
	//generate(BLEND, fileList.size(), 0.3);
	//generate(BLUR, fileList.size(), 0.3);
	//generate(DRAWLINE, fileList.size(), 0.3);
	//generate(DRAWPOLY, fileList.size(), 0.3);
	generate(DRAWTEXT, fileList.size(), 5000);

	//int dataCount = bgFileList.size();
	//for (int index = 0; index < dataCount; ++index) {
	//	cv::Mat bgImage = ReadImage(bgFileList[index].c_str());
	//	if (bgImage.rows && bgImage.cols)
	//	{
	//		wchar_t indexBuffer[MAX_PATH] = { 0 };
	//		swprintf_s(indexBuffer, L"Output\\%d\\", index);
	//		CreateDirectory((outputPath + indexBuffer).c_str(), 0);
	//		int nameIndex = 0;
	//		for (auto station : stations) {
	//			wchar_t nameIndexBuffer[MAX_PATH] = { 0 };
	//			swprintf_s(nameIndexBuffer, L"%d", nameIndex++);
	//			WriteImage((outputPath + indexBuffer + nameIndexBuffer + station + L".jpg").c_str(),
	//				putText(bgImage, station.c_str(), cv::Point(0, 0), cv::Scalar(0, 0, 0), bgImage.rows, "黑体", false, false));
	//		}
	//	}
	//}

	std::cout << "End Program" << std::endl;
	return 0;
}