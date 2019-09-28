#!/bin/sh
#
# Cyan <https://cyan.fxarena.net>
# Copyright Ole-André Rodlie, FxArena DA.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#

CWD=`pwd`
MAGICK=${MAGICK:-7.0.8-29}
MKJOBS=${MKJOBS:-4}
STATIC=${STATIC:-0}
QUANTUM=${QUANTUM:-16}
REBUILD=${REBUILD:-0}
NOBUILD=${NOBUILD:-0}
PREFIX=${PREFIX:-$CWD/ImageMagick/install}

if [ "$STATIC" = 1 ]; then
    BUILD_TYPE="--enable-static --disable-shared"
else
    BUILD_TYPE="--disable-static --enable-shared"
fi

echo "==> Building ImageMagick PREFIX=${PREFIX} MAGICK=${MAGICK} QUANTUM=${QUANTUM} MKJOBS=${MKJOBS} STATIC=${STATIC} REBUILD=${REBUILD} ..."
CWD=`pwd`
if [ ! -d "${CWD}/ImageMagick" ]; then
    git clone https://github.com/ImageMagick/ImageMagick || exit 1
    cd ImageMagick || exit 1
    git checkout $MAGICK || exit 1
fi

if [ "${NOBUILD}" = 1 ]; then
    echo "==> ImageMagick source-only, skipping!"
    exit 0
fi

if [ -f "${PREFIX}/lib/pkgconfig/Magick++.pc" ] && [ "${REBUILD}" = 0 ]; then
    echo "==> ImageMagick already installed, skipping!"
    exit 0
fi

#if [ -d "${CWD}/ImageMagick/install" ]; then
#    rm -rf "${CWD}/ImageMagick/install" || exit 1
#fi

cd "${CWD}/ImageMagick" || exit 1
CXXFLAGS="-fPIC" ./configure \
  --prefix="$PREFIX" \
  ${BUILD_TYPE} \
  --enable-zero-configuration \
  --enable-hdri \
  --enable-largefile \
  --disable-deprecated \
  --disable-pipes \
  --disable-docs \
  --disable-legacy-support \
  --with-package-release-name=Cyan \
  --with-utilities=no \
  --with-quantum-depth=${QUANTUM} \
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
  --with-heic=no \
  --with-jbig=no \
  --with-jpeg=yes \
  --with-lcms=yes \
  --with-lqr=no \
  --with-ltdl=no \
  --with-lzma=yes \
  --with-magick-plus-plus=yes \
  --with-openexr=yes \
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
  --with-zstd=no || exit 1
make -j${MKJOBS} || exit 1
make install || exit 1
