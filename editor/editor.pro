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

# include common
include($${top_srcdir}/share/cyan.pri)

# project requires ImageMagick
include($${top_srcdir}/share/magick.pri)

TARGET = Cyan
TEMPLATE = app
QT += concurrent

SOURCES += \
    main.cpp \
    editor.cpp \
    about.cpp \
    setup.cpp \
    color.cpp \
    viewtool.cpp \
    messages.cpp \
    layers.cpp \
    tabs.cpp \
    mdi.cpp \

HEADERS += \
    editor.h \
    mdi.h \

# bundle core icons
RESOURCES += $${top_srcdir}/share/icons_core.qrc

# on deploy bundle icon theme and color profiles
CONFIG(deploy) : RESOURCES += $${top_srcdir}/share/icons_theme.qrc $${top_srcdir}/share/icc.qrc

# on debug bundle icon theme
CONFIG(debug, release|debug): RESOURCES += $${top_srcdir}/share/icons_theme.qrc

OTHER_FILES += \
    $${top_srcdir}/scripts/ci.sh \
    $${top_srcdir}/scripts/gimp.py \
    $${top_srcdir}/docs/README.md

DEFINES += CYAN_VERSION=\"\\\"$${VERSION}$${VERSION_TYPE}\\\"\"
DEFINES += CYAN_GIT=\"\\\"$${GIT}\\\"\"
!CONFIG(deploy): DEFINES += CYAN_DEVEL

# install on unix (not mac)
unix:!mac {
    target.path = $${BINDIR}
    docs.path = $${DOCDIR}/$${TARGET}-$${VERSION}$${VERSION_TYPE}
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

# mac stuff
mac {
    CONFIG(deploy) {
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
        QMAKE_CXXFLAGS += -fopenmp
        QMAKE_LFLAGS += -fopenmp
    }
    ICON = $${top_srcdir}/share/icons/Cyan.icns
    QMAKE_INFO_PLIST = $${top_srcdir}/share/Info.plist
}

# deploy fix
CONFIG(deploy): win32-g++: LIBS += -lpthread

# add win32 icon
win32: RC_ICONS += $${top_srcdir}/share/icons/cyan.ico

# add libraries
INCLUDEPATH += \
    $${top_srcdir}/common \
    $${top_srcdir}/layers \
    $${top_srcdir}/colors \
    $${top_srcdir}/dialogs

LIBS += \
    -L$${DESTDIR} \
    -lCyanCommon \
    -lCyanLayers \
    -lCyanColors \
    -lCyanDialogs

# avoid linking against system libraries
!CONFIG(staticlib): unix:!mac: QMAKE_RPATHDIR += $ORIGIN/../lib$${LIBSUFFIX}

# link against lcms2 if static
CONFIG(staticlib): PKGCONFIG += lcms2
