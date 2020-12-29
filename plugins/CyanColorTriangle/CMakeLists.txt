#
# Cyan <https://cyan.fxarena.net>
# Copyright Ole-André Rodlie, FxArena DA.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#

cmake_minimum_required(VERSION 3.0)

project(CyanColorTriangle VERSION 1.0)
set(UUID "net.fxarena.cyan.plugin.widget.colorTriangle")
set(TITLE "Cyan Color Triangle")
set(DESC "A color triangle.")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SOURCES ${PROJECT_NAME}.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/qtcolortriangle.cpp)
set(HEADERS ${PROJECT_NAME}.h ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/qtcolortriangle.h)

add_definitions(-DCYAN_PLUGIN_UUID="${UUID}")
add_definitions(-DCYAN_PLUGIN_TITLE="${TITLE}")
add_definitions(-DCYAN_PLUGIN_DESC="${DESC}")
add_definitions(-DCYAN_PLUGIN_VERSION="${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

if(!WIN32)
    add_compile_options(-Wall -Wextra)
endif()
if(CMAKE_BUILD_TYPE MATCHES "^(release|Release|RELEASE)$")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

find_package(Qt5Widgets REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lib)

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
    else()
        set(LIB_INSTALL_DIR ${CMAKE_INSTALL_FULL_LIBDIR})
    endif()
    install(TARGETS ${PROJECT_NAME} DESTINATION ${LIB_INSTALL_DIR}/Cyan)
endif()
