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
#include "box-detection.hpp"
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
  if (argc != 4) {
    printf("usage: ./graph-candidates <img> <img-notext> <graph-base-name>\n");
    return 0;
  }
  cv::Mat input = cv::imread(argv[1]);//input image
  // do the OCR here itself.
  {
    char buf[1000];
    sprintf(buf, "tesseract %s /tmp/out -l eng hocr", argv[1]);
    system(buf);
  }
  // rotate image by 90 and do OCR again. sometimes vertical text is only found in rotated image.
  {
    Mat rot;
    rotate_90n(input, rot, 90);
    imwrite("/tmp/tmp-rot.png", rot);
    char buf[1000];
    sprintf(buf, "tesseract /tmp/tmp-rot.png /tmp/out-rot -l eng hocr");
    // imshow("rotated", rot);
    // waitKey();
    system(buf);
  }
  Mat input_notext = imread(argv[2]);
  std::vector<std::vector<cv::Point> >  rectContoursTemp = getBoxes(input_notext, 0), rectContours;
  // get more precise rectangles by using getRectangularContour
  for (int i = 0; i < rectContoursTemp.size(); i++) {
    rectContours.push_back(getRectangularContour(rectContoursTemp[i]));
  }
  cv::Mat drawing = cv::Mat::zeros(input_notext.size(), CV_8UC3);
  printf("num rect contours = %d\n", (int)rectContours.size());
  // xml stuff to find vertical text
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file("/tmp/out.hocr");
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
  printf("normal vertical pars = %d\n", (int)vertical_pars.size());
  // select vertical texts from the rotated image.
  {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file("/tmp/out-rot.hocr");
    // finds all paragraph nodes that have a child ocr_line node that has textangle 90 attribute
    pugi::xpath_node_set nodes_vert= doc.select_nodes("//span[not(contains(@title, 'textangle 270')) and (@class='ocr_line')]/parent::*");
    // maybe check if para is empty or not?
    int cnt = 0;
    for (pugi::xpath_node_set::const_iterator it = nodes_vert.begin(); it != nodes_vert.end(); ++it) {
      pugi::xpath_node node = *it;
      string title = node.node().attribute("title").value();
      int x1, y1, x2, y2;
      sscanf(title.c_str(), "%*s %d %d %d %d;", &x1, &y1, &x2, &y2);
      // convert to original coordinates;
      std::swap(y1,x1); std::swap(y2, x2); y1 = input.rows - y1-1; y2 = input.rows-y2-1;
      vertical_pars.push_back(Rect(Point(x1, y1), Point(x2, y2)));
      cnt++;
    }
    printf("vertical pars found after rotation by -90: %d\n", cnt);
  }
  printf("total num vertial pars = %d\n", (int)vertical_pars.size());
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
          // draw the contour of the graph box we identified 
          cv::Scalar color = cv::Scalar(255, 255, 255);
          drawContours(drawing, rectContours, i, color, 1, 8);
          sprintf(buf, "%s-%d.png", argv[3], (int)imgCandidates.size());
          imgCandidates.push_back(imgRect);
          imwrite(buf, croppedGraph);
        }
      }
    }
  }
  imwrite("/tmp/contours.png", drawing);
  // imgCandidates rect has the boundary boxes for the graph (excluding axis labels/numbers)
  // print it out if needed. currently only output is in the graph images generated
  return 0;
}