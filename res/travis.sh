#!/bin/sh

CWD=`pwd`
MXE=/opt/mxe

sudo chmod 777 /opt

echo "Extracting win64 sdk ..."
mkdir -p $MXE
wget https://sourceforge.net/projects/prepress/files/sdk/cyan-1.2-sdk-win64.tar.xz/download && mv download download.tar.xz
tar xvf download.tar.xz -C $MXE/
rm -f download.tar.xz

echo "Extracting xenial64 sdk ..."
wget https://sourceforge.net/projects/prepress/files/sdk/cyan-1.2-sdk-xenial64.tar.xz/download && mv download download.tar.xz
sudo tar xvf download.tar.xz -C /

echo "Building xenial64 ..."
mkdir -p $CWD/xenial64
cd $CWD/xenial64
cmake -DCMAKE_INSTALL_PREFIX=/usr -DBUILD_LIB=ON -DBUILD_CONVERTER=ON -DBUILD_UTILS=ON -DBUILD_TESTS=ON -DUSE_PKGCONFIG=ON ..
make
make test
make DESTDIR=`pwd`/pkg install
tree pkg

echo "Building win64 ..."
mkdir -p $CWD/win64
cd $CWD/win64
TARGET=x86_64-w64-mingw32.static
MINGW=${MXE}/usr/$TARGET
CMAKE=$TARGET-cmake
STRIP="$MXE/usr/bin/$TARGET-strip"
PATH=${MXE}/usr/bin:$PATH
PKG_CONFIG_PATH=${MINGW}/lib/pkgconfig
$CMAKE -DBUILD_LIB=OFF -DBUILD_CONVERTER=ON -DBUILD_UTILS=ON -DBUILD_TESTS=ON -DUSE_PKGCONFIG=ON ..
make
wine64 tests/tests.exe
$STRIP -s converter/Cyan.exe
zip -9 Cyan.zip converter/Cyan.exe

DATE=${DATE:-`date "+%Y%m%d%H%M"`}
#TRAVIS_COMMIT
#TRAVIS_PULL_REQUEST
#TRAVIS_TAG
#TRAVIS_BRANCH
if [ "$TRAVIS_TAG" != "" ]; then
# CI
    if [ "$TRAVIS_PULL_REQUEST" != "false" ]; then
        UPLOAD_URL=`curl --upload-file ./Cyan.zip https://transfer.sh/Cyan.zip`
        if [ "$UPLOAD_URL" != "" ]; then
            COMMENT="Win64 build for this pull request is available at $UPLOAD_URL"
            curl -H "Authorization: token $GITHUB_TOKEN" -X POST -d "{\"body\": \"$COMMENT\"}" "https://api.github.com/repos/${TRAVIS_REPO_SLUG}/issues/${TRAVIS_PULL_REQUEST}/comments"
        fi
    fi
else
# Release
fi
