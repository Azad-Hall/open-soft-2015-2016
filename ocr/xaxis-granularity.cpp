// takes input the graph image and its bounding contour (the rectangle), and outputs a vector of x pixel coordinates at which
// the y coordinates need to be evaluated. granularity is 1/10th of the least count (which has to be detectd)

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <math.h>
#include <algorithm>
#define ff first
#define ss second

using namespace cv;
using namespace std;

//diect 1: above, -1: below
vector<int> getXGranularity(Mat graph, Point left, Point right, int direct) {
	
	float slope = ((float)(left.y - right.y))/((float)(left.x - right.x));
	//cout << right.x << " " << left.x << endl;
	int hist[right.x - left.x + 1] = {0};
	vector<int> Points, granularPoints;
	//cout << "here" << endl;
	for(int i = left.x + graph.rows/100; i <= right.x - graph.rows/100; i++){
		int y = left.y + (i - left.x)*slope;
		for(int j = 0 ; j < 20 ; j++){
			uchar colour = graph.at<uchar>(Point(i, y + direct*j));
			if(colour < 240){
				hist[i - left.x]++;
			}
		}
	}
	
	for(int i = left.x + 2 ; i <= right.x - 2; i++){
		if(hist[i - left.x] > 10){
			//cout <<  "here" << endl;
			Points.push_back(i);
		}
	}
	//cout << Points.size() << endl;
	if(Points.empty())
		return vector<int>();
	int cnt = 1, val = Points[0];
	for(int i = 1 ; i < Points.size() ; i++){
		if( (Points[i] - Points[i - 1]) < 20){
			val += Points[i];
			cnt++;
		}
		else{
			if(cnt > 1)
				granularPoints.push_back(val/cnt);
			val = Points[i];
			cnt = 1;
		}
	}
	if(cnt > 1)
		granularPoints.push_back(val/cnt);

	imwrite("/home/arnav/Desktop/abcd.png", graph);
	return granularPoints;
}

int main(int argc, char const *argv[])
{
  // if (argc != 2) {
  //   printf("usage: ./xaxis-granularity <graph-img>\n");
  //   return 0;
  // }
  vector<pair<int, int> > contour;
  int x, y;
  for(int i = 0 ; i < 4 ; i++){
  	scanf("%d%d", &x, &y);
  	//reversed for sorting
  	contour.push_back(make_pair(y, x));
  }
  sort(contour.begin(), contour.end());

  Mat img = imread(argv[1]);
  Mat grayimg(img.size(), CV_8U);
  cvtColor(img, grayimg, CV_RGB2GRAY);
  Mat binimg(grayimg.size(), grayimg.type());
  Mat binimgdilated(grayimg.size(), grayimg.type());
  threshold(grayimg, binimg, 240, 255, CV_THRESH_BINARY);
  
  for(int i = 0 ; i < 3 ; i++)
  	erode(binimg, binimg, Mat());
  namedWindow("Output", cv::WINDOW_AUTOSIZE);
  imshow("Output", binimgdilated);
  waitKey(0);
  //imwrite("/tmp/tmp.png", img);
  vector<int> ugranularPoints, diff;
  if(contour[2].ss < contour[3].ss)
  	ugranularPoints = getXGranularity(binimg, Point(contour[2].ss, contour[2].ff), Point(contour[3].ss, contour[3].ff), -1);
  else
  	ugranularPoints = getXGranularity(binimg, Point(contour[3].ss, contour[3].ff), Point(contour[2].ss, contour[2].ff), -1);
  // if(contour[0].ss < contour[1].ss)
  // 	ugranularPoints = getXGranularity(binimg, Point(contour[0].ss, contour[0].ff), Point(contour[1].ss, contour[1].ff), -1);
  // else
  // 	ugranularPoints = getXGranularity(binimg, Point(contour[1].ss, contour[1].ff), Point(contour[0].ss, contour[0].ff), -1);
  
  int val = 0;
  if(ugranularPoints.size() < 2)
  	return 0;
  for(int i = 1 ; i < ugranularPoints.size() ; i++)
  	diff.push_back(ugranularPoints[i] - ugranularPoints[i - 1]);
  sort(diff.begin(), diff.end());
  cout << diff[diff.size()/2] << endl;
  return 0;
}