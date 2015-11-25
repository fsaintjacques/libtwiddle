cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME libtwiddle)
set(RELEASE_DATE 2015-09-03)
project(${PROJECT_NAME})
include(CTest)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")

find_package(ParseArguments)
find_package(Prereqs)
find_package(CTargets)
find_package(Version)
find_package(Options)

#-----------------------------------------------------------------------
# Check for prerequisite libraries

#pkgconfig_prereq(libcork>=0.14.0)
# HACK: forces travis to apply '-lm' linking.
library_prereq(m)
library_prereq(rt)

#-----------------------------------------------------------------------
# Include our subdirectories

add_subdirectory(docs)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(tests)
