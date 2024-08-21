# fmt/CMakeLists.txt
add_library(fmt SHARED fmt.c)
target_include_directories(fmt PUBLIC ${CMAKE_SOURCE_DIR}/fmt)
target_link_libraries(fmt encoding stdstring)