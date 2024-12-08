# time/CMakeLists.txt
add_library(stdtime SHARED std_time.c)
target_include_directories(stdtime PUBLIC ${CMAKE_SOURCE_DIR}/time)
target_link_libraries(stdtime)
