// for detecting the graph's bounding box inside a single graph's image.
// outputs a reduced box.
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <math.h>
#include <algorithm>


using namespace cv;
using namespace std;
#include "box-detection.hpp"


// returns finalContour and cropped image which is shrunk

vector<Point> get_outer_box(Mat& input, Mat& cropped)
{
  Mat contourImg = input.clone();
  vector<vector<Point> > contours = getBoxes(input, input.rows/4);
  if (contours.size() == 0) {
    printf("no contours.?\n");
    return vector<Point>();
  }
  for (int i =0; i < contours.size(); i++) {
    drawContours(contourImg, contours, i, Scalar(255,0,0), 1, 8);
  }
  imwrite("tmp/contours.png", contourImg);
  // get max area contour
  vector<Point> largest = contours[0];
  int maxArea = contourArea(largest);
  for (int i = 1; i < contours.size(); i++) {
    int area = contourArea(contours[i]);
    if (area > maxArea) {
      maxArea = area;
      largest = contours[i];
    }
  }
  vector<Point> finalContour = getRectangularContour2(largest);
  // need to shrink alittle, since we don't want the black boundary to be there
  // in the output image.
  // shirnk by 7% contour height, just to remove all the  black ticks
  // can't do more than 4 else sometimes cuts other image :(
  vector<Point> shrinkedContour = shrinkContour(finalContour, 0.04*boundingRect(finalContour).height);
  // don't crop the image, just make everything outside contour white.
  cropped = input.clone();
  for (int i =0; i < cropped.rows; i++) {
    for (int j = 0; j < cropped.cols; j++) {
      if (pointPolygonTest(shrinkedContour, Point(j,i), false) < 0) {
        cropped.at<Vec3b>(i,j) = Vec3b(255,255,255);
      }
    }
  }
  return finalContour;
}

int main(int argc, char const *argv[])
{
  if (argc != 3) {
    printf("usage: ./graph-box <graph-img> <output-img>\n");
    return 0;
  }
  
  Mat input = imread(argv[1]);
  Mat temp1,// cropped 1st time
    cropped;
  vector<Point> finalContour=get_outer_box(input,temp1);
  
  vector<Point> tempContour=finalContour;
  if(finalContour.empty())return 0;

  vector<Point> extendedContour;
  extendedContour=shrinkContour(finalContour,-0.01*boundingRect(finalContour).height);

  cv::Mat gray;
  // will threshold this gray image
  cv::cvtColor(input, gray, CV_BGR2GRAY);
  cv::Mat mask;
  // threshold white/non-white
  cv::threshold(gray, mask,240, 255, THRESH_BINARY );
  // check if there's another box inside it which is the acutal graph
  int ctr=0;
  
  for (int i =0; i < input.rows; i++) {
    for (int j = 0; j < input.cols; j++) {
      if (pointPolygonTest(extendedContour, Point(j,i), false) < 0) {
        if(!mask.at<uchar>(i,j))
          ++ctr;
      }
    }
  }
  
  // some fixes
  if(ctr<=input.rows*0.0008*input.cols)
    {
      try
      {
        finalContour=get_outer_box(temp1,cropped);
        if(finalContour.empty() || contourArea(finalContour)<0.7*contourArea(tempContour))finalContour.swap(tempContour);
      }
      catch(...)
      {
        finalContour.swap(tempContour);
      }
    }
  else
    cropped=temp1.clone();
  

  // write the contour coordinates to stdout. we don't need to write the shrunk contour,
  // just the exact one.
  
    // for some reason we need final contour in an array for drawing..  
  for (int i = 0; i < finalContour.size(); ++i)
  {
    printf("%d %d\n", finalContour[i].x, finalContour[i].y);
  }
  // don't crop the image, just make everything outside contour white.
  Mat drawing = input.clone();
  vector<vector<Point> > dummy(1, finalContour);
  drawContours(drawing, dummy, 0, Scalar(255,0,255), 2, 8);
  imwrite("tmp/boxes.png", drawing);
    
  imwrite(argv[2], cropped);

  return 0;
}