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


QT += testlib
QT -= gui
CONFIG += qt console warn_on depend_includepath testcase no_testcase_installs
CONFIG -= app_bundle
TEMPLATE = app
SOURCES +=  src/tst_cyan.cpp src/FXX.cpp
HEADERS += src/FXX.h
RESOURCES += res/tests.qrc
DESTDIR = build
OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.qrc

unix:QMAKE_POST_LINK = ./build/$${TARGET}
win32-g++:QMAKE_POST_LINK = "wine64 build/$${TARGET}.exe"

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

mac {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

win32-g++: LIBS += -lpthread
