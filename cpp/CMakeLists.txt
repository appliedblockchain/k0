cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0076 NEW)

project(zktrade)

add_definitions(-DZKTRADE_DUMMYHASH)

set(
        CURVE
        "ALT_BN128"
        CACHE
        STRING
        "Default curve: one of ALT_BN128, BN128, EDWARDS, MNT4, MNT6"
)

set(
        DEPENDS_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/depends"
        CACHE
        STRING
        "Optionally specify the dependency installation directory relative to the source directory (default: inside dependency folder)"
)

option(
        DEBUG
        "Enable debugging mode"
        OFF
)

set(
        OPT_FLAGS
        ""
        CACHE
        STRING
        "Override C++ compiler optimization flags"
)

option(
        MULTICORE
        "Enable parallelized execution, using OpenMP"
        ON
)

option(
        WITH_PROCPS
        "Use procps for memory profiling"
        ON
)

option(
        VERBOSE
        "Print internal messages"
        OFF
)

option(
        BINARY_OUTPUT
        "In serialization, output raw binary data (instead of decimal), which is smaller and faster."
        ON
)

if (CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # Common compilation flags and warning configuration
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wfatal-errors -pthread")

    if ("${MULTICORE}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
    endif ()

    # Default optimizations flags (to override, use -DOPT_FLAGS=...)
    if ("${OPT_FLAGS}" STREQUAL "")
        set(OPT_FLAGS "-ggdb3 -O2 -march=native -mtune=native")
    endif ()
endif ()

add_definitions(-DCURVE_${CURVE})

if (${CURVE} STREQUAL "BN128")
    add_definitions(-DBN_SUPPORT_SNARK=1)
endif ()

if ("${DEBUG}")
    add_definitions(-DDEBUG=1)
endif ()

if ("${VERBOSE}")
    add_definitions(-DVERBOSE=1)
endif ()

if ("${MULTICORE}")
    add_definitions(-DMULTICORE=1)
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPT_FLAGS}")

include(FindPkgConfig)
if ("${WITH_PROCPS}")
    pkg_check_modules(PROCPS REQUIRED libprocps)
else ()
    add_definitions(-DNO_PROCPS)
endif ()

find_package(PkgConfig)
pkg_search_module(GTEST REQUIRED gtest_main)

add_subdirectory(depends)
add_subdirectory(src)
add_subdirectory(test)
