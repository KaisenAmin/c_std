# json/CMakeLists.txt
add_library(json SHARED json.c)
target_include_directories(json PUBLIC ${CMAKE_SOURCE_DIR}/json)
target_link_libraries(json string fmt file_io vector map)
