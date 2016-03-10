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

int main(int argc, char const *argv[])
{
  if (argc != 2) {
    printf("usage: ./graph-box <graph-img>\n");
    return 0;
  }
  Mat input = imread(argv[1]);
  vector<vector<Point> > contours = getBoxes(input);
  Mat drawing = input.clone();
  for (int i = 0; i < contours.size(); ++i)
  {
    drawContours(drawing, contours, i, Scalar(255,0,0), 2, 8);
  }
  imwrite("/tmp/boxes.png", drawing);
  return 0;
}