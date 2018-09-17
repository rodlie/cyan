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

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cyan
TEMPLATE = app

SOURCES += main.cpp \
    viewer.cpp \
    view.cpp \
    layertree.cpp

HEADERS += viewer.h \
    ../plugins/interfaces.h \
    view.h \
    layertree.h \
    common.h

INCLUDEPATH += ../plugins

include(../cyan.pri)

target.path = $${PREFIX}/bin
target_desktop.path = $${PREFIX}/share/applications
target_desktop.files = ../$${TARGET}.desktop
target_docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}
target_docs.files = ../LICENSE ../README.md

INSTALLS += target target_desktop target_docs
