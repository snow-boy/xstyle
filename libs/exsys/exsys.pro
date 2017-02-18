#-------------------------------------------------
#
# Project created by QtCreator 2016-07-27T17:18:13
#
#-------------------------------------------------

QT       -= gui

TARGET = exsys

include($$PWD/../dll.pri)

SOURCES += pluginmanager.cpp

HEADERS  += \
    iplugin.h \
    pluginmanager.h \
    pluginmacros.h \
    lsignal.h \
    objectptr.h \
    exsys_global.h

CONFIG += c++11

DEFINES += EXTENSIONSYSTEM_LIBRARY
