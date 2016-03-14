#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
//#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <fstream>
#include <math.h>
#include <vector>
#include <map>
#include <string>
#include </home/aytas32/OpenSoft/inter_hall_16/ocr/peakdetect.cpp>
// #include </home/aytas32/OpenSoft/inter_hall_16/ocr/peakdetect.cpp>


using namespace cv;
using namespace std;

/*int getHues(int hue,vector<pair<int,int>> hues){
	int i;
	for(i=0;i<hues.size();i++){
		if(hue>hues[i].first && hue<hues[i].second)
			return i;
	}
	return i;
}

int legendNos(Mat imgHSV,vector<pair<int,int>> hues,vector<pair<int,int>> boxesX,vector<pair<int,int>> boxesY){
	//no of plots > no of boxes means increase params
	//else if  no of plots < no of boxes means decrease params
	//else finding the pair of corresponding text box and hue range
	vector<int> boxHueLeft,boxHueRight;
	//for left
	for(int i=0;i<boxesX.size();i++){
		vector<int> carts(hues.size()+1,0);
		for(int i=boxesX[i].first;i>0&&i>boxesX[i].first-20;i--){
			for(int j=boxesY[i].first;j<imgHSV.cols && j<boxesY[i].second;j++){
				int hue=imgHSV.at<Vec3b>(i,j)[0];
				++carts[getHues(hue,hues)];

			}
		}
		int maxPos=0;
		for(int i=0;i<carts.size()-1;i++){
			if(carts[i]>carts[maxPos])
				maxPos=i;
		}
		boxHueLeft.push_back(maxPos);
	}
	//check for uniqueness
	//duplicate values means no of plots is 

	for(int i=0;i<boxesX.size();i++){
		vector<int> carts(hues.size()+1,0);
		for(int i=boxesX[i].first;i<imgHSV.cols &&i<boxesX[i].first+20;i++){
			for(int j=boxesY[i].first;j<imgHSV.cols && j<boxesY[i].second;j++){
				int hue=imgHSV.at<Vec3b>(i,j)[0];
				++carts[getHues(hue,hues)];

			}
		}
		int maxPos=0;
		for(int i=0;i<carts.size()-1;i++){
			if(carts[i]>carts[maxPos])
				maxPos=i;
		}
		boxHueRight.push_back(maxPos);
	}
}*/

int max(int a , int b) {
	return ( a > b) ? a: b;
}
int main(int argc, char const *argv[]){
	/*Mat un= imread(argv[1]);//"./unColored/b.png"
	Mat unCol;
	cvtColor(un,unCol,CV_BGR2HSV);
	imshow("dgsfag",un);*/
	Mat un= imread(argv[1]);
	double mean,maxdev=50,r,b,g;
	double diffr,diffg,diffb;
	Mat img1=imread(argv[1],1); 
	Mat img0=imread(argv[1],0),eimg0;
	imshow("middle",img0);




	for(int i=0;i<img0.cols;i++){
		for(int j=0;j<img0.rows;j++){
			Vec3b color = img1.at<Vec3b>(j,i);
			r=color[0];g=color[1];b=color[2];
			
			mean = (r + g + b) / 3;
	        diffr = abs(mean - r);
	        diffg = abs(mean - g);
	        diffb = abs(mean - b);
	        if ((diffr + diffg + diffb) < maxdev && img0.at<uchar>(j,i)<150)
	            img0.at<uchar>(j,i)=0;
	        else
	        	img0.at<uchar>(j,i)=255;
		}
	}
	erode(img0, img0, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

	// erode(img0, img0, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)) );
	imshow("yo",img0);
	Mat unCol=img0.clone();

	//imshow("sidnv",unCol);
	waitKey(0);
	vector<int> rowHist(unCol.rows,0);
	for(int i=0.01*unCol.rows;i<unCol.rows*0.99;i++){
		for(int j=0.01*unCol.cols;j<unCol.cols*0.99;j++){
			if(unCol.at<uchar>(i,j)==0){
				rowHist[i]++;
			}
		}
	}


	int maxH = 0;
	for(int i=0;i<rowHist.size();i++){
		cout<<rowHist[i]<<endl;
		maxH=max(maxH,rowHist[i]);
	}
	cout<<"maxH :"<<maxH<<endl;
	// std::vector<int> minima;
	// for(int i=0;i<rowHist.size()-1;i++){
	//   	if(rowHist[i]-rowHist[i+1] > 1*maxH/5){
	//   		minima.push_back(i);
	//   		//cout<<"gsah"<<rowHist[i]<<endl;
	//   	}
	//   }
	// cout << maxH << endl;
// imshow("sidnv",unCol);
	////////////////////////
	int hist_w = rowHist.size(); int hist_h = 400;
	  for (int i = 0; i < rowHist.size(); ++i)
	  {
	    rowHist[i] = (rowHist[i] *hist_h)/maxH;
	  }
	 for(int k = 0 ; k < 0	; k++){

	    int nhist[260] = {0};
	    for(int i = 5 ; i < 250 ;i++){
	      nhist[i] = 0;
	      for(int j = -1 ; j <= 1 ; j++){//-4 4
	        nhist[i] += (1/(abs(j)/3.0 + 1.0))*rowHist[i + j];
	      }
	    }
	    for(int i = 5 ; i < 250 ; i++){
	      rowHist[i] = nhist[i];
	    }
	  }
	FILE* fp = fopen("res1.csv","w");
	  for(int i=0;i<256;i++){
	    fprintf(fp,"%d,%d\n",i,rowHist[i]);
	  }
	 fclose(fp);

    string cmd = "dummy -i res1.csv -d 1e1 -o out1.csv";
    std::vector<char *> args;
    std::istringstream iss(cmd);

    std::string token;
    while(iss >> token) {
      char *arg = new char[token.size() + 1];
      copy(token.begin(), token.end(), arg);
      arg[token.size()] = '\0';
      args.push_back(arg);
    }

	run(args.size(), &args[0]);
	cout<<"HERE\n";
	for(size_t i = 0; i < args.size(); i++)
      delete[] args[i];
  
  // system("./peakdetect -i res.csv -d 1e2 -o out.csv");//1e2
  ifstream FP("out1.csv");
  int i,j;
  string as;
  vector<float> maxima;
  i=0;
  while(getline(FP,as)){
    // cout<<stof(as)<<"\n";
    maxima.push_back(stof(as));
    cout<<"hola"<<maxima[i]<<endl;
    ++i;
    //hist[stof(as)]=10000;
  }
  // print the number of maxima
  int maxNo=maxima.size()/2;
  printf("%d\n", (int)maxima.size()/2);
  // first half of maxima array is max, second half is min
  //maxima.push_back(256);


	cout<<"HERE \n";
  int bin_w = cvRound( (double) hist_w/rowHist.size() );
  Mat histImg(hist_h, hist_w, CV_8U, Scalar(0));
  for (int i = 1; i < rowHist.size(); i++) {
    // histImg.at<uchar>(i,rowHist[i])=255;
    /*if(rowHist[i - 1] - rowHist[i] < 50)
    	continue;*/
    line( histImg, Point( bin_w*(i-1), hist_h - cvRound(rowHist[i-1]) ) ,
                       Point( bin_w*(i), hist_h - cvRound(rowHist[i]) ),
                       Scalar( 255), 2, 8, 0  );
  }
  for(int i=maxNo;i<maxima.size();i++){

  }
  
  for(int i=0;i<maxNo;i++){
  	cout<<maxima[i]<<endl;
  	for(int j=0;j<un.cols;j++){
  		un.at<Vec3b>(maxima[i]+4,j)=Vec3b(0,255,0);
  	}
  }
  for(int i=maxNo;i<maxima.size();i++){
  	cout<<maxima[i]<<endl;
  	for(int j=0;j<un.cols;j++){
  		un.at<Vec3b>(maxima[i]+4,j)=Vec3b(0,0,255);
  	}
  }
  imshow("green",un);
   imshow("histogram", histImg);
waitKey(0);
}