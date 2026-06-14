/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class FileWriter
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in file_writer.c (file-level convention).
 */

#ifndef FILE_WRITER_H_
#define FILE_WRITER_H_

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define FILE_WRITER_LOGGING_ENABLE */

#ifdef FILE_WRITER_LOGGING_ENABLE
    #define FILE_WRITER_LOG(fmt, ...) \
        do { fprintf(stderr, "[FILE_WRITER LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define FILE_WRITER_LOG(...) do { } while (0)
#endif


/* Cursor anchor used by `file_writer_seek` (and the matching reader). */
typedef enum {
    POS_BEGIN,
    POS_END,
    POS_CURRENT
} CursorPosition;


/* Wide-character encoding for `WRITE_UNICODE` mode. */
typedef enum {
    WRITE_ENCODING_UTF16,
    WRITE_ENCODING_UTF32
} WriteEncodingType;


/* File-open mode. */
typedef enum {
    WRITE_TEXT,          /* text mode                                          */
    WRITE_BINARY,        /* binary mode                                        */
    WRITE_UNICODE,       /* Unicode text (encoding conversion happens)         */
    WRITE_BUFFERED,      /* buffered writing                                   */
    WRITE_UNBUFFERED,    /* direct, unbuffered writing                         */
    WRITE_LINE,          /* line-by-line writing                               */
    WRITE_APPEND
} WriteMode;


typedef struct {
    FILE*             file_writer;
    WriteMode         mode;
    bool              is_open;
    WriteEncodingType encoding;
    char*             file_path;
} FileWriter;


/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */

FileWriter*   file_writer_open                 (const char* filename, const WriteMode mode);
FileWriter*   file_writer_append               (const char* filename, const WriteMode mode);


/* ------------------------------------------------------------------ */
/* Destruction / lifecycle                                            */
/* ------------------------------------------------------------------ */

bool          file_writer_close                (FileWriter* writer);
bool          file_writer_is_open              (FileWriter* writer);
bool          file_writer_flush                (FileWriter* writer);


/* ------------------------------------------------------------------ */
/* Writing                                                            */
/* ------------------------------------------------------------------ */

size_t        file_writer_write                (void* buffer, size_t size, size_t count, FileWriter* writer);
size_t        file_writer_write_fmt            (FileWriter* writer, const char* format, ...);
bool          file_writer_write_line           (char* buffer, size_t size, FileWriter* writer);
bool          file_writer_write_batch          (FileWriter* writer, const void** buffers, const size_t* sizes, size_t count);
bool          file_writer_append_fmt           (FileWriter* writer, const char* format, ...);
bool          file_writer_copy                 (FileWriter* src_writer, FileWriter* dest_writer);
bool          file_writer_write_all_atomic     (const char* path, const void* data, size_t size);


/* ------------------------------------------------------------------ */
/* Cursor / size                                                      */
/* ------------------------------------------------------------------ */

size_t        file_writer_get_position         (FileWriter* writer);
size_t        file_writer_get_size             (FileWriter* writer);
bool          file_writer_seek                 (FileWriter* writer, long offset, const CursorPosition cursor_pos);
bool          file_writer_truncate             (FileWriter* writer, size_t size);


/* ------------------------------------------------------------------ */
/* Locking                                                            */
/* ------------------------------------------------------------------ */

bool          file_writer_lock                 (FileWriter* writer);
bool          file_writer_unlock               (FileWriter* writer);


/* ------------------------------------------------------------------ */
/* Encoding                                                           */
/* ------------------------------------------------------------------ */

bool          file_writer_set_encoding         (FileWriter* writer, const WriteEncodingType encoding);
const char*   file_writer_get_encoding         (FileWriter* writer);


/* ------------------------------------------------------------------ */
/* Introspection                                                      */
/* ------------------------------------------------------------------ */

const char*   file_writer_get_file_name        (FileWriter* writer);


#ifdef __cplusplus
}
#endif

#endif 
