cmake_minimum_required(VERSION 3.10)

# Create the sort library
add_library(sort SHARED
    sort.c
)

# Set target properties
set_target_properties(sort PROPERTIES
    OUTPUT_NAME "sort"
    VERSION 1.0.0
    SOVERSION 1
)

# Include directories
target_include_directories(sort PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set compile definitions
target_compile_definitions(sort PRIVATE
    sort_EXPORTS
)
