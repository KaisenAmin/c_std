# queue/CMakeLists.txt
add_library(queue SHARED queue.c)
target_include_directories(queue PUBLIC ${CMAKE_SOURCE_DIR}/queue)
target_link_libraries(queue vector algorithm)
