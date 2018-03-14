#-------------------------------------------------
#
# Project created by QtCreator 2018-01-17T00:15:00
#
#-------------------------------------------------

QT       -= gui

QT += network
CONFIG += c++11

TARGET = clilib
TEMPLATE = lib

DEFINES += CLILIB_LIBRARY

SOURCES += clilib.cpp

HEADERS += clilib.h\
        clilib_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
