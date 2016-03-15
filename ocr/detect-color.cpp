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


using namespace cv;
using namespace std;
typedef unsigned char uchar;


// expands black mask using a looser threshold for saturation to be considered black (eg. 20%)
void traverse(Mat &bmask, Mat &visited, Mat &imgHSV, int i, int j, Mat &bmaskNew, Mat &whiteMask, int lowSatThresh) {
  int iArr[] = {-2, -1, 0, 1, 2};
  for (int k = 0; k < 5; k++) {
    for (int l = 0; l < 5; l++) {
      int ii = i+iArr[k], jj = j+iArr[l];
      if (ii < 0 || ii >= bmask.rows || jj < 0 || jj >= bmask.cols)
        continue;
      if (ii == i && jj == j)
        continue;
      Vec3b hsv = imgHSV.at<Vec3b>(ii,jj);
      if (visited.at<uchar>(ii,jj))
        continue;
      visited.at<uchar>(ii,jj) = 1;
      // add special conditions for brown and blue to counter anti aliasing
      if ((hsv[1] <= lowSatThresh || fabs(hsv[0]-30) < 20 || fabs(hsv[0]-200) < 20 || fabs(hsv[0]-216) < 8) && whiteMask.at<uchar>(ii,jj) ) {
        visited.at<uchar>(ii,jj) = 1;
        bmaskNew.at<uchar>(ii,jj) = 0;
        traverse(bmask, visited, imgHSV, ii, jj, bmaskNew, whiteMask, lowSatThresh);
      }
    }
  }
}
Mat expandBMask(Mat bmask, Mat imgHSV, Mat whiteMask, int lowSatThresh) {
  Mat visited = Mat::zeros(bmask.size(), CV_8U);
  Mat bmaskNew = Mat::ones(bmask.size(), CV_8U);
  for (int i = 0; i < bmask.rows; ++i)
  {
    for (int j = 0; j < bmask.cols; ++j)
    {
      if (bmask.at<uchar>(i,j) == 0)
        bmaskNew.at<uchar>(i,j) = 0;
      if (bmask.at<uchar>(i,j) == 0 && visited.at<uchar>(i,j) == 0) {
        visited.at<uchar>(i,j) = 1;
        traverse(bmask, visited, imgHSV, i, j, bmaskNew, whiteMask, lowSatThresh);
      }
    }
  }
  return bmaskNew;
}

bool detect_color(Mat img)
{
  Mat imgHSV;
  cvtColor(img, imgHSV, CV_BGR2HSV);
  Mat mask(img.rows, img.cols, CV_8U, Scalar(255)), bmask = mask.clone(); // to mask away all colorless pixels
  // bmask = black mask. will dilate to remove some anti-aliased points
  for (int i = 0; i < mask.rows; ++i)
  {
    for (int j = 0; j < mask.cols; ++j)
    {
      Vec3b cRGB = img.at<Vec3b>(i,j);
      Vec3b cHSV = imgHSV.at<Vec3b>(i,j);
      if (cRGB[0] >= 210 && cRGB[1] >= 210 && cRGB[2] >= 210) {//210
        mask.at<uchar>(i,j) = 0;
      } else if (cRGB[0] <= 120 && cRGB[1] <= 120 && cRGB[2] <= 120) {
        bmask.at<uchar>(i,j) = 0;
      }  // if saturation is low i.e less than 10%, it could be black/grey, we don't need that.
      else if (cHSV[1] <= .1*255) {
        bmask.at<uchar>(i,j) = 0;
      }      
    }
  }
  //anti aliasing correction
  // using search method
  //again adding all black pixels to bmask for higher threshold of saturation
  bmask = expandBMask(bmask, imgHSV, mask, 0.15*255);//0.11
  mask = mask & bmask;
  int count = 0;
   for(int i = 0 ; i < mask.rows ; ++i ){
   		for(int j = 0 ; j < mask.cols ; ++j ){
   				if (mask.at<uchar>(i,j) >= 1){
   					mask.at<uchar>(i,j) = 255;
   					count++;
   				}
   		}
   }
  bool isGraph = false;
 	if( count > (mask.rows * mask.cols) * 0.01)
 		isGraph = true;
 	return isGraph;
 	
}

// int main(int argc, char const *argv[])
// {
  
//   Mat img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
//   //cout << img.size() << endl;
//   cout<<detect_color(img)<<endl;
//  //cout << isGraph << endl;

// //namedWindow("my window", CV_WINDOW_NORMAL);
// 	//imshow("my window", mask);
// 	//waitKey(0);
// 	return 0;
// }