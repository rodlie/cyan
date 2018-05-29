#
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lumina-pixel
TEMPLATE = app

SOURCES += main.cpp viewer.cpp view.cpp \
    layertree.cpp
HEADERS += viewer.h ../interfaces.h view.h \
    layertree.h
INCLUDEPATH += ../

CONFIG += link_pkgconfig
PKGCONFIG += Magick++

include(../../lumina-extra.pri)

target.path = $${PREFIX}/bin
target_desktop.path = $${PREFIX}/share/applications
target_desktop.files = $${TARGET}.desktop
target_docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}
target_docs.files = ../../LICENSE ../../README.md
INSTALLS += target target_desktop target_docs
