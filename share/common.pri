# Cyan
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

CONFIG += c++11
CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

# build dir
!win32-msvc {
    DESTDIR = $${top_builddir}/build
    OBJECTS_DIR = $${DESTDIR}/.obj_$${TARGET}
    MOC_DIR = $${DESTDIR}/.moc_$${TARGET}
    RCC_DIR = $${DESTDIR}/.qrc_$${TARGET}
}

# mac
mac {
    CONFIG(deploy) {
        # win/lin/mac use the same version of Qt (and depends) to avoid "issues",
        # since Qt 5.9 is the lowest we can go on Windows due to misc bugs
        # we also need to ship Qt 5.9 on Mac, this means OSX 10.10 is the
        # lowest version we can target.
        # You can however build against Qt 5.6 to get compatibility with OSX 10.7.
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
    }
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
}
