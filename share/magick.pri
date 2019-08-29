#
# Cyan <https://cyan.fxarena.net>
# Copyright Ole-André Rodlie, FxArena DA.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#

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
