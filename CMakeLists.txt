cmake_minimum_required(VERSION 3.1)

enable_testing()

option(BUILD_CC "Build Cyan Color Converter" ON)
option(BUILD_TST "Build Cyan Unit Tests" ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/fxx/cmake/modules/")

if(BUILD_CC)
add_subdirectory(cc)
endif()

if(BUILD_TST)
add_subdirectory(tests)
endif()
