#!/bin/sh
#
# Cyan <http://cyan.fxarena.net> <https://github.com/olear/cyan>,
# Copyright (C) 2016, 2017 Ole-André Rodlie
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

CWD=`pwd`

if [ ! -f "$CWD/cyan.pro" ]; then
  echo "Can't find cyan.pro" || exit 1
fi
VERSION=`cat $CWD/cyan.pro | sed '/VERSION =/!d' | awk '{print $3}'`

MXE=/opt/mxe
MINGW=${MXE}/usr/i686-w64-mingw32.static
QT=${MINGW}/qt5
QMAKE=${QT}/bin/qmake

export PATH=${MXE}/usr/bin:$PATH
export PKG_CONFIG_PATH=${MINGW}/lib/pkgconfig

rm -f *.exe || true
rm -rf build || true

git log>CHANGES || exit 1
${QMAKE} cyan.pro CONFIG+=release CONFIG+=mxe || exit 1
make || exit 1

strip -s build/Cyan.exe
rm -rf Makefile* || true
rm -rf cyan_plugin_import.cpp  Cyan_resource.rc debug release .qmake.stash || true
mv build/Cyan.exe Cyan.exe || exit 1
zip Cyan-$VERSION-Win32.zip Cyan.exe || exit 1
rm -rf build Cyan.exe|| exit 1

MINGW=${MXE}/usr/x86_64-w64-mingw32.static
QT=${MINGW}/qt5
QMAKE=${QT}/bin/qmake

export PATH=${MXE}/usr/bin:$PATH
export PKG_CONFIG_PATH=${MINGW}/lib/pkgconfig

rm -rf build || true

${QMAKE} cyan.pro CONFIG+=release CONFIG+=mxe || exit 1
make || exit 1

strip -s build/Cyan.exe
rm -rf Makefile* || true
rm -rf cyan_plugin_import.cpp  Cyan_resource.rc debug release .qmake.stash || true
mv build/Cyan.exe Cyan.exe || exit 1
zip Cyan-$VERSION-Win64.zip Cyan.exe || exit 1
rm -rf build Cyan.exe|| exit 1
