# Variant Library CMakeLists.txt

# Create the variant library
add_library(variant SHARED
    variant.c
)

# Set target properties
set_target_properties(variant PROPERTIES
    OUTPUT_NAME "variant"
    VERSION 1.0.0
    SOVERSION 1
)

# Set compiler flags
target_compile_options(variant PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -pedantic -std=c17>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /std:c17>
)

# Include directories
target_include_directories(variant PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(variant PRIVATE
    $<$<PLATFORM_ID:Linux>:-lm>
)

# Install rules
install(TARGETS variant
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES variant.h
    DESTINATION include
) 