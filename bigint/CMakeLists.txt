# bigint/CMakeLists.txt
add_library(bigint SHARED bigint.c)
target_include_directories(bigint PUBLIC ${CMAKE_SOURCE_DIR}/bigint)
target_link_libraries(bigint gmp)