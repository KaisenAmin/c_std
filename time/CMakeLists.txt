# Standalone build of the c_std "time" module.
#
# The top-level CMakeLists.txt does not use this file: it builds every module
# into the single `cstd` library (link `cstd` to get all of them). Use this
# file to build only this module, together with the c_std modules it uses
# (none), e.g. from your own project:
#
#     add_subdirectory(path/to/c_std/time c_std_time)
#     target_link_libraries(your_app PRIVATE c_std_time)
#
# or on its own: cmake -S path/to/c_std/time -B build-time && cmake --build build-time
# Sources are #include "<module>/<header>.h" relative to the c_std root.
# (Generated from the modules' #include graph.)
cmake_minimum_required(VERSION 3.15)
project(c_std_time LANGUAGES C)

get_filename_component(_c_std_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(_c_std_time_sources "")
foreach(_m time)
    file(GLOB _s CONFIGURE_DEPENDS "${_c_std_root}/${_m}/*.c")
    list(APPEND _c_std_time_sources ${_s})
endforeach()

add_library(c_std_time STATIC ${_c_std_time_sources})
target_include_directories(c_std_time PUBLIC "${_c_std_root}")
set_target_properties(c_std_time PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED ON C_EXTENSIONS ON)
if(MSVC)
    target_compile_options(c_std_time PRIVATE /W3 /utf-8)
    target_compile_definitions(c_std_time PRIVATE _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    target_compile_options(c_std_time PRIVATE -Wall -Wextra)
endif()

if(WIN32)
    target_link_libraries(c_std_time PUBLIC ws2_32 iphlpapi ole32 oleaut32 wbemuuid rpcrt4 bthprops winmm bcrypt crypt32 setupapi shlwapi)
else()
    target_link_libraries(c_std_time PUBLIC ${CMAKE_DL_LIBS} m)
endif()
