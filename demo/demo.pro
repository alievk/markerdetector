#-------------------------------------------------
#
# Project created by QtCreator 2016-03-07T14:12:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = demo
TEMPLATE = app


SOURCES += main.cpp\
        demomainwindow.cpp \
    imagewidget.cpp \
    imgconvert.cpp

HEADERS  += demomainwindow.h \
    imagewidget.h \
    imgconvert.h

FORMS    += demomainwindow.ui

include(../markerdetector.pri)
include(../opencv.pri)
