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
#include <string>
// #include </home/aytas32/OpenSoft/inter_hall_16/ocr/peakdetect.cpp>


using namespace cv;
using namespace std;

int getHues(int hue,vector<pair<int,int>> hues){
	int i;
	for(i=0;i<hues.size();i++){
		if(hue>hues[i].first && hue<hues[i].second)
			return i;
	}
	return i;
}

int legendNos(Mat imgHSV,vector<pair<int,int>> hues,vector<pair<int,int>> boxesX,vector<pair<int,int>> boxesY){
	//no of plots > no of boxes means increase params
	//else if  no of plots < no of boxes means decrease params
	//else finding the pair of corresponding text box and hue range
	vector<int> boxHueLeft,boxHueRight;
	//for left
	for(int i=0;i<boxesX.size();i++){
		vector<int> carts(hues.size()+1,0);
		for(int i=boxesX[i].first;i>0&&i>boxesX[i].first-20;i--){
			for(int j=boxesY[i].first;j<imgHSV.cols && j<boxesY[i].second;j++){
				int hue=imgHSV.at<Vec3b>(i,j)[0];
				++carts[getHues(hue,hues)];

			}
		}
		int maxPos=0;
		for(int i=0;i<carts.size()-1;i++){
			if(carts[i]>carts[maxPos])
				maxPos=i;
		}
		boxHueLeft.push_back(maxPos);
	}
	//check for uniqueness
	//duplicate values means no of plots is 

	for(int i=0;i<boxesX.size();i++){
		vector<int> carts(hues.size()+1,0);
		for(int i=boxesX[i].first;i<imgHSV.cols &&i<boxesX[i].first+20;i++){
			for(int j=boxesY[i].first;j<imgHSV.cols && j<boxesY[i].second;j++){
				int hue=imgHSV.at<Vec3b>(i,j)[0];
				++carts[getHues(hue,hues)];

			}
		}
		int maxPos=0;
		for(int i=0;i<carts.size()-1;i++){
			if(carts[i]>carts[maxPos])
				maxPos=i;
		}
		boxHueRight.push_back(maxPos);
	}
}