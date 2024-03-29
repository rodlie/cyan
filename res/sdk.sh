#!/bin/sh
#
# Cyan Linux/macOS SDK
# (c) 2019-2022 Ole-André Rodlie
#
set -e -x

# common
CWD=`pwd`
OS=`uname -s`
JOBS=${JOBS:-2}
WRK=$CWD/tmp
KEEP_SDK=${KEEP_SDK:-1}
KEEP_TMP=${KEEP_TMP:-0}
SDK=$CWD/${OS}
SRC=$CWD/3rdparty
LEGACY_OSX=${LEGACY_OSX:-0}
HDRI=${HDRI:-1}
MP=${MP:-1}
CLANG_MP=${CLANG_MP:-1}
NO_QT=${NO_QT:-0}

# components
XKB=0.7.1
PKGCONFIG_V=0.29.2
ZLIB=1.2.11
XZ=5.2.5
JPEG=9e
TIFF=4.3.0
PNG=1.6.37
LCMS=2.12 # 2.13(.1) breaks GRAY unit test!
MAGICK=${MAGICK:-"6.9.11-62"} # 6.9.10-17, 6.9.10-97
MAGICK7=${MAGICK7:-"7.1.0-57"}
IM7=${IM7:-0}
QT=${QT:-"5.15.8"} # 5.9.9, 5.12.12, 5.15.8
QT_TAR="everywhere"
BZIP=1.0.8
XML=2.9.12

if [ "${IM7}" = 1 ]; then
    MAGICK=${MAGICK7}
fi

# on macOS we target High Sierra (10.13) with clang (MP) from macports
OSX_MIN=${OSX_MIN:-10.13}
CLANG=${CLANG:-12} # 9.0
CLANG_ROOT="/opt/local"

# on OSX we target Yosemite (10.10) with Qt 5.9 and clang (MP) from macports
if [ "${LEGACY_OSX}" = 1 ]; then
    QT=5.9.9
    OSX_MIN=10.10
    QT_TAR="opensource"
fi

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
  if [ "${CLANG_MP}" = 1 ]; then
    export CC="$CLANG_ROOT/bin/clang-mp-$CLANG -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
    export CXX="$CLANG_ROOT/bin/clang++-mp-$CLANG -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
  else
    MP=0
    export CC="/usr/bin/clang -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
    export CXX="/usr/bin/clang++ -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
  fi
  export PATH=$SDK/bin:/usr/bin:/usr/sbin:/bin:/sbin
else
    export PATH="${SDK}/bin:${PATH}"
fi

# pkgconfig .pc path
export PKG_CONFIG_PATH="$SDK/lib/pkgconfig"

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
    if [ "${OS}" = "Darwin" ]; then
        mv VERSION VERSION.txt
        patch -p0 < $CWD/res/clang-tiff-version.diff
    fi
    TIF_CONFIGURE=$DEFAULT_CONFIGURE
    #TIF_CONFIGURE=$SHARED_CONFIGURE
    #if [ "${OS}" = "Linux" ]; then
    #    TIF_CONFIGURE=$DEFAULT_CONFIGURE
    #fi
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
REBUILD_MAGICK=${REBUILD_MAGICK:-0}
if [ "${REBUILD_MAGICK}" = 1 ]; then
    rm -rf \
    ${SDK}/include/ImageMagick* \
    ${SDK}/lib/ImageMagick* \
    ${SDK}/lib/libMagick* \
    ${SDK}/lib/pkgconfig/ImageMagick* \
    ${SDK}/lib/pkgconfig/Magick* \
    ${SDK}/bin/Magick*
fi
if [ ! -f "$SDK/lib/pkgconfig/Magick++.pc" ]; then
    cd $WRK || exit 1
    rm -rf ImageMagick* || true
    tar xvf $SRC/ImageMagick-$MAGICK.tar.xz || exit 1
    cd ImageMagick-$MAGICK || exit 1
    if [ "${IM7}" != 1 ]; then
        if [ ! -f "${SRC}/imagemagick-3-gimp_2_10.patch" ]; then
            curl -L https://github.com/nettstudio/mxe/raw/Cyan-1.2.3/src/imagemagick-3-gimp_2_10.patch -o "${SRC}/imagemagick-3-gimp_2_10.patch"
        fi
        patch -p1 < "${SRC}/imagemagick-3-gimp_2_10.patch"
    fi
    if [ "${MP}" = 1 ]; then
        MAGICK_MP="--enable-openmp"
    else
        MAGICK_MP="--disable-openmp"
    fi
    if [ "${HDRI}" = 1 ]; then
        MAGICK_HDRI="--enable-hdri"
    else
        MAGICK_HDRI="--disable-hdri"
    fi
    MAGICK_LDFLAGS=""
    if [ "$OS" = "Darwin" ]; then
        MAGICK_LDFLAGS="-mmacosx-version-min=$OSX_MIN"
    fi
    sed -i '' 's#-ltiff#-ltiff -llzma -ljpeg -lz#g' configure
    LDFLAGS="$MAGICK_LDFLAGS" LIBS="-lz -llzma -lbz2" CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure --prefix=$SDK \
    --enable-static --disable-shared \
    --with-x=no --disable-docs --with-xml --without-gslib \
    --with-zlib --with-lzma --without-jasper --with-quantum-depth=16 \
    --enable-largefile --without-pango --without-webp --without-fftw --without-lqr \
    --without-freetype --without-openexr --without-fontconfig \
    --enable-zero-configuration ${MAGICK_MP} ${MAGICK_HDRI} \
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

if [ "${NO_QT}" = 1 ]; then
    echo "==> Done with Cyan SDK!"
    exit 0
fi

# qtbase
QT_CONFIGURE="-static -force-pkg-config -optimize-size -opensource -release -confirm-license -gui -widgets -strip -nomake examples -nomake tests -no-glib -no-dbus  -no-openssl -no-ico -no-icu -no-sql-sqlite -no-sse4.2 -no-avx -no-avx2 -no-avx512 -no-cups -no-gtk -no-libjpeg -no-fontconfig -no-mtdev -no-syslog -no-gif -qt-pcre -qt-freetype -qt-harfbuzz -opengl desktop -system-libpng -system-zlib"
if [ ! -f "$SDK/bin/qmake" ]; then
    cd $WRK 
    rm -rf qtbase-${QT_TAR}-src-$QT
    tar xvf $SRC/qtbase-${QT_TAR}-src-$QT.tar.xz
    cd qtbase-${QT_TAR}-src-$QT || exit 1
    export LD_LIBRARY_PATH="$SDK/lib:$LD_LIBRARY_PATH"
    if [ "$OS" = "Darwin" ]; then
        echo "QMAKE_CC = $CLANG_ROOT/bin/clang-mp-${CLANG} -mmacosx-version-min=$OSX_MIN" >> mkspecs/common/clang.conf
        echo "QMAKE_CXX = $CLANG_ROOT/bin/clang++-mp-${CLANG} -mmacosx-version-min=$OSX_MIN" >> mkspecs/common/clang.conf
        QT_CPP="-c++std c++14"
        if [ "${QT}" = "5.12.12" ]; then
            QT_CONFIGURE="${QT_CONFIGURE} -no-feature-testlib"
        fi
        CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure -prefix $SDK $QT_CONFIGURE $QT_CPP -no-securetransport -I $SDK/include -L $SDK/lib
    elif [ "$OS" = "Linux" ]; then
        CFLAGS="$DEFAULT_FLAGS" CXXFLAGS="$DEFAULT_FLAGS" ./configure -prefix $SDK $QT_CONFIGURE -no-eglfs -no-kms -no-linuxfb -xkbcommon -qt-xcb -I $SDK/include -L $SDK/lib
    fi
    make -j$JOBS
    make install
fi

# macdeployqt
if [ ! -f "$SDK/bin/macdeployqt" ] && [ "$OS" = "Darwin" ]; then
  cd $WRK || exit 1
  rm -rf qttools* || true
  tar xvf $SRC/qttools-${QT_TAR}-src-$QT.tar.xz || exit 1
  cd qttools-${QT_TAR}-src-$QT || exit 1
  $SDK/bin/qmake || exit 1
  make || exit 1
  make install || exit 1
fi

if [ "${KEEP_TMP}" = 0 ]; then
    rm -rf "$WRK" || true
fi

echo "==> Done with Cyan SDK!"
