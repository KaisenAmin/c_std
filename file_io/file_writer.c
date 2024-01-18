#include "file_writer.h"
#include "../encoding/encoding.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64) 
#include <windows.h>
#endif 


FileWriter* file_writer_open(const char* filename, const WriteMode mode) {
    if (!filename) {
        fprintf(stderr, "Error: filename is null in file_writer_open.\n");
        return NULL;
    }

    FileWriter* writer = (FileWriter*) malloc(sizeof(FileWriter));
    if (!writer) {
        fprintf(stderr, "Error: Can not allocate memory for FileWriter in file_writer_open.\n");
        return NULL;
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
        fprintf(stdout, "Warning: Not Valid mode for writing in file_writer_open i initialize default mode that is 'w'.\n");
        break;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wFileName = encoding_utf8_to_wchar(filename);
    wchar_t* wMode = encoding_utf8_to_wchar(modeStr);

    if (!wMode) {
        fprintf(stderr, "Error: Can not convert mode to wchar in file_writer_open.\n");
        return NULL;
    }
    if (!wFileName) {
        fprintf(stderr, "Error: Can not convert filename to wchar in file_writer_open.\n");
        return NULL;
    }
    writer->file_writer = _wfopen(wFileName, wMode);
    free(wMode);
    free(wFileName);
    #else 
    writer->file_writer = fopen(filename, modeStr);
    #endif 

    if (writer->file_writer == NULL) {
        fprintf(stderr, "Error: Can not open file in file_writer_open.\n");
        free(writer);
        return NULL;
    }
    writer->mode = mode;
    writer->is_open = true;
    writer->encoding = ENCODING_UTF16;
    return writer;
}

// Open an existing file for appending. If the file does not exist, it will be created.
FileWriter *file_writer_append(const char *filename, const WriteMode mode) {
    if (!filename) {
        fprintf(stderr, "Error: filename is null in file_writer_open.\n");
        return NULL;
    }

    FileWriter* writer = (FileWriter*) malloc(sizeof(FileWriter));
    if (!writer) {
        fprintf(stderr, "Error: Can not allocate memory for FileWriter in file_writer_open.\n");
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
        fprintf(stdout, "Warning: Not Valid mode for writing in file_writer_open i initialize default mode that is 'w'.\n");
        break;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wFileName = encoding_utf8_to_wchar(filename);
    wchar_t* wMode = encoding_utf8_to_wchar(modeStr);

    if (!wMode) {
        fprintf(stderr, "Error: Can not convert mode to wchar in file_writer_open.\n");
        return NULL;
    }
    if (!wFileName) {
        fprintf(stderr, "Error: Can not convert filename to wchar in file_writer_open.\n");
        return NULL;
    }
    writer->file_writer = _wfopen(wFileName, wMode);
    free(wMode);
    free(wFileName);
    #else 
    writer->file_writer = fopen(filename, modeStr);
    #endif 

    if (writer->file_writer == NULL) {
        fprintf(stderr, "Error: Can not open file in file_writer_open.\n");
        free(writer);
        return NULL;
    }
    writer->mode = mode;
    writer->is_open = true;
    writer->encoding = ENCODING_UTF16;

    return writer;
}

bool file_writer_close(FileWriter *writer)
{
    if (writer->file_writer == NULL) {
        fprintf(stdout, "Warning: Right now the file is NULL no need to close it in file_writer_close.\n");
        return false;
    }
    if (fclose(writer->file_writer)) {
        fprintf(stderr, "Error: Failed to close file in file_writer_close.\n");
        return false;
    }
    writer->is_open = false;
    return true;
}

size_t file_writer_get_position(FileWriter *writer)
{
    if (writer->file_writer == NULL) {
        fprintf(stderr, "Error: FileWriter object is null and not valid in file_writer_get_position.\n");
        return (size_t)-1;
    }

    long cursor_position = ftell(writer->file_writer);
    if (cursor_position == -1L) {
        fprintf(stderr, "Error: Could not determine file position.\n");
        return (size_t)-1;
    }
    return (size_t)cursor_position;
}

size_t file_writer_write(void *buffer, size_t size, size_t count, FileWriter *writer) {
    if (!writer || !writer->file_writer || !buffer) {
        fprintf(stderr, "Error: Invalid argument in file_writer_write.\n");
        return 0;
    }

    size_t written = 0;

    switch (writer->encoding) {
        case ENCODING_UTF32: {
            // Convert UTF-8 to UTF-32 and then write
            uint32_t* utf32Buffer = encoding_utf8_to_utf32((const uint8_t*)buffer, size * count);
            if (!utf32Buffer) {
                fprintf(stderr, "Error: Conversion to UTF-32 failed in file_writer_write.\n");
                return 0;
            }
            written = fwrite(utf32Buffer, sizeof(uint32_t), wcslen((wchar_t*)utf32Buffer), writer->file_writer);
            free(utf32Buffer);
            break;
        }

        default: // Default case is ENCODING_UTF16
        case ENCODING_UTF16: {
            #if defined(_WIN32) || defined(_WIN64)
            // On Windows, UTF-16 is native, so use your UTF-8 to wchar_t conversion
            wchar_t* wBuffer = encoding_utf8_to_wchar((const char*)buffer);
            if (!wBuffer) {
                fprintf(stderr, "Error: Conversion to wchar_t failed in file_writer_write.\n");
                return 0;
            }
            written = fwrite(wBuffer, sizeof(wchar_t), wcslen(wBuffer), writer->file_writer);
            free(wBuffer);
            #else
            // On other systems, convert UTF-8 to UTF-16 and then write
            uint16_t* utf16Buffer = encoding_utf8_to_utf16((const uint8_t*)buffer, size * count);
            if (!utf16Buffer) {
                fprintf(stderr, "Error: Conversion to UTF-16 failed in file_writer_write.\n");
                return 0;
            }
            written = fwrite(utf16Buffer, sizeof(uint16_t), wcslen((wchar_t*)utf16Buffer), writer->file_writer);
            free(utf16Buffer);
            #endif
            break;
        }
    }
    return written;
}


bool file_writer_write_line(char *buffer, size_t size, FileWriter *writer) {
    if (writer->file_writer == NULL || !writer) {
        fprintf(stderr, "Error: FileWriter object is NULL and not valid in file_writer_write_line.\n");
        return false;
    }
    if (buffer == NULL) {
        fprintf(stderr, "Error: Invalid arg 'buffer is NULL' in file_writer_write_line.\n");
        return false;
    }

    size_t written = 0;
    size_t elementToWriteSize = size;

    #if defined(_WIN32) || defined(_WIN64)
    if (writer->mode == WRITE_UNICODE) {
        wchar_t* wBuffer = encoding_utf8_to_wchar(buffer);
        if (!wBuffer) {
            fprintf(stderr, "Error: Can not convert buffer to wchar in file_writer_write_line.\n");
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
        fprintf(stderr, "Error: could not write entire buffer in file in file_writer_write_line.\n");
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
    if (writer->file_writer == NULL) {
        fprintf(stderr, "Error: FileWriter object is NULL and its not open.\n");
        return false;
    }
    return writer->is_open;
}

// This function will flush the buffer of the file writer, ensuring that all written data is physically stored in the file.
bool file_writer_flush(FileWriter* writer) {
    if (!writer || writer->file_writer == NULL) {
        fprintf(stderr, "Error: FileWriter object is NULL and not valid in file_writer_flush");
        return false;
    }

    // fflush will flush the write buffer associated with the file writer.
    // This is necessary for both text and binary modes. 
    // It works correctly for Unicode text as well, as it does not interpret the buffer's content.
    if (fflush(writer->file_writer) == EOF) {
        fprintf(stderr, "Error: Failed to flush the writer in file_writer_flush.\n");
        return false;
    }

    return true;
}

// Set the character encoding for writing to the file.
bool file_writer_set_encoding(FileWriter* writer, const EncodingType encoding) {
    if (!writer || writer->file_writer == NULL) {
        fprintf(stderr, "Error: Filewriter object is invalid or NULL in file_writer_set_encoding.\n");
        return false;
    }
    if (!(encoding >= ENCODING_UTF16 && encoding <= ENCODING_UTF32)) {
        fprintf(stderr, "Error: Encoding type is Invalid in file_writer_set_encoding.\n");
        return false;
    }
    writer->encoding = encoding;
    return true;
}

// Write formatted data to the file, similar to `fprintf`.
size_t file_writer_write_fmt(FileWriter* writer, const char* format, ...) {
    if (!writer || !writer->file_writer || !format) {
        fprintf(stderr, "Error: Invalid argument in file_writer_write_fmt.\n");
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