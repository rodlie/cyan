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

PKGCONFIG_V=0.29.2
ZLIB=1.2.11
TJPEG=1.5.2
JPEG=9b
TIFF=4.0.8
PNG=1.6.30
LCMS=2.8
MAGICK=6.9.0-0 #7.0.6-4
QT=5.6.2
#SSL=1.0.2l
OSX_MIN=10.7
JOBS=2
JTYPE="" #"turbo" # has issues on macosx, so use vanilla jpeg on all platforms.

CWD=`pwd`
WRK=$CWD/tmp
SDK=$CWD/sdk

OS=`uname -s`
DEFAULT_FLAGS="-I$SDK/include -L$SDK/lib"
if [ "$OS" = "Darwin" ]; then
    DEFAULT_FLAGS="-mmacosx-version-min=${OSX_MIN} $DEFAULT_FLAGS"
else
    DEFAULT_FLAGS="-fPIC -march=core2 -mtune=corei7-avx $DEFAULT_FLAGS"
fi

DEFAULT_CONFIGURE="--prefix=${SDK} --disable-shared --enable-static"

if [ "$OS" = "Darwin" ]; then
  export CC="clang -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
  export CXX="clang++ -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
fi

export PKG_CONFIG_PATH="$SDK/lib/pkgconfig:$PKG_CONFIG_PATH"
export PATH=$SDK/bin:/usr/bin:/usr/sbin:/bin:/sbin

rm -rf "$WRK" || true
mkdir -p "$WRK" || exit 1

rm -rf "$SDK" || true
mkdir -p "$SDK" || exit 1

if [ ! -d "$CWD/3rdparty" ]; then
  mkdir -p "$CWD/3rdparty" || exit 1
fi

if [ "$OS" = "Darwin" ]; then
    cd $WRK || exit 1
    rm -rf pkg-config-$PKGCONFIG_V || true
    tar xvf $CWD/3rdparty/pkg-config-$PKGCONFIG_V.tar.gz || (cd $CWD/3rdparty; curl -k -L -O http://pkg-config.freedesktop.org/releases/pkg-config-${PKGCONFIG_V}.tar.gz) && tar xvf $CWD/3rdparty/pkg-config-$PKGCONFIG_V.tar.gz || exit 1
    cd pkg-config-$PKGCONFIG_V || exit 1
    export LDFLAGS="-framework Foundation -framework Cocoa"
    ./configure --prefix=${SDK} --with-pc-path=${SDK}/lib/pkgconfig --with-internal-glib || exit 1
    make || exit 1
    make install || exit 1
    unset LDFLAGS
fi

if [ "$OS" = "Darwin" ]; then
    cd $WRK || exit 1
    rm -rf zlib-$ZLIB || true
    tar xvf $CWD/3rdparty/zlib-$ZLIB.tar.gz || (cd $CWD/3rdparty; curl -k -L -O http://zlib.net/zlib-${ZLIB}.tar.gz) && tar xvf $CWD/3rdparty/zlib-$ZLIB.tar.gz || exit 1
    cd zlib-$ZLIB || exit 1
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure  --prefix=$SDK || exit 1
    make || exit
    make install || exit 1
    rm -f $SDK/lib/*.dylib || true
fi

if [ "$JTYPE" = "turbo" ]; then
    cd $WRK || exit 1
    tar xvf $CWD/3rdparty/libjpeg-turbo-$TJPEG.tar.gz || (cd $CWD/3rdparty; curl -k -L -O http://downloads.sourceforge.net/project/libjpeg-turbo/$TJPEG/libjpeg-turbo-$TJPEG.tar.gz) && tar xvf $CWD/3rdparty/libjpeg-turbo-$TJPEG.tar.gz || exit 1
    cd libjpeg-turbo-$TJPEG || exit 1
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE --with-jpeg8 --without-turbojpeg --with-12bit || exit 1
    make -j$JOBS || exit 1
    make install || exit 1
else
    cd $WRK || exit 1
    tar xvf $CWD/3rdparty/jpegsrc.v${JPEG}.tar.gz || (cd $CWD/3rdparty; curl -k -L -O http://www.ijg.org/files/jpegsrc.v${JPEG}.tar.gz) && tar xvf $CWD/3rdparty/jpegsrc.v${JPEG}.tar.gz || exit 1
    cd jpeg-$JPEG || exit 1
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
    make -j$JOBS || exit
    make install || exit 1
fi

cd $WRK || exit 1
tar xvf $CWD/3rdparty/tiff-$TIFF.tar.gz || (cd $CWD/3rdparty; curl -k -L -O http://download.osgeo.org/libtiff/tiff-$TIFF.tar.gz) && tar xvf $CWD/3rdparty/tiff-$TIFF.tar.gz || exit 1
cd tiff-$TIFF || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE --without-x --enable-jpeg12 --with-jpeg12-include-dir=$SDK/include || exit 1
make -j$JOBS || exit
make install || exit 1

cd $WRK || exit 1
tar xvf $CWD/3rdparty/libpng-$PNG.tar.gz || (cd $CWD/3rdparty; curl -k -L -O http://downloads.sourceforge.net/sourceforge/libpng/libpng-$PNG.tar.gz) && tar xvf $CWD/3rdparty/libpng-$PNG.tar.gz || exit 1
cd libpng-$PNG || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
make -j$JOBS || exit
make install || exit 1

cd $WRK || exit 1
tar xvf $CWD/3rdparty/lcms2-$LCMS.tar.gz || (cd $CWD/3rdparty; curl -k -L -O http://downloads.sourceforge.net/sourceforge/lcms/lcms2-$LCMS.tar.gz) && tar xvf $CWD/3rdparty/lcms2-$LCMS.tar.gz || exit 1
cd lcms2-$LCMS || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
make -j$JOBS || exit
make install || exit 1

cd $WRK || exit 1
tar xvf $CWD/3rdparty/ImageMagick-$MAGICK.tar.gz || (cd $CWD/3rdparty; curl -k -L -O http://www.imagemagick.org/download/releases/ImageMagick-$MAGICK.tar.gz) && tar xvf $CWD/3rdparty/ImageMagick-$MAGICK.tar.gz || exit 1
cd ImageMagick-$MAGICK || exit 1
LDFLAGS="-mmacosx-version-min=$OSX_MIN" LIBS="-lz" CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE \
--disable-docs \
--disable-deprecated \
--with-magick-plus-plus=yes \
--with-quantum-depth=32 \
--without-dps \
--without-djvu \
--without-fftw \
--without-fpx \
--without-gslib \
--without-gvc \
--without-jbig \
--with-jpeg \
--with-lcms2 \
--without-openjp2 \
--without-lqr \
--without-lzma \
--without-openexr \
--without-pango \
--with-png \
--without-rsvg \
--with-tiff \
--without-webp \
--without-xml \
--with-zlib \
--with-bzlib \
--enable-hdri \
--without-freetype \
--without-fontconfig \
--without-x \
--without-modules || exit 1
make -j$JOBS || exit 1
make install || exit 1

if [ "$OS" = "Linux" ]; then
    #cd $WRK || exit 1
    #tar xvf $CWD/3rdparty/openssl-$SSL.tar.gz || (cd $CWD/3rdparty; curl -L -O https://www.openssl.org/source/openssl-$SSL.tar.gz) && tar xvf $CWD/3rdparty/openssl-$SSL.tar.gz || exit 1
    #cd $WRK/openssl-$SSL || exit 1
    #CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./Configure --prefix=$SDK linux-x86_64 || exit 1
    #make || exit 1
    #make install || exit 1

    cd $WRK || exit 1
    tar xvf $CWD/3rdparty/qt-everywhere-opensource-src-$QT.tar.xz || (cd $CWD/3rdparty; curl -L -O http://download.qt.io/official_releases/qt/5.6/$QT/single/qt-everywhere-opensource-src-$QT.tar.xz) && tar xvf $CWD/3rdparty/qt-everywhere-opensource-src-$QT.tar.xz || exit 1
    cd $WRK/qt-everywhere-opensource-src-$QT || exit 1
    patch -p0< $CWD/res/qt5.diff || exit 1
    OPENSSL_LIBS="-L$SDK/lib" CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure \
    -prefix $SDK \
    -release \
    -opensource \
    -confirm-license \
    -static \
    -c++std c++11 \
    -largefile \
    -no-sql-sqlite \
    -no-sql-mysql \
    -no-avx \
    -system-zlib \
    -no-mtdev \
    -no-syslog \
    -no-gif \
    -system-libpng \
    -system-libjpeg \
    -qt-harfbuzz \
    -no-openssl \
    -no-libproxy \
    -no-pulseaudio \
    -no-alsa \
    -no-gtkstyle \
    -no-compile-examples \
    -gui \
    -widgets \
    -no-cups \
    -no-icu \
    -fontconfig \
    -strip \
    -no-dbus \
    -no-eglfs \
    -no-kms \
    -no-linuxfb \
    -opengl desktop \
    -no-gstreamer \
    -system-xcb \
    -qt-pcre \
    -skip qtcanvas3d \
    -skip qtconnectivity \
    -skip qtdeclarative \
    -skip qtdoc \
    -skip qtgraphicaleffects \
    -skip qtlocation \
    -skip qtmultimedia \
    -skip qtscript \
    -skip qtsensors \
    -skip qtserialbus \
    -skip qtserialport \
    -skip qtsvg \
    -skip qtwebchannel \
    -skip qtwebengine \
    -skip qtwebview \
    -skip qtwebsockets \
    -skip qtxmlpatterns || exit 1
    make -j$JOBS || exit 1
    make install || exit 1
fi

rm -rf "$WRK" || true
echo "Done!"
