#-------------------------------------------------
#
# Project created by QtCreator 2013-10-10T07:08:44
#
#-------------------------------------------------

QT       += core gui network multimedia script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TediumRemedy
TEMPLATE = app

RC_ICONS = icon/TediumRemedy.ico
#ICON = icon/TediumRemedy.icns

SOURCES += main.cpp\
        mainwindow.cpp \
    typingbox.cpp \
    stranger.cpp \
    spy.cpp \
    russtranger.cpp \
    chinastranger.cpp \
    cometclient.cpp \
    strangerprefswindow.cpp \
    intereststextedit.cpp \
    wavsound.cpp

HEADERS  += mainwindow.h \
    typingbox.h \
    stranger.h \
    spy.h \
    russtranger.h \
    chinastranger.h \
    cometclient.h \
    strangerprefswindow.h \
    intereststextedit.h \
    wavsound.h

FORMS    += mainwindow.ui \
    strangerprefswindow.ui

OTHER_FILES += \
    stylesheet.qss \
    Readme.txt \
    stylesheet_bright.qss

RESOURCES += \
    Resources.qrc


