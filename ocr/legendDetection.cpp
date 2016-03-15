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
// #include </home/evil999man/inter_hall_16/ocr/peakdetect.cpp>
#include </home/aytas32/OpenSoft/inter_hall_16/ocr/peakdetect.cpp>


using namespace cv;
using namespace std;



pair<int,int> histogram(vector<int> &rowHist,Mat& img){

	int maxH=0;
	int sum=0,avg;
	for(int i=0;i<rowHist.size();i++){
		sum+=rowHist[i];
		maxH=max(maxH,rowHist[i]);
	}
	avg=sum/rowHist.size();
	int hist_w = rowHist.size(); int hist_h = 400;
	  for (int i = 0; i < rowHist.size(); ++i)
	  {
	    rowHist[i] = (rowHist[i] *hist_h)/maxH;
	  }
	 int bin_w = cvRound( (double) hist_w/rowHist.size() );
	  Mat histImg(hist_h, hist_w, CV_8U, Scalar(0));
	  for (int i = 1; i < rowHist.size(); i++) {
	    line( histImg, Point( bin_w*(i-1), hist_h - cvRound(rowHist[i-1]) ) ,
	                       Point( bin_w*(i), hist_h - cvRound(rowHist[i]) ),
	                       Scalar( 255), 2, 8, 0  );
	  }
	  imshow("column Histogram",histImg);
	 

	  vector<pair<int,int> > seg;
	{
		int prv=-1; bool on=false;
		for(int i=0;i<rowHist.size();++i){

			if(rowHist[i]<0.7*maxH)
				if(on)	on=false,seg.push_back(make_pair(prv,i-1));
				else continue;

			else if(!on)	prv=i,on=true;
		}	
		if(on) seg.push_back(make_pair(prv,rowHist.size()-1));
	}

	vector<pair<int,int> > finalseg;

  for(int it=0;it<seg.size();++it){
  	finalseg.push_back(seg[it]);
  	if(it)
  	if(-seg[it-1].second+seg[it].first<=img.cols*0.03){
  		finalseg.pop_back();
  		finalseg.rbegin()->second=seg[it].second;
  	}	
  }
  pair<int,int> maxLen=make_pair(0,0);
  for(int i=0;i<finalseg.size();i++){
  	if(finalseg[i].second-finalseg[i].first>maxLen.second-maxLen.first)
  		maxLen=finalseg[i];
  }
  
  return maxLen;
}


void tessy(int ind,map<int,string>& mp,Mat& imgx, pair<int,int> X,pair<int,int> Y)
{
	Point a,b2;
  double c,d;

  // imshow("",imgx);

  	c=X.second-X.first;
  	d=Y.second-Y.first;
  	a.x=max(0.0,X.first-imgx.cols*0.01);
  	a.y=max(0.0,Y.first-imgx.rows*0.007);
  	b2.x=min(imgx.cols-1.0,X.second+imgx.cols*0.01);
  	b2.y=min(imgx.rows-1.0,Y.second+imgx.rows*0.007);
  	Rect rct(a,b2);
  	Mat imgTemp=imgx(rct);
  	imwrite("output.jpg",imgTemp);

  	char buf1[1000],buf2[1000];
  	remove("temp.ppm");remove("temp.png");
	sprintf(buf1,"convert output.jpg temp.ppm");
	sprintf(buf2,"unpaper  --no-mask-center --no-border-align temp.ppm temp.png");
	system(buf1);
	system(buf2);
	system("tesseract temp.png dig_out -psm 6");
	 char txt[1024];
  ifstream in;
  in.open("dig_out.txt");
  in.getline(txt, 1000, '\n');
  in.close();
  mp[ind]=txt;
}

int main(int argc, char const *argv[]){
	Mat un= imread(argv[1]);
	double mean,maxdev=50,r,b,g;
	double diffr,diffg,diffb;
	Mat img1=imread(argv[1],1); 
	Mat img0=imread(argv[1],0),eimg0;




	for(int i=0;i<img0.cols;i++){
		for(int j=0;j<img0.rows;j++){
			Vec3b color = img1.at<Vec3b>(j,i);
			r=color[0];g=color[1];b=1.3*color[2];
			
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
	Mat imgBW=img0.clone();

	int erosion_size=5;
	erode(img0, img0, getStructuringElement(MORPH_ELLIPSE, Size(5 , 1)));	
	
	imshow("yo",img0);
	Mat unCol=img0.clone();

	waitKey(0);
	vector<int> rowHist(unCol.rows,0);
	for(int i=0.01*unCol.rows;i<unCol.rows*0.99;i++){
		for(int j=0.01*unCol.cols;j<unCol.cols*0.99;j++){
			if(unCol.at<uchar>(i,j)==0){
				rowHist[i]++;
			}
		}
	}


	int maxH = 0,sum=0,avg;
	for(int i=0;i<rowHist.size();i++){
		maxH=max(maxH,rowHist[i]);
		sum+=rowHist[i];
	}
	maxH=0;//shift histogram by the avg
	avg=sum/rowHist.size();
	// cout<<"ave "<<avg<<endl;
	for(int i=0;i<rowHist.size();i++){
		//cout<<rowHist[i]<<endl;
		rowHist[i]-=0.5*avg;
		maxH=max(maxH,rowHist[i]);

		//sum+=rowHist[i];
	}
	vector<int> zeroMaxima;
	bool high=false,low=true;
	for(int i=0;i<rowHist.size();i++){
		
		if(rowHist[i]<=0)
			zeroMaxima.push_back(i);
	}
	
	int hist_w = rowHist.size(); int hist_h = 400;
	  for (int i = 0; i < rowHist.size(); ++i)
	  {
	    rowHist[i] = (rowHist[i] *hist_h)/maxH;
	  }
	
//segments 
	vector<pair<int,int> > seg;
	{
		int prv=-1; bool on=false;
		for(int i=0;i<rowHist.size();++i){

			if(rowHist[i]>0)
				if(on)	on=false,seg.push_back(make_pair(prv,i-1));
				else continue;

			else if(!on)	prv=i,on=true;
		}	
		if(on) seg.push_back(make_pair(prv,rowHist.size()-1));
	}


	FILE* fp = fopen("res1.csv","w");
	  for(int i=0;i<rowHist.size();i++){
	    fprintf(fp,"%d,%d\n",i,rowHist[i]);
	  }
	 fclose(fp);

    string cmd = "dummy -i res1.csv -d 5e1 -o out1.csv";
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
	// cout<<"HERE\n";
	for(size_t i = 0; i < args.size(); i++)
      delete[] args[i];
  
  ifstream FP("out1.csv");
  int i,j;
  string as;
  vector<float> maxima;
  i=0;
  while(getline(FP,as)){
    maxima.push_back(stof(as));
    ++i;
  }
  // print the number of maxima
  int maxNo=maxima.size()/2;


  int bin_w = cvRound( (double) hist_w/rowHist.size() );
  Mat histImg(hist_h, hist_w, CV_8U, Scalar(0));
  for (int i = 1; i < rowHist.size(); i++) {
    line( histImg, Point( bin_w*(i-1), hist_h - cvRound(rowHist[i-1]) ) ,
                       Point( bin_w*(i), hist_h - cvRound(rowHist[i]) ),
                       Scalar( 255), 2, 8, 0  );
  }

 
  	
  vector<pair<int,int> > finalseg;

  for(int it=0;it<seg.size();++it){
  	finalseg.push_back(seg[it]);
  	if(it)
  	if(-seg[it-1].second+seg[it].first<=un.rows*0.022){
  		finalseg.pop_back();
  		finalseg.rbegin()->second=seg[it].second;
  	}	
  }

  erode(img0, img0, getStructuringElement(MORPH_ELLIPSE, Size(5,11)) );
  vector<pair<int,int>> whites;
  for(int i=1;i<finalseg.size();i++){
  	whites.push_back(make_pair(finalseg[i-1].second+1,finalseg[i].first-1));
  }
 
  ofstream fout("legend_boxes.txt");
  map<int,string> mp;
  Mat imgx=imread(argv[1]);
  fout<<whites.size()<<"\n";
  for(int i=0;i<whites.size();i++){
  	vector<int> colHist(un.cols,0);
  	for(int j=whites[i].first;j<whites[i].second;j++){
  		for(int k=0;k<un.cols;k++){
  			if(img0.at<uchar>(j,k)==0)
  				colHist[k]++;
  		}
  	}
  	pair<int,int> X=histogram(colHist,un);
  	pair<int,int> Y=whites[i];
  	tessy(i,mp,imgx,X,Y);
  	fout<<X.first<<" "<<X.second<<" "<<Y.first<<" "<<Y.second<<"\n"<<mp[i]<<"\n";
  	for(int m=Y.first;m<Y.second;m++){
  		for(int n=X.first;n<X.second;n++){
  			un.at<Vec3b>(m,n)=Vec3b(255,0,0);
  		}
  	}

  	

  }
  fout.close();

  
 
 //  imshow("green",un);
 //   imshow("histogram", histImg);
	// waitKey(0);
 
 return 0;
}