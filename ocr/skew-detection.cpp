#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>
// #include <gsl/gsl_sort.h>
// #include <gsl/gsl_statistics.h>

using namespace cv;
using namespace std;


void normalizeAngle(double& angle) {
  while (angle > 2*M_PI)
    angle -= 2*M_PI;

  while (angle < 0)
    angle += 2*M_PI;
}
int main(int argc, char const *argv[])
{
  if (argc != 3) {
    printf("usage: ./skew-detection <inputimg> <outputimg>\n");
  }
  Mat input = imread(argv[1]);
  int maxr = 0, maxg = 0, maxb = 0;
  for (MatIterator_<Vec3b> it = input.begin<Vec3b>(); it != input.end<Vec3b>(); it++) {
    Vec3b c = *it;
    maxr = max(maxr, (int)c[2]);
    maxg = max(maxg, (int)c[1]);
    maxb = max(maxb, (int)c[0]);
  }
  cv::Mat gray;
  // will threshold this gray image
  cv::cvtColor(input, gray, CV_BGR2GRAY);
  cv::Mat mask;
  // threshold white/non-white
  cv::threshold(gray, mask,240, 255, CV_THRESH_BINARY_INV );

  vector<Vec4i> lines;
  vector<Vec4i> hlines, vlines; // horizontal and vertical lines
  HoughLinesP( mask, lines, 1, CV_PI/180, 230, 30, 20 );
  Canny(input, mask, 50, 150, 3);
  cv::Mat linesImg = cv::Mat::zeros(mask.size(), CV_8UC1), fullLinesImg = linesImg.clone();
  vector<double> vangles, hangles;
  // fullLinesImg has full lines, linesImg only has line segments.
  for( size_t i = 0; i < lines.size(); i++ )
  {
    // check if horizontal or vertical. if not, don't draw it.
    double angle = atan2(lines[i][1]-lines[i][3], lines[i][0] - lines[i][2]);
    double refAngles[5] = {0, M_PI/2, M_PI, -M_PI/2., -M_PI};
    double eps = (1e-2)*M_PI; // 0.1% error
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
      vangles.push_back(angle);
    else
      hangles.push_back(angle);
    line( linesImg, Point(lines[i][0], lines[i][1]),
        Point(lines[i][2], lines[i][3]), Scalar(255), 1, 8 );
  }
  imwrite("tmp/lines-skew.png", linesImg);
  for (int i = 0; i < vangles.size(); ++i)
  {
    if (vangles[i] < 0)
      vangles[i] += M_PI;
  }
  for (int i = 0; i < hangles.size(); ++i)
  {
    if (hangles[i] > M_PI/2)
      hangles[i] -= M_PI;
    else if (hangles[i] < -M_PI/2)
      hangles[i] += M_PI;
    printf("angle = %lf\n", hangles[i]);
  }
  sort(vangles.begin(), vangles.end());
  sort(hangles.begin(), hangles.end());
  printf("num vertical = %d, horizontal = %d\n", vangles.size(), hangles.size());
  double medianV = vangles[vangles.size()/2];
  double medianH = hangles[hangles.size()/2];
  // double meanV = gsl_stats_mean(&vangles[0], 1, vangles.size());
  // double meanH = gsl_stats_mean(&hangles[0], 1, hangles.size());

  printf("Horizontal: (%lf), vertical: (%lf)\n", medianH, medianV);
  // append all the data together and take the median.
  for (int i = 0; i < vangles.size(); ++i)
  {
    hangles.push_back(vangles[i] - M_PI/2);
  }
  sort(hangles.begin(), hangles.end());
  double median = hangles[hangles.size()/2];
  printf("combined median = %lf\n", median);
  cv::Mat rot_mat = cv::getRotationMatrix2D(Point(input.cols/2., input.rows/2.), median*180/M_PI, 1);
  cv::Mat rotated;
  cv::warpAffine(input, rotated, rot_mat, input.size(), cv::INTER_CUBIC, BORDER_CONSTANT, Scalar(maxb,maxg,maxr));
  imwrite(argv[2], rotated);
  return 0;
}