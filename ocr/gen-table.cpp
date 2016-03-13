// given the binary images of each color, and x and y scales,
// make the table.
// we also need x,y axis labels , legend labels and graph title also, should keep this in the xml file only.

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <math.h>
#include <algorithm>

using namespace cv;
using namespace std;
#include "box-detection.hpp"
#include "pugixml.hpp"
#include <algorithm>
#include <fstream>
using namespace std;
using namespace cv;
// take pixels xsamples as input, gives pixels ysamples as output.
vector<pair<bool, int> > getData(Mat bin, vector<int> xsamples) {
  vector<pair<bool, int> > ysamples;
  for (int i = 0; i < xsamples.size(); i++) {
    int x = xsamples[i];
    assert(x >= 0 && x < bin.cols);
    bool found = false;
    int val = 0;
    for (int i = 0; i < bin.rows; i++) {
      int intensity = bin.at<uchar>(i,x);
      // should probably see all non-zero pixels and choose one
      if (intensity > 0) {
        found = true;
        val = i;
        break;
      }
    }
    ysamples.push_back(make_pair(found, val));
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
      sprintf(buf, "%.2lf", v);
      ret.push_back(string(buf));
    }
  }
  return ret;
}
int main(int argc, char const *argv[])
{
  printf("usage: ./gen-table <xml-file> <binimg-basename> <outxml-file>\n");
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
  // read the least count granularity
  int lc = 10;
  scanf("%d\n", &lc);
  if (lc == 0)
    lc = 10;
  // make the samples array
  vector<int> xsamples;
  for (int x = bl.x; x <= br.x; x += lc/10.) {
    xsamples.push_back(x);
  }
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
    // need leegend text here
    table.push_back(getColumn(buf, ysamples, yscale, yrefPix, yrefVal));
  }
  for (int i = 0; i < table.size(); ++i)
  {
    printf("col %d: \n", i);
    for (int j = 0; j < table[i].size(); ++j)
    {
      printf("%s ", table[i][j].c_str());
    }
    printf("\n");
  }
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
  xml_node html = odoc.append_child("html");
  xml_node body = html.append_child("body");
  for (int i = 0; i < table.size(); i++) {
    xml_node tr = body.append_child();
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
