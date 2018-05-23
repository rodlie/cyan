#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

QT += core gui dbus
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lumina-power-manager
TEMPLATE = app

SOURCES += main.cpp systray.cpp
HEADERS += systray.h
RESOURCES += ../lumina-power-manager.qrc
LIBS += -L../lib -lPower
INCLUDEPATH += ..  ../lib

CONFIG += link_pkgconfig
PKGCONFIG += x11 xscrnsaver
#LIBS += -lX11 -lXss

include(../../lumina-extra.pri)

target.path = $${PREFIX}/bin
target_desktop.path = $${XDGDIR}/autostart
target_desktop.files = $${TARGET}.desktop
target_docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}
target_docs.files = ../../LICENSE ../../README.md
INSTALLS += target target_desktop target_docs
