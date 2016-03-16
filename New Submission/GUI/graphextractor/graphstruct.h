#ifndef GRAPHSTRUCT_H
#define GRAPHSTRUCT_H

#include "plotstructure.h"
class GraphStruct {
public:
    QString xtitle, ytitle;
    QString title;
    QVector<PlotStructure> plots;
    QVector<QString> plotLegends;
    GraphStruct(QString xtitle = "", QString ytitle = "", QString title="");
    void push_back(const PlotStructure& plot, QString legend);
};

#endif // GRAPHSTRUCT_H
