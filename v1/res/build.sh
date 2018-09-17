#!/bin/sh
CWD=`pwd`
rm $CWD/*.zip $CWD/*.tgz $CWD/*.dmg || true
sh $CWD/res/win.sh || exit 1
sh $CWD/res/linux.sh || exit 1
sh $CWD/res/osx.sh || exit 1
