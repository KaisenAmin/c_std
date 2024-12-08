# list/CMakeLists.txt
add_library(log SHARED log.c)
target_include_directories(log PUBLIC ${CMAKE_SOURCE_DIR}/log)
target_link_libraries(log fileio stdstring)
