#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include "pugixml.hpp"
#include <sstream>
using namespace std;
using namespace cv;


int main(int argc, char const *argv[])
{
  if (argc != 4) {
    printf("usage: ./remove-text <img> <hocr xml> <output-img>\n");
    return 0;
  }
  Mat img = imread(argv[1]);
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(argv[2]);
  pugi::xpath_node_set tools= doc.select_nodes("//span[@class='ocrx_word']");
  printf("num of words = %d\n", tools.size());
  for (pugi::xpath_node_set::const_iterator it = tools.begin(); it != tools.end(); ++it)
  {
    pugi::xpath_node node = *it;
    string coords = node.node().attribute("title").value();
    string val = node.node().child_value();
    if (val.size() == 0 || val == " ")
      continue;
    int x1, y1, x2, y2;
    sscanf(coords.c_str(), "%*s %d %d %d %d;", &x1, &y1, &x2, &y2);
    // coords >> dummy >> x1 >> y1 >> x2 >> y2;
    cv::rectangle(img, Point(x1, y1), Point(x2, y2), Scalar(255,255,255), CV_FILLED);
  }
  imwrite(argv[3], img);
  return 0;
}