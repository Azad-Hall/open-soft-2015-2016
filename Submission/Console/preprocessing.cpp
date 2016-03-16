// this should take the input image and do the following:
// * orientation detection: correct the image so that page is straight
// * rotate cw by 90 and save as a separate image (needed for OCR)
// * maybe perform ocr and remove all detected text. (might be good for box-detection using hough lines)
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "pugixml.hpp"
#include <iostream>
#include <math.h>
#include <algorithm>
using namespace cv;
using namespace std;

void compute_skew(const char* filename)
{
   // Load in grayscale.
   cv::Mat src = cv::imread(filename, 0);
   cv::Size size = src.size();
   cv::bitwise_not(src, src);
   std::vector<cv::Vec4i> lines;
       cv::HoughLinesP(src, lines, 1, CV_PI/180, 100, size.width / 2.f, 20);
  cv::Mat disp_lines(size, CV_8UC1, cv::Scalar(0, 0, 0));
    double angle = 0.;
    unsigned nb_lines = lines.size();
    for (unsigned i = 0; i < nb_lines; ++i)
    {
        cv::line(disp_lines, cv::Point(lines[i][0], lines[i][1]),
                 cv::Point(lines[i][2], lines[i][3]), cv::Scalar(255, 0 ,0));
        angle += atan2((double)lines[i][3] - lines[i][1],
                       (double)lines[i][2] - lines[i][0]);
    }
    angle /= nb_lines; // mean angle, in radians.
 
    std::cout << "File " << filename << ": " << angle * 180 / CV_PI << std::endl;
 
    //imshow(filename, src);
    //cv::waitKey(0);
    cv::destroyWindow(filename);
}


int main(int argc, char const *argv[])
{
  compute_skew(argv[1]);
  return 0;
}
