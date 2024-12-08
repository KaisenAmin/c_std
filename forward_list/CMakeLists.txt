# forwardlist/CMakeLists.txt
add_library(forwardlist SHARED forward_list.c)
target_include_directories(forwardlist PUBLIC ${CMAKE_SOURCE_DIR}/forward_list)
target_link_libraries(forwardlist)
