// PutText.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <opencv2\opencv.hpp>

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

	int offsetX = rand() / (RAND_MAX + 1.0) * (imageBG.cols * 0.8 + imageFG.cols) - imageFG.cols;
	int offsetY = rand() / (RAND_MAX + 1.0) * (imageBG.rows * 0.8 + imageFG.rows) - imageFG.rows;
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

double random() {
	return rand() / (RAND_MAX + 1.0);
}

int randomInt(int min, int max) {
	int value = random() * 20 + 1;
	while ((value % 2) == 0) {
		value = random() * 20 + 1;
	}
	value = clamp(value, min, max);
	return value % 2 == 0 ? value - 1 : value;
}

cv::Mat BlurImage(const cv::Mat &imageBG) {
	cv::Mat result;

	cv::Size kSize = { randomInt(3, std::min(imageBG.size[0] / 2, imageBG.size[1] / 2)), randomInt(3, std::min(imageBG.size[0] / 2, imageBG.size[1] / 2)) };
	cv::GaussianBlur(imageBG, result, kSize, 0);

	return result;
}

cv::Mat DrawLine(const cv::Mat &imageBG) {
	cv::Mat result;
	imageBG.copyTo(result);

	cv::Point start = { int(random() * imageBG.cols * 0.5), int(random() * imageBG.rows * 0.5) };
	cv::Point end = { int(random() * imageBG.cols * 0.5 + imageBG.cols * 0.5), int(random() * imageBG.rows * 0.5 + imageBG.rows * 0.5) };
	if (int(rand() / (RAND_MAX + 1.0) * 2)) {
		start.x = 0;
	}
	else {
		start.y = 0;
	}

	if (int(rand() / (RAND_MAX + 1.0) * 2)) {
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
	cv::Point poly[4] = { {0, 0,}, { imageBG.cols, 0}, { imageBG.cols, int(random() * imageBG.rows * 0.25 + imageBG.rows * 0.25) }, { 0, int(random() * imageBG.rows * 0.5) } };
	const cv::Point* ppt[1] = { poly };
	cv::fillPoly(result, ppt, npt, 1, cv::Scalar(0, 0, 0));

	//cv::imshow("result", result);
	//cv::waitKey();

	return result;
}

int main()
{
	std::cout << "Start Program" << std::endl;

	srand((int)time(0));

	std::string outputFolder = "C:/Users/User/Desktop/";
	std::string chaptersFolder = "C:/Users/User/Desktop/chapters/";
	std::vector<std::string> chapters = {"1.png", "2.png", "3.png",  "4.png",  "5.png",  "6.png", };
	std::vector<std::string> bgFileList;
	auto vistor = [&](const std::string &filePath, const std::string &fileName) {
		if (strstr(fileName.c_str(), ".jpg") == nullptr) {
			return;
		}

		std::size_t dotIndex = fileName.rfind('.');
		if (dotIndex == std::string::npos)
			return;

		std::string name = fileName.substr(0, dotIndex);
		std::string imageBGPath = filePath + fileName;
		std::string imageInfo = filePath + name + ".txt";

		std::cout << "Start Handler " << name << std::endl;

		cv::Mat imageBG = cv::imread(imageBGPath);
		for (int index = 0; index < 6; ++index) {
			//cv::Mat imageFG = cv::imread(chaptersFolder + chapters[index], -1);
			//cv::Mat Result = BlendTwoImage(imageBG, imageFG);
			//cv::Mat Result = BlurImage(imageBG);
			//cv::Mat Result = DrawLine(imageBG);
			cv::Mat Result = DrawPoly(imageBG);
			char ResultPath[MAX_PATH] = { 0 };
			sprintf_s(ResultPath, "%soutput/%s-%d.jpg", outputFolder.c_str(), name.c_str(), index);
			cv::imwrite(ResultPath, Result);
			char InfoPath[MAX_PATH] = { 0 };
			sprintf_s(InfoPath, "%soutput/%s-%d.txt", outputFolder.c_str(), name.c_str(), index);
			CopyFileA(imageInfo.c_str(), InfoPath, FALSE);
		}
	};

	//vistor(outputFolder + "cls5/train/", "5acca607030e2475730b1171.jpg");

	VisitFolder(outputFolder + "cls5/train/", vistor);

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