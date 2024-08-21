# config/CMakeLists.txt
add_library(config SHARED config.c)
target_include_directories(config PUBLIC ${CMAKE_SOURCE_DIR}/config)
target_link_libraries(config fmt stdstring fileio)
