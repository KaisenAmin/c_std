# Standalone build of the c_std "numbers" module.
#
# The top-level CMakeLists.txt does not use this file: it builds every module
# into the single `cstd` library (link `cstd` to get all of them). Use this
# file to build only this module, together with the c_std modules it uses
# (none), e.g. from your own project:
#
#     add_subdirectory(path/to/c_std/numbers c_std_numbers)
#     target_link_libraries(your_app PRIVATE c_std_numbers)
#
# or on its own: cmake -S path/to/c_std/numbers -B build-numbers && cmake --build build-numbers
# Sources are #include "<module>/<header>.h" relative to the c_std root.
# (Generated from the modules' #include graph.)
cmake_minimum_required(VERSION 3.15)
project(c_std_numbers LANGUAGES C)

get_filename_component(_c_std_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(c_std_numbers INTERFACE)
target_include_directories(c_std_numbers INTERFACE "${_c_std_root}")
target_compile_features(c_std_numbers INTERFACE c_std_17)
