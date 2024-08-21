add_library(fileio SHARED
    file_writer.c
    file_reader.c
)

target_include_directories(fileio PUBLIC ${CMAKE_SOURCE_DIR}/file_io)
target_link_libraries(fileio fmt stdstring encoding)
