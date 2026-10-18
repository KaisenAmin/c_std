# Standalone build of the c_std "bigfloat" module.
#
# The top-level CMakeLists.txt does not use this file: it builds every module
# into the single `cstd` library (link `cstd` to get all of them). Use this
# file to build only this module, together with the c_std modules it uses
# (none), e.g. from your own project:
#
#     add_subdirectory(path/to/c_std/bigfloat c_std_bigfloat)
#     target_link_libraries(your_app PRIVATE c_std_bigfloat)
#
# or on its own: cmake -S path/to/c_std/bigfloat -B build-bigfloat && cmake --build build-bigfloat
# Sources are #include "<module>/<header>.h" relative to the c_std root.
# (Generated from the modules' #include graph.)
cmake_minimum_required(VERSION 3.15)
project(c_std_bigfloat LANGUAGES C)

get_filename_component(_c_std_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(_c_std_bigfloat_sources "")
foreach(_m bigfloat)
    file(GLOB _s CONFIGURE_DEPENDS "${_c_std_root}/${_m}/*.c")
    list(APPEND _c_std_bigfloat_sources ${_s})
endforeach()

add_library(c_std_bigfloat STATIC ${_c_std_bigfloat_sources})
target_include_directories(c_std_bigfloat PUBLIC "${_c_std_root}")
set_target_properties(c_std_bigfloat PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED ON C_EXTENSIONS ON)
if(MSVC)
    target_compile_options(c_std_bigfloat PRIVATE /W3 /utf-8)
    target_compile_definitions(c_std_bigfloat PRIVATE _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    target_compile_options(c_std_bigfloat PRIVATE -Wall -Wextra)
endif()

find_path(MPFR_INCLUDE_DIR NAMES mpfr.h)
find_library(MPFR_LIBRARY NAMES mpfr)
if(NOT MPFR_INCLUDE_DIR OR NOT MPFR_LIBRARY)
    message(FATAL_ERROR "MPFR not found (needed by bigfloat) -- see the c_std README.md.")
endif()
target_include_directories(c_std_bigfloat PUBLIC ${MPFR_INCLUDE_DIR})
target_link_libraries(c_std_bigfloat PUBLIC ${MPFR_LIBRARY})

find_path(GMP_INCLUDE_DIR NAMES gmp.h)
find_library(GMP_LIBRARY NAMES gmp)
if(NOT GMP_INCLUDE_DIR OR NOT GMP_LIBRARY)
    message(FATAL_ERROR "GMP not found (needed by bigfloat) -- see the c_std README.md.")
endif()
target_include_directories(c_std_bigfloat PUBLIC ${GMP_INCLUDE_DIR})
target_link_libraries(c_std_bigfloat PUBLIC ${GMP_LIBRARY})

if(WIN32)
    target_link_libraries(c_std_bigfloat PUBLIC ws2_32 iphlpapi ole32 oleaut32 wbemuuid rpcrt4 bthprops winmm bcrypt crypt32 setupapi shlwapi)
else()
    target_link_libraries(c_std_bigfloat PUBLIC ${CMAKE_DL_LIBS} m)
endif()
