# set/CMakeLists.txt
add_library(set SHARED set.c)
target_include_directories(set PUBLIC ${CMAKE_SOURCE_DIR}/set)
target_link_libraries(set)
