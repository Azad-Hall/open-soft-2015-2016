TEMPLATE = app

QT += widgets

HEADERS += mainwindow.h \
    pdfprocessor.h \
    plotstructure.h \
    pugixml.hpp \
    customitem.h \
    graphstruct.h

SOURCES += mainwindow.cpp\
           main.cpp \
    pdfprocessor.cpp \
    plotstructure.cpp \
    pugixml.cpp \
    customitem.cpp \
    graphstruct.cpp

RESOURCES += \
    graphextractor.qrc


CONFIG += c++11

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qpdflib/release/ -lqpdf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qpdflib/debug/ -lqpdf
else:unix: LIBS += -L$$OUT_PWD/../qpdflib/ -lqpdf

INCLUDEPATH += $$PWD/../qpdflib
DEPENDPATH += $$PWD/../qpdflib


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qcustomplot/release/ -lqcp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qcustomplot/debug/ -lqcp
else:unix: LIBS += -L$$OUT_PWD/../qcustomplot/ -lqcp

INCLUDEPATH += $$PWD/../qcustomplot
DEPENDPATH += $$PWD/../qcustomplot
