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

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Cyan
VERSION = 1.1.0
TEMPLATE = app

SOURCES += src/main.cpp src/cyan.cpp src/magenta.cpp src/yellow.cpp
HEADERS  += src/cyan.h src/magenta.h src/yellow.h
RESOURCES += res/cyan.qrc
OTHER_FILES += res/cyan.spec res/win.sh res/osx.sh res/sdk.sh res/build_linux.sh README.md res/osx/Info.plist

DESTDIR = build
OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.qrc

DEFINES += CYAN_VERSION=\"\\\"$${VERSION}\\\"\"
QMAKE_TARGET_COMPANY = "Cyan"
QMAKE_TARGET_PRODUCT = "Cyan"
QMAKE_TARGET_DESCRIPTION = "Prepress Toolkit"
QMAKE_TARGET_COPYRIGHT = "Copyright (c)2016, 2017 Ole-Andre Rodlie <olear@fxarena.net>"

!mac {
    CONFIG += link_pkgconfig
    PKGCONFIG += lcms2
    CONFIG(gmagick) {
        DEFINES += GMAGICK
        PKGCONFIG += GraphicsMagick++
        LIBS += `pkg-config --libs --static GraphicsMagick++`
    } else {
        PKGCONFIG += Magick++
        LIBS += `pkg-config --libs --static Magick++`
    }
}

lessThan(QT_MAJOR_VERSION, 5): win32:RC_FILE += res/cyan.rc
greaterThan(QT_MAJOR_VERSION, 4): win32:RC_ICONS += res/cyan.ico

#release {
#    DEFINES += QT_NO_DEBUG_OUTPUT
#}

unix:!mac {
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    isEmpty(DOCDIR) {
        DOCDIR = $$PREFIX/share/doc
    }
    target.path = $${PREFIX}/bin
    target_icon.path = $${PREFIX}/share/pixmaps
    target_icon.files = res/cyan.png
    target_desktop.path = $${PREFIX}/share/applications
    target_desktop.files = res/cyan.desktop
    target_docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}
    target_docs.files = COPYING README.md
    INSTALLS += target target_icon target_desktop target_docs
}

mac {
    # Run res/sdk.sh to build third-party
    ICON = $$top_srcdir/res/Cyan.icns
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
    DEFINES += MAGICKCORE_HDRI_ENABLE=1 MAGICKCORE_QUANTUM_DEPTH=32
    INCLUDEPATH += $$top_srcdir/sdk/include $$top_srcdir/sdk/include/ImageMagick-6
    LIBS += -L$$top_srcdir/sdk/lib -lMagickCore-6.Q32HDRI -lMagickWand-6.Q32HDRI -lMagick++-6.Q32HDRI \
            -lz -llcms2 -ltiff -ljpeg -lpng16 -lbz2 -lz -lm
}

CONFIG(magick7) {
    DEFINES += MAGICK7
    mac {
        INCLUDEPATH -= $$top_srcdir/sdk/include/ImageMagick-6
        INCLUDEPATH += $$top_srcdir/sdk/include/ImageMagick-7
        LIBS -= -lMagickCore-6.Q32HDRI -lMagickWand-6.Q32HDRI -lMagick++-6.Q32HDRI
        LIBS += -lMagickCore-7.Q32HDRI -lMagickWand-7.Q32HDRI -lMagick++-7.Q32HDRI
    }
}

CONFIG(mxe) {
    # Fix issues with static mxe
    LIBS += -lpthread
    #-lws2_32 -lole32
}
