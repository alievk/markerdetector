#-------------------------------------------------
#
# Project created by QtCreator 2016-03-06T21:12:32
#
#-------------------------------------------------

QT       += testlib

QT       += gui

TARGET = tst_teststest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    markerdetectortest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    markerdetectortest.h

RESOURCES += \
    data.qrc

include(../opencv.pri)
include(../markerdetector.pri)

