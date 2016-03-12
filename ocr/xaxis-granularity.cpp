// takes input the graph image and its bounding contour (the rectangle), and outputs a vector of x pixel coordinates at which
// the y coordinates need to be evaluated. granularity is 1/10th of the least count (which has to be detectd)

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


vector<int> getXGranularity(Mat graph, vector<Point> contour) {

}

int main(int argc, char const *argv[])
{
  if (argc != 2) {
    printf("usage: ./xaxis-granularity <graph-img>\n");
    return 0;
  }
  Mat img = imread(argv[1]);
  imwrite("/tmp/tmp.png", img);
  return 0;
}