# encoding/CMakeLists.txt
add_library(encoding SHARED encoding.c)
target_include_directories(encoding PUBLIC ${CMAKE_SOURCE_DIR}/encoding)
target_link_libraries(encoding)