#ifndef FILE_WRITER_H_
#define FILE_WRITER_H_ 

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    WRITE_TEXT,          // Open for writing in text mode
    WRITE_BINARY,        // Open for writing in binary mode
    WRITE_UNICODE,       // Open for writing Unicode text (may involve encoding conversions)
    WRITE_BUFFERED,      // Open for buffered writing (optimizes write operations)
    WRITE_UNBUFFERED,    // Open for unbuffered writing (direct write operations)
    WRITE_LINE,          // Open for line-by-line writing (useful for text files)
} WriteMode;

typedef struct {
    FILE* file_writer;
    WriteMode mode;
    bool is_open;
    
} FileWriter;



FileWriter* file_writer_open(const char* filename, const WriteMode mode);
bool file_writer_close(FileWriter* writer);
bool file_writer_is_open(FileWriter* writer);

size_t file_writer_get_position(FileWriter* writer);
size_t file_writer_write(void* buffer, size_t size, size_t count, FileWriter* writer);
bool file_writer_write_line(char* buffer, size_t size, FileWriter* writer);

#endif 