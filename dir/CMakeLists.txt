# vector/CMakeLists.txt
add_library(dir SHARED dir.c)
target_include_directories(dir PUBLIC ${CMAKE_SOURCE_DIR}/dir)
target_link_libraries(dir fmt vector stdstring encoding stdcrypto fileio Shlwapi)
