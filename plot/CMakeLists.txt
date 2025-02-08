add_library(plot SHARED plot.c)
target_include_directories(plot PUBLIC ${CMAKE_SOURCE_DIR}/dependency/include)

if (WIN32)
    target_link_libraries(plot raylib opengl32 gdi32 winmm)
else()
    target_link_libraries(plot raylib GL X11 m pthread)
endif()
