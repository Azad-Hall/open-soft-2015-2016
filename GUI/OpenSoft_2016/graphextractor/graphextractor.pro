TEMPLATE = app

QT += widgets

HEADERS += mainwindow.h

SOURCES += mainwindow.cpp\
           main.cpp

RESOURCES += \
    graphextractor.qrc


CONFIG += c++11

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qpdflib/release/ -lqpdf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qpdflib/debug/ -lqpdf
else:unix: LIBS += -L$$OUT_PWD/../qpdflib/ -lqpdf

INCLUDEPATH += $$PWD/../qpdflib
DEPENDPATH += $$PWD/../qpdflib
