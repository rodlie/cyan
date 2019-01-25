QT += widgets

DESTDIR = $${top_builddir}/build
OBJECTS_DIR = $${DESTDIR}/.obj_$${TARGET}
MOC_DIR = $${DESTDIR}/.moc_$${TARGET}
RCC_DIR = $${DESTDIR}/.qrc_$${TARGET}

# pkg-config
QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig

# lcms
PKGCONFIG += lcms2

# ImageMagick7
MAGICK_CONFIG = Magick++-7.Q16HDRI
!isEmpty(MAGICK): MAGICK_CONFIG = $${MAGICK}
PKGCONFIG += $${MAGICK_CONFIG}
CONFIG(deploy): LIBS += `pkg-config --libs --static $${MAGICK_CONFIG}`

# ffmpeg
CONFIG(with_ffmpeg) {
    DEFINES += WITH_FFMPEG
    PKGCONFIG += libavdevice \
                 libswscale \
                 libavformat \
                 libavcodec \
                 libavutil
}
