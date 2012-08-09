#-------------------------------------------------
#
# Project created by QtCreator 2012-07-19T09:09:50
#
#-------------------------------------------------

QT       -= gui

win32 {
   TARGET = ../../bin/fwjson1
}
else {
   TARGET = ../bin/fwjson1
}

TEMPLATE = lib

DEFINES += FWJSON_LIBRARY

INCLUDEPATH += ../include/

HEADERS += \
           ../include/fwjson.h \
           ../include/fwjsonparser.h \
           ../include/fwjsoncharmap.h \
           ../include/fwjson_inl.h \
           ../include/fwjson_global.h \
           ../include/fwjsonexception.h \
    fwjsonparserhelper.h

SOURCES += \
     fwjsonparser.cpp \
     fwjson.cpp \
     fwjsonexception.cpp \
    fwjsonparserhelper.cpp

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE009F12E
    TARGET.CAPABILITY =
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = fwjson.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
