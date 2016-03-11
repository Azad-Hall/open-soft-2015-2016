#include <bits/stdc++.h>

using namespace std;

class PDFbuilder
{
	string latex_string;
public:
	PDFbuilder();
	void beginDocument();
	void addImage(const string& image_file);
	void addTable(const vector< vector<string> >& table);
	void endDocument();
	bool renderPDF(const string& filename);
};