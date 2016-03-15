#include "pdfbuilder.h"

PDFbuilder::PDFbuilder() {
	latex_string += "\\documentclass{article}\n";
	latex_string += "\\usepackage[utf8]{inputenc}\n";
	latex_string += "\\usepackage{graphicx}\n";
	latex_string += "\\usepackage{geometry}\n";
	latex_string += "\\usepackage{longtable}\n";
	latex_string += "\\usepackage{multirow}\n";
	latex_string += "\\title{}\n";
	latex_string += "\\author{Team 7}\n";
}

void PDFbuilder::beginDocument() {
	latex_string += "\\begin{document}\n";
	latex_string += "\\maketitle\n";
}

void PDFbuilder::addImage(const string& image_file) {
	latex_string += "\\newpage\n";
	latex_string += "\\newgeometry{left=0pt,top=0pt,right=0pt,bottom=0pt}\n";
	latex_string += "\\includegraphics[width=\\paperwidth,height=\\paperheight]{" + image_file +"}\n";
	latex_string += "\\restoregeometry\n";
}

void PDFbuilder::addTable(const vector< vector<string> >& table, const string& title, const string &x_title, const string &y_title) {
	if(table.empty()) return;	

	int rows = table.size();
	if(rows == 0)
		return;
	int cols = table[0].size();

	latex_string += "\\begin{center}\n";
	if(!title.empty() and title != "NA") latex_string += "\\caption{" + preprocess(title) +"}\n";	
	latex_string += "\\begin{longtable}{ |";
	for (int i = 0; i < cols; ++i)
	{
		latex_string += "c|";
	}
	latex_string += " }\n";
	
	latex_string += "\\multicolumn{" + to_string(cols) + "}{|r|}{{Continued on next page}} \\\\ \\hline\n";
	latex_string += "\\endfoot\n";
	latex_string += "\\endlastfoot\n";
	latex_string += "\\hline\n";
	latex_string += "\\multirow{2}{*}{ " + preprocess(table[0][0]) + " } & \\multicolumn{" + to_string(cols-1) + "}{|c|}{{" + preprocess(y_title) + "}}\\\\\n";
	latex_string += "\\cline{2-" + to_string(cols) + "}\n";
	
	for (int j = 1; j < cols; ++j)
	{	
		latex_string += "& ";
		latex_string += preprocess(table[0][j]) + " ";
	}
	latex_string += "\\\\\n";
	latex_string += "\\cline{1-" + to_string(cols) + "}\n";
	
	for (int i = 1; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			latex_string += preprocess(table[i][j]) + " ";
			if(j != cols - 1)
				latex_string += "& ";			
		}
		latex_string += "\\\\\n";
		latex_string += "\\hline\n";
	}
	latex_string += "\\end{longtable}\n";	
	latex_string += "\\end{center}\n";
}

void PDFbuilder::endDocument() {
	latex_string += "\\end{document}\n";
}

bool PDFbuilder::renderPDF(const string& filename) {
	ofstream out;
	string tex_filename = filename + ".tex";

	out.open(tex_filename.c_str(), ios::out | ios::trunc );
	out << latex_string << endl;
	out.close();

	string command = "latexmk -cd -e -f -pdf -interaction=nonstopmode -synctex=1 " + tex_filename;
	system(command.c_str());
}

string PDFbuilder::preprocess(const string& str) {
	string ret;

	for (int i = 0; i < str.size(); ++i)
	{
		if(find(special_chars.begin(), special_chars.end(), str[i]) != special_chars.end()) {
			ret.push_back('\\');
		}
		ret.push_back(str[i]);
	}

	return ret;
}

// int main(int argc, char const *argv[])
// {
// 	PDFbuilder builder;

// 	builder.beginDocument();
// 	builder.addImage("/home/utkarsh/Desktop/images/scan-0.png");
// 	vector< vector <string> > table = { {"&cell1", "%cell2", "$cell3"},
// 								  {"#cell4", "_cell5", "c{ell6"},
// 								  {"c}ell7", "ce~ll8", "ce^ll9"},
// 								  {"cell\\10", "", ""}
// 								};
// 	builder.addTable(table, "Cell Table", "x", "y");
// 	builder.addImage("/home/utkarsh/Desktop/images/scan-1.png");
// 	builder.addImage("/home/utkarsh/Desktop/images/scan-2.png");
// 	builder.addImage("/home/utkarsh/Desktop/images/scan-3.png");
// 	builder.addImage("/home/utkarsh/Desktop/images/scan-4.png");
// 	builder.endDocument();

// 	builder.renderPDF("output");

// 	return 0;
// }

