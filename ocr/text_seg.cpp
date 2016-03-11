// Uses graph box.cpp
// and finds the vertical text and y-label
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <fstream>
#include <math.h>
#include <algorithm>

using namespace cv;
using namespace std;
std::vector<std::vector<cv::Point> > getBoxes(Mat input);
vector<Point> getRectangularContour(vector<Point> largest);

int main(int argc, char const *argv[])
{
  if (argc != 2) {
    printf("usage: ./test_seg <graph-img>\n");
    return 0;
  }
  Mat input = imread(argv[1]);
  Mat drawing = input.clone();
  vector<vector<Point> > contours = getBoxes(input);
  if (contours.size() == 0) {
    printf("no contours.?\n");
    return 0;
  }
  // get max area contour
  vector<Point> largest = contours[0];
  int maxArea = contourArea(largest);
  for (int i = 1; i < contours.size(); i++) {
    int area = contourArea(contours[i]);
    if (area > maxArea) {
      maxArea = area;
      largest = contours[i];
    }
  }
  vector<Point> finalContour = getRectangularContour(largest);
  
  // for some reason we need final contour in an array for drawing..
  vector<vector<Point> > dummy(1, finalContour);
  
  Rect boundRect;
  boundRect = boundingRect( Mat(finalContour) );
  rectangle( drawing, boundRect.tl(), boundRect.br(), Scalar(100,100,100), 2, 8, 0 );
  cv::Point2f corners[4];
  corners[0] = boundRect.tl();
  corners[1] = Point2f(boundRect.tl().x + boundRect.width, boundRect.tl().y);
  corners[2] = boundRect.br();
  corners[3] = Point2f(boundRect.br().x - boundRect.width, boundRect.br().y);
  

  //////////////////////////////////////
  Mat img = imread(argv[1], 0);
  long long contain_sum = 0;
  Vector<int> contain(img.cols/3,0);
  for(int j=0; j<img.cols/3; j++) {
    for (int i=0;i<img.rows;i++) {
      if(img.at<uchar>(i,j) < 200) {                                     ///////////////////Threshold used
        contain[j] += 1;
      }
    }
    contain_sum += contain[j];
  }
  double thres = 3;
  Mat vert_text;
  Mat y_label;
  while(1)
  {
  int first_start = 0;
  int first_end = 0;
  int second_end = 0;
  int second_start = 0;
  int last = 0;
  for(int i=0; i<corners[0].x; i++)
  {
    if(contain[i]>img.rows/15)
      {
        cout<<"\n"<<i<<" "<<contain[i];
        if(first_start==0) {
          first_start = i;
          first_end = i;
        }
        else if(i-first_end < img.cols * thres/100)                   //////////////////Threshold used
          first_end = i;
        else if(second_start==0) {
          if(first_start == first_end)
            {
              i--;
              first_start = 0;
              continue;
            }
          second_start = i;
          second_end = i;
        }
        else if(i-second_end < img.cols * thres/100)                    //////////////////Threshold used
          second_end = i;
      }
  }
  cout<<"\n"<<first_start<<" "<<first_end<<" "<<second_start<<" "<<second_end;
  bool not_special = true;
  if(second_start == 0)
  {
    
    if(round(thres)==1) {
      not_special =false; // TODO: Handle this case
      cout<<"\n Error: Very close vertical text and label";
      exit(-1);
    }
    if(thres > 2.5)
      thres = 2.5;
    else if(thres > 2.0)
      thres = 2.0;
    else if(thres > 1.5)
      thres = 1.5;
    else if(thres > 1.0)
      thres = 1.0;
    if(not_special)
      continue;
  }

  int mid = (first_end+second_start)/2;

  vert_text = Mat(img, Rect(Point2f(first_start - (mid - first_start), corners[0].y - img.rows*1/100), Point2f(mid, corners[3].y + img.rows*1/100))); // change the random value
  y_label = Mat(img, Rect(Point2f(mid, corners[0].y - img.rows*1/100), Point2f(corners[3].x + img.cols*1/100, corners[3].y + img.rows*1/100)));
  
  if(second_start)
    break;
  }


  Mat res;
  cv::transpose(vert_text, res);

  cv::flip(res, res, 1);
  imwrite("vert_text.jpg",res);
  FILE *pPipe;
  pPipe = popen("tesseract vert_text.jpg stdout -psm 7", "r");
  char txt_ver[1024];
  fgets(txt_ver, 128, pPipe);
  cout<<"\nvert_text: "<<txt_ver;
  system("rm vert_text.jpg");
  imwrite("y_label.jpg", y_label);
  system("tesseract y_label.jpg tes_out");

  string line;
  stringstream s;
  ifstream myfile ("tes_out.txt");
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      s<<line<<"\n";
    }
    myfile.close();
  }
  else cout << "Unable to open file tes_out"; 
  string txt_y_label = s.str();
  replace( txt_y_label.begin(), txt_y_label.end(), 'U', '0');
  cout<<"\nY label:\n"<<txt_y_label;
  imshow("vert_text", vert_text);
  imshow("y_label", y_label);
  waitKey(0);

  return 0;
}