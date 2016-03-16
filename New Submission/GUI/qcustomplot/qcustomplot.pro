TEMPLATE = lib
TARGET = qcp

CONFIG += c++11

QT += core\
      printsupport\
      widgets

DEFINES += QCUSTOMPLOT_COMPILE_LIBRARY

HEADERS = qcustomplot.h \
    qcp.h

SOURCES = qcustomplot.cpp
