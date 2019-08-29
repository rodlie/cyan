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

# custom magick features
CONFIG(custom_magick): DEFINES += CUSTOM_MAGICK

# Use ImageMagick-Windows on msvc
win32-msvc {
    # path to your "VisualMagick" build
    # follow the instructions on https://github.com/ImageMagick/ImageMagick-Windows to build it
    # DO NOT USE PRECOMPILED BINARIES FROM IMAGEMAGICK, ENABLE ZERO-CONF IN CONFIGURE.EXE !!!
    isEmpty(MAGICK_WINDOWS_PATH) : MAGICK_WINDOWS_PATH = $${top_srcdir}/ImageMagick-Windows

    INCLUDEPATH += \
        $${MAGICK_WINDOWS_PATH}/ImageMagick/Magick++/lib \
        $${MAGICK_WINDOWS_PATH}/ImageMagick \
        $${MAGICK_WINDOWS_PATH}/lcms/include

    LIBS += \
        -L$${MAGICK_WINDOWS_PATH}/VisualMagick/lib \
        -L$${MAGICK_WINDOWS_PATH}/VisualMagick/bin

    CONFIG(release, debug|release) {
        LIBS += \
            -lCORE_RL_lcms_ \
            -lCORE_RL_MagickCore_ \
            -lCORE_RL_MagickWand_ \
            -lCORE_RL_Magick++_
    }

    CONFIG(debug, debug|release) {
        LIBS += \
            -lCORE_DB_lcms_ \
            -lCORE_DB_MagickCore_ \
            -lCORE_DB_MagickWand_ \
            -lCORE_DB_Magick++_
    }
}

# Use pkg-config on anything else
!win32-msvc {
    QT_CONFIG -= no-pkg-config
    CONFIG += link_pkgconfig

    # optional pkg-config name for Magick++, default is Magick++-7.Q16HDRI
    isEmpty(MAGICK) : MAGICK = Magick++-7.Q16HDRI
    PKGCONFIG += $${MAGICK} lcms2

    # get static 3rdparty depends
    LIBS += `pkg-config --libs --static $${MAGICK}`

    # fontconfig is needed on mingw
    win32-g++ : PKGCONFIG += fontconfig
}
