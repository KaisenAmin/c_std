# ============================================================================
#  c_std  --  C++ STL / Python-style standard library, reimplemented in C
#  Cross-platform CMake build (GCC, Clang, MSVC) -- C17.
#
#  Quick start (from the project root):
#       cmake -S . -B build
#       cmake --build build                 # build everything
#       cmake --build build --target run     # build + run the main demo
#
#  Build a single module library .................. cmake --build build --target vector
#  Build / run a single example ................... cmake --build build --target vector_example
#  Disable the per-module examples ................ cmake -S . -B build -DC_STD_BUILD_EXAMPLES=OFF
#
#  Third-party libraries are expected to come from the system package manager
#  (MSYS2 pacman on Windows, apt on Debian/Ubuntu, dnf on Fedora) -- see README.md.
# ============================================================================
cmake_minimum_required(VERSION 3.15)
project(c_std LANGUAGES C)

# ----------------------------------------------------------------------------
# Language standard : C17 (with platform extensions, since several modules use
# POSIX / Win32 APIs). MSVC uses /std:c17, GCC/Clang use -std=gnu17.
# ----------------------------------------------------------------------------
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Default to a Release build when the user did not pick one (single-config gens).
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Put every executable in build/bin so data files in ./sources stay reachable.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ----------------------------------------------------------------------------
# Options
# ----------------------------------------------------------------------------
option(C_STD_BUILD_MAIN     "Build the main demo executable"       ON)
option(C_STD_BUILD_EXAMPLES "Build the per-module example programs" ON)

# ----------------------------------------------------------------------------
# Warning / optimisation flags (compiler agnostic)
# ----------------------------------------------------------------------------
if(MSVC)
    add_compile_options(/W3 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wno-deprecated-declarations)
endif()

# ----------------------------------------------------------------------------
# Third-party dependencies (installed system-wide; see README.md)
#   OpenSSL  -> crypto, jwt, network
#   raylib   -> plot, turtle
#   GMP      -> bigint
#   MPFR     -> bigfloat
#   libpq    -> database  (optional)
#   Threads  -> concurrent
# ----------------------------------------------------------------------------
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(raylib  REQUIRED)
find_package(PostgreSQL QUIET)            # optional: only needed for the database module

# GMP and MPFR ship no standard CMake module -- locate them by hand.
find_path(GMP_INCLUDE_DIR  NAMES gmp.h)
find_library(GMP_LIBRARY   NAMES gmp)
find_path(MPFR_INCLUDE_DIR NAMES mpfr.h)
find_library(MPFR_LIBRARY  NAMES mpfr)
if(NOT GMP_LIBRARY)
    message(FATAL_ERROR "GMP not found (needed by the 'bigint' module). Install it -- see README.md.")
endif()
if(NOT MPFR_LIBRARY)
    message(FATAL_ERROR "MPFR not found (needed by the 'bigfloat' module). Install it -- see README.md.")
endif()

# Extra OS / third-party libraries pulled in by various modules:
#   ws2_32/iphlpapi -> network, sysinfo      ole32/oleaut32/wbemuuid/rpcrt4 -> sysinfo (WMI)
#   bthprops -> sysinfo (bluetooth)          bcrypt -> secrets        crypt32 -> crypto/openssl
#   setupapi -> serial_port                  shlwapi -> dir           pcre -> regex (Windows only)
if(WIN32)
    set(C_STD_PLATFORM_LIBS
        ws2_32 iphlpapi ole32 oleaut32 wbemuuid rpcrt4 bthprops winmm
        bcrypt crypt32 setupapi shlwapi pcre)
else()
    set(C_STD_PLATFORM_LIBS ${CMAKE_DL_LIBS} m)
    # The regex module prefers PCRE when <pcre.h> is available (it auto-detects
    # via __has_include) and otherwise falls back to POSIX regex in libc. Link
    # PCRE here when it is installed so the link matches the compiled backend.
    find_library(PCRE_LIBRARY NAMES pcre)
    if(PCRE_LIBRARY)
        list(APPEND C_STD_PLATFORM_LIBS ${PCRE_LIBRARY})
        message(STATUS "PCRE found (${PCRE_LIBRARY}) -- regex will use the PCRE backend.")
    else()
        message(STATUS "PCRE not found -- regex will use the POSIX <regex.h> backend.")
    endif()
endif()

# Header search paths shared by every translation unit.
include_directories(${PROJECT_SOURCE_DIR})
foreach(_inc ${OPENSSL_INCLUDE_DIR} ${GMP_INCLUDE_DIR} ${MPFR_INCLUDE_DIR})
    if(_inc)
        include_directories(${_inc})
    endif()
endforeach()
if(PostgreSQL_FOUND)
    include_directories(${PostgreSQL_INCLUDE_DIRS})
endif()

# ----------------------------------------------------------------------------
# Modules.  Each directory becomes an OBJECT library (so it can be compiled on
# its own) and every object is rolled up into the single static library `cstd`.
# A module is skipped automatically if it has no .c files (e.g. header-only
# `numbers`) or if its third-party dependency is missing (e.g. `database`).
# ----------------------------------------------------------------------------
set(C_STD_MODULES
    string fmt encoding random map vector time statistics tuple turtle sysinfo
    stack span queue priority_queue algorithm array bitset cli concurrent config
    file_io crypto csv date deque dir forward_list list log matrix network numbers
    json secrets xml regex unittest serial_port plot bigint evalexpr bigfloat
    hashmap sort variant uniqueptr jwt set)

# The database module only builds when libpq was found.
if(PostgreSQL_FOUND)
    list(APPEND C_STD_MODULES database)
else()
    message(STATUS "PostgreSQL/libpq not found -- the 'database' module will be skipped.")
endif()

set(C_STD_OBJECTS "")
set(C_STD_MODULE_TARGETS "")
foreach(_mod ${C_STD_MODULES})
    file(GLOB _srcs CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/${_mod}/*.c)
    if(NOT _srcs)
        continue()
    endif()
    add_library(${_mod} OBJECT ${_srcs})
    target_include_directories(${_mod} PUBLIC ${PROJECT_SOURCE_DIR})
    # raylib's interface include dir is needed to compile plot/turtle headers.
    target_link_libraries(${_mod} PUBLIC raylib)
    list(APPEND C_STD_OBJECTS $<TARGET_OBJECTS:${_mod}>)
    list(APPEND C_STD_MODULE_TARGETS ${_mod})
endforeach()

# Aggregate static library containing every module.
add_library(cstd STATIC ${C_STD_OBJECTS})
set_target_properties(cstd PROPERTIES LINKER_LANGUAGE C)
target_include_directories(cstd PUBLIC ${PROJECT_SOURCE_DIR})

# Everything an executable must link against to use the whole library.
set(C_STD_LINK_LIBS
    cstd
    OpenSSL::SSL OpenSSL::Crypto
    raylib
    ${GMP_LIBRARY} ${MPFR_LIBRARY}
    Threads::Threads
    ${C_STD_PLATFORM_LIBS})
if(PostgreSQL_FOUND)
    list(APPEND C_STD_LINK_LIBS ${PostgreSQL_LIBRARIES})
endif()

# Convenience target: `cmake --build build --target modules` builds all module libs.
add_custom_target(modules DEPENDS ${C_STD_MODULE_TARGETS})

# ----------------------------------------------------------------------------
# main demo executable
# ----------------------------------------------------------------------------
if(C_STD_BUILD_MAIN)
    add_executable(main main.c)
    target_link_libraries(main PRIVATE ${C_STD_LINK_LIBS})

    # `cmake --build build --target run` -- run from the source tree so the
    # program can find data files under ./sources.
    add_custom_target(run
        COMMAND $<TARGET_FILE:main>
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        DEPENDS main
        USES_TERMINAL)
endif()

# ----------------------------------------------------------------------------
# Per-module examples : examples/<name>.c  ->  executable <name>
# Build one with:  cmake --build build --target <name>
# Build all with:  cmake --build build --target examples
# ----------------------------------------------------------------------------
if(C_STD_BUILD_EXAMPLES)
    file(GLOB _example_srcs CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/examples/*.c)
    add_custom_target(examples)
    foreach(_ex ${_example_srcs})
        get_filename_component(_exname ${_ex} NAME_WE)
        add_executable(${_exname} ${_ex})
        target_link_libraries(${_exname} PRIVATE ${C_STD_LINK_LIBS})
        add_dependencies(examples ${_exname})
    endforeach()
endif()
