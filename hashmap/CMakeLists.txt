# Standalone build of the c_std "hashmap" module.
#
# The top-level CMakeLists.txt does not use this file: it builds every module
# into the single `cstd` library (link `cstd` to get all of them). Use this
# file to build only this module, together with the c_std modules it uses
# (none: it includes crypto/crypto.h only for the static inline XXH3 there,
# which needs no crypto sources and no OpenSSL), e.g. from your own project:
#
#     add_subdirectory(path/to/c_std/hashmap c_std_hashmap)
#     target_link_libraries(your_app PRIVATE c_std_hashmap)
#
# or on its own: cmake -S path/to/c_std/hashmap -B build-hashmap && cmake --build build-hashmap
# Sources are #include "<module>/<header>.h" relative to the c_std root.
# (Generated from the modules' #include graph.)
cmake_minimum_required(VERSION 3.15)
project(c_std_hashmap LANGUAGES C)

get_filename_component(_c_std_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(_c_std_hashmap_sources "")
foreach(_m hashmap)
    file(GLOB _s CONFIGURE_DEPENDS "${_c_std_root}/${_m}/*.c")
    list(APPEND _c_std_hashmap_sources ${_s})
endforeach()

add_library(c_std_hashmap STATIC ${_c_std_hashmap_sources})
target_include_directories(c_std_hashmap PUBLIC "${_c_std_root}")
set_target_properties(c_std_hashmap PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED ON C_EXTENSIONS ON)
if(MSVC)
    target_compile_options(c_std_hashmap PRIVATE /W3 /utf-8)
    target_compile_definitions(c_std_hashmap PRIVATE _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    target_compile_options(c_std_hashmap PRIVATE -Wall -Wextra)
endif()

if(WIN32)
    target_link_libraries(c_std_hashmap PUBLIC ws2_32 iphlpapi ole32 oleaut32 wbemuuid rpcrt4 bthprops winmm bcrypt crypt32 setupapi shlwapi)
else()
    target_link_libraries(c_std_hashmap PUBLIC ${CMAKE_DL_LIBS} m)
endif()
