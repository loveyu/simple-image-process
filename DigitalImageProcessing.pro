#-------------------------------------------------
#
# Project created by QtCreator 2014-11-26T17:59:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DigitalImageProcessing
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mypainterwidget.cpp \
    imageaction.cpp \
    backimg.cpp

HEADERS  += mainwindow.h \
    mypainterwidget.h \
    ActionType.h \
    imageaction.h \
    backimg.h
TRANSLATIONS = zh_CN.ts
FORMS    += mainwindow.ui

#win32:CONFIG += console

RESOURCES += \
    icon.qrc
RC_FILE += icon.rc

OTHER_FILES += \
    icon.rc
CONFIG += c++11
