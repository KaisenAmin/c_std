# list/CMakeLists.txt
add_library(list SHARED list.c)
target_include_directories(list PUBLIC ${CMAKE_SOURCE_DIR}/list)
target_link_libraries(list)
