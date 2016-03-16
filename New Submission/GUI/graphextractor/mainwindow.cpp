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

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *pParent, Qt::WindowFlags flags)
    : QMainWindow(pParent, flags)
{
    setWindowTitle("Graph Extractor");
    setWindowIcon(QIcon(":/icons/app.png"));

    viewChanger = new QComboBox();

    viewChanger->addItem("PDF View");
    viewChanger->addItem("Graph view");

    statusProgressBar = new QProgressBar();
    statusProgressBar->setMinimum(0);
    statusProgressBar->setMaximum(100);
    statusProgressBar->setTextVisible(false);

    stackedWidget = new QStackedWidget();
    treeWidget = new QTreeWidget();
    pdfWidget = new QPdfWidget();
    plotWidget = new QCustomPlot();

    QSplitter* splitter = new QSplitter();
    splitter->addWidget(treeWidget);
    splitter->addWidget(plotWidget);
    splitter->setStretchFactor(0,1);
    splitter->setStretchFactor(1,15);

    stackedWidget->addWidget(pdfWidget);
    stackedWidget->addWidget(splitter);

    setCentralWidget(stackedWidget);

    createActions();
    createMenus();
    createToolBar();
    createStatusBar();

    connect(viewChanger, SIGNAL(activated(int)), stackedWidget, SLOT(setCurrentIndex(int)));
    connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(plotItem(QTreeWidgetItem*,int)));
}

void MainWindow::loadFile(const QString &path)
{
    if (pdfWidget->loadFile(path)) {
        // Set the output file path for save as
        outFilePath = path;
        //Enable SaveAs
        saveAsAct->setEnabled(true);
        // Update window title with the file name
        QFileInfo fi(path);
        setWindowTitle(fi.fileName());
    }
}

void MainWindow::createActions()
{
    openAct = new QAction(QIcon(":/icons/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(openFile()));

    saveAsAct = new QAction(QIcon(":/icons/save.png"),tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveToFile()));
    saveAsAct->setEnabled(false);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus() {

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBar()
{
    QToolBar *pToolBar = addToolBar(tr("File"));
    pToolBar->setMovable(false);
    pToolBar->addAction(openAct);
    pToolBar->addAction(saveAsAct);
    pToolBar->addWidget(viewChanger);
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage("Ready");
    statusBar()->addPermanentWidget(statusProgressBar);
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open PDF file"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    tr("PDF file (*.pdf)"));
    if (fileName.isEmpty()) {
        wrapUpOpenFileWithFailure();
        return;
    }

    processorThread = new QThread();
    processor = new PDFProcessor(fileName);
    processor->moveToThread(processorThread);

    connect(processorThread, SIGNAL(started()), processor, SLOT(run()));
    connect(processor, SIGNAL(finished()), processorThread, SLOT(quit()));

    connect(processor, SIGNAL(finished()), processor, SLOT(deleteLater()));
    connect(processorThread, SIGNAL(finished()), processorThread, SLOT(deleteLater()));

    connect(processor, SIGNAL(initialize()), this, SLOT(initializeOpenFile()));
    connect(processor, SIGNAL(updateStatus(QString,int)), this, SLOT(updateStatusBar(QString,int)));
    connect(processor, SIGNAL(loadOutputFile(QString)), this, SLOT(loadFile(QString)));
    connect(processor, SIGNAL(loadGraphs(QVector<QVector<GraphStruct > >)), this, SLOT(loadGraphs(QVector<QVector<GraphStruct > >)));
    connect(processor, SIGNAL(wrapUpSuccessfully()), this, SLOT(wrapUpOpenFileWithSuccess()));
    connect(processor, SIGNAL(wrapUpWithError()), this, SLOT(wrapUpOpenFileWithFailure()));

    processorThread->start();
}

void MainWindow::saveToFile() {

    if(outFilePath.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Nothing to save!!"));
    }

    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("pdf");
    QStringList files;
    if(dialog.exec())
        files = dialog.selectedFiles();
    else
        return;

    return saveFile(files.at(0));
}

void MainWindow::saveFile(const QString &fileName) {
    // No need to copy to same path
    if(fileName == outFilePath)
        return;

    if(!QFile::copy(outFilePath, fileName)) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot write file %1:\n.")
                             .arg(fileName));
        return;
    }

    QFileInfo fi(fileName);
    setWindowTitle(fi.fileName());

}

void MainWindow::initializeOpenFile() {
    //Disable Open
    openAct->setEnabled(false);
    // Initialize status bar
    updateStatusBar("Loading!!", 0);
}

void MainWindow::updateStatusBar(QString msg, int value) {
    statusBar()->showMessage(msg);
    statusProgressBar->setValue(value);
}

void MainWindow::wrapUpOpenFileWithSuccess() {
    // Enable Open
    openAct->setEnabled(true);
}

void MainWindow::wrapUpOpenFileWithFailure() {
    // Enable Open
    openAct->setEnabled(true);
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About"),
                tr("<b>Graph Extractor</b> is a graph extracting tool "
                   "from scanned PDF files. For user guide and software documentation "
                   "please open the Documentation.pdf file."));
}

void MainWindow::loadGraphs(QVector<QVector<GraphStruct > > graphs) {
    this->graphs = graphs;
    treeWidget->clear();
    for (int page = 0; page < graphs.size(); ++page) {
        CustomItem* page_item = new CustomItem(page);
        page_item->setText(0,tr("page %1").arg(page));
        for (int graph = 0; graph < graphs[page].size(); ++graph) {
            CustomItem* graph_item = new CustomItem(page, graph);
            graph_item->setText(0, tr("graph %1").arg(graph));
            page_item->addChild(graph_item);
        }
        treeWidget->addTopLevelItem(page_item);
    }
}

void MainWindow::plotItem(QTreeWidgetItem *item, int column) {
    CustomItem* c_item =  dynamic_cast<CustomItem*>(item);

    if(c_item->i == -1 || c_item->j == -1)
        return;

    plot(graphs[c_item->i][c_item->j]);
}

void MainWindow::plot(const GraphStruct &graph) {
    plotWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    plotWidget->xAxis2->setVisible(true);
    plotWidget->xAxis2->setTickLabels(false);
    plotWidget->clearGraphs();
    plotWidget->legend->setVisible(true);

    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    plotWidget->legend->setFont(legendFont);
    plotWidget->legend->setBrush(QBrush(QColor(255,255,255,230)));
    plotWidget->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
    for (int i = 0; i < graph.plots.size(); ++i) {
        plotWidget->addGraph();
        plotWidget->graph(i)->setData(graph.plots[i].x, graph.plots[i].y);
        plotWidget->graph(i)->setPen(QPen(graph.plots[i].color));
        plotWidget->graph(i)->setLineStyle(QCPGraph::lsLine);
        plotWidget->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
        plotWidget->graph(i)->setName(graph.plotLegends[i]);
    }
    plotWidget->xAxis->setLabel(graph.xtitle);
    plotWidget->yAxis->setLabel(graph.ytitle);
    plotWidget->xAxis2->setLabel(graph.title);
    plotWidget->rescaleAxes();
    plotWidget->replot();
}
