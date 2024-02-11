#include "file_writer.h"
#include "../encoding/encoding.h"
#include "../fmt/fmt.h"
#include "../string/string.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64) 
#include <windows.h>
#include <io.h> // _get_osfhandle
#else
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif 

FileWriter* file_writer_open(const char* filename, const WriteMode mode) {
    if (!filename) {
        fmt_fprintf(stderr, "Error: filename is null in file_writer_open.\n");
        exit(-1);
    }

    FileWriter* writer = (FileWriter*) malloc(sizeof(FileWriter));
    if (!writer) {
        fmt_fprintf(stderr, "Error: Can not allocate memory for FileWriter in file_writer_open.\n");
        exit(-1);
    }

    const char* modeStr = NULL;
    switch (mode){
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
        fmt_fprintf(stdout, "Warning: Not Valid mode for writing in file_writer_open i initialize default mode that is 'w'.\n");
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

    if (!wMode) {
        fmt_fprintf(stderr, "Error: Can not convert mode to wchar in file_writer_open.\n");
        exit(-1);
    }
    if (!wFileName) {
        fmt_fprintf(stderr, "Error: Can not convert filename to wchar in file_writer_open.\n");
        exit(-1);
    }
    writer->file_writer = _wfopen(wFileName, wMode);
    free(wMode);
    free(wFileName);
    #else 
    writer->file_writer = fopen(filename, modeStr);
    #endif 

    if (writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: Can not open file in file_writer_open.\n");
        free(writer);
        exit(-1);
    }
    writer->mode = mode;
    writer->is_open = true;
    writer->encoding = WRITE_ENCODING_UTF16;
    writer->file_path = string_strdup(filename);
    return writer;
}

// Open an existing file for appending. If the file does not exist, it will be created.
FileWriter *file_writer_append(const char *filename, const WriteMode mode) {
    if (!filename) {
        fmt_fprintf(stderr, "Error: filename is null in file_writer_open.\n");
        return NULL;
    }

    FileWriter* writer = (FileWriter*) malloc(sizeof(FileWriter));
    if (!writer) {
        fmt_fprintf(stderr, "Error: Can not allocate memory for FileWriter in file_writer_open.\n");
        return NULL;
    }

    const char* modeStr = NULL;
    switch (mode){
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
        fmt_fprintf(stdout, "Warning: Not Valid mode for writing in file_writer_open i initialize default mode that is 'w'.\n");
        break;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wFileName = encoding_utf8_to_wchar(filename);
    wchar_t* wMode = encoding_utf8_to_wchar(modeStr);

    if (!wMode) {
        fmt_fprintf(stderr, "Error: Can not convert mode to wchar in file_writer_open.\n");
        return NULL;
    }
    if (!wFileName) {
        fmt_fprintf(stderr, "Error: Can not convert filename to wchar in file_writer_open.\n");
        return NULL;
    }
    writer->file_writer = _wfopen(wFileName, wMode);
    free(wMode);
    free(wFileName);
    #else 
    writer->file_writer = fopen(filename, modeStr);
    #endif 

    if (writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: Can not open file in file_writer_open.\n");
        free(writer);
        return NULL;
    }
    writer->mode = mode;
    writer->is_open = true;
    writer->encoding = WRITE_ENCODING_UTF16;
    writer->file_path = string_strdup(filename);

    return writer;
}

bool file_writer_close(FileWriter *writer) {
    if (writer->file_writer == NULL) {
        fmt_fprintf(stdout, "Warning: Right now the file is NULL no need to close it in file_writer_close.\n");
        return false;
    }
    if (fclose(writer->file_writer)) {
        fmt_fprintf(stderr, "Error: Failed to close file in file_writer_close.\n");
        return false;
    }
    writer->is_open = false;
    return true;
}

size_t file_writer_get_position(FileWriter *writer) {
    if (writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: FileWriter object is null and not valid in file_writer_get_position.\n");
        return (size_t)-1;
    }

    long cursor_position = ftell(writer->file_writer);
    if (cursor_position == -1L) {
        fmt_fprintf(stderr, "Error: Could not determine file position.\n");
        return (size_t)-1;
    }
    return (size_t)cursor_position;
}

size_t file_writer_write(void *buffer, size_t size, size_t count, FileWriter *writer) {
    if (!writer || !writer->file_writer || !buffer) {
        fmt_fprintf(stderr, "Error: Invalid argument in file_writer_write.\n");
        return 0;
    }

    // Directly write binary data without conversion
    if (writer->mode == WRITE_BINARY) {
        return fwrite(buffer, size, count, writer->file_writer);
    }

    size_t written = 0;

    // Handle text and unicode data with conversion if necessary
    switch (writer->encoding) {
        case WRITE_ENCODING_UTF32: {
            // Convert UTF-8 to UTF-32 and then write
            uint32_t* utf32Buffer = encoding_utf8_to_utf32((const uint8_t*)buffer, size * count);
            if (!utf32Buffer) {
                fmt_fprintf(stderr, "Error: Conversion to UTF-32 failed in file_writer_write.\n");
                return 0;
            }
            written = fwrite(utf32Buffer, sizeof(uint32_t), wcslen((wchar_t*)utf32Buffer), writer->file_writer);
            free(utf32Buffer);
            break;
        }

        case WRITE_ENCODING_UTF16: {
            #if defined(_WIN32) || defined(_WIN64)
            // For Windows, if mode requires UTF-16, convert and write
            if (writer->mode == WRITE_UNICODE || writer->mode == WRITE_APPEND) {
                wchar_t* wBuffer = encoding_utf8_to_wchar((const char*)buffer);
                if (!wBuffer) {
                    fmt_fprintf(stderr, "Error: Conversion to wchar_t failed in file_writer_write.\n");
                    return 0;
                }
                written = fwrite(wBuffer, sizeof(wchar_t), wcslen(wBuffer), writer->file_writer);
                free(wBuffer);
            }
            else {
                // For non-Unicode modes, write directly
                written = fwrite(buffer, size, count, writer->file_writer);
            }
            #else
            // For non-Windows systems, convert UTF-8 to UTF-16 if required and write
            if (writer->encoding == WRITE_ENCODING_UTF16) {
                uint16_t* utf16Buffer = encoding_utf8_to_utf16((const uint8_t*)buffer, size * count);
                if (!utf16Buffer) {
                    fmt_fprintf(stderr, "Error: Conversion to UTF-16 failed in file_writer_write.\n");
                    return 0;
                }
                written = fwrite(utf16Buffer, sizeof(uint16_t), wcslen((wchar_t*)utf16Buffer), writer->file_writer);
                free(utf16Buffer);
            } 
            else {
                // For non-Unicode modes, write directly
                written = fwrite(buffer, size, count, writer->file_writer);
            }
            #endif
            break;
        }

        // Other encoding types or default handling can be added here
        default:
            // For safety, default to direct writing for undefined encoding types
            written = fwrite(buffer, size, count, writer->file_writer);
            break;
    }

    return written;
}

bool file_writer_write_line(char *buffer, size_t size, FileWriter *writer) {
    if (writer->file_writer == NULL || !writer) {
        fmt_fprintf(stderr, "Error: FileWriter object is NULL and not valid in file_writer_write_line.\n");
        return false;
    }
    if (buffer == NULL) {
        fmt_fprintf(stderr, "Error: Invalid arg 'buffer is NULL' in file_writer_write_line.\n");
        return false;
    }

    size_t written = 0;
    size_t elementToWriteSize = size;

    #if defined(_WIN32) || defined(_WIN64)
    if (writer->mode == WRITE_UNICODE) {
        wchar_t* wBuffer = encoding_utf8_to_wchar(buffer);
        if (!wBuffer) {
            fmt_fprintf(stderr, "Error: Can not convert buffer to wchar in file_writer_write_line.\n");
            return false;
        }

        elementToWriteSize = wcslen(wBuffer);
        written = fwrite(wBuffer, sizeof(wchar_t), elementToWriteSize, writer->file_writer);
        free(wBuffer);
    }
    else {
        written = fwrite(buffer, sizeof(char), elementToWriteSize, writer->file_writer);
    }
    #else 
    written = fwrite(buffer, sizeof(char), size, writer->file_writer);
    #endif 

    if (written < elementToWriteSize) {
        fmt_fprintf(stderr, "Error: could not write entire buffer in file in file_writer_write_line.\n");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    if (writer->mode == WRITE_UNICODE) {
        wchar_t newLine[] = L"\n";
        written = fwrite(newLine, sizeof(wchar_t), 1, writer->file_writer);
    }
    else {
        char newLine[] = "\n";
        written = fwrite(newLine, sizeof(char), 1, writer->file_writer);
    }
    #else 
    char newLine[] = "\n";
    written = fwrite(newLine, sizeof(char), 1, writer->file_writer);
    #endif 

    return written == 1;
}

bool file_writer_is_open(FileWriter* writer) {
    if (!writer) {
        fmt_fprintf(stderr, "Error: FileWriter pointer is NULL in file_writer_is_open.\n");
        return false;
    }
    if (writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: FileWriter object is NULL and its not open in file_writer_is_open.\n");
        return false;
    }
    return writer->is_open;
}

// This function will flush the buffer of the file writer, ensuring that all written data is physically stored in the file.
bool file_writer_flush(FileWriter* writer) {
    if (!writer || writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: FileWriter object is NULL and not valid in file_writer_flush");
        return false;
    }

    // fflush will flush the write buffer associated with the file writer.
    // This is necessary for both text and binary modes. 
    // It works correctly for Unicode text as well, as it does not interpret the buffer's content.
    if (fflush(writer->file_writer) == EOF) {
        fmt_fprintf(stderr, "Error: Failed to flush the writer in file_writer_flush.\n");
        return false;
    }
    return true;
}

// Set the character encoding for writing to the file.
bool file_writer_set_encoding(FileWriter* writer, const WriteEncodingType encoding) {
    if (!writer || writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: Filewriter object is invalid or NULL in file_writer_set_encoding.\n");
        return false;
    }
    if (!(encoding >= WRITE_ENCODING_UTF16 && encoding <= WRITE_ENCODING_UTF32)) {
        fmt_fprintf(stderr, "Error: Encoding type is Invalid in file_writer_set_encoding.\n");
        return false;
    }
    writer->encoding = encoding;
    return true;
}

// Copy content from one file to another.
bool file_writer_copy(FileWriter *src_writer, FileWriter *dest_writer){
    if (!src_writer || src_writer->file_writer == NULL || src_writer->file_path == NULL) {
        fmt_fprintf(stderr, "Error: src_writer object or file_path or both them are null and not valid in file_writer_copy.\n");
        return false;
    }
    if (!src_writer || src_writer->file_writer == NULL || src_writer->file_path == NULL) {
        fmt_fprintf(stderr, "Error: des_writer object or file_path or both them are null and not valid in file_writer_copy.\n");
        return false;
    }

    FILE* src_file = fopen(src_writer->file_path, "rb");
    if (!src_file) {
        fmt_fprintf(stderr, "Error: Can not reopen source file for reading in file_writer_copy.\n");
        return false;
    }

    FILE* dest_file = fopen(dest_writer->file_path, "wb");
    if (!dest_file) {
        fmt_fprintf(stderr, "Error: Can not reopen destination file for writing in file_writer_copy.\n");
        return false;
    }

    char buffer[4096];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), src_file))) {
        if (fwrite(buffer, sizeof(char), bytes_read, dest_file) != bytes_read) {
            fmt_fprintf(stderr, "Error: Write Error occurred in file_writer_copy.\n");
            
            fclose(src_file);
            fclose(dest_file);
            return false;
        }
    }
    fclose(src_file);
    fclose(dest_file);

    return true;
}

// get absolute path of FileWriter object 
const char *file_writer_get_file_name(FileWriter *writer){
    if (!writer || writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: FileWriter object is null and not valid in file_writer_get_file_name.\n");
        return NULL;
    }
    if (!writer->file_path) {
        fmt_fprintf(stderr, "Error: file path for FileWriter is null in file_writer_get_file_name.\n");
        return NULL;
    }
    return (const char*)writer->file_path;
}

// get encoding type of FileWriter
const char* file_writer_get_encoding(FileWriter *writer) {
    if (!writer || writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: FileWriter object is null and not valid in file_writer_get_encoding.\n");
        return NULL;
    }

    if (!(writer->encoding >= WRITE_ENCODING_UTF16 && writer->encoding <= WRITE_ENCODING_UTF32)) {
        fmt_fprintf(stderr, "Error: Encoding type is Invalid in file_writer_get_encoding.\n");
        return NULL;
    }

    char *encoding = NULL;
    switch (writer->encoding){
        case WRITE_ENCODING_UTF16:
            encoding = string_strdup("ENCODING_UTF16");
            break;
        case WRITE_ENCODING_UTF32:
            encoding = string_strdup("ENCODING_UTF32");
            break;
    }
    return encoding;
}

// Write formatted data to the file, similar to `fprintf`.
size_t file_writer_write_fmt(FileWriter* writer, const char* format, ...) {
    if (!writer || !writer->file_writer || !format) {
        fmt_fprintf(stderr, "Error: Invalid argument in file_writer_write_fmt.\n");
        return 0;
    }

    va_list args;
    va_start(args, format);

    char buffer[2048]; // Adjust the buffer size as necessary
    vsnprintf(buffer, sizeof(buffer), format, args);

    // Write the formatted string to the file
    size_t written = file_writer_write(buffer, strlen(buffer), 1, writer);

    va_end(args);
    return written;
}

size_t file_writer_get_size(FileWriter* writer) {
    if (!writer || writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: FileWriter object is not valid and NULL in file_writer_get_size.\n");
        return 0;
    }

    if (!file_writer_flush(writer)) {
        fmt_fprintf(stderr, "Error: Failed in flushing the data in file_writer_get_size.\n");
        return 0;
    }
    size_t current_position = file_writer_get_position(writer);
    if (fseek(writer->file_writer, 0, SEEK_END) != 0) {
        fmt_fprintf(stderr, "Error: fseek failed to seek to end of file in file_writer_get_size.\n");
        return 0;
    }

    size_t size = file_writer_get_position(writer);
    if (fseek(writer->file_writer, current_position, SEEK_SET) != 0) {
        fmt_fprintf(stderr, "Error: fseek failed to return to original position in file_writer_get_position.\n");
    }
    return size;
}

// Lock the file to prevent other processes from modifying it while it's being written to
bool file_writer_lock(FileWriter* writer) {
    if (!writer || writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: FileWriter object is NULL in file_writer_lock.\n");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(writer->file_writer));
    OVERLAPPED overlapped = {0};
    if (LockFileEx(hFile, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &overlapped) == 0) {
        fmt_fprintf(stderr, "Error: Unable to lock file in Windows.\n");
        return false;
    }
    #else
    struct flock fl = {0};
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0; // Lock the whole file
    if (fcntl(fileno(writer->file_writer), F_SETLKW, &fl) == -1) {
        fmt_fprintf(stderr, "Error: Unable to lock file in Unix.\n");
        return false;
    }
    #endif
    return true;
}

// Unlock the file once operations are done.
bool file_writer_unlock(FileWriter* writer) {
    if (!writer || writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: FileWriter object is NULL in file_writer_unlock.\n");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    HANDLE hFile = (HANDLE)_get_osfhandle(_fileno(writer->file_writer));
    OVERLAPPED overlapped = {0};
    if (UnlockFileEx(hFile, 0, MAXDWORD, MAXDWORD, &overlapped) == 0) {
        fmt_fprintf(stderr, "Error: Unable to unlock file in Windows.\n");
        return false;
    }
    #else
    struct flock fl = {0};
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0; // Unlock the whole file
    if (fcntl(fileno(writer->file_writer), F_SETLK, &fl) == -1) {
        fmt_fprintf(stderr, "Error: Unable to unlock file in Unix.\n");
        return false;
    }
    #endif
    return true;
}

// Move the file pointer to a specific location for random access writing
bool file_writer_seek(FileWriter *writer, long offset, const CursorPosition cursor_pos) {
    if (!writer || writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: FileWriter object is null and invalid in file_writer_seek.\n");
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
            fmt_fprintf(stderr,"Warning: Cursor position is Invalid defailt pos is POS_BEGIN in file_writer_seek.\n");
            pos = SEEK_SET;
            break;
    }
    
    if (fseek(writer->file_writer, offset, pos) != 0) {
        fmt_fprintf(stderr, "Error: fseek failed in file_writer_seek.\n");
        return false;
    }
    return true;
}

bool file_writer_truncate(FileWriter *writer, size_t size) {
    if (!writer || writer->file_writer == NULL) {
        fmt_fprintf(stderr, "Error: FileWriter object is null and invalid in file_writer_truncate.\n");
        return false;
    }
    if (!file_writer_flush(writer)) {
        fmt_fprintf(stderr, "Error: Failed to flush the file in file_writer_truncate.\n");
        return false;
    }
    int fd;

    #if defined(_WIN32) || defined(_WIN64)
    fd = _fileno(writer->file_writer);
    if (_chsize_s(fd, size) != 0) {
        fmt_fprintf(stderr, "Error: Could not truncate file in file_writer_truncate.\n");
        return false;
    }
    #else 
    fd = fileno(writer->file_writer);
    if (ftruncate(fd, size) == -1) {
        fmt_fprintf(stderr, "Error: Could not truncate file in file_writer_truncate.\n");
        return false;
    }
    #endif 

    return true;
}

// Allows writing multiple buffers in a single operation, potentially optimizing I/O operations by reducing the number of system calls
bool file_writer_write_batch(FileWriter* writer, const void** buffers, const size_t* sizes, size_t count) {
    if (!writer || !writer->file_writer || !buffers || !sizes) {
        fmt_fprintf(stderr, "Error: Invalid arguments in file_writer_write_batch.\n");
        return false;
    }

    size_t all_bytes = 0;
    size_t total_written = 0;
    for (size_t i = 0; i < count; ++i) {
        const void* buffer = buffers[i];
        size_t size = sizes[i];
        all_bytes += size;
        if (!buffer || size == 0) {
            fmt_fprintf(stderr, "Error: Invalid buffer or size in file_writer_write_batch at index %zu.\n", i);
            continue;
        }

        size_t written = 0;
        void* convertedBuffer = NULL; // Pointer for holding converted data
        size_t convertedSize = 0;     // Size of the converted data

        // Convert buffer based on encoding
        switch (writer->encoding) {
            case WRITE_ENCODING_UTF32: {
                uint32_t* utf32Buffer = encoding_utf8_to_utf32((const uint8_t*)buffer, size);
                if (!utf32Buffer) {
                    fmt_fprintf(stderr, "Error: Conversion to UTF-32 failed in file_writer_write_batch.\n");
                    continue;
                }
                convertedBuffer = utf32Buffer;
                convertedSize = wcslen((wchar_t*)utf32Buffer) * sizeof(uint32_t);
                break;
            }
            default: // Default case is ENCODING_UTF16
            case WRITE_ENCODING_UTF16: {
                #if defined(_WIN32) || defined(_WIN64)
                wchar_t* wBuffer = encoding_utf8_to_wchar((const char*)buffer);
                if (!wBuffer) {
                    fmt_fprintf(stderr, "Error: Conversion to wchar_t failed in file_writer_write_batch.\n");
                    continue;
                }
                convertedBuffer = wBuffer;
                convertedSize = wcslen(wBuffer) * sizeof(wchar_t);
                #else
                uint16_t* utf16Buffer = encoding_utf8_to_utf16((const uint8_t*)buffer, size);
                if (!utf16Buffer) {
                    fmt_fprintf(stderr, "Error: Conversion to UTF-16 failed in file_writer_write_batch.\n");
                    continue;
                }
                convertedBuffer = utf16Buffer;
                convertedSize = wcslen((wchar_t*)utf16Buffer) * sizeof(uint16_t);
                #endif
                break;
            }
        }

        // Write the buffer to the file
        written = fwrite(convertedBuffer, 1, convertedSize, writer->file_writer);
        free(convertedBuffer);

        if (written != convertedSize) {
            fmt_fprintf(stderr, "Error: Partial or failed write in file_writer_write_batch at index %zu.\n", i);
            return false;
        }

        total_written += written;
    }
    if (writer->mode == WRITE_UNICODE)
        all_bytes *= 2; // because we use wchar_t in unicode 

    return total_written == all_bytes;
}

// Similar to file_writer_write_fmt, but specifically for appending formatted text to a file.
bool file_writer_append_fmt(FileWriter* writer, const char* format, ...) {
    if (!writer || !writer->file_writer || !format) {
        fmt_fprintf(stderr, "Error: Invalid argument in file_writer_append_fmt.\n");
        return false;
    }
    if (writer->mode != WRITE_APPEND) {
        fmt_fprintf(stderr, "Error: FileWriter object must be in append mode in file_writer_write_fmt.\n");
        return false;
    }

    va_list args;
    va_start(args, format);

    char buffer[2048]; // Adjust the buffer size as necessary
    vsnprintf(buffer, sizeof(buffer), format, args);

    size_t written = file_writer_write(buffer, strlen(buffer), 1, writer);

    va_end(args);
    return written > 0;
}
