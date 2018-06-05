#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Cyan is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# Cyan is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>
#

QT += core gui widgets
TEMPLATE = lib
CONFIG += plugin shared
INCLUDEPATH += ../
HEADERS = filters.h filterdialog.h common.h
SOURCES = filters.cpp filterdialog.cpp
TARGET = $$qtLibraryTarget(ImageFilters)
DESTDIR = ../../app/plugins

CONFIG += link_pkgconfig
PKGCONFIG += Magick++

include(../../cyan.pri)
CONFIG -= staticlib

target.path = $${PIXEL_PLUGINS}
INSTALLS += target
