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

include($${top_srcdir}/cyan.pri)

TARGET = Cyan
TEMPLATE = app
QT += concurrent

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
    common/mdi.cpp \
    common/colorconvert.cpp \
    colors/qtcolorpicker.cpp \
    colors/qtcolortriangle.cpp \
    colors/colorrgb.cpp \
    colors/colorcmyk.cpp \
    colors/colorhsv.cpp \
    dialog/newmediadialog.cpp \
    dialog/convertdialog.cpp
CONFIG(with_ffmpeg): SOURCES += dialog/videodialog.cpp

HEADERS += \
    app/editor.h \
    common/mdi.h \
    common/colorconvert.h \
    colors/qtcolorpicker.h \
    colors/qtcolortriangle.h \
    colors/colorrgb.h \
    colors/colorcmyk.h \
    colors/colorhsv.h \
    dialog/newmediadialog.h \
    dialog/convertdialog.h
CONFIG(with_ffmpeg): HEADERS += dialog/videodialog.h

CONFIG(deploy) : RESOURCES += share/icons.qrc share/icc.qrc
CONFIG(debug, release|debug): RESOURCES += share/icons.qrc

OTHER_FILES += \
    $${top_srcdir}/scripts/ci.sh \
    $${top_srcdir}/scripts/gimp.py \
    $${top_srcdir}/docs/README.md

INCLUDEPATH += \
    app \
    canvas \
    common \
    colors \
    dialog \
    layers

DEFINES += CYAN_VERSION=\"\\\"$${VERSION}$${VERSION_TYPE}\\\"\"
DEFINES += CYAN_GIT=\"\\\"$${GIT}\\\"\"
!CONFIG(deploy): DEFINES += CYAN_DEVEL

unix:!mac {
    target.path = $${BINDIR}
    docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}$${VERSION_TYPE}
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
    docs.files = \
        $${top_srcdir}/README.md \
        $${top_srcdir}/docs/LICENSE.CeCILLv21 \
        $${top_srcdir}/docs/LICENSE.txt \
        $${top_srcdir}/docs/LICENSE-FatCow.txt \
        $${top_srcdir}/docs/LICENSE-Adwaita.txt \
        $${top_srcdir}/docs/LICENSE-ICC.txt \
        $${top_srcdir}/docs/LGPL_EXCEPTION.txt \
        $${top_srcdir}/docs/LICENSE.LGPLv21

    INSTALLS += \
        target \
        docs \
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

# add libraries
INCLUDEPATH += \
    $${top_srcdir}/common \
    $${top_srcdir}/canvas \
    $${top_srcdir}/layers

LIBS += \
    -L$${DESTDIR} \
    -lCyanCommon \
    -lCyanCanvas \
    -lCyanLayers

unix:!mac: QMAKE_RPATHDIR += $ORIGIN/../lib$${LIBSUFFIX}

CONFIG(staticlib): PKGCONFIG += lcms2
