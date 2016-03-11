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
std::vector<std::vector<cv::Point> > getBoxes(Mat input);
vector<Point> getRectangularContour(vector<Point> largest);

int main(int argc, char const *argv[])
{
  if (argc != 2) {
    printf("usage: ./graph-box <graph-img>\n");
    return 0;
  }
  Mat input = imread(argv[1]);
  vector<vector<Point> > contours = getBoxes(input);
  if (contours.size() == 0) {
    printf("no contours.?\n");
    return 0;
  }
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
  imwrite("/tmp/boxes.png", drawing);
  return 0;
}