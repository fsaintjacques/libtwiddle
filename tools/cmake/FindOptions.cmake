#-----------------------------------------------------------------------
# Set some options

# set C standard
if (CMAKE_VERSION VERSION_LESS "3.1")
  if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
  endif ()
else ()
  set (CMAKE_C_STANDARD 99)
endif ()


set(CMAKE_INSTALL_LIBDIR lib CACHE STRING
      "The base name of the installation directory for libraries")

# default to Release type if not provided
# compile to -03 with no assert
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif(NOT CMAKE_BUILD_TYPE)


if(USE_AVX512)
  add_definitions(-DUSE_AVX512=1 -mavx512f)
  add_definitions(-DUSE_AVX2=1 -mavx2)
  add_definitions(-DUSE_AVX=1 -mavx)
elseif(USE_AVX2)
  add_definitions(-DUSE_AVX2=1 -mavx2)
  add_definitions(-DUSE_AVX=1 -mavx)
elseif(USE_AVX)
  add_definitions(-DUSE_AVX=1 -mavx)
else()
  add_definitions(-DUSE_PORTABLE=1)
endif()

# strict compile options
add_definitions(-Wall -Werror -pedantic -Wextra -Wshadow -Wfloat-equal -Wundef -Wwrite-strings -Wunreachable-code -Wformat=2 -Wswitch-enum -Wswitch-default -Winit-self)
