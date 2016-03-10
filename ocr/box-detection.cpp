#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>

using namespace cv;
using namespace std;

// check if intervals on a 1d line overlap
bool overlap(int a1, int a2, int b1, int b2) {
  if (a2<a1)
    swap(a1, a2);
  if (b2<b1)
    swap(b1, b2);
  return a1 <= b2 && b1 <= a2;
}
void swapCoords(vector<Vec4i> &lines) {
  for (int i = 0; i < lines.size(); ++i)
  {
    swap(lines[i][0], lines[i][1]);
    swap(lines[i][2], lines[i][3]);
  }
}
// given set of vertical line segments, checks if they overlap. horizontal distance thresh = 10 pix. returns filtered set.
vector<Vec4i> getNonoverlappingVerticalLines(vector<Vec4i> lines) {
  vector<Vec4i> ret;
  for (int i =0; i < lines.size(); i++) {
    Point p1(lines[i][0], lines[i][1]), p2(lines[i][2], lines[i][3]);
    bool flag = false;
    for (int j = i+1; j < lines.size(); j++) {
      Point p3(lines[j][0], lines[j][1]), p4(lines[j][2], lines[j][3]);
      if (fabs(p1.x - p3.x) <= 10) {
        if (overlap(p1.y, p2.y, p3.y, p4.y)) {
          flag = 1;
          break;
        }
      }
    }
    if (!flag) 
      ret.push_back(lines[i]);
  }
  return ret;
}
// sorting functions for horizontal and vertical line segments
bool vSort(const Vec4i &a, const Vec4i &b) {
  return min(a[0], a[2]) < min(b[0], b[2]);
}

bool hSort(const Vec4i &a, const Vec4i &b) {
  return min(a[1], a[3]) < min(b[1], b[3]);
}

Point intersection(Vec4i a, Vec4i b) {
 // Store the values for fast access and easy
  // equations-to-code conversion
  Point p1(a[0], a[1]), p2(a[2], a[3]), p3(b[0], b[1]), p4(b[2], b[3]);
  float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
  float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;
   
  float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
  // If d is zero, there is no intersection
  assert(d!=0);
  // Get the x and y
  float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
  float x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
  float y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;
 
  // Return the point of intersection
  return Point(x,y);
}

bool outOfBounds(Point p, Size s) {
  return p.x >= s.width || p.x < 0 || p.y >= s.height || p.y < 0;
}
double dist (int x1, int y1, int x2, int y2) {
  return sqrt((x1-x2)*(double)(x1-x2) + (y1-y2)*(double)(y1-y2));
}
typedef unsigned char uchar;
// check if the rectangle (p1, p2) indeed exists in img. return the match score.
float rectScore(Point p1, Point p2, Mat img) {
  // iterate over all pixels on the rectangle and see if they are white in img.
  // img shoudl be single channel unsigned char
  // not hcekcing bounds here, assume everything is good...
 vector<int> cnt(4, 0), tot(4, 0);
 // counting each side separately, score will be min of 4 sides' score.
  double score = 99;
  for (int i = p1.y; i <= p2.y; i++) {
    if (img.at<uchar>(i, p1.x))
      cnt[0]++;
    if (img.at<uchar>(i, p2.x))
      cnt[1]++;
    tot[0]++; tot[1]++;
  }
  for (int j = p1.x; j <= p2.x; j++) {
    if (img.at<uchar>(p1.y, j))
      cnt[2]++;
    if (img.at<uchar>(p2.y, j))
      cnt[3]++;
    tot[2]++; tot[3]++;
  }
  for (int i = 0; i < 4; ++i)
    score = min(score, cnt[i]/(double)tot[i]);
  return score;
}

std::vector<std::vector<cv::Point> > getBoxes(Mat input) {
  cv::Mat gray;
  // will threshold this gray image
  cv::cvtColor(input, gray, CV_BGR2GRAY);
  cv::Mat mask;
  // threshold white/non-white
  cv::threshold(gray, mask,240, 255, CV_THRESH_BINARY_INV );
  vector<Vec4i> lines;
  vector<Vec4i> hlines, vlines; // horizontal and vertical lines
  HoughLinesP( mask, lines, 1, CV_PI/180, 230, 30, 20 );
  cv::Mat linesImg = cv::Mat::zeros(mask.size(), CV_8UC1), fullLinesImg = linesImg.clone();
  // fullLinesImg has full lines, linesImg only has line segments.
  for( size_t i = 0; i < lines.size(); i++ )
  {
    // check if horizontal or vertical. if not, don't draw it.
    double angle = atan2(lines[i][1]-lines[i][3], lines[i][0] - lines[i][2]);
    double eps = (1e-2)*M_PI; // 0.1% error
    if (fabs(angle-M_PI/2)>eps && fabs(angle) >eps && fabs(angle-M_PI) > eps && fabs(angle+M_PI/2)>eps && fabs(angle+M_PI) > eps)
      continue;
    if (fabs(lines[i][1]-lines[i][3]) > fabs(lines[i][0] - lines[i][2]))
      vlines.push_back(lines[i]);
    else
      hlines.push_back(lines[i]);
    // extend line by 1% each side.
    // actually don't extend. just dilate image afterwards.
    double r = 0.00 * dist (lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
    line( linesImg, Point(lines[i][0]+r*cos(angle), lines[i][1]+r*sin(angle)),
        Point(lines[i][2]-r*cos(angle), lines[i][3]-r*sin(angle)), Scalar(255), 3, 8 );
  }
  // dilate to close the rectangles
  dilate(linesImg, linesImg, Mat());
  dilate(linesImg, linesImg, Mat());
  imwrite("/tmp/lines.png", linesImg);
  // find contours. keep rectangle contours.
  std::vector<std::vector<cv::Point> > contours, rectContours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(linesImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
  cv::Mat drawing = cv::Mat::zeros(mask.size(), CV_8UC3);

  for (int i = 0; i < contours.size(); i++)
  {
    float ctArea = cv::contourArea(contours[i]);
    cv::Rect boundingBox = cv::boundingRect(contours[i]);
    // contours that have 80% of area of bounding box are rectangles for consideration
    float percentRect = ctArea / (float)(boundingBox.height * boundingBox.width);
    assert (percentRect >= 0 && percentRect <= 1.0);
    if (percentRect > 70/100.) {
      vector<Point> approxContour;
      // approx to rectangle maybe?
      approxPolyDP(contours[i], approxContour, 5, true);
      rectContours.push_back(approxContour);
      cv::Scalar color = cv::Scalar(0, 255, 0);
      drawContours(drawing, contours, i, color, 1, 8, hierarchy, 0, cv::Point());
    }
  }
  return rectContours;
}
// int main(int argc, char const *argv[])
// {
//   cv::Mat input = cv::imread(argv[1]);//input image
//   cv::Mat gray;
//   // will threshold this gray image
//   cv::cvtColor(input, gray, CV_BGR2GRAY);
//   cv::Mat mask;
//   // threshold white/non-white
//   cv::threshold(gray, mask,240, 255, CV_THRESH_BINARY_INV );
//   vector<Vec4i> lines;
//   vector<Vec4i> hlines, vlines; // horizontal and vertical lines
//   HoughLinesP( mask, lines, 1, CV_PI/180, 230, 30, 20 );
//   cv::Mat linesImg = cv::Mat::zeros(mask.size(), CV_8UC1), fullLinesImg = linesImg.clone();
//   // fullLinesImg has full lines, linesImg only has line segments.
//   for( size_t i = 0; i < lines.size(); i++ )
//   {
//     // check if horizontal or vertical. if not, don't draw it.
//     double angle = atan2(lines[i][1]-lines[i][3], lines[i][0] - lines[i][2]);
//     double eps = (1e-2)*M_PI; // 0.1% error
//     if (fabs(angle-M_PI/2)>eps && fabs(angle) >eps && fabs(angle-M_PI) > eps && fabs(angle-3*M_PI/2)>eps && fabs(angle-2*M_PI) > eps)
//       continue;
//     if (fabs(lines[i][1]-lines[i][3]) > fabs(lines[i][0] - lines[i][2]))
//       vlines.push_back(lines[i]);
//     else
//       hlines.push_back(lines[i]);
//     // extend line by 1% each side.
//     double r = 0.01 * dist (lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
//     line( linesImg, Point(lines[i][0]+r*cos(angle), lines[i][1]+r*sin(angle)),
//         Point(lines[i][2]-r*cos(angle), lines[i][3]-r*sin(angle)), Scalar(255), 3, 8 );
//   }

//   // find contours. keep rectangle contours.
//   std::vector<std::vector<cv::Point> > contours, rectContours;
//   std::vector<cv::Vec4i> hierarchy;
//   cv::findContours(linesImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
//   cv::Mat drawing = cv::Mat::zeros(mask.size(), CV_8UC3);

//   for (int i = 0; i < contours.size(); i++)
//   {
//     float ctArea = cv::contourArea(contours[i]);
//     cv::Rect boundingBox = cv::boundingRect(contours[i]);
//     // contours that have 80% of area of bounding box are rectangles for consideration
//     float percentRect = ctArea / (float)(boundingBox.height * boundingBox.width);
//     assert (percentRect >= 0 && percentRect <= 1.0);
//     if (percentRect > 80/100.) {
//       rectContours.push_back(contours[i]);
//       cv::Scalar color = cv::Scalar(0, 255, 0);
//       drawContours(drawing, contours, i, color, 1, 8, hierarchy, 0, cv::Point());
//     }
//   }
//   // only keep lines which are far apart from each other.
//   // if lines are closer than 10 pix and overlap, then keep only 1.
//   // vlines = getNonoverlappingVerticalLines(vlines);
//   // swapCoords(hlines);
//   // hlines = getNonoverlappingVerticalLines(hlines);
//   // swapCoords(hlines);
//   // sort(hlines.begin(), hlines.end(), hSort);
//   // sort(vlines.begin(), vlines.end(), vSort);
//   // cv::Mat rectImg = cv::Mat::zeros(mask.size(), CV_8UC1);
//   // // dilate original ( a lot) and then do rectangle detection
//   // dilate(mask, mask, Mat());
//   // dilate(mask, mask, Mat());
//   // dilate(mask, mask, Mat());
//   // // dilate(mask, mask, Mat());
//   // // dilate(mask, mask, Mat());
//   // imwrite("/tmp/dilated-mask.png", mask);
//   // for (int i = 0; i < vlines.size(); ++i)
//   // {
//   //   for (int j = 0; j < hlines.size(); ++j)
//   //   {
//   //     Point p1 = intersection(vlines[i], hlines[j]);
//   //     if (outOfBounds(p1, mask.size()))
//   //       continue;
//   //     for (int k = i+1; k < vlines.size(); k++) {
//   //       for (int l = j+1; l < hlines.size(); l++) {
//   //         Point p2 = intersection(vlines[k], hlines[l]);
//   //         if (outOfBounds(p2, mask.size()))
//   //           continue;
//   //         if (p2.x <= p1.x || p2.y <= p1.y)
//   //           continue;
//   //         // if rectangle score is good, draw it
//   //         if (rectScore(p1, p2, mask) > 0.9) {
//   //           rectangle(rectImg, p1, p2, Scalar(255), 2);
//   //         }
//   //       }
//   //     }
//   //   }
//   // }
//   // vector<Vec4i> filteredLines;
//   // filteredLines.insert(filteredLines.end(), hlines.begin(), hlines.end());
//   // filteredLines.insert(filteredLines.end(), vlines.begin(), vlines.end());
//   // for (int i = 0; i < filteredLines.size(); ++i)
//   // {
//   //   double angle = atan2(filteredLines[i][1]-filteredLines[i][3], filteredLines[i][0] - filteredLines[i][2]);
//   //   double r = 1e5;
//   //   line( fullLinesImg, Point(filteredLines[i][0]+r*cos(angle), filteredLines[i][1]+r*sin(angle)),
//   //       Point(filteredLines[i][2]-r*cos(angle), filteredLines[i][3]-r*sin(angle)), Scalar(255), 1, 8 );
//   // }
//   // printf("horizontal = %d, vertical = %d, total = %d\n", (int)hlines.size(), (int)vlines.size(), 
//   //   (int)hlines.size()+(int)vlines.size());
//   // std::vector<std::vector<cv::Point> > contours, rectContours;
//   // std::vector<cv::Vec4i> hierarchy;
//   // cv::findContours(linesImg, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
//   // cv::Mat drawing = cv::Mat::zeros(mask.size(), CV_8UC3);
//   // for (int i = 0; i < contours.size(); i++)
//   // {
//   //   float ctArea = cv::contourArea(contours[i]);
//   //   cv::Rect boundingBox = cv::boundingRect(contours[i]);
//   //   // contours that have 80% of area of bounding box are rectangles for consideration
//   //   float percentRect = ctArea / (float)(boundingBox.height * boundingBox.width);
//   //   assert (percentRect >= 0 && percentRect <= 1.0);
//   //   if (percentRect > 50/100. || 1) {
//   //     rectContours.push_back(contours[i]);
//   //     cv::Scalar color = cv::Scalar(0, 255, 0);
//   //     drawContours(drawing, contours, i, color, 1, 8, hierarchy, 0, cv::Point());
//   //   }
//   // }
//   imwrite("/tmp/lines.png", drawing);
//   return 0;
// }