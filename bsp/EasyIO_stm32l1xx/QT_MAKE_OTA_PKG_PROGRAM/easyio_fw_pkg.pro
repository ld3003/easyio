#-------------------------------------------------
#
# Project created by QtCreator 2015-12-18T10:11:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = easyio_fw_pkg
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    md5sum.c

HEADERS  += mainwindow.h \
    md5sum.h

FORMS    += mainwindow.ui
