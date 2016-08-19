# Cyan <https://github.com/olear/cyan>,
# Copyright (C) 2016 Ole-André Rodlie <olear@fxarena.net>
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

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cyan
VERSION = 1.0.0.RC2
TEMPLATE = app

SOURCES += src/main.cpp src/cyan.cpp src/magenta.cpp src/yellow.cpp
HEADERS  += src/cyan.h src/magenta.h src/yellow.h
RESOURCES += res/cyan.qrc
OTHER_FILES += res/cyan.spec

DESTDIR = build
OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.qrc

DEFINES += CYAN_VERSION=\"\\\"$${VERSION}\\\"\"
QMAKE_TARGET_COMPANY = "Cyan"
QMAKE_TARGET_PRODUCT = "Cyan"
QMAKE_TARGET_DESCRIPTION = "Prepress viewer and converter"
QMAKE_TARGET_COPYRIGHT = "Copyright (c)2016 Ole-André Rodlie <olear@fxarena.net>"

CONFIG += link_pkgconfig
PKGCONFIG += Magick++ lcms2

LIBS += `pkg-config --libs --static Magick++`

lessThan(QT_MAJOR_VERSION, 5): win32:RC_FILE += res/cyan.rc
greaterThan(QT_MAJOR_VERSION, 4): win32:RC_ICONS += res/cyan.ico

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
