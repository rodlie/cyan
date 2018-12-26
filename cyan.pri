# Copyright Ole-André Rodlie.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.

VERSION = 2.0.0
VERSION_TYPE=alpha1

DESTDIR = build
OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.qrc

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES += CYAN_VERSION=\"\\\"$${VERSION}$${VERSION_TYPE}\\\"\"
DEFINES += CYAN_GIT=\"\\\"$${GIT}\\\"\"
DEFINES += CYAN_DEVEL

# force static lib on release and avoid spamming the terminal with debug
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
    CONFIG += staticlib
}

# unix path for installation
isEmpty(PREFIX): PREFIX = /usr/local
isEmpty(DOCDIR): DOCDIR = $$PREFIX/share/doc
isEmpty(MANDIR): MANDIR = $$PREFIX/share/man
isEmpty(LIBDIR): LIBDIR = $$PREFIX/lib

# win32 stuff
QMAKE_TARGET_COMPANY = "$${TARGET}"
QMAKE_TARGET_PRODUCT = "$${TARGET}"
QMAKE_TARGET_DESCRIPTION = "$${TARGET}"
QMAKE_TARGET_COPYRIGHT = "Copyright Ole-Andre Rodlie"

# If you have an older compiler than gcc 4.8 maybe about time to upgrade!?
CONFIG += c++11

mac {
    # we have standarized on Qt 5.9 on all platforms, and 10.10 is the lowest supported.
    # If you build against Qt 5.6 then you will be able to target 10.7 at the lowest.
    # fopenmp is only needed when you build against a static IM, but
    # Cyan on Mac is only supported through the official binaries, so that's what we hardcode:
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

# Needed by the official binary
win32-g++: LIBS += -lpthread

# pkg-config
QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig

# lcms
PKGCONFIG += lcms2

# ImageMagick
# IM is the most important part of Cyan and also is the component that breaks the most.
# We only support the IM version included in the official binaries (and available in magick/engine), everything else is NOT SUPPORTED.
#MAGICK_PC_PATH = $${OUT_PWD}/../magick/lib/pkgconfig
#exists($${MAGICK_PC_PATH}) {
#    MAGICK_PC_CONFIG=ImageMagick++-6.Q16HDRI
#    QMAKE_CXXFLAGS += $$system("PKG_CONFIG_PATH=$${MAGICK_PC_PATH} pkg-config" \
#                               " --cflags  --static $${MAGICK_PC_CONFIG}")
#    LIBS += $$system("PKG_CONFIG_PATH=$${MAGICK_PC_PATH} pkg-config" \
#                     " --libs --static $${MAGICK_PC_CONFIG}")
#}
# Not using our ImageMagick, you are on your own ...
!exists($${MAGICK_PC_PATH}) {
#    warning("UNSUPPORTED IMAGEMAGICK VERSION! THE BUILD/TESTS WILL PROBABLY BREAK")
    MAGICK_CONFIG = Magick++-7.Q16HDRI
    !isEmpty(MAGICK): MAGICK_CONFIG = $${MAGICK}
    PKG_CONFIG_BIN = pkg-config
    !isEmpty(CUSTOM_PKG_CONFIG): PKG_CONFIG_BIN = $${CUSTOM_PKG_CONFIG}
    PKGCONFIG += $${MAGICK_CONFIG}
    CONFIG(staticlib): LIBS += `$${PKG_CONFIG_BIN} --libs --static $${MAGICK_CONFIG}`
}

# ffmpeg
# v3 is the only tested version, may work on v4
CONFIG(no_ffmpeg): DEFINES += NO_FFMPEG
!CONFIG(no_ffmpeg): PKGCONFIG += libavdevice \
                                 libswscale \
                                 libavformat \
                                 libavcodec \
                                 libavutil
