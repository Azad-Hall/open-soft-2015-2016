/*
                          graphextractor

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QToolBar>
#include <QIcon>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFileInfo>
#include <QMessageBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QComboBox>
#include <QThread>
#include <QStackedWidget>
#include "QPdfWidget"
#include "QCustomPlot"
#include <QTreeWidget>
#include <QSplitter>
#include <QVector>
#include "plotstructure.h"
#include "pdfprocessor.h"
#include "customitem.h"

class QMenu;
class QProgressBar;
class QAction;
class QPdfWidget;

/**
 * @brief PDF viewer main window.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    MainWindow(QWidget *pParent = nullptr, Qt::WindowFlags flags = 0);

public slots:

    void loadFile(const QString &path);

private slots:

    void openFile();
    void saveToFile();
    void about();
    void initializeOpenFile();
    void updateStatusBar(QString msg, int value);
    void loadGraphs( QVector< QVector< QVector< PlotStructure > > > graphs );
    void plotItem(QTreeWidgetItem * item, int column);
    void wrapUpOpenFileWithSuccess();
    void wrapUpOpenFileWithFailure();

private:
    void createMenus();
    void createActions();
    void createToolBar();
    void createStatusBar();
    void saveFile(const QString &fileName);
    void plot(const QVector< PlotStructure >&);

    QMenu *fileMenu;
    QMenu *helpMenu;
    QComboBox *viewChanger;
    QProgressBar *statusProgressBar;
    QAction *openAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QStackedWidget *stackedWidget;
    QPdfWidget *pdfWidget;
    QTreeWidget *treeWidget;
    QCustomPlot *plotWidget;
    QThread *processorThread;
    PDFProcessor *processor;
    QString outFilePath;
    QVector< QVector< QVector< PlotStructure > > > graphs;
};

#endif // MAINWINDOW_H
