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

VERSION = 1.9.0
VERSION_TYPE = "Alpha"

QMAKE_TARGET_COMPANY = "FxArena DA"
QMAKE_TARGET_PRODUCT = "Cyan"
QMAKE_TARGET_DESCRIPTION = "Cyan"
QMAKE_TARGET_COPYRIGHT = "Copyright Ole-Andre Rodlie, FxArena DA. All rights reserved."

QT = core gui widgets concurrent

CONFIG += c++11
CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT

DEFINES += CYAN_VERSION=\"\\\"$${VERSION}\\\"\"
DEFINES += CYAN_VERSION_TYPE=\"\\\"$${VERSION_TYPE}\\\"\"
DEFINES += CYAN_GIT=\"\\\"$${GIT}\\\"\"
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# deploy
CONFIG(deploy): DEFINES += DEPLOY


# build dir
!win32-msvc {
    DESTDIR = $${top_builddir}/build
    OBJECTS_DIR = $${DESTDIR}/.obj_$${TARGET}
    MOC_DIR = $${DESTDIR}/.moc_$${TARGET}
    RCC_DIR = $${DESTDIR}/.qrc_$${TARGET}
}

# mac
mac {
    CONFIG(deploy) {
        # win/lin/mac use the same version of Qt (and depends) to avoid "issues",
        # since Qt 5.9 is the lowest we can go on Windows due to misc bugs
        # we also need to ship Qt 5.9 on Mac, this means OSX 10.10 is the
        # lowest version we can target.
        # You can however build against Qt 5.6 to get compatibility with OSX 10.7.
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -fopenmp
    }
}

# install on unix (not mac)
unix:!mac {
    # define default install paths
    isEmpty(PREFIX) : PREFIX = /usr/local
    isEmpty(DOCDIR) : DOCDIR = $$PREFIX/share/doc
    isEmpty(MANDIR) : MANDIR = $$PREFIX/share/man
    isEmpty(LIBDIR) : LIBDIR = $$PREFIX/lib$${LIBSUFFIX}
    isEmpty(BINDIR) : BINDIR = $$PREFIX/bin
    isEmpty(ICONDIR) : ICONDIR = $$PREFIX/share/icons
    isEmpty(ICCDIR) : ICCDIR = $$PREFIX/share/color/icc
    isEmpty(APPDIR) : APPDIR = $$PREFIX/share/applications
}

INCLUDEPATH += $${top_srcdir}/lib

include($${top_srcdir}/share/magick.pri)
