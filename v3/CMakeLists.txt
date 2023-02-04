#
# Cyan Graphics - https://cyan.graphics - https://github.com/rodlie/cyan
#
# Copyright (c) Ole-André Rodlie <ole.andre.rodlie@gmail.com>. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#

cmake_minimum_required(VERSION 3.9)
enable_testing()
project(Cyan VERSION 3.0.0 LANGUAGES CXX)

if(NOT PROJECT_VERSION_TWEAK)
    set(PROJECT_VERSION_TWEAK 0)
endif()

add_definitions(-DCYAN_VERSION="${PROJECT_VERSION}")

add_compile_options(-Wall -Wextra)
if(CMAKE_BUILD_TYPE MATCHES "^(release|Release|RELEASE)$")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
else()
    add_definitions(-DQT_MESSAGELOGCONTEXT)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(MAGICK_PKG_CONFIG "Magick++" CACHE STRING "Magick++ pkg-config")
option(STATIC_PKGCONF "Use pkg-config static" OFF)

set(COMPANY "Cyan")
set(COPYRIGHT "Copyright Ole-Andre Rodlie")
set(IDENTIFIER "graphics.cyan")

if(APPLE)
    set(ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/share/icons/${PROJECT_NAME}.icns)
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    set(MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${IDENTIFIER})
    set(MACOSX_BUNDLE_ICON_FILE ${PROJECT_NAME})
    set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")
endif()


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.9 COMPONENTS Concurrent Widgets Test REQUIRED)
find_package(PkgConfig)

pkg_search_module(MAGICK REQUIRED ${MAGICK_PKG_CONFIG})
pkg_search_module(LCMS2 REQUIRED lcms2)

if(${STATIC_PKGCONF})
    add_compile_options(${MAGICK_STATIC_CFLAGS})
    find_package(OpenMP)
else()
    add_compile_options(${MAGICK_CFLAGS})
endif()

include_directories(${MAGICK_INCLUDE_DIRS} ${LCMS2_INCLUDE_DIRS})

set(PROJECT_SOURCES
    main.cpp
    engine.cpp
    engine.h
    basicview.cpp
    basicview.h
    mdi.cpp
    mdi.h
    mdisubwindow.cpp
    mdisubwindow.h
    cyan.cpp
    cyan.h
    qtwindowlistmenu.cpp
    qtwindowlistmenu.h
    share/icons.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/../res/icc.qrc)
set(TEST_SOURCES
    tests.cpp
    engine.cpp
    engine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../res/tests.qrc)

add_executable(${PROJECT_NAME} ${ICON_FILE} ${PROJECT_SOURCES})
add_executable(tests ${TEST_SOURCES})

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE TRUE)
endif()

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      Qt${QT_VERSION_MAJOR}::Concurrent
                      Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(tests
                      PRIVATE
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Test)

if(${STATIC_PKGCONF})
    target_link_libraries(${PROJECT_NAME}
                          PRIVATE
                          ${MAGICK_STATIC_LIBRARIES}
                          ${LCMS2_STATIC_LIBRARIES}
                          ${MAGICK_STATIC_LDFLAGS}
                          ${LCMS2_STATIC_LDFLAGS})
    if(OpenMP_CXX_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
    endif()
    if(APPLE)
        target_link_libraries(${PROJECT_NAME} PRIVATE iconv)
    endif()
    target_link_libraries(tests
                          PRIVATE
                          ${MAGICK_STATIC_LIBRARIES}
                          ${LCMS2_STATIC_LIBRARIES}
                          ${MAGICK_STATIC_LDFLAGS}
                          ${LCMS2_STATIC_LDFLAGS})
    if(OpenMP_CXX_FOUND)
        target_link_libraries(tests PRIVATE OpenMP::OpenMP_CXX)
    endif()
    if(APPLE)
        target_link_libraries(tests PRIVATE iconv)
    endif()
else()
    target_link_libraries(${PROJECT_NAME}
                          PRIVATE
                          ${MAGICK_LIBRARIES}
                          ${LCMS2_LIBRARIES}
                          ${MAGICK_LDFLAGS}
                          ${LCMS2_LDFLAGS})
    target_link_libraries(tests
                          PRIVATE
                          ${MAGICK_LIBRARIES}
                          ${LCMS2_LIBRARIES}
                          ${MAGICK_LDFLAGS}
                          ${LCMS2_LDFLAGS})
endif()

add_test(NAME tests COMMAND tests)
