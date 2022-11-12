#
# Cyan - https://github.com/rodlie/cyan
#
# Copyright (c) 2020-2022 Ole-André Rodlie. All rights reserved.
# Copyright (c) 2018-2019 Ole-André Rodlie, FxArena. All rights reserved.
# Copyright (c) 2016-2017 Ole-André Rodlie, INRIA. All rights reserved.
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

cmake_minimum_required(VERSION 3.5)
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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Concurrent Widgets Test REQUIRED)
find_package(PkgConfig)

pkg_search_module(MAGICK REQUIRED ${MAGICK_PKG_CONFIG})
pkg_search_module(LCMS2 REQUIRED lcms2)

add_compile_options(${MAGICK_CFLAGS})
include_directories(${MAGICK_INCLUDE_DIRS} ${LCMS2_INCLUDE_DIRS})

set(PROJECT_SOURCES
    main.cpp
    engine.cpp
    engine.h
    basicview.cpp
    mdi.cpp
    mdisubwindow.cpp
    cyan.cpp
    cyan.h
    qtwindowlistmenu.cpp
    qtwindowlistmenu.h)
set(TEST_SOURCES
    test.cpp
    engine.cpp
    engine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../res/tests.qrc)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
add_executable(tests ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME}
                      PRIVATE
                      Qt${QT_VERSION_MAJOR}::Concurrent
                      Qt${QT_VERSION_MAJOR}::Widgets
                      ${MAGICK_LIBRARIES}
                      ${LCMS2_LIBRARIES}
                      ${MAGICK_LDFLAGS}
                      ${LCMS2_LDFLAGS})
target_link_libraries(tests
                      PRIVATE
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Test
                      ${MAGICK_LIBRARIES}
                      ${LCMS2_LIBRARIES}
                      ${MAGICK_LDFLAGS}
                      ${LCMS2_LDFLAGS})

add_test(NAME tests COMMAND tests)
