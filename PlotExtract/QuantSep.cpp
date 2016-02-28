#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
//#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <fstream>
#include <math.h>
#include <vector>
#include <map>

using namespace cv;
struct Color {
	int r, g, b;
	Color(int r, int g, int b) : r(r), g(g), b(b) {}
	Color(Vec3b c) : r(c[0]), g(c[1]), b(c[2]) {}
	Color() : r(0), g(0), b(0) {}
	bool operator<(const Color& c) const {
		return (r + (g << 8) + (b << 16)) < (c.r + (c.g << 8) + (c.b << 16));
	}
};


using namespace std;
using namespace cv;

typedef struct R{
	int l;
	int r;
}range;
bool sortFunc(const pair<Color, int> &p1, const pair<Color, int> &p2) {
	return p1.second > p2.second;
}

void colorReduce(cv::Mat& image, int div =140)
{
	int nl = image.rows;                    // number of lines
	int nc = image.cols * image.channels(); // number of elements per line

	for (int j = 0; j < nl; j++)
	{
		// get the address of row j
		uchar* data = image.ptr<uchar>(j);

		for (int i = 0; i < nc; i++)
		{
			// process each pixel
			data[i] = data[i] / div * div + div / 2;
		}
	}
}



int main(){
	int d=25;
	Mat img1 = imread("./pics/1.png", CV_LOAD_IMAGE_COLOR);
	Mat img2 = img1;
	colorReduce(img2);
	cv::imshow("colorReduce", img2);


	/*std::vector<cv::Mat> imgRGBs;
	cv::split(img2, imgRGBs);
	int ks = 5;
	int ns = img2.rows *img2.cols;
	cv::Mat img3xNs(ns, 3, CV_8U);
	for (int i = 0; i != 3; ++i)
		imgRGBs[i].reshape(1, ns).copyTo(img3xNs.col(i));
	img3xNs.convertTo(img3xNs, CV_32F);
	cv::Mat bestLabless;
	cv::kmeans(img3xNs, ks, bestLabless, cv::TermCriteria(), 10, cv::KMEANS_RANDOM_CENTERS);
	bestLabless = bestLabless.reshape(0, img2.rows);
	cv::convertScaleAbs(bestLabless, bestLabless, int(255 / ks));
	//cv::imshow("pre-K-split", bestLabless);
	// cv::waitKey();
	// waitKey(0);
	Mat img=bestLabless;*/

	Mat img=img2;



	namedWindow("img", WINDOW_AUTOSIZE);
	imshow("img", img);
	map<Color, int> histogram;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			::Color c(img.at<Vec3b>(i,j));
			if (histogram.find(c) == histogram.end())
				histogram[c] = 0;
			histogram[c]++;
		}
	}

	int maxCount = 20;
	int cnt = 0;
	cout << "map elements = " << histogram.size() << endl;
	vector<pair<Color, int> > output;
	for (map<Color, int>::reverse_iterator it = histogram.rbegin(); it != histogram.rend(); it++) {
		output.push_back(make_pair(it->first, it->second));
	}

	sort(output.begin(), output.end(), sortFunc);
	for (int i = 0; i < 20 && i<output.size(); i++) {
		cout << "count = " << output[i].second << "color : " << output[i].first.r << " " << output[i].first.g << " " <<
			output[i].first.b << endl;
	}

	/*Mat temp = Mat(img.rows,img.cols, CV_8UC3);
	for(int i=0;i<output.size();i++)
	{
		//int i=5;
		temp=Scalar(output[i].first.r,output[i].first.g,output[i].first.b);
		imshow("colours",temp);
		waitKey(0);

	}*/



	imwrite("imgout.png",img);
	//Mat temp = Mat(img.rows,img.cols, CV_8UC1);
	//temp=Scalar(0);
	int count=0;
	for(int i=0;i<output.size();i++)
	{
		//int i=2;
		Mat temp = Mat(img.rows,img.cols, CV_8UC3);
		Vec3b comparator;//= Scalar(output[i].first.r,output[i].first.g,output[i].first.b);
		comparator[0]=output[i].first.r;
		comparator[1]=output[i].first.g;
		comparator[2]=output[i].first.b;
		cout <<"color : " << output[i].first.r << " " << output[i].first.g << " " <<
			output[i].first.b << endl;
		for(int j=0;j<img.rows;j++)
		{
			for(int k=0;k<img.cols;k++)
			{
				//::Color c(img.at<Vec3b>(i,j));
				//cout<<"Comparator : ("<<comparator[0]<<","<<comparator[1]<<","<<comparator[2]<<")\n";
				//cout<<"Img values : ("<<img.at<Vec3b>(i,j)<<")\n";
				if(comparator == img.at<Vec3b>(j,k))
				{
					count++;
					temp.at<Vec3b>(j,k)[0]=255;//img.at<Vec3b>(j,k);
					temp.at<Vec3b>(j,k)[1]=255;
					temp.at<Vec3b>(j,k)[2]=255;
				}
			}
		}
		imshow("temp", temp);
		//imshow("temp img", img);
		
		cout<<"count="<<count<<"\n";
		waitKey(0);
		//temp = Scalar(0);
		count=0;


	}
	


	std::vector<cv::Mat> imgRGB;
	cv::split(img, imgRGB);
	int k = 4;
	int n = img.rows *img.cols;
	cv::Mat img3xN(n, 3, CV_8U);
	for (int i = 0; i != 3; ++i)
		imgRGB[i].reshape(1, n).copyTo(img3xN.col(i));
	img3xN.convertTo(img3xN, CV_32F);
	cv::Mat bestLables;
	cv::kmeans(img3xN, k, bestLables, cv::TermCriteria(), 10, cv::KMEANS_RANDOM_CENTERS);
	bestLables = bestLables.reshape(0, img.rows);
	cv::convertScaleAbs(bestLables, bestLables, int(255 / k));
	cv::imshow("result", bestLables);

	colorReduce(bestLables);
	cv::imshow("result reduced", bestLables);




	map<Color, int> histogram2;
	for (int i = 0; i < bestLables.rows; i++) {
		for (int j = 0; j < bestLables.cols; j++) {
			::Color c(bestLables.at<Vec3b>(i,j));
			if (histogram2.find(c) == histogram2.end())
				histogram2[c] = 0;
			histogram2[c]++;
		}
	}

	cout << "map elements = " << histogram2.size() << endl;
	vector<pair<Color, int> > output2;
	for (map<Color, int>::reverse_iterator it = histogram2.rbegin(); it != histogram2.rend(); it++) {
		output2.push_back(make_pair(it->first, it->second));
	}

	sort(output2.begin(), output2.end(), sortFunc);
	for (int i = 0; i < 20 && i<output2.size(); i++) {
		cout << "count2 = " << output2[i].second << "color2 : " << output2[i].first.r << " " << output2[i].first.g << " " <<
			output2[i].first.b << endl;
	}


	/*//Mat temp = Mat(bestLables.rows,bestLables.cols, CV_8UC1);
	temp=Scalar(0);
	int count=0;
	for(int i=2;i<output.size();i++)
	{
		//int i=2;
		Mat temp = Mat(bestLables.rows,img.cols, CV_8UC1);
		Vec3b comparator;//= Scalar(output[i].first.r,output[i].first.g,output[i].first.b);
		comparator[0]=output[i].first.r;
		comparator[1]=output[i].first.g;
		comparator[2]=output[i].first.b;
		cout <<"color : " << output[i].first.r << " " << output[i].first.g << " " <<
			output[i].first.b << endl;
		for(int j=0;j<img.rows;j++)
		{
			for(int k=0;k<img.cols;k++)
			{
				//::Color c(img.at<Vec3b>(i,j));
				//cout<<"Comparator : ("<<comparator[0]<<","<<comparator[1]<<","<<comparator[2]<<")\n";
				//cout<<"Img values : ("<<img.at<Vec3b>(i,j)<<")\n";
				if(comparator == img.at<Vec3b>(j,k))
				{
					count++;
					temp.at<Vec3b>(j,k)=255;
				}
			}
		}
		imshow("temp", temp);
		imshow("temp img", img);
		
		cout<<"count="<<count<<"\n";
		waitKey(0);
		//temp = Scalar(0);
		count=0;


	}
*/

	/*for(int i=0;i<bestLables.rows;i++)
	{
		for(int j=0;j<bestLables.cols;j++)
		{
			cout<<bestLables.at<Vec3b>(i,j)<<"\n";
		}
	}*/
	cv::waitKey();


	//waitKey(0);

	return 0;

}  //*/