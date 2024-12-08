add_library(csv SHARED csv.c)

# Include directories where the header files are located
target_include_directories(csv PUBLIC ${CMAKE_SOURCE_DIR}/csv)

# Link against the necessary OpenSSL libraries
target_link_libraries(csv
    stdstring 
    fileio 
)