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
        # Windows installer
        if [ "${UBUNTU}" = "bionic" ]; then
            sudo apt-get install wine-stable wine32
            # sudo apt-get install autoconf automake autopoint bash bison bzip2 flex g++ g++-multilib gettext git gperf intltool libc6-dev-i386 libgdk-pixbuf2.0-dev libltdl-dev libssl-dev libtool-bin libxml-parser-perl lzip make openssl p7zip-full patch perl pkg-config python ruby sed unzip wget xz-utils
        fi
        # Windows SDK
        if [ "${UBUNTU}" = "bionic" ]; then
            echo "==> Extracting Windows x64 SDK for Ubuntu ${UBUNTU} ..."
            mkdir -p "${MXE}"
            wget -O download.tar.xz https://sourceforge.net/projects/prepress/files/sdk/Cyan-mxe-xenial-20190902.tar.xz/download
            tar xf download.tar.xz -C "${MXE}/"
            rm -f download.tar.xz
        fi
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
    if [ "${UBUNTU}" = "bionic" ]; then
        echo "==> Building for Windows x64 on Ubuntu ${UBUNTU} ..."
        cd "${CWD}"
        MKJOBS=2 sh share/scripts/build-win64.sh
	cp "${CWD}/"*.exe "${DEPLOY}/"
    fi

    cp "$CWD/"*.deb "${DEPLOY}/"
    echo "==> Deploy result:"
    tree -lah "${DEPLOY}"
fi

#if [ "${TRAVIS_PULL_REQUEST}" != "false" ] && [ "${TRAVIS_PULL_REQUEST}" != "" ]; then
#    echo "===> Uploading archives to transfer.sh ..."
#    if [ "${OS}" = "Linux" ]; then
#      UPLOAD_WIN=`curl --upload-file ./Cyan-${TAG}-Windows.7z https://transfer.sh/Cyan-${TAG}-Windows.7z`
#      UPLOAD_LIN=`curl --upload-file ./Cyan-${TAG}-Linux.txz https://transfer.sh/Cyan-${TAG}-Linux.txz`
#      echo "===> Windows snapshot ${UPLOAD_WIN}"
#      echo "===> Linux snapshot ${UPLOAD_LIN}"
#      if [ "${UPLOAD_WIN}" != "" ] && [ "${UPLOAD_LIN}" != "" ]; then
#          COMMENT="**CI:** Windows build is available at ${UPLOAD_WIN} with SHA256 checksum ${WIN_CHECKSUM}. Linux build is available at ${UPLOAD_LIN} with SHA256 checksum ${LIN_CHECKSUM}."
#          curl -H "Authorization: token ${GITHUB_TOKEN}" -X POST -d "{\"body\": \"${COMMENT}\"}" "https://api.github.com/repos/${TRAVIS_REPO_SLUG}/issues/${TRAVIS_PULL_REQUEST}/comments"
#      fi
#    fi
#fi
