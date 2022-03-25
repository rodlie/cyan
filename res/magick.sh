#!/bin/sh
set -e -x

CWD=`pwd`
OS=`uname -s`
SDK=${SDK:-/opt/$OS}
MAGICK=${MAGICK:-"6.9.11-62"}
OSX_MIN=10.7
CLANG_VER=mp-6.0
CLANG_PATH=/opt/local/bin
JOBS=${JOBS:-2}
#TIFF=4.3.0
#LCMS=2.13.1

DEFAULT_FLAGS="-I$SDK/include -L$SDK/lib"
if [ "$OS" = "Darwin" ]; then
    DEFAULT_FLAGS="-mmacosx-version-min=${OSX_MIN} $DEFAULT_FLAGS"
else
    DEFAULT_FLAGS="-fPIC -march=core2 -mtune=corei7-avx $DEFAULT_FLAGS"
fi

DEFAULT_CONFIGURE="--prefix=${SDK} --disable-shared --enable-static --disable-docs --disable-deprecated --with-magick-plus-plus=yes --with-quantum-depth=16 --without-dps --without-djvu --without-fftw --without-fpx --without-heic --without-gslib --without-gvc --without-jbig --with-jpeg --with-lcms --with-lzma --without-openjp2 --without-lqr --with-lzma --without-openexr --without-pango --with-png --without-rsvg --with-tiff --without-webp --with-xml --with-zlib --with-bzlib --enable-hdri --without-freetype --without-fontconfig --without-x --without-modules"

if [ "$OS" = "Darwin" ]; then
  export CC="${CLANG_PATH}/clang-${CLANG_VER} -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
  export CXX="${CLANG_PATH}/clang++-${CLANG_VER} -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
fi

export PKG_CONFIG_PATH="$SDK/lib/pkgconfig:$PKG_CONFIG_PATH"
#export PATH=$SDK/bin:/usr/bin:/usr/sbin:/bin:/sbin

#cd $CWD
#git clone https://gitlab.com/libtiff/libtiff
#cd libtiff
#git checkout v${TIFF}
#./autogen.sh

#cd $CWD
#mkdir build-tiff && cd build-tiff
#CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ../libtiff/configure --prefix=${SDK} --enable-static --disable-shared
#make -j$JOBS
#make install

#cd $CWD
#git clone https://github.com/mm2/Little-CMS lcms2
#cd lcms2
#git checkout lcms${LCMS}
##./autogen.sh

#cd $CWD
#mkdir build-lcms2 && cd build-lcms2
#CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ../lcms2/configure --prefix=${SDK} --enable-static --disable-shared
#make -j$JOBS
#make install

cd $CWD
git clone https://github.com/ImageMagick/ImageMagick6
cd ImageMagick6
git checkout $MAGICK

cd $CWD
mkdir build-magick && cd build-magick
#EXTRA_LDFLAGS=""
#if [ "$OS" = "Darwin" ]; then
#    EXTRA_LDFLAGS="-mmacosx-version-min=$OSX_MIN"
#fi
#export PKG_CONFIG_PATH="$SDK/lib/pkgconfig:$PKG_CONFIG_PATH"
#LDFLAGS="$EXTRA_LDFLAGS" LIBS="-lz -llzma" CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ../ImageMagick6/configure $DEFAULT_CONFIGURE
sed -i'.original' -e 's/-ltiff/-ltiff -llzma -ljbig -ljpeg -lz/g' ../ImageMagick6/configure
CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ../ImageMagick6/configure $DEFAULT_CONFIGURE
make -j$JOBS
make install

echo "Done!"
