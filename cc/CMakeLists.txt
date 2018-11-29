cmake_minimum_required(VERSION 3.1)

project(Cyan VERSION 1.2.0)

if(NOT PROJECT_VERSION_PATCH)
    set(PROJECT_VERSION_PATCH 0)
endif()
if(NOT PROJECT_VERSION_TWEAK)
    set(PROJECT_VERSION_TWEAK 0)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_definitions(-DCYAN_VERSION="${PROJECT_VERSION}")
add_definitions(-DCYAN_GIT="$ENV{GIT}")

set(SOURCES main.cpp cyan.cpp imageview.cpp profiledialog.cpp helpdialog.cpp ../fxx/FXX.cxx ../res/cyan.qrc ../docs/docs.qrc)
set(HEADERS cyan.h imageview.cpp profiledialog.cpp helpdialog.cpp ../fxx/FXX.h)
set(RESOURCE_FILES ../res/cyan.qrc ../docs/docs.qrc)
set(RESOURCE_FOLDER ../res)

set(COMPANY "Cyan")
set(COPYRIGHT "(c)2016-2018 Ole-Andre Rodlie. All rights reserved.")
set(IDENTIFIER "net.fxarena.cyan")

if(APPLE)
    set(ICON_FILE ${RESOURCE_FOLDER}/${PROJECT_NAME}.icns)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.7")
    set_source_files_properties(${PROJECT_NAME}.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    set(MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${IDENTIFIER})
    set(MACOSX_BUNDLE_ICON_FILE Cyan)
endif()

if(WIN32)
    set(ICON_FILE ${CC_RESOURCE_FOLDER}/cyan.ico)
endif()

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Concurrent REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../fxx/cmake/modules/")
include_directories(../fxx)
include(CommonSetup)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES_FILES})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Concurrent)
include(CommonLink)

if(UNIX)
    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
    #configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
    #nstall(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

