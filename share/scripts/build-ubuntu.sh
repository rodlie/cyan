#!/bin/bash
set -e -x

CWD=`pwd`
MKJOBS=${MKJOBS:-4}
PKG=${PKG:-1}
PREFIX=${PREFIX:-"/usr/local"}
APT=${APT:-1}
PKG_DIR="${CWD}/build-pkg"
CLEAN=${CLEAN:-1}
DATE=`date "+%Y%m%d%H%M"`
DISTRO=`cat /etc/os-release | sed '/UBUNTU_CODENAME/!d;s/UBUNTU_CODENAME=//'`
DEPLOY=${DEPLOY:-0}

if [ "${APT}" = 1 ]; then
    apt-get -v &> /dev/null && sudo apt-get install \
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
    libheif-dev \
    liblzma-dev \
    libbz2-dev
fi

if [ ! -d ImageMagick ]; then
    git clone https://github.com/ImageMagick/ImageMagick
fi

if [ "$PKG" = 1 ]; then
    PREFIX=/usr
    rm -rf "${PKG_DIR}" || true
    mkdir -p "${PKG_DIR}"
fi

HEIC="yes"
if [ "${DISTRO}" = "bionic" ]; then
    HEIC="no"
fi
if [ "${CLEAN}" = 1 ]; then
    rm -rf build-magick || true
    mkdir build-magick && cd build-magick
    CXXFLAGS="-fPIC" CFLAGS="-fPIC" ../ImageMagick/configure \
--prefix=${PKG_DIR}/usr --enable-static --disable-shared \
--with-utilities=no \
--disable-docs \
--enable-zero-configuration \
--enable-hdri \
--enable-largefile \
--disable-deprecated \
--disable-legacy-support \
--with-quantum-depth=16 \
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
if [ "${PKG}" = 1 ]; then
    make install
else
    sudo make install
fi

if [ "${PKG}" = 1 ]; then
    export PKG_CONFIG_PATH=${PKG_DIR}/$PREFIX/lib/pkgconfig
else
    sudo ldconfig
    export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
fi

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
if [ "${PKG}" = 1 ]; then
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
    if [ "${DEPLOY}" = 1 ]; then
        sudo chown root:root ${DEB}
        sudo dpkg-deb -b $DEB || exit 1
        sudo mv $CWD/build-pkg.deb $CWD/cyan_$VERSION.$DATE-1${DISTRO}_amd64.deb
    else
        dpkg-deb -b $DEB || exit 1
        mv $CWD/build-pkg.deb $CWD/cyan_$VERSION.$DATE-1${DISTRO}_amd64.deb
    fi
    ls -lah $CWD/*.deb
    if [ "${DEPLOY}" = 1 ]; then
        sudo mkdir -p /opt/deploy
        sudo cp $CWD/*.deb /opt/deploy
        tree -lah /opt/deploy
    fi
else
    sudo make install
    sudo ldconfig
    sudo update-mime-database /usr/local/share/mime
    sudo update-desktop-database
fi
