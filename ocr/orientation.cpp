// for correcting orientation using tesseract.

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <math.h>
using namespace cv;
using namespace std;

void rotate_90n(cv::Mat &src, cv::Mat &dst, int angle)
{
    dst.create(src.size(), src.type());
    if(angle == 270 || angle == -90){
        // Rotate clockwise 270 degrees
        cv::transpose(src, dst);
        cv::flip(dst, dst, 0);
    }else if(angle == 180 || angle == -180){
        // Rotate clockwise 180 degrees
        cv::flip(src, dst, -1);
    }else if(angle == 90 || angle == -270){
        // Rotate clockwise 90 degrees
        cv::transpose(src, dst);
        cv::flip(dst, dst, 1);
    }else if(angle == 360 || angle == 0){
        if(src.data != dst.data){
            src.copyTo(dst);
        }
    }
}
int main(int argc, char const *argv[])
{
  if (argc != 3) {
    printf("usage: ./orientation <img> <outimg>\n");
    return 0;
  }
  // we simply overwrite the image with the orientation corrected imgage.
  Mat input=imread(argv[1]);
  FILE *pPipe;
  char buf[1000];
  sprintf(buf, "tesseract %s stdout -psm 0 2>&1", argv[1]);
  pPipe = popen(buf, "r");
  printf("here\n");
  char str[1000];
  fgets(str, 500, pPipe);
  fgets(str, 500, pPipe);
  int deg;
  sscanf(str, "%*s %*s %*s %d", &deg);
  printf("angle = %d\n", deg);
  Mat rot;
  rotate_90n(input, rot, deg);
  imwrite(argv[2], rot);
  return 0;
}