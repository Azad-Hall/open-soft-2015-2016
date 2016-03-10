#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>

using namespace cv;
using namespace std;



int main(int argc, char const *argv[])
{
  Mat img = imread(argv[1]);
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
  vector<double> vangles, hangles;
  // fullLinesImg has full lines, linesImg only has line segments.
  for( size_t i = 0; i < lines.size(); i++ )
  {
    // check if horizontal or vertical. if not, don't draw it.
    double angle = atan2(lines[i][1]-lines[i][3], lines[i][0] - lines[i][2]);
    double refAngles[5] = {0, M_PI/2, M_PI, -M_PI/2., -M_PI};
    double eps = (500e-2)*M_PI; // 0.1% error
    bool flag = false;
    for (int j = 0; j < 5; j++) {
      if (fabs(angle - refAngles[j]) < eps) {
        flag = true;
        break;
      }
    }
    if (!flag)
      continue; // not a vertical or horizontal line.
    if (fabs(lines[i][1]-lines[i][3]) > fabs(lines[i][0] - lines[i][2]))
      vlines.push_back(lines[i]);
    else
      hlines.push_back(lines[i]);
    
  }
  return 0;
}