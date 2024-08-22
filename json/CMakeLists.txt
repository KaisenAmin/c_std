# json/CMakeLists.txt
add_library(json SHARED json.c)
target_include_directories(json PUBLIC ${CMAKE_SOURCE_DIR}/json)
target_link_libraries(json stdstring fmt fileio vector map)
