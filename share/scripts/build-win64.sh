#!/bin/sh
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

CWD=`pwd`
BUILD="$CWD/build-win64"
MXE=/opt/Cyan-mxe
TOOLCHAIN=x86_64-w64-mingw32.shared
CMAKE=${TOOLCHAIN}-cmake
STRIP=${TOOLCHAIN}-strip
QT="$MXE/usr/$TOOLCHAIN/qt5"
BIN="$MXE/usr/$TOOLCHAIN/bin"
MAGICK=Magick++-7.Q16HDRI
if [ -d "$CWD/mxe" ]; then
    MXE="$CWD/mxe"
fi
INNO="$MXE/inno6/ISCC.exe"

rm -rf "$BUILD" || true
mkdir -p "$BUILD" || exit 1
cd "$BUILD" || exit 1

export PATH="$MXE/usr/bin:$PATH"
export PKG_CONFIG_PATH="$MXE/usr/$TOOLCHAIN/lib/pkgconfig"
$CMAKE \
    -DCMAKE_BUILD_TYPE=Release \
    -DMINGW_ROOT="$MXE/usr/$TOOLCHAIN" \
    -DCMAKE_INSTALL_PREFIX=/ \
    -DMAGICK_PKG_CONFIG=$MAGICK "$CWD" || exit 1
make -j2 || exit 1
$STRIP -s *.exe *.dll */*.dll
wine $INNO Cyan.iss
