# random/CMakeLists.txt
add_library(random SHARED random.c)
target_include_directories(random PUBLIC ${CMAKE_SOURCE_DIR}/random)
target_link_libraries(random)