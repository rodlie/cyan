#!/bin/sh
#
# Build Cyan SDK on MacOSX
#

CWD=`pwd`
ZLIB=1.2.11
JPEG=9b
TIFF=4.0.8
PNG=1.6.30
LCMS=2.8
MAGICK=6.9.8-0
OSX_MIN=10.9
SDK=/Users/olear/Development/cyan/sdk

DEFAULT_FLAGS="-mmacosx-version-min=${OSX_MIN} -I$SDK/include -L$SDK/lib"
DEFAULT_CONFIGURE="--prefix=${SDK} --disable-shared --enable-static"

export PKG_CONFIG_PATH="$SDK/lib/pkgconfig:$PKG_CONFIG_PATH"
export PATH=~/pkg/bin:$PATH

rm -rf "$SDK" || true
mkdir -p "$SDK" || exit 1

cd $CWD || exit 1
rm -rf zlib-$ZLIB || true
tar xvf $CWD/zlib-$ZLIB.tar.gz || exit 1
cd zlib-$ZLIB || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure  --prefix=$SDK || exit 1
make || exit
make install || exit 1
rm -f $SDK/lib/*.dylib || true

cd $CWD || exit 1
rm -rf jpeg-$JPEG || true
tar xvf $CWD/jpegsrc.v${JPEG}.tar.gz || exit 1
cd jpeg-$JPEG || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
make -j4 || exit
make install || exit 1

cd $CWD || exit 1
rm -rf tiff-$TIFF || true
tar xvf $CWD/tiff-$TIFF.tar.gz || exit 1
cd tiff-$TIFF || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
make -j4 || exit
make install || exit 1

cd $CWD || exit 1
rm -rf libpng-$PNG || true
tar xvf $CWD/libpng-$PNG.tar.gz || exit 1
cd libpng-$PNG || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
make -j4 || exit
make install || exit 1

cd $CWD || exit 1
rm -rf lcms2-$LCMS || true
tar xvf $CWD/lcms2-$LCMS.tar.gz || exit 1
cd lcms2-$LCMS || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
make -j4 || exit
make install || exit 1

cd $CWD || exit 1
rm -rf ImageMagick-$MAGICK || true
tar xvf $CWD/ImageMagick-$MAGICK.tar.xz || exit 1
cd ImageMagick-$MAGICK || exit 1
LIBS="-lz" CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE --enable-hdri --with-quantum-depth=32 --with-zlib=yes --with-png=yes --with-jpeg=yes --with-tiff=yes --with-lcms=yes || exit 1
make -j8 || exit 1
make install || exit 1
