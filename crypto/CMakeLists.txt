add_library(stdcrypto SHARED crypto.c)

# Include directories where the header files are located
target_include_directories(stdcrypto PUBLIC ${CMAKE_SOURCE_DIR}/crypto)
target_link_libraries(stdcrypto ssl crypto)