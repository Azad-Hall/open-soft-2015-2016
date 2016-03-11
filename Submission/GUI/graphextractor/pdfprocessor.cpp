#include "pdfprocessor.h"

PDFProcessor::PDFProcessor(QString filePath, QObject *parent) : QObject(parent), filePath(filePath) {}

void PDFProcessor::run() {
    emit initialize();
    emit updateStatus("Initiating...", 0);
    emit loadOutputFile(filePath);
    emit updateStatus("Done!!", 100);
    emit wrapUpSuccessfully();
    emit finished();
}
