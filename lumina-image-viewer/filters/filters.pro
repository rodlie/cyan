TEMPLATE      = lib
CONFIG       += plugin static
INCLUDEPATH  += ../
HEADERS       = filters.h
SOURCES       = filters.cpp
TARGET        = $$qtLibraryTarget(imageFilters)
DESTDIR       = ../app/plugins

CONFIG += link_pkgconfig
PKGCONFIG += Magick++
