/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class FileWriter
*/

#if defined(_WIN32) || defined(_WIN64) 
#include <windows.h>
#include <io.h> // _get_osfhandle, _setmode
#include <fcntl.h> // _O_BINARY
#else
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif 

#include <stdlib.h>
#include <string.h>
#include "file_writer.h"
#include "../encoding/encoding.h"
#include "../string/std_string.h"
#include <limits.h>
#include <stdarg.h>


/*
 * Writes `len` bytes of UTF-8 text through the stream's encoding. On Windows
 * the WRITE_UNICODE and WRITE_APPEND streams are opened with "ccs=UTF-8": the
 * CRT then expects wide characters (and stores them as UTF-8 itself), so raw
 * UTF-8 bytes passed to fwrite came out as CJK mojibake or were dropped. The
 * text is converted with its explicit length (it need not be NUL-terminated).
 * Returns true when all of it was written.
 */
static bool fw_write_utf8(FileWriter* writer, const char* text, size_t len) {
    if (len == 0) {
        return true;
    }
#if defined(_WIN32) || defined(_WIN64)
    if (writer->mode == WRITE_UNICODE || writer->mode == WRITE_APPEND) {
        if (len > (size_t)INT_MAX) {
            return false;
        }
        int wlen = MultiByteToWideChar(CP_UTF8, 0, text, (int)len, NULL, 0);
        if (wlen <= 0) {
            return false;
        }
        wchar_t* wide = (wchar_t*)malloc((size_t)wlen * sizeof(wchar_t));
        if (!wide) {
            return false;
        }
        MultiByteToWideChar(CP_UTF8, 0, text, (int)len, wide, wlen);
        size_t n = fwrite(wide, sizeof(wchar_t), (size_t)wlen, writer->file_writer);
        free(wide);
        return n == (size_t)wlen;
    }
#else
    /* POSIX WRITE_UNICODE files are UTF-16LE (see file_writer_write): the
       line/format writers used to put raw UTF-8 into the same file. */
    if (writer->mode == WRITE_UNICODE && writer->encoding == WRITE_ENCODING_UTF16) {
        uint16_t* units = encoding_utf8_to_utf16((const uint8_t*)text, len);
        if (!units) {
            return false;
        }
        size_t count = 0;
        while (units[count] != 0) {
            count++;
        }
        size_t n = fwrite(units, sizeof(uint16_t), count, writer->file_writer);
        free(units);
        return n == count;
    }
#endif
    return fwrite(text, 1, len, writer->file_writer) == len;
}

/* vsnprintf into a heap buffer of exactly the needed size (no 2048-byte cap,
   which silently truncated long lines). Caller frees; *outLen gets the length. */
static char* fw_vformat(const char* format, va_list args, size_t* outLen) {
    va_list copy;
    va_copy(copy, args);
    int needed = vsnprintf(NULL, 0, format, copy);
    va_end(copy);
    if (needed < 0) {
        return NULL;
    }
    char* buf = (char*)malloc((size_t)needed + 1);
    if (!buf) {
        return NULL;
    }
    vsnprintf(buf, (size_t)needed + 1, format, args);
    *outLen = (size_t)needed;
    return buf;
}


/**
 * @brief Opens a file for writing, based on the specified mode.
 *
 * This function opens a file for writing according to the specified `WriteMode`.
 * It handles various modes such as text, binary, Unicode, buffered, unbuffered, and append.
 * On Windows, it can open files with UTF-8 encoding for Unicode modes.
 *
 * @param filename The name of the file to open.
 * @param mode The mode in which the file is to be opened, specified by the `WriteMode` enum.
 * 
 * @return A pointer to a `FileWriter` structure on success, or `NULL` on failure.
 */
FileWriter* file_writer_open(const char* filename, const WriteMode mode) {
    if (!filename) {
        FILE_WRITER_LOG("[file_writer_open] Error: filename is null.");
        return NULL;
    }

    FileWriter* writer = (FileWriter*) malloc(sizeof(FileWriter));
    if (!writer) {
        FILE_WRITER_LOG("[file_writer_open] Error: Cannot allocate memory for FileWriter.");
        return NULL;
    }

    const char* modeStr = NULL;
    switch (mode) {
    case WRITE_TEXT:
        modeStr = "w";
        break;
    case WRITE_BINARY:
        modeStr = "wb";
        break;
    case WRITE_UNICODE:
        #if defined(_WIN32) || defined(_WIN64)
        modeStr = "w, ccs=UTF-8";
        #else
        modeStr = "w";
        #endif 
        break;
    case WRITE_BUFFERED:
        modeStr = "w";
        break;
    case WRITE_UNBUFFERED:
        modeStr = "w";
        break;
    case WRITE_APPEND:
        #if defined(_WIN32) || defined(_WIN64)
        modeStr = "a, ccs=UTF-8";
        #else 
        modeStr = "a";
        #endif 
        break;
    default:
        FILE_WRITER_LOG("[file_writer_open] Warning: Invalid mode for writing, defaulting to 'w'.");
        #if defined(_WIN32) || defined(_WIN64)
        modeStr = "w, ccs=UTF-8";
        #else
        modeStr = "w";
        #endif 
        break;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wFileName = encoding_utf8_to_wchar(filename);
    wchar_t* wMode = encoding_utf8_to_wchar(modeStr);

    if (!wMode || !wFileName) {
        FILE_WRITER_LOG("[file_writer_open] Error: Cannot convert filename/mode to wchar.");
        free(wMode);
        free(wFileName);
        free(writer);
        return NULL;
    }
    writer->file_writer = _wfopen(wFileName, wMode);
    free(wMode);
    free(wFileName);
    #else
    writer->file_writer = fopen(filename, modeStr);
    #endif

    if (writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_open] Error: Cannot open file.");
        free(writer);
        return NULL;
    }
    writer->mode = mode;
    writer->is_open = true;
    writer->encoding = WRITE_ENCODING_UTF16;
    writer->file_path = string_strdup(filename);

    FILE_WRITER_LOG("[file_writer_open] File '%s' opened successfully in mode '%s'.", filename, modeStr);
    return writer;
}


/**
 * @brief Opens an existing file for appending. If the file does not exist, it will be created.
 *
 * This function opens a file for appending according to the specified `WriteMode`.
 * If the file does not exist, it will create a new file.
 * On Windows, it can open files with UTF-8 encoding for Unicode modes.
 *
 * @param filename The name of the file to open.
 * @param mode The mode in which the file is to be opened, specified by the `WriteMode` enum.
 * 
 * @return A pointer to a `FileWriter` structure on success, or `NULL` on failure.
 */
FileWriter* file_writer_append(const char* filename, const WriteMode mode) {
    if (!filename) {
        FILE_WRITER_LOG("[file_writer_append] Error: filename is null.");
        return NULL;
    }

    FileWriter* writer = (FileWriter*) malloc(sizeof(FileWriter));
    if (!writer) {
        FILE_WRITER_LOG("[file_writer_append] Error: Cannot allocate memory for FileWriter.");
        return NULL;
    }

    const char* modeStr = NULL;
    switch (mode) {
    case WRITE_TEXT:
        modeStr = "a";
        break;
    case WRITE_BINARY:
        modeStr = "ab";
        break;
    case WRITE_UNICODE:
        #if defined(_WIN32) || defined(_WIN64)
        modeStr = "a, ccs=UTF-8";
        #else
        modeStr = "a";
        #endif 
        break;
    case WRITE_BUFFERED:
        modeStr = "a";
        break;
    case WRITE_UNBUFFERED:
        modeStr = "a";
        break;
    case WRITE_APPEND:
        #if defined(_WIN32) || defined(_WIN64)
        modeStr = "a, ccs=UTF-8";
        #else 
        modeStr = "a";
        #endif 
        break;
    default:
        FILE_WRITER_LOG("[file_writer_append] Warning: Invalid mode for writing, defaulting to 'a'.");
        modeStr = "a";
        break;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wFileName = encoding_utf8_to_wchar(filename);
    wchar_t* wMode = encoding_utf8_to_wchar(modeStr);

    if (!wMode) {
        FILE_WRITER_LOG("[file_writer_append] Error: Cannot convert mode to wchar.");
        free(writer);
        return NULL;
    }
    if (!wFileName) {
        FILE_WRITER_LOG("[file_writer_append] Error: Cannot convert filename to wchar.");
        free(writer);
        free(wMode);
        return NULL;
    }
    writer->file_writer = _wfopen(wFileName, wMode);
    free(wMode);
    free(wFileName);
    #else 
    writer->file_writer = fopen(filename, modeStr);
    #endif 

    if (writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_append] Error: Cannot open file.");
        free(writer);
        return NULL;
    }

    writer->mode = mode;
    writer->is_open = true;
    writer->encoding = WRITE_ENCODING_UTF16;
    writer->file_path = string_strdup(filename);

    FILE_WRITER_LOG("[file_writer_append] File '%s' opened for appending in mode '%s'.", filename, modeStr);
    return writer;
}


/**
 * @brief Closes the file associated with the given `FileWriter`.
 *
 * This function closes the file associated with the `FileWriter` structure.
 * It ensures that all data is flushed to the file before closing.
 *
 * @param writer A pointer to the `FileWriter` structure.
 * 
 * @return `true` if the file was successfully closed, `false` otherwise.
 */
bool file_writer_close(FileWriter *writer) {
    if (!writer) {
        FILE_WRITER_LOG("[file_writer_close] Error: FileWriter object is null.");
        return false;
    }
    if (writer->file_writer && fclose(writer->file_writer)) {
        FILE_WRITER_LOG("[file_writer_close] Error: Failed to close the file.");
        return false;
    }
    writer->file_writer = NULL;
    writer->is_open = false;

    if (writer->file_path) {
        free(writer->file_path);
        writer->file_path = NULL;
    }

    FILE_WRITER_LOG("[file_writer_close] File successfully closed.");
    free(writer);
    return true;
}


/**
 * @brief Gets the current position of the file pointer.
 *
 * This function returns the current position of the file pointer in the file associated with the `FileWriter`.
 *
 * @param writer A pointer to the `FileWriter` structure.
 * 
 * @return The current position of the file pointer as a `size_t`. Returns `(size_t)-1` on error.
 */
size_t file_writer_get_position(FileWriter *writer) {
    if (writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_get_position] Error: FileWriter object is NULL and invalid.");
        return (size_t)-1;
    }

    long cursor_position = ftell(writer->file_writer);
    if (cursor_position == -1L) {
        FILE_WRITER_LOG("[file_writer_get_position] Error: Could not determine file position.");
        return (size_t)-1;
    }

    FILE_WRITER_LOG("[file_writer_get_position] Current file position: %ld.", cursor_position);
    return (size_t)cursor_position;
}


/**
 * @brief Writes data from a buffer to a file using the specified `FileWriter`.
 *
 * This function handles writing data to a file, with special handling for binary, UTF-16, and UTF-32 encodings.
 * If the file is opened in binary mode, the data is written directly without conversion.
 * If the file is opened in a Unicode mode, the data is converted to the appropriate encoding before writing.
 *
 * @param buffer The data buffer to write from.
 * @param size The size of each element to be written.
 * @param count The number of elements to write.
 * @param writer A pointer to the `FileWriter` structure.
 * 
 * @return The number of elements successfully written.
 */
size_t file_writer_write(void *buffer, size_t size, size_t count, FileWriter *writer) {
    if (!writer || !writer->file_writer || !buffer) {
        FILE_WRITER_LOG("[file_writer_write] Error: Invalid argument (FileWriter or buffer is NULL).");
        return 0;
    }
    if (writer->mode == WRITE_BINARY) {
        FILE_WRITER_LOG("[file_writer_write] Writing binary data.");
        return fwrite(buffer, size, count, writer->file_writer);
    }

    size_t written = 0;

    // Handle text and Unicode data with conversion if necessary
    switch (writer->encoding) {
        case WRITE_ENCODING_UTF32: {
            if (size != 0 && count > SIZE_MAX / size) {
                return 0;
            }
            uint32_t* utf32Buffer = encoding_utf8_to_utf32((const uint8_t*)buffer, size * count);
            if (!utf32Buffer) {
                FILE_WRITER_LOG("[file_writer_write] Error: Conversion to UTF-32 failed.");
                return 0;
            }
            /* Count uint32_t units: wcslen on this buffer read 16-bit units on
               Windows and stopped inside the first character. */
            size_t utf32Len = 0;
            while (utf32Buffer[utf32Len] != 0) {
                utf32Len++;
            }
            size_t units = fwrite(utf32Buffer, sizeof(uint32_t), utf32Len, writer->file_writer);
            free(utf32Buffer);
            /* Like every other path: the caller's element count on success. */
            written = (units == utf32Len) ? count : 0;

            FILE_WRITER_LOG("[file_writer_write] Successfully wrote %zu UTF-32 elements.", written);
            break;
        }

        case WRITE_ENCODING_UTF16: {
            #if defined(_WIN32) || defined(_WIN64)
            // ccs=UTF-8 streams (WRITE_UNICODE / WRITE_APPEND) take wide characters.
            // Convert exactly size * count bytes (the buffer need not be
            // NUL-terminated) and report the caller's element count on success.
            if (writer->mode == WRITE_UNICODE || writer->mode == WRITE_APPEND) {
                if (size != 0 && count > SIZE_MAX / size) {
                    return 0;
                }
                written = fw_write_utf8(writer, (const char*)buffer, size * count) ? count : 0;
                FILE_WRITER_LOG("[file_writer_write] Wrote %zu elements through the UTF-8 stream.", written);
            }  
            else {
                // For non-Unicode modes, write directly
                written = fwrite(buffer, size, count, writer->file_writer);
                FILE_WRITER_LOG("[file_writer_write] Successfully wrote %zu elements without conversion.", written);
            }
            #else
            // For non-Windows systems, convert UTF-8 to UTF-16 ONLY for the explicit
            // Unicode mode. WRITE_TEXT and WRITE_APPEND must write bytes as-is so plain
            // text round-trips (e.g. json write/read, and write-then-append of text).
            // On Windows these modes use an "ccs=UTF-8" stream, i.e. UTF-8 on disk too,
            // so this keeps the on-disk bytes consistent across platforms.
            if (writer->mode == WRITE_UNICODE) {
                uint16_t* utf16Buffer = encoding_utf8_to_utf16((const uint8_t*)buffer, size * count);
                if (!utf16Buffer) {
                    FILE_WRITER_LOG("[file_writer_write] Error: Conversion to UTF-16 failed.");
                    return 0;
                }
                /* utf16Buffer is a NUL-terminated uint16_t array. Count its units
                   directly -- wcslen() must NOT be used here, because wchar_t is
                   4 bytes on Linux (not uint16_t), so it reads past the buffer. */
                size_t utf16Len = 0;
                while (utf16Buffer[utf16Len] != 0) {
                    utf16Len++;
                }
                size_t units = fwrite(utf16Buffer, sizeof(uint16_t), utf16Len, writer->file_writer);
                free(utf16Buffer);
                /* The caller's element count on success (it returned UTF-16
                   units, unlike the Windows and binary paths). */
                written = (units == utf16Len) ? count : 0;

                FILE_WRITER_LOG("[file_writer_write] Successfully wrote %zu UTF-16 elements.", written);
            } 
            else {
                // For non-Unicode modes, write directly
                written = fwrite(buffer, size, count, writer->file_writer);
                FILE_WRITER_LOG("[file_writer_write] Successfully wrote %zu elements without conversion.", written);
            }
            #endif
            break;
        }

        default:
            written = fwrite(buffer, size, count, writer->file_writer);
            FILE_WRITER_LOG("[file_writer_write] Writing %zu elements with default handling.", written);
            break;
    }

    return written;
}


/**
 * @brief Writes a line of text to the file using the specified `FileWriter`.
 *
 * This function writes a line of text to the file, with special handling for Unicode text.
 * It adds a newline character at the end of the line.
 *
 * @param buffer The buffer containing the line of text to write.
 * @param size The size of the text line in bytes.
 * @param writer A pointer to the `FileWriter` structure.
 * 
 * @return `true` if the line is successfully written, `false` otherwise.
 */
bool file_writer_write_line(char *buffer, size_t size, FileWriter *writer) {
    if (!writer || writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_write_line] Error: FileWriter object is NULL or invalid.");
        return false;
    }
    if (buffer == NULL) {
        FILE_WRITER_LOG("[file_writer_write_line] Error: Buffer is NULL.");
        return false;
    }

    /* fw_write_utf8 routes the text through the stream's encoding (WRITE_APPEND
       is a ccs=UTF-8 stream on Windows too, and used to get raw bytes). */
    if (!fw_write_utf8(writer, buffer, size)) {
        FILE_WRITER_LOG("[file_writer_write_line] Error: Could not write the entire buffer.");
        return false;
    }
    if (!fw_write_utf8(writer, "\n", 1)) {
        FILE_WRITER_LOG("[file_writer_write_line] Error: Could not write the newline.");
        return false;
    }
    FILE_WRITER_LOG("[file_writer_write_line] Successfully wrote %zu bytes and a newline.", size);
    return true;
}


/**
 * @brief Checks if the file associated with the `FileWriter` is currently open.
 *
 * This function verifies whether the file managed by the `FileWriter` is open for writing.
 *
 * @param writer A pointer to the `FileWriter` structure.
 * 
 * @return `true` if the file is open, `false` otherwise.
 */
bool file_writer_is_open(FileWriter* writer) {
    if (!writer) {
        FILE_WRITER_LOG("[file_writer_is_open] Error: FileWriter pointer is NULL.");
        return false;
    }
    if (writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_is_open] Error: FileWriter object is NULL, file is not open.");
        return false;
    }
    FILE_WRITER_LOG("[file_writer_is_open] File is currently open.");
    return writer->is_open;
}


/**
 * @brief Flushes the buffer of the `FileWriter`, ensuring all written data is stored in the file.
 *
 * This function flushes any buffered data, forcing it to be physically written to the file.
 * It is applicable for both text and binary modes and works correctly with Unicode text.
 *
 * @param writer A pointer to the `FileWriter` structure.
 * 
 * @return `true` if the flush operation succeeds, `false` otherwise.
 */
bool file_writer_flush(FileWriter* writer) {
    if (!writer || writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_flush] Error: FileWriter object is NULL or invalid.");
        return false;
    }
    if (fflush(writer->file_writer) == EOF) {
        FILE_WRITER_LOG("[file_writer_flush] Error: Failed to flush the writer.");
        return false;
    }

    FILE_WRITER_LOG("[file_writer_flush] File flushed successfully.");
    return true;
}


/**
 * @brief Sets the character encoding for writing to the file.
 *
 * This function configures the `FileWriter` to use the specified encoding type for writing data.
 *
 * @param writer A pointer to the `FileWriter` structure.
 * @param encoding The `WriteEncodingType` to be set (e.g., UTF-16, UTF-32).
 * 
 * @return `true` if the encoding type is successfully set, `false` otherwise.
 */
bool file_writer_set_encoding(FileWriter* writer, const WriteEncodingType encoding) {
    if (!writer || writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_set_encoding] Error: FileWriter object is NULL or invalid.");
        return false;
    }
    if (!(encoding >= WRITE_ENCODING_UTF16 && encoding <= WRITE_ENCODING_UTF32)) {
        FILE_WRITER_LOG("[file_writer_set_encoding] Error: Invalid encoding type.");
        return false;
    }
    writer->encoding = encoding;

    FILE_WRITER_LOG("[file_writer_set_encoding] Encoding set to %d.", encoding);
    return true;
}


/**
 * @brief Copies the content from one file to another.
 *
 * This function copies the entire content of the file managed by the `src_writer` to the file managed by the `dest_writer`.
 *
 * @param src_writer A pointer to the source `FileWriter` structure.
 * @param dest_writer A pointer to the destination `FileWriter` structure.
 * 
 * @return `true` if the file content is successfully copied, `false` otherwise.
 */
bool file_writer_copy(FileWriter *src_writer, FileWriter *dest_writer) {
    if (!src_writer || src_writer->file_writer == NULL || src_writer->file_path == NULL) {
        FILE_WRITER_LOG("[file_writer_copy] Error: src_writer object or file path is NULL or invalid.");
        return false;
    }
    if (!dest_writer || dest_writer->file_writer == NULL || dest_writer->file_path == NULL) {
        FILE_WRITER_LOG("[file_writer_copy] Error: dest_writer object or file path is NULL or invalid.");
        return false;
    }

    /* Flush first: data still buffered in the source writer is not on disk
       yet and was silently left out of the copy. */
    if (fflush(src_writer->file_writer) != 0 || fflush(dest_writer->file_writer) != 0) {
        FILE_WRITER_LOG("[file_writer_copy] Error: Could not flush the writers.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(src_writer->file_path);
    FILE* src_file = wPath ? _wfopen(wPath, L"rb") : NULL;
    free(wPath);
#else
    FILE* src_file = fopen(src_writer->file_path, "rb");
#endif
    if (!src_file) {
        FILE_WRITER_LOG("[file_writer_copy] Error: Cannot reopen source file for reading.");
        return false;
    }

    /* The bytes go through the destination's OWN open stream, at its current
       position. Reopening the destination path with "wb" truncated the file
       behind that stream, which later wrote its pending data over the copy.
       The bytes are copied verbatim, so a text or "ccs=" stream is switched
       to binary for the copy (no newline or encoding translation). */
#if defined(_WIN32) || defined(_WIN64)
    int destFd = _fileno(dest_writer->file_writer);
    int oldMode = _setmode(destFd, _O_BINARY);
#endif

    bool ok = true;
    char buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), src_file)) > 0) {
        if (fwrite(buffer, sizeof(char), bytes_read, dest_writer->file_writer) != bytes_read) {
            FILE_WRITER_LOG("[file_writer_copy] Error: Failed to write data to the destination file.");
            ok = false;
            break;
        }
    }
    if (ok && ferror(src_file)) {
        FILE_WRITER_LOG("[file_writer_copy] Error: Failed to read the source file.");
        ok = false;
    }
    if (fflush(dest_writer->file_writer) != 0) {
        ok = false;
    }

#if defined(_WIN32) || defined(_WIN64)
    if (oldMode != -1) {
        _setmode(destFd, oldMode);
    }
#endif
    fclose(src_file);

    if (ok) {
        FILE_WRITER_LOG("[file_writer_copy] File content copied successfully.");
    }
    return ok;
}


/**
 * @brief Atomically write `size` bytes to `path` (crash-safe replace).
 *
 * Writes the data to a temporary file in the SAME directory, flushes it to
 * stable storage, then atomically renames it over `path`. If the process or
 * machine crashes mid-write, `path` is left untouched — a reader always sees
 * either the complete old content or the complete new content, never a
 * half-written file. This is the standard production pattern for config / state
 * files. Cross-platform: POSIX `rename()` / Windows `MoveFileExW` with
 * `MOVEFILE_REPLACE_EXISTING`. Unlike the rest of the API this is a one-shot,
 * path-based call (atomicity requires owning the whole open→write→rename
 * sequence), so there is no `FileWriter` handle to manage.
 *
 * @param path Destination path to (over)write. Must not be NULL.
 * @param data Bytes to write (may be NULL only if `size == 0`).
 * @param size Number of bytes to write.
 * @return true on success; false on any error (the temporary file is removed).
 */
bool file_writer_write_all_atomic(const char* path, const void* data, size_t size) {
    if (!path || (!data && size > 0)) {
        FILE_WRITER_LOG("[file_writer_write_all_atomic] Error: invalid argument.");
        return false;
    }

    /* Temp file in the SAME directory so the rename stays on one filesystem
       (cross-device renames are not atomic). Tag with the PID to avoid
       collisions between concurrent writers. */
#if defined(_WIN32) || defined(_WIN64)
    long pid = (long)GetCurrentProcessId();
#else
    long pid = (long)getpid();
#endif
    size_t tmp_len = strlen(path) + 32;
    char* tmp = (char*)malloc(tmp_len);
    if (!tmp) {
        return false;
    }
    snprintf(tmp, tmp_len, "%s.tmp.%ld", path, pid);

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wtmp = encoding_utf8_to_wchar(tmp);
    if (!wtmp) { free(tmp); return false; }
    FILE* f = _wfopen(wtmp, L"wb");
#else
    FILE* f = fopen(tmp, "wb");
#endif
    if (!f) {
        FILE_WRITER_LOG("[file_writer_write_all_atomic] Error: cannot open temp file '%s'.", tmp);
#if defined(_WIN32) || defined(_WIN64)
        free(wtmp);
#endif
        free(tmp);
        return false;
    }

    bool ok = (size == 0) || (fwrite(data, 1, size, f) == size);
    if (ok) {
        fflush(f);
#if defined(_WIN32) || defined(_WIN64)
        HANDLE h = (HANDLE)_get_osfhandle(_fileno(f));
        if (h != INVALID_HANDLE_VALUE) {
            FlushFileBuffers(h);
        }
#else
        int fd = fileno(f);
        if (fd >= 0) {
            fsync(fd);
        }
#endif
    }
    fclose(f);

    if (!ok) {
        FILE_WRITER_LOG("[file_writer_write_all_atomic] Error: write to temp file failed.");
#if defined(_WIN32) || defined(_WIN64)
        _wremove(wtmp);
        free(wtmp);
#else
        remove(tmp);
#endif
        free(tmp);
        return false;
    }

    /* Atomic replace of the destination. */
#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wpath = encoding_utf8_to_wchar(path);
    bool renamed = (wpath != NULL) &&
                   MoveFileExW(wtmp, wpath, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
    free(wpath);
    if (!renamed) {
        _wremove(wtmp);
    }
    free(wtmp);
#else
    bool renamed = (rename(tmp, path) == 0);
    if (!renamed) {
        remove(tmp);
    }
#endif
    free(tmp);

    if (!renamed) {
        FILE_WRITER_LOG("[file_writer_write_all_atomic] Error: atomic rename failed.");
        return false;
    }
    FILE_WRITER_LOG("[file_writer_write_all_atomic] Wrote %zu byte(s) atomically to '%s'.", size, path);
    return true;
}


/**
 * @brief Retrieves the absolute path of the file associated with the `FileWriter` object.
 *
 * This function returns the absolute file path that the `FileWriter` is currently operating on.
 *
 * @param writer A pointer to the `FileWriter` structure.
 * 
 * @return A constant character pointer to the file path if the `FileWriter` is valid, `NULL` otherwise.
 */
const char *file_writer_get_file_name(FileWriter *writer) {
    if (!writer || writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_get_file_name] Error: FileWriter object is null or invalid.");
        return NULL;
    }
    if (!writer->file_path) {
        FILE_WRITER_LOG("[file_writer_get_file_name] Error: File path for FileWriter is null.");
        return NULL;
    }
    
    FILE_WRITER_LOG("[file_writer_get_file_name] Returning file path: %s", writer->file_path);
    return (const char*)writer->file_path;
}


/**
 * @brief Retrieves the encoding type currently set in the `FileWriter`.
 *
 * This function returns the encoding type used by the `FileWriter` object, such as UTF-16 or UTF-32.
 *
 * @param writer A pointer to the `FileWriter` structure.
 * 
 * @return A constant character pointer representing the encoding type if valid, `NULL` otherwise.
 */
const char* file_writer_get_encoding(FileWriter *writer) {
    if (!writer || writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_get_encoding] Error: FileWriter object is null or invalid.");
        return NULL;
    }
    if (!(writer->encoding >= WRITE_ENCODING_UTF16 && writer->encoding <= WRITE_ENCODING_UTF32)) {
        FILE_WRITER_LOG("[file_writer_get_encoding] Error: Invalid encoding type.");
        return NULL;
    }

    char *encoding = NULL;
    switch (writer->encoding) {
        case WRITE_ENCODING_UTF16:
            encoding = string_strdup("ENCODING_UTF16");
            break;
        case WRITE_ENCODING_UTF32:
            encoding = string_strdup("ENCODING_UTF32");
            break;
    }

    FILE_WRITER_LOG("[file_writer_get_encoding] Returning encoding type: %s", encoding);
    return encoding;
}


/**
 * @brief Writes formatted data to the file, similar to `fprintf`.
 *
 * This function formats a string according to the specified format and writes it to the file associated
 * with the `FileWriter` object.
 *
 * @param writer A pointer to the `FileWriter` structure.
 * @param format A format string, similar to `printf`, followed by the values to format.
 * 
 * @return The number of characters written, or `0` if an error occurs.
 */
size_t file_writer_write_fmt(FileWriter* writer, const char* format, ...) {
    if (!writer || !writer->file_writer || !format) {
        FILE_WRITER_LOG("[file_writer_write_fmt] Error: Invalid argument.");
        return 0;
    }

    va_list args;
    va_start(args, format);
    size_t len = 0;
    char* buffer = fw_vformat(format, args, &len);
    va_end(args);
    if (!buffer) {
        FILE_WRITER_LOG("[file_writer_write_fmt] Error: Formatting failed.");
        return 0;
    }

    // Write the formatted string through the stream's encoding
    bool ok = fw_write_utf8(writer, buffer, len);
    fflush(writer->file_writer);
    free(buffer);

    if (!ok) {
        FILE_WRITER_LOG("[file_writer_write_fmt] Error: Failed to write full data to the file.");
        return 0;
    }
    FILE_WRITER_LOG("[file_writer_write_fmt] Successfully written %zu characters to the file.", len);
    return len;
}


/**
 * @brief Retrieves the current size of the file associated with the `FileWriter`.
 *
 * This function flushes any pending write operations, moves the file pointer to the end of the file,
 * and returns the size of the file in bytes.
 *
 * @param writer A pointer to the `FileWriter` structure.
 * 
 * @return The size of the file in bytes, or `0` if an error occurs.
 */
size_t file_writer_get_size(FileWriter* writer) {
    if (!writer || writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_get_size] Error: FileWriter object is NULL or invalid.");
        return 0;
    }

    if (!file_writer_flush(writer)) {
        FILE_WRITER_LOG("[file_writer_get_size] Error: Failed to flush data.");
        return 0;
    }

    /* ftell / fseek work with long (see file_reader_get_size). */
    long current_position = ftell(writer->file_writer);
    if (current_position < 0) {
        FILE_WRITER_LOG("[file_writer_get_size] Error: ftell failed.");
        return 0;
    }
    if (fseek(writer->file_writer, 0, SEEK_END) != 0) {
        FILE_WRITER_LOG("[file_writer_get_size] Error: Failed to seek to end of file.");
        return 0;
    }

    long end = ftell(writer->file_writer);
    if (fseek(writer->file_writer, current_position, SEEK_SET) != 0) {
        FILE_WRITER_LOG("[file_writer_get_size] Error: Failed to return to original position.");
    }
    if (end < 0) {
        FILE_WRITER_LOG("[file_writer_get_size] Error: ftell failed at the end of the file.");
        return 0;
    }
    size_t size = (size_t)end;

    FILE_WRITER_LOG("[file_writer_get_size] File size: %zu bytes.", size);
    return size;
}


/**
 * @brief Locks the file associated with the `FileWriter` to prevent other processes from modifying it.
 *
 * This function locks the file to ensure exclusive access, preventing other processes from writing
 * to it while the lock is held. It uses platform-specific mechanisms to achieve the lock.
 *
 * @param writer A pointer to the `FileWriter` structure representing the file.
 * 
 * @return `true` if the file was successfully locked, `false` otherwise.
 */
bool file_writer_lock(FileWriter* writer) {
    if (!writer || writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_lock] Error: FileWriter object is NULL or invalid.");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(writer->file_writer));
    OVERLAPPED overlapped = {0};
    if (LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &overlapped) == 0) {
        FILE_WRITER_LOG("[file_writer_lock] Error: Unable to lock file in Windows.");
        return false;
    }
    #else
    struct flock fl = {0};
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0; // Lock the whole file
    if (fcntl(fileno(writer->file_writer), F_SETLKW, &fl) == -1) {
        FILE_WRITER_LOG("[file_writer_lock] Error: Unable to lock file in Unix.");
        return false;
    }
    #endif

    FILE_WRITER_LOG("[file_writer_lock] File successfully locked.");
    return true;
}


/**
 * @brief Unlocks the file associated with the `FileWriter` after operations are completed.
 *
 * This function releases the lock on the file, allowing other processes to access it.
 * It uses platform-specific mechanisms to release the lock.
 *
 * @param writer A pointer to the `FileWriter` structure representing the file.
 * 
 * @return `true` if the file was successfully unlocked, `false` otherwise.
 */
bool file_writer_unlock(FileWriter* writer) {
    if (!writer || writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_unlock] Error: FileWriter object is NULL or invalid.");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(writer->file_writer));
    OVERLAPPED overlapped = {0};

    if (UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &overlapped) == 0) {
        FILE_WRITER_LOG("[file_writer_unlock] Error: Unable to unlock file in Windows.");
        return false;
    }

    #else
    struct flock fl = {0};
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0; // Unlock the whole fileqqq
    if (fcntl(fileno(writer->file_writer), F_SETLK, &fl) == -1) {
        FILE_WRITER_LOG("[file_writer_unlock] Error: Unable to unlock file in Unix.");
        return false;
    }
    #endif

    FILE_WRITER_LOG("[file_writer_unlock] File successfully unlocked.");
    return true;
}


/**
 * @brief Moves the file pointer to a specific location for random access writing.
 *
 * This function allows the file pointer to be moved to a specific location, enabling random access writing.
 * The position can be set relative to the beginning, end, or current position of the file.
 *
 * @param writer A pointer to the `FileWriter` structure representing the file.
 * @param offset The offset in bytes to move the file pointer.
 * @param cursor_pos The position from which the offset is applied (beginning, end, or current position).
 * 
 * @return `true` if the file pointer was successfully moved, `false` otherwise (e.g., if the `FileWriter` is invalid
 * or the seek operation fails).
 */
bool file_writer_seek(FileWriter *writer, long offset, const CursorPosition cursor_pos) {
    if (!writer || writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_seek] Error: FileWriter object is NULL or invalid.");
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
            FILE_WRITER_LOG("[file_writer_seek] Warning: Invalid cursor position, defaulting to POS_BEGIN.");
            pos = SEEK_SET;
            break;
    }
    
    if (fseek(writer->file_writer, offset, pos) != 0) {
        FILE_WRITER_LOG("[file_writer_seek] Error: fseek failed.");
        return false;
    }

    FILE_WRITER_LOG("[file_writer_seek] File pointer successfully moved by offset %ld.", offset);
    return true;
}


/**
 * @brief Truncates the file associated with the `FileWriter` to the specified size.
 *
 * This function shortens the file to the given size. If the size is larger than
 * the current file size, the file is extended, and the added space is filled with zeros.
 *
 * @param writer A pointer to the `FileWriter` structure representing the file.
 * @param size The size to which the file should be truncated.
 * 
 * @return `true` if the file was successfully truncated, `false` otherwise (e.g., if
 * the `FileWriter` is not valid, the flush operation fails, or the truncation fails).
 */
bool file_writer_truncate(FileWriter *writer, size_t size) {
    if (!writer || writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_truncate] Error: FileWriter object is NULL or invalid.");
        return false;
    }
    if (!file_writer_flush(writer)) {
        FILE_WRITER_LOG("[file_writer_truncate] Error: Failed to flush the file.");
        return false;
    }
    int fd;

    #if defined(_WIN32) || defined(_WIN64)
    fd = _fileno(writer->file_writer);
    if (_chsize_s(fd, size) != 0) {
        FILE_WRITER_LOG("[file_writer_truncate] Error: Could not truncate file in Windows.");
        return false;
    }
    #else 
    fd = fileno(writer->file_writer);
    if (ftruncate(fd, size) == -1) {
        FILE_WRITER_LOG("[file_writer_truncate] Error: Could not truncate file in Unix.");
        return false;
    }
    #endif 

    FILE_WRITER_LOG("[file_writer_truncate] File successfully truncated to size %zu bytes.", size);
    return true;
}


/**
 * @brief Writes multiple buffers to a file in a single operation, potentially optimizing I/O operations.
 *
 * This function allows writing multiple buffers to the file in a batch operation,
 * which can reduce the number of system calls and improve performance. The function
 * handles different encoding types (UTF-16, UTF-32) and converts the data as needed before writing.
 *
 * @param writer A pointer to the `FileWriter` structure representing the file.
 * @param buffers An array of pointers to the buffers to be written.
 * @param sizes An array of sizes corresponding to the buffers to be written.
 * @param count The number of buffers to be written.
 * 
 * @return `true` if all buffers were successfully written, `false` otherwise (e.g., if
 * there is an error with the buffers, sizes, or the `FileWriter`).
 */
bool file_writer_write_batch(FileWriter* writer, const void** buffers, const size_t* sizes, size_t count) {
    if (!writer || !writer->file_writer || !buffers || !sizes) {
        FILE_WRITER_LOG("[file_writer_write_batch] Error: Invalid arguments.");
        return false;
    }

    size_t all_bytes = 0;
    size_t total_written = 0;

    /* Binary fast-path: writes raw bytes verbatim, no encoding conversion.
       This is the only sane behavior when the caller asks for WRITE_BINARY —
       any UTF-16 expansion would corrupt arbitrary byte data. */
    if (writer->mode == WRITE_BINARY) {
        for (size_t i = 0; i < count; ++i) {
            const void* buffer = buffers[i];
            size_t size = sizes[i];
            if (!buffer || size == 0) {
                FILE_WRITER_LOG("[file_writer_write_batch] Error: Invalid buffer or size at index %zu.", i);
                continue;
            }
            size_t w = fwrite(buffer, 1, size, writer->file_writer);
            if (w != size) {
                FILE_WRITER_LOG("[file_writer_write_batch] Error: Short write at index %zu.", i);
                return false;
            }
            total_written += w;
            all_bytes    += size;
        }
        FILE_WRITER_LOG("[file_writer_write_batch] Binary mode: wrote %zu bytes.", total_written);
        return total_written == all_bytes;
    }

    /* Every other mode: each buffer goes through file_writer_write, so a
       batch writes exactly what the same writes one by one would. The batch
       used to widen even WRITE_TEXT data to UTF-16 on Windows (mixing
       encodings in one file and breaking on the text-mode LF -> CRLF), and
       compared bytes written with input bytes (x2 for Unicode only), so it
       reported failure for writes that had succeeded. */
    for (size_t i = 0; i < count; ++i) {
        const void* buffer = buffers[i];
        size_t size = sizes[i];
        if (!buffer || size == 0) {
            FILE_WRITER_LOG("[file_writer_write_batch] Error: Invalid buffer or size at index %zu.", i);
            continue;
        }
        if (file_writer_write((void*)buffer, 1, size, writer) != size) {
            FILE_WRITER_LOG("[file_writer_write_batch] Error: Partial or failed write at index %zu.", i);
            return false;
        }
        total_written += size;
    }

    FILE_WRITER_LOG("[file_writer_write_batch] Successfully wrote %zu bytes.", total_written);
    return true;
}


/**
 * @brief Appends formatted text to a file using the specified `FileWriter`.
 *
 * This function formats a string according to the specified format string
 * and arguments, then appends the resulting string to the file associated
 * with the given `FileWriter`. The function requires that the `FileWriter`
 * is in append mode (`WRITE_APPEND`).
 *
 * @param writer A pointer to the `FileWriter` structure representing the file.
 * @param format A format string that follows the same specifications as `printf`.
 * @param ... Additional arguments that correspond to the format string.
 * 
 * @return `true` if the formatted text was successfully appended to the file, 
 * `false` otherwise (e.g., if the `FileWriter` is not in append mode 
 *  or if an error occurs during the writing process).
 */
bool file_writer_append_fmt(FileWriter* writer, const char* format, ...) {
    if (!writer || !writer->file_writer || !format) {
        FILE_WRITER_LOG("[file_writer_append_fmt] Error: Invalid arguments.");
        return false;
    }
    if (writer->mode != WRITE_APPEND) {
        FILE_WRITER_LOG("[file_writer_append_fmt] Error: FileWriter must be in append mode.");
        return false;
    }

    va_list args;
    va_start(args, format);
    size_t len = 0;
    char* buffer = fw_vformat(format, args, &len);
    va_end(args);
    if (!buffer) {
        FILE_WRITER_LOG("[file_writer_append_fmt] Error: Formatting failed.");
        return false;
    }

    bool ok = fw_write_utf8(writer, buffer, len);
    free(buffer);

    FILE_WRITER_LOG("[file_writer_append_fmt] Appended %zu characters to the file (ok=%d).", len, ok);
    return ok;
}

