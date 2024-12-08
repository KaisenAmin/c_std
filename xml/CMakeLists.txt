# xml/CMakeLists.txt
add_library(xml SHARED xml.c)
target_include_directories(xml PUBLIC ${CMAKE_SOURCE_DIR}/xml)
target_link_libraries(xml)

