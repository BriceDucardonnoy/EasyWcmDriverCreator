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
    treemibmodel.cpp \
    mibnode.cpp \
    mibitem.cpp

HEADERS  += mainwindow.h \
    treemibmodel.h \
    mibnode.h \
    mibitem.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
