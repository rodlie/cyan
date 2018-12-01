# Copyright or © or Copr. Ole-André Rodlie, INRIA.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.

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

set(SOURCES main.cpp cyan.cpp imageview.cpp profiledialog.cpp helpdialog.cpp ../lib/FXX.cxx ../res/cyan.qrc ../docs/docs.qrc)
set(HEADERS cyan.h imageview.cpp profiledialog.cpp helpdialog.cpp ../lib/FXX.h)
set(RESOURCE_FILES ../res/cyan.qrc ../docs/docs.qrc)
set(RESOURCE_FOLDER ../res)

set(COMPANY "Cyan")
set(COPYRIGHT "Copyright Ole-Andre Rodlie, INRIA. All rights reserved.")
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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../lib/cmake/modules/")
include_directories(../lib)
include(CommonSetup)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES_FILES})

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Concurrent)
include(CommonLink)

if(UNIX)
    include(GNUInstallDirs)
    install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
    install(DIRECTORY ../res/hicolor DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons)
    install(FILES ../res/cyan.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
    install(FILES ../COPYING ../docs/README.md DESTINATION ${CMAKE_INSTALL_DOCDIR}-${PROJECT_VERSION})
endif()

