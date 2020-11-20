#-------------------------------------------------
#
# Project created by QtCreator 2016-11-22T14:42:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SubdivCurves
TEMPLATE = app

SOURCES += main.cpp\
    curvenetrenderer.cpp \
    curverenderer.cpp \
        mainwindow.cpp \
    mainview.cpp \
    settings.cpp \
    subdivisioncurve.cpp

HEADERS  += mainwindow.h \
    curvenetrenderer.h \
    curverenderer.h \
    mainview.h \
    renderer.h \
    settings.h \
    subdivisioncurve.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
