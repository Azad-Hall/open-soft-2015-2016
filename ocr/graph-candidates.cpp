#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "pugixml.hpp"
#include <iostream>
#include <math.h>

using namespace cv;
int main(int argc, char const *argv[])
{
  if (argc != 4) {
    printf("usage: ./graph-candidates <img> <hocr-xml> <graph-base-name>\n");
    return 0;
  }
  cv::Mat input = cv::imread(argv[1]);//input image
  
  cv::Mat gray;
  // will threshold this gray image
  cv::cvtColor(input, gray, CV_BGR2GRAY);
  cv::Mat mask;
  // threshold white/non-white
  cv::threshold(gray, mask,240, 255, CV_THRESH_BINARY_INV );
  std::vector<std::vector<cv::Point> > contours, rectContours;
  std::vector<cv::Vec4i> hierarchy;
  cv::findContours(mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
  for (int i = 0; i < contours.size(); i++)
  {
    float ctArea = cv::contourArea(contours[i]);
    cv::Rect boundingBox = cv::boundingRect(contours[i]);
    // contours that have 80% of area of bounding box are rectangles for consideration
    float percentRect = ctArea / (float)(boundingBox.height * boundingBox.width);
    assert (percentRect >= 0 && percentRect <= 1.0);
    if (percentRect > 80/100.)
      rectContours.push_back(contours[i]);
  }
  printf("num rect contours = %d\n", rectContours.size());
  // xml stuff to find vertical text
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(argv[2]);
  // finds all paragraph nodes that have a child ocr_line node that has textangle 90 attribute
  pugi::xpath_node_set nodes_vert= doc.select_nodes("//span[contains(@title, 'textangle 90')]/parent::*");
  vector<cv::Rect> vertical_pars;
  for (pugi::xpath_node_set::const_iterator it = nodes_vert.begin(); it != nodes_vert.end(); ++it) {
    pugi::xpath_node node = *it;
    string title = node.node().attribute("title").value();
    int x1, y1, x2, y2;
    sscanf(title.c_str(), "%*s %d %d %d %d;", &x1, &y1, &x2, &y2);
    vertical_pars.push_back(Rect(Point(x1, y1), Point(x2, y2)));
  }
  // now match the contours with the vertical paragraphs to figure out graphs.
  vector<cv::Rect> imgCandidates;
  vector<bool> vertParUsed(vertical_pars.size(), false);
  for (int i = 0; i < rectContours.size(); i++) {
    cv::Rect imgRect = cv::boundingRect(rectContours[i]);
    bool done = false;
    for (int j = 0; j < vertical_pars.size() && !done; j++) {
      if (vertParUsed[j])
        continue;
      // if vert text is within y-range of rect and covers 10% height, and at most 30% x-distance apart and to the left, then it matches.
      cv::Rect txtRect = vertical_pars[j];
      if (txtRect.tl().y > imgRect.tl().y && txtRect.br().y < imgRect.br().y && txtRect.height >= 0.1 * imgRect.height) {
        int dist = imgRect.tl().x - txtRect.br().x;
        if (dist <= 0.30*imgRect.width && (imgRect.tl().x - txtRect.tl().x) > 0) {
          vertParUsed[j] = true;
          done = true; // done with this imgRect.
          // pad 2.5% in each direction
          int xpad = imgRect.width*0.025;
          int ypad = imgRect.height*0.025;
          Point tl(txtRect.tl().x-xpad, imgRect.tl().y-ypad);
          Point br(imgRect.br().x+xpad, imgRect.br().y + txtRect.width + dist+ypad);
          // make sure ROI is within image.
          tl.x = max(0,tl.x); tl.y = max(0,tl.y);
          br.x = min(input.cols-1, br.x); br.y = min(input.rows-1, br.y);
          Mat croppedGraph = input(cv::Rect(tl, br));
          char buf[1000];
          printf("writinh.\n");
          sprintf(buf, "%s-%d.png", argv[3], imgCandidates.size());
          imgCandidates.push_back(imgRect);
          imwrite(buf, croppedGraph);
        }
      }
    }
  }
  // imgCandidates rect has the boundary boxes for the graph (excluding axis labels/numbers)
  // print it out if needed. currently only output is in the graph images generated
  return 0;
}