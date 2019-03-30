QT += widgets

TARGET = CyanQtSolutions
VERSION = 1.0.0
TEMPLATE = lib
CONFIG -= static

#DEFINES += QT_QTCOLORPICKER_EXPORT
#DEFINES += QT_QTCOLORTRIANGLE_EXPORT
#DEFINES += QT_QTWINDOWLISTMENU_EXPORT

SOURCES += qtcolorpicker.cpp qtcolortriangle.cpp qtwindowlistmenu.cpp
HEADERS += qtcolorpicker.h qtcolortriangle.h qtwindowlistmenu.h

include($${top_srcdir}/share/common.pri)

unix:!macx {
    DESTDIR = $${top_builddir}/build
    OBJECTS_DIR = $${DESTDIR}/.obj_solutions
    MOC_DIR = $${DESTDIR}/.moc_solutions
    RCC_DIR = $${DESTDIR}/.qrc_solutions

    target.path = $${LIBDIR}
    docs.path = $${DOCDIR}/CyanQtSolutions-$${VERSION}
    docs.files += COPYING LGPL_EXCEPTION.txt README.md

    INSTALLS += target docs
}
