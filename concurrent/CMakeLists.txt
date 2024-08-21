add_library(concurrent SHARED
    concurrent.c
    thread_pool.c
)

target_include_directories(concurrent PUBLIC ${CMAKE_SOURCE_DIR}/concurrent)

# Platform-specific configurations
if(WIN32)
    target_compile_definitions(concurrent PRIVATE _TTHREAD_WIN32_)
    target_link_libraries(concurrent PRIVATE ws2_32 fmt)  # Winsock library for network functions
elseif(UNIX)
    target_compile_definitions(concurrent PRIVATE _TTHREAD_POSIX_)
    target_link_libraries(concurrent PRIVATE pthread fmt)
endif()

