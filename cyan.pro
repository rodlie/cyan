# Copyright Ole-André Rodlie, INRIA.
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
VERSION = 1.2.2

SOURCES += \
    src/main.cpp \
    src/cyan.cpp \
    src/FXX.cpp \
    src/imageview.cpp \
    src/profiledialog.cpp \
    src/openlayerdialog.cpp \
    src/helpdialog.cpp
HEADERS += \
    src/cyan.h \
    src/FXX.h \
    src/imageview.h \
    src/profiledialog.h \
    src/openlayerdialog.h \
    src/helpdialog.h
RESOURCES += \
    res/cyan.qrc \
    docs/docs.qrc

DESTDIR = build
OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.qrc

QT += widgets concurrent
TEMPLATE = app

CONFIG += c++11
QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += lcms2
MAGICK_CONFIG = Magick++
!isEmpty(MAGICK): MAGICK_CONFIG = $${MAGICK}
PKG_CONFIG_BIN = pkg-config
!isEmpty(CUSTOM_PKG_CONFIG): PKG_CONFIG_BIN = $${CUSTOM_PKG_CONFIG}

PKGCONFIG += $${MAGICK_CONFIG}
LIBS += `$${PKG_CONFIG_BIN} --libs --static $${MAGICK_CONFIG}`

isEmpty(PREFIX): PREFIX = /usr/local
isEmpty(DOCDIR): DOCDIR = $$PREFIX/share/doc
isEmpty(MANDIR): MANDIR = $$PREFIX/share/man

DEFINES += CYAN_VERSION=\"\\\"$${VERSION}$${VERSION_TYPE}\\\"\"
DEFINES += CYAN_GIT=\"\\\"$${GIT}\\\"\"
QMAKE_TARGET_COMPANY = "$${TARGET}"
QMAKE_TARGET_PRODUCT = "$${TARGET}"
QMAKE_TARGET_DESCRIPTION = "$${TARGET}"
QMAKE_TARGET_COPYRIGHT = "Copyright Ole-Andre Rodlie, INRIA"

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

unix:!mac {
    target.path = $${PREFIX}/bin
    target_desktop.path = $${PREFIX}/share/applications
    target_desktop.files = res/cyan.desktop
    target_docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}
    target_docs.files = LICENSE
    INSTALLS += target target_desktop target_docs

    icon16.files = res/hicolor/16x16/apps/cyan.png
    icon16.path = $${PREFIX}/share/icons/hicolor/16x16/apps
    icon20.files = res/hicolor/20x20/apps/cyan.png
    icon20.path = $${PREFIX}/share/icons/hicolor/20x20/apps
    icon22.files = res/hicolor/22x22/apps/cyan.png
    icon22.path = $${PREFIX}/share/icons/hicolor/22x22/apps
    icon24.files = res/hicolor/24x24/apps/cyan.png
    icon24.path = $${PREFIX}/share/icons/hicolor/24x24/apps
    icon32.files = res/hicolor/32x32/apps/cyan.png
    icon32.path = $${PREFIX}/share/icons/hicolor/32x32/apps
    icon48.files = res/hicolor/48x48/apps/cyan.png
    icon48.path = $${PREFIX}/share/icons/hicolor/48x48/apps
    icon64.files = res/hicolor/64x64/apps/cyan.png
    icon64.path = $${PREFIX}/share/icons/hicolor/64x64/apps
    icon72.files = res/hicolor/72x72/apps/cyan.png
    icon72.path = $${PREFIX}/share/icons/hicolor/72x72/apps
    icon96.files = res/hicolor/96x96/apps/cyan.png
    icon96.path = $${PREFIX}/share/icons/hicolor/96x96/apps
    icon128.files = res/hicolor/128x128/apps/cyan.png
    icon128.path = $${PREFIX}/share/icons/hicolor/128x128/apps
    icon160.files = res/hicolor/160x160/apps/cyan.png
    icon160.path = $${PREFIX}/share/icons/hicolor/160x160/apps
    icon192.files = res/hicolor/192x192/apps/cyan.png
    icon192.path = $${PREFIX}/share/icons/hicolor/192x192/apps
    icon256.files = res/hicolor/256x256/apps/cyan.png
    icon256.path = $${PREFIX}/share/icons/hicolor/256x256/apps
    icon512.files = res/hicolor/512x512/apps/cyan.png
    icon512.path = $${PREFIX}/share/icons/hicolor/512x512/apps

    INSTALLS += icon16 icon20 icon22 \
                icon24 icon32 icon48 \
                icon64 icon72 icon96 \
                icon128 icon160 icon192 \
                icon256 icon512
}
mac {
    ICON = res/Cyan.icns
    QMAKE_INFO_PLIST = res/Info.plist
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

win32 {
    RC_ICONS += res/cyan.ico
    LIBS += -lpthread
}
