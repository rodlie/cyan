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
VERSION_TYPE = "alpha1"

SOURCES += \
    main.cpp \
    cyan.cpp \
    FXX.cxx \
    imageloader.cpp \
    imageview.cpp \
    profiledialog.cpp
HEADERS  += \
    cyan.h \
    FXX.h \
    imageloader.h \
    imageview.h \
    profiledialog.h
RESOURCES += \
    ../res/cyan.qrc

DESTDIR = build
OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.qrc

QT += core gui widgets
TEMPLATE = app

include(../cyan.pri)

DEFINES += CYAN_VERSION=\"\\\"$${VERSION}$${VERSION_TYPE}\\\"\"
QMAKE_TARGET_COMPANY = "$${TARGET}"
QMAKE_TARGET_PRODUCT = "$${TARGET}"
QMAKE_TARGET_DESCRIPTION = "Prepress Editor"
QMAKE_TARGET_COPYRIGHT = "Copyright (c)2016-2018 Ole-Andre Rodlie"

unix:!mac {
    target.path = $${PREFIX}/bin
    target_icon.path = $${PREFIX}/share/pixmaps
    target_icon.files = ../res/cyan.png
    target_desktop.path = $${PREFIX}/share/applications
    target_desktop.files = ../res/cyan.desktop
    target_docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}
    target_docs.files = COPYING README.md
    INSTALLS += target target_icon target_desktop target_docs
}
mac {
    ICON = ../res/Cyan.icns
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}
win32 {
    RC_ICONS += ../res/cyan.ico
    LIBS += -lpthread
}
