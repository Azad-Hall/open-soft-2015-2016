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

#include <QApplication>
#include <QTimer>
#include "mainwindow.h"

// This is a simple application that demonstrated the QPdfWidget usage.
// The main function here can accept a single (optional) argument to trigger
// the PDF file loading from the command prompt.

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();
    mainWindow.showMaximized();

    if (argc > 1) {
        // Load file provided as an argument
        QString filePath = QString::fromLatin1(argv[1]);
        QTimer::singleShot(0, [filePath, &mainWindow]() {
            mainWindow.loadFile(filePath);
        });
    }

    return app.exec();
}
