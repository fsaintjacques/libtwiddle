# Credits goes to nkurz see
# https://github.com/RoaringBitmap/CRoaring/blob/master/tools/cmake/FindLTO.cmake

macro(append var string)
  set(${var} "${${var}} ${string}")
endmacro(append)

if(USE_LTO)
    if ("${CMAKE_C_COMPILER_ID}" MATCHES "Intel")
        append(CMAKE_C_FLAGS "-ipo")
        append(CMAKE_CXX_FLAGS "-ipo")
    elseif ("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
        append(CMAKE_C_FLAGS "-flto")
        append(CMAKE_CXX_FLAGS "-flto")
        append(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=gold -flto")
        append(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=gold -flto")
        if(NOT ("${CMAKE_AR}" MATCHES "llvm-ar-*"))
            message(FATAL_ERROR
              "You requested clang compiler & LTO without providing CMAKE_AR. "
              "CMAKE_AR is currently set to ${CMAKE_AR}. "
              "To fix this, invoke cmake with `-DCMAKE_AR=$(which llvm-ar-$version)`.")
        endif()
        if(NOT ("${CMAKE_RANLIB}" MATCHES "llvm-ranlib-*"))
            message(FATAL_ERROR
              "You requested clang compiler & LTO without providing CMAKE_RANLIB. "
              "CMAKE_RANLIB is currently set to ${CMAKE_RANLIB}. "
              "To fix this, invoke cmake `-DCMAKE_RANLIB=$(which llvm-ranlib-$version).`")
        endif()
    else()
        append(CMAKE_C_FLAGS "-flto")
        append(CMAKE_CXX_FLAGS "-flto")
        set(CMAKE_AR "gcc-ar")
        set(CMAKE_RANLIB "gcc-ranlib")
    endif()
endif(USE_LTO)
