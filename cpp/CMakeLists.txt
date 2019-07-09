cmake_minimum_required(VERSION 2.8)

cmake_policy(SET CMP0076 NEW)

project(k0)

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
    OFF
)

option(
    WITH_SUPERCOP
    "Support for Ed25519 signatures required by ADSNARK"
    OFF
)

option(
    WITH_PROCPS
    "Use procps for memory profiling"
    OFF
)

option(
    VERBOSE
    "Print internal messages"
    OFF
)

option(
    BINARY_OUTPUT
    "In serialization, output raw binary data (instead of decimal), which is smaller and faster."
    OFF
)

option(
    MONTGOMERY_OUTPUT
    "Serialize Fp elements as their Montgomery representations (faster but not human-readable)"
    OFF
)

option(
    USE_PT_COMPRESSION
    "Use point compression"
    ON
)

option(
    CPPDEBUG
    "Enable debugging of C++ STL (does not imply DEBUG)"
    OFF
)

option(
    PERFORMANCE
    "Enable link-time and aggressive optimizations"
    OFF
)

option(
    USE_ASM
    "Use architecture-specific optimized assembly code"
    ON
)

if (CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # Common compilation flags and warning configuration
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wfatal-errors -pthread")

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

if("${MULTICORE}")
  add_definitions(-DMULTICORE=1)
endif()

if("${BINARY_OUTPUT}")
  add_definitions(-DBINARY_OUTPUT)
endif()

if("${MONTGOMERY_OUTPUT}")
  add_definitions(-DMONTGOMERY_OUTPUT)
endif()

if(NOT "${USE_PT_COMPRESSION}")
  add_definitions(-DNO_PT_COMPRESSION=1)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPT_FLAGS}")

include(FindPkgConfig)
if ("${WITH_PROCPS}")
    pkg_check_modules(PROCPS REQUIRED libprocps)
else ()
    add_definitions(-DNO_PROCPS)
endif ()

if ("${USE_DUMMYHASH}")
  add_definitions(-DK0_DUMMYHASH)
endif ()

if("${PERFORMANCE}")
  add_definitions(-DNDEBUG)
  set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -flto -fuse-linker-plugin"
  )
  set(
    CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -flto"
  )
endif()

if("${USE_ASM}")
  add_definitions(-DUSE_ASM)
endif()

find_package(PkgConfig)
pkg_search_module(GTEST REQUIRED gtest_main)

if (NOT "${USE_INSTALLED_LIBSNARK}")
  add_subdirectory(depends)
endif ()

add_subdirectory(src)
add_subdirectory(test)
