#!/bin/sh
#
# Cyan <http://cyan.fxarena.net> <https://github.com/olear/cyan>,
# Copyright (C) 2016, 2017 Ole-André Rodlie
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
#

CWD=`pwd`
HOST=10.10.10.133
SSH="ssh $HOST"
ID=$$
CYAN="~/Cyan-build"

git log >CHANGES || exit 1
$SSH "mkdir -p $CYAN ; rm -rf $CYAN/res $CYAN/src $CYAN/README.md $CYAN/cyan.pro $CYAN/COPYING $CYAN/CHANGES" || exit 1
scp -r $CWD/.qmake* $CWD/res $CWD/src $CWD/COPYING $CWD/CHANGES $CWD/README.md $CWD/cyan.pro $HOST:$CYAN/ || exit 1
$SSH "cd $CYAN ; sh res/osx/build.sh" || exit 1
scp $HOST:$CYAN/*.dmg . || exit 1
