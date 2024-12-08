# Create a shared library from std_string.c
add_library(stdstring SHARED std_string.c)
target_include_directories(stdstring PUBLIC ${CMAKE_SOURCE_DIR}/string)
target_link_libraries(stdstring)
