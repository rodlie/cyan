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

if [ ! -d "$CWD/sdk" ]; then
    sh $CWD/res/sdk.sh || exit 1
fi

if [ ! -f "$CWD/cyan.pro" ]; then
  echo "Can't find cyan.pro"
  exit 1
fi
VERSION=`cat $CWD/cyan.pro | sed '/VERSION =/!d' | awk '{print $3}'`

QT=~/Qt/5.6/clang_64
QMAKE=$QT/bin/qmake
DEPLOY=$QT/bin/macdeployqt

rm -rf $CWD/build || true
$QMAKE CONFIG+=release || exit 1
make || exit 1
$DEPLOY build/Cyan.app || exit 1

APP=$CWD/build/Cyan.app
PLUGINS=$APP/Contents/Plugins
FRAMEWORKS=$APP/Contents/Frameworks

rm -rf $FRAMEWORKS/QtPrintSupport.framework || exit 1
rm -f $APP/Contents/Resources/qt.conf || exit 1
mkdir -p $APP/Contents/MacOS/imageformats || exit 1
mv $PLUGINS/imageformats/libqtiff.dylib $APP/Contents/MacOS/imageformats || exit 1
mv $PLUGINS/platforms $APP/Contents/MacOS/ || exit 1
cat $CWD/res/osx/Info.plist > $APP/Contents/Info.plist || exit 1

rm -rf $PLUGINS || exit 1
mkdir -p $CWD/build/tmp || exit 1
mv $APP $CWD/build/tmp/ || exit 1

cd $CWD/build/ || exit 1
hdiutil create -volname Cyan -srcfolder tmp -ov -format UDBZ Cyan.dmg || exit 1
mv Cyan.dmg $CWD/Cyan-$VERSION.dmg || exit 1
cd $CWD || exit 1
