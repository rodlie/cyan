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

QT=5.13.1
FFMPEG=4.2.1
PELF=0.7
QT_NAME=${QTNAME:-everywhere}
MKJOBS=${MKJOBS:-4}
KEEP_SDK=${KEEP_SDK:-1}

CWD=`pwd`
WRK=$CWD/tmp
SDK=$CWD/sdk

DEFAULT_FLAGS="-I$SDK/include -L$SDK/lib"
DEFAULT_FLAGS="-fPIC -march=core2 -mtune=corei7-avx $DEFAULT_FLAGS"
DEFAULT_CONFIGURE="--prefix=${SDK}"

export PKG_CONFIG_PATH="$SDK/lib/pkgconfig:$PKG_CONFIG_PATH"
export PATH=$SDK/bin:/usr/bin:/usr/sbin:/bin:/sbin

rm -rf "$WRK" || true
mkdir -p "$WRK" || exit 1

if [ "$KEEP_SDK" = 0 ]; then
   rm -rf "$SDK" || true
fi
if [ ! -d "$SDK" ]; then
  mkdir -p "$SDK" || exit 1
fi
if [ ! -d "$CWD/3rdparty" ]; then
  mkdir -p "$CWD/3rdparty" || exit 1
fi

if [ ! -f "$SDK/bin/patchelf" ]; then
  cd $WRK || exit 1
  tar xvf $CWD/3rdparty/patchelf-$PELF.tar.bz2 || (cd $CWD/3rdparty; curl -L -O https://nixos.org/releases/patchelf/patchelf-0.7/patchelf-$PELF.tar.bz2) && tar xvf $CWD/3rdparty/patchelf-$PELF.tar.bz2 || exit 1
  cd $WRK/patchelf-$PELF || exit 1
  ./configure --prefix=$SDK || exit 1
  make -j$MKJOBS || exit 1
  make install || exit 1
fi

if [ ! -f "$SDK/lib/libavcodec.so" ]; then
  cd $WRK || exit 1
  tar xvf $CWD/3rdparty/ffmpeg-$FFMPEG.tar.bz2 || (cd $CWD/3rdparty; curl -L -O https://ffmpeg.org/releases/ffmpeg-$FFMPEG.tar.bz2) && tar xvf $CWD/3rdparty/ffmpeg-$FFMPEG.tar.bz2 || exit 1
  cd $WRK/ffmpeg-$FFMPEG || exit 1
  CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure \
  --prefix=$SDK \
  --disable-avx \
  --disable-amf \
  --disable-xlib \
  --disable-sdl2 \
  --disable-sndio \
  --disable-libxcb \
  --disable-alsa \
  --disable-devices \
  --disable-hwaccels \
  --disable-encoders \
  --disable-doc \
  --disable-programs \
  --enable-shared \
  --disable-static \
  --disable-vaapi \
  --disable-vdpau \
  --disable-libxcb \
  --disable-libdrm \
  --enable-libopenjpeg \
  --enable-small || exit 1
  make -j${MKJOBS} || exit 1
  make install || exit 1
fi

if [ ! -f "$SDK/bin/qmake" ]; then
  cd $WRK || exit 1
  tar xvf $CWD/3rdparty/qtbase-${QT_NAME}-src-$QT.tar.xz || (cd $CWD/3rdparty; curl -L -O http://download.qt.io/official_releases/qt/5.13/$QT/submodules/qtbase-${QT_NAME}-src-$QT.tar.xz) && tar xvf $CWD/3rdparty/qtbase-${QT_NAME}-src-$QT.tar.xz || exit 1
  cd $WRK/qtbase-${QT_NAME}-src-$QT || exit 1
  CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure \
  -prefix $SDK \
  -release \
  -opensource \
  -confirm-license \
  -shared \
  -c++std c++11 \
  -no-sql-sqlite \
  -no-sql-mysql \
  -no-avx \
  -system-zlib \
  -no-mtdev \
  -no-syslog \
  -no-gif \
  -no-glib \
  -system-libpng \
  -no-libjpeg \
  -fontconfig \
  -system-freetype \
  -qt-harfbuzz \
  -no-openssl \
  -no-ico \
  -gui \
  -widgets \
  -no-cups \
  -no-icu \
  -no-gtk \
  -strip \
  -no-dbus \
  -no-eglfs \
  -no-kms \
  -no-linuxfb \
  -opengl desktop \
  -qt-xcb \
  -qt-pcre || exit 1
  make -j$MKJOBS || exit 1
  make install || exit 1
fi

rm -rf "$WRK" || true
echo "Done!"
