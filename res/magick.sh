#!/bin/sh
set -e -x

CWD=`pwd`
OS=`uname -s`
SDK=${SDK:-/opt/$OS}
MAGICK=${MAGICK:-"7.0.8-68"}
OSX_MIN=10.7
CLANG_VER=mp-6.0
CLANG_PATH=/opt/local/bin
JOBS=${JOBS:-2}

DEFAULT_FLAGS="-I$SDK/include -L$SDK/lib"
if [ "$OS" = "Darwin" ]; then
    DEFAULT_FLAGS="-mmacosx-version-min=${OSX_MIN} $DEFAULT_FLAGS"
else
    DEFAULT_FLAGS="-fPIC -march=core2 -mtune=corei7-avx $DEFAULT_FLAGS"
fi

DEFAULT_CONFIGURE="--prefix=${SDK} --disable-shared --enable-static --disable-docs --disable-deprecated --with-magick-plus-plus=yes --with-quantum-depth=16 --without-dps --without-djvu --without-fftw --without-fpx --without-gslib --without-gvc --without-jbig --with-jpeg --with-lcms2 --without-openjp2 --without-lqr --with-lzma --without-openexr --without-pango --with-png --without-rsvg --with-tiff --without-webp --without-xml --with-zlib --with-bzlib --enable-hdri --without-freetype --without-fontconfig --without-x --without-modules"

if [ "$OS" = "Darwin" ]; then
  export CC="${CLANG_PATH}/clang-${CLANG_VER} -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
  export CXX="${CLANG_PATH}/clang++-${CLANG_VER} -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
fi

export PKG_CONFIG_PATH="$SDK/lib/pkgconfig:$PKG_CONFIG_PATH"
export PATH=$SDK/bin:/usr/bin:/usr/sbin:/bin:/sbin

git clone https://github.com/ImageMagick/ImageMagick
cd ImageMagick
git checkout $MAGICK
cd $CWD
mkdir build-magick && cd build-magick
LIBS="-lz -llzma" CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ../ImageMagick/configure $DEFAULT_CONFIGURE
sed -i 's/-ltiff/-ltiff -llzma/g' ../ImageMagick/configure
LIBS="-lz -llzma" CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ../ImageMagick/configure $DEFAULT_CONFIGURE
make -j$JOBS || exit 1
make install || exit 1

echo "Done!"
