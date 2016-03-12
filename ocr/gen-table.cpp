// given the binary images of each color, and x and y scales,
// make the table.
// we also need x,y axis labels , legend labels and graph title also, should keep this in the xml file only.

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
#include "pugixml.hpp"

vector<Point2f> getData(Mat bin, vector<int> xsamples, )
int main(int argc, char const *argv[])
{
  printf("usage: ./gen-table <xml-file> <binimg-basename>\n");
  int n = 0;
  // read number of binary images from stdin
  scanf("%d\n", &n);

  return 0;
}
