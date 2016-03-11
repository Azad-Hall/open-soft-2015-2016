#ifndef PDFPROCESSOR_H
#define PDFPROCESSOR_H

#include <QObject>
#include <QString>

class PDFProcessor : public QObject
{
    Q_OBJECT
public:
    explicit PDFProcessor(QString filePath, QObject *parent = 0);

signals:
    void initialize();
    void loadOutputFile(QString fileName);
    void updateStatus(QString msg, int value);
    void wrapUpWithError();
    void wrapUpSuccessfully();
    void finished();

public slots:
    void run();

private:
    QString filePath;
};

#endif // PDFPROCESSOR_H
