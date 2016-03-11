// for detecting if the legen has a bounding box or not
// input should be the graph's cropped image (output of graph-box)
// should output image with legend box removed, if not detected thenw e should just remove text.
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
  // normalize image using imagemagick!
  {
    char buf[1000];
    sprintf(buf, "convert %s -auto-level /tmp/normalized.png", argv[1]);
    system(buf);
  }
  input = imread("/tmp/normalized.png");
  Mat contourImg = input.clone();
  vector<vector<Point> > contours = getBoxes(input, 5, 10, 20);
  if (contours.size() > 0) {
    // find largest contour by area
    vector<Point> largest = contours[0];
    int maxArea = contourArea(largest);
    for (int i = 1; i < contours.size(); i++) {
      int area = contourArea(contours[i]);
      if (area > maxArea) {
        maxArea = area;
        largest = contours[i];
      }
    }
    // check if area is large enough to be considered as a legend box.
    // it would help if we knew how many colors were there, since hegiht of legend box 
    // depends on tha.
    cv::Rect bb = boundingRect(largest);
    // 10% dimension = 1% area?
    if (bb.width * bb.height > 0.1*0.1*input.cols*input.rows) {
      // lets expand it by 10 pix to make sure that all of legend gets deletd
      largest = shrinkContour(largest, -10);
      // for some reason we need final contour in an array for drawing..
      vector<vector<Point> > dummy(1, largest);
      drawContours(contourImg, dummy, 0, Scalar(255,0,255), 2, 8);
      imwrite("/tmp/contours.png", contourImg);
      return 0;
    }
  }
  printf("no legend box detected.\n");
  return 0;
}