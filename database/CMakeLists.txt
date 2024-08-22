# database/CMakeLists.txt
add_library(database SHARED postgres.c)
target_include_directories(database PUBLIC ${CMAKE_SOURCE_DIR}/database)
target_link_libraries(database fmt stdstring pq)
