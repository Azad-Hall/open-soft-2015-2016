// given the binary images of each color, and x and y scales,
// make the table.
// we also need x,y axis labels , legend labels and graph title also, should keep this in the xml file only.
// TODO:
// * align xsamples with text detedcted.
// * take median/mean instead of first pixel in yscan, or more sophisticated
// * interpolation
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <math.h>
#include <algorithm>
#include <map>

using namespace cv;
using namespace std;
#include "box-detection.hpp"
#include "pugixml.hpp"
#include <algorithm>
#include <fstream>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>

using namespace std;
using namespace cv;

map<int, string> hueColor;

string getColorName(int hue){
  return hueColor[(hue + 5)%360/20];
}

// take pixels xsamples as input, gives pixels ysamples as output.
vector<pair<bool, int> > getData(Mat bin, vector<int> xsamples) {
  vector<pair<bool, int> > ysamples;
  for (int i = 0; i < xsamples.size(); i++) {
    int x = xsamples[i];
    assert(x >= 0 && x < bin.cols);
    bool found = false;
    vector<int> val;
    for (int i = 0; i < bin.rows; i++) {
      int intensity = bin.at<uchar>(i,x);
      // should probably see all non-zero pixels and choose one
      if (intensity > 0) {
        found = true;
        val.push_back(i);
      }
    }
    // take median
    sort(val.begin(), val.end());
    if (found)
      ysamples.push_back(make_pair(found, val[val.size()/2]));
    else
      ysamples.push_back(make_pair(found, 0));
  }
  return ysamples;
}

double pixToVal(int pix, double scale, double refPix, double refVal) {
  return refVal + (pix-refPix)*scale;
}
// column for data of a graph
vector<string> getColumn(string title, vector<pair<bool, int> > samples, double scale, double refPix, double refVal) {
  vector<string> ret;
  ret.push_back(title);
  for (int i = 0; i < samples.size(); i++) {
    if (samples[i].first == false)
      ret.push_back(" - ");
    else {
      char buf[1000];
      double v = pixToVal(samples[i].second, scale, refPix, refVal);
      sprintf(buf, "%.3lf", v);
      ret.push_back(string(buf));
    }
  }
  return ret;
}

vector<pair<bool, int> > interpolate(vector<int> xsamples, vector<pair<bool, int> > ysamples){
  int degree = 3, np = 0;
  // cout << xsamples.size() << endl;
  vector<double> x, y;
  int ind = 0;

  int start,start1,end,end1;
  for(int i = 0  ; i < ysamples.size() ; i++ ){
    if ( ysamples[i].first == true){
      x.push_back(xsamples[i]);
      y.push_back(ysamples[i].second);
      ind++;
    }
  }
  // make sure atleast 2 poitns are there in truee samples..
  assert(x.size() >= 2);
  // check if start point is needed
  if (ysamples[0].first == false) {
    double x1 = x[0], x2 = x[1];
    double y1 = y[0], y2 = y[1];
    double m = (y2-y1)/(x2-x1);
    double c = y1 - m*x1;
    // get the y value
    double x0 = xsamples[0];
    double y0 = x0*m+c;
    x.insert(x.begin(), x0);
    y.insert(y.begin(), y0);
  }
  // check if end point is needed
  if (ysamples[ysamples.size()-1].first == false) {
    int sz = y.size();
    double x1 = x[sz-1], x2 = x[sz-2];
    double y1 = y[sz-1], y2 = y[sz-2];
    double m = (y2-y1)/(x2-x1);
    double c = y1 - m*x1;
    // get the y value
    double x0 = xsamples[xsamples.size()-1];
    double y0 = x0*m+c;
    x.insert(x.end(), x0);
    y.insert(y.end(), y0);
  }
  np = y.size();
  // for (int i = 0; i < np; ++i)
  // {
  //   printf("%lf ", x[i]);
  // }
  // printf("\n");
  // for (int i = 0; i < np; i++) {
  //   printf("%lf", y[i]);
  // }
  // printf("\n");
  gsl_interp_accel *acc = gsl_interp_accel_alloc ();
  gsl_spline *spline  = gsl_spline_alloc (gsl_interp_cspline, np); 

  //cout<<"np = "<<np << " " << xsamples.size() <<endl;
  gsl_spline_init (spline, &x[0], &y[0], np);

  for(int i = 0 ; i < ysamples.size(); i++ ){
    if ( ysamples[i].first == false){
        if (!(xsamples[i] >= x[0])) {
          assert(0);
        }
        ysamples[i].second = gsl_spline_eval(spline, xsamples[i], acc);
		ysamples[i].first = true;
    }
  }
   gsl_spline_free (spline);
   gsl_interp_accel_free (acc);

  //  //computing for end points using linear extrapolation
  // start = ysamples.size()-1;end = 1;
  // for(int i = 1 ; i < ysamples.size() - 1; i++ ){
  //   if( ysamples[i].first == true && start==ysamples.size()-1)
  //     {start = i;/*cout<<"starta "<<start<<"\n";*/}
  //   if( ysamples[ysamples.size() - i].first == true && end==1)
  //     {end = ysamples.size() - i;/*cout<<"enda "<<end<<"\n";*/}
  // }
  // start1 = start;end1 = end;
  // while(ysamples[++start1].first==false)
  // {
  //   ++start1;
  // }
  // //cout<<"start1 "<<start1<<"\n";
  // while(ysamples[--end1].first==false)
  // {
  //   --end1;
  // }
  // //cout<<"end1 "<<end1<<"\n";
  //   ysamples[0].second = ( ysamples[start1].second - ysamples[start].second ) * xsamples[0] / ( xsamples[start1] - xsamples[start] ) ;
   
  //   ysamples[ysamples.size()-1].second = ( ysamples[end].second - ysamples[end1].second ) * xsamples[ysamples.size()-1] / ( xsamples[end] - xsamples[end1] ) ;
    
  return ysamples;
}



int main(int argc, char const *argv[])
{
  printf("usage: ./gen-table <xml-file> <binimg-basename> <outxml-file>\n");
  hueColor[0] = "red";
  hueColor[1] = "orange";
  hueColor[2] = "yellowish orange";
  hueColor[3] = "yellow";
  hueColor[4] = "lime";
  hueColor[5] = "light green";
  hueColor[6] = "green";
  hueColor[7] = "dark green";
  hueColor[8] = "cyanish green";
  hueColor[9] = "cyan";
  hueColor[10] = "light blue";
  hueColor[11] = "blue";
  hueColor[12] = "dark blue";
  hueColor[13] = "indigo";
  hueColor[14] = "violet";
  hueColor[15] = "purple";
  hueColor[16] = "magenta";
  hueColor[17] = "rose red";

  vector<Point> contour;
  // read the bb conotur;
  for (int i = 0; i < 4; i++) {
    int x, y;
    scanf("%d %d\n", &x, &y);
    contour.push_back(Point(x,y));
  }
  Rect bb = boundingRect(contour);
  Point bl(bb.tl().x, bb.br().y), br(bb.br());
  int n = 0;
  // read number of binary images from stdin
  scanf("%d\n", &n);
  // read the least count granularity. keep it atleast 10 or we will get infinite loop...
  int lc = 10;
  scanf("%d\n", &lc);
  if (lc == 0)
    lc = 10;
  // read xml
  pugi::xml_document doc;
  printf("loading xml %s\n", argv[1]);
  pugi::xml_parse_result result = doc.load_file(argv[1]);
  std::cout << "Load result: " << result.description() <<  std::endl;
  double xscale, yscale;
  double xrefPix, xrefVal, yrefPix, yrefVal;
  string htext, vtext, title;
  pugi::xml_node xs = doc.child("xscale");
  pugi::xml_node ys = doc.child("yscale");
  htext = doc.child("horizontal_text").attribute("htxt").value();
  vtext = doc.child("Vertical_text").attribute("vtxt").value();
  title = doc.child("Title_text").attribute("ttxt").value();
  xscale = stof(xs.attribute("valPerPix").value());
  printf("htext = %s, vtext = %s\n", htext.c_str(), vtext.c_str());
  // since yscale shoudl decrease
  yscale = -stof(ys.attribute("valPerPix").value());
  xrefPix = stof(xs.attribute("xrefCoord").value());
  xrefVal = stof(xs.attribute("xrefValue").value());
  yrefPix = stof(ys.attribute("yrefCoord").value());
  yrefVal = stof(ys.attribute("yrefValue").value());
  // make the samples array
  double xOffset = 0;
  // push the xsamples by an offset, since we want them to align
  // with the axis text.
  double closestDiff = 1e15;
  // make xsamples only with lest count
  vector<int> xsamples;
  assert (lc > 0);
  for (int x = bl.x; x <= br.x; x += lc) {
    xsamples.push_back(x);
  }
  // find xOffset using this xsamples
  for (int i = 0; i < xsamples.size(); i++) {
    double offset = xrefPix - xsamples[i];
    if (fabs(offset) < fabs(closestDiff)) {
      closestDiff = offset;
      printf("diff = %d, xsample new value = %d\n", (int)offset, (int)xsamples[i]+(int)offset);
    }
  }
  xsamples.clear();
  printf("offset = %lf, xstart = %d, xend = %d, incr = %d\n", closestDiff, bl.x + (int)closestDiff, 
    br.x, (int)(lc/10.));
  assert(lc/10. >= 1);
  for (int x = bl.x+closestDiff; x <= br.x; x += lc/10.) {
    xsamples.push_back(x);
  }
  vector<pair<bool, int> > xsamples_p;
  for (int i = 0 ; i < xsamples.size(); i++) {
    xsamples_p.push_back(make_pair(true, xsamples[i]));
  }
  vector<vector<string> > table;
  table.push_back(getColumn(htext, xsamples_p, xscale, xrefPix, xrefVal));
  for (int i = 0; i < n; i++) {
    char buf[1000];
    sprintf(buf, "%s-%d.png", argv[2], i);
    Mat bin = imread(buf, 0);
    vector<pair<bool, int> > ysamples = getData(bin, xsamples);
    
    ysamples = interpolate(xsamples,ysamples);
    // need leegend text here
    table.push_back(getColumn(buf, ysamples, yscale, yrefPix, yrefVal));
  }
  // for (int i = 0; i < table.size(); ++i)
  // {
  //   printf("col %d: \n", i);
  //   for (int j = 0; j < table[i].size(); ++j)
  //   {
  //     printf("%s ", table[i][j].c_str());
  //   }
  //   printf("\n");
  // }
  // transose the table
  assert(table.size()>0);
  for (int i =1 ; i < table.size(); i++) 
    assert(table[i].size() == table[i-1].size());
  int sz = table[0].size();
  vector<vector<string> > table2(sz, vector<string>(table.size(), ""));
  for (int i = 0; i < table.size(); ++i)
  {
    for (int j = 0; j < table[i].size(); j++) {
      table2[j][i] = table[i][j];
    }
  }
  table = table2;
  // write it out in xml format so it can be read later
  using namespace pugi;
  pugi::xml_document odoc;
  pugi::xml_parse_result result2 = odoc.load_file(argv[3]);
  xml_node tablenode = odoc.append_child("table");
  tablenode.append_attribute("title") = title.c_str();
  for (int i = 0; i < table.size(); i++) {
    xml_node tr = tablenode.append_child();
    tr.set_name("tr");
    for (int j = 0; j < table[i].size(); j++) {
      xml_node td = tr.append_child();
      td.set_name("td");
      td.text().set(table[i][j].c_str());
    }
  }
  ofstream ofile;
  ofile.open(argv[3]);
  odoc.print(ofile);
  ofile.close();
  // PDFbuilder builder;

  // builder.beginDocument();
  // builder.addImage("/home/utkarsh/Desktop/images/scan-0.png");
  // vector< vector <string> > table = { {"cell1", "cell2", "cell3"},
  //                {"cell4", "cell5", "cell6"},
  //                {"cell7", "cell8", "cell9"}
  //              };
  // builder.addTable(table, "Cell Table");
  // builder.addImage("/home/utkarsh/Desktop/images/scan-1.png");
  // builder.addImage("/home/utkarsh/Desktop/images/scan-2.png");
  // builder.addImage("/home/utkarsh/Desktop/images/scan-3.png");
  // builder.addImage("/home/utkarsh/Desktop/images/scan-4.png");
  // builder.endDocument();

  // builder.renderPDF("output");
  return 0;
}
