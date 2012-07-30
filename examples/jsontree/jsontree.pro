QT      += core gui

TARGET = ../../bin/jsontree

TEMPLATE = app

INCLUDEPATH += ../../include/

win32 {
    LIBS += -L../../../bin \
            -lfwjson1
}
else {
    LIBS += -L../../bin \
            -lfwjson1
}

SOURCES  += main.cpp\
            mainwindow.cpp

HEADERS  += mainwindow.h

FORMS += \
    mainwindow.ui
