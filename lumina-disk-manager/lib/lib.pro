#
# lumina-extra
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#

QT += dbus
QT -= gui

TARGET = Disks
VERSION = 1.0.0

TEMPLATE = lib
SOURCES += disks.cpp device.cpp
HEADERS += disks.h udisks2.h device.h

include(../../lumina-extra.pri)

target.path = $${PREFIX}/lib$${LIBSUFFIX}
target_docs.path = $${DOCDIR}/libdisks-$${VERSION}
target_docs.files = ../../LICENSE ../../README.md
target_inc.path = $${PREFIX}/include/libdisks
target_inc.files = disks.h device.h
INSTALLS += target target_docs target_inc
