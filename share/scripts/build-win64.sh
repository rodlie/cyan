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

# Build Cyan for Windows x64 on Linux

# Options:
# MKJOBS : threads used to build
# MAGICK : Magick++ pkg-config name
# BUILD_DIR : Build output
# MXE : MXE directory
# MXE_TC : MXE toolchain
# SNAPSHOT : 

CWD=`pwd`
SNAPSHOT=${SNAPSHOT:-0}
BTYPE=${BTYPE:-}
MKJOBS=${MKJOBS:-4}
MAGICK=${MAGICK:-Magick++-7.Q16HDRI}
BUILD_DIR=${BUILD_DIR:-"${CWD}/build-win64"}
MXE=${MXE:-/opt/Cyan-mxe}
MXE_TC=${MXE_TC:-x86_64-w64-mingw32.shared}
CMAKE=${MXE_TC}-cmake
STRIP=${MXE_TC}-strip
SDK_URL="https://sourceforge.net/projects/prepress/files/sdk"
INNO_TAR="inno6.tar.xz"
INNO_URL="${SDK_URL}/${INNO_TAR}/download"
LEGAL_TAR="cyan-win-legal-20190927.tar.xz"
LEGAL_URL="${SDK_URL}/${LEGAL_TAR}/download"

if [ -d "${CWD}/mxe" ]; then
    MXE="${CWD}/mxe"
fi
INNO="${MXE}/inno6/ISCC.exe"
LEGAL="${MXE}/legal"

if [ ! -d "${MXE}" ]; then
    echo "Please setup MXE!"
    exit 1
fi

if [ ! -f "${INNO}" ]; then
    wget -O ${INNO_TAR} ${INNO_URL} || exit 1
    tar xf ${INNO_TAR} -C "${MXE}/" || exit 1
    rm -f ${INNO_TAR}
fi
if [ ! -d "${LEGAL}" ]; then
    wget -O ${LEGAL_TAR} ${LEGAL_URL} || exit 1
    tar xf ${LEGAL_TAR} -C "${MXE}/" || exit 1
    rm -f ${LEGAL_TAR}
fi

rm -rf "${BUILD_DIR}" || true
mkdir -p "${BUILD_DIR}" || exit 1
cd "${BUILD_DIR}" || exit 1

export PATH="${MXE}/usr/bin:${PATH}"
export PKG_CONFIG_PATH="${MXE}/usr/${MXE_TC}/lib/pkgconfig"
VERSION_ORIG=`cat ${CWD}/CMakeLists.txt | sed '/Cyan VERSION/!d;s/)//' | awk '{print $3}'`
ISS="Cyan.iss"
if [ "${SNAPSHOT}" = 1 ]; then
    export CYAN_VERSION=`sh ${CWD}/share/scripts/gitversion.sh`
fi
$CMAKE \
    -DCMAKE_BUILD_TYPE=Release \
    -DMINGW_ROOT="${MXE}/usr/${MXE_TC}" \
    -DCMAKE_INSTALL_PREFIX=/ \
    -DPROJECT_VERSION_TYPE="${BTYPE}" \
    -DMAGICK_PKG_CONFIG=${MAGICK} "${CWD}" || exit 1
make -j${MKJOBS} || exit 1
${STRIP} -s *.exe *.dll */*.dll
if [ "${SNAPSHOT}" = 1 ]; then
    VERSION_ORIG=`cat ${CWD}/CMakeLists.txt | sed '/Cyan VERSION/!d;s/)//' | awk '{print $3}'`
    cat Cyan.iss | sed 's#'${VERSION_ORIG}'#'${CYAN_VERSION}'#g' > snapshot.iss
    ISS="snapshot.iss"
fi
if [ "${CYAN_VERSION}" = "" ]; then
    export CYAN_VERSION="${VERSION_ORIG}"
fi
echo "Build installer ${ISS} ..."
cp -a "${LEGAL}/"* docs/ || exit 1
wine ${INNO} ${ISS} || exit 1
ZIP_PATH="Cyan-${CYAN_VERSION}-win64"
echo "Build archive (portable) ..."
mkdir -p "${ZIP_PATH}" || exit 1
cp *.exe *.dll "${ZIP_PATH}/" || exit 1
cp -a profiles docs platforms etc "${ZIP_PATH}/" || exit 1
7za a -mx=9 ${ZIP_PATH}.7z "${ZIP_PATH}" || exit 1
if [ ! -d "deploy" ]; then
    mkdir deploy || exit 1
fi
mv *.7z deploy/
tree -lah deploy
cp deploy/* "${CWD}/"

