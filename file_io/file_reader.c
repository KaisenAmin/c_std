/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class FileReader
*/

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "file_reader.h"
#include "../string/std_string.h"
#include "../encoding/encoding.h"
#include <stdint.h>
#include <stdarg.h>
#include <wchar.h>




/* UTF-8 encoding of a code point; returns its length (1..4). */
static int fr_utf8_encode(uint32_t cp, char* out) {
    if (cp < 0x80) {
        out[0] = (char)cp;
        return 1;
    }
    if (cp < 0x800) {
        out[0] = (char)(0xC0 | (cp >> 6));
        out[1] = (char)(0x80 | (cp & 0x3F));
        return 2;
    }
    if (cp < 0x10000) {
        out[0] = (char)(0xE0 | (cp >> 12));
        out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
        out[2] = (char)(0x80 | (cp & 0x3F));
        return 3;
    }
    out[0] = (char)(0xF0 | (cp >> 18));
    out[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
    out[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
    out[3] = (char)(0x80 | (cp & 0x3F));
    return 4;
}

static int fr_utf8_len(uint32_t cp) {
    return cp < 0x80 ? 1 : cp < 0x800 ? 2 : cp < 0x10000 ? 3 : 4;
}

#if defined(_WIN32) || defined(_WIN64)

/* Next character as UTF-8 into out (room bytes). Returns its length, 0 at end
   of file or on error, -1 if it does not fit (it is left unread). */
static int fr_unicode_getc(FileReader* reader, char* out, size_t room) {
    FILE* f = reader->file_reader;
    wint_t u = fgetwc(f);
    if (u == WEOF) {
        return 0;
    }
    uint32_t cp = (uint32_t)u;
    if (u >= 0xD800 && u <= 0xDBFF) {
        if (room < 4) {
            ungetwc(u, f);
            return -1;
        }
        wint_t lo = fgetwc(f);
        if (lo != WEOF && lo >= 0xDC00 && lo <= 0xDFFF) {
            cp = 0x10000 + (((uint32_t)u - 0xD800) << 10) + ((uint32_t)lo - 0xDC00);
        }
        else {
            if (lo != WEOF) {
                ungetwc(lo, f);
            }
            cp = 0xFFFD;
        }
    }
    else if (u >= 0xDC00 && u <= 0xDFFF) {
        cp = 0xFFFD;
    }
    if ((size_t)fr_utf8_len(cp) > room) {
        ungetwc(u, f);
        return -1;
    }
    return fr_utf8_encode(cp, out);
}

#else

/* Detects the encoding at the start of the file and skips its BOM. */
static void fr_unicode_at_start(FileReader* reader) {
    FILE* f = reader->file_reader;
    unsigned char b[3];
    size_t n = fread(b, 1, 3, f);
    if (n == 3 && b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF) {
        reader->unicode_form = 1;
        return;
    }
    reader->unicode_form = 0;
    fseek(f, (n >= 2 && b[0] == 0xFF && b[1] == 0xFE) ? 2 : 0, SEEK_SET);
}

/* One code point at the current position, without CR LF folding; false at
   end of file. Invalid input decodes as U+FFFD. */
static bool fr_unicode_next_cp(FileReader* reader, uint32_t* cp) {
    FILE* f = reader->file_reader;
    if (reader->unicode_form == 1) {
        int c = fgetc(f);
        if (c == EOF) {
            return false;
        }
        int extra = (c < 0x80) ? 0 : ((c & 0xE0) == 0xC0) ? 1 : ((c & 0xF0) == 0xE0) ? 2 : ((c & 0xF8) == 0xF0) ? 3 : -1;
        if (extra < 0) {
            *cp = 0xFFFD;
            return true;
        }
        uint32_t v = (extra == 0) ? (uint32_t)c : (uint32_t)(c & (0x3F >> extra));
        for (int i = 0; i < extra; i++) {
            long at = ftell(f);
            int d = fgetc(f);
            if (d == EOF || (d & 0xC0) != 0x80) {
                if (d != EOF) {
                    fseek(f, at, SEEK_SET);
                }
                *cp = 0xFFFD;
                return true;
            }
            v = (v << 6) | (uint32_t)(d & 0x3F);
        }
        *cp = v;
        return true;
    }

    unsigned char b[2];
    if (fread(b, 1, 2, f) != 2) {
        return false;   /* end of file (an odd trailing byte is not a unit) */
    }
    uint32_t u = (uint32_t)(b[0] | (b[1] << 8));
    if (u >= 0xD800 && u <= 0xDBFF) {
        long at = ftell(f);
        if (fread(b, 1, 2, f) == 2) {
            uint32_t lo = (uint32_t)(b[0] | (b[1] << 8));
            if (lo >= 0xDC00 && lo <= 0xDFFF) {
                *cp = 0x10000 + ((u - 0xD800) << 10) + (lo - 0xDC00);
                return true;
            }
        }
        fseek(f, at, SEEK_SET);
        *cp = 0xFFFD;
        return true;
    }
    *cp = (u >= 0xDC00 && u <= 0xDFFF) ? 0xFFFD : u;
    return true;
}

static int fr_unicode_getc(FileReader* reader, char* out, size_t room) {
    FILE* f = reader->file_reader;
    long start = ftell(f);
    if (start == 0) {
        fr_unicode_at_start(reader);
    }
    long pos = ftell(f);

    uint32_t cp;
    if (!fr_unicode_next_cp(reader, &cp)) {
        return 0;
    }
    if (cp == '\r') {
        long afterCr = ftell(f);
        uint32_t next;
        if (fr_unicode_next_cp(reader, &next) && next == '\n') {
            cp = '\n';
        }
        else {
            fseek(f, afterCr, SEEK_SET);
        }
    }
    if ((size_t)fr_utf8_len(cp) > room) {
        fseek(f, pos, SEEK_SET);
        return -1;
    }
    return fr_utf8_encode(cp, out);
}

#endif

/* Reads one line of a READ_UNICODE stream as UTF-8 without its line break.
   Returns false only when nothing at all could be read (end of file or an
   error). A line longer than the buffer continues on the next call. */
static bool fr_unicode_read_line(FileReader* reader, char* buffer, size_t size) {
    size_t out = 0;
    bool any = false;
    while (out + 1 < size) {
        int n = fr_unicode_getc(reader, buffer + out, size - 1 - out);
        if (n <= 0) {
            break;
        }
        any = true;
        if (n == 1 && buffer[out] == '\n') {
            break;
        }
        out += (size_t)n;
    }
    buffer[out] = '\0';
    if (out > 0 && buffer[out - 1] == '\r') {
        buffer[--out] = '\0';
    }
    return any;
}



/**
 * @brief Opens a file for reading, based on the specified mode.
 *
 * This function opens a file for reading according to the specified `ReadMode`.
 * It handles various modes such as text, binary, Unicode, buffered, unbuffered, and line-by-line reading.
 * On Windows, it can open files with UTF-16 encoding for Unicode modes.
 *
 * @param filename The name of the file to open.
 * @param mode The mode in which the file is to be opened, specified by the `ReadMode` enum.
 * 
 * @return A pointer to a `FileReader` structure on success, or `NULL` on failure.
 */
FileReader* file_reader_open(const char* filename, const ReadMode mode) {
    if (!filename) {
        FILE_READER_LOG("[file_reader_open] Error: filename is null.");
        return NULL;
    }

    FileReader* reader = (FileReader*) malloc(sizeof(FileReader));
    if (!reader) {
        FILE_READER_LOG("[file_reader_open] Error: Cannot allocate memory for FileReader.");
        return NULL;
    }
    const char* modeStr = NULL;

    switch (mode) {
        case READ_TEXT:
            modeStr = "r";
            break;
        case READ_BINARY:
            modeStr = "rb";
            break;
        case READ_UNICODE:
            #if defined(_WIN32) || defined(_WIN64)
            modeStr = "r, ccs=UTF-16LE";
            #else
            modeStr = "rb";   /* decoded by fr_unicode_getc */
            #endif
            break;
        case READ_BUFFERED:
            modeStr = "r";
            break;
        case READ_UNBUFFERED:
            modeStr = "r";
            break;
        default:
            FILE_READER_LOG("[file_reader_open] Warning: Not a valid mode for reading, initializing default mode 'r'.");
            #if defined(_WIN32) || defined(_WIN64)
            modeStr = "r, ccs=UTF-16LE";
            #else
            modeStr = "r";
            #endif
            break;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wFileName = encoding_utf8_to_wchar(filename);
    wchar_t* wMode = encoding_utf8_to_wchar(modeStr);

    if (!wMode || !wFileName) {
        FILE_READER_LOG("[file_reader_open] Error: Cannot convert filename/mode to wchar.");
        free(wMode);
        free(wFileName);
        free(reader);
        return NULL;
    }
    reader->file_reader = _wfopen(wFileName, wMode);
    free(wMode);
    free(wFileName);
    #else
    reader->file_reader = fopen(filename, modeStr);
    #endif

    if (reader->file_reader == NULL) {
        FILE_READER_LOG("[file_reader_open] Error: Cannot open file.");
        free(reader);
        return NULL;
    }
    reader->mode = mode;
    reader->is_open = true;
    reader->encoding = READ_ENCODING_UTF16;
    reader->file_path = string_strdup(filename);
    reader->unicode_form = 0;

    FILE_READER_LOG("[file_reader_open] File '%s' opened successfully in mode '%s'.", filename, modeStr);
    return reader;
}


/**
 * @brief Closes the file associated with the given `FileReader`.
 *
 * This function closes the file associated with the `FileReader` structure.
 * It ensures that all resources are properly released.
 *
 * @param reader A pointer to the `FileReader` structure.
 * 
 * @return `true` if the file was successfully closed, `false` otherwise.
 */
bool file_reader_close(FileReader *reader) {
    if (!reader) {
        FILE_READER_LOG("[file_reader_close] Error: FileReader object is null.");
        return false;
    }
    if (reader->file_reader && fclose(reader->file_reader)) {
        FILE_READER_LOG("[file_reader_close] Error: Failed to close file.");
        return false;
    }
    if (reader->file_path) {
        free(reader->file_path);
        reader->file_path = NULL;
    }

    reader->is_open = false;
    free(reader); // Optionally free the FileReader object itself if it's dynamically allocated
    
    FILE_READER_LOG("[file_reader_close] File successfully closed.");
    return true;
}


/**
 * @brief Gets the current position of the file pointer.
 *
 * This function returns the current position of the file pointer in the file associated with the `FileReader`.
 *
 * @param reader A pointer to the `FileReader` structure.
 * 
 * @return The current position of the file pointer as a `size_t`. Returns `(size_t)-1` on error.
 */
size_t file_reader_get_position(FileReader *reader) {
    if (!reader || reader->file_reader == NULL) {
        FILE_READER_LOG("[file_reader_get_position] Error: FileReader object is null or invalid.");
        return (size_t)-1;
    }

    long cursor_position = ftell(reader->file_reader);
    if (cursor_position == -1L) {
        FILE_READER_LOG("[file_reader_get_position] Error: Could not determine file position.");
        return (size_t)-1;
    }

    FILE_READER_LOG("[file_reader_get_position] Current file position: %ld.", cursor_position);
    return (size_t)cursor_position;
}


/**
 * @brief Checks if the file is open.
 *
 * This function checks if the file associated with the `FileReader` structure is open.
 *
 * @param reader A pointer to the `FileReader` structure.
 * 
 * @return `true` if the file is open, `false` otherwise.
 */
bool file_reader_is_open(FileReader* reader) {
    if (!reader) {
        FILE_READER_LOG("[file_reader_is_open] Error: FileReader pointer is NULL.");
        return false;
    }
    if (reader->file_reader == NULL) {
        FILE_READER_LOG("[file_reader_is_open] Error: FileReader object is NULL and the file is not open.");
        return false;
    }

    FILE_READER_LOG("[file_reader_is_open] File is currently %s.", reader->is_open ? "open" : "closed");
    return reader->is_open;
}


/**
 * @brief Sets the encoding for reading the file.
 *
 * This function sets the encoding type for reading the file associated with the `FileReader`.
 *
 * @param reader A pointer to the `FileReader` structure.
 * @param encoding The encoding type to set, specified by the `ReadEncodingType` enum.
 * 
 * @return `true` if the encoding was successfully set, `false` otherwise.
 */
bool file_reader_set_encoding(FileReader* reader, const ReadEncodingType encoding) {
    if (!reader || reader->file_reader == NULL) {
        FILE_READER_LOG("[file_reader_set_encoding] Error: FileReader object is invalid or NULL.");
        return false;
    }
    if (!(encoding >= READ_ENCODING_UTF16 && encoding <= READ_ENCODING_UTF32)) {
        FILE_READER_LOG("[file_reader_set_encoding] Error: Invalid encoding type.");
        return false;
    }
    reader->encoding = encoding;

    FILE_READER_LOG("[file_reader_set_encoding] Encoding set to %d.", encoding);
    return true;
}


/**
 * @brief Retrieves the absolute path of the file associated with the `FileReader`.
 *
 * This function returns the absolute path of the file associated with the `FileReader` structure.
 *
 * @param reader A pointer to the `FileReader` structure.
 * 
 * @return The absolute path of the file as a constant string. Returns `NULL` on error.
 */
const char *file_reader_get_file_name(FileReader *reader) {
    if (!reader || reader->file_reader == NULL) {
        FILE_READER_LOG("[file_reader_get_file_name] Error: FileReader object is null or invalid.");
        return NULL;
    }
    if (!reader->file_path) {
        FILE_READER_LOG("[file_reader_get_file_name] Error: File path for FileReader is null.");
        return NULL;
    }

    FILE_READER_LOG("[file_reader_get_file_name] Returning file path: %s.", reader->file_path);
    return (const char*)reader->file_path;
}


/**
 * @brief Moves the file pointer to a specific location.
 *
 * This function moves the file pointer to a specific location in the file for random access reading.
 *
 * @param reader A pointer to the `FileReader` structure.
 * @param offset The offset from the position specified by `cursor_pos`.
 * @param cursor_pos The reference position from which to calculate the offset, specified by the `CursorPosition` enum.
 * 
 * @return `true` if the seek operation was successful, `false` otherwise.
 */
bool file_reader_seek(FileReader *reader, long offset, const CursorPosition cursor_pos) {
    if (!reader || reader->file_reader == NULL) {
        FILE_READER_LOG("[file_reader_seek] Error: FileReader object is null or invalid.");
        return false;
    }
    int pos;

    switch (cursor_pos) {
        case POS_BEGIN:
            pos = SEEK_SET;
            break;
        case POS_END:
            pos = SEEK_END;
            break;
        case POS_CURRENT:
            pos = SEEK_CUR;
            break;
        default:
            FILE_READER_LOG("[file_reader_seek] Warning: Invalid cursor position, defaulting to POS_BEGIN.");
            pos = SEEK_SET;
            break;
    }
    
    if (fseek(reader->file_reader, offset, pos) != 0) {
        FILE_READER_LOG("[file_reader_seek] Error: fseek failed.");
        return false;
    }

    FILE_READER_LOG("[file_reader_seek] File pointer moved to offset %ld with base %d.", offset, pos);
    return true;
}


/**
 * @brief Checks if the end of the file has been reached.
 *
 * This function checks if the end of the file associated with the `FileReader` structure has been reached.
 *
 * @param reader A pointer to the `FileReader` structure.
 * 
 * @return `true` if the end of the file has been reached, `false` otherwise.
 */
bool file_reader_eof(FileReader* reader) {
    if (!reader || reader->file_reader == NULL) {
        FILE_READER_LOG("[file_reader_eof] Error: FileReader object is NULL or invalid.");
        return false;
    }

    bool eof_reached = feof(reader->file_reader) != 0;
    FILE_READER_LOG("[file_reader_eof] End of file status: %s.", eof_reached ? "true" : "false");

    return eof_reached;
}


/**
 * @brief Gets the size of the file associated with the `FileReader`.
 *
 * This function returns the size of the file in bytes.
 *
 * @param reader A pointer to the `FileReader` structure.
 * 
 * @return The size of the file in bytes, or `0` on error.
 */
size_t file_reader_get_size(FileReader* reader) {
    if (!reader || reader->file_reader == NULL) {
        FILE_READER_LOG("[file_reader_get_size] Error: FileReader object is not valid or NULL.");
        return 0;
    }

    /* ftell / fseek work with long: read the position as long (a failed
       ftell used to turn into a size of SIZE_MAX through the size_t helper). */
    long current_position = ftell(reader->file_reader);
    if (current_position < 0) {
        FILE_READER_LOG("[file_reader_get_size] Error: ftell failed.");
        return 0;
    }
    if (fseek(reader->file_reader, 0, SEEK_END) != 0) {
        FILE_READER_LOG("[file_reader_get_size] Error: fseek failed to seek to end of file.");
        return 0;
    }

    long end = ftell(reader->file_reader);
    if (fseek(reader->file_reader, current_position, SEEK_SET) != 0) {
        FILE_READER_LOG("[file_reader_get_size] Error: fseek failed to return to original position.");
    }
    if (end < 0) {
        FILE_READER_LOG("[file_reader_get_size] Error: ftell failed at the end of the file.");
        return 0;
    }
    size_t size = (size_t)end;

    FILE_READER_LOG("[file_reader_get_size] File size is %zu bytes.", size);
    return size;
}


/**
 * @brief Reads data from the file into a buffer.
 *
 * This function reads data from the file associated with the `FileReader` into a specified buffer.
 *
 * @param buffer A pointer to the buffer where the data will be stored.
 * @param size The size of each element to read.
 * @param count The number of elements to read.
 * @param reader A pointer to the `FileReader` structure.
 * 
 * @return The total number of elements successfully read, or `0` on error.
 */
size_t file_reader_read(void* buffer, size_t size, size_t count, FileReader* reader) {
    if (!reader || !reader->file_reader || !buffer) {
        FILE_READER_LOG("[file_reader_read] Error: Invalid argument (FileReader or buffer is NULL).");
        return 0;
    }

    if (reader->mode == READ_BINARY || reader->mode == READ_UNBUFFERED || reader->mode == READ_BUFFERED) {
        size_t elements_read = fread(buffer, size, count, reader->file_reader);
        FILE_READER_LOG("[file_reader_read] Read %zu elements from binary or unbuffered file.", elements_read);
        return elements_read;
    }

    if (reader->mode == READ_TEXT) {
        size_t elements_read = fread(buffer, size, count, reader->file_reader);
        /* NUL-terminate at the byte offset (not the element index), and only
           inside the region the caller declared (size * count bytes): writing
           at buffer[elements_read] overflowed a full buffer by one byte and,
           for size > 1, clobbered data mid-buffer. A full read is therefore not
           terminated; pass count - 1 (as the examples do) to always get one. */
        if (size > 0 && elements_read < count) {
            ((char*)buffer)[elements_read * size] = '\0';
        }
        FILE_READER_LOG("[file_reader_read] Read %zu elements from text file.", elements_read);
        return elements_read;
    }

    /* Unicode: up to `count` characters, decoded to UTF-8 into the
       size * count byte buffer (NUL-terminated). Returns the number of UTF-8
       bytes stored. Characters that do not fit stay for the next call; the
       old code truncated the converted text (losing it) and, on POSIX, read
       4-byte wchar_t elements as UTF-16 units. */
    if (reader->mode == READ_UNICODE) {
        if (size == 0 || count == 0 || count > SIZE_MAX / size) {
            return 0;
        }
        size_t cap = size * count;
        char* out = (char*)buffer;
        size_t len = 0;
        for (size_t chars = 0; chars < count && len + 1 < cap; chars++) {
            int n = fr_unicode_getc(reader, out + len, cap - 1 - len);
            if (n <= 0) {
                break;
            }
            len += (size_t)n;
        }
        out[len] = '\0';
        FILE_READER_LOG("[file_reader_read] Read %zu UTF-8 bytes from a Unicode file.", len);
        return len;
    }

    FILE_READER_LOG("[file_reader_read] Error: Unsupported read mode.");
    return 0;
}


/**
 * @brief Read everything from the current position to EOF into one buffer.
 *
 * Allocates a buffer sized to the remaining bytes, reads them in a single pass,
 * and NUL-terminates the result (so it is safe to use as a C string when the
 * data is text). `*out_size` receives the actual number of bytes read, which can
 * be smaller than the on-disk byte count in text mode (the C runtime translates
 * CRLF to LF) — open the file with READ_BINARY for an exact byte count. The
 * buffer is heap-allocated; the caller frees it with free().
 *
 * @param reader   An open FileReader.
 * @param out_buf  Receives the malloc'd, NUL-terminated buffer. Must not be NULL.
 * @param out_size Optional; receives the number of bytes read. May be NULL.
 * @return true on success; false on invalid arguments, a seek/read error, or OOM.
 */
bool file_reader_read_all(FileReader* reader, char** out_buf, size_t* out_size) {
    if (out_size) {
        *out_size = 0;
    }
    if (!reader || !reader->file_reader || !out_buf) {
        FILE_READER_LOG("[file_reader_read_all] Error: invalid argument.");
        return false;
    }
    *out_buf = NULL;

    FILE* f = reader->file_reader;
    long start = ftell(f);
    if (start < 0) {
        FILE_READER_LOG("[file_reader_read_all] Error: ftell failed.");
        return false;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        FILE_READER_LOG("[file_reader_read_all] Error: fseek to end failed.");
        return false;
    }
    long end = ftell(f);
    if (end < 0 || fseek(f, start, SEEK_SET) != 0) {
        FILE_READER_LOG("[file_reader_read_all] Error: ftell/fseek-restore failed.");
        return false;
    }

    size_t remaining = (size_t)(end - start);
    char* buf = (char*)malloc(remaining + 1);
    if (!buf) {
        FILE_READER_LOG("[file_reader_read_all] Error: allocation failed.");
        return false;
    }

    size_t got = (remaining > 0) ? fread(buf, 1, remaining, f) : 0;
    if (got < remaining && ferror(f)) {
        FILE_READER_LOG("[file_reader_read_all] Error: fread failed.");
        free(buf);
        return false;
    }

    buf[got] = '\0';
    *out_buf = buf;
    if (out_size) {
        *out_size = got;
    }
    FILE_READER_LOG("[file_reader_read_all] Read %zu byte(s).", got);
    return true;
}


/**
 * @brief Reads a line of text from the file.
 *
 * This function reads a single line of text from the file associated with the `FileReader`.
 *
 * @param buffer A pointer to the buffer where the line will be stored.
 * @param size The maximum number of characters to read, including the null terminator.
 * @param reader A pointer to the `FileReader` structure.
 * 
 * @return `true` if a line was successfully read, `false` otherwise.
 */
bool file_reader_read_line(char* buffer, size_t size, FileReader* reader) {
    if (!reader || !reader->file_reader || !buffer) {
        FILE_READER_LOG("[file_reader_read_line] Error: Invalid argument.");
        return false;
    }
    if (size == 0) {
        return false;
    }
    if (reader->encoding == READ_ENCODING_UTF16 && reader->mode == READ_UNICODE) {
        /* Same result as the text path: the line without its line break
           (the Windows fgetws path used to keep the '\n'). */
        if (!fr_unicode_read_line(reader, buffer, size)) {
            FILE_READER_LOG("[file_reader_read_line] No more lines in UTF-16 mode.");
            return false;
        }
        FILE_READER_LOG("[file_reader_read_line] Line read successfully in UTF-16 mode.");
    }
    else {
        // For UTF-8 and other encodings
        /* fgets takes an int count; a larger buffer is simply used up to INT_MAX */
        if (fgets(buffer, (size > INT_MAX) ? INT_MAX : (int)size, reader->file_reader) == NULL) {
            if (!feof(reader->file_reader)) {
                FILE_READER_LOG("[file_reader_read_line] Error: Failed to read line in non-UTF-16 mode.");
            }
            return false;
        }
        buffer[strcspn(buffer, "\r\n")] = '\0';
        FILE_READER_LOG("[file_reader_read_line] Line read successfully.");
    }

    return true;
}


/**
 * @brief Reads formatted data from the file.
 *
 * This function reads formatted data from the file associated with the `FileReader`.
 *
 * @param reader A pointer to the `FileReader` structure.
 * @param format The format string specifying how to interpret the data.
 * 
 * @return The number of items successfully read, or `0` on error.
 */
size_t file_reader_read_fmt(FileReader* reader, const char* format, ...) {
    if (!reader || !reader->file_reader || !format) {
        FILE_READER_LOG("[file_reader_read_fmt] Error: Invalid argument.");
        return 0;
    }

    /* One line through file_reader_read_line, so the stream is decoded the
       same way as there: a READ_TEXT stream is UTF-8 bytes as is (fgetws
       used to widen each byte and re-encode it as Latin-1: mojibake). */
    char line[4096];
    if (!file_reader_read_line(line, sizeof(line), reader)) {
        FILE_READER_LOG("[file_reader_read_fmt] Error: Failed to read formatted data.");
        return 0;
    }

    va_list args;
    va_start(args, format);
    int matched = vsscanf(line, format, args);
    va_end(args);

    /* vsscanf returns EOF (-1) when nothing could be converted; as size_t
       that became SIZE_MAX. The documented result is 0. */
    if (matched < 0) {
        matched = 0;
    }
    FILE_READER_LOG("[file_reader_read_fmt] Read %d formatted items successfully.", matched);
    return (size_t)matched;
}


/**
 * @brief Copies the contents of one file to another.
 *
 * This function copies the contents of the source file associated with the `FileReader` to the destination file associated with the `FileWriter`.
 *
 * @param src_reader A pointer to the `FileReader` structure for the source file.
 * @param dest_writer A pointer to the `FileWriter` structure for the destination file.
 * 
 * @return `true` if the copy operation was successful, `false` otherwise.
 */
bool file_reader_copy(FileReader* src_reader, FileWriter* dest_writer) {
    if (!src_reader || !src_reader->file_reader || !dest_writer || !dest_writer->file_writer) {
        FILE_READER_LOG("[file_reader_copy] Error: Invalid argument (FileReader or FileWriter is NULL).");
        return false;
    }

    /* Binary fast-path: byte-for-byte copy with no encoding conversion. */
    if (src_reader->mode == READ_BINARY ||
        dest_writer->mode == WRITE_BINARY) {
        unsigned char buf[4096];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), src_reader->file_reader)) > 0) {
            size_t w = fwrite(buf, 1, n, dest_writer->file_writer);
            if (w != n) {
                FILE_READER_LOG("[file_reader_copy] Error: Short write in binary copy.");
                return false;
            }
        }
        if (!feof(src_reader->file_reader)) {
            FILE_READER_LOG("[file_reader_copy] Error: Read error in binary copy.");
            return false;
        }
        FILE_READER_LOG("[file_reader_copy] Binary copy completed.");
        return true;
    }

    if (src_reader->mode != READ_UNICODE) {
        /* Text source: the bytes already are the (UTF-8/ASCII) text. They used
           to be reinterpreted as UTF-16 code units, which turned "Hello" into
           CJK mojibake. file_writer_write applies the destination's own mode
           (e.g. UTF-16 output for WRITE_UNICODE). */
        char buf[4096];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), src_reader->file_reader)) > 0) {
            if (file_writer_write(buf, 1, n, dest_writer) < n) {
                FILE_READER_LOG("[file_reader_copy] Error: Could not write all data to the destination file.");
                return false;
            }
        }
        if (!feof(src_reader->file_reader)) {
            FILE_READER_LOG("[file_reader_copy] Error: Read error in text copy.");
            return false;
        }
        FILE_READER_LOG("[file_reader_copy] Text copy completed.");
        return true;
    }

    /* UTF-16 (little-endian) source -> UTF-8. Read raw bytes and convert an
       explicit number of 16-bit units: the old loop handed an unterminated
       wchar_t buffer to a NUL-terminated converter (stack over-read, stale
       data in the output), used wchar_t (4 bytes on POSIX) as the unit, and
       wrote a code-point count as a byte count (truncating non-ASCII text).
       An odd trailing byte or a high surrogate at the end of a chunk is
       carried over to the next chunk. A leading byte-order mark is skipped. */
    unsigned char raw[4096 + 4];
    size_t carry = 0;
    bool first = true;

    for (;;) {
        size_t n = fread(raw + carry, 1, 4096, src_reader->file_reader);
        size_t avail = carry + n;
        size_t start = 0;
        if (first && avail >= 2) {
            if (raw[0] == 0xFF && raw[1] == 0xFE) {
                start = 2;
            }
            first = false;
        }

        size_t units = (avail - start) / 2;
        bool eof = (n == 0);
        if (units > 0 && !eof) {
            uint16_t lastUnit = (uint16_t)(raw[start + 2 * (units - 1)] | (raw[start + 2 * (units - 1) + 1] << 8));
            if (lastUnit >= 0xD800 && lastUnit <= 0xDBFF) {
                units--; /* keep the high surrogate for its low half */
            }
        }

        if (units > 0) {
            uint16_t* codeUnits = (uint16_t*)malloc(units * sizeof(uint16_t));
            if (!codeUnits) {
                FILE_READER_LOG("[file_reader_copy] Error: Memory allocation failed.");
                return false;
            }
            for (size_t i = 0; i < units; ++i) {
                codeUnits[i] = (uint16_t)(raw[start + 2 * i] | (raw[start + 2 * i + 1] << 8));
            }
            char* utf8Buffer = (char*)encoding_utf16_to_utf8(codeUnits, units);
            free(codeUnits);
            if (!utf8Buffer) {
                FILE_READER_LOG("[file_reader_copy] Error: Conversion to UTF-8 failed.");
                return false;
            }
            size_t bytesToWrite = strlen(utf8Buffer);   /* bytes, not code points */
            size_t bytesWritten = file_writer_write(utf8Buffer, sizeof(char), bytesToWrite, dest_writer);
            free(utf8Buffer);
            if (bytesWritten < bytesToWrite) {
                FILE_READER_LOG("[file_reader_copy] Error: Could not write all data to the destination file.");
                return false;
            }
            FILE_READER_LOG("[file_reader_copy] Successfully copied %zu bytes.", bytesWritten);
        }

        size_t consumed = start + units * 2;
        carry = avail - consumed;
        if (carry) {
            memmove(raw, raw + consumed, carry);
        }
        if (eof) {
            break;
        }
    }

    if (!feof(src_reader->file_reader)) {
        FILE_READER_LOG("[file_reader_copy] Error: Read error during copy operation.");
        return false;
    }
    if (carry) {
        FILE_READER_LOG("[file_reader_copy] Warning: %zu trailing byte(s) did not form a complete UTF-16 character.", carry);
    }
    FILE_READER_LOG("[file_reader_copy] File copy operation completed successfully.");
    return true;
}


/**
 * @brief Reads multiple lines of text from the file.
 *
 * This function reads a specified number of lines from the file associated with the `FileReader`.
 *
 * @param reader A pointer to the `FileReader` structure.
 * @param buffer A pointer to an array of strings where the lines will be stored.
 * @param num_lines The number of lines to read.
 * 
 * @return `true` if the lines were successfully read, `false` otherwise.
 */
bool file_reader_read_lines(FileReader* reader, char*** buffer, size_t num_lines) {
    if (!reader || !reader->file_reader || !buffer) {
        FILE_READER_LOG("[file_reader_read_lines] Error: Invalid arguments (FileReader or buffer is NULL).");
        return false;
    }

    *buffer = NULL;
    if (num_lines == 0 || num_lines > SIZE_MAX / sizeof(char*)) {
        return false;
    }
    *buffer = (char**) calloc(num_lines, sizeof(char*));
    if (!*buffer) {
        FILE_READER_LOG("[file_reader_read_lines] Error: Memory allocation failed for buffer.");
        return false;
    }

    size_t lines_read = 0;
    char line_buffer[1024]; 

    while (lines_read < num_lines && !feof(reader->file_reader)) {
        if (file_reader_read_line(line_buffer, sizeof(line_buffer), reader)) {
            (*buffer)[lines_read] = string_strdup(line_buffer);
            if (!(*buffer)[lines_read]) {
                FILE_READER_LOG("[file_reader_read_lines] Error: Memory allocation failed for line %zu.", lines_read);
                // Free previously allocated lines
                for (size_t i = 0; i < lines_read; ++i) {
                    free((*buffer)[i]);
                }
                free(*buffer);
                *buffer = NULL;
                return false;
            }
            FILE_READER_LOG("[file_reader_read_lines] Successfully read line %zu: %s", lines_read, line_buffer);
            lines_read++;
        } 
        else {
            /* End of file or a read error (ferror set, feof clear): stop.
               Retrying the same failing read spun forever at 100% CPU. */
            FILE_READER_LOG("[file_reader_read_lines] Warning: Failed to read line %zu.", lines_read);
            break;
        }
    }

    if (lines_read == num_lines) {
        FILE_READER_LOG("[file_reader_read_lines] Successfully read all %zu lines.", num_lines);
        return true;
    }

    /* Short read: release everything and hand back NULL. The partial array
       used to leak, with its unread slots uninitialized. */
    FILE_READER_LOG("[file_reader_read_lines] Warning: Only %zu lines were read out of %zu.", lines_read, num_lines);
    for (size_t i = 0; i < lines_read; ++i) {
        free((*buffer)[i]);
    }
    free(*buffer);
    *buffer = NULL;
    return false;
}
