// for detecting the graph's bounding box inside a single graph's image.

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
std::vector<std::vector<cv::Point> > getBoxes(Mat input, int minLineLength);
vector<Point> getRectangularContour(vector<Point> largest);
vector<Point> getRectangularContour2(vector<Point> largest);

int main(int argc, char const *argv[])
{
  if (argc != 2) {
    printf("usage: ./graph-box <graph-img>\n");
    return 0;
  }
  Mat input = imread(argv[1]);
  Mat contourImg = input.clone();
  vector<vector<Point> > contours = getBoxes(input, input.rows/4);
  if (contours.size() == 0) {
    printf("no contours.?\n");
    return 0;
  }
  for (int i =0; i < contours.size(); i++) {
    drawContours(contourImg, contours, i, Scalar(255,0,0), 1, 8);
  }
  imwrite("/tmp/contours.png", contourImg);
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
  vector<Point> finalContour = getRectangularContour(largest);
  Mat drawing = input.clone();
  // for some reason we need final contour in an array for drawing..
  vector<vector<Point> > dummy(1, finalContour);
  drawContours(drawing, dummy, 0, Scalar(255,0,255), 2, 8);
  imwrite("/tmp/boxes.png", drawing);
  return 0;
}