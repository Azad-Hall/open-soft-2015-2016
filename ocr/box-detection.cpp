#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"


using namespace cv;
using namespace std;

int main(int argc, char const *argv[])
{
  cv::Mat input = cv::imread(argv[1]);//input image
   cv::Mat gray;
  // will threshold this gray image
  cv::cvtColor(input, gray, CV_BGR2GRAY);
  cv::Mat mask;
  // threshold white/non-white
  cv::threshold(gray, mask,240, 255, CV_THRESH_BINARY_INV );
  vector<Vec4i> lines;
  HoughLinesP( mask, lines, 1, CV_PI/180, 200, 30, 20 );
  cv::Mat drawing = cv::Mat::zeros(mask.size(), CV_8UC3);
  for( size_t i = 0; i < lines.size(); i++ )
  {
      line( drawing, Point(lines[i][0], lines[i][1]),
          Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
  }
  imwrite("/tmp/lines.png", drawing);
  return 0;
}