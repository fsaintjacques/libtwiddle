#-----------------------------------------------------------------------
# Retrieve the current version number

execute_process(
    COMMAND git describe
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE VERSION_RESULT
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(VERSION_RESULT)
    set(VERSION "0.0.0")
    message(STATUS "Cannot determine version number reverting to: " ${VERSION})
endif(VERSION_RESULT)
message(STATUS "Current version: " ${VERSION})

if(VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-.*)?$")
    set(VERSION_MAJOR "${CMAKE_MATCH_1}")
    set(VERSION_MINOR "${CMAKE_MATCH_2}")
    set(VERSION_PATCH "${CMAKE_MATCH_3}")
    set(VERSION_COMMIT "${CMAKE_MATCH_4}")
else(VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-.*)?$")
    message(FATAL_ERROR "Invalid version number: ${VERSION}")
endif(VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-.*)?$")

execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE GIT_SHA1_RESULT
    OUTPUT_VARIABLE GIT_SHA1
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(GIT_SHA1_RESULT)
    message(FATAL_ERROR
            "Cannot determine git commit: " ${GIT_SHA1_RESULT})
endif(GIT_SHA1_RESULT)
message(STATUS "Current revision: " ${GIT_SHA1})
