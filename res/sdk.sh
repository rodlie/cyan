#!/bin/sh
#
# Cyan Linux/macOS SDK
# (c) 2019-2022 Ole-André Rodlie
#
set -e -x

# common
CWD=`pwd`
OS=`uname -s`
JOBS=${JOBS:-8}
WRK=$CWD/tmp
KEEP_SDK=${KEEP_SDK:-1}
KEEP_TMP=${KEEP_TMP:-0}
SDK=$CWD/${OS}
SRC=$CWD/3rdparty

# components
XKB=0.7.1
PKGCONFIG_V=0.29.2
ZLIB=1.2.11
XZ=5.2.5
JPEG=9e
TIFF=4.3.0
PNG=1.6.37
LCMS=2.12 # 2.13(.1) breaks GRAY unit test!
MAGICK=6.9.11-62
QT=5.12.12
BZIP=1.0.8
XML=2.9.12

# on OSX we target High Sierra (10.13) with clang (MP) from macports
OSX_MIN=10.13
CLANG=9.0
CLANG_ROOT="/opt/local"

# always include SDK dir during build
DEFAULT_FLAGS="-I$SDK/include -L$SDK/lib"

# default compiler flags
if [ "$OS" = "Darwin" ]; then
    DEFAULT_FLAGS="-mmacosx-version-min=${OSX_MIN} $DEFAULT_FLAGS"
else
    DEFAULT_FLAGS="-fPIC -march=core2 -mtune=corei7-avx $DEFAULT_FLAGS"
fi

# common configure flags
DEFAULT_CONFIGURE="--prefix=${SDK} --disable-shared --enable-static"
SHARED_CONFIGURE="--prefix=${SDK} --disable-static --enable-shared"

# setup clang on OSX
if [ "$OS" = "Darwin" ]; then
  # sudo port install clang-$CLANG ld64
  export CC="$CLANG_ROOT/bin/clang-mp-$CLANG -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
  export CXX="$CLANG_ROOT/bin/clang++-mp-$CLANG -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
fi

# export common env
export PKG_CONFIG_PATH="$SDK/lib/pkgconfig"
#export PATH=$SDK/bin:/usr/bin:/usr/sbin:/bin:/sbin
export PATH="${SDK}/bin:${PATH}"

# always clean tmp
rm -rf "$WRK" || true
mkdir -p "$WRK"

# setup sdk dir
if [ "$KEEP_SDK" = 0 ]; then
  rm -rf "$SDK" || true
fi
if [ ! -d "$SDK" ]; then
  mkdir -p "$SDK/lib"
  (cd "$SDK"; ln -sf lib lib64)
fi

# create source dist folder if not exists
if [ ! -d "$SRC" ]; then
  mkdir -p "$SRC"
fi

# pkgconfig
if [ "$OS" = "Darwin" ] && [ ! -f "$SDK/bin/pkg-config" ]; then
    cd $WRK
    rm -rf pkg-config-$PKGCONFIG_V || true
    tar xvf $SRC/pkg-config-$PKGCONFIG_V.tar.gz
    cd pkg-config-$PKGCONFIG_V
    export LDFLAGS="-framework Foundation -framework Cocoa"
    ./configure --prefix=${SDK} --with-pc-path=${SDK}/lib/pkgconfig --with-internal-glib
    make
    make install
    unset LDFLAGS
fi

# zlib
if [ ! -f "$SDK/lib/pkgconfig/zlib.pc" ]; then
    cd $WRK
    rm -rf zlib-$ZLIB || true
    tar xvf $SRC/zlib-$ZLIB.tar.xz
    cd zlib-$ZLIB
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure  --prefix=$SDK
    make
    make install
    if [ "$OS" = "Darwin" ]; then
        rm -f $SDK/lib/*.dylib || true
    elif [ "$OS" = "Linux" ]; then
        rm -f $SDK/lib/libz*.so* || true
    fi
fi

# bzip
if [ ! -f "$SDK/lib/libbz2.a" ]; then
    cd $WRK
    rm -rf bzip2-$BZIP || true
    tar xvf $SRC/bzip2-$BZIP.tar.gz
    cd bzip2-$BZIP
    if [ "$OS" = "Linux" ]; then
        sed -i 's#CFLAGS=-Wall#CFLAGS=-fPIC -Wall#g' Makefile
    fi
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" make
    make PREFIX=$SDK install
fi

# xz/lzma
if [ ! -f "$SDK/lib/pkgconfig/liblzma.pc" ]; then
    cd $WRK
    rm -rf xz-${XZ} || true
    tar xvf $SRC/xz-${XZ}.tar.gz
    cd xz-$XZ
    XZ_CONFIGURE=$DEFAULT_CONFIGURE
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure $XZ_CONFIGURE
    make -j$JOBS
    make install
fi

# jpeg
if [ ! -f "$SDK/lib/pkgconfig/libjpeg.pc" ]; then
    cd $WRK
    rm -rf jpeg-${JPEG} || true
    tar xvf $SRC/jpegsrc.v${JPEG}.tar.gz
    cd jpeg-$JPEG
    JPEG_CONFIGURE=$DEFAULT_CONFIGURE
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure $JPEG_CONFIGURE
    make -j$JOBS
    make install
fi

# libpng
if [ ! -f "$SDK/lib/pkgconfig/libpng16.pc" ]; then
    cd $WRK
    rm -rf libpng-$PKG || true
    tar xvf $SRC/libpng-$PNG.tar.xz
    cd libpng-$PNG
    PNG_CONFIGURE=$DEFAULT_CONFIGURE
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $PNG_CONFIGURE
    make -j$JOBS
    make install
fi

# libtiff
if [ ! -f "$SDK/lib/pkgconfig/libtiff-4.pc" ]; then
    cd $WRK
    rm -rf tiff-$TIFF || true
    tar xvf $SRC/tiff-$TIFF.tar.gz
    cd tiff-$TIFF
    TIF_CONFIGURE=$DEFAULT_CONFIGURE
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $TIF_CONFIGURE \
    --without-x --disable-webp --enable-lzma --enable-jpeg12 --with-jpeg12-include-dir=$SDK/include
    make -j$JOBS
    make install
fi

# lcms
if [ ! -f "$SDK/lib/pkgconfig/lcms2.pc" ]; then
    cd $WRK
    rm -rf lcms2-$LCMS || true
    tar xvf $SRC/lcms2-$LCMS.tar.gz
    cd lcms2-$LCMS
    LCMS_CONFIGURE=$DEFAULT_CONFIGURE
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $LCMS_CONFIGURE
    make -j$JOBS
    make install
fi

# libxml2
if [ ! -f "$SDK/lib/pkgconfig/libxml-2.0.pc" ]; then
    cd $WRK
    rm -rf libxml2-$XML || true
    tar xvf $SRC/libxml2-$XML.tar.gz
    cd libxml2-$XML
    XML_CONFIGURE=$DEFAULT_CONFIGURE
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $XML_CONFIGURE \
    --without-python
    make -j$JOBS
    make install
fi

# imagemagick
if [ ! -f "$SDK/lib/pkgconfig/Magick++.pc" ]; then
    cd $WRK || exit 1
    rm -rf ImageMagick* || true
    tar xvf $SRC/ImageMagick-$MAGICK.tar.xz || exit 1
    cd ImageMagick-$MAGICK || exit 1
    if [ ! -f "${CWD}/imagemagick-3-gimp_2_10.patch" ]; then
        curl -L https://github.com/nettstudio/mxe/raw/Cyan-1.2.3/src/imagemagick-3-gimp_2_10.patch -o "${CWD}/imagemagick-3-gimp_2_10.patch"
    fi
    patch -p1 < "${CWD}/imagemagick-3-gimp_2_10.patch"
    MAGICK_LDFLAGS=""
    if [ "$OS" = "Darwin" ]; then
        MAGICK_LDFLAGS="-mmacosx-version-min=$OSX_MIN"
    fi
    sed -i 's#-ltiff#-ltiff -llzma -ljpeg -lz#g' configure
    LDFLAGS="$MAGICK_LDFLAGS" LIBS="-lz -llzma -lbz2" CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure --prefix=$SDK \
    --enable-static --disable-shared \
    --with-x=no --disable-docs --disable-modules --without-modules --with-xml --without-gslib \
    --with-zlib --with-lzma --without-jasper --enable-hdri --with-quantum-depth=16 \
    --enable-largefile --without-pango --without-webp --without-fftw --without-lqr \
    --without-freetype --without-openexr --without-fontconfig \
    --enable-zero-configuration \
  || exit 1
  make -j$JOBS || exit 1
  make install || exit 1
fi

if [ "$OS" = "Darwin" ]; then
    for i in $SDK/lib/pkgconfig/*.pc ; do
        sed -i '' "s/-lgomp//g;s/-fopenmp /-fopenmp=libomp /g" $i || exit 1
    done
fi

# libxkbcommon
if [ "$OS" = "Linux" ] && [ ! -f "$SDK/lib/pkgconfig/xkbcommon.pc" ]; then
    tar xvf $SRC/libxkbcommon-$XKB.tar.xz || exit 1
    cd libxkbcommon-$XKB || exit 1
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" CPPFLAGS="$DEFAULT_FLAGS" ./configure $DEFAULT_CONFIGURE || exit 1
    make -j$JOBS || exit
    make install || exit 1
fi

# qtbase
QT_CONFIGURE="-optimize-size -opensource -release -confirm-license -gui -widgets -strip -static -nomake examples -nomake tests -no-glib -no-dbus  -no-openssl -no-ico -no-icu -no-sql-sqlite -no-sse4.2 -no-avx -no-avx2 -no-avx512 -no-cups -no-gtk -no-libjpeg -no-fontconfig -no-mtdev -no-syslog -no-gif -qt-pcre -qt-freetype -qt-harfbuzz -opengl desktop"
if [ ! -f "$SDK/bin/qmake" ] && [ "$OS" = "Darwin" ]; then
    ## we use the legacy LTS release (5.9) on OSX
    #cd $WRK || exit 1
    #tar xvf $SRC/qtbase-opensource-src-$QT.tar.xz || exit 1
    #cd $WRK/qtbase-opensource-src-$QT || exit 1
    ## set correct compiler
    #sed -i '' "s#QMAKE_CC                = clang#QMAKE_CC                = $CLANG_ROOT/bin/clang-mp-${CLANG}#" mkspecs/common/clang.conf || exit 1
    #sed -i '' "s#QMAKE_CXX               = clang++#QMAKE_CXX               = $CLANG_ROOT/bin/clang++-mp-${CLANG}#" mkspecs/common/clang.conf || exit 1
    #./configure -prefix $SDK -release -opensource -c++std c++11 -framework \
    #-no-sql-sqlite -no-avx -no-avx2 -force-pkg-config -system-zlib -no-gif -no-libjpeg \
    #-system-libpng -no-fontconfig -qt-freetype -qt-harfbuzz -no-openssl -qt-pcre -nomake examples \
    #-gui -widgets -no-glib -no-cups -no-dbus -opengl desktop -no-securetransport -confirm-license -I $SDK/include -L $SDK/lib || exit 1
    #make -j$JOBS || exit 1
    #make install || exit 1
    echo "update me!"
elif [ ! -f "$SDK/bin/qmake" ] && [ "$OS" = "Linux" ]; then
    cd $WRK 
    rm -rf qtbase-everywhere-src-$QT
    tar xvf $SRC/qtbase-everywhere-src-$QT.tar.xz
    cd qtbase-everywhere-src-$QT || exit 1
    export LD_LIBRARY_PATH="$SDK/lib:$LD_LIBRARY_PATH"
    CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure -prefix $SDK $QT_CONFIGURE \
    -no-eglfs -no-kms -no-linuxfb -xkbcommon -qt-xcb -system-libpng -system-zlib \
    -I $SDK/include -L $SDK/lib || exit 1
    make -j$JOBS || exit 1
    make install || exit 1
fi

# macdeployqt
if [ ! -f "$SDK/bin/macdeployqt" ] && [ "$OS" = "Darwin" ]; then
  cd $WRK || exit 1
  rm -rf qttools* || true
  tar xvf $SRC/qttools-opensource-src-$QT.tar.xz || exit 1
  cd qttools-opensource-src-$QT || exit 1
  $SDK/bin/qmake || exit 1
  make || exit 1
  make install || exit 1
fi

if [ "${KEEP_TMP}" = 0 ]; then
    rm -rf "$WRK" || true
fi

echo "==> Done with Cyan SDK!"
