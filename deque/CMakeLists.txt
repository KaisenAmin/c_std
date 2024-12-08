add_library(deque SHARED deque.c)
target_include_directories(deque PUBLIC ${CMAKE_SOURCE_DIR}/deque)
target_link_libraries(deque)