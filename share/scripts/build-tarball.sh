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
VERSION=`cat ${CWD}/CMakeLists.txt | sed '/Cyan VERSION/!d;s/)//' | awk '{print $3}'`
TARBALL=/tmp/Cyan-${VERSION}

if [ -d "${TARBALL}" ]; then
    rm -rf ${TARBALL} || exit 1
fi

git clone . "${TARBALL}" || exit 1
cd "${TARBALL}" || exit 1
NOBUILD=1 sh share/scripts/build-magick.sh || exit 1
rm -rf .git* ImageMagick/.git*
cd .. || exit 1
tar cvvf Cyan-${VERSION}.tar Cyan-${VERSION} || exit 1
xz -9 Cyan-${VERSION}.tar || exit 1
mv "${TARBALL}.tar.xz" "${CWD}/" || exit 1
