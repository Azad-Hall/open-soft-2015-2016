#include "pdfprocessor.h"

PDFProcessor::PDFProcessor(QString filePath, QObject *parent) : QObject(parent), num_pages(0) , filePath(filePath), outputFilePath(filePath), outputFilePathSet(false){}

void PDFProcessor::run() {
    emit initialize();

    process = new QProcess();
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    process->start("/bin/bash", QStringList() << "run.sh" << filePath);
    process->waitForFinished(-1);

    emit loadOutputFile(outputFilePath);

    QVector< QVector< GraphStruct > > graphs;

//    for (int page = 0; page < 3; ++page) {
//        graphs.push_back( QVector< QVector < PlotStructure > >() );
//        for (int graph = 0; graph < 3; ++graph) {
//            graphs[page].push_back( QVector< PlotStructure >() );
//            for (int plot = 0; plot < 4; ++plot) {
//                graphs[page][graph].push_back( PlotStructure() );
//                for (double x = 0; x < 10; ++x) {
//                    double y = (double)(qrand() % 1000);
//                    graphs[page][graph][plot].x.push_back(x);
//                    graphs[page][graph][plot].y.push_back(y);
//                }
//            }
//        }
//    }
    QFileInfo fi(filePath);

    QString dir = fi.baseName();
    dir += "-dir";

    for (int i = 0; i < num_pages; ++i) {
        QString path = dir + "/" + tr("scan-%1-table.xml").arg(i);
        QVector< GraphStruct > page = parseXML(path);
        graphs.push_back(page);
    }

    emit loadGraphs(graphs);
    emit wrapUpSuccessfully();
    emit finished();
}


void PDFProcessor::readOutput() {
    process->setReadChannel(QProcess::StandardOutput);
    while (process->canReadLine()) {

        QString line(process->readLine());
        int value;
        QString status;
        char buff[1000];

        std::sscanf(line.toStdString().c_str(), " %d %[^\n]", &value, buff);
        status = QString::fromLocal8Bit(buff);
        if(status.isEmpty())
            continue;

        if(!outputFilePathSet) {
            num_pages = value;
            outputFilePath = status;
            outputFilePathSet = true;
        }
        else {
            emit updateStatus(status, value);
        }
    }
}

QVector<GraphStruct> PDFProcessor::parseXML(QString xmlFilePath) {
    QVector< GraphStruct > plots;
    pugi::xml_document odoc;
    pugi::xml_parse_result result = odoc.load_file(xmlFilePath.toStdString().c_str());
    int graph = 0;

    for (pugi::xml_node n = odoc.first_child(); n; n = n.next_sibling()) {
        QVector< double > hues;
        QString xtitle, ytitle, title;
        int count = 0;
        for(pugi::xml_attribute attr = n.first_attribute() ; attr ; attr=attr.next_attribute()) {
            if(count < 2) {
                if (count == 1) {
                    // get ytitle
                    ytitle = QString(attr.value());
                } else if (count == 0) {
                    // get title
                    title = QString(attr.value());
                }
                count++;
                continue;
            }
            double hue_value = QString(attr.value()).toDouble();
            hues.push_back(hue_value);
            count++;
        }
        plots.push_back(GraphStruct("", ytitle, title) );
        QVector<QString> legends;
        int row = 0;
        for (pugi::xml_node tr = n.first_child(); tr; tr = tr.next_sibling()) {
            int col = 0;
            double xVal;
            for (pugi::xml_node td = tr.first_child(); td; td = td.next_sibling()) {                
                if(row != 0) {
                    if(col == 0) {
                        xVal = QString(td.child_value()).toDouble();
//                        for (int i = 0; i < plots[graph].size(); ++i) {
//                            plots[graph][i].x.push_back(QString(td.child_value()).toDouble());
//                        }
                    }
                    else {
                        bool ok = true;
//                        plots[graph][col -1].y.push_back(
                        double yVal = QString(td.child_value()).toDouble(&ok);
                        if (ok) {
                            plots[graph].plots[col-1].y.push_back(yVal);
                            plots[graph].plots[col-1].x.push_back(xVal);
                        }
                    }
                } else {
                    if (col == 0) {
                        plots[graph].xtitle = QString(td.child_value());
                    } else {
                        legends.push_back(QString(td.child_value()));
                    }
                }
                col++;
            }
            if(row == 0) {
                // Determine number of plots in the graph
                plots[graph].plotLegends = legends;
                for (int i = 0; i < col - 1; ++i) {
                    plots[graph].push_back(PlotStructure(), legends[i]);
                    plots[graph].plots[i].color.setHsvF(hues[i],1,1);
                }
            }
            row++;
        }
        graph++;
    }

    return plots;
}
