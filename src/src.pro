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
CONFIG += c++11
CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT

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

CONFIG(deploy) : RESOURCES += share/icons.qrc share/icc.qrc

OTHER_FILES += \
    common/ci.sh \
    share/gimp.py \
    ../docs/README.md

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
!CONFIG(deploy): DEFINES += CYAN_DEVEL

QMAKE_TARGET_COMPANY = "$${TARGET}"
QMAKE_TARGET_PRODUCT = "$${TARGET}"
QMAKE_TARGET_DESCRIPTION = "$${TARGET}"
QMAKE_TARGET_COPYRIGHT = "Copyright Ole-Andre Rodlie"

unix:!mac {
    isEmpty(PREFIX): PREFIX = /usr/local
    isEmpty(DOCDIR): DOCDIR = $$PREFIX/share/doc
    isEmpty(MANDIR): MANDIR = $$PREFIX/share/man
    isEmpty(LIBDIR): LIBDIR = $$PREFIX/lib
    isEmpty(BINDIR): BINDIR = $$PREFIX/bin
    isEmpty(ICONDIR): ICONDIR = $$PREFIX/share/icons
    isEmpty(ICCDIR): ICCDIR = $$PREFIX/share/color/icc
    isEmpty(APPDIR): APPDIR = $$PREFIX/share/applications

    target.path = $${BINDIR}
    docs1.path = $${DOCDIR}/$${TARGET}-$${VERSION}$${VERSION_TYPE}
    docs2.path = $${DOCDIR}/$${TARGET}-$${VERSION}$${VERSION_TYPE}/Icons
    docs3.path = $${DOCDIR}/$${TARGET}-$${VERSION}$${VERSION_TYPE}/QtSolutions
    icons.path = $${ICONDIR}
    icc.path = $${ICCDIR}/$${TARGET}
    desktop.path = $${APPDIR}

    desktop.files = \
        share/cyan.desktop
    icc.files = \
        share/icc/rgb.icc \
        share/icc/cmyk.icc \
        share/icc/gray.icc
    icons.files = \
        share/icons/Cyan \
        share/icons/hicolor
    docs1.files = \
        ../docs/LICENSE.CeCILLv21 \
        ../docs/LICENSE.txt \
        ../README.md
    docs2.files = \
        ../docs/LICENSE-FATCOW.txt
    docs3.files = \
        ../docs/LGPL_EXCEPTION.txt \
        ../docs/LICENSE.LGPLv21

    INSTALLS += \
        target \
        docs1 \
        docs2 \
        docs3 \
        icons \
        icc \
        desktop
}

mac {
    CONFIG(deploy) {
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -fopenmp
    }
    ICON = share/icons/Cyan.icns
    QMAKE_INFO_PLIST = share/Info.plist
}

# deploy fix
CONFIG(deploy): win32-g++: LIBS += -lpthread

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
PKGCONFIG += $${MAGICK_CONFIG}
CONFIG(deploy): LIBS += `pkg-config --libs --static $${MAGICK_CONFIG}`

# ffmpeg
CONFIG(with_ffmpeg) {
    DEFINES += WITH_FFMPEG
    PKGCONFIG += libavdevice \
                 libswscale \
                 libavformat \
                 libavcodec \
                 libavutil
}
