#
# Cyan - https://github.com/rodlie/cyan
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

if(CMAKE_BUILD_TYPE MATCHES "^(release|Release|RELEASE)$")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(MAGICK_PKG_CONFIG "Magick++" CACHE STRING "Magick++ pkg-config")
option(STATIC_PKGCONF "Use pkg-config static" OFF)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.9 COMPONENTS Concurrent Widgets Test REQUIRED)
find_package(PkgConfig)

pkg_search_module(MAGICK REQUIRED ${MAGICK_PKG_CONFIG})
pkg_search_module(LCMS2 REQUIRED lcms2)

if(${STATIC_PKGCONF})
    add_compile_options(${MAGICK_STATIC_CFLAGS})
    find_package(OpenMP REQUIRED)
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

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
add_executable(tests ${TEST_SOURCES})

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
                          ${LCMS2_STATIC_LDFLAGS}
                          OpenMP::OpenMP_CXX)
    if(APPLE)
        target_link_libraries(${PROJECT_NAME} PRIVATE iconv)
    endif()
    target_link_libraries(tests
                          PRIVATE
                          ${MAGICK_STATIC_LIBRARIES}
                          ${LCMS2_STATIC_LIBRARIES}
                          ${MAGICK_STATIC_LDFLAGS}
                          ${LCMS2_STATIC_LDFLAGS}
                          OpenMP::OpenMP_CXX)
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
