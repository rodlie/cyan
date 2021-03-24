#!/bin/bash
#
# Build and package/deploy Cyan on Ubuntu Bionic+
#

set -e -x

CWD=`pwd`
MKJOBS=${MKJOBS:-4}
PREFIX=${PREFIX:-"/usr"}
APT=${APT:-0}
PKG_DIR="${CWD}/build-pkg"
CLEAN=${CLEAN:-1}
DATE=`date "+%Y%m%d%H%M"`
DISTRO=${DISTRO:-`cat /etc/os-release | sed '/UBUNTU_CODENAME/!d;s/UBUNTU_CODENAME=//'`}
HEIC="yes"
QDEPTH=${QDEPTH:-16}
HDRI=${HDRI:-"HDRI"}
PATH_ORIG=$PATH
MAGICK_RELEASE=5da028039d605ccfc5cda978dce30ff65346c2f4
MAGICK_TYPE=Magick++-7.Q${QDEPTH}${HDRI}
PKG_DEB=${PKG_DEB:-1}
GIT_SHORT=`git rev-parse --short HEAD`
BTAG="Q${QDEPTH}${HDRI}.${GIT_SHORT}"
LIBDEPS="dpkg-shlibdeps --ignore-missing-info"
LIBDIR="lib/x86_64-linux-gnu"
LD_LIBRARY_PATH_ORIG="${LD_LIBRARY_PATH}"
VERSION=`cat ${CWD}/CMakeLists.txt | sed '/Cyan VERSION/!d;s/)//' | awk '{print $3}'`

if [ "$DISTRO" = "bionic" ]; then
    HEIC="no"
fi

if [ "${APT}" = 1 ]; then
    sudo apt-get install git dpkg dpkg-dev debhelper build-essential cmake pkg-config qtbase5-dev libcairo2-dev libpango1.0-dev libwebp-dev liblcms2-dev libjpeg-dev libpng-dev libtiff-dev liblzma-dev zlib1g-dev libopenjp2-7-dev liblzma-dev libbz2-dev libheif-dev libgoogle-perftools-dev
fi

rm -rf "${PKG_DIR}" || true
mkdir -p "${PKG_DIR}"

if [ ! -d ImageMagick ]; then
    git clone https://github.com/ImageMagick/ImageMagick
    ( cd ImageMagick ; git checkout $MAGICK_RELEASE )
fi

cd $CWD
if [ "${CLEAN}" = 1 ]; then
    ENABLE_HDRI="disable"
    if [ "${HDRI}" = "HDRI" ]; then
        ENABLE_HDRI="enable"
    fi
    rm -rf build-magick || true
    mkdir build-magick && cd build-magick
    ../ImageMagick/configure --prefix=${PKG_DIR}/usr --libdir=${PKG_DIR}/usr/lib/x86_64-linux-gnu --with-package-release-name=Cyan --disable-static --enable-shared --with-utilities=no --disable-docs --enable-zero-configuration --${ENABLE_HDRI}-hdri --enable-largefile --disable-deprecated --disable-legacy-support --with-quantum-depth=${QDEPTH} --with-tcmalloc=yes --with-bzlib=yes --with-autotrace=no --with-djvu=no --with-dps=no --with-fftw=no --with-flif=no --with-fpx=no --with-fontconfig=no --with-freetype=no --with-gslib=no --with-gvc=no --with-heic=${HEIC} --with-jbig=no --with-jpeg=yes --with-lcms=yes --with-lqr=no --with-ltdl=no --with-lzma=yes --with-magick-plus-plus=yes --with-openexr=no --with-openjp2=yes --with-pango=no --with-librsvg=no --with-perl=no --with-png=yes --with-raqm=no --with-raw=no --with-tiff=yes --with-webp=yes --with-wmf=no --with-x=no --with-xml=yes --with-zlib=yes --with-zstd=no
else
    cd build-magick
fi
make -j${MKJOBS}
make install

export PKG_CONFIG_PATH=${PKG_DIR}/$PREFIX/lib/x86_64-linux-gnu/pkgconfig

cd $CWD
rm -rf build-cyan || true
mkdir build-cyan && cd build-cyan
cmake -DLINUX_DEPLOY=ON -DCMAKE_BUILD_TYPE=Release -DMAGICK_PKG_CONFIG=$MAGICK_TYPE -DCMAKE_INSTALL_PREFIX=${PREFIX} ..
make -j${MKJOBS}
make DESTDIR=${PKG_DIR} install

MAGICK_LICENSE=${PKG_DIR}/$PREFIX/share/doc/Cyan-$VERSION/IMAGEMAGICK_LICENSE
cat $CWD/ImageMagick/LICENSE > $MAGICK_LICENSE
echo "" >> $MAGICK_LICENSE
cat $CWD/ImageMagick/AUTHORS.txt >> $MAGICK_LICENSE

rm -rf ${PKG_DIR}/$PREFIX/{etc,include}
rm -rf ${PKG_DIR}/$PREFIX/bin/Magick*
rm -rf ${PKG_DIR}/$PREFIX/share/{ImageMagick-*,man}
rm -rf ${PKG_DIR}/$PREFIX/lib/x86_64-linux-gnu/{ImageMagick-*,pkgconfig}
rm -rf ${PKG_DIR}/$PREFIX/lib/x86_64-linux-gnu/{*.la,*.a}
strip -s ${PKG_DIR}/$PREFIX/bin/*
strip -s ${PKG_DIR}/$PREFIX/lib/x86_64-linux-gnu/*

cd $CWD
if [ "${PKG_DEB}" = 1 ]; then
    DEB=${PKG_DIR}
    mkdir $DEB/DEBIAN || true
    CONTROL=$DEB/DEBIAN/control
    DEB_SIZE=`du -ks $DEB/usr|cut -f 1`
    echo "Package: cyan" > $CONTROL || exit 1
    echo "Version: $VERSION.$BTAG" >> $CONTROL || exit 1
    echo "Section: X11" >> $CONTROL || exit 1
    echo "Priority: optional" >> $CONTROL || exit 1
    echo "Maintainer: Cyan <https://github.com/rodlie/cyan>" >> $CONTROL || exit 1
    echo "Standards-Version: 3.9.6" >> $CONTROL || exit 1
    echo "Homepage: https://github.com/rodlie/cyan" >> $CONTROL || exit 1
    echo "Architecture: amd64" >> $CONTROL || exit 1
    echo "Description: Cyan Pixel Editor" >> $CONTROL || exit 1
    echo "Installed-Size: $DEB_SIZE" >> $CONTROL || exit 1
    cd $DEB
    mkdir debian || true
    touch debian/control
    mkdir -p ${PKG_DIR}/${PREFIX}/bin/DEBIAN
    export LD_LIBRARY_PATH="`pwd`/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH_ORIG"
    $LIBDEPS usr/bin/Cyan
    DEPENDS_APP="`cat debian/substvars | sed 's#shlibs:Depends=# #g'`"
    $LIBDEPS usr/lib/x86_64-linux-gnu/libMagickCore-7.Q${QDEPTH}${HDRI}-Cyan.so.9
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
    sudo mv $CWD/build-pkg.deb $CWD/cyan_$VERSION.$BTAG-${DISTRO}_amd64.deb
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH_ORIG"
fi

echo "DONE"
