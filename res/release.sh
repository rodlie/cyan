#!/bin/sh
#
# Build Cyan release for Windows and Linux
#
# Windows release is built on Ubuntu 20.04 using MXE
# Linux release is built on CentOS 7 using DTS-7
#
# macOS support will be added if demand is high enough.
#

set -e

CWD=`pwd`
OS=${OS:-"Windows"} # Windows is default
SDK=${SDK:-${CWD}/${OS}}
TYPE=${TYPE:-Release}
MKJOBS=${MKJOBS:-1}
MAGICK=${MAGICK:-Magick++-6.Q16HDRI}
BUILD_DIR=${BUILD_DIR:-"${CWD}/tmp"}
MXE=${MXE:-/opt/cyan-mxe}
MXE_TC=${MXE_TC:-x86_64-w64-mingw32.static}
CMAKE=${MXE_TC}-cmake
STRIP="${MXE_TC}-strip -s"
WINE=${WINE:-wine64}
QMAKE=${MXE}/usr/${MXE_TC}/qt5/bin/qmake
VERSION=`cat CMakeLists.txt | sed '/Cyan VERSION/!d;s/)//' | awk '{print $3}'`
CI=20220315

ARCH="x86_64"
if [ "${OS}" = "Windows" ]; then
    ARCH="x64"
fi

if [ "${OS}" = "Windows" ]; then
    SDK="${MXE}/usr/${MXE_TC}"
    export PATH="${MXE}/usr/bin:${MXE}/usr/${MXE_TC}/qt5/bin:${PATH}"
elif [ "${OS}" = "Linux" ]; then
    CMAKE="cmake"
    STRIP="strip -s"
    WINE=""
    QMAKE="${SDK}/bin/qmake"
    export PATH="${SDK}/usr/bin:${PATH}"
fi
export PKG_CONFIG_PATH="${SDK}/lib/pkgconfig"

echo "Building Cyan ${VERSION} ${ARCH} for ${OS} using ${SDK} ... (Ctrl+C to cancel)\n"
sleep 5
echo "Get ready ...\n\n"
sleep 5

rm -rf "${BUILD_DIR}" || true
mkdir -p "${BUILD_DIR}" && cd "${BUILD_DIR}"

${QMAKE} ../cyan.pro
make -j${MKJOBS}
AEXE=build/Cyan
if [ "${OS}" = "Windows" ]; then
    AEXE=build/Cyan.exe
fi
${STRIP} ${AEXE}
ls -lah ${AEXE}

mkdir tst
cd tst
${QMAKE} ../../tests.pro
make
TEXE="./build/tests"
if [ "${OS}" = "Windows" ]; then
    TEXE=build/tests.exe
fi
${WINE} ${TEXE}

cd "${BUILD_DIR}"

if [ ! -d "${CWD}/legal" ]; then
    curl -L https://github.com/rodlie/cyan/releases/download/continuous/cyan-sdk-legal-${CI}.tar.xz -o legal.tar.xz
    tar xvf legal.tar.xz -C "${CWD}/"
fi

ZIP_DIR=Cyan-${VERSION}-${OS}-${ARCH}
mkdir -p ${ZIP_DIR}
cp ${AEXE} ${ZIP_DIR}/
cp ../LICENSE ${ZIP_DIR}/LICENSE.TXT
cp -a ${CWD}/legal ${ZIP_DIR}/

if [ "${OS}" = "Windows" ]; then
    rm -rf ${ZIP_DIR}/legal/libxkbcommon || true
else
    rm -rf ${ZIP_DIR}/legal/gcc ${ZIP_DIR}/legal/mingw-w64 || true
fi

echo "Licenses for included open-source software.\n\nSource code :\n\nhttps://github.com/rodlie/cyan/releases/download/continuous/cyan-sdk-src-${CI}.tar\nhttps://github.com/rodlie/cyan/releases/download/continuous/cyan-mxe-${CI}.tar.xz" > ${ZIP_DIR}/legal/README.TXT
zip -9 -r ${ZIP_DIR}.zip ${ZIP_DIR}

ls -lah ${ZIP_DIR}.zip
