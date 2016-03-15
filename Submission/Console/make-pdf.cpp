// takes img base name as input, and makes the pdf
#include "pdfbuilder.h"
#include <stdio.h>

#include <iostream>
#include <math.h>
#include <algorithm>

using namespace std;
#include "pugixml.hpp"
#include <algorithm>
#include <fstream>

using namespace pugi;
using namespace std;

int main(int argc, char const *argv[])
{
  if (argc != 3) {
    printf("usage: ./make-pdf <basename> <outname>\n");
    return 0;
  }
  // get the number of pages from stdin
  int n;
  scanf("%d", &n);
  printf("number of pages= %d\n", n);
  PDFbuilder builder;
  builder.beginDocument();
  for (int i = 0; i < n; i++) {
    char buf[1000];
    sprintf(buf, "%s-%d.png", argv[1], i);
    builder.addImage(buf);
    pugi::xml_document odoc;
    sprintf(buf, "%s-%d-table.xml", argv[1], i);
    pugi::xml_parse_result result2 = odoc.load_file(buf);
    for (pugi::xml_node n = odoc.first_child(); n; n = n.next_sibling()) {
      vector<vector<string> > table;
      string title = n.attribute("title").value();
      for (xml_node tr = n.first_child(); tr; tr = tr.next_sibling()) {
        table.push_back(vector<string>());
        for (xml_node td = tr.first_child(); td; td = td.next_sibling()) {
          table[table.size()-1].push_back(td.child_value());
        }
      }
      printf("adding table with title %s\n", title.c_str());
      builder.addTable(table, title);
    }
  }
  builder.endDocument();
  printf("out = %s\n", argv[2]);
  builder.renderPDF(argv[2]);
 // builder.addImage("/home/utkarsh/Desktop/images/scan-0.png");
 // vector< vector <string> > table = { {"cell1", "cell2", "cell3"},
 //                 {"cell4", "cell5", "cell6"},
 //                 {"cell7", "cell8", "cell9"}
 //               };
 // builder.addTable(table, "Cell Table");
 // builder.addImage("/home/utkarsh/Desktop/images/scan-1.png");
 // builder.addImage("/home/utkarsh/Desktop/images/scan-2.png");
 // builder.addImage("/home/utkarsh/Desktop/images/scan-3.png");
 // builder.addImage("/home/utkarsh/Desktop/images/scan-4.png");
 // builder.endDocument();

 // builder.renderPDF("output");
  return 0;
}