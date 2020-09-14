#!/bin/sh
set -e -x

CWD=`pwd`
SDK=${SDK:-/opt/Linux}
MAGICK=${MAGICK:-"7.0.8-68"}
JOBS=${JOBS:-2}

DEFAULT_FLAGS="-fPIC -march=core2 -mtune=corei7-avx -I$SDK/include -L$SDK/lib"
DEFAULT_CONFIGURE="--prefix=${SDK} --disable-shared --enable-static"

export PKG_CONFIG_PATH="$SDK/lib/pkgconfig:$PKG_CONFIG_PATH"
export PATH=$SDK/bin:/usr/bin:/usr/sbin:/bin:/sbin

git clone https://github.com/ImageMagick/ImageMagick
cd ImageMagick
git checkout $MAGICK
cd $CWD
mkdir build-magick && cd build-magick
LIBS="-lz" CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ../ImageMagick/configure $DEFAULT_CONFIGURE \
--disable-docs \
--disable-deprecated \
--with-magick-plus-plus=yes \
--with-quantum-depth=16 \
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

echo "Done!"
