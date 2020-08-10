#!/bin/bash
set -e -x

CWD=`pwd`
MKJOBS=${MKJOBS:-4}
PREFIX=${PREFIX:-"/usr"}
APT=${APT:-1}
PKG_DIR="${CWD}/build-pkg"
CLEAN=${CLEAN:-1}
DATE=`date "+%Y%m%d%H%M"`
DISTRO=`cat /etc/os-release | sed '/UBUNTU_CODENAME/!d;s/UBUNTU_CODENAME=//'`
WIN32=${WIN32:-0}
WIN64=${WIN64:-0}
HEIC="no"
QDEPTH=16
HDRI="enable"
PATH_ORIG=$PATH
SDK_TAR=cyan-mxe-usr-focal-20200810-1.tar.xz
SDK_URL=https://github.com/rodlie/cyan/releases/download/1.2.2
SDK_LEGAL=cyan-mxe-legal-20200810.tar.xz
MXE=/opt/cyan-mxe

if [ "${DISTRO}" = "focal" ]; then
    HEIC="yes"
    WIN32=1
    WIN64=1
fi

SHORT=`git rev-parse --short HEAD`
DATE="${DATE}.${SHORT}"

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

if [ ! -d ImageMagick ]; then
    git clone https://github.com/ImageMagick/ImageMagick
fi

if [ "${CLEAN}" = 1 ]; then
    rm -rf build-magick || true
    mkdir build-magick && cd build-magick
    CXXFLAGS="-fPIC" CFLAGS="-fPIC" ../ImageMagick/configure \
--prefix=${PKG_DIR}/usr \
--enable-static \
--disable-shared \
--with-utilities=no \
--disable-docs \
--enable-zero-configuration \
--${HDRI}-hdri \
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

export PKG_CONFIG_PATH=${PKG_DIR}/$PREFIX/lib/pkgconfig

cd $CWD
rm -rf build-cyan || true
mkdir build-cyan && cd build-cyan
VERSION=`cat ../CMakeLists.txt | sed '/Cyan VERSION/!d;s/)//' | awk '{print $3}'`
cmake \
-DCMAKE_BUILD_TYPE=Release \
-DLINUX_DEPLOY=ON \
-DMAGICK_PKG_CONFIG=Magick++-7.Q16HDRI \
-DCMAKE_INSTALL_PREFIX=${PREFIX} ..
make -j${MKJOBS}
make DESTDIR=${PKG_DIR} install
rm -rf ${PKG_DIR}/$PREFIX/{etc,include} ${PKG_DIR}/$PREFIX/bin/Magick* ${PKG_DIR}/$PREFIX/lib ${PKG_DIR}/$PREFIX/share/ImageMagick-*
DEB=${PKG_DIR}
mkdir $DEB/DEBIAN
CONTROL=$DEB/DEBIAN/control
DEB_SIZE=`du -ks $DEB/usr|cut -f 1`
echo "Package: cyan" > $CONTROL || exit 1
echo "Version: $VERSION.$DATE" >> $CONTROL || exit 1
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
dpkg-shlibdeps usr/bin/Cyan
cat debian/substvars | sed 's#shlibs:Depends=#Depends: #g' >> $CONTROL
sudo chown root:root ${DEB}
sudo dpkg-deb -b $DEB || exit 1
sudo mv $CWD/build-pkg.deb $CWD/cyan_$VERSION.$DATE-1${DISTRO}_amd64.deb
if [ -d "/opt/deploy" ]; then
    cp $CWD/*.deb /opt/deploy/
fi

# CROSSBUILD FOR WINDOWS

if [ "${WIN32}" = 1 ] || [ "${WIN64}" = 1 ]; then
    if [ ! -d "/opt/cyan-mxe" ]; then
        mkdir -p /opt/cyan-mxe
        cd $CWD
        wget $SDK_URL/$SDK_TAR
        tar xf $SDK_TAR -C /opt/cyan-mxe
    fi
    cd $CWD
    wget $SDK_URL/$SDK_LEGAL
fi
if [ "${WIN32}" = 1 ]; then
    MXE_TC=i686-w64-mingw32.static
    CMAKE=${MXE_TC}-cmake
    STRIP=${MXE_TC}-strip
    WIN_BUILD=build-win32
    WIN_PKG=Cyan-$VERSION.$DATE-Windows-x32
    export PATH=$MXE/usr/bin:$PATH_ORIG
    export PKG_CONFIG_PATH="${MXE}/usr/${MXE_TC}/lib/pkgconfig"

    cd $CWD
    rm -rf $WIN_BUILD || true
    mkdir $WIN_BUILD && cd $WIN_BUILD
    $CMAKE -DCMAKE_BUILD_TYPE=Release -DENABLE_FONTCONFIG=ON -DMAGICK_PKG_CONFIG=Magick++-7.Q16HDRI -DCMAKE_INSTALL_PREFIX=/ ..
    make -j${MKJOBS}
    $STRIP -s Cyan.exe
    cd $CWD
    mkdir -p $WIN_PKG/platforms $WIN_PKG/profiles
    tar xf $SDK_LEGAL -C $WIN_PKG
    cp $WIN_BUILD/Cyan.exe $WIN_PKG/
    cp $MXE/usr/$MXE_TC/qt5/plugins/platforms/qwindows.dll $WIN_PKG/platforms/
    cp $MXE/usr/$MXE_TC/qt5/bin/{Qt5Concurrent.dll,Qt5Core.dll,Qt5Gui.dll,Qt5Widgets.dll} $WIN_PKG/
    cp $CWD/COPYING $WIN_PKG/
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
    WIN_PKG=Cyan-$VERSION.$DATE-Windows-x64
    export PATH=$MXE/usr/bin:$PATH_ORIG
    export PKG_CONFIG_PATH="${MXE}/usr/${MXE_TC}/lib/pkgconfig"

    cd $CWD
    rm -rf $WIN_BUILD || true
    mkdir $WIN_BUILD && cd $WIN_BUILD
    $CMAKE -DCMAKE_BUILD_TYPE=Release -DENABLE_FONTCONFIG=ON -DMAGICK_PKG_CONFIG=Magick++-7.Q16HDRI -DCMAKE_INSTALL_PREFIX=/ ..
    make -j${MKJOBS}
    $STRIP -s Cyan.exe
    cd $CWD
    mkdir -p $WIN_PKG/platforms $WIN_PKG/profiles
    tar xf $SDK_LEGAL -C $WIN_PKG
    cp $WIN_BUILD/Cyan.exe $WIN_PKG/
    cp $MXE/usr/$MXE_TC/qt5/plugins/platforms/qwindows.dll $WIN_PKG/platforms/
    cp $MXE/usr/$MXE_TC/qt5/bin/{Qt5Concurrent.dll,Qt5Core.dll,Qt5Gui.dll,Qt5Widgets.dll} $WIN_PKG/
    cp $CWD/COPYING $WIN_PKG/
    cp $CWD/docs/README.md $WIN_PKG/
    cp $CWD/share/icc/* $WIN_PKG/profiles/
    zip -9 -r ${WIN_PKG}.zip $WIN_PKG
    if [ -d "/opt/deploy" ]; then
        cp ${WIN_PKG}.zip /opt/deploy/
    fi
fi
