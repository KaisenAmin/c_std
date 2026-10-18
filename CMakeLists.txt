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
#  Build your own example programs ................ put <name>.c files in ./examples, then
#                                                   cmake --build build --target <name>
#                                                   (no examples are shipped; the README ones
#                                                   can be copied there)
#  Strict ISO C17 (no GNU extensions) ............. cmake -S . -B build -DCMAKE_C_EXTENSIONS=OFF
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
# Only a default: a plain set() shadowed -DCMAKE_C_EXTENSIONS=OFF from the
# command line, so a strict -std=c17 build was impossible.
if(NOT DEFINED CMAKE_C_EXTENSIONS)
    set(CMAKE_C_EXTENSIONS ON)
endif()

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
option(C_STD_BUILD_EXAMPLES "Build the programs found in ./examples (none are shipped)" ON)

# ----------------------------------------------------------------------------
# Warning / optimisation flags (compiler agnostic)
# ----------------------------------------------------------------------------
if(MSVC)
    add_compile_options(/W3 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    # No global -Wno-deprecated-declarations: it hid deprecations everywhere
    # and overrode the user's flags. Only crypto.c, which deliberately offers
    # legacy OpenSSL algorithms (MD4, MD5, DES, ...), silences them below.
    add_compile_options(-Wall -Wextra)
endif()

# ----------------------------------------------------------------------------
# Third-party dependencies (installed system-wide; see README.md). Every one
# but Threads is optional: when one is missing, only the modules that need it
# (directly or through another module) are skipped, instead of the whole
# build failing (building `vector` used to need raylib, OpenSSL, GMP, MPFR).
#   OpenSSL  -> crypto, jwt, network, and config / dir (they use crypto)
#   raylib   -> plot, turtle
#   GMP      -> bigint, bigfloat
#   MPFR     -> bigfloat
#   libpq    -> database
#   Threads  -> concurrent (and log / network through it)
# ----------------------------------------------------------------------------
find_package(Threads REQUIRED)
find_package(OpenSSL QUIET)
find_package(raylib  QUIET)
find_package(PostgreSQL QUIET)

# GMP and MPFR ship no standard CMake module -- locate them by hand.
find_path(GMP_INCLUDE_DIR  NAMES gmp.h)
find_library(GMP_LIBRARY   NAMES gmp)
find_path(MPFR_INCLUDE_DIR NAMES mpfr.h)
find_library(MPFR_LIBRARY  NAMES mpfr)

set(C_STD_OPENSSL_MODULES crypto jwt network config dir)
set(C_STD_RAYLIB_MODULES  plot turtle)
set(C_STD_SKIPPED_MODULES "")
if(NOT OpenSSL_FOUND)
    list(APPEND C_STD_SKIPPED_MODULES ${C_STD_OPENSSL_MODULES})
    list(JOIN C_STD_OPENSSL_MODULES " " _c_std_names)
    message(WARNING "OpenSSL not found -- skipping: ${_c_std_names}. Install it -- see README.md.")
endif()
if(NOT raylib_FOUND)
    list(APPEND C_STD_SKIPPED_MODULES ${C_STD_RAYLIB_MODULES})
    list(JOIN C_STD_RAYLIB_MODULES " " _c_std_names)
    message(WARNING "raylib not found -- skipping: ${_c_std_names}. Install it -- see README.md.")
endif()
if(NOT GMP_LIBRARY OR NOT GMP_INCLUDE_DIR)
    list(APPEND C_STD_SKIPPED_MODULES bigint bigfloat)
    message(WARNING "GMP not found -- skipping: bigint bigfloat. Install it -- see README.md.")
elseif(NOT MPFR_LIBRARY OR NOT MPFR_INCLUDE_DIR)
    list(APPEND C_STD_SKIPPED_MODULES bigfloat)
    message(WARNING "MPFR not found -- skipping: bigfloat. Install it -- see README.md.")
endif()

# Extra OS / third-party libraries pulled in by various modules:
#   ws2_32/iphlpapi -> network, sysinfo      ole32/oleaut32/wbemuuid/rpcrt4 -> sysinfo (WMI)
#   bthprops -> sysinfo (bluetooth)          bcrypt -> secrets        crypt32 -> crypto/openssl
#   setupapi -> serial_port                  shlwapi -> dir           pcre -> regex (Windows only)
if(WIN32)
    set(C_STD_PLATFORM_LIBS
        ws2_32 iphlpapi ole32 oleaut32 wbemuuid rpcrt4 bthprops winmm
        bcrypt crypt32 setupapi shlwapi)
    # Windows has no POSIX <regex.h>, so the regex module needs PCRE.
    find_library(PCRE_LIBRARY NAMES pcre libpcre)
    if(PCRE_LIBRARY)
        list(APPEND C_STD_PLATFORM_LIBS ${PCRE_LIBRARY})
    else()
        message(WARNING "PCRE not found: the regex module needs it on Windows (MSYS2: pacman -S mingw-w64-x86_64-pcre; MSVC: vcpkg install pcre). Linking plain 'pcre'.")
        list(APPEND C_STD_PLATFORM_LIBS pcre)
    endif()
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
        # Force the backend: with <pcre.h> installed but no library, the
        # source's __has_include check alone picked PCRE and the link failed.
        set(C_STD_REGEX_FORCE_POSIX ON)
        message(STATUS "PCRE not found -- regex will use the POSIX <regex.h> backend.")
    endif()
endif()

# Header search path shared by every translation unit. Third-party include
# directories are attached only to the modules that use them (below): the
# PostgreSQL *server* include directory, for one, used to be on the path of
# every file.
include_directories(${PROJECT_SOURCE_DIR})

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
    hashmap sort variant uniqueptr jwt set uuid)

# The database module only builds when libpq was found.
if(PostgreSQL_FOUND)
    list(APPEND C_STD_MODULES database)
else()
    message(STATUS "PostgreSQL/libpq not found -- the 'database' module will be skipped.")
endif()
if(C_STD_SKIPPED_MODULES)
    list(REMOVE_ITEM C_STD_MODULES ${C_STD_SKIPPED_MODULES})
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
    # Each third-party library goes only to the modules that use it (raylib
    # used to be linked into all of them).
    if(_mod IN_LIST C_STD_RAYLIB_MODULES)
        target_link_libraries(${_mod} PUBLIC raylib)
    endif()
    if(_mod IN_LIST C_STD_OPENSSL_MODULES)
        target_link_libraries(${_mod} PUBLIC OpenSSL::SSL OpenSSL::Crypto)
    endif()
    list(APPEND C_STD_OBJECTS $<TARGET_OBJECTS:${_mod}>)
    list(APPEND C_STD_MODULE_TARGETS ${_mod})
endforeach()
if(TARGET crypto AND NOT MSVC)
    target_compile_options(crypto PRIVATE -Wno-deprecated-declarations)
endif()
if(TARGET bigint)
    target_include_directories(bigint PUBLIC ${GMP_INCLUDE_DIR})
endif()
if(TARGET bigfloat)
    target_include_directories(bigfloat PUBLIC ${MPFR_INCLUDE_DIR} ${GMP_INCLUDE_DIR})
endif()
if(TARGET database)
    # libpq's client headers only; postgres.c needs nothing from the server dir.
    target_include_directories(database PUBLIC ${PostgreSQL_INCLUDE_DIR})
endif()
if(TARGET regex AND C_STD_REGEX_FORCE_POSIX)
    target_compile_definitions(regex PUBLIC CSTD_REGEX_USE_PCRE=0)
endif()

# Aggregate static library containing every module. It carries its usage
# requirements (include paths and every library it needs), so a program only
# has to link `cstd` (it used to export none, and consumers had to repeat the
# whole list).
add_library(cstd STATIC ${C_STD_OBJECTS})
set_target_properties(cstd PROPERTIES LINKER_LANGUAGE C)
target_include_directories(cstd PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/c_std>)
if(TARGET crypto)
    target_link_libraries(cstd PUBLIC OpenSSL::SSL OpenSSL::Crypto)
endif()
if(TARGET plot OR TARGET turtle)
    target_link_libraries(cstd PUBLIC raylib)
endif()
if(TARGET bigfloat)
    target_include_directories(cstd PUBLIC $<BUILD_INTERFACE:${MPFR_INCLUDE_DIR}>)
    target_link_libraries(cstd PUBLIC ${MPFR_LIBRARY})
endif()
if(TARGET bigint)
    target_include_directories(cstd PUBLIC $<BUILD_INTERFACE:${GMP_INCLUDE_DIR}>)
    target_link_libraries(cstd PUBLIC ${GMP_LIBRARY})
endif()
if(TARGET database)
    target_include_directories(cstd PUBLIC $<BUILD_INTERFACE:${PostgreSQL_INCLUDE_DIR}>)
    target_link_libraries(cstd PUBLIC ${PostgreSQL_LIBRARIES})
endif()
if(TARGET regex AND C_STD_REGEX_FORCE_POSIX)
    target_compile_definitions(cstd PUBLIC CSTD_REGEX_USE_PCRE=0)
endif()
target_link_libraries(cstd PUBLIC Threads::Threads ${C_STD_PLATFORM_LIBS})

# Kept for compatibility: everything an executable must link against.
set(C_STD_LINK_LIBS cstd)

# `cmake --install build --prefix <dir>`: the library and every module header
# (under include/c_std/<module>/, so #include "vector/vector.h" keeps working).
include(GNUInstallDirs)
install(TARGETS cstd ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
foreach(_mod ${C_STD_MODULES})
    file(GLOB _hdrs ${PROJECT_SOURCE_DIR}/${_mod}/*.h)
    if(_hdrs)
        install(FILES ${_hdrs} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/c_std/${_mod})
    endif()
endforeach()

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
# Your own examples : examples/<name>.c  ->  executable <name>
# (the repository ships no examples/ directory; it is in .gitignore)
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
