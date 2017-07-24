#!/bin/sh
#
# Build SDK for Linux/Mac OS X
#

ZLIB=1.2.11
TJPEG=1.5.2
JPEG=9
TIFF=4.0.8
PNG=1.6.30
LCMS=2.8
MAGICK=6.9.0-0
QT=5.6.2
OSX_MIN=10.9
JOBS=4

CWD=`pwd`
WRK=$CWD/tmp
SDK=$CWD/sdk

OS=`uname -s`
if [ "$OS" = "Darwin" ]; then
    DEFAULT_FLAGS="-mmacosx-version-min=${OSX_MIN} -I$SDK/include -L$SDK/lib"
else
    DEFAULT_FLAGS="-fPIC -march=core2 -mtune=corei7-avx"
fi

DEFAULT_CONFIGURE="--prefix=${SDK} --disable-shared --enable-static"

export PKG_CONFIG_PATH="$SDK/lib/pkgconfig:$PKG_CONFIG_PATH"
export PATH=$SDK/bin:$PATH

if [ ! -d "$HOME/pkg/bin" ]; then
    export PATH=$HOME/pkg/bin:$PATH
fi

rm -rf "$WRK" || true
mkdir -p "$WRK" || exit 1

rm -rf "$SDK" || true
mkdir -p "$SDK" || exit 1

if [ "$OS" = "Darwin" ]; then
    cd $WRK || exit 1
    rm -rf zlib-$ZLIB || true
    tar xvf $CWD/3rdparty/zlib-$ZLIB.tar.gz || exit 1
    cd zlib-$ZLIB || exit 1
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure  --prefix=$SDK || exit 1
    make || exit
    make install || exit 1
    rm -f $SDK/lib/*.dylib || true
fi

cd $WRK || exit 1
tar xvf $CWD/3rdparty/libjpeg-turbo-$TJPEG.tar.gz || (cd $CWD/3rdparty; curl -L -O http://downloads.sourceforge.net/project/libjpeg-turbo/$TJPEG/libjpeg-turbo-$JPEG.tar.gz) && tar xvf $CWD/3rdparty/libjpeg-turbo-$TJPEG.tar.gz || exit 1
cd libjpeg-turbo-$TJPEG || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE --with-jpeg8 --without-turbojpeg --with-12bit || exit 1
make -j$JOBS || exit 1
make install || exit 1

cd $WRK || exit 1
tar xvf $CWD/3rdparty/tiff-$TIFF.tar.gz || (cd $CWD/3rdparty; curl -L -O http://download.osgeo.org/libtiff/tiff-$TIFF.tar.gz) && tar xvf $CWD/3rdparty/tiff-$TIFF.tar.gz || exit 1
cd tiff-$TIFF || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE --without-x --enable-jpeg12 --with-jpeg12-include-dir=$SDK/include || exit 1
make -j$JOBS || exit
make install || exit 1

cd $WRK || exit 1
tar xvf $CWD/3rdparty/libpng-$PNG.tar.xz || (cd $CWD/3rdparty; curl -L -O https://downloads.sourceforge.net/sourceforge/libpng/libpng-$PNG.tar.xz) && tar xvf $CWD/3rdparty/libpng-$PNG.tar.xz || exit 1
cd libpng-$PNG || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
make -j$JOBS || exit
make install || exit 1

cd $WRK || exit 1
tar xvf $CWD/3rdparty/lcms2-$LCMS.tar.gz || (cd $CWD/3rdparty; curl -L -O http://downloads.sourceforge.net/sourceforge/lcms/lcms2-$LCMS.tar.gz) && tar xvf $CWD/3rdparty/lcms2-$LCMS.tar.gz || exit 1
cd lcms2-$LCMS || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
make -j$JOBS || exit
make install || exit 1

cd $WRK || exit 1
tar xvf $CWD/3rdparty/ImageMagick-$MAGICK.tar.xz || (cd $CWD/3rdparty; curl -L -O http://www.imagemagick.org/download/releases/ImageMagick-$MAGICK.tar.xz) && tar xvf $CWD/3rdparty/ImageMagick-$MAGICK.tar.xz || exit 1
cd ImageMagick-$MAGICK || exit 1
LIBS="-lz" CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE \
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
--with-lzma \
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
    cd $WRK || exit 1
    tar xvf $CWD/3rdparty/qt-everywhere-opensource-src-$QT.tar.xz || (cd $CWD/3rdparty; curl -L -O http://download.qt.io/official_releases/qt/5.6/$QT/single/qt-everywhere-opensource-src-$QT.tar.xz) && tar xvf $CWD/3rdparty/qt-everywhere-opensource-src-$QT.tar.xz || exit 1
    cd $WRK/qt-everywhere-opensource-src-$QT || exit 1
    LIBS="-llzma" CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure \
    -prefix $SDK \
    -release \
    -opensource \
    -confirm-license \
    -static \
    -c++std c++98 \
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
