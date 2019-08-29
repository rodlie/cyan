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

include($${top_srcdir}/share/common.pri)

TARGET = Cyan
TEMPLATE = app

SOURCES += \
    $${top_srcdir}/app/cyan_common.cpp \
    $${top_srcdir}/app/layers/cyan_layertree.cpp \
    $${top_srcdir}/app/layers/cyan_layertreeitem.cpp \
    $${top_srcdir}/app/layers/cyan_layerwidget.cpp \
    $${top_srcdir}/app/widgets/cyan_textwidget.cpp \
    $${top_srcdir}/app/widgets/colorrgb.cpp \
    $${top_srcdir}/app/widgets/colorcmyk.cpp \
    $${top_srcdir}/app/widgets/colorhsv.cpp \
    $${top_srcdir}/app/dialogs/newmediadialog.cpp \
    $${top_srcdir}/app/dialogs/convertdialog.cpp \
    $${top_srcdir}/app/main.cpp \
    $${top_srcdir}/app/editor.cpp \
    $${top_srcdir}/app/about.cpp \
    $${top_srcdir}/app/setup.cpp \
    $${top_srcdir}/app/color.cpp \
    $${top_srcdir}/app/viewtool.cpp \
    $${top_srcdir}/app/messages.cpp \
    $${top_srcdir}/app/layers.cpp \
    $${top_srcdir}/app/tabs.cpp \
    $${top_srcdir}/app/mdi.cpp
HEADERS += \
    $${top_srcdir}/app/cyan_common.h \
    $${top_srcdir}/app/layers/cyan_layertree.h \
    $${top_srcdir}/app/layers/cyan_layertreeitem.h \
    $${top_srcdir}/app/layers/cyan_layerwidget.h \
    $${top_srcdir}/app/widgets/cyan_textwidget.h \
    $${top_srcdir}/app/widgets/colorrgb.h \
    $${top_srcdir}/app/widgets/colorcmyk.h \
    $${top_srcdir}/app/widgets/colorhsv.h \
    $${top_srcdir}/app/dialogs/newmediadialog.h \
    $${top_srcdir}/app/dialogs/convertdialog.h \
    $${top_srcdir}/app/editor.h \
    $${top_srcdir}/app/mdi.h
INCLUDEPATH += \
    $${top_srcdir}/app/layers \
    $${top_srcdir}/app/widgets \
    $${top_srcdir}/app/dialogs \
    $${top_srcdir}/app/

# lib
CLIB = Cyan
win32 : CLIB = Cyan1
LIBS += -L$${top_builddir}/build -l$${CLIB}
QMAKE_RPATHDIR += $ORIGIN/../lib$${LIBSUFFIX}

RESOURCES += $${top_srcdir}/docs/docs2.qrc

# icons
RESOURCES += $${top_srcdir}/share/icons_bundle.qrc
#RESOURCES += $${top_srcdir}/share/icons.qrc

# bundle theme icons on debug
#CONFIG(debug, release|debug) : RESOURCES += $${top_srcdir}/share/icons_core.qrc $${top_srcdir}/share/icons_theme.qrc

# add win32 rc icon
win32 : RC_ICONS += $${top_srcdir}/share/icons/cyan.ico

# use fontconfig on mingw
win32-g++ {
    DEFINES+= USE_FC
    RESOURCES += $${top_srcdir}/share/splash.qrc
}

# mac
mac {
    ICON = $${top_srcdir}/share/icons/Cyan.icns
    QMAKE_INFO_PLIST = $${top_srcdir}/share/Info.plist
}

# install on unix (not mac)
#unix:!mac {
#    target.path = $${BINDIR}
#    docs.path = $${DOCDIR}/CyanFX-$${VERSION}
#    icons.path = $${ICONDIR}
#    hicolor.path = $${ICONDIR}
#    icc.path = $${ICCDIR}/Cyan
#    desktop.path = $${APPDIR}
#
#    desktop.files = \
#        $${top_srcdir}/share/Cyan.desktop
#    icc.files = \
#        $${top_srcdir}/share/icc/rgb.icc \
#        $${top_srcdir}/share/icc/cmyk.icc \
#        $${top_srcdir}/share/icc/gray.icc
#    icons.files = $${top_srcdir}/share/icons/Cyan
#    hicolor.files = $${top_srcdir}/share/icons/hicolor
#    docs.files = \
#        $${top_srcdir}/README.md \
#        $${top_srcdir}/docs/LICENSE.txt \
#        $${top_srcdir}/docs/LICENSE-ICC.txt \
#        $${top_srcdir}/docs/LICENSE-FatCow.txt \
#        $${top_srcdir}/docs/LICENSE-Adwaita.txt
#
#    INSTALLS += \
#        target \
#        docs \
#        hicolor \
#        desktop \
#        icons \
#        icc
#}



# ffmpeg experimental support
#CONFIG(with_ffmpeg) {
#    SOURCES += $${top_srcdir}/app/dialogs/videodialog.cpp
#    HEADERS += $${top_srcdir}/app/dialogs/videodialog.h
#    DEFINES += WITH_FFMPEG
#
#    !win32-msvc {
#        PKGCONFIG += libavdevice \
#                     libswscale \
#                     libavformat \
#                     libavcodec \
#                     libavutil
#    }
#    win32-msvc : error("Not supported/tested")
#}
