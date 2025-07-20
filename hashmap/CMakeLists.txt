# hashmap/CMakeLists.txt
add_library(hashmap SHARED hashmap.c)
target_include_directories(hashmap PUBLIC ${CMAKE_SOURCE_DIR}/hashmap)
target_link_libraries(hashmap) 