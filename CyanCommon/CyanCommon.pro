
QT += widgets
TARGET = CyanCommon
TEMPLATE = lib

SOURCES += \
        common.cpp \
        tileitem.cpp \
        render.cpp

HEADERS += \
        common.h \
        tileitem.h \
        render.h

include(../cyan.pri)
