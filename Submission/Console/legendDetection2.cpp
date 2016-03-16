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
#include <stdio.h>
// 

using namespace cv;
using namespace std;

string tessy(Mat legend) {
  // expand it around the edges
  Mat largerImage(Size(legend.cols*1.25,legend.rows*1.25),legend.type());
  largerImage = Scalar(255,255,255);
  legend.copyTo(largerImage(Rect(legend.cols*0.125,legend.rows*0.125,legend.cols,legend.rows)));
  imwrite("output.jpg",largerImage);
  char buf1[1000],buf2[1000];
  remove("temp.ppm");remove("temp.png");
  sprintf(buf1,"convert output.jpg temp.ppm");
  sprintf(buf2,"unpaper  --no-mask-center --no-border-align temp.ppm temp.png");
  system(buf1);
  system(buf2);
  system("tesseract temp.png dig_out -psm 7");
   char txt[1024];
  ifstream in;
  in.open("dig_out.txt");
  in.getline(txt, 1000, '\n');
  in.close();
  return string(txt);
}

// crops rectangle to stay within image.
Rect cropRect(Rect r, cv::Size size) {
  Point tl(max(r.tl().x, 0), max(r.tl().y, 0));
  Point br(min(r.br().x, size.width-1), min(r.br().y, size.height-1));
  return Rect(tl, br);
}


int main(int argc, char const *argv[]){
  // outputs image with legend removed. need to furtehr remove the legend box (if any) frm that.
  if (argc != 4) {
    printf("usage: ./legendDetection2 <uncolored> <original> <outname>\n");
    return 0;
  }
  Mat uc = imread(argv[1], 0);
  // crop from left and right a little just to remove those shitty axis points
  // Rect imgRect(Point(uc.cols*0.05,0), Point(uc.cols*0.95-1, uc.rows-1));
  // uc = uc(imgRect);
  // erode in y direction to connect all the legends, this would give us a region to segment later.
  Mat lgBox = uc.clone();
  // opening operation ftw
  morphologyEx( lgBox, lgBox, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(uc.cols*0.01, uc.rows*0.1)) );
  // choosing largest contour!
  cv::threshold(lgBox, lgBox,100, 255, CV_THRESH_BINARY_INV );
  std::vector<std::vector<cv::Point> > contours, rectContours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(lgBox, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
  if (contours.size() == 0) {
    // no contours, output 0
    printf("0\n");
    return 0;
  }
  float maxArea = 0;
  vector<Point> largest = contours[0];
  for (int i = 0; i < contours.size(); ++i)
  {
    float ctArea = contourArea(contours[i]);
    if (ctArea > maxArea) {
      maxArea = ctArea;
      largest = contours[i];
    }
  }
  Rect lgdRect = boundingRect(largest);
  Mat lgdImg = uc(lgdRect).clone();
  // open along x direction now
  morphologyEx( lgdImg, lgdImg, MORPH_OPEN, getStructuringElement(MORPH_RECT, Size(lgdImg.cols*0.2, 1)) );
  cv::threshold(lgdImg, lgdImg,100, 255, CV_THRESH_BINARY_INV );
  cv::findContours(lgdImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
  // remove contours whose width is less than 10% of box
  vector<vector<Point> > contours_;
  for (int i = 0; i < contours.size(); i++) {
    if (boundingRect(contours[i]).width >= 0.1*lgdImg.cols)
      contours_.push_back(contours[i]);
  }
  contours = contours_;
  printf("num Countours = %d\n", contours.size());
  Mat original = imread(argv[2]);
  Mat drawing = Mat::zeros(lgdImg.size(), CV_8UC3);
  vector<string> keyStrings;
  vector<Rect> keyRects;
  // vector<vector<Point> > dummy(1, largest);
  for (int i= 0; i < contours.size(); i++)
    drawContours(drawing, contours, i, Scalar(255,255,0), CV_FILLED, 8);
  // get text for each legend!
  for (int i= 0; i < contours.size(); i++) {
    Rect bb = boundingRect(contours[i]);
    Point margin(bb.width*0.05, bb.height*0.10);
    // add lgdImg offset, since we need to crop from the whole big image.
    Point tl(bb.tl()-margin+lgdRect.tl()), br(bb.br()+margin+lgdRect.tl());
    Rect keyRect(tl, br);
    keyRect = cropRect(keyRect, original.size());
    Mat legend = original(keyRect);
    keyStrings.push_back(tessy(legend));
    // append bb by lgdRect.tl()
    bb = Rect(bb.tl()+lgdRect.tl(), bb.br()+lgdRect.tl());
    keyRects.push_back(bb);
    // imshow("legend", legend);
    // waitKey();
  }
  FILE *f = fopen("legend_boxes.txt", "w");
  if (!f) {
    printf("could not open filE?\n");
    return 0;
  }
  fprintf(f, "%d\n", (int)keyStrings.size());
  for (int i = 0; i < keyStrings.size(); ++i)
  {
    Rect r = keyRects[i];
    fprintf(f, "%d %d %d %d\n", r.tl().x, r.br().x, r.tl().y, r.br().y);
    fprintf(f, "%s\n", keyStrings[i].c_str());
    // cout << "legend: " << keyStrings[i] << ", box " << keyRects[i] << endl;
  }
  fclose(f);
  // make a margin each side of the lgdRect, and make that shit WHITE, write it to new file.
  int wd = lgdRect.width*1.2;
  Rect toRemoveRect(lgdRect.tl()-Point(wd,0), lgdRect.br()+Point(wd,0));
  toRemoveRect = cropRect(toRemoveRect, original.size());
  Mat nolegend = original.clone();
  rectangle(nolegend, toRemoveRect, Scalar(255,255,255), CV_FILLED);
  imwrite(argv[3], nolegend);
  //imshow("eroded", drawing);
  // waitKey()s;
  return 0;
}