#include "pdfprocessor.h"

PDFProcessor::PDFProcessor(QString filePath, QObject *parent) : QObject(parent), filePath(filePath), outputFilePath(filePath), outputFilePathSet(false) {}

void PDFProcessor::run() {
    emit initialize();

    process = new QProcess();
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
    process->start("/bin/bash", QStringList() << "run.sh" << filePath);
    process->waitForFinished(-1);

    emit loadOutputFile(outputFilePath);
    emit wrapUpSuccessfully();
    emit finished();
}


void PDFProcessor::readOutput() {
    process->setReadChannel(QProcess::StandardOutput);
    while (process->canReadLine()) {

        QString line(process->readLine());
        QTextStream stream(&line);
        int value;
        QString status;
        stream >> value >> status;

        if(status.isEmpty())
            continue;

        if(!outputFilePathSet) {
            outputFilePath = status;
            outputFilePathSet = true;
        }
        else {
            emit updateStatus(status, value);
        }
    }
}
