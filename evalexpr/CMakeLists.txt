# evalexpr/CMakeLists.txt
add_library(evalexpr SHARED evalexpr.c)
target_include_directories(evalexpr PUBLIC ${CMAKE_SOURCE_DIR}/evalexpr)
target_link_libraries(evalexpr)