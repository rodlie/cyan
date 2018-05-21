#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

TARGET = lumina-screensaver-service
VERSION = 1.0.0
TEMPLATE = app

QT += core dbus
QT -= gui
CONFIG += console
CONFIG -= app_bundle

SOURCES += main.cpp
HEADERS += screensaver.h
INCLUDEPATH += ../lumina-power-manager/lib

include(../lumina-extra.pri)

target.path = $${PREFIX}/bin
target_desktop.path = $${XDGDIR}/autostart
target_desktop.files = $${TARGET}.desktop
target_docs.path = $$DOCDIR/$${TARGET}-$${VERSION}
target_docs.files = ../LICENSE ../README.md
INSTALLS += target target_desktop target_docs
