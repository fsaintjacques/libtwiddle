#-----------------------------------------------------------------------
# Set some options

# set C standard
if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
  endif ()
else ()
  set (CMAKE_C_STANDARD 99)
endif ()

if(APPLE)
    if (NOT CMAKE_INSTALL_NAME_DIR)
        set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
    endif (NOT CMAKE_INSTALL_NAME_DIR)
endif(APPLE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_INSTALL_LIBDIR lib CACHE STRING
    "The base name of the installation directory for libraries")

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_definitions(-Wall -Werror -pedantic)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -Werror -pedantic)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Intel")
    add_definitions(-Wall -Werror)
endif(CMAKE_C_COMPILER_ID STREQUAL "GNU")

