cmake_minimum_required(VERSION 2.8.12.2)
#
ADD_DEFINITIONS(-DLINUX)
#
INCLUDE_DIRECTORIES(../libmisc-cpp)
INCLUDE_DIRECTORIES($ENV{LIB_CURL}/include)
#
ADD_COMPILE_OPTIONS(-std=c++17 -fPIC -Wall)
ADD_DEFINITIONS(-Wno-builtin-macro-redefined)
SET(CMAKE_CXX_FLAGS $ENV{CXX_FLAGS})
#
FILE(GLOB SRC0 "./*.cpp")
FILE(GLOB SRC1 "./*/*.cpp")
FILE(GLOB SRC2 "./*/*/*.cpp")
#
PROJECT(misc-influxdb-c)
#
ADD_LIBRARY(misc-influxdb-c STATIC ${SRC0} ${SRC1} ${SRC2})
INCLUDE(./__file__.cmake)
REDEFINE_FILE_MACRO(misc-influxdb-c)

