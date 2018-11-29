QT += testlib
QT -= gui
CONFIG += qt console warn_on depend_includepath testcase no_testcase_installs
CONFIG -= app_bundle
TEMPLATE = app
SOURCES +=  tst_cyan.cpp ../fxx/FXX.cxx
HEADERS += ../fxx/FXX.h
RESOURCES += ../res/tests.qrc
INCLUDEPATH += ../fxx
DESTDIR = build
OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.qrc

include(../cyan.pri)

unix:QMAKE_POST_LINK = ./build/$${TARGET}
win32-g++:QMAKE_POST_LINK = "wine64 build/$${TARGET}.exe"
