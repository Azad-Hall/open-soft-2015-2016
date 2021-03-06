#ifndef PDFPROCESSOR_H
#define PDFPROCESSOR_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QTextStream>
#include <string>
#include "plotstructure.h"
#include "pugixml.hpp"
#include <QFileInfo>
#include <cstdio>
#include <cstdlib>
#include "graphstruct.h"
class PDFProcessor : public QObject
{
    Q_OBJECT
public:
    explicit PDFProcessor(QString filePath, QObject *parent = 0);

signals:
    void initialize();
    void loadOutputFile(QString fileName);
    void loadGraphs(QVector< QVector< GraphStruct > > graphs);
    void updateStatus(QString msg, int value);
    void wrapUpWithError();
    void wrapUpSuccessfully();
    void finished();

public slots:
    void run();

private slots:
    void readOutput();

private:
    QVector< GraphStruct > parseXML(QString xmlFilePath);
    int num_pages;
    QString filePath;
    QString outputFilePath;
    QProcess* process;
    bool outputFilePathSet;
};

#endif // PDFPROCESSOR_H
