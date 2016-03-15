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
    sprintf(buf, "convert %s -auto-level tmp/normalized.png", argv[1]);
    system(buf);
  }
  cout<<"\n------------------";
  input = imread("tmp/normalized.png");
  
  if(getBoxesNew(input, 5, 30, 20, 10e-2, argv[2]) == -1)
    printf("\n legend not detected :( ");
  return 0;
}