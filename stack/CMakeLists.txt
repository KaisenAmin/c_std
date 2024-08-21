# stack/CMakeLists.txt
add_library(stack SHARED stack.c)
target_include_directories(stack PUBLIC ${CMAKE_SOURCE_DIR}/stack)
target_link_libraries(stack fmt vector)
