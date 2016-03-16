#include "graphstruct.h"

GraphStruct::GraphStruct(QString xtitle, QString ytitle, QString title): xtitle(xtitle), ytitle(ytitle), title(title)
{

}

void GraphStruct::push_back(const PlotStructure &plot, QString legend)
{
    plots.push_back(plot);
    plotLegends.push_back(legend);
}

