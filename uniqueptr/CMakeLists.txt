# UniquePtr Library CMakeLists.txt

# Create the uniqueptr library
add_library(uniqueptr SHARED
    uniqueptr.c
)

# Set target properties
set_target_properties(uniqueptr PROPERTIES
    OUTPUT_NAME "uniqueptr"
    VERSION 1.0.0
    SOVERSION 1
)

# Set compiler flags
target_compile_options(uniqueptr PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -pedantic -std=c17>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /std:c17>
)

# Include directories
target_include_directories(uniqueptr PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link libraries
target_link_libraries(uniqueptr PRIVATE
    $<$<PLATFORM_ID:Linux>:-lm>
)

# Install rules
install(TARGETS uniqueptr
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES uniqueptr.h
    DESTINATION include
) 