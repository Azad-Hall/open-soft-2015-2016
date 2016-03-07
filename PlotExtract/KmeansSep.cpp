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
	Color operator+(const Color &c) const {
		return Color(r+c.r, g+c.g, b+c.b);
	}
	Color &operator+=(const Color &c) {
		*(this) = *(this)+c;
		return *this;
	}
	Color operator/(int x) const {
		assert (x != 0);
		return Color(r/x, g/x, b/x);
	}
	Color &operator/=(int x) {
		return (*this) = (*this)/x;
	}
	Color operator*(int x) const {
		return Color(r*x, g*x, b*x);
	}
	Color &operator*=(int x) {
		return (*this) = (*this)*x;
	}
};

struct ColorClass {
	Color avgColor;
	Color qColor;
	int numPixels;
	ColorClass(): avgColor(), qColor(), numPixels(0) {}
	ColorClass(Color qColor) : avgColor(), qColor(qColor), numPixels(0) {}
	bool operator<(const ColorClass &c) const {
		return qColor < c.qColor;
	}
};

struct MergedColorClass {
	Color avgColor;
	vector<Color> qColors;
	int numPixels;
	MergedColorClass(): avgColor(Vec3b(0,0,0)), numPixels(0) {}
};



using namespace std;
using namespace cv;


bool sortFunc(const pair<Color, int> &p1, const pair<Color, int> &p2) {
	return p1.second > p2.second;
}

void colorReduce(cv::Mat& image, int div =32)
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

int colorDistSq (const Color &a, const Color &b) {
	return (a.r-b.r)*(a.r-b.r) + (a.g-b.g)*(a.g-b.g) + (a.b-b.b)*(a.b-b.b);
}


int main(int argc, char **argv){
	int d=25;
	Mat img1 = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	Mat img2 = img1.clone();
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
	map<Color, ColorClass> histogram;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			::Color c(img.at<Vec3b>(i,j));
			if (histogram.find(c) == histogram.end())
				histogram[c] = ColorClass(c);
			histogram[c].avgColor = histogram[c].avgColor + Color(img1.at<Vec3b>(i,j));
			histogram[c].numPixels++;
		}
	}
	vector<ColorClass> classes;
	
	cout << "map elements = " << histogram.size() << endl;
	for (map<Color, ColorClass>::reverse_iterator it = histogram.rbegin(); it != histogram.rend(); it++) {
		ColorClass c = it->second;
		assert(c.numPixels > 0);
		c.avgColor /= c.numPixels;
		classes.push_back(c);
		Color &avg = c.avgColor;
		printf("qColor: (%d, %d, %d), avgColor = (%d, %d, %d), numPixels = %d\n", c.qColor.r, c.qColor.g, c.qColor.b, 
			avg.r, avg.g, avg.b, c.numPixels);
	}

	int distThreshSq = 80*80;
	vector<bool> classDone(classes.size(), false);
	vector<MergedColorClass> mergedClasses;
	// remove white and black. they are the first and last class respectivley.
	classDone[0] = true;
	classDone[classDone.size()-1] = true;
	for (int i = 0; i < classes.size(); ++i)
	{
		if (classDone[i])
			continue;
		classDone[i] = true;
		MergedColorClass mc;
		mc.qColors.push_back(classes[i].qColor);
		// right now avg holds the sum.
		mc.numPixels = classes[i].numPixels;
		mc.avgColor = classes[i].avgColor*mc.numPixels;
		for (int j = i+1; j < classes.size(); ++j) {
			if (classDone[j])
				continue;
			int distSq = colorDistSq(classes[i].avgColor, classes[j].avgColor);
			if (distSq < distThreshSq) {
				// merge it
				classDone[j] = true;
				mc.qColors.push_back(classes[j].qColor);
				mc.numPixels += classes[j].numPixels;
				mc.avgColor += classes[j].avgColor * classes[j].numPixels;
			}
		}
		mc.avgColor /= mc.numPixels;
		mergedClasses.push_back(mc);
	}
	printf("merged classes=  %d\n", mergedClasses.size());
	for (int i = 0; i < mergedClasses.size(); ++i)
	{
		MergedColorClass &mc = mergedClasses[i];
		for (int j= 0; j < mc.qColors.size();j++) {
			printf("(%d, %d, %d), ", mc.qColors[j].r, mc.qColors[j].g, mc.qColors[j].b);
		}
		printf(". avg Color = %d, %d, %d. Num Pixels = %d\n", mc.avgColor.r, mc.avgColor.g, mc.avgColor.b, mc.numPixels);
	}	
	cv::waitKey();


	//waitKey(0);

	return 0;

}  //*/