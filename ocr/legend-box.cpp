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
#include "box-detection.hpp"


int main(int argc, char const *argv[])
{
  if (argc != 3) {
    printf("usage: ./legend-box <graph-img> <output-img>\n");
    return 0;
  }
  Mat input = imread(argv[1]);
  equalizeHist(input, input);
  Mat contourImg = input.clone();
  vector<vector<Point> > contours = getBoxes(input, 0, 50);
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