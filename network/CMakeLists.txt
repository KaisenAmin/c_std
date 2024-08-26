
add_library(network SHARED tcp.c udp.c http.c)

# Specify the include directories for this library
target_include_directories(network PUBLIC ${CMAKE_SOURCE_DIR}/network)

target_link_libraries(network fmt ssl stdcrypto stdstring json)

# Link platform-specific libraries
if(WIN32)
    target_link_libraries(network ws2_32)
else()
    target_link_libraries(network pthread)
endif()
