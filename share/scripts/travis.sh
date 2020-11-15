#!/bin/bash
#
# Build Cyan on Ubuntu (and for Windows through MXE)
#

set -e -x

CWD=`pwd`
MKJOBS=${MKJOBS:-4}
PREFIX=${PREFIX:-"/usr"}
APT=${APT:-1}
PKG_DIR="${CWD}/build-pkg"
CLEAN=${CLEAN:-1}
DATE=`date "+%Y%m%d"`
DISTRO=${DISTRO:-`cat /etc/os-release | sed '/UBUNTU_CODENAME/!d;s/UBUNTU_CODENAME=//'`}
WIN32=${WIN32:-0}
WIN64=${WIN64:-0}
LINUX=${LINUX:-1}
HEIC="no"
QDEPTH=${QDEPTH:-16}
#HDRI=${HDRI:-"HDRI"}
PATH_ORIG=$PATH
SDK_TAR=cyan-mxe-focal-20200816.tar.xz
SDK_URL=https://github.com/rodlie/cyan/releases/download/1.2.2
MXE=/opt/cyan-mxe
MAGICK_RELEASE=7.0.10-28
MAGICK_SAFE=7.0.8-34
MAGICK_TYPE=Magick++-7.Q${QDEPTH}${HDRI}
LOCAL_BUILD=${LOCAL_BUILD:-0}
PKG_DEB=${PKG_DEB:-1}
GIT_SHORT=`git rev-parse --short HEAD`
BTAG="Q${QDEPTH}${HDRI}.${GIT_SHORT}"
PELF="$PKG_DIR/$PREFIX/bin/patchelf"
LIBDEPS="dpkg-shlibdeps --ignore-missing-info"
LIBDIR="lib/x86_64-linux-gnu"
LD_LIBRARY_PATH_ORIG="${LD_LIBRARY_PATH}"
VERSION=`cat ${CWD}/CMakeLists.txt | sed '/Cyan VERSION/!d;s/)//' | awk '{print $3}'`

if [ "${DISTRO}" != "focal" ]; then
    WIN32=0
    WIN64=0
else
    HEIC="yes"
fi

if [ "${LOCAL_BUILD}" = 1 ] || [ "${LINUX}" = 0 ]; then
    APT=0
fi
if [ "${APT}" = 1 ]; then
    sudo apt-get install \
    git \
    dpkg \
    dpkg-dev \
    debhelper \
    build-essential \
    cmake \
    pkg-config \
    qtbase5-dev \
    libcairo2-dev \
    libpango1.0-dev \
    libwebp-dev \
    liblcms2-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    liblzma-dev \
    zlib1g-dev \
    libopenjp2-7-dev \
    liblzma-dev \
    libbz2-dev \
    tree
    if [ "${DISTRO}" = "focal" ]; then
        sudo apt-get install libheif-dev
    fi
fi

rm -rf "${PKG_DIR}" || true
mkdir -p "${PKG_DIR}"

if [ "${DISTRO}" = "xenial" ]; then
    MAGICK_RELEASE=$MAGICK_SAFE
fi
if [ ! -d ImageMagick ] && [ "${LINUX}" = 1 ]; then
    git clone https://github.com/ImageMagick/ImageMagick
    ( cd ImageMagick ; git checkout $MAGICK_RELEASE )
fi

#cd $CWD
#if [ ! -d patchelf ]; then
#    git clone https://github.com/NixOS/patchelf
#    ( cd patchelf ;
#        git checkout 0.7
#        bash bootstrap.sh
#    )
#fi

#cd $CWD
#rm -rf build-patchelf || true
#mkdir build-patchelf && cd build-patchelf
#../patchelf/configure --prefix=${PKG_DIR}/${PREFIX}
#make && make install

if [ "${LINUX}" = 1 ]; then

cd $CWD
if [ "${CLEAN}" = 1 ]; then
    ENABLE_HDRI="disable"
    if [ "${HDRI}" = "HDRI" ]; then
        ENABLE_HDRI="enable"
    fi
    rm -rf build-magick || true
    mkdir build-magick && cd build-magick
    ../ImageMagick/configure \
--prefix=${PKG_DIR}/usr \
--libdir=${PKG_DIR}/usr/lib/x86_64-linux-gnu \
--with-package-release-name=Cyan \
--disable-static \
--enable-shared \
--with-utilities=no \
--disable-docs \
--enable-zero-configuration \
--${ENABLE_HDRI}-hdri \
--enable-largefile \
--disable-deprecated \
--disable-legacy-support \
--with-quantum-depth=${QDEPTH} \
--with-bzlib=yes \
--with-autotrace=no \
--with-djvu=no \
--with-dps=no \
--with-fftw=no \
--with-flif=no \
--with-fpx=no \
--with-fontconfig=no \
--with-freetype=no \
--with-gslib=no \
--with-gvc=no \
--with-heic=${HEIC} \
--with-jbig=no \
--with-jpeg=yes \
--with-lcms=yes \
--with-lqr=no \
--with-ltdl=no \
--with-lzma=yes \
--with-magick-plus-plus=yes \
--with-openexr=no \
--with-openjp2=yes \
--with-pango=yes \
--with-librsvg=no \
--with-perl=no \
--with-png=yes \
--with-raqm=no \
--with-raw=no \
--with-tiff=yes \
--with-webp=yes \
--with-wmf=no \
--with-x=no \
--with-xml=no \
--with-zlib=yes \
--with-zstd=no
else
    cd build-magick
fi
make -j${MKJOBS}
make install

#cd ${PKG_DIR}/${PREFIX}/lib/x86_64-linux-gnu
#for so in *.so*; do
#    $PELF --set-rpath '$ORIGIN' $so
#done

export PKG_CONFIG_PATH=${PKG_DIR}/$PREFIX/lib/x86_64-linux-gnu/pkgconfig

cd $CWD
rm -rf build-cyan || true
mkdir build-cyan && cd build-cyan
cmake \
-DCMAKE_BUILD_TYPE=Release \
-DMAGICK_PKG_CONFIG=$MAGICK_TYPE \
-DCMAKE_INSTALL_PREFIX=${PREFIX} ..
make -j${MKJOBS}
make DESTDIR=${PKG_DIR} install

rm -rf ${PKG_DIR}/$PREFIX/{etc,include}
rm -rf ${PKG_DIR}/$PREFIX/bin/{Magick*,patchelf}
rm -rf ${PKG_DIR}/$PREFIX/share/{ImageMagick-*,man}
rm -rf ${PKG_DIR}/$PREFIX/share/doc/patchelf
rm -rf ${PKG_DIR}/$PREFIX/lib/x86_64-linux-gnu/{ImageMagick-*,pkgconfig}
rm -rf ${PKG_DIR}/$PREFIX/lib/x86_64-linux-gnu/{*.la,*.so}
strip -s ${PKG_DIR}/$PREFIX/lib/x86_64-linux-gnu/*
strip -s ${PKG_DIR}/$PREFIX/bin/*

cd $CWD
if [ "${PKG_DEB}" = 1 ]; then
    DEB=${PKG_DIR}
    mkdir $DEB/DEBIAN
    CONTROL=$DEB/DEBIAN/control
    DEB_SIZE=`du -ks $DEB/usr|cut -f 1`
    echo "Package: cyan" > $CONTROL || exit 1
    echo "Version: $VERSION.$BTAG" >> $CONTROL || exit 1
    echo "Section: X11" >> $CONTROL || exit 1
    echo "Priority: optional" >> $CONTROL || exit 1
    echo "Maintainer: Ole-André Rodlie <ole.andre.rodlie@gmail.com>" >> $CONTROL || exit 1
    echo "Standards-Version: 3.9.6" >> $CONTROL || exit 1
    echo "Homepage: https://github.com/rodlie/cyan" >> $CONTROL || exit 1
    echo "Architecture: amd64" >> $CONTROL || exit 1
    echo "Description: Cyan Pixel Editor" >> $CONTROL || exit 1
    echo "Installed-Size: $DEB_SIZE" >> $CONTROL || exit 1
    cd $DEB
    mkdir debian
    touch debian/control
    mkdir -p ${PKG_DIR}/${PREFIX}/bin/DEBIAN
    export LD_LIBRARY_PATH="`pwd`/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH_ORIG"
    $LIBDEPS usr/bin/Cyan
    DEPENDS_APP="`cat debian/substvars | sed 's#shlibs:Depends=# #g'`"
    $LIBDEPS usr/lib/x86_64-linux-gnu/libMagickCore-7.Q${QDEPTH}${HDRI}-Cyan.so.7
    DEPENDS_LIB="`cat debian/substvars | sed 's#shlibs:Depends=# #g'`"
    rm -rf ${PKG_DIR}/${PREFIX}/bin/DEBIAN
    DEPENDS="${DEPENDS_APP},${DEPENDS_LIB}"
    IFS=","
    read -a deparr <<< "$DEPENDS"
    printf -v joined '%s,' "${deparr[@]}"
    echo "shlibs:Depends=${joined%,}" >> $CONTROL
    cat $CONTROL
    sudo chown root:root ${DEB}
    sudo dpkg-deb -b $DEB || exit 1
    sudo mv $CWD/build-pkg.deb $CWD/cyan_$VERSION.$BTAG-1-${DISTRO}_amd64.deb
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH_ORIG"
    if [ -d "/opt/deploy" ]; then
        cp $CWD/*.deb /opt/deploy/
    fi
fi

fi # if LINUX=1

cd $CWD
# CROSSBUILD FOR WINDOWS

if [ "${WIN32}" = 1 ] || [ "${WIN64}" = 1 ]; then
    if [ ! -d "/opt/cyan-mxe" ]; then
        mkdir -p /opt/cyan-mxe
        cd $CWD
        wget $SDK_URL/$SDK_TAR
        tar xf $SDK_TAR -C /opt/cyan-mxe
    fi
    cd $CWD
fi
if [ "${WIN32}" = 1 ]; then
    MXE_TC=i686-w64-mingw32.static
    CMAKE=${MXE_TC}-cmake
    STRIP=${MXE_TC}-strip
    WIN_BUILD=build-win32
    WIN_PKG=Cyan-$VERSION.$BTAG-Windows-x32
    export PATH=$MXE/usr/bin:$PATH_ORIG
    export PKG_CONFIG_PATH="${MXE}/usr/${MXE_TC}/lib/pkgconfig"

    cd $CWD
    rm -rf $WIN_BUILD || true
    mkdir $WIN_BUILD && cd $WIN_BUILD
    $CMAKE -DCMAKE_BUILD_TYPE=Release -DMAGICK_PKG_CONFIG=$MAGICK_TYPE -DCMAKE_INSTALL_PREFIX=/ ..
    make -j${MKJOBS}
    $STRIP -s Cyan.exe
    cd $CWD
    mkdir -p $WIN_PKG/platforms $WIN_PKG/profiles
    if [ -d "$MXE/usr/legal" ]; then
      cp -a $MXE/usr/legal $WIN_PKG/
    fi
    cp $WIN_BUILD/Cyan.exe $WIN_PKG/
    cp -a $WIN_BUILD/etc $WIN_PKG/
    cp $MXE/usr/$MXE_TC/qt5/plugins/platforms/qwindows.dll $WIN_PKG/platforms/
    cp $MXE/usr/$MXE_TC/qt5/bin/{Qt5Concurrent.dll,Qt5Core.dll,Qt5Gui.dll,Qt5Widgets.dll} $WIN_PKG/
    cp $CWD/COPYING* $WIN_PKG/
    cp $CWD/docs/README.md $WIN_PKG/
    cp $CWD/share/icc/* $WIN_PKG/profiles/
    zip -9 -r ${WIN_PKG}.zip $WIN_PKG
    if [ -d "/opt/deploy" ]; then
        cp ${WIN_PKG}.zip /opt/deploy/
    fi
fi
if [ "${WIN64}" = 1 ]; then
    MXE_TC=x86_64-w64-mingw32.static
    CMAKE=${MXE_TC}-cmake
    STRIP=${MXE_TC}-strip
    WIN_BUILD=build-win64
    WIN_PKG=Cyan-$VERSION.$BTAG-Windows-x64
    export PATH=$MXE/usr/bin:$PATH_ORIG
    export PKG_CONFIG_PATH="${MXE}/usr/${MXE_TC}/lib/pkgconfig"

    cd $CWD
    rm -rf $WIN_BUILD || true
    mkdir $WIN_BUILD && cd $WIN_BUILD
    $CMAKE -DCMAKE_BUILD_TYPE=Release -DMAGICK_PKG_CONFIG=$MAGICK_TYPE -DCMAKE_INSTALL_PREFIX=/ ..
    make -j${MKJOBS}
    $STRIP -s Cyan.exe
    cd $CWD
    mkdir -p $WIN_PKG/platforms $WIN_PKG/profiles
    if [ -d "$MXE/usr/legal" ]; then
      cp -a $MXE/usr/legal $WIN_PKG/
    fi
    cp $WIN_BUILD/Cyan.exe $WIN_PKG/
    cp -a $WIN_BUILD/etc $WIN_PKG/
    cp $MXE/usr/$MXE_TC/qt5/plugins/platforms/qwindows.dll $WIN_PKG/platforms/
    cp $MXE/usr/$MXE_TC/qt5/bin/{Qt5Concurrent.dll,Qt5Core.dll,Qt5Gui.dll,Qt5Widgets.dll} $WIN_PKG/
    cp $CWD/COPYING* $WIN_PKG/
    cp $CWD/docs/README.md $WIN_PKG/
    cp $CWD/share/icc/* $WIN_PKG/profiles/
    zip -9 -r ${WIN_PKG}.zip $WIN_PKG
    if [ -d "/opt/deploy" ]; then
        cp ${WIN_PKG}.zip /opt/deploy/
    fi
fi

cd $CWD
