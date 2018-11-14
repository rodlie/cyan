QT += testlib
QT -= gui
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
TEMPLATE = app
SOURCES +=  tst_cyan.cpp ../src/FXX.cxx
HEADERS += ../src/FXX.h
RESOURCES += ../res/tests.qrc
INCLUDEPATH += ../src
DESTDIR = build

include(../cyan.pri)

unix:QMAKE_POST_LINK = ./build/$${TARGET}
win32-g++:QMAKE_POST_LINK = "wine64 build/$${TARGET}.exe"
