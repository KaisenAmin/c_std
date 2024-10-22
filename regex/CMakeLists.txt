# Create a shared library for the regex implementation
add_library(stdregex SHARED std_regex.c)
target_include_directories(stdregex PUBLIC ${CMAKE_SOURCE_DIR}/regex)

if(WIN32)
    target_link_libraries(stdregex pcre)
else()
    target_link_libraries(stdregex pcre m)
endif()


