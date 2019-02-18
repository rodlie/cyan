# Cyan Image Editor
#
# <https://cyan.fxarena.net>
# <http://prepress.sf.net>
# <https://github.com/rodlie/cyan>
# <https://sourceforge.net/projects/prepress>
#
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

TARGET = Cyan
VERSION = 2.0.0
VERSION_EXTRA = alpha1

TEMPLATE = app
QT += concurrent widgets

CONFIG += c++11
CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT

QMAKE_TARGET_COMPANY = "Cyan"
QMAKE_TARGET_PRODUCT = "Cyan"
QMAKE_TARGET_DESCRIPTION = "Cyan Image Editor"
QMAKE_TARGET_COPYRIGHT = "(c) Ole-Andre Rodlie <ole.andre.rodlie@gmail.com>"

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
DEFINES += CYAN_VERSION=\"\\\"$${VERSION}$${VERSION_EXTRA}\\\"\"
DEFINES += CYAN_GIT=\"\\\"$${GIT}\\\"\"
!CONFIG(deploy): DEFINES += CYAN_DEVEL

SOURCES += \
    $${top_srcdir}/common/cyan_common.cpp \
    $${top_srcdir}/common/cyan_tileitem.cpp \
    $${top_srcdir}/common/cyan_render.cpp \
    $${top_srcdir}/common/cyan_layeritem.cpp \
    $${top_srcdir}/common/cyan_view.cpp \
    $${top_srcdir}/common/cyan_colorconvert.cpp \
    $${top_srcdir}/layers/cyan_layertree.cpp \
    $${top_srcdir}/layers/cyan_layertreeitem.cpp \
    $${top_srcdir}/layers/cyan_layerwidget.cpp \
    $${top_srcdir}/colors/qtcolorpicker.cpp \
    $${top_srcdir}/colors/qtcolortriangle.cpp \
    $${top_srcdir}/colors/colorrgb.cpp \
    $${top_srcdir}/colors/colorcmyk.cpp \
    $${top_srcdir}/colors/colorhsv.cpp \
    $${top_srcdir}/editor/main.cpp \
    $${top_srcdir}/editor/editor.cpp \
    $${top_srcdir}/editor/about.cpp \
    $${top_srcdir}/editor/setup.cpp \
    $${top_srcdir}/editor/color.cpp \
    $${top_srcdir}/editor/viewtool.cpp \
    $${top_srcdir}/editor/messages.cpp \
    $${top_srcdir}/editor/layers.cpp \
    $${top_srcdir}/editor/tabs.cpp \
    $${top_srcdir}/editor/mdi.cpp \
    $${top_srcdir}/dialogs/newmediadialog.cpp \
    $${top_srcdir}/dialogs/convertdialog.cpp

HEADERS += \
    $${top_srcdir}/common/cyan_common.h \
    $${top_srcdir}/common/cyan_tileitem.h \
    $${top_srcdir}/common/cyan_render.h \
    $${top_srcdir}/common/cyan_layeritem.h \
    $${top_srcdir}/common/cyan_view.h \
    $${top_srcdir}/common/cyan_colorconvert.h \
    $${top_srcdir}/layers/cyan_layertree.h \
    $${top_srcdir}/layers/cyan_layertreeitem.h \
    $${top_srcdir}/layers/cyan_layerwidget.h \
    $${top_srcdir}/colors/qtcolorpicker.h \
    $${top_srcdir}/colors/qtcolortriangle.h \
    $${top_srcdir}/colors/colorrgb.h \
    $${top_srcdir}/colors/colorcmyk.h \
    $${top_srcdir}/colors/colorhsv.h \
    $${top_srcdir}/editor/editor.h \
    $${top_srcdir}/editor/mdi.h \
    $${top_srcdir}/dialogs/newmediadialog.h \
    $${top_srcdir}/dialogs/convertdialog.h

INCLUDEPATH += \
    $${top_srcdir}/common \
    $${top_srcdir}/layers \
    $${top_srcdir}/colors \
    $${top_srcdir}/dialogs

# misc related
OTHER_FILES += \
    $${top_srcdir}/scripts/ci.sh \
    $${top_srcdir}/scripts/gimp.py \
    $${top_srcdir}/docs/README.md \
    $${top_srcdir}/README.md

# build dir
!win32-msvc {
    DESTDIR = $${top_builddir}/build
    OBJECTS_DIR = $${DESTDIR}/.obj_$${TARGET}
    MOC_DIR = $${DESTDIR}/.moc_$${TARGET}
    RCC_DIR = $${DESTDIR}/.qrc_$${TARGET}
}

# bundle core icons
RESOURCES += $${top_srcdir}/share/icons_core.qrc

# bundle theme icons and color profiles on deploy
CONFIG(deploy) : RESOURCES += $${top_srcdir}/share/icons_theme.qrc $${top_srcdir}/share/icc.qrc

# bundle theme icons and color profiles on msvc
win32-msvc : RESOURCES += $${top_srcdir}/share/icons_theme.qrc $${top_srcdir}/share/icc.qrc

# bundle theme icons on debug
CONFIG(debug, release|debug) : RESOURCES += $${top_srcdir}/share/icons_theme.qrc

# add win32 rc icon
win32 : RC_ICONS += $${top_srcdir}/share/icons/cyan.ico

# mingw deploy+static fix
CONFIG(deploy) : win32-g++ : LIBS += -lpthread

# mac
mac {
    CONFIG(deploy) {
        # win/lin/mac use the same version of Qt (and depends) to avoid "issues",
        # since Qt 5.9 is the lowest we can go on Windows due to misc bugs
        # we also need to ship Qt 5.9 on Mac, this means OSX 10.10 is the
        # lowest version we can target.
        # You can however build against Qt 5.6 to get compatibility with OSX 10.7.
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
        # static fix
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -fopenmp
    }
    ICON = $${top_srcdir}/share/icons/Cyan.icns
    QMAKE_INFO_PLIST = $${top_srcdir}/share/Info.plist
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

    target.path = $${BINDIR}
    docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}$${VERSION_EXTRA}
    icons.path = $${ICONDIR}
    hicolor.path = $${ICONDIR}
    icc.path = $${ICCDIR}/$${TARGET}
    desktop.path = $${APPDIR}

    desktop.files = \
        $${top_srcdir}/share/cyan.desktop
    icc.files = \
        $${top_srcdir}/share/icc/rgb.icc \
        $${top_srcdir}/share/icc/cmyk.icc \
        $${top_srcdir}/share/icc/gray.icc \
        $${top_srcdir}/docs/LICENSE-ICC.txt \
    icons.files = \
        $${top_srcdir}/share/icons/Cyan \
        $${top_srcdir}/docs/LICENSE-FatCow.txt \
        $${top_srcdir}/docs/LICENSE-Adwaita.txt \
    hicolor.files = $${top_srcdir}/share/icons/hicolor
    docs.files = \
        $${top_srcdir}/README.md \
        $${top_srcdir}/docs/LICENSE.CeCILLv21 \
        $${top_srcdir}/docs/LICENSE.txt \
        $${top_srcdir}/docs/LGPL_EXCEPTION.txt \
        $${top_srcdir}/docs/LICENSE.LGPLv21

    INSTALLS += \
        target \
        docs \
        hicolor \
        desktop
    !CONFIG(deploy): INSTALLS += icons icc
}

# Use ImageMagick-Windows on msvc
win32-msvc {
    # path to your "VisualMagick" build
    # follow the instructions on https://github.com/ImageMagick/ImageMagick-Windows to build it
    # DO NOT USE PRECOMPILED BINARIES FROM IMAGEMAGICK!!!
    isEmpty(MAGICK_WINDOWS_PATH) : MAGICK_WINDOWS_PATH = C:/Users/olear/Documents/ImageMagick-Windows
    INCLUDEPATH += \
        $${MAGICK_WINDOWS_PATH}/ImageMagick/Magick++/lib \
        $${MAGICK_WINDOWS_PATH}/ImageMagick \
        $${MAGICK_WINDOWS_PATH}/lcms/include
    LIBS += \
        -L$${MAGICK_WINDOWS_PATH}/VisualMagick/lib \
        -L$${MAGICK_WINDOWS_PATH}/VisualMagick/bin \
        -lCORE_RL_lcms_ \
        -lCORE_RL_MagickCore_ \
        -lCORE_RL_MagickWand_ \
        -lCORE_RL_Magick++_
}

# Use pkg-config on anything else
!win32-msvc {
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig
    # optional pkg-config name for Magick++, default is Magick++-7.Q16HDRI
    isEmpty(MAGICK) : MAGICK = Magick++-7.Q16HDRI
    PKGCONFIG += $${MAGICK} lcms2
    # deploy+static fix
    CONFIG(deploy) : LIBS += `pkg-config --libs --static $${MAGICK}`
}

# ffmpeg experimental support
# supports video (frames) and embedded images in audio files (coverart)
CONFIG(with_ffmpeg) {
    SOURCES += $${top_srcdir}/dialogs/videodialog.cpp
    HEADERS += $${top_srcdir}/dialogs/videodialog.h
    DEFINES += WITH_FFMPEG
    !win32-msvc {
        PKGCONFIG += libavdevice \
                     libswscale \
                     libavformat \
                     libavcodec \
                     libavutil
    }
    win32-msvc : error("Not supported/tested")
}
