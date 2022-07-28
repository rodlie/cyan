#!/bin/sh
#
# Build Cyan release for Windows/macOS/OSX/Linux
#
# Windows release is built on Ubuntu 20.04 using MXE (GCC 7)
# Linux release is built on CentOS 7 using DTS (GCC 7)
# macOS release is built on macOS 10.13 (CLANG 9.0 from macports)
# OSX release is built in OSX 10.11 (CLANG 9.0 from macports)
#

set -e

CWD=`pwd`
OS=${OS:-"Windows"} # Windows is default (use Windows/Linux/Darwin)
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
OSX_MIN=10.13
CLANG=9.0
CLANG_ROOT="/opt/local"
ARCH="x86_64"
LEGACY_OSX=${LEGACY_OSX:-0}
MAC_TAG=macOS
DMG_FORMAT=UDBZ

if [ "${LEGACY_OSX}" = 1 ]; then
    OSX_MIN=10.10
    MAC_TAG=OSX
fi

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
elif [ "${OS}" = "Darwin" ]; then
    CMAKE="$CLANG_ROOT/bin/cmake"
    STRIP="strip -u -r"
    WINE=""
    QMAKE="${SDK}/bin/qmake"
    export CC="$CLANG_ROOT/bin/clang-mp-$CLANG -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
    export CXX="$CLANG_ROOT/bin/clang++-mp-$CLANG -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
    export PATH=$SDK/bin:/usr/bin:/usr/sbin:/bin:/sbin
fi

export PKG_CONFIG_PATH="${SDK}/lib/pkgconfig"

echo "Building Cyan ${VERSION} ${ARCH} for ${OS} using ${SDK} ... (Ctrl+C to cancel)\n"
sleep 5
echo "Get ready ...\n\n"
sleep 5

rm -rf "${BUILD_DIR}" || true
mkdir -p "${BUILD_DIR}" && cd "${BUILD_DIR}"

${QMAKE} ../cyan.pro OSX_MIN=${OSX_MIN}
make -j${MKJOBS}
AEXE=build/Cyan
if [ "${OS}" = "Windows" ]; then
    AEXE=build/Cyan.exe
fi

if [ "${OS}" != "Darwin" ]; then
    ${STRIP} ${AEXE}
    ls -lah ${AEXE}
fi

if [ "${OS}" = "Darwin" ]; then
    AEXE=build/Cyan.app
    $SDK/bin/macdeployqt ${AEXE}
    ${STRIP} ${AEXE}/Contents/MacOS/*
    ls -lah ${AEXE}/Contents/MacOS/*
    otool -L ${AEXE}/Contents/MacOS/*
fi

# qt 5.12.12 testlib is currently broken on macos
if [ "${OS}" != "Darwin" ]; then
    mkdir tst
    cd tst
    ${QMAKE} ../../tests.pro
    make
    if [ "${OS}" = "Windows" ]; then
        ${WINE} build/tests.exe
    fi
fi

cd "${BUILD_DIR}"

if [ ! -d "${CWD}/legal" ]; then
    curl -L https://github.com/rodlie/cyan/releases/download/continuous/cyan-sdk-legal-${CI}.tar.xz -o legal.tar.xz
    tar xvf legal.tar.xz -C "${CWD}/"
fi

if [ "${OS}" = "Darwin" ]; then
    ZIP_DIR=Cyan-${VERSION}-${MAC_TAG}-${ARCH}
else
    ZIP_DIR=Cyan-${VERSION}-${OS}-${ARCH}
fi

mkdir -p ${ZIP_DIR}
cp -a ${AEXE} ${ZIP_DIR}/
cp ../LICENSE ${ZIP_DIR}/LICENSE.TXT
cp -a ${CWD}/legal ${ZIP_DIR}/

if [ "${OS}" != "Windows" ]; then
    rm -rf ${ZIP_DIR}/legal/gcc ${ZIP_DIR}/legal/mingw-w64 || true
fi
if [ "${OS}" != "Linux" ]; then
    rm -rf ${ZIP_DIR}/legal/libxkbcommon || true
fi

echo "Licenses for included open-source software.\n\nSource code :\n\nhttps://github.com/rodlie/cyan/releases/download/continuous/cyan-sdk-src-${CI}.tar\nhttps://github.com/rodlie/cyan/releases/download/continuous/cyan-mxe-${CI}.tar.xz" > ${ZIP_DIR}/legal/README.TXT

EXT=zip
if [ "${OS}" = "Linux" ]; then
    EXT=tgz
    tar czf ${ZIP_DIR}.${EXT} ${ZIP_DIR}
elif [ "${OS}" = "Darwin" ]; then
    EXT=dmg
    hdiutil create -volname "Cyan $VERSION" -srcfolder ${ZIP_DIR} -ov -format ${DMG_FORMAT} ${ZIP_DIR}.${EXT}
else
    zip -9 -r ${ZIP_DIR}.${EXT} ${ZIP_DIR}
fi

ls -lah ${ZIP_DIR}.${EXT}
