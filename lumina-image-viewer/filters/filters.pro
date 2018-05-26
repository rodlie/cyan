QT += core gui widgets
TEMPLATE      = lib
CONFIG       += plugin shared
INCLUDEPATH  += ../
HEADERS       = filters.h filterdialog.h
SOURCES       = filters.cpp filterdialog.cpp
TARGET        = $$qtLibraryTarget(ImageFilters)
DESTDIR       = ../app/plugins

CONFIG += link_pkgconfig
PKGCONFIG += Magick++
