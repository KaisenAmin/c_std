# matrix/CMakeLists.txt
add_library(matrix SHARED matrix.c)
target_include_directories(matrix PUBLIC ${CMAKE_SOURCE_DIR}/matrix)
target_link_libraries(matrix)
