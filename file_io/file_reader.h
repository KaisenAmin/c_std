/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class FileReader
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in file_reader.c (file-level convention).
 */

#ifndef FILE_READER_H_
#define FILE_READER_H_

#include "file_writer.h"   

#ifdef __cplusplus
extern "C" {
#endif


/* #define FILE_READER_LOGGING_ENABLE */

#ifdef FILE_READER_LOGGING_ENABLE
    #define FILE_READER_LOG(fmt, ...) \
        do { fprintf(stderr, "[FILE_READER LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define FILE_READER_LOG(...) do { } while (0)
#endif


/* File-open mode. */
typedef enum {
    READ_TEXT,          /* text mode                                          */
    READ_BINARY,        /* binary mode                                        */
    READ_UNICODE,       /* Unicode text (encoding conversion happens)         */
    READ_BUFFERED,      /* buffered reading                                   */
    READ_UNBUFFERED,    /* direct, unbuffered reading                         */
    READ_LINE           /* line-by-line reading                               */
} ReadMode;


/* Wide-character encoding for `READ_UNICODE` mode. */
typedef enum {
    READ_ENCODING_UTF16,
    READ_ENCODING_UTF32
} ReadEncodingType;


typedef struct {
    FILE*            file_reader;
    ReadMode         mode;
    bool             is_open;
    ReadEncodingType encoding;
    char*            file_path;
} FileReader;


/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */

FileReader*   file_reader_open                 (const char* filename, const ReadMode mode);


/* ------------------------------------------------------------------ */
/* Destruction / lifecycle                                            */
/* ------------------------------------------------------------------ */

bool          file_reader_close                (FileReader* reader);
bool          file_reader_is_open              (FileReader* reader);
bool          file_reader_eof                  (FileReader* reader);


/* ------------------------------------------------------------------ */
/* Reading                                                            */
/* ------------------------------------------------------------------ */

size_t        file_reader_read                 (void* buffer, size_t size, size_t count, FileReader* reader);
size_t        file_reader_read_fmt             (FileReader* reader, const char* format, ...);
bool          file_reader_read_line            (char* buffer, size_t size, FileReader* reader);
bool          file_reader_read_lines           (FileReader* reader, char*** buffer, size_t num_lines);
bool          file_reader_copy                 (FileReader* src_reader, FileWriter* dest_writer);


/* ------------------------------------------------------------------ */
/* Cursor / size                                                      */
/* ------------------------------------------------------------------ */

size_t        file_reader_get_position         (FileReader* reader);
size_t        file_reader_get_size             (FileReader* reader);
bool          file_reader_seek                 (FileReader* reader, long offset, const CursorPosition cursor_pos);


/* ------------------------------------------------------------------ */
/* Encoding                                                           */
/* ------------------------------------------------------------------ */

bool          file_reader_set_encoding         (FileReader* reader, const ReadEncodingType encoding);


/* ------------------------------------------------------------------ */
/* Introspection                                                      */
/* ------------------------------------------------------------------ */

const char*   file_reader_get_file_name        (FileReader* reader);


#ifdef __cplusplus
}
#endif

#endif 
