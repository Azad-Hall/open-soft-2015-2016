#include "pdfbuilder.h"

PDFbuilder::PDFbuilder() {
	latex_string += "\\documentclass{article}\n";
	latex_string += "\\usepackage[utf8]{inputenc}\n";
	latex_string += "\\usepackage{graphicx}\n";
	latex_string += "\\usepackage{geometry}\n";
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

void PDFbuilder::addTable(const vector< vector<string> >& table, const string& title) {
	int rows = table.size();
	if(rows == 0)
		return;
	int cols = table[0].size();

	latex_string += "\\begin{table}\n";
	latex_string += "\\centering\n";
	latex_string += "\\begin{tabular}{ |";
	for (int i = 0; i < cols; ++i)
	{
		latex_string += "c|";
	}
	latex_string += " }\n";
	latex_string += "\\hline\n";

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			latex_string += table[i][j] + " ";
			if(j != cols - 1)
				latex_string += "& ";

		}
		latex_string += "\\\\\n";
		latex_string += "\\hline\n";
	}

	latex_string += "\\end{tabular}\n";

	if(!title.empty())
		latex_string += "\\caption{" + title +"}\n";
	latex_string += "\\end{table}\n";
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

int main(int argc, char const *argv[])
{
	PDFbuilder builder;

	builder.beginDocument();
	builder.addImage("/home/utkarsh/Desktop/images/scan-0.png");
	vector< vector <string> > table = { {"cell1", "cell2", "cell3"},
								  {"cell4", "cell5", "cell6"},
								  {"cell7", "cell8", "cell9"}
								};
	builder.addTable(table, "Cell Table");
	builder.addImage("/home/utkarsh/Desktop/images/scan-1.png");
	builder.addImage("/home/utkarsh/Desktop/images/scan-2.png");
	builder.addImage("/home/utkarsh/Desktop/images/scan-3.png");
	builder.addImage("/home/utkarsh/Desktop/images/scan-4.png");
	builder.endDocument();

	builder.renderPDF("output");

	return 0;
}

