#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <math.h>

using namespace cv;
Mat thresh(Mat& img, int val) {
	// for thresholding black
	Mat dest(img.rows, img.cols, CV_8U);
	for (int i = 0; i < dest.rows; ++i)
	{
		for (int j = 0; j < dest.cols; ++j)
		{
			Vec3b clr = img.at<Vec3b>(i,j);
			if (clr[0] < val && clr[1] < val && clr[2] < val)
				dest.at<unsigned char>(i,j) = 255;
			else
				dest.at<unsigned char>(i,j) = 0;
		}
	}
	return dest;
}
int main(int argc, char const *argv[])
{
	cv::Mat input = cv::imread(argv[1]);//input image
	// resize(input, input, Size(input.cols/4, input.rows/4));
	float a=input.cols*input.rows;
	
	cv::Mat gray;
	cv::cvtColor(input, gray, CV_BGR2GRAY);
	int max = 1;
	namedWindow("input");
	createTrackbar("Threshold", "input",&max,100);
	// cv::Mat mask = thresh(input, 120);
	// imshow("bw", mask);
	cv::Mat mask;
	cv::threshold(gray, mask,240, 255, CV_THRESH_BINARY_INV );
	imwrite("/tmp/tmp-bw.png",  mask);
	// Canny(mask, mask, 20, 100, 3);
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int biggestContourIdx = -1;
	float biggestContourArea = 0;
	while (1){
		cv::Mat drawing = cv::Mat::zeros(mask.size(), CV_8UC3);
		int numContours = 0;
		for (int i = 0; i < contours.size(); i++)
		{
			float ctArea = cv::contourArea(contours[i]);
			cv::Rect boundingBox = cv::boundingRect(contours[i]);
			float percentRect = ctArea / (float)(boundingBox.height * boundingBox.width);
			assert (percentRect >= 0 && percentRect <= 1.0);
			// printf("percentRect = %f\n", percentRect);
			if (percentRect < 80/100.)
				continue;
			cv::Scalar color = cv::Scalar(0, 255, 0);
			drawContours(drawing, contours, i, color, 1, 8, hierarchy, 0, cv::Point());
			numContours++;
			// if (ctArea > a / max || 1){
			// 	cv::Rect boundingBox = cv::boundingRect(contours[i]);
			// 	cv::Point2f corners[4];
			// 	//boundingBox.points(corners);
			// 	corners[0] = boundingBox.tl();
			// 	corners[1] = Point2f(boundingBox.tl().x + boundingBox.width, boundingBox.tl().y);
			// 	corners[2] = boundingBox.br();
			// 	corners[3] = Point2f(boundingBox.br().x - boundingBox.width, boundingBox.br().y);
			// 	cv::line(drawing, corners[0], corners[1], cv::Scalar(255, 255, 255));
			// 	cv::line(drawing, corners[1], corners[2], cv::Scalar(255, 255, 255));
			// 	cv::line(drawing, corners[2], corners[3], cv::Scalar(255, 255, 255));
			// 	cv::line(drawing, corners[3], corners[0], cv::Scalar(255, 255, 255));
			// 	cv::Rect myROI(corners[0], corners[2]);
			// 	cv::Mat croppedImage = input(myROI);
			// 	char buf[1000];
			// 	sprintf(buf, "%d", i);
			// 	cv::imshow(buf, croppedImage);
			// 	/*cv::imwrite("rotatedRect.png", drawing);*/
			// }
		}
		printf("numContours = %d\n", numContours);
		imwrite("/tmp/tmp-drawing.png", drawing);
		cv::imshow("input", input);
		cv::imshow("drawing", drawing);
		break;
		if (waitKey(30) == 27) break;
	}
	cv::waitKey(0);
	return 0;
}