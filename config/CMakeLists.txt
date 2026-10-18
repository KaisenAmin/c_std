# Standalone build of the c_std "config" module.
#
# The top-level CMakeLists.txt does not use this file: it builds every module
# into the single `cstd` library (link `cstd` to get all of them). Use this
# file to build only this module, together with the c_std modules it uses
# (crypto, encoding, file_io, fmt, string), e.g. from your own project:
#
#     add_subdirectory(path/to/c_std/config c_std_config)
#     target_link_libraries(your_app PRIVATE c_std_config)
#
# or on its own: cmake -S path/to/c_std/config -B build-config && cmake --build build-config
# Sources are #include "<module>/<header>.h" relative to the c_std root.
# (Generated from the modules' #include graph.)
cmake_minimum_required(VERSION 3.15)
project(c_std_config LANGUAGES C)

get_filename_component(_c_std_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(_c_std_config_sources "")
foreach(_m config crypto encoding file_io fmt string)
    file(GLOB _s CONFIGURE_DEPENDS "${_c_std_root}/${_m}/*.c")
    list(APPEND _c_std_config_sources ${_s})
endforeach()

add_library(c_std_config STATIC ${_c_std_config_sources})
target_include_directories(c_std_config PUBLIC "${_c_std_root}")
set_target_properties(c_std_config PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED ON C_EXTENSIONS ON)
if(MSVC)
    target_compile_options(c_std_config PRIVATE /W3 /utf-8)
    target_compile_definitions(c_std_config PRIVATE _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    target_compile_options(c_std_config PRIVATE -Wall -Wextra)
endif()

find_package(OpenSSL REQUIRED)
target_link_libraries(c_std_config PUBLIC OpenSSL::SSL OpenSSL::Crypto)
if(NOT MSVC)
    # crypto deliberately offers legacy OpenSSL algorithms (MD4, MD5, DES, ...)
    set_source_files_properties("${_c_std_root}/crypto/crypto.c" PROPERTIES COMPILE_OPTIONS -Wno-deprecated-declarations)
endif()

if(WIN32)
    target_link_libraries(c_std_config PUBLIC ws2_32 iphlpapi ole32 oleaut32 wbemuuid rpcrt4 bthprops winmm bcrypt crypt32 setupapi shlwapi)
else()
    target_link_libraries(c_std_config PUBLIC ${CMAKE_DL_LIBS} m)
endif()
