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
#include <string>
#include <algorithm>
#include <exception>
#include <cstdlib>
#include <queue>
#include <bits/stdc++.h>
using namespace cv;
using namespace std;
using namespace cv;
using namespace pugi;

typedef struct a  {
  Point a,b;
  double x;
  string text;
} labels;

typedef struct b  {
  double x,a,b;
  int z;
} scale; 

double startx=0,starty=0,startCoordinatex=0,startCoordinatey=0,to_add=0;
queue<string> Q_str;
queue<pair<string, pair<Point, Point> > > Q_str_point_ver;
queue<pair<string, pair<Point, Point> > > Q_str_point_hor;
double yValue(double yCoordinate,double scale){
	double temp;
	temp=startCoordinatey-yCoordinate;
	return (starty+temp*scale);
}
double xValue(double xCoordinate,double scale){
	double temp;
	temp=xCoordinate-startCoordinatex;
	return (startx+temp*scale);
}
const char* node_types[] =
{
  "null", "document", "element", "pcdata", "cdata", "comment", "pi", "declaration"
};

//[code_traverse_walker_impl
struct simple_walker: pugi::xml_tree_walker
{
  virtual bool for_each(pugi::xml_node& node)
  {
        for (int i = 0; i < depth(); ++i) std::cout << "  "; // indentation

          std::cout << node_types[node.type()] << ": name='" << node.name() << "', value='" << node.value() << "'\n";
        if(node_types[node.type()] == "pcdata") {
          Q_str.push(node.value());
        }
        return true; // continue traversal
      }
    };

double yScale(vector<labels> labelv,double yAxisLength){
  vector<scale> y_scale;
  scale temp1;
  vector<double> y_scale2;
  double v;

  for(int i=0;i<labelv.size();i++){
    try{
      double temp=atof(labelv[i].text.c_str());
    }
    catch(exception e){
      continue;
    }
    for(int j=i+1;j<labelv.size();j++){
      double a1,a2;
      try{
        a1=::atof(labelv[i].text.c_str());
        a2=::atof(labelv[j].text.c_str());
        temp1.x=(a2-a1)/(labelv[i].x-labelv[j].x);
        temp1.z=0;
        temp1.a=a1;
        temp1.b=labelv[i].x;
        y_scale.push_back(temp1);
        cout<<"\nVertical Scale:     "<<temp1.x;
      }
      catch(exception e){
        continue;
      }
    }
  }



  int z=0;
  for(int k=0;k<y_scale.size();k++){
    z=0;
    for(int i=0;i<labelv.size();i++){
      try{
        double temp=::atof(labelv[i].text.c_str());
      }
      catch(exception e){
        continue;
      }
      for(int j=i+1;j<labelv.size();j++){
        double a1,a2;
        try{
          a1=::atof(labelv[i].text.c_str());
          a2=::atof(labelv[j].text.c_str());
          v=labelv[i].x-labelv[j].x;
          if((y_scale[k].x*(v - yAxisLength/250)<(a2-a1))&&(y_scale[k].x*(v + yAxisLength/250)>(a2-a1))) {//threshold
            z++;
          }
        }
        catch(exception e){
          continue;
        }
      }
    }
    y_scale[k].z=z;
  }

  int w=90; 
  bool flag=true;
  while(y_scale2.size()==0&&w>0){
    w=w-5;
    for(int i=0;i<y_scale.size();i++){
  if(y_scale[i].z>=y_scale.size()*w/100) {//thresholding handled by while loop
    y_scale2.push_back(y_scale[i].x);
    if(flag) {
    	starty=y_scale[i].a;
    	startCoordinatey=y_scale[i].b;
    	flag=false;
    }
  } //returns scale of y axis in per pixel
}
}
//cout<<"\n"<<w<<"   ";
v=0;
for(int i=0;i<y_scale2.size();i++) v=v+y_scale2[i];//Taking average of inline scales
v=v/y_scale2.size();                    //Vertical Scale Approximated
return v;
}


double xScale(vector<labels> labelh,double xAxisLength){ 
  vector<scale> x_scale;
  scale temp1;
  vector<double> x_scale2;
  double h;
  for(int i=0;i<labelh.size();i++){
    try{
      double temp=atof(labelh[i].text.c_str());
    }
    catch(exception e){
      continue;
    }
    for(int j=i+1;j<labelh.size();j++){
      double a1,a2;
      try{
        a1=atof(labelh[i].text.c_str());
        a2=atof(labelh[j].text.c_str());
        temp1.x=(a2-a1)/(labelh[j].x-labelh[i].x);
        temp1.z=0;
        temp1.a=a1;
        temp1.b=labelh[i].x;
        x_scale.push_back(temp1);
        cout<<"\nHorizontal Scale:     "<<temp1.x;

      }
      catch(exception e){
        continue;
      }
    }
  }




  int z=0;
  for(int k=0;k<x_scale.size();k++){
    z=0;
    for(int i=0;i<labelh.size();i++){
      try{
        double temp=atof(labelh[i].text.c_str());
      }
      catch(exception e){
        continue;
      }
      for(int j=i+1;j<labelh.size();j++){
        double a1,a2;
        try{
          a1=atof(labelh[i].text.c_str());
          a2=atof(labelh[j].text.c_str());
          h=labelh[j].x-labelh[i].x;
          if( (x_scale[k].x*(h-xAxisLength/250) < a2-a1) && (x_scale[k].x*(h+xAxisLength/250) > a2-a1)  ) {//threshold
            z++;
          }
        }
        catch(exception e){
          continue;
        }
      }
    }
    x_scale[k].z=z;
  }

  int w=90;
  bool flag=true;
  while(x_scale2.size()==0&&w>0){
    w=w-5;
    for(int i=0;i<x_scale.size();i++) {
  if(x_scale[i].z>=x_scale.size()*w/100) {//thresholding handled by while loop
    x_scale2.push_back(x_scale[i].x);
    if(flag){
    	startx=x_scale[i].a;
    	startCoordinatex=x_scale[i].b;
    	flag=false;
    }
  }     //returns scale of x axis in per pixel
}
} 
//cout<<"\n"<<w<<"   ";
h=0;
for(int i=0;i<x_scale2.size();i++) h=h+x_scale2[i];//taking average of inline scales
h=h/x_scale2.size();                //Horizontal Scale Approximation  
return h;
}

double xGranularity(vector<labels> labelh,double xAxisLength){
	vector<scale> x_scale;
	scale temp1;
	vector<double> x_scale2;
	double h;
	int z=0;

	for(int i=0;i<labelh.size();i++){
		for(int j=i+1;j<labelh.size();j++){
		double dist=labelh[j].x-labelh[i].x;
		temp1.x=dist;
		temp1.z=0;
		x_scale.push_back(temp1);
		}
	}
	
	for(int k=0;k<x_scale.size();k++){
	z=0;
	for(int i=0;i<labelh.size();i++){
		for(int j=i+1;j<labelh.size();j++){
		double dist=labelh[j].x-labelh[i].x;
		if((x_scale[k].x*(j-i)<dist+xAxisLength/200)&&(x_scale[k].x*(j-i)>dist-xAxisLength/200)){      //thresholding
			z++;
		}	
		}
	}
	x_scale[k].z=z;
	}

	int w=90; 
	while(x_scale2.size()==0&&w>0){
		w=w-5;
		for(int i=0;i<x_scale.size();i++){
		if(x_scale[i].z>=x_scale.size()*w/100) {//thresholding handled by while loop
		x_scale2.push_back(x_scale[i].x);
	}
}
}
//cout<<"\nw:    "<<w;
h=0;
for(int i=0;i<x_scale2.size();i++) h=h+x_scale2[i];//Taking average of inline scales
h=h/x_scale2.size();                    //Vertical Scale Approximated
return h/10;
}

string toString(double d){
std::ostringstream os;
os << d;
std::string str = os.str();
return str;
}



int main(int argc, char* argv[])
{
	double xscale,yscale,xgran;
	string hor_text,ver_text,title_text;


		//reading from xml file "indoc.xml" created by text_seg.cpp
    pugi::xml_document indoc;
    if (!indoc.load_file("indoc.xml")) return -1;

		pugi::xml_node to_add_y = indoc.child("to_add_y");
		pugi::xml_attribute attr = to_add_y.first_attribute();
		to_add=atof(attr.value());

				pugi::xml_node veri_text= indoc.child("ver_text");
		pugi::xml_attribute attr1 = veri_text.first_attribute();
		ver_text=attr1.value();

				pugi::xml_node hori_text = indoc.child("hori_text");
		pugi::xml_attribute attr2 = hori_text.first_attribute();
		hor_text=attr2.value();

pugi::xml_node title_text1 = indoc.child("title_text");
		pugi::xml_attribute attr3 = title_text1.first_attribute();
		title_text=attr3.value();

pugi::xml_document doc;
pugi::xml_parse_result result = doc.load_file("tes_out0.hocr");
simple_walker walker;
doc.traverse(walker);
xml_node main_wrapper = doc.child("html").child("body").child("div");


vector<labels> labelh,labelv1,labelv;
labels temp2;
double v,h;
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
          temp2.text=Q_str.front();
          temp2.a=a;
          temp2.b=b;
          temp2.x=(a.y+b.y)/2;
          labelv1.push_back(temp2);
          Q_str_point_ver.push({Q_str.front(), {a, b}});
          Q_str.pop();
        }
      }
  labelv=labelv1;
  for(int i=0;i<labelv1.size();i++) {
    labelv[labelv1.size()-1-i]=labelv1[i];
  }
  for(int i=0;i<labelv.size();i++) {
    cout<<"\n"<<labelv[i].text;
  }
    v=yScale(labelv,(labelv[0].x-labelv[labelv.size()-1].x));
  cout<<"\n\nVertical Scale Final:      "<<v;
  
pugi::xml_document doc1;
pugi::xml_parse_result result1 = doc1.load_file("tes_out1.hocr");
simple_walker walker1;
doc1.traverse(walker1);
xml_node main_wrapper1 = doc1.child("html").child("body").child("div");

for(xml_node x = main_wrapper1.child("div"); x; x = x.next_sibling("div") )
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
          temp2.text=Q_str.front();
          temp2.a=a;
          temp2.b=b;
          temp2.x=(a.x+b.x)/2;
          labelh.push_back(temp2);
          Q_str_point_hor.push({Q_str.front(), {a, b}});
          Q_str.pop();
        }
      }
for(int i=0;i<labelh.size();i++) {
    cout<<"\n"<<labelh[i].text;
  }
    h=xScale(labelh,(labelh[labelh.size()-1].x-labelh[0].x));
  cout<<"\n\nHorizontal Scale Final:      "<<h;

cout<<"\nStartx:               "<<startx;
cout<<"\nStartcoordinatex:     "<<startCoordinatex;
cout<<"\nStarty:               "<<starty;
cout<<"\nStartcoordinatey:     "<<startCoordinatey;

	startCoordinatey=startCoordinatey+to_add;
	pugi::xml_document outdoc;

  // add node with some name
  pugi::xml_node y_label = outdoc.append_child();
  y_label.set_name("y_label");

 /* pugi::xml_node y_label_tl_x = y_label.append_child();
  y_label_tl_x.set_name("y_label_tl_x");
   pugi::xml_node y_label_tl_y = y_label.append_child();
  y_label_tl_y.set_name("y_label_tl_y");
   pugi::xml_node y_label_br_x = y_label.append_child();
  y_label_br_x.set_name("y_label_br_x");
   pugi::xml_node y_label_br_y = y_label.append_child();
  y_label_br_y.set_name("y_label_br_y");*/

  for(int i=0;i<labelv.size();i++){
  	string s=labelv[i].text;
  	y_label.append_attribute(s.c_str()) = labelv[i].x;
  	/*y_label_tl_x.append_attribute(s.c_str())=labelv[i].a.x;
  	y_label_tl_y.append_attribute(s.c_str())=labelv[i].a.y;
  	y_label_br_x.append_attribute(s.c_str())=labelv[i].b.x;
  	y_label_br_y.append_attribute(s.c_str())=labelv[i].b.y;*/
  }

  pugi::xml_node x_label = outdoc.append_child();
  x_label.set_name("x_label");

 /* pugi::xml_node x_label_tl_x = x_label.append_child();
  x_label_tl_x.set_name("x_label_tl_x");
   pugi::xml_node x_label_tl_y = x_label.append_child();
  x_label_tl_y.set_name("x_label_tl_y");
   pugi::xml_node x_label_br_x = x_label.append_child();
  x_label_br_x.set_name("x_label_br_x");
   pugi::xml_node x_label_br_y= x_label.append_child();
  x_label_br_y.set_name("x_label_br_y");*/

  for(int i=0;i<labelh.size();i++){
  	string s=labelh[i].text;
  	x_label.append_attribute(s.c_str()) = labelh[i].x;
  	/*x_label_tl_x.append_attribute(s.c_str())=labelv[i].a.x;
  	x_label_tl_y.append_attribute(s.c_str())=labelv[i].a.y;
  	x_label_br_x.append_attribute(s.c_str())=labelv[i].b.x;
  	x_label_br_y.append_attribute(s.c_str())=labelv[i].b.y;*/
  }
 
  pugi::xml_node xnode = outdoc.append_child();
  xnode.set_name("xscale");
  xnode.append_attribute("valPerPix") = h;
  xnode.append_attribute("xrefCoord") = startCoordinatex;
  xnode.append_attribute("xrefValue") = startx;
  pugi::xml_node ynode = outdoc.append_child();
  ynode.set_name("yscale");
  ynode.append_attribute("valPerPix") = v;
  ynode.append_attribute("yrefCoord") = startCoordinatey;
  ynode.append_attribute("yrefValue") = starty;

 	pugi::xml_node htxt = outdoc.append_child();
  htxt.set_name("horizontal_text");
  htxt.append_attribute("htxt") = hor_text.c_str();

  pugi::xml_node vtxt = outdoc.append_child();
  vtxt.set_name("Vertical_text");
  vtxt.append_attribute("vtxt") = ver_text.c_str();

   pugi::xml_node ttxt = outdoc.append_child();
  ttxt.set_name("Title_text");
  ttxt.append_attribute("ttxt") = title_text.c_str();

  // pugi::xml_node 
  ofstream xml_out;
  xml_out.open("scale.xml");
  outdoc.print(xml_out);
  xml_out.close();

return 0;
}