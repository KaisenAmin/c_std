# numeric/CMakeLists.txt
add_library(numeric SHARED numeric.c)
target_include_directories(numeric PUBLIC ${CMAKE_SOURCE_DIR}/numeric)

