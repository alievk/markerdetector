#-------------------------------------------------
#
# Project created by QtCreator 2016-03-07T13:28:05
#
#-------------------------------------------------

QT       -= core gui

TARGET = markerdetector
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    markerdetector.cpp \
    utils.cpp

HEADERS += \
    markerdetector.h \
    utils.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

include(../opencv.pri)
