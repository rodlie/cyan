#!/bin/sh
# Cyan <https://github.com/olear/cyan>,
# Copyright (C) 2016, 2017 Ole-André Rodlie <olear@fxarena.net>
#
# Cyan is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.
#
# Cyan is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Cyan.  If not, see <http://www.gnu.org/licenses/gpl-2.0.html>

CWD=`pwd`
SDK=$CWD/sdk

if [ ! -f "$CWD/cyan.pro" ]; then
  echo "Can't find cyan.pro"
  exit 1
fi
VERSION=`cat $CWD/cyan.pro | sed '/VERSION =/!d' | awk '{print $3}'`

if [ ! -d "$SDK" ]; then
    sh $CWD/res/sdk.sh || exit 1
fi

QMAKE=$SDK/bin/qmake

export PKG_CONFIG_PATH=$SDK/lib/pkgconfig:$PKG_CONFIG_PATH

rm -rf $CWD/build || true
git log>CHANGES || exit 1
$QMAKE CONFIG+=release || exit 1
make || exit 1
cd $CWD/build || exit 1
strip -s Cyan || exit 1
tar cvvzf Cyan-$VERSION-Linux64.tgz Cyan || exit 1
mv Cyan-$VERSION-Linux64.tgz $CWD/ || exit 1
cd $CWD || exit 1
rm -rf cyan_plugin_import.cpp Makefile build || exit 1
