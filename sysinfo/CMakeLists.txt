add_library(sysinfo SHARED sysinfo.c)
target_include_directories(sysinfo PUBLIC ${CMAKE_SOURCE_DIR}/sysinfo)

if (WIN32)
    target_link_libraries(sysinfo ole32 bthprops ws2_32 iphlpapi vector)
else()
    target_link_libraries(sysinfo m vector)
endif()
