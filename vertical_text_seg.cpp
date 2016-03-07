//11-12 no vertical text detection :/

#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "pugixml.hpp"
#include <sstream>
#include <fstream>
using namespace std;
using namespace cv;
using namespace pugi;
RNG rng(12345);


int main(int argc, char* argv[])
{


/*
	Corners are co-ordinates of the corners of bounding box.
	We search the whole image column-wise, wherever there is more than img.cols/10 pixels as black
	we take that as a reference and crop out the text around it.
*/


	string a="4.png";
	//a[1]=48+ii;
	cout<<"------>"<<a;
	cv::Mat input = cv::imread(a.c_str());//input image
	float input_area=input.cols*input.rows;


	
	cv::Mat gray;
	cv::cvtColor(input, gray, CV_BGR2GRAY);
	int max = 70;
	
	namedWindow("input");
	//createTrackbar("Threshold", "input",&max,100);
	
	cv::Mat mask;
	cv::threshold(gray, mask,240, 255, CV_THRESH_BINARY_INV );
	//Canny(mask, mask, 20, 100, 3);
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	Mat input_new;
	cv::Mat drawing2 = cv::Mat::zeros(mask.size(), CV_8UC3);
	cv::Point2f corners[4];
			
	for (int i = 0; i < contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(0, 255, 0);
		drawContours(drawing2, contours, i, color, 1, 8, hierarchy, 0, cv::Point());

		float ctArea = cv::contourArea(contours[i]);

		if (ctArea > (input_area * 40.00 / 100.00)){		// area of bounding is greater than 70%
			cv::Rect boundingBox = cv::boundingRect(contours[i]);
			corners[0] = boundingBox.tl();
			corners[1] = Point2f(boundingBox.tl().x + boundingBox.width, boundingBox.tl().y);
			corners[2] = boundingBox.br();
			corners[3] = Point2f(boundingBox.br().x - boundingBox.width, boundingBox.br().y);
		}	
	}
	
	rectangle(input, corners[0] + Point2f(5,5), corners[2] - Point2f(5,5), Scalar(255, 255, 255), -1); // change the random value

	cv::imshow("input", input);
	cv::imshow("drawing", drawing2);



	Mat img = imread("4.png", 0);
	
    cout<<"\n"<<corners[0]<<corners[1]<<corners[2]<<corners[3];

    long long contain_sum = 0;
    Vector<int> contain(img.cols,0);
    for(int j=0; j<img.cols; j++) {
        
    	for (int i=0;i<img.rows;i++) {
        
        	// for(int k=-1; k<=1 ;k++)
        	// {
        	// 	for(int l=-1;l<=1;l++) {
        	// 		if(i+k>=0 && j+l>=0 && j+l<img.cols && i+k<img.rows)
        	int k=0, l=0;
        			if(img.at<uchar>(i+k,j+l) < 100) {
        				contain[j] += 1;
        			}
        	// 	}
        	// }


        }

        contain_sum += contain[j];
        //cout<<"\n"<<j<<" "<<contain[j];
    }
    int first_ver = 0;
    int second_ver = 0;
    cout<<"\n-->"<<corners[0].x;
    for(int i=0; i<corners[0].x; i++)
    {
    	if(contain[i]>img.cols/10)
    		{
    			cout<<"\n"<<i<<" "<<contain[i];
    			if(first_ver==0)
    				first_ver = i;
    			else if(i-first_ver < img.rows*3/100)
    				first_ver = (first_ver+i)/2;
    			else if(second_ver==0)
    				second_ver = i;

    		}
    }



    cout<<"\n"<<first_ver<<" "<<second_ver;
    int third_ver = (first_ver+second_ver)/2;

    Mat vert_text(img, Rect(Point2f(first_ver - (third_ver - first_ver), corners[0].y), Point2f(third_ver, corners[3].y))); // change the random value
    Mat y_label(img, Rect(Point2f(third_ver, corners[0].y), Point2f(corners[3].x, corners[3].y)));

    imshow("NEw", img);
    
    imshow("vert_text", vert_text);

    imshow("y_label", y_label);

    Mat res;
    cv::transpose(y_label, res);

	cv::flip(res, res, 1);
	
    imshow("y_label2", res);
    imwrite("vert_text.jpg",res);
    waitKey(0);

}

