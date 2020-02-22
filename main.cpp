#include <opencv2/opencv.hpp>
#include <chrono>
#include "coarse_to_fine_patchmatch.h"
#include"preprocess.h"
const double ransac_thresh = 3.0f;  // 随机序列采样 内点阈值 
const std::string in_file_path = "D:\\FILES\\V";
const std::string out_file_path = "D:\\FILES\\Vout\\V";
void drawOpticalFlow(cv::Mat& img, const cv::Mat4f& matches, float maxnorm = -1)
{
	const int w = img.cols;
	const int h = img.rows;
	const int nmatches = matches.rows;

	if (maxnorm < 0)
	{
		for (int i = 0; i < nmatches; i++)
		{
			const cv::Vec4f& match = matches(i);
			const float fx = match[2] - match[0];
			const float fy = match[3] - match[1];
			maxnorm = std::max(maxnorm, std::hypotf(fx, fy));
		}
	}

	const float INV_2PI = static_cast<float>(1 / CV_2PI);
	const int radius = 1;
	for (int i = 0; i < nmatches; i++)
	{
		const cv::Vec4f& match = matches(i);
		const float fx = match[2] - match[0];
		const float fy = match[3] - match[1];

		// convert flow angle to hue
		float angle = INV_2PI * atan2f(fy, fx);
		if (angle < 0.f) angle += 1.f;
		const uchar hue = static_cast<uchar>(180 * angle);

		// convert flow norm to saturation
		const float norm = std::hypotf(fx, fy) / maxnorm;
		const uchar sat = static_cast<uchar>(255 * norm);

		// draw each match as a 3x3 color block
		for (int dy = -radius; dy <= radius; dy++)
		{
			for (int dx = -radius; dx <= radius; dx++)
			{
				const int x = std::max(0, std::min(static_cast<int>(match[0] + dx + 0.5f), w - 1));
				const int y = std::max(0, std::min(static_cast<int>(match[1] + dy + 0.5f), h - 1));
				img.ptr<cv::Vec3b>(y)[x] = cv::Vec3b(hue, sat, 255);
			}
		}
	}

	cv::cvtColor(img, img, cv::COLOR_HSV2BGR);
}
void setName(std::string& in,std::string& out,int i,int j)
{
	if (i <= 9)
	{
		in = in_file_path + "0" + std::to_string(i) + "\\V0"
			+ std::to_string(i) + "_" + std::to_string(j) + ".avi";
		out = out_file_path + "0" + std::to_string(i) + "\\out_V0" +
			std::to_string(i) + "_" + std::to_string(j) + ".avi";
	}

	else if (i >= 10)
	{
		in = in_file_path + std::to_string(i) + "\\V"
			+ std::to_string(i) + "_" + std::to_string(j) + ".avi";
		out = out_file_path + std::to_string(i) + "\\out_V" +
			std::to_string(i) + "_" + std::to_string(j) + ".avi";
	}
}
void cpmProcess(cv::Mat& img1,cv::Mat& img2,
	std::vector<cv::Point2f>& points1, 
	std::vector<cv::Point2f>& points2)
{
	CoarseToFinePatchMatch cpm;
	const cv::Mat4f matches = cpm.compute(img1, img2);
	for (int i = 0; i < matches.rows; i++)
	{
		const cv::Vec4f& match = matches(i);
		points1.push_back(cv::Point2f(match[0], match[1]));
		points2.push_back(cv::Point2f(match[2], match[3]));
	}
}
int main()
{
	for (int i = 9; i <= 9; i++)
	{
		for (int j = 1; j <= 7; j++)
		{
			std::string in_file_name,out_file_name;
			setName(in_file_name,out_file_name,i,j);
			cv::VideoCapture capture;
			capture.open(in_file_name);
			double fps = (int)capture.get(cv::CAP_PROP_FPS);
			cv::Size frameSize = cv::Size((int)capture.get(cv::CAP_PROP_FRAME_WIDTH), (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT));
			cv::VideoWriter writer;
			writer.open(out_file_name, cv::VideoWriter::fourcc('M', 'P', '4', '2'), fps, frameSize);
			//CV_Assert(writer.isOpened()&&capture.isOpened());
			if (!capture.isOpened())
			{
				std::cerr << "video not open" << std::endl;
			}
			if (!writer.isOpened())
			{
				std::cerr << "out video not open" << std::endl;
			}
			//处理第一帧
			cv::Mat firstImg,lastImg;
			capture >> firstImg;
			cv::cvtColor(firstImg, lastImg, cv::COLOR_BGR2GRAY);
			cv::namedWindow("capture");
			cv::setMouseCallback("capture", mouse, (void*)&firstImg);
			while (!vertexs.empty()) vertexs.pop_back();
			while (1)
			{
				cv::Mat tmp;
				firstImg.copyTo(tmp);
				if (onMove)
				{
					cv::rectangle(tmp, cv::Point(featureRect.x, featureRect.y),
						cv::Point(featureRect.x + featureRect.width, featureRect.y + featureRect.height), cv::Scalar(0, 0, 255));
				}
				cv::imshow("capture", tmp);
				if (cv::waitKey(10) == 27)
				{
					cv::destroyWindow("capture");
					break;
				}
			}
			processFirst = true;
			int cnt = 2;
			
			std::vector<cv::Point2f> new_vertexs;
			std::cout << out_file_name << std::endl;
			const auto t1 = std::chrono::system_clock::now();
			while (capture.isOpened() && cnt <= 501)
			{
				std::cout << "the " << cnt << "th frame" << std::endl;
				
				std::vector<cv::Point2f> points1,points2;
				cv::Mat curImg,tmp;
				capture >> tmp;
				cv::cvtColor(tmp, curImg, cv::COLOR_BGR2GRAY);
				
				cpmProcess(lastImg, curImg, points1, points2);
				cv::Mat homo = cv::findHomography(points1, points2,
					cv::RANSAC, ransac_thresh);
				while (!new_vertexs.empty()) new_vertexs.pop_back();
				cv::perspectiveTransform(vertexs, new_vertexs, homo);
				drawBoundingBox(tmp, new_vertexs);
				writer << tmp;
				//cv::imshow("tmp", tmp);
				//cv::waitKey();			
				//CoarseToFinePatchMatch cpm;
				//const cv::Mat4f matches = cpm.compute(lastImg, curImg);
				//cv::Mat draw = cv::Mat::zeros(lastImg.size(), CV_8UC3);
				//drawOpticalFlow(draw, matches);
				//writer << draw;
				cnt++;
			}
			const auto t2 = std::chrono::system_clock::now();
			const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
			std::cout << "The " << i << " group " << j << " video has done" << std::endl;
			std::cout << "cpm computation time: " << duration << "[msec]" << std::endl;
		}
	}
	return 0;
}