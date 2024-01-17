#ifndef FILE_READER_H_
#define FILE_READER_H_

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    FILE* file_reader;
}FilerReader;

typedef enum {
    READ_TEXT,          // Open for reading in text mode
    READ_BINARY,        // Open for reading in binary mode
    READ_UNICODE,       // Open for reading Unicode text (may involve encoding conversions)
    READ_BUFFERED,      // Open for buffered reading (optimizes read operations)
    READ_UNBUFFERED,    // Open for unbuffered reading (direct read operations)
    READ_LINE,          // Open for line-by-line reading (useful for text files)
} ReadMode;

FilerReader* file_reader_open(const char* filename, const ReadMode mode);
bool file_reader_close(FilerReader* reader);

size_t file_reader_tell_position(FilerReader* reader);
size_t file_reader_read(void* buffer, size_t size, size_t count, FilerReader* reader);
char* file_reader_read_line(char* buffer, size_t size, FilerReader* reader);

#endif 