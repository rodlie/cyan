#
# Copyright Ole-André Rodlie, FxArena DA.
#
# ole.andre.rodlie@gmail.com / support@fxarena.net
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
#        $${top_srcdir}/docs/LICENSE.CeCILLv21 \
#        $${top_srcdir}/docs/LICENSE.txt \
#        $${top_srcdir}/docs/LICENSE-ICC.txt \
#        $${top_srcdir}/docs/LICENSE-FatCow.txt \
#        $${top_srcdir}/docs/LICENSE-Adwaita.txt \
#        $${top_srcdir}/docs/LICENSE-CTK.txt
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
