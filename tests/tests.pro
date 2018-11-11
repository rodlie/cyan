QT += testlib
QT -= gui
CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle
TEMPLATE = app
SOURCES +=  tst_cyan.cpp

include(../cyan.pri)

unix:QMAKE_POST_LINK = ./$${TARGET}
#win32 {
#    CONFIG(debug, debug|release) {
#        QMAKE_POST_LINK = "wine debug/$${TARGET}.exe"
#    }
#    else {
#        QMAKE_POST_LINK = "wine release/$${TARGET}.exe"
#    }
#}
