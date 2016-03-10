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
double dist (int x1, int y1, int x2, int y2);
Point intersection(Vec4i a, Vec4i b);

bool sortFnRev(const pair<double, Vec4i> &s1, const pair<double, Vec4i> &s2)  {
  return s1.first > s2.first;
}
Point imgCentre;
// sort line segments ccw around the image
bool sortCCW(const Vec4i &s1, const Vec4i &s2) {
  Point p1((s1[0]+s1[2])/2., (s1[1]+s1[3])/2.);
  Point p2((s2[0]+s2[2])/2., (s2[1]+s2[3])/2.);
  return atan2(p1.y-imgCentre.y, p1.x-imgCentre.x) < atan2(p2.y-imgCentre.y,p2.x-imgCentre.x);
}
int main(int argc, char const *argv[])
{
  if (argc != 2) {
    printf("usage: ./graph-box <graph-img>\n");
    return 0;
  }
  Mat input = imread(argv[1]);
  imgCentre = Point(input.cols/2, input.rows/2);
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
  vector<pair<double, Vec4i> > segments;
  Point p1 = largest[0];
  for (int i = 1; i < largest.size(); i++) {
    Point p2 = largest[i];
    segments.push_back(make_pair(dist(p1.x, p1.y, p2.x, p2.y), Vec4i(p1.x, p1.y, p2.x, p2.y)));
    p1 = largest[i];
  }
  // add last segment
  Point p2 = largest[0];
  segments.push_back(make_pair(dist(p1.x, p1.y, p2.x, p2.y), Vec4i(p1.x, p1.y, p2.x, p2.y)));
  sort(segments.begin(), segments.end(), sortFnRev );
  assert(largest.size() >= 4);
  vector<Vec4i> rectSegments;
  vector<Point> finalContour;
  for (int i = 0; i < 4; i++) {
    rectSegments.push_back(segments[i].second);
  }
  sort(rectSegments.begin(), rectSegments.end(), sortCCW);
  // add first segment again to the last since we need intersections only
  rectSegments.push_back(rectSegments[0]);
  for (int i = 1; i < rectSegments.size(); i++) {
    Point p = intersection(rectSegments[i-1], rectSegments[i]);
    finalContour.push_back(p);
  }
  Mat drawing = input.clone();
  // for (int i = 0; i < 4; i++) {
  //   line( drawing, Point(segments[i].second[0], segments[i].second[1]),
  //       Point(segments[i].second[2], segments[i].second[3]), Scalar(255), 3, 8 );
  // }
  // for some reason we need final contour in an array for drawing..
  vector<vector<Point> > dummy(1, finalContour);
  for (int i = 0; i < contours.size(); ++i)
  {
    drawContours(drawing, dummy, 0, Scalar(255,0,0), 2, 8);
  }
  imwrite("/tmp/boxes.png", drawing);
  return 0;
}