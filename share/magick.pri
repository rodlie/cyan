#
# Copyright Ole-André Rodlie, FxArena DA.
#
# ole.andre.rodlie@gmail.com / support@fxarena.net
#
# This software is governed either by the CeCILL or the CeCILL-C license
# under French law and abiding by the rules of distribution of free software.
# You can  use, modify and or redistribute the software under the terms of
# the CeCILL or CeCILL-C licenses as circulated by CEA, CNRS and INRIA
# at the following URL: "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL and CeCILL-C licenses and that you accept its terms.
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
