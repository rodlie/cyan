#
# Cyan <https://cyan.fxarena.net>
# Copyright Ole-André Rodlie, FxArena DA.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#

include($${top_srcdir}/share/common.pri)

TARGET = Cyan
TEMPLATE = lib

DEFINES += CYAN_LIBRARY
DEFINES += CYAN_IMAGE_FORMAT_VERSION=\"\\\"$${VERSION}\\\"\"

SOURCES += CyanImageFormat.cpp CyanTileItem.cpp CyanLayerItem.cpp CyanView.cpp CyanColorConvert.cpp
HEADERS += CyanImageFormat.h CyanTileItem.h CyanLayerItem.h CyanView.h CyanColorConvert.h CyanWidgetApi.h CyanGlobal.h

# Components taken from Qt4 Solutions a long time ago, maintained here
SOURCES += qtcolorpicker.cpp qtcolortriangle.cpp qtwindowlistmenu.cpp
HEADERS += qtcolorpicker.h qtcolortriangle.h qtwindowlistmenu.h
