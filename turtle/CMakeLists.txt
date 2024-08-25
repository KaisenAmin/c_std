add_library(turtle SHARED turtle.c)
target_include_directories(turtle PUBLIC ${CMAKE_SOURCE_DIR}/dependency/include)

if (WIN32)
    target_link_libraries(turtle raylib opengl32 gdi32 winmm)
else()
    target_link_libraries(turtle raylib GL X11 m pthread)
endif()
