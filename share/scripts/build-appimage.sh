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
SDK=${SDK:-${CWD}/sdk}
PKGCONF_PATH="${SDK}/lib/pkgconfig"
VERSION_ORIG=`cat ${CWD}/CMakeLists.txt | sed '/Cyan VERSION/!d;s/)//' | awk '{print $3}'`
SNAPSHOT=${SNAPSHOT:-0}
BTYPE=${BTYPE:-}
MKJOBS=${MKJOBS:-4}

if [ "${SNAPSHOT}" = 1 ]; then
    export CYAN_VERSION=`sh ${CWD}/share/scripts/gitversion.sh`
fi
if [ "${CYAN_VERSION}" = "" ]; then
    export CYAN_VERSION="${VERSION_ORIG}"
fi

rm -rf build-appimage || true
mkdir build-appimage || exit 1
cd build-appimage || exit 1
APPDIR=${CWD}/build-appimage/AppDir
PELF=${SDK}/bin/patchelf

export LD_LIBRARY_PATH="${SDK}/lib"
export PKG_CONFIG_PATH=${PKGCONF_PATH}
cmake \
-DLINUX_DEPLOY=ON \
-DCMAKE_INSTALL_PREFIX=/usr \
-DCMAKE_PREFIX_PATH=${SDK} \
-DCMAKE_BUILD_TYPE=Release \
-DPROJECT_VERSION_TYPE="${BTYPE}" \
-DMAGICK_PKG_CONFIG=Magick++-7.Q16HDRI .. || exit 1
make -j${MKJOBS} || exit 1

make DESTDIR=$APPDIR install || exit 1
mkdir -p $APPDIR/usr/lib $APPDIR/usr/share/metainfo $APPDIR/usr/plugins/platforms || exit 1
cp ${CWD}/share/Cyan.appdata.xml $APPDIR/usr/share/metainfo/ || exit 1

DOCS="
ImageMagick
qtbase
openexr
bzip2
jpeg
lcms2
xz
openjpeg
libpng
tiff
libwebp
zlib
ffmpeg
"
for DOC in $DOCS; do
    cp -a ${CWD}/mxe/legal/$DOC $APPDIR/usr/share/doc/Cyan-$CYAN_VERSION/ || exit 1
done

cd $APPDIR || exit 1
strip -s usr/bin/Cyan || exit 1
ln -sf usr/bin/Cyan AppRun || exit 1
cp usr/share/icons/hicolor/48x48/apps/Cyan.png . || exit 1
cp usr/share/icons/hicolor/48x48/apps/Cyan.png .DirIcon || exit 1
cp usr/share/applications/Cyan.desktop . || exit 1

echo "[Paths]" > usr/bin/qt.conf || exit 1
echo "Prefix = ../" >> usr/bin/qt.conf || exit 1
echo "Plugins = plugins" >> usr/bin/qt.conf || exit 1

SDK_SO="
libQt5Core.so.5
libQt5Concurrent.so.5
libQt5Gui.so.5
libQt5Widgets.so.5
libQt5XcbQpa.so.5
libavcodec.so.58
libavdevice.so.58
libavformat.so.58
libavutil.so.56
libswresample.so.3
libswscale.so.5
"
SYS1_SO="
libHalf.so.12
libIlmThread-2_2.so.12
libjpeg.so.62
libpng16.so.16
libz.so.1
libIex-2_2.so.12
libImath-2_2.so.12
liblcms2.so.2
libtiff.so.5
libIexMath-2_2.so.12
libwebp.so.7
libIlmImf-2_2.so.22
libopenjp2.so.7
libwebpmux.so.3
"
SYS2_SO="liblzma.so.5 libbz2.so.1"

cd $APPDIR/usr/lib || exit 1

for SO in $SDK_SO; do
    cp $SDK/lib/$SO . || exit 1
    strip -s $SO || exit 1
    $PELF --set-rpath '$ORIGIN' $SO || exit 1
done
for SO in $SYS1_SO; do
    cp /usr/lib64/$SO . || exit 1
    $PELF --set-rpath '$ORIGIN' $SO || exit 1
done
for SO in $SYS2_SO; do
    cp /lib64/$SO . || exit 1
    strip -s $SO || exit 1
    $PELF --set-rpath '$ORIGIN' $SO || exit 1
done

cp $SDK/plugins/platforms/libqxcb.so $APPDIR/usr/plugins/platforms/ || exit 1
strip -s $APPDIR/usr/plugins/platforms/libqxcb.so || exit 1
$PELF --set-rpath '$ORIGIN/../../lib' $APPDIR/usr/plugins/platforms/libqxcb.so || exit 1
 
if [ ! -f "$CWD/appimagetool-x86_64.AppImage" ]; then
    cd $CWD || exit 1
    curl -L -O https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage || exit 1
    chmod +x appimagetool-x86_64.AppImage || exit 1
fi

cd $APPDIR/.. || exit 1
export VERSION=$CYAN_VERSION
${CWD}/appimagetool-x86_64.AppImage AppDir
mv *.AppImage ${CWD}/ || exit 1

echo "DONE!"