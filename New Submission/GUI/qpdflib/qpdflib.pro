TEMPLATE = lib
TARGET = qpdf

CONFIG += c++11

QT += widgets\
      webengine\
      webenginecore\
      webenginewidgets

DEFINES += QPDFLIB_BUILD

HEADERS = webengineview.h\
          qpdfwidget.h\
          qpdf.h

SOURCES = webengineview.cpp\
          qpdfwidget.cpp

RESOURCES += pdfview.qrc
