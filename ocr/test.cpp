#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/background_segm.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "pugixml.hpp"
#include <sstream>
using namespace std;
using namespace cv;
using namespace pugi;
RNG rng(12345);

int main(int argc, char* argv[])
{
	Mat img = imread("sel.png", 0);
	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	pugi::xml_document doc;
	system("tesseract sel.png out hocr");
	pugi::xml_parse_result result = doc.load_file("out.hocr");
	xml_node main_wrapper = doc.child("html").child("body").child("div");

    for(xml_node x = main_wrapper.child("div"); x; x = x.next_sibling("div") )
    {
    	for(xml_node gist_up = x.child("p"); gist_up; gist_up = gist_up.next_sibling("p"))
    		for(xml_node gist = gist_up.child("span"); gist; gist = gist.next_sibling("span"))
		    for(xml_node gist_inside = gist.child("span"); gist_inside; gist_inside = gist_inside.next_sibling("span"))
		    	if(gist_inside.attribute("title")) {
		    		std::istringstream iss;
		    		iss.str(gist_inside.attribute("title").value());
		    		cout<<"\n"<<gist_inside.attribute("title").value();
		    		string bbox;
		    		iss>>bbox;
		    		Point a, b;
		    		iss>>a.x;
		    		iss>>a.y;
		    		iss>>b.x;
		    		iss>>b.y;
		    		cout<<"\n "<<a.x<<" "<<a.y;
		    		rectangle(img, a, b, color, 2, 8, 0);
		    	}
    }
    imwrite("Final.png", img);
    
}


