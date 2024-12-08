# span/CMakeLists.txt
add_library(span SHARED span.c)
target_include_directories(span PUBLIC ${CMAKE_SOURCE_DIR}/span)
target_link_libraries(span)
