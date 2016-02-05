cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME libtwiddle)
set(RELEASE_DATE 2015-09-03)
project(${PROJECT_NAME})
include(CTest)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")

option(USE_AVX "Enable AVX instructions" ON)
option(USE_AVX2 "Enable AVX2 instructions" OFF)
option(USE_AVX512 "Enable AVX512 instructions" OFF)

find_package(ParseArguments)
find_package(Prereqs)
find_package(CTargets)
find_package(Version)
find_package(Options)

#-----------------------------------------------------------------------
# Check for prerequisite libraries

pkgconfig_prereq(check OPTIONAL)

#-----------------------------------------------------------------------
# Include our subdirectories

add_subdirectory(docs)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(tests)
