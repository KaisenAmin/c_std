# vector/CMakeLists.txt
add_library(vector SHARED vector.c)
target_include_directories(vector PUBLIC ${CMAKE_SOURCE_DIR}/vector)
target_link_libraries(vector)
