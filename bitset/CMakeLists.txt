# bitset/CMakeLists.txt
add_library(bitset SHARED bitset.c)
target_include_directories(bitset PUBLIC ${CMAKE_SOURCE_DIR}/bitset)
target_link_libraries(bitset)
