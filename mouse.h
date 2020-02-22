#pragma once
#include<opencv2/opencv.hpp>

bool onMove = false;
cv::Rect featureRect;
void mouse(int event, int x, int y, int flags, void* param)
{
	cv::Mat img = *(cv::Mat*) param;
	if (event == cv::EVENT_LBUTTONDOWN)
	{
		onMove = true;
		featureRect = cv::Rect(x, y, 0, 0);
	}
	if (event == cv::EVENT_MOUSEMOVE)
	{
		if (onMove)
		{
			featureRect.width = abs(x - featureRect.x);
			featureRect.height = abs(y - featureRect.y);
		}
	}
	if (event == cv::EVENT_LBUTTONUP)
	{
		onMove = false;
		featureRect.x = std::min(x, featureRect.x);
		featureRect.y = std::min(y, featureRect.y);
		cv::rectangle(img, cv::Point(featureRect.x, featureRect.y),
			cv::Point(featureRect.x + featureRect.width, featureRect.y + featureRect.height), cv::Scalar(0, 0, 255));
	}
}


