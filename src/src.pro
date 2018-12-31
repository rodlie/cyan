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

TARGET = Cyan
TEMPLATE = app
QT += widgets concurrent

SOURCES += \
    app/main.cpp \
    app/editor.cpp \
    app/about.cpp \
    app/setup.cpp \
    app/color.cpp \
    app/viewtool.cpp \
    app/messages.cpp \
    app/layers.cpp \
    app/tabs.cpp \
    canvas/view.cpp \
    canvas/layeritem.cpp \
    canvas/tileitem.cpp \
    common/common.cpp \
    common/mdi.cpp \
    colors/qtcolorpicker.cpp \
    colors/qtcolortriangle.cpp \
    colors/colorrgb.cpp \
    colors/colorcmyk.cpp \
    colors/colorhsv.cpp \
    dialog/newmediadialog.cpp \
    dialog/convertdialog.cpp \
    layers/layertree.cpp
CONFIG(with_ffmpeg): SOURCES += dialog/videodialog.cpp

HEADERS += \
    app/editor.h \
    canvas/view.h \
    canvas/layeritem.h \
    canvas/tileitem.h \
    common/common.h \
    common/mdi.h \
    colors/qtcolorpicker.h \
    colors/qtcolortriangle.h \
    colors/colorrgb.h \
    colors/colorcmyk.h \
    colors/colorhsv.h \
    dialog/newmediadialog.h \
    dialog/convertdialog.h \
    layers/layertree.h
CONFIG(with_ffmpeg): HEADERS += dialog/videodialog.h

RESOURCES += \
    share/icons.qrc \
    share/icc.qrc

INCLUDEPATH += \
    app \
    canvas \
    common \
    colors \
    dialog \
    layers

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

CONFIG += c++11

# docs
target.path = $${LIBDIR}
docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}$${VERSION_TYPE}
docs.files = \
    ../docs/LGPL_EXCEPTION.txt \
    ../docs/LICENSE.CeCILLv21 \
    ../docs/LICENSE.LGPLv21 \
    ../docs/LICENSE-ImageMagick.txt \
    ../docs/LICENSE.txt \
    ../docs/LICENSE-FATCOW.txt
INSTALLS += docs

mac {
    # we have standarized on Qt 5.9 on all platforms, and 10.10 is the lowest supported.
    # If you build against Qt 5.6 then you will be able to target 10.7 at the lowest.
    # fopenmp is only needed when you build against a static IM, but
    # Cyan on Mac is only supported through the official binaries, so that's what we hardcode:
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
    ICON = share/icons/Cyan.icns
    QMAKE_INFO_PLIST = share/Info.plist
}

# Needed by the official binary
win32-g++: LIBS += -lpthread

# add win32 icon
win32: RC_ICONS += share/icons/cyan.ico

# pkg-config
QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig

# lcms
PKGCONFIG += lcms2

# ImageMagick7
MAGICK_CONFIG = Magick++-7.Q16HDRI
!isEmpty(MAGICK): MAGICK_CONFIG = $${MAGICK}
PKG_CONFIG_BIN = pkg-config
!isEmpty(CUSTOM_PKG_CONFIG): PKG_CONFIG_BIN = $${CUSTOM_PKG_CONFIG}
PKGCONFIG += $${MAGICK_CONFIG}
CONFIG(staticlib): LIBS += `$${PKG_CONFIG_BIN} --libs --static $${MAGICK_CONFIG}`

# ffmpeg
CONFIG(with_ffmpeg) {
    DEFINES += WITH_FFMPEG
    PKGCONFIG += libavdevice \
                 libswscale \
                 libavformat \
                 libavcodec \
                 libavutil
}
