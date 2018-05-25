#-------------------------------------------------
#
# Project created by QtCreator 2018-05-25T03:58:23
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lumina-image-viewer
TEMPLATE = app

SOURCES += main.cpp viewer.cpp
HEADERS += viewer.h

CONFIG += link_pkgconfig
PKGCONFIG += Magick++
