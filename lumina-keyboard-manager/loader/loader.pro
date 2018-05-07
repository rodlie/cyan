#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

QT += core
QT -= gui
CONFIG += console
CONFIG -= app_bundle

TARGET = lumina-keyboard-loader
VERSION = 1.0.0

SOURCES += main.cpp
HEADERS += ../common.h
INCLUDEPATH += ..

include(../../lumina-extra.pri)

target.path = $${PREFIX}/bin
target_desktop.path = $${XDGDIR}/autostart
target_desktop.files = $${TARGET}.desktop
target_docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}
target_docs.files = ../../LICENSE ../../README.md
INSTALLS += target target_desktop target_docs
