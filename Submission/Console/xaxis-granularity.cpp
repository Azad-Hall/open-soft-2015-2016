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
vector<int> getXGranularity(Mat graph, Mat img, Point left, Point right, int direct) {
	
	assert(left.x < right.x);
	float slope = ((float)(left.y - right.y))/((float)(left.x - right.x));
	//cout << right.x << " " << left.x << endl;

	int hist[right.x - left.x + 1] ;
	memset(hist,0,sizeof(hist));
	int minval, maxval;
	vector<int> Points, granularPoints;
	//cout << "here" << endl;
	for(int i = left.x; i <= right.x ; i++){
		int y = left.y + (i - left.x)*slope;
		for(int j = 0 ; j < img.cols/80 ; j++){
			uchar colour = graph.at<uchar>(Point(i, y + direct*j));
			if(colour < 240){
				hist[i - left.x]++;
			}
		}
	}
	
	minval = *min_element(hist, hist + right.x - left.x + 1);
	maxval = *max_element(hist, hist + right.x - left.x + 1);
	
	//cout << minval << " " << maxval << endl;
	for(int i = left.x + 2 ; i <= right.x - 2; i++){
		if(hist[i - left.x] > (maxval - minval)/2 + minval ){
			Vec3b color;
			color.val[0] = color.val[2] = 0;
			color.val[1] = 255;
			img.at<Vec3b>(Point(i, left.y)) = color;
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
			if(cnt > 2)
				granularPoints.push_back(val/cnt);
			val = Points[i];
			cnt = 1;
		}
	}
	if(cnt > 1)
		granularPoints.push_back(val/cnt);

	imwrite("/home/arnav/Desktop/abcd.png", img);
	return granularPoints;
}

int main(int argc, char const *argv[])
{
  // if (argc != 2) {
  //   printf("usage: ./xaxis-granularity <graph-img>\n");
  //   return 0;
  // }
  vector<pair<int, int> > contour;
  vector<Point> cnt;
  int x, y;
  for(int i = 0 ; i < 4 ; i++){
  	scanf("%d %d", &x, &y);
  	//reversed for sorting
  	contour.push_back(make_pair(y, x));
    cnt.push_back(Point(x,y));
  }
  sort(contour.begin(), contour.end());
  Mat img = imread(argv[1]);
  Mat grayimg(img.size(), CV_8U);
  cvtColor(img, grayimg, CV_BGR2GRAY);
  Mat binimg(grayimg.size(), grayimg.type());
  Mat binimgdilated(grayimg.size(), grayimg.type());
  threshold(grayimg, binimg, 240, 255, CV_THRESH_BINARY);
  
  // for(int i = 0 ; i < 3 ; i++)
  erode(binimg, binimg, Mat());
  namedWindow("Output", cv::WINDOW_AUTOSIZE);
  imshow("Output", binimg);
  // waitKey(0);
  imwrite("tmp/tmp.png", binimg);
  vector<int> ugranularPoints, diff;
  if(contour[2].ss < contour[3].ss)
  	ugranularPoints = getXGranularity(binimg, img, Point(contour[2].ss, contour[2].ff), Point(contour[3].ss, contour[3].ff), -1);
  else
  	ugranularPoints = getXGranularity(binimg, img, Point(contour[3].ss, contour[3].ff), Point(contour[2].ss, contour[2].ff), -1);

  int val = 0;
  if (ugranularPoints.size() <= 1){
  		if(contour[2].ss < contour[3].ss ){
  			cout << (contour[3].ss - contour[2].ss)/10 << endl;
  		}	
  		else{
  			cout << (contour[2].ss - contour[3].ss)/10 << endl;
  		}
  }
  for(int i = 1 ; i < ugranularPoints.size() ; i++)
  	diff.push_back(ugranularPoints[i] - ugranularPoints[i - 1]);
  sort(diff.begin(), diff.end());
  cout << diff[diff.size()/2] << endl;
  return 0;
}