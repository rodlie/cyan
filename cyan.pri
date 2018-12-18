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

DESTDIR = build
OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.qrc

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
    CONFIG += staticlib
}

isEmpty(PREFIX): PREFIX = /usr/local
isEmpty(DOCDIR): DOCDIR = $$PREFIX/share/doc
isEmpty(MANDIR): MANDIR = $$PREFIX/share/man
isEmpty(LIBDIR): LIBDIR = $$PREFIX/lib

QMAKE_TARGET_COMPANY = "$${TARGET}"
QMAKE_TARGET_PRODUCT = "$${TARGET}"
QMAKE_TARGET_DESCRIPTION = "$${TARGET}"
QMAKE_TARGET_COPYRIGHT = "Copyright Ole-Andre Rodlie"

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += lcms2

!exists(3rdparty/ImageMagick6/configure): warning("Using unsupported Magick++, this may not work")
exists(3rdparty/ImageMagick6/configure) {
    PKGCONFIG -= $${MAGICK_CONFIG}
    CWD = 3rdparty/ImageMagick6
    MAGICK_PC_PATH = $${CWD}/install/lib/pkgconfig
    MAGICK_PC = $${MAGICK_PC_PATH}/ImageMagick++-6.Q16HDRI.pc
    MAGICK_PC_CONFIG=ImageMagick++-6.Q16HDRI
    !exists($${MAGICK_PC}) {
        message("Configure Magick++ ...")
        MAGICK_CONFIGURE = $$system("cd $${CWD}; ./configure CXXFLAGS=-fPIC CFLAGS=-fPIC CPPFLAGS=-fPIC --prefix=`pwd`/install" \
                                    " --disable-openmp --disable-opencl" \
                                    " --enable-largefile --disable-shared --enable-static" \
                                    " --enable-cipher --enable-zero-configuration" \
                                    " --enable-hdri --disable-pipes --disable-docs" \
                                    " --without-modules --with-magick-plus-plus" \
                                    " --with-package-release-name=Cyan" \
                                    " --with-bzlib --with-lzma --with-zlib" \
                                    " --with-jpeg --with-png --with-tiff" \
                                    " --with-lcms --with-quantum-depth=16" \
                                    " --with-x=no" \
                                    " --without-zstd --without-autotrace --without-dps" \
                                    " --without-fftw --without-flif --without-fpx" \
                                    " --without-djvu --without-fontconfig --without-freetype" \
                                    " --without-raqm --without-gslib --without-gvc" \
                                    " --without-heic --without-jbig --with-openjp2" \
                                    " --without-lqr --without-openexr --without-pango" \
                                    " --without-raw --without-rsvg --without-webp" \
                                    " --without-wmf --without-xml")
        #message($${MAGICK_CONFIGURE})
        message("Building Magick++ ...")
        MAGICK_MAKE = $$system("cd $${CWD}; make -j2")
        #message($${MAGICK_MAKE})
        MAGICK_INSTALL = $$system("cd $${CWD}; make install")
        #message($${MAGICK_INSTALL})
    }
    QMAKE_CXXFLAGS += $$system("PKG_CONFIG_PATH=$${MAGICK_PC_PATH} pkg-config" \
                               " --cflags  --static $${MAGICK_PC_CONFIG}")
    LIBS += $$system("PKG_CONFIG_PATH=$${MAGICK_PC_PATH} pkg-config" \
                     " --libs --static $${MAGICK_PC_CONFIG}")
}
