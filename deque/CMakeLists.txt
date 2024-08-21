add_library(deque SHARED deque.c)

# Include directories where the header files are located
target_include_directories(deque PUBLIC ${CMAKE_SOURCE_DIR}/deque)

# Link against the necessary OpenSSL libraries
target_link_libraries(deque
    fmt  
)