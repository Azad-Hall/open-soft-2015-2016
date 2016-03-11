// for detecting if the legen has a bounding box or not
// input should be the graph's cropped image (output of graph-box)

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
// experimental, not yet done. will need to finish this (or modify getRectangularContouur) because does not wokr
// whjen there is grid.
vector<Point> getRectangularContour2(vector<Point> largest);
vector<Point> shrinkContour(vector<Point> contour, double pix) ;


int main(int argc, char const *argv[])
{
  if (argc != 3) {
    printf("usage: ./legend-box <graph-img> <output-img>\n");
    return 0;
  }
  Mat input = imread(argv[1]);
  Mat contourImg = input.clone();
  vector<vector<Point> > contours = getBoxes(input, input.rows/4);
  if (contours.size() == 0) {
    printf("no contours.?\n");
  } else {
    for (int i =0; i < contours.size(); i++) {
      drawContours(contourImg, contours, i, Scalar(255,0,0), 1, 8);
    }
    imwrite("/tmp/contours.png", contourImg);
  }
  return 0;
}