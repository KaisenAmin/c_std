add_library(date SHARED date.c)

# Include directories where the header files are located
target_include_directories(date PUBLIC ${CMAKE_SOURCE_DIR}/date)

# Link against the necessary OpenSSL libraries
target_link_libraries(date 
    stdtime  
)