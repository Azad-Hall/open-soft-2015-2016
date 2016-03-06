/*
                          qpdf

    Copyright (C) 2015 Arthur Benilov,
    arthur.benilov@gmail.com
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
    setWindowIcon(QIcon(":/icons/pdf.png"));

    statusProgressBar = new QProgressBar();
    statusProgressBar->setMinimum(0);
    statusProgressBar->setMaximum(100);
    statusProgressBar->setTextVisible(false);

    pdfWidget = new QPdfWidget();
    setCentralWidget(pdfWidget);

    createActions();
    createMenus();
    createToolBar();
    createStatusBar();
}

void MainWindow::loadFile(const QString &path)
{
    if (pdfWidget->loadFile(path)) {
        // Update window title with the file name
        QFileInfo fi(path);
        setWindowTitle(fi.fileName());
    }
}

void MainWindow::createActions()
{
    openAct = new QAction(QIcon(":/icons/folder.png"), tr("&Open..."), this);
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
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage("Ready");
    statusBar()->addPermanentWidget(statusProgressBar);
}

void MainWindow::openFile()
{
    //Initialize status bar to Done!!
    statusBar()->showMessage("Loading...");
    statusProgressBar->setValue(0);

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open PDF file"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    tr("PDF file (*.pdf)"));
    if (fileName.isEmpty()) {
        return;
    }

    loadFile(fileName);

    // Set the processed file path here
    outFilePath = fileName;

    // Enable SaveAs
    saveAsAct->setEnabled(true);

    //Update status bar to Done!!
    statusBar()->showMessage("Done!!");
    statusProgressBar->setValue(100);
}

void MainWindow::saveToFile() {
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
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        file.close();
        return;
    }

    QString copyPath = file.fileName();

    file.remove();
    file.close();

    QFile::copy(outFilePath, copyPath);

    QFileInfo fi(copyPath);
    setWindowTitle(fi.fileName());
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About"),
                tr("<b>Graph Extractor</b> is a graph extracting tool "
                   "from scanned PDF files. This application is developed by Team ID."));
}
