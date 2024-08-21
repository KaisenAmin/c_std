# turtle/CMakeLists.txt
add_library(turtle SHARED turtle.c)
target_include_directories(turtle PUBLIC ${CMAKE_SOURCE_DIR}/dependency/include)
target_link_libraries(turtle raylib opengl32 gdi32 winmm)
