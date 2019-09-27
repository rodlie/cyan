#!/bin/sh
#
# Setup (and build) Windows SDK for Cyan
#

CWD=`pwd`
BUILD=${BUILD:-0}
MXE_GIT=${MXE_GIT:-https://github.com/mxe/mxe}
MXE_COMMIT=${MXE_COMMIT:-36f30a647bc0599c8b21f579fabc8516b69559b2}

if [ -d "${CWD}/mxe" ]; then
    echo "MXE exists!"
    exit 0
else
    git clone ${MXE_GIT} || exit 1
    git checkout ${MXE_COMMIT} || exit 1
    cd mxe || exit 1
    cp -a "$CWD/share/mingw/src/"* src/ || exit 1
    cp "$CWD/share/mingw/settings.mk" settings.mk || exit 1
fi

if [ "${BUILD}" = 1 ]; then
    make MXE_USE_CCACHE= || exit 1
fi

