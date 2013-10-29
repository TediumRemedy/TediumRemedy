#-------------------------------------------------
#
# Project created by QtCreator 2013-10-10T07:08:44
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TediumRemedy
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    typingbox.cpp \
    stranger.cpp \
    spy.cpp

HEADERS  += mainwindow.h \
    typingbox.h \
    stranger.h \
    spy.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    stylesheet.qss \
    Readme.txt
