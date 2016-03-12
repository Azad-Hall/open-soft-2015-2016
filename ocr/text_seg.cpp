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
#include "pugixml.hpp"
#include <string>
#include <queue>

using namespace cv;
using namespace std;
#include "box-detection.hpp"
using namespace pugi;

queue<string> Q_str;
queue<pair<string, pair<Point, Point> > > Q_str_point_ver;
queue<pair<string, pair<Point, Point> > > Q_str_point_hor;


    int main(int argc, char const *argv[])
    {
      if (argc != 6) {
        printf("usage: ./test_seg <graph-img> <left-up-corner-x> <left-up-corner-y> <left-down-corner-x> <left-down-corner-y>\n");
        return 0;
      }

      cv::Point2f corners[4];
      corners[0] = Point2f(atoi(argv[2]), atoi(argv[3]));
      corners[3] = Point2f(atoi(argv[4]), atoi(argv[5]));


  //////////////////////////////////////

      Mat img = imread(argv[1], 0);
      long long contain_sum = 0;
      vector<int> contain(img.cols/3,0);
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
  Point to_add;
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
  to_add.x = mid;
  to_add.y = corners[0].y - img.rows*1/100;
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
cout<<"\nvertical text: "<<txt_ver;
system("rm vert_text.jpg");
imwrite("y_label.jpg", y_label);
system("tesseract y_label.jpg tes_out0 digits hocr");

      imshow("vert_text", vert_text);
      imshow("y_label", y_label);


/////////////////////////Horizontal Text

      Vector<int> contain_hor(img.rows/2,0);
      for(int i=img.rows-1; i>corners[3].y; i--) {

       for (int j=0;j<img.cols;j++) {
     if(img.at<uchar>(i, j) < 200) {                                     ///////////////////Threshold used
       contain_hor[i] += 1;
     }
   }
 }
 int first_start = 0;
 int first_end = 0;
 int second_start = 0;
 int second_end =0;
 int third_start = 0;
 int third_end = 0;
    //for(int i=0; i<corners[0].x; i++)

 for(int i=img.rows-1; i>corners[3].y; i--)
 {
   if(contain_hor[i]>img.cols/20)
   {
     if(first_start==0) {
       first_start = i;
       first_end = i;
     }
       else if(first_end-i < img.rows*1/100)                   //////////////////Threshold used
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
       else if(second_end-i < img.rows*1/100)                    //////////////////Threshold used
         second_end = i; 
       else if(third_start == 0) {
         if(second_start == second_end)
         {
           i--;
           second_start = 0;
           continue;
         }
         third_start = i;
         third_end = i;
       }
       else if(third_end-i < img.rows*1/100) {
         third_end = i;
       }
    }
  }

     if(third_start == third_end)
       third_start = 0;
     Mat hor_text, x_label, title;
     if(third_start!=0) {
      int mid_first_second = (first_end+second_start)/2;
      int mid_second_third = (second_end+third_start)/2; 

   title = Mat(img, Rect(Point2f(0, mid_first_second), Point2f(img.cols-1, img.rows -1 ))); // change the random value
      hor_text = Mat(img, Rect(Point2f(0, mid_second_third), Point2f(img.cols-1, mid_first_second))); //second_ver + second_ver - third_ver
      x_label = Mat(img, Rect(Point2f(0, corners[3].y), Point2f(img.cols-1, mid_second_third)));
    }
    else {
     int mid_first_second = (first_end+second_start)/2;
      hor_text = Mat(img, Rect(Point2f(0, mid_first_second), Point2f(img.cols-1, img.rows - 1))); // change the random value
      x_label = Mat(img, Rect(Point2f(0, corners[3].y), Point2f(img.cols-1, mid_first_second)));
    }
    imshow("NEw", img);
    
    imshow("hor_text", hor_text);
    
    char txt_title[1024];
    char txt_hor[1024];    
    
    imwrite("hor_text.jpg", hor_text);
    pPipe = popen("tesseract hor_text.jpg stdout -psm 7", "r");
    fgets(txt_hor, 128, pPipe);
    cout<<"\nhorizontal text: "<<txt_hor;
    
    if(title.rows) {
     imshow("Title", title);
     imwrite("title.jpg", title);
     pPipe = popen("tesseract title.jpg stdout -psm 7", "r");
     fgets(txt_title, 128, pPipe);
     cout<<"\ntitle: "<<txt_title;
   }


imwrite("x_label.jpg", x_label);
system("tesseract x_label.jpg tes_out1 digits hocr");
imshow("x_label", x_label);



    waitKey(0);
   return 0;
 }
