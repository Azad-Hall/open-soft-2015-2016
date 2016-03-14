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

  dilate(binary, binary, getStructuringElement(MORPH_RECT, Size(3,3)));
  int a = 0.03 * img.cols;
  if(!a%2)
  	a=a+1;
  erode(binary, binary, getStructuringElement(MORPH_RECT, Size(a,a)));
  imshow("sdv",binary);
  vector<int> contain(binary.cols-1,0);
  for(int j=0;j<binary.cols-1;j++)
  {
  	for(int i=0;i<binary.rows-1;i++)
  	{
  		if(binary.at<uchar>(i,j)<220)
  		{
  			contain[j]++;
  		}
  	}
  }

  int seg = 0;
  for(int j=0;j<img.cols-1;j++)
  {
  	int a,b;
  	bool isStart = 0;
  	while(contain[j] < img.rows/3 && j<img.cols-1)
  		j++;
  	if(contain[j] >= img.rows/3 && j<img.cols-1 )
  	{	
  		a = j;
  		isStart = true;
  	}
  	if(isStart)
  	{
  		while(contain[j]>img.rows/3 && j<img.cols-1) {
  			j++;
  		}
  		b = j;
  	}
  	if(isStart) {
  	cout<<"\n"<<a<<" "<<b;
  	Mat new_img = Mat(img, Rect(Point2f(a, 0), Point2f(b, img.rows-1)));

  	string aa = "x_label_batman" + to_string(seg) + ".jpg";
  	Q_points.push({a, b});
  	imshow(aa.c_str(),new_img);
  	imwrite(aa.c_str(),new_img);
  	seg++;
  	}
  }
  system("rm tot_out.txt");
  for(int i=0;i<seg;i++)
  {
  	string a =  "x_label_batman" + to_string(i) + ".jpg";
  	cout<<"\n"<<a;
  	string to_tess ="tesseract " + a + " dig_out -psm 8 digits";
	system(to_tess.c_str());
	char txt_ver[1024];
	ifstream in;
	in.open("dig_out.txt");
	in.getline(txt_ver, 1000, '\n');
	in.close();
	ofstream of;
	of.open("tot_out.txt",ofstream::app);
	pair<int,int> frnt = Q_points.front();
	Q_points.pop();
	of<<(frnt.first+frnt.second)/2<<" "<<txt_ver<<"\n";
	of.close();
	cout<<"\n txt_ver "<<txt_ver;
  }

  

 // waitKey(0);

}