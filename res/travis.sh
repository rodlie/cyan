#!/bin/sh
set -e

sudo chmod 777 /opt

CWD=`pwd`
MXE=/opt/mxe
SDK=/opt/`uname -s`
DEPLOY=/opt/deploy

COMMIT="${TRAVIS_COMMIT}"
if [ "${TRAVIS_TAG}" != "" ]; then
  COMMIT=""
fi

echo "Extracting win64 sdk ..."
mkdir -p $MXE
wget https://sourceforge.net/projects/prepress/files/sdk/cyan-1.2-sdk-win64.tar.xz/download && mv download download.tar.xz
tar xf download.tar.xz -C $MXE/
rm -f download.tar.xz

echo "Extracting xenial64 sdk ..."
wget https://sourceforge.net/projects/prepress/files/sdk/cyan-1.2-sdk-xenial64.tar.xz/download && mv download download.tar.xz
sudo tar xf download.tar.xz -C /
rm -f download.tar.xz

echo "Extracting linux64 sdk ..."
wget https://sourceforge.net/projects/prepress/files/sdk/cyan-1.2-sdk-linux64.tar.xz/download && mv download download.tar.xz
tar xf download.tar.xz -C /opt
rm -f download.tar.xz

echo "Extracting sdk legal ..."
wget https://sourceforge.net/projects/prepress/files/sdk/cyan-1.2-sdk-legal.tar.xz/download && mv download download.tar.xz
tar xf download.tar.xz -C /opt
rm -f download.tar.xz

echo "Building CI ..."
mkdir -p $CWD/ci
cd $CWD/ci
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
make test
make DESTDIR=`pwd`/pkg install
tree pkg

# I can't get cmake+qt+linux+static to work, so fallback to qmake ...
cd ${CWD}
export PATH="${SDK}/bin:/usr/bin:/bin"
export PKG_CONFIG_PATH="${SDK}/lib/pkgconfig"
echo "===> Building for Linux64 ..."
mkdir -p ${CWD}/linux64 && cd ${CWD}/linux64
qmake GIT=${COMMIT} CONFIG+=release PREFIX=/usr ../cyan.pro
make
strip -s build/Cyan
mv build/Cyan .
mkdir -p ${CWD}/linux64-test && cd ${CWD}/linux64-test
qmake ../tests.pro
make

# cmake+qt+static works on mingw, thanks MXE!
echo "===> Building win64 ..."
mkdir -p ${CWD}/win64
cd ${CWD}/win64
TARGET=x86_64-w64-mingw32.static
MINGW="${MXE}/usr/${TARGET}"
CMAKE="${TARGET}-cmake"
STRIP="${MXE}/usr/bin/${TARGET}-strip"
PATH="${MXE}/usr/bin:/usr/bin:/bin"
PKG_CONFIG_PATH="${MINGW}/lib/pkgconfig"
GIT=${COMMIT} ${CMAKE} ..
make
wine64 tests.exe
${STRIP} -s Cyan.exe

echo "===> Creating archives ..."
mkdir -p ${CWD}/deploy $DEPLOY && cd ${CWD}/deploy
TAG=`date "+%Y%m%d%H%M"`
if [ "${TRAVIS_TAG}" != "" ]; then
  echo "===> RELEASE MODE"
  TAG="${TRAVIS_TAG}"
fi

mkdir -p Cyan-${TAG}-Windows/third-party Cyan-${TAG}-Linux/third-party
cp ${CWD}/LICENSE Cyan-${TAG}-Windows/
cp ${CWD}/LICENSE Cyan-${TAG}-Linux/

cp -a /opt/legal/Windows/* Cyan-${TAG}-Windows/third-party/
cp -a /opt/legal/Linux/* Cyan-${TAG}-Linux/third-party/

cp ${CWD}/win64/Cyan.exe Cyan-${TAG}-Windows/
cp ${CWD}/linux64/Cyan Cyan-${TAG}-Linux/

cp ${CWD}/res/cyan.desktop Cyan-${TAG}-Linux/
cp -a ${CWD}/res/hicolor/128x128/apps/cyan.png Cyan-${TAG}-Linux/

7za -mx=9 a -r Cyan-${TAG}-Windows.7z Cyan-${TAG}-Windows
WIN_CHECKSUM=`sha256sum Cyan-${TAG}-Windows.7z | awk '{print $1}'`
cp Cyan-${TAG}-Windows.7z $DEPLOY/
echo "===> Windows checksum ${WIN_CHECKSUM}"

tar cvvf Cyan-${TAG}-Linux.tar Cyan-${TAG}-Linux
xz -9 Cyan-${TAG}-Linux.tar
mv Cyan-${TAG}-Linux.tar.xz Cyan-${TAG}-Linux.txz
cp Cyan-${TAG}-Linux.txz $DEPLOY/
LIN_CHECKSUM=`sha256sum Cyan-${TAG}-Linux.txz | awk '{print $1}'`
echo "===> Linux checksum ${LIN_CHECKSUM}"

if [ "${TRAVIS_PULL_REQUEST}" != "false" ] && [ "${TRAVIS_PULL_REQUEST}" != "" ]; then
echo "===> Uploading archives to transfer.sh ..."
    UPLOAD_WIN=`curl --upload-file ./Cyan-${TAG}-Windows.7z https://transfer.sh/Cyan-${TAG}-Windows.7z`
    UPLOAD_LIN=`curl --upload-file ./Cyan-${TAG}-Linux.txz https://transfer.sh/Cyan-${TAG}-Linux.txz`
    echo "===> Windows snapshot ${UPLOAD_WIN}"
    echo "===> Linux snapshot ${UPLOAD_LIN}"
    if [ "${UPLOAD_WIN}" != "" ] && [ "${UPLOAD_LIN}" != "" ]; then
        COMMENT="**CI for this pull request:** Windows build is available at ${UPLOAD_WIN} with SHA256 checksum ${WIN_CHECKSUM}. Linux build is available at ${UPLOAD_LIN} with SHA256 checksum ${LIN_CHECKSUM}."
        curl -H "Authorization: token ${GITHUB_TOKEN}" -X POST -d "{\"body\": \"${COMMENT}\"}" "https://api.github.com/repos/${TRAVIS_REPO_SLUG}/issues/${TRAVIS_PULL_REQUEST}/comments"
    fi
fi

