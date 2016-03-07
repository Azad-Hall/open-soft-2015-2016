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
using namespace std;
int main(int argc, char const *argv[])
{
  Mat img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  Mat imgHSV;
  cvtColor(img, imgHSV, CV_BGR2HSV);
  Mat mask(img.rows, img.cols, CV_8U, Scalar(1)), bmask = mask.clone(); // to mask away all colorless pixels
  // bmask = black mask. will dilate to remove some anti-aliased points
  for (int i = 0; i < mask.rows; ++i)
  {
    for (int j = 0; j < mask.cols; ++j)
    {
      mask.at<uchar>(i,j) = 1;
      bmask.at<uchar>(i,j) = 1;
      Vec3b cRGB = img.at<Vec3b>(i,j);
      Vec3b cHSV = imgHSV.at<Vec3b>(i,j);
      if (cRGB[0] >= 240 && cRGB[1] >= 240 && cRGB[2] >= 240) {
        mask.at<uchar>(i,j) = 0;
      } else if (cRGB[0] <= 30 && cRGB[1] <= 30 && cRGB[2] <= 30) {
        bmask.at<uchar>(i,j) = 0;
      }  // if saturation is low i.e less than 10%, it could be black/grey, we don't need that.
      else if (cHSV[1] <= .03*255) {
        bmask.at<uchar>(i,j) = 0;
      }      
    }
  }
  // erode(bmask, bmask, Mat());
  // mask = mask & bmask;
  mask = mask & bmask;
  // we will now classify with H value only.
  // make histogram on H
  vector<int> hist(256, 0);
  for (int i = 0; i < mask.rows; ++i)
  {
    for (int j = 0; j < mask.cols; ++j)
    {
      if (!mask.at<uchar>(i,j))
        continue;
      hist[imgHSV.at<Vec3b>(i,j)[0]]++;
    }
  }
  int maxH = 0;
  for (int i = 0; i < 256; ++i)
  {
    maxH = max(maxH, hist[i]);
  }
  // normalize histogram
  int hist_w = 512; int hist_h = 400;
  for (int i = 0; i < 256; ++i)
  {
    hist[i] = (hist[i] *hist_h)/maxH;
  }
  int bin_w = cvRound( (double) hist_w/256. );
  Mat histImg(hist_h, hist_w, CV_8U, Scalar(0));
  for (int i = 1; i < 256; i++) {
    line( histImg, Point( bin_w*(i-1), hist_h - cvRound(hist[i-1]) ) ,
                       Point( bin_w*(i), hist_h - cvRound(hist[i]) ),
                       Scalar( 255), 2, 8, 0  );
  }
  imshow("histogram", histImg);
  // draw all pixels with H < 50 degrees. Troublesome in 1.png?
  Mat yellow = img.clone();
  for (int i = 0; i < yellow.rows; ++i)
  {
    for (int j = 0; j < yellow.cols; ++j)
    {
      if (!mask.at<uchar>(i,j))
        yellow.at<Vec3b>(i,j) = Vec3b(0,0,0);
      // else
      //   yellow.at<Vec3b>(i,j) = Vec3b(0,0,0);

      // if (imgHSV.at<Vec3b>(i,j)[0] >= 50/2.) // since H = deg/2 to fit in 255
      //   yellow.at<Vec3b>(i,j) = Vec3b(0,0,0);
    }
  }
  imshow("weird", yellow);
  imwrite("weird2.png", yellow);
  waitKey();
  return 0;
}