#!/bin/bash

set -e -x

CWD=`pwd`
BUILD_DIR=${BUILD_DIR:-"${CWD}/../cyan-tmp"}
VERSION=${VERSION:-1.2.3}

rm -rf "${BUILD_DIR}" || true
mkdir -p "${BUILD_DIR}" && cd "${BUILD_DIR}"

git clone ${CWD} cyan-${VERSION}
cd cyan-${VERSION}
git checkout ${VERSION}
rm -rf .git* docs/_config.yml docs/_layouts docs/CNAME .travis* || true

cd "${BUILD_DIR}"
tar cvvf cyan-${VERSION}.tar cyan-${VERSION}
xz -9 cyan-${VERSION}.tar
ls -lah *.tar.xz
