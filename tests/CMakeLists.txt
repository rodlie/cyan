cmake_minimum_required(VERSION 3.1)

project(tests)
enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES tst_cyan.cpp ../fxx/FXX.cxx ../res/tests.qrc)
set(HEADERS ../fxx/FXX.h)
set(RESOURCE_FILES ../res/tests.qrc)

find_package(Qt5Test REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/../fxx/cmake/modules/")
include_directories(../fxx)
include(CommonSetup)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES_FILES})
target_link_libraries(${PROJECT_NAME} Qt5::Test)

include(CommonLink)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

