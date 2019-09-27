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

set -e

OS=`uname -s`
CWD=`pwd`
UBUNTU=`cat /etc/os-release | sed '/UBUNTU_CODENAME=/!d;s/UBUNTU_CODENAME=//'`
TAG=`date "+%Y%m%d%H%M"`
VERSION=`cat ${CWD}/CMakeLists.txt | sed '/Cyan VERSION/!d;s/)//' | awk '{print $3}'`
MKJOBS=${MKJOBS:-4}

echo "==> Building for Ubuntu ${UBUNTU} ..."
MJOBS=${MKJOBS} sh share/scripts/build-magick.sh
BUILD_DIR="$CWD/build-$UBUNTU"
mkdir -p "${BUILD_DIR}" && cd "${BUILD_DIR}"
PKG_CONFIG_PATH="${CWD}/ImageMagick/install/lib/pkgconfig" \
cmake \
 -DCMAKE_BUILD_TYPE=Release \
 -DMAGICK_PKG_CONFIG=Magick++-7.Q16HDRI \
 -DCMAKE_INSTALL_PREFIX=/usr \
 "$CWD"
make -j${MKJOBS}
make DESTDIR=`pwd`/pkg install
if [ ! -d "pkg/usr/lib/x86_64-linux-gnu" ]; then
    mkdir -p pkg/usr/lib/x86_64-linux-gnu
fi
cp -av "$CWD/ImageMagick/install/lib/"*Cyan* pkg/usr/lib/x86_64-linux-gnu
strip -s pkg/usr/lib/x86_64-linux-gnu/* pkg/usr/bin/*
tree -lah pkg
