#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>
#include "pugixml.hpp"
#include <string>
#include <queue>

using namespace cv;
using namespace std;
#include "box-detection.hpp"
using namespace pugi;

int main(int argc, char const *argv[]) {
if (argc != 2) {
        printf("usage: ./helper <graph-img>\n");
        return 0;
      }
      Mat input = imread(argv[1]);
      Mat drawing = input.clone();
      vector<vector<Point> > contours = getBoxes(input);
      if (contours.size() == 0) {
        printf("no contours.?\n");
        return 0;
      }
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

  // for some reason we need final contour in an array for drawing..
      vector<vector<Point> > dummy(1, finalContour);

      Rect boundRect;
      boundRect = boundingRect( Mat(finalContour) );
      rectangle( drawing, boundRect.tl(), boundRect.br(), Scalar(100,100,100), 2, 8, 0 );
      cv::Point2f corners[4];
      corners[0] = boundRect.tl();
      corners[1] = Point2f(boundRect.tl().x + boundRect.width, boundRect.tl().y);
      corners[2] = boundRect.br();
      corners[3] = Point2f(boundRect.br().x - boundRect.width, boundRect.br().y);
      cout<<"\n"<<corners[0].x<<" "<<corners[0].y<<" "<<corners[3].x<<" "<<corners[3].y<<"\n";
}