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

using namespace cv;
using namespace std;
typedef unsigned char uchar;
// expands black mask using a looser threshold for saturation to be considered black (eg. 20%)
void traverse(Mat &bmask, Mat &visited, Mat &imgHSV, int i, int j, Mat &bmaskNew, Mat &whiteMask, int lowSatThresh) {
  int iArr[] = {-2, -1, 0, 1, 2};
  for (int k = 0; k < 5; k++) {
    for (int l = 0; l < 5; l++) {
      int ii = i+iArr[k], jj = j+iArr[l];
      if (ii < 0 || ii >= bmask.rows || jj < 0 || jj >= bmask.cols)
        continue;
      if (ii == i && jj == j)
        continue;
      Vec3b hsv = imgHSV.at<Vec3b>(ii,jj);
      if (visited.at<uchar>(ii,jj))
        continue;
      visited.at<uchar>(ii,jj) = 1;
      if (hsv[1] <= lowSatThresh && whiteMask.at<uchar>(ii,jj) ) {
        visited.at<uchar>(ii,jj) = 1;
        bmaskNew.at<uchar>(ii,jj) = 0;
        traverse(bmask, visited, imgHSV, ii, jj, bmaskNew, whiteMask, lowSatThresh);
      }
    }
  }
}
Mat expandBMask(Mat bmask, Mat imgHSV, Mat whiteMask, int lowSatThresh) {
  Mat visited = Mat::zeros(bmask.size(), CV_8U);
  Mat bmaskNew = Mat::ones(bmask.size(), CV_8U);
  for (int i = 0; i < bmask.rows; ++i)
  {
    for (int j = 0; j < bmask.cols; ++j)
    {
      if (bmask.at<uchar>(i,j) == 0)
        bmaskNew.at<uchar>(i,j) = 0;
      if (bmask.at<uchar>(i,j) == 0 && visited.at<uchar>(i,j) == 0) {
        visited.at<uchar>(i,j) = 1;
        traverse(bmask, visited, imgHSV, i, j, bmaskNew, whiteMask, lowSatThresh);
      }
    }
  }
  return bmaskNew;
}

int main(int argc, char const *argv[])
{
  Mat img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
  Mat imgHSV;
  cvtColor(img, imgHSV, CV_BGR2HSV);
  Mat mask(img.rows, img.cols, CV_8U, Scalar(1)), bmask = mask.clone(); // to mask away all colorless pixels
  // bmask = black mask. will dilate to remove some anti-aliased points
  for (int i = 0; i < mask.rows; ++i)
  {
    for (int j = 0; j < mask.cols; ++j)
    {
      mask.at<uchar>(i,j) = 1;
      bmask.at<uchar>(i,j) = 1;
      Vec3b cRGB = img.at<Vec3b>(i,j);
      Vec3b cHSV = imgHSV.at<Vec3b>(i,j);
      if (cRGB[0] >= 210 && cRGB[1] >= 210 && cRGB[2] >= 210) {
        mask.at<uchar>(i,j) = 0;
      } else if (cRGB[0] <= 30 && cRGB[1] <= 30 && cRGB[2] <= 30) {
        bmask.at<uchar>(i,j) = 0;
      }  // if saturation is low i.e less than 10%, it could be black/grey, we don't need that.
      else if (cHSV[1] <= .08*255) {
        bmask.at<uchar>(i,j) = 0;
      }      
    }
  }
  //anti aliasing correction
  // using search method
  bmask = expandBMask(bmask, imgHSV, mask, 0.18*255);
  bool got=false;
  /*for (int i = 1; i < mask.rows-1; ++i)
  {
    for (int j = 1; j < mask.cols-1; ++j)
    {
      if(bmask.at<uchar>(i,j)==0)
        continue;
      got=false;
      for(int m=i-1;m<=i+1;m++)
      {
        for(int n=j-1;n<=j+1;n++)//if there is even 1 black in kernel, centre is black
        {
          if(bmask.at<uchar>(i,j)==0)
          {got =true;break;}
        }if(got)break;
      }
      if(got)
        bmask.at<uchar>(i,j)=0;
    }
  }*/
  //   Mat bmask_new = bmask.clone();
  // for(int times=0;times<1;times++)
  // {
  //   for (int i = 3; i < mask.rows-3; ++i)
  //   {
  //     for (int j = 3; j < mask.cols-3; ++j)
  //     {
  //       if(bmask.at<uchar>(i,j)==0)
  //         continue;
  //       got=false;
  //       for(int m=i-3;m<=i+3;m++)
  //       {
  //         for(int n=j-3;n<=j+3;n++)//if there is even 1 black in kernel, centre is black
  //         {
  //           if(bmask.at<uchar>(m,n)==0)
  //           {got =true;break;}
  //         }if(got)break;
  //       }
  //       if(got)
  //         bmask_new.at<uchar>(i,j)=0;
  //     }
  //   }
  // }
  
  // bmask = bmask_new;
  



  // erode(bmask, bmask, Mat());
  // mask = mask & bmask;
  mask = mask & bmask;
  // mask = bmask;
  //crisp up the plots
  //bool got=false;
  Mat avgImg=imgHSV.clone();
  int Max,avg,values[9];
  // for(int times=0;times<1;times++)
  // {
  //   for (int i = 1; i < mask.rows-1; ++i)
  //   {
  //     for (int j = 1; j < mask.cols-1; ++j)
  //     {
  //       if(mask.at<uchar>(i,j)==0)
  //         continue;
  //       got=false;
  //       avg=0;
  //       vector<int>values;
  //       int count=0;
  //       for(int m=i-1;m<=i+1;m++)
  //       {
  //         for(int n=j-1;n<=j+1;n++)
  //         {
  //           if(mask.at<uchar>(i,j)==0)
  //             continue;
  //           values.push_back(imgHSV.at<Vec3b>(m,n)[0]);
  //           avg+=imgHSV.at<Vec3b>(m,n)[0];
  //         }
  //       }
  //       sort(values.begin(),values.end());
  //       avg/=values.size();
  //       avgImg.at<Vec3b>(i,j)[0]=values.back();//values[8];//avg;//values[4];
  //     }
  //   }
  // }
  //imwrite("")

  // we will now classify with H value only.
  // make histogram on H
  vector<int> hist(256, 0);
  for (int i = 0; i < mask.rows; ++i)
  {
    for (int j = 0; j < mask.cols; ++j)
    {
      if (!mask.at<uchar>(i,j))
        continue;
      hist[imgHSV.at<Vec3b>(i,j)[0]]++;
    }
  }
  int maxH = 0;
  for (int i = 0; i < 256; ++i)
  {
    maxH = max(maxH, hist[i]);
  }
  std::vector<int> Hpeaks;
   for (int i = 0; i < 256; ++i)
    {
      if(hist[i]>0.5*maxH)
        Hpeaks.push_back(i);
    }
  

  // normalize histogram
  int hist_w = 512; int hist_h = 400;
  for (int i = 0; i < 256; ++i)
  {
    hist[i] = (hist[i] *hist_h)/maxH;
  }
  

  int bin_w = cvRound( (double) hist_w/256. );
  Mat histImg(hist_h, hist_w, CV_8U, Scalar(0));
  for (int i = 1; i < 256; i++) {
    line( histImg, Point( bin_w*(i-1), hist_h - cvRound(hist[i-1]) ) ,
                       Point( bin_w*(i), hist_h - cvRound(hist[i]) ),
                       Scalar( 255), 2, 8, 0  );
  }
  imshow("histogram", histImg);
  // draw all pixels with H < 50 degrees. Troublesome in 1.png?
  
  Mat yellow;
  cvtColor(avgImg,yellow,CV_HSV2BGR);
  //Mat yellow = img.clone();
  for (int i = 0; i < yellow.rows; ++i)
  {
    for (int j = 0; j < yellow.cols; ++j)
    {
      if (!mask.at<uchar>(i,j))
        yellow.at<Vec3b>(i,j) = Vec3b(0,0,0);
      // else
      //   yellow.at<Vec3b>(i,j) = Vec3b(0,0,0);

      // if (imgHSV.at<Vec3b>(i,j)[0] >= 50/2.) // since H = deg/2 to fit in 255
      //   yellow.at<Vec3b>(i,j) = Vec3b(0,0,0);
    }
  }
  imshow("weird", yellow);
  imwrite("weird2.png", yellow);
  waitKey();
  /*for(int p=0;p<Hpeaks.size();p++)
  {
    for (int i = 0; i < yellow.rows; ++i)
    {
      for (int j = 0; j < yellow.cols; ++j)
      {
        if(imgHSV.at<Vec3b>(i,j)[0]>Hpeaks[p]-5 && imgHSV.at<Vec3b>(i,j)[0]<Hpeaks[p]+5){
          if (!mask.at<uchar>(i,j))
            yellow.at<Vec3b>(i,j) = Vec3b(0,0,0);
        }
        
        // else
        //   yellow.at<Vec3b>(i,j) = Vec3b(0,0,0);

        // if (imgHSV.at<Vec3b>(i,j)[0] >= 50/2.) // since H = deg/2 to fit in 255
        //   yellow.at<Vec3b>(i,j) = Vec3b(0,0,0);
      }
    }
    imshow("weird", yellow);
    imwrite("weird2.png", yellow);
    waitKey();
  }*/
  
  return 0;
}
