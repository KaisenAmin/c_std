# bigfloat/CMakeLists.txt
add_library(bigfloat SHARED bigfloat.c)
target_include_directories(bigfloat PUBLIC ${CMAKE_SOURCE_DIR}/bigfloat)
target_link_libraries(bigfloat MPFR)