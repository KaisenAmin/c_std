# fmt/CMakeLists.txt
add_library(statistics SHARED statistics.c)
target_include_directories(statistics PUBLIC ${CMAKE_SOURCE_DIR}/statistics)
