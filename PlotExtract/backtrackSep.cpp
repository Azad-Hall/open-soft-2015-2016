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
	void addAvg(Vec3b c,int count)
	{
		r= (r*(count-1) +c[0])/count;
		g= (g*(count-1) +c[0])/count;
		b= (b*(count-1) +c[0])/count;
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

void colorReduce(cv::Mat& image, int div =64)
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

bool inThreshold(Vec3b a,Vec3b b,int thresh)
{
	if(a[0] +thresh<b[0] || a[0]-thresh>b[0] )
		return 0;
	if(a[1] +thresh<b[1] || a[1]-thresh>b[1] )
		return 0;
	if(a[1] +thresh<b[1] || a[1]-thresh>b[1] )
		return 0;
	return 1;
	//cout<<"in function: ("<<a[0]<<","<<a[1]<<","<<a[2]<<")\n";
	/*if(a[0]==b[0] && a[1]==b[1] && a[2]==b[2])
		return true;
	return false;*/
}


int main(){
	int d=25;
	Mat img1 = imread("./pics/4.png", CV_LOAD_IMAGE_COLOR);
	cv::imshow("actual", img1);
	Mat img2;
	img1.copyTo(img2);
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



	//*******Calculating average of clusters corresponding to original image
	vector<pair<Color, Color> > backtrack;
	int count;
	for(int i=0;i<output.size();i++)
	{
		Vec3b comparator=Vec3b(output[i].first.r,output[i].first.g,output[i].first.b);//= Scalar(output[i].first.r,output[i].first.g,output[i].first.b);
		count=0;
		Color OrigAvg(0,0,0);
		cout <<"color : " << output[i].first.r << " " << output[i].first.g << " " <<
			output[i].first.b << endl;
		for(int j=0;j<img.rows;j++)
		{
			for(int k=0;k<img.cols;k++)
			{
				if(comparator == img.at<Vec3b>(j,k))
				{

					count++;
					OrigAvg.addAvg(img1.at<Vec3b>(j,k),count);

					
				}
			}
		}
		backtrack.push_back(make_pair(output[i].first,OrigAvg));
		cout<<"OrigAvg : ("<<OrigAvg.r<<","<<OrigAvg.g<<","<<OrigAvg.b<<")\n";
		cout<<"count="<<count<<"\n";
	}
	waitKey(0);



	cout<<"Showing image backtracked seperation***********************************\n";
	Mat temp = Mat(img1.rows,img1.cols, CV_8UC3);
	for(int i=1;i<backtrack.size();i++)
	{
		//int i=2;
		temp=Scalar(0,0,0);
		count=0;
		Vec3b comparator=Vec3b(backtrack[i].second.r,backtrack[i].second.g,backtrack[i].second.b);//= Scalar(output[i].first.r,output[i].first.g,output[i].first.b);
		int threshold=50;
		cout<<"OrigAvg : ("<<backtrack[i].second.r<<","<<backtrack[i].second.g<<","<<backtrack[i].second.b<<")\n";
		for(int j=0;j<img1.rows;j++)
		{
			for(int k=0;k<img1.cols;k++)
			{
				if(inThreshold(comparator,img1.at<Vec3b>(j,k),threshold))
				{
					//cout<<inThreshold(comparator,img1.at<Vec3b>(i,j),threshold)<<endl;
					//cout<<inThreshold(comparator,img1.at<Vec3b>(i,j),threshold)<<endl;
					count++;
					temp.at<Vec3b>(j,k)[0]=255;
					temp.at<Vec3b>(j,k)[1]=255;
					temp.at<Vec3b>(j,k)[2]=255;
				}
				//else
					//cout<<"Values : "<<comparator<<","<<img.at<Vec3b>(j,k)<<","<<inThreshold(comparator,img1.at<Vec3b>(i,j),threshold)<<endl;
					
			}
		}
		imshow("temp", temp);
		imshow("img1",img1);
		cout<<"count="<<count<<"\n";
		waitKey(0);
		
		


	}



	/*cout<<"Showing image  seperation***********************************\n";
	imwrite("imgout.png",img);
	//Mat temp = Mat(img.rows,img.cols, CV_8UC1);
	//temp=Scalar(0);
	//int count=0;
	temp = Mat(img.rows,img.cols, CV_8UC3);
		
	for(int i=0;i<output.size();i++)
	{
		//int i=2;
		temp=Scalar(0,0,0);
		count=0;
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
		


	}*/
	


	

	
	cv::waitKey();


	//waitKey(0);

	return 0;

}  //*/