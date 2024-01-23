#ifndef FILE_READER_H_
#define FILE_READER_H_

// #include <stdio.h>
// #include <stdbool.h>
// #include <stddef.h>
#include "file_writer.h"

typedef enum {
    READ_TEXT,          // Open for reading in text mode
    READ_BINARY,        // Open for reading in binary mode
    READ_UNICODE,       // Open for reading Unicode text (may involve encoding conversions)
    READ_BUFFERED,      // Open for buffered reading (optimizes read operations)
    READ_UNBUFFERED,    // Open for unbuffered reading (direct read operations)
    READ_LINE,          // Open for line-by-line reading (useful for text files)
} ReadMode;

typedef enum {
    READ_ENCODING_UTF16,
    READ_ENCODING_UTF32,
} ReadEncodingType;

typedef struct {
    FILE* file_reader;
    ReadMode mode;
    bool is_open;
    ReadEncodingType encoding;
    char* file_path;
}FileReader;

FileReader* file_reader_open(const char* filename, const ReadMode mode);

bool file_reader_close(FileReader* reader);
bool file_reader_set_encoding(FileReader* reader, const ReadEncodingType encoding);
bool file_reader_seek(FileReader* reader, long offset, const CursorPosition cursor_pos);
bool file_reader_is_open(FileReader* reader);
bool file_reader_eof(FileReader* reader);
bool file_reader_copy(FileReader* src_reader, FileWriter* dest_writer);
bool file_reader_read_line(char* buffer, size_t size, FileReader* reader);
bool file_reader_read_lines(FileReader* reader, char*** buffer, size_t num_lines);

size_t file_reader_get_position(FileReader* reader);
size_t file_reader_read(void* buffer, size_t size, size_t count, FileReader* reader);
size_t file_reader_get_size(FileReader* reader);
size_t file_reader_read_fmt(FileReader* reader, const char* format, ...);

const char* file_reader_get_file_name(FileReader* reader);

#endif 