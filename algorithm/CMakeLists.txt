# algorithm/CMakeLists.txt
add_library(algorithm SHARED algorithm.c)
target_include_directories(algorithm PUBLIC ${CMAKE_SOURCE_DIR}/algorithm)
target_link_libraries(algorithm)
