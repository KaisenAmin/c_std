# tuple/CMakeLists.txt
add_library(tuple SHARED tuple.c)
target_include_directories(tuple PUBLIC ${CMAKE_SOURCE_DIR}/tuple)
target_link_libraries(tuple)
