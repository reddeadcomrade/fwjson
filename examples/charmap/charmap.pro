#-------------------------------------------------
#
# Project created by QtCreator 2012-07-19T10:20:11
#
#-------------------------------------------------

QT       += core

QT       -= gui

win32 {
   TARGET = ../../../bin/charmap
}
else {
   TARGET = ../../bin/charmap
}

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../include

LIBS += \
      -L../../bin \
      -lfwjson1

SOURCES += main.cpp
