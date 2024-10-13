# secrets/CMakeLists.txt
add_library(secrets SHARED secrets.c)
target_include_directories(secrets PUBLIC ${CMAKE_SOURCE_DIR}/secrets)

if (WIN32)
    target_link_libraries(secrets bcrypt)
endif()