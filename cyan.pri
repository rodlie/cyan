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

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
    CONFIG += staticlib
}

isEmpty(PREFIX): PREFIX = /usr/local
isEmpty(DOCDIR): DOCDIR = $$PREFIX/share/doc
isEmpty(MANDIR): MANDIR = $$PREFIX/share/man
isEmpty(LIBDIR): LIBDIR = $$PREFIX/lib

QMAKE_TARGET_COMPANY = "$${TARGET}"
QMAKE_TARGET_PRODUCT = "$${TARGET}"
QMAKE_TARGET_DESCRIPTION = "$${TARGET}"
QMAKE_TARGET_COPYRIGHT = "Copyright Ole-Andre Rodlie"

CONFIG += c++11

# pkg-config
QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig

# lcms
PKGCONFIG += lcms2

# Magick++
MAGICK_PC_PATH = $${OUT_PWD}/../magick/lib/pkgconfig
exists($${MAGICK_PC_PATH}) {
    MAGICK_PC_CONFIG=ImageMagick++-6.Q16HDRI
    QMAKE_CXXFLAGS += $$system("PKG_CONFIG_PATH=$${MAGICK_PC_PATH} pkg-config" \
                               " --cflags  --static $${MAGICK_PC_CONFIG}")
    LIBS += $$system("PKG_CONFIG_PATH=$${MAGICK_PC_PATH} pkg-config" \
                     " --libs --static $${MAGICK_PC_CONFIG}")
}
!exists($${MAGICK_PC_PATH}) {
    warning("Cyan currently uses a fork of ImageMagick6 (magick/engine submodule), building against vanilla ImageMagick will not work. This will be fixed before final release of Cyan v2.0, but until then use the magick/engine submodule supplied with Cyan.")
    MAGICK_CONFIG = ImageMagick++
    !isEmpty(MAGICK): MAGICK_CONFIG = $${MAGICK}
    PKG_CONFIG_BIN = pkg-config
    !isEmpty(CUSTOM_PKG_CONFIG): PKG_CONFIG_BIN = $${CUSTOM_PKG_CONFIG}
    PKGCONFIG += $${MAGICK_CONFIG}
    CONFIG(staticlib): LIBS += `$${PKG_CONFIG_BIN} --libs --static $${MAGICK_CONFIG}`
}

mac {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

win32-g++: LIBS += -lpthread
