# array/CMakeLists.txt
add_library(array SHARED array.c)
target_include_directories(array PUBLIC ${CMAKE_SOURCE_DIR}/array)
target_link_libraries(array vector)
