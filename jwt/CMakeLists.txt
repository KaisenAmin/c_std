# jwt/CMakeLists.txt
add_library(jwt SHARED jwt.c)
target_include_directories(jwt PUBLIC ${CMAKE_SOURCE_DIR}/jwt)
target_link_libraries(jwt json stdstring ssl crypto)
