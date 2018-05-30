#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

QT += core gui widgets
TEMPLATE = lib
CONFIG += plugin shared
INCLUDEPATH += ../
HEADERS = filters.h filterdialog.h common.h
SOURCES = filters.cpp filterdialog.cpp
TARGET = $$qtLibraryTarget(ImageFilters)
DESTDIR = ../app/plugins

CONFIG += link_pkgconfig
PKGCONFIG += Magick++

include(../../lumina-extra.pri)
CONFIG -= staticlib

target.path = $${PIXEL_PLUGINS}
INSTALLS += target
