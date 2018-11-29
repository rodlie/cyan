#
# Cyan <http://prepress.sf.net> <https://cyan.fxarena.net>,
# Copyright (C) 2016, 2017, 2018 Ole-André Rodlie<ole.andre.rodlie@gmail.com>
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

TARGET = Cyan
VERSION = 1.2.0
VERSION_TYPE = "BETA1"

SOURCES += \
    main.cpp \
    cyan.cpp \
    ../fxx/FXX.cxx \
    imageview.cpp \
    profiledialog.cpp \
    helpdialog.cpp
HEADERS  += \
    cyan.h \
    ../fxx/FXX.h \
    imageview.h \
    profiledialog.h \
    helpdialog.h
RESOURCES += \
    ../res/cyan.qrc \
    ../docs/docs.qrc

INCLUDEPATH += ../fxx

DESTDIR = build
OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.qrc

QT += core gui widgets concurrent
TEMPLATE = app

include(../cyan.pri)

DEFINES += CYAN_VERSION=\"\\\"$${VERSION}$${VERSION_TYPE}\\\"\"
DEFINES += CYAN_GIT=\"\\\"$${GIT}\\\"\"
QMAKE_TARGET_COMPANY = "$${TARGET}"
QMAKE_TARGET_PRODUCT = "$${TARGET}"
QMAKE_TARGET_DESCRIPTION = "Cyan"
QMAKE_TARGET_COPYRIGHT = "(c)2016-2018 Ole-Andre Rodlie"

unix:!mac {
    target.path = $${PREFIX}/bin
    target_desktop.path = $${PREFIX}/share/applications
    target_desktop.files = ../res/cyan.desktop
    target_docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}$${VERSION_TYPE}
    target_docs.files = ../COPYING
    INSTALLS += target target_desktop target_docs

    icon16.files = ../res/hicolor/16x16/apps/cyan.png
    icon16.path = $${PREFIX}/share/icons/hicolor/16x16/apps
    icon20.files = ../res/hicolor/20x20/apps/cyan.png
    icon20.path = $${PREFIX}/share/icons/hicolor/20x20/apps
    icon22.files = ../res/hicolor/22x22/apps/cyan.png
    icon22.path = $${PREFIX}/share/icons/hicolor/22x22/apps
    icon24.files = ../res/hicolor/24x24/apps/cyan.png
    icon24.path = $${PREFIX}/share/icons/hicolor/24x24/apps
    icon32.files = ../res/hicolor/32x32/apps/cyan.png
    icon32.path = $${PREFIX}/share/icons/hicolor/32x32/apps
    icon48.files = ../res/hicolor/48x48/apps/cyan.png
    icon48.path = $${PREFIX}/share/icons/hicolor/48x48/apps
    icon64.files = ../res/hicolor/64x64/apps/cyan.png
    icon64.path = $${PREFIX}/share/icons/hicolor/64x64/apps
    icon72.files = ../res/hicolor/72x72/apps/cyan.png
    icon72.path = $${PREFIX}/share/icons/hicolor/72x72/apps
    icon96.files = ../res/hicolor/96x96/apps/cyan.png
    icon96.path = $${PREFIX}/share/icons/hicolor/96x96/apps
    icon128.files = ../res/hicolor/128x128/apps/cyan.png
    icon128.path = $${PREFIX}/share/icons/hicolor/128x128/apps
    icon160.files = ../res/hicolor/160x160/apps/cyan.png
    icon160.path = $${PREFIX}/share/icons/hicolor/160x160/apps
    icon192.files = ../res/hicolor/192x192/apps/cyan.png
    icon192.path = $${PREFIX}/share/icons/hicolor/192x192/apps
    icon256.files = ../res/hicolor/256x256/apps/cyan.png
    icon256.path = $${PREFIX}/share/icons/hicolor/256x256/apps
    icon512.files = ../res/hicolor/512x512/apps/cyan.png
    icon512.path = $${PREFIX}/share/icons/hicolor/512x512/apps

    INSTALLS += icon16 icon20 icon22 \
                icon24 icon32 icon48 \
                icon64 icon72 icon96 \
                icon128 icon160 icon192 \
                icon256 icon512
}
mac {
    ICON = ../res/Cyan.icns
    QMAKE_INFO_PLIST = ../res/Info.plist
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}
win32 {
    RC_ICONS += ../res/cyan.ico
    LIBS += -lpthread
}
