# map/CMakeLists.txt
add_library(map SHARED map.c)
target_include_directories(map PUBLIC ${CMAKE_SOURCE_DIR}/map)
target_link_libraries(map)
