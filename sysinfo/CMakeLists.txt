# sysinfo/CMakeLists.txt
add_library(sysinfo SHARED sysinfo.c)
target_include_directories(sysinfo PUBLIC ${CMAKE_SOURCE_DIR}/sysinfo)
target_link_libraries(sysinfo ole32 bthprops)
