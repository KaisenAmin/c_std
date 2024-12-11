add_library(unittest SHARED unittest.c)
target_include_directories(unittest PUBLIC ${CMAKE_SOURCE_DIR}/unittest)
target_link_libraries(unittest stdregex)

