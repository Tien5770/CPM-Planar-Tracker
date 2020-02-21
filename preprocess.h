#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include <iostream>
#include<cstring>
#include<string>
#include<cmath>
#include<opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include<ctime>
bool onMove = false;
bool processFirst = true;
bool chooseVertex = true;
int pointCnt = 1;
cv::Rect featureRect;
std::vector<cv::Point2f> vertexs;
void mouse(int event, int x, int y, int flags, void* param)
{
	cv::Mat& image = *(cv::Mat*)param;
	if (processFirst)
	{

		if (event == cv::EVENT_LBUTTONDOWN)
		{
			onMove = true;
			featureRect = cv::Rect(x, y, 0, 0);
		}
		if (event == cv::EVENT_MOUSEMOVE)
		{
			if (onMove)
			{
				featureRect.width = x - featureRect.x;
				featureRect.height = y - featureRect.y;
			}
		}
		if (event == cv::EVENT_LBUTTONUP)
		{
			onMove = false;
			if (featureRect.width < 0)
			{
				featureRect.x += featureRect.width;
				featureRect.width *= -1;
			}
			if (featureRect.height < 0)
			{
				featureRect.y += featureRect.height;
				featureRect.height *= -1;
			}
			cv::rectangle(image, cv::Point(featureRect.x, featureRect.y),
				cv::Point(featureRect.x + featureRect.width, featureRect.y + featureRect.height), cv::Scalar(0, 0, 255));
			processFirst = false;
		}
	}
	else if (chooseVertex)
	{
		if (event == cv::EVENT_LBUTTONDOWN && pointCnt <= 4)
		{//左上开始顺时针
			cv::circle(image, cv::Point(x, y), 3, cv::Scalar(255, 0, 0), -1);
			vertexs.push_back(cv::Point(x, y));
			pointCnt++;
			if (pointCnt == 5)
			{
				pointCnt = 1;
			}
		}
	}
}

void getTargetImg(cv::Mat img,std::string path)
{
	
	cv::Mat temp;
	while (1) 
	{		
		img.copyTo(temp);		
		if (onMove)
		{
			cv::rectangle(temp, cv::Point(featureRect.x, featureRect.y),
				cv::Point(featureRect.x + featureRect.width, featureRect.y + featureRect.height), cv::Scalar(0, 0, 255));
		}
		if (cv::waitKey(10) == 27)
		{
			break;
		}
	}
	imwrite(path + "\\out.png", temp);
}


std::vector<cv::Point2f> Points(std::vector<cv::KeyPoint> keypoints)

{

	std::vector<cv::Point2f> res;

	for (unsigned i = 0; i < keypoints.size(); i++) 
	{
		res.push_back(keypoints[i].pt);

	}

	return res;

}


void drawBoundingBox(cv::Mat image, std::vector<cv::Point2f> bb)

{

	for (unsigned i = 0; i < bb.size() - 1; i++) 
	{
		line(image, bb[i], bb[i + 1], cv::Scalar(0, 255, 0), 2);

	}
	line(image, bb[bb.size() - 1], bb[0], cv::Scalar(0, 255, 0), 2);
	
}