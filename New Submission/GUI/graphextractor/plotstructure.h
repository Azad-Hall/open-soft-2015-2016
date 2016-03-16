#ifndef PLOTSTRUCTURE_H
#define PLOTSTRUCTURE_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QColor>

class PlotStructure
{
public:
    QVector<double> x;
    QVector<double> y;

    QColor color;

    PlotStructure(double hue = 0.0);
};




#endif // PLOTSTRUCTURE_H
