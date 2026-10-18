# Standalone build of the c_std "deque" module.
#
# The top-level CMakeLists.txt does not use this file: it builds every module
# into the single `cstd` library (link `cstd` to get all of them). Use this
# file to build only this module, together with the c_std modules it uses
# (none), e.g. from your own project:
#
#     add_subdirectory(path/to/c_std/deque c_std_deque)
#     target_link_libraries(your_app PRIVATE c_std_deque)
#
# or on its own: cmake -S path/to/c_std/deque -B build-deque && cmake --build build-deque
# Sources are #include "<module>/<header>.h" relative to the c_std root.
# (Generated from the modules' #include graph.)
cmake_minimum_required(VERSION 3.15)
project(c_std_deque LANGUAGES C)

get_filename_component(_c_std_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(_c_std_deque_sources "")
foreach(_m deque)
    file(GLOB _s CONFIGURE_DEPENDS "${_c_std_root}/${_m}/*.c")
    list(APPEND _c_std_deque_sources ${_s})
endforeach()

add_library(c_std_deque STATIC ${_c_std_deque_sources})
target_include_directories(c_std_deque PUBLIC "${_c_std_root}")
set_target_properties(c_std_deque PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED ON C_EXTENSIONS ON)
if(MSVC)
    target_compile_options(c_std_deque PRIVATE /W3 /utf-8)
    target_compile_definitions(c_std_deque PRIVATE _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    target_compile_options(c_std_deque PRIVATE -Wall -Wextra)
endif()

if(WIN32)
    target_link_libraries(c_std_deque PUBLIC ws2_32 iphlpapi ole32 oleaut32 wbemuuid rpcrt4 bthprops winmm bcrypt crypt32 setupapi shlwapi)
else()
    target_link_libraries(c_std_deque PUBLIC ${CMAKE_DL_LIBS} m)
endif()
