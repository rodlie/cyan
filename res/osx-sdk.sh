#!/bin/sh
#
# Build Cyan SDK on MacOSX
#

CWD=`pwd`
WRK=$CMD/tmp
ZLIB=1.2.11
JPEG=9b
TIFF=4.0.8
PNG=1.6.30
LCMS=2.8
MAGICK=6.9.0-0
OSX_MIN=10.9
SDK=$CWD/sdk

DEFAULT_FLAGS="-mmacosx-version-min=${OSX_MIN} -I$SDK/include -L$SDK/lib"
DEFAULT_CONFIGURE="--prefix=${SDK} --disable-shared --enable-static"

export PKG_CONFIG_PATH="$SDK/lib/pkgconfig:$PKG_CONFIG_PATH"

if [ ! -d "$HOME/pkg/bin"]; then
  export PATH=$HOME/pkg/bin:$PATH
fi

rm -rf "$WRK" || true
mkdir -p "$WRK" || exit 1

rm -rf "$SDK" || true
mkdir -p "$SDK" || exit 1

cd $WRK || exit 1
rm -rf zlib-$ZLIB || true
tar xvf $CWD/3rdparty/zlib-$ZLIB.tar.gz || exit 1
cd zlib-$ZLIB || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure  --prefix=$SDK || exit 1
make || exit
make install || exit 1
rm -f $SDK/lib/*.dylib || true

cd $WRK || exit 1
rm -rf jpeg-$JPEG || true
tar xvf $CWD/3rdparty/jpegsrc.v${JPEG}.tar.gz || exit 1
cd jpeg-$JPEG || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
make -j4 || exit
make install || exit 1

cd $WRK || exit 1
rm -rf tiff-$TIFF || true
tar xvf $CWD/3rdparty/tiff-$TIFF.tar.gz || exit 1
cd tiff-$TIFF || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
make -j4 || exit
make install || exit 1

cd $WRK || exit 1
rm -rf libpng-$PNG || true
tar xvf $CWD/3rdparty/libpng-$PNG.tar.gz || exit 1
cd libpng-$PNG || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
make -j4 || exit
make install || exit 1

cd $WRK || exit 1
rm -rf lcms2-$LCMS || true
tar xvf $CWD/3rdparty/lcms2-$LCMS.tar.gz || exit 1
cd lcms2-$LCMS || exit 1
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
make -j4 || exit
make install || exit 1

cd $WRK || exit 1
rm -rf ImageMagick-$MAGICK || true
tar xvf $CWD/3rdparty/ImageMagick-$MAGICK.tar.gz || exit 1
cd ImageMagick-$MAGICK || exit 1
LIBS="-lz" CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE --enable-hdri --with-quantum-depth=32 --with-zlib=yes --with-png=yes --with-jpeg=yes --with-tiff=yes --with-lcms=yes || exit 1
make -j8 || exit 1
make install || exit 1

rm -rf "$WRK" || true
echo "Done!"
