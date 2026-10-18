# Standalone build of the c_std "unittest" module.
#
# The top-level CMakeLists.txt does not use this file: it builds every module
# into the single `cstd` library (link `cstd` to get all of them). Use this
# file to build only this module, together with the c_std modules it uses
# (regex), e.g. from your own project:
#
#     add_subdirectory(path/to/c_std/unittest c_std_unittest)
#     target_link_libraries(your_app PRIVATE c_std_unittest)
#
# or on its own: cmake -S path/to/c_std/unittest -B build-unittest && cmake --build build-unittest
# Sources are #include "<module>/<header>.h" relative to the c_std root.
# (Generated from the modules' #include graph.)
cmake_minimum_required(VERSION 3.15)
project(c_std_unittest LANGUAGES C)

get_filename_component(_c_std_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(_c_std_unittest_sources "")
foreach(_m regex unittest)
    file(GLOB _s CONFIGURE_DEPENDS "${_c_std_root}/${_m}/*.c")
    list(APPEND _c_std_unittest_sources ${_s})
endforeach()

add_library(c_std_unittest STATIC ${_c_std_unittest_sources})
target_include_directories(c_std_unittest PUBLIC "${_c_std_root}")
set_target_properties(c_std_unittest PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED ON C_EXTENSIONS ON)
if(MSVC)
    target_compile_options(c_std_unittest PRIVATE /W3 /utf-8)
    target_compile_definitions(c_std_unittest PRIVATE _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    target_compile_options(c_std_unittest PRIVATE -Wall -Wextra)
endif()

# regex prefers PCRE; without the PCRE library it is built on POSIX <regex.h>
# (Windows has no <regex.h>, so PCRE is required there).
find_library(PCRE_LIBRARY NAMES pcre libpcre)
if(PCRE_LIBRARY)
    target_link_libraries(c_std_unittest PUBLIC ${PCRE_LIBRARY})
elseif(WIN32)
    message(FATAL_ERROR "PCRE not found (needed by unittest on Windows) -- see regex/README.md.")
else()
    target_compile_definitions(c_std_unittest PUBLIC CSTD_REGEX_USE_PCRE=0)
endif()

if(WIN32)
    target_link_libraries(c_std_unittest PUBLIC ws2_32 iphlpapi ole32 oleaut32 wbemuuid rpcrt4 bthprops winmm bcrypt crypt32 setupapi shlwapi)
else()
    target_link_libraries(c_std_unittest PUBLIC ${CMAKE_DL_LIBS} m)
endif()
