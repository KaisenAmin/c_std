# priority_queue/CMakeLists.txt
add_library(priority_queue SHARED priority_queue.c)
target_include_directories(priority_queue PUBLIC ${CMAKE_SOURCE_DIR}/priority_queue)
target_link_libraries(priority_queue vector)
