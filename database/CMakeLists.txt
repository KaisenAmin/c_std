# Standalone build of the c_std "database" module.
#
# The top-level CMakeLists.txt does not use this file: it builds every module
# into the single `cstd` library (link `cstd` to get all of them). Use this
# file to build only this module, together with the c_std modules it uses
# (string), e.g. from your own project:
#
#     add_subdirectory(path/to/c_std/database c_std_database)
#     target_link_libraries(your_app PRIVATE c_std_database)
#
# or on its own: cmake -S path/to/c_std/database -B build-database && cmake --build build-database
# Sources are #include "<module>/<header>.h" relative to the c_std root.
# (Generated from the modules' #include graph.)
cmake_minimum_required(VERSION 3.15)
project(c_std_database LANGUAGES C)

get_filename_component(_c_std_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(_c_std_database_sources "")
foreach(_m database string)
    file(GLOB _s CONFIGURE_DEPENDS "${_c_std_root}/${_m}/*.c")
    list(APPEND _c_std_database_sources ${_s})
endforeach()

add_library(c_std_database STATIC ${_c_std_database_sources})
target_include_directories(c_std_database PUBLIC "${_c_std_root}")
set_target_properties(c_std_database PROPERTIES C_STANDARD 17 C_STANDARD_REQUIRED ON C_EXTENSIONS ON)
if(MSVC)
    target_compile_options(c_std_database PRIVATE /W3 /utf-8)
    target_compile_definitions(c_std_database PRIVATE _CRT_SECURE_NO_WARNINGS _WINSOCK_DEPRECATED_NO_WARNINGS)
else()
    target_compile_options(c_std_database PRIVATE -Wall -Wextra)
endif()

find_package(PostgreSQL REQUIRED)
target_include_directories(c_std_database PUBLIC ${PostgreSQL_INCLUDE_DIR})
target_link_libraries(c_std_database PUBLIC ${PostgreSQL_LIBRARIES})

if(WIN32)
    target_link_libraries(c_std_database PUBLIC ws2_32 iphlpapi ole32 oleaut32 wbemuuid rpcrt4 bthprops winmm bcrypt crypt32 setupapi shlwapi)
else()
    target_link_libraries(c_std_database PUBLIC ${CMAKE_DL_LIBS} m)
endif()
