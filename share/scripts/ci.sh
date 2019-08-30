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
MXE=/opt/Cyan-mxe
SDK=/opt/${OS}2
DEPLOY=/opt/deploy
COMMIT=${COMMIT:-${TRAVIS_COMMIT}}
UBUNTU=`cat /etc/os-release | sed '/UBUNTU_CODENAME=/!d;s/UBUNTU_CODENAME=//'`

if [ "${SETUP}" = 1 ]; then
  if [ "${OS}" = "Linux" ]; then
    echo "==> Setup ubuntu ..."
    sudo apt remove --purge imagemagick imagemagick-common
    if [ "${UBUNTU}" = "trusty" ]; then
      sudo add-apt-repository -y ppa:george-edison55/cmake-3.x
    fi
    sudo apt-get update
    sudo apt-get install cmake pkg-config xz-utils tree dpkg qtbase5-dev
    sudo apt-get install libcairo2-dev libpango1.0-dev libwebp-dev liblcms2-dev libopenexr-dev libjpeg-dev libpng-dev libtiff-dev liblzma-dev zlib1g-dev
    #libfontconfig1-dev
    if [ "${UBUNTU}" = "bionic" ]; then
      sudo apt-get install libopenjp2-7-dev
    fi
    if [ "${UBUNTU}" = "xenial" ]; then
      sudo apt-get install p7zip-full zip wine
      sudo apt-get install libopenjpeg-dev
      echo "==> Extracting win64 sdk ..."
      mkdir -p ${MXE}
      wget https://sourceforge.net/projects/prepress/files/sdk/Cyan-MinGW-Xenial64-core-20190829.tar.xz/download && mv download download.tar.xz
      tar xf download.tar.xz -C ${MXE}/
      rm -f download.tar.xz
      wget https://sourceforge.net/projects/prepress/files/sdk/Cyan-MinGW-Xenial64-pkg-20190829.tar.xz/download && mv download download.tar.xz
      tar xf download.tar.xz -C ${MXE}/
      rm -f download.tar.xz
    fi
    echo "==> Building ImageMagick ..."
    cd ${CWD}
    git clone https://github.com/ImageMagick/ImageMagick
    cd ImageMagick
    git checkout 7.0.8-34
    ./configure --prefix=`pwd`/install --enable-static --disable-shared \
    --enable-zero-configuration \
    --enable-hdri \
    --enable-largefile \
    --disable-deprecated \
    --disable-pipes \
    --disable-docs \
    --disable-legacy-support \
    --with-package-release-name=Cyan \
    --with-utilities=no \
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
    --with-zstd=no
    make -j2
    make install
    #echo "Extracting linux64 sdk ..."
    #wget https://sourceforge.net/projects/prepress/files/sdk/cyan-sdk-20190104-linux64.tar.xz/download && mv download download.tar.xz
    #tar xf download.tar.xz -C /opt
    #rm -f download.tar.xz
  elif [ "${OS}" = "Darwin" ]; then
    curl -L https://sourceforge.net/projects/prepress/files/sdk/cyan-1.2-sdk-mac11clang6.tar.xz/download --output download.tar.xz
    tar xf download.tar.xz -C /opt
    rm -f download.tar.xz
    curl -L https://sourceforge.net/projects/prepress/files/sdk/cyan-sdk-20181226-mac11.tar.xz/download --output download.tar.xz
    tar xf download.tar.xz -C /opt
    rm -f download.tar.xz
  fi
fi

if [ "${OS}" = "Linux" ]; then
  #export PATH="${SDK}/bin:/usr/bin:/bin"
  #export PKG_CONFIG_PATH="${SDK}/lib/pkgconfig"

  echo "==> Building regular CI ..."
  mkdir -p "$CWD/build-ci" && cd "$CWD/build-ci"
  export PKG_CONFIG_PATH="${CWD}/ImageMagick/install/lib/pkgconfig"
  GIT=${COMMIT} cmake -DMAGICK_PKG_CONFIG=Magick++-7.Q16HDRI -DCMAKE_INSTALL_PREFIX=`pwd`/pkg "$CWD"
  make -j2
  make install
  tree pkg

#  mkdir -p $CWD/ci1
#  cd $CWD/ci1
#  qmake PREFIX=/usr ..
#  make
#  #make test
#  make INSTALL_ROOT=`pwd`/pkg install
#  tree pkg
#  mkdir -p $CWD/ci2
#  cd $CWD/ci2
#  qmake PREFIX=/usr CONFIG+=deploy CONFIG+=with_ffmpeg ..
#  make
#  #make test
#  make INSTALL_ROOT=`pwd`/pkg install
#  tree pkg

  #cd ${CWD}
  #echo "===> Building for Linux64 ..."
  #mkdir -p ${CWD}/linux64 && cd ${CWD}/linux64
  #qmake GIT=${COMMIT} CONFIG+=release CONFIG+=staticlib PREFIX=/usr CONFIG+=deploy ..
  #make
  #make test
  #strip -s build/Cyan
  #mv build/Cyan .

  #echo "===> Building win64 ..."
  #mkdir -p ${CWD}/win64
  #cd ${CWD}/win64
  #TARGET=x86_64-w64-mingw32.static
  #MINGW="${MXE}/usr/${TARGET}"
  #CMAKE="${TARGET}-cmake"
  #QT=${MINGW}/qt5
  #QMAKE=${QT}/bin/qmake
  #STRIP="${MXE}/usr/bin/${TARGET}-strip"
  #PATH="${MXE}/usr/bin:/usr/bin:/bin"
  #PKG_CONFIG_PATH="${MINGW}/lib/pkgconfig"
  #${QMAKE} GIT=${COMMIT} CONFIG+=release CONFIG+=staticlib CONFIG+=deploy ..
  #make
  ##make test
  #${STRIP} -s build/Cyan.exe
  #mv build/Cyan.exe .
elif [ "${OS}" = "Darwin" ]; then
  echo "===> Building mac64 ..."
  PKG_CONFIG=${SDK}/bin/pkg-config
  PKG_CONFIG_PATH="${SDK}/lib/pkgconfig:${PKG_CONFIG_PATH}"
  PATH=${SDK}/bin:/usr/bin:/bin
  mkdir -p ${CWD}/mac64 && cd ${CWD}/mac64
  qmake GIT=${COMMIT} CONFIG+=release CONFIG+=staticlib CONFIG+=deploy ..
  make
  #make test
  #MP=/opt/local/lib/libomp/libomp.dylib
  #cp ${MP} build/Cyan.app/Contents/MacOS/
  #install_name_tool -change ${MP} @executable_path/libomp.dylib build/Cyan.app/Contents/MacOS/Cyan
  #install_name_tool -id @executable_path/libomp.dylib build/Cyan.app/Contents/MacOS/libomp.dylib
  #strip -u -r build/Cyan.app/Contents/MacOS/*
  #mv build/Cyan.app .
fi

#echo "===> Creating archives ..."
#mkdir -p ${CWD}/deploy ${DEPLOY} && cd ${CWD}/deploy
#TAG=`date "+%Y%m%d%H%M"`
#if [ "${TRAVIS_TAG}" != "" ]; then
#  echo "===> RELEASE MODE"
#  TAG="${TRAVIS_TAG}"
#fi

#if [ "${OS}" = "Linux" ]; then
  #mkdir -p Cyan-${TAG}-Windows/legal Cyan-${TAG}-Linux/legal
  #cp ${CWD}/docs/LICENSE.CeCILLv21 Cyan-${TAG}-Windows/
  #cp ${CWD}/docs/LICENSE.CeCILLv21 Cyan-${TAG}-Linux/
  #cp -a /opt/legal/Windows/* Cyan-${TAG}-Windows/third-party/
  #cp -a /opt/legal/Linux/* Cyan-${TAG}-Linux/third-party/
  #cp ${CWD}/win64/Cyan.exe Cyan-${TAG}-Windows/
  #cp ${CWD}/linux64/Cyan Cyan-${TAG}-Linux/
  #cp ${CWD}/share/Cyan.desktop Cyan-${TAG}-Linux/
  #cp -a ${CWD}/share/icons/hicolor/128x128/apps/cyan.png Cyan-${TAG}-Linux/
  #7za -mx=9 a -r Cyan-${TAG}-Windows.7z Cyan-${TAG}-Windows
  #WIN_CHECKSUM=`sha256sum Cyan-${TAG}-Windows.7z | awk '{print $1}'`
  #cp Cyan-${TAG}-Windows.7z ${DEPLOY}/
  #echo "===> Windows checksum ${WIN_CHECKSUM}"
  #tar cvvf Cyan-${TAG}-Linux.tar Cyan-${TAG}-Linux
  #xz -9 Cyan-${TAG}-Linux.tar
  #mv Cyan-${TAG}-Linux.tar.xz Cyan-${TAG}-Linux.txz
  #cp Cyan-${TAG}-Linux.txz ${DEPLOY}/
  #LIN_CHECKSUM=`sha256sum Cyan-${TAG}-Linux.txz | awk '{print $1}'`
  #echo "===> Linux checksum ${LIN_CHECKSUM}"
#elif [ "${OS}" = "Darwin" ]; then
  #mkdir -p Cyan-${TAG}-Mac/legal
  ##cp ${CWD}/docs/LICENSE.CeCILLv21 Cyan-${TAG}-Mac/
  ##cp -a /opt/legal/Mac/* Cyan-${TAG}-Mac/third-party/
  #cp -a ${CWD}/mac64/Cyan.app Cyan-${TAG}-Mac/
  #hdiutil create -volname "Cyan ${TAG}" -srcfolder Cyan-${TAG}-Mac -ov -format UDBZ Cyan-${TAG}-Mac.dmg
  #cp Cyan-${TAG}-Mac.dmg ${DEPLOY}/
  #MAC_CHECKSUM=`shasum -a 256 Cyan-${TAG}-Mac.dmg | awk '{print $1}'`
  #echo "===> Mac checksum ${MAC_CHECKSUM}"
#fi

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
#    elif [ "${OS}" = "Darwin" ]; then
#      UPLOAD_MAC=`curl --upload-file ./Cyan-${TAG}-Mac.dmg https://transfer.sh/Cyan-${TAG}-Mac.dmg`
#      echo "===> Mac snapshot ${UPLOAD_MAC}"
#      if [ "${UPLOAD_MAC}" != "" ]; then
#        COMMENT="**CI:** Mac build is available at ${UPLOAD_MAC} with SHA256 checksum ${MAC_CHECKSUM}."
#        curl -H "Authorization: token ${GITHUB_TOKEN}" -X POST -d "{\"body\": \"${COMMENT}\"}" "https://api.github.com/repos/${TRAVIS_REPO_SLUG}/issues/${TRAVIS_PULL_REQUEST}/comments"
#      fi
#    fi
#fi

