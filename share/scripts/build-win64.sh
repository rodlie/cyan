#!/bin/sh

CWD=`pwd`
BUILD="$CWD/build-win64"
MXE=/opt/Cyan-mxe
TOOLCHAIN=x86_64-w64-mingw32.shared
CMAKE=${TOOLCHAIN}-cmake
STRIP=${TOOLCHAIN}-strip
QT="$MXE/usr/$TOOLCHAIN/qt5"
BIN="$MXE/usr/$TOOLCHAIN/bin"
MAGICK=Magick++-7.Q16HDRI
INNO="$MXE/inno6/ISCC.exe"

rm -rf "$BUILD" || true
mkdir -p "$BUILD" || exit 1
cd "$BUILD" || exit 1

export PATH="$MXE/usr/bin:$PATH"
export PKG_CONFIG_PATH="$MXE/usr/$TOOLCHAIN/lib/pkgconfig"
$CMAKE -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/ -DMAGICK_PKG_CONFIG=$MAGICK "$CWD" || exit 1
make -j2 || exit 1
$STRIP -s *.exe *.dll */*.dll
wine $INNO Cyan.iss

