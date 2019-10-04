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

SETUP=${SETUP:-1}
if [ "${SETUP}" = 1 ]; then
    sudo chmod 777 /opt
fi

OS=`uname -s`
CWD=`pwd`
MXE=${MXE:-/opt/Cyan-mxe}
DEPLOY=${DEPLOY:-/opt/deploy}
COMMIT=${COMMIT:-${TRAVIS_COMMIT}}
UBUNTU=`cat /etc/os-release | sed '/UBUNTU_CODENAME=/!d;s/UBUNTU_CODENAME=//'`
VERSION=""

if [ "${SETUP}" = 1 ]; then
    if [ "${OS}" = "Linux" ]; then
        echo "==> Setup Ubuntu ${UBUNTU} ..."
        sudo apt remove --purge imagemagick imagemagick-common
        sudo apt-get update
        sudo apt-get install cmake pkg-config p7zip-full zip xz-utils tree dpkg qtbase5-dev libfontconfig1-dev
        sudo apt-get install libcairo2-dev libpango1.0-dev libwebp-dev liblcms2-dev libopenexr-dev libjpeg-dev libpng-dev libtiff-dev liblzma-dev zlib1g-dev libopenjp2-7-dev
    fi
fi

TAG=`date "+%Y%m%d%H%M"`
VERSION=`cat ${CWD}/CMakeLists.txt | sed '/Cyan VERSION/!d;s/)//' | awk '{print $3}'`
if [ ! -d "$DEPLOY" ]; then
    mkdir -p "$DEPLOY"
fi

if [ "${OS}" = "Linux" ]; then
    cd "${CWD}"
    MKJOBS=2 sh share/scripts/build-ubuntu.sh
fi
