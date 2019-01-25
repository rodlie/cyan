include($${top_srcdir}/cyan.pri)

TARGET = CyanCanvas
TEMPLATE = lib

SOURCES += \
        cyan_layeritem.cpp \
        cyan_view.cpp

HEADERS += \
        cyan_layeritem.h \
        cyan_view.h
INCLUDEPATH += $${top_srcdir}/common
LIBS += -L$${DESTDIR} -lCyanCommon

