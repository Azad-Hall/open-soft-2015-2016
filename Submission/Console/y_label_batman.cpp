#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <vector>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "pugixml.hpp"
#include <bits/stdc++.h>

using namespace cv;
using namespace std;
using namespace pugi;


int main(int argc, char const *argv[])
{
  queue<pair<int,int> > Q_points; 
  if (argc != 2) {
    printf("usage: ./{prog name} <graph-img>\n");
    return 0;
  }
  Mat img = imread(argv[1],0);
  Mat binary = img > 220;
  imshow("sdva",binary);
  int a = 0.005 * img.rows;
  if(a==0)
    a = 3;
  if(!a%2)
    a+=1;
  dilate(binary, binary, getStructuringElement(MORPH_RECT, Size(a,a)));
  a = 0.03 * img.rows;
  if(!a%2)
    a=a+1;
  erode(binary, binary, getStructuringElement(MORPH_RECT, Size(a,a)));
  imshow("sdv",binary);
  vector<int> contain(binary.rows-1,0);
  for(int i=0;i<binary.rows-1;i++)
  {
    for(int j=0;j<binary.cols-1;j++)
    {
      if(binary.at<uchar>(i,j)<220)
      {
        contain[i]++;
      }
    }
  }
  bool isStartBlack = false;
  for (int i = 0; i < contain.size(); i++) {
    if (contain[i] < img.cols/5)
      contain[i] = 0;
    else
      contain[i] = 1;
  }
  if (contain[0] == 1)
    isStartBlack = true;
  bool isStart = false;
  int b = 0;
  a = 0;
  if (isStartBlack) {
    isStart = true;
    a = 0;
  }
  int seg = 0;
  for (int i = 1; i < contain.size(); i++) {
    if (contain[i] != contain[i-1]) {
      // edge
      if (!isStart) {
        isStart = true;
        a = i;
      } else {
        isStart =  false;
        b = i;
        cout<<"\n-->"<<a<<" "<<b;
        Mat new_img = Mat(img, Rect(Point2f(0, a), Point2f(img.cols-1, b)));

        string aa = "y_label_batman" + to_string(seg) + ".jpg";
        Q_points.push({a, b});
        imshow(aa.c_str(),new_img);
        
        imwrite(aa.c_str(),new_img);
        seg++;
      }
    }
  }
  // for(int i=0;i<img.rows-1;i++)
  // {
  //   int a,b;
  //   bool isStart = 0;
  //   while(contain[i] < img.cols/5 && i<img.rows-1)
  //     i++;
  //   if(contain[i] >= img.cols/5 && i<img.rows-1)
  //   { 
  //     a = i;
  //     isStart = true;
  //   }
  //   if(isStart)
  //   {
  //     while(contain[i]>img.cols/5 && i<img.rows-2) {
  //       i++;
  //     }
  //     b = i;
  //   }
  //   if(isStart) {
  //   cout<<"\n-->"<<a<<" "<<b;
  //   Mat new_img = Mat(img, Rect(Point2f(0, a), Point2f(img.cols-1, b)));

  //   string aa = "y_label_batman" + to_string(seg) + ".jpg";
  //   Q_points.push({a, b});
  //   imshow(aa.c_str(),new_img);
    
  //   imwrite(aa.c_str(),new_img);
  //   seg++;
  //   }
  // }
  system("rm tot_out_y.txt");
  for(int i=0;i<seg;i++)
  {
  string a =  "y_label_batman" + to_string(i) + ".jpg";
  cout<<"\n"<<a;
  string to_tess ="tesseract " + a + " dig_out -psm 8 digits";
  system(to_tess.c_str());
  char txt_ver[1024];
  ifstream in;
  in.open("dig_out.txt");
  in.getline(txt_ver, 1000, '\n');
  in.close();
  ofstream of;
  of.open("tot_out_y.txt",ofstream::app);
  pair<int,int> frnt = Q_points.front();
  Q_points.pop();
  of<<(frnt.first+frnt.second)/2<<" "<<txt_ver<<"\n";
  of.close();
  cout<<"\n txt_ver "<<txt_ver;
  }

  

 // waitKey(0);

}