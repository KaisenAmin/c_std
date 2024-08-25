add_library(sysinfo SHARED sysinfo.c)
target_include_directories(sysinfo PUBLIC ${CMAKE_SOURCE_DIR}/sysinfo)

if (WIN32)
    target_link_libraries(sysinfo ole32 bthprops)
else()
    # Add any Linux-specific libraries here if needed
    target_link_libraries(sysinfo)
endif()
