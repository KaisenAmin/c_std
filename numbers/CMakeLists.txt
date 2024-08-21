# CMakeLists.txt for numbers module (header-only)

# Add the numbers directory as an include directory
add_library(numbers INTERFACE)
target_include_directories(numbers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
