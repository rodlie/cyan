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

project(tests)
enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES tst_cyan.cpp ../lib/FXX.cxx ../res/tests.qrc)
set(HEADERS ../lib/FXX.h)
set(RESOURCE_FILES ../res/tests.qrc)

find_package(Qt5Test REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../fxx/cmake/modules/")
include_directories(../lib)
include(CommonSetup)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES_FILES})
target_link_libraries(${PROJECT_NAME} Qt5::Test)

include(CommonLink)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

