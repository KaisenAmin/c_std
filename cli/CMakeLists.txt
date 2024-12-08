# cli/CMakeLists.txt
add_library(cli SHARED cli.c)
target_include_directories(cli PUBLIC ${CMAKE_SOURCE_DIR}/cli)
target_link_libraries(cli stdstring)
