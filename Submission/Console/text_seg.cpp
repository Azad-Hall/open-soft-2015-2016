// Uses graph box.cpp
// and finds the vertical text and y-label
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <vector>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include "pugixml.hpp"
#include <exception>
#include <cstdlib>
#include <string>
#include <queue>
#include <bits/stdc++.h>

using namespace cv;
using namespace std;
using namespace pugi;

queue<string> Q_str;
queue<pair<string, pair<Point, Point> > > Q_str_point_ver;
queue<pair<string, pair<Point, Point> > > Q_str_point_hor;

#include "box-detection.hpp"


// crops rectangle to stay within image.
Rect cropRect(Rect r, cv::Size size) {
  Point tl(max(r.tl().x, 0), max(r.tl().y, 0));
  Point br(min(r.br().x, size.width-1), min(r.br().y, size.height-1));
  return Rect(tl, br);
}





int main(int argc, char const *argv[])
{
  if (argc != 2) {
    printf("usage: ./test_seg <graph-img>\n");
    return 0;
  }
  Mat input = imread(argv[1]);
  Mat drawing = input.clone();
      // read the box contour from stdin
  vector<Point> finalContour;
  for (int i = 0; i < 4; i++) {
    int x, y;
    scanf("%d %d\n", &x, &y);
    finalContour.push_back(Point(x,y));
  }

  // for some reason we need final contour in an array for drawing..

  Rect boundRect;
  boundRect = boundingRect( Mat(finalContour) );
  rectangle( drawing, boundRect.tl(), boundRect.br(), Scalar(100,100,100), 2, 8, 0 );
  imshow("dra",drawing);
      //waitKey(0);
  cv::Point2f corners[4];
  corners[0] = boundRect.tl();
  corners[1] = Point2f(boundRect.tl().x + boundRect.width, boundRect.tl().y);
  corners[2] = boundRect.br();
  corners[3] = Point2f(boundRect.br().x - boundRect.width, boundRect.br().y);


  //////////////////////////////////////

  Mat img = imread(argv[1], 0);
  vector<int> contain(corners[0].x,0);
  for(int j=0; j<corners[0].x; j++) {
    for (int i=0;i<img.rows;i++) {
      if(img.at<uchar>(i,j) < 200) {                                     ///////////////////Threshold used
        contain[j] += 1;
      }
    }
    cout<<"\n"<<j<<" "<<contain[j];
  }
  double thres = 3;
  Mat vert_text;
  Mat y_label;
  Point to_add;
  int numItr = 0;
  while(++numItr < 1e3)
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
    cout<<"\n Text Seg: "<<first_start<<" "<<first_end<<" "<<second_start<<" "<<second_end;
    bool not_special = true;
    if(second_start == 0)
    {

      if(round(thres)==0.2) {
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
    else if(thres > 0.8)
      thres = 0.8;
    else if(thres > 0.6)
      thres = 0.6;
    else if(thres > 0.4)
      thres = 0.4;
    else if(thres > 0.2)
      thres = 0.2;
    if(not_special)
      continue;
  }

  int mid = (first_end+second_start)/2;
  Rect vert_rect = Rect(Point2f(0, corners[0].y), Point2f(mid, corners[3].y + img.rows*1/100));
  vert_rect = cropRect(vert_rect, img.size());
  vert_text = Mat(img, vert_rect); 
  Rect y_rect(Point2f(mid, corners[0].y), Point2f(corners[3].x, corners[3].y));
  y_rect = cropRect(y_rect, img.size());
  y_label = Mat(img, y_rect);
  to_add.x = mid;
  to_add.y = corners[0].y - img.rows*1/100;
  if(second_start)
    break;
}
// something went wrong..
if (numItr >= 1e3)
  return -1;

Mat res;
cv::transpose(vert_text, res);

cv::flip(res, res, 1);
imwrite("vert_text.jpg",res);
FILE *pPipe;
pPipe = popen("tesseract vert_text.jpg stdout -psm 7", "r");
char txt_ver[1024];
fgets(txt_ver, 128, pPipe);
cout<<"\nvert_text: "<<txt_ver;
imwrite("y_label.jpg", y_label);
system("tesseract y_label.jpg tes_out0 digits hocr");

imshow("vert_text", vert_text);
imshow("y_label", y_label);


imshow("sidnvinsv",img);


/////////////////////////Horizontal Text


      vector<int> contain_hor(img.rows,0);
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
    //cout<<"\n-->"<<corners[0].x;
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
   cout<<"\ntext_seg -->"<<first_start<<" "<<first_end<<" "<<second_start<<" "<<second_end<<" "<<third_start<<" "<<third_end;
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
     Rect hor_rect = Rect(Point2f(0, mid_first_second), Point2f(img.cols-1, img.rows - 1));
     hor_rect = cropRect(hor_rect, img.size());
      hor_text = Mat(img, hor_rect); // change the random value
      Rect xrect = Rect(Point2f(0, corners[3].y), Point2f(img.cols-1, mid_first_second));
      xrect = cropRect(xrect, img.size());
      x_label = Mat(img, xrect);
    }
    imshow("NEw", img);
    
    imshow("hor_text", hor_text);
    
    char txt_title[1024];
    char txt_hor[1024];    
    txt_title[0] = 'N';
    txt_title[1] = 'A';
    txt_title[2] = '\0';
    
    imwrite("hor_text.jpg", hor_text);
    pPipe = popen("tesseract hor_text.jpg stdout -psm 7", "r");
    fgets(txt_hor, 128, pPipe);
    cout<<"\nhor_text: "<<txt_hor;
    
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
   pugi::xml_document indoc;
   pugi::xml_node to_add_x = indoc.append_child();
   to_add_x.set_name("to_add_x");
   to_add_x.append_attribute("y_axis_offset_x") = to_add.x;

   pugi::xml_node to_add_y = indoc.append_child();
   to_add_y.set_name("to_add_y");
   to_add_y.append_attribute("y_axis_offset_y") = to_add.y;

   pugi::xml_node ver_text = indoc.append_child();
   ver_text.set_name("ver_text");
   ver_text.append_attribute("vertical_text") = txt_ver;

   pugi::xml_node hori_text = indoc.append_child();
   hori_text.set_name("hori_text");
   hori_text.append_attribute("horizontal_text") = txt_hor;

   pugi::xml_node title_text = indoc.append_child();
   title_text.set_name("title_text");
   title_text.append_attribute("title_text") = txt_title;

   ofstream xml_out;
   xml_out.open("indoc.xml");
   indoc.print(xml_out);
   xml_out.close();

    //waitKey(0);
   return 0;
 }
