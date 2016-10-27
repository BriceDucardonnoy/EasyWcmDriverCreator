#-------------------------------------------------
#
# Project created by QtCreator 2016-10-18T14:09:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EasyWcmDriverCreator
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qmibitem.cpp \
    qtreemibmodel.cpp

HEADERS  += mainwindow.h \
    qmibitem.h \
    qtreemibmodel.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
