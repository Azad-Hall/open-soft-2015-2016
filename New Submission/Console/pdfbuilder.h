#include <bits/stdc++.h>

using namespace std;

class PDFbuilder
{
	vector<char> special_chars = {'&', '%', '$', '#', '_', '{', '}', '~', '^', '\\'};
	string latex_string;

	string preprocess(const string& str);
public:
	PDFbuilder();
	void beginDocument();
	void addImage(const string& image_file);
	void addTable(const vector< vector<string> >& table, const string& title, const string &x_title, const string &y_title);
	void endDocument();
	bool renderPDF(const string& filename);
};