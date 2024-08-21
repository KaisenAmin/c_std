#include "file_reader.h"
#include "../string/std_string.h"
#include "../encoding/encoding.h"
#include <stdlib.h>
#include <string.h>

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
        fprintf(stderr, "Error: filename is null in file_reader_open.\n");
        exit(-1);
    }

    FileReader* reader = (FileReader*) malloc(sizeof(FileReader));
    if (!reader) {
        fprintf(stderr, "Error: Can not allocate memory for FileReader in file_reader_open.\n");
        exit(-1);
    }
    const char* modeStr = NULL;

    switch (mode){
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
        modeStr = "r";
        #endif 
        break;
    case READ_BUFFERED:
        modeStr = "r";
        break;
    case READ_UNBUFFERED:
        modeStr = "r";
        break;
    default:
        fprintf(stdout, "Warning: Not Valid mode for reading in file_reader_open i initialize default mode that is 'r'.\n");
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

    if (!wMode) {
        fprintf(stderr, "Error: Can not convert mode to wchar in file_reader_open.\n");
        exit(-1);
    }
    if (!wFileName) {
        fprintf(stderr, "Error: Can not convert filename to wchar in file_reader_open.\n");
        exit(-1);
    }
    reader->file_reader = _wfopen(wFileName, wMode);
    free(wMode);
    free(wFileName);
    #else 
    reader->file_reader = fopen(filename, modeStr);
    #endif 

    if (reader->file_reader == NULL) {
        fprintf(stderr, "Error: Can not open file in file_reader_open.\n");
        free(reader);
        exit(-1);
    }
    reader->mode = mode;
    reader->is_open = true;
    reader->encoding = READ_ENCODING_UTF16;
    reader->file_path = string_strdup(filename);
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
        fprintf(stderr, "Error: FileReader object is null in file_reader_close.\n");
        return false;
    }
    if (reader->file_reader && fclose(reader->file_reader)) {
        fprintf(stderr, "Error: Failed to close file in file_reader_close.\n");
        return false;
    }
    if (reader->file_path) {
        free(reader->file_path);
        reader->file_path = NULL;
    }

    reader->is_open = false;
    free(reader); // Optionally free the FileReader object itself if it's dynamically allocated
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
    if (reader->file_reader == NULL) {
        fprintf(stderr, "Error: FileReader object is null and not valid in file_reader_get_position.\n");
        return (size_t)-1;
    }

    long cursor_position = ftell(reader->file_reader);
    if (cursor_position == -1L) {
        fprintf(stderr, "Error: Could not determine file position.\n");
        return (size_t)-1;
    }
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
        fprintf(stderr, "Error: FileReader pointer is NULL in file_reader_is_open.\n");
        return false;
    }
    if (reader->file_reader == NULL) {
        fprintf(stderr, "Error: FileReader object is NULL and its not open in file_reader_is_open.\n");
        return false;
    }
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
        fprintf(stderr, "Error: FileReader object is invalid or NULL in file_reader_set_encoding.\n");
        return false;
    }
    if (!(encoding >= READ_ENCODING_UTF16 && encoding <= READ_ENCODING_UTF32)) {
        fprintf(stderr, "Error: Encoding type is Invalid in file_reader_set_encoding.\n");
        return false;
    }
    reader->encoding = encoding;
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
const char *file_reader_get_file_name(FileReader *reader){
    if (!reader || reader->file_reader == NULL) {
        fprintf(stderr, "Error: FileReader object is null and not valid in file_reader_get_file_name.\n");
        return NULL;
    }
    if (!reader->file_path) {
        fprintf(stderr, "Error: file path for FileReader is null in file_reader_get_file_name.\n");
        return NULL;
    }
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
        fprintf(stderr, "Error: FileReader object is null and invalid in file_reader_seek.\n");
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
            fprintf(stderr,"Warning: Cursor position is Invalid defailt pos is POS_BEGIN in file_reader_seek.\n");
            pos = SEEK_SET;
            break;
    }
    
    if (fseek(reader->file_reader, offset, pos) != 0) {
        fprintf(stderr, "Error: fseek failed in file_reader_seek.\n");
        return false;
    }
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
        fprintf(stderr, "Error: FileReader object is NULL and invalid in file_reader_eof.\n");
        return false;
    }
    return feof(reader->file_reader) != 0;
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
        fprintf(stderr, "Error: FileReader object is not valid and NULL in file_reader_get_size.\n");
        return 0;
    }
    long current_position = file_reader_get_position(reader);
    if (fseek(reader->file_reader, 0, SEEK_END) != 0) {
        fprintf(stderr, "Error: fseek failed to seek to end of file in file_reader_get_size.\n");
        return 0;
    }

    size_t size = file_reader_get_position(reader);
    if (fseek(reader->file_reader, current_position, SEEK_SET) != 0) {
        fprintf(stderr, "Error: fseek failed to return to original position in file_reader_get_size.\n");
    }
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
        fprintf(stderr, "Error: Invalid argument in file_reader_read.\n");
        return 0;
    }

    // For binary reading, no encoding conversion is needed
    if (reader->mode == READ_BINARY || reader->mode == READ_UNBUFFERED || reader->mode == READ_BUFFERED) {
        return fread(buffer, size, count, reader->file_reader);
    }

    // For text reading, handle UTF-16 to UTF-8 conversion
    if (reader->mode == READ_TEXT || reader->mode == READ_UNICODE) {
        char* rawBuffer = (char*)malloc(sizeof(char) * (count + 1));  // +1 for null-termination
        if (!rawBuffer) {
            fprintf(stderr, "Error: Memory allocation failed in file_reader_read.\n");
            return 0;
        }

        size_t actualRead = fread(rawBuffer, sizeof(char), count, reader->file_reader);
        
        rawBuffer[actualRead] = '\0';  // Null-terminate the buffer
        memcpy(buffer, rawBuffer, actualRead);
        free(rawBuffer);  // Free the UTF-8 buffer

        return actualRead;
    }

    fprintf(stderr, "Unsupported read mode in file_reader_read.\n");
    return 0;
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
        fprintf(stderr, "Error: Invalid argument in file_reader_read_line.\n");
        return false;
    }

    if (reader->encoding == READ_ENCODING_UTF16 && reader->mode == READ_UNICODE) {
        // For UTF-16 encoded files
        wchar_t wBuffer[1024];
        if (fgetws(wBuffer, 1024, reader->file_reader) == NULL) {
            if (!feof(reader->file_reader)) {
                fprintf(stderr, "Error: Failed to read line in UTF-16 mode.\n");
            }
            return false;
        }

        // Convert wchar buffer to UTF-8
        char* utf8Buffer = encoding_wchar_to_utf8(wBuffer);
        if (!utf8Buffer) {
            fprintf(stderr, "Error: Conversion to UTF-8 failed in file_reader_read_line.\n");
            return false;
        }

        strncpy(buffer, utf8Buffer, size - 1);
        buffer[size - 1] = '\0';
        free(utf8Buffer);
    } else {
        // For UTF-8 and other encodings
        if (fgets(buffer, size, reader->file_reader) == NULL) {
            if (!feof(reader->file_reader)) {
                fprintf(stderr, "Error: Failed to read line in non-UTF-16 mode.\n");
            }
            return false;
        }

        // Remove newline characters
        buffer[strcspn(buffer, "\r\n")] = '\0';
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
        fprintf(stderr, "Error: Invalid argument in file_reader_read_fmt.\n");
        return 0;
    }

    wchar_t wBuffer[1024]; // Wide character buffer for UTF-16 data
    if (fgetws(wBuffer, sizeof(wBuffer) / sizeof(wchar_t), reader->file_reader) == NULL) {
        return 0; // No data read or error occurred
    }

    // Convert UTF-16 to UTF-8
    char* utf8Buffer = encoding_wchar_to_utf8(wBuffer);
    if (!utf8Buffer) {
        fprintf(stderr, "Error: UTF-16 to UTF-8 conversion failed in file_reader_read_fmt.\n");
        return 0;
    }

    va_list args;
    va_start(args, format);

    size_t read = vsscanf(utf8Buffer, format, args);

    va_end(args);

    free(utf8Buffer);
    return read; // Number of items successfully read
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
        fprintf(stderr, "Error: Invalid argument in file_reader_copy.\n");
        return false;
    }

    wchar_t wBuffer[1024]; // Wide character buffer for UTF-16 data
    size_t bytesRead, bytesToWrite;

    while ((bytesRead = fread(wBuffer, sizeof(wchar_t), 1024, src_reader->file_reader)) > 0) {
        char* utf8Buffer = NULL;
        size_t utf8BufferSize = 0;

        // Convert the read data to UTF-8 if necessary
        switch (src_reader->encoding) {
            case READ_ENCODING_UTF16:
                utf8Buffer = encoding_wchar_to_utf8(wBuffer);
                if (!utf8Buffer) {
                    fprintf(stderr, "Error: Conversion to UTF-8 failed in file_reader_copy.\n");
                    return false;
                }
                utf8BufferSize = string_length_utf8(utf8Buffer);
                break;

            default:
                fprintf(stderr, "Unsupported encoding in file_reader_copy.\n");
                return false;
        }

        // Write the UTF-8 data to the destination file
        bytesToWrite = utf8BufferSize;
        size_t bytesWritten = file_writer_write(utf8Buffer, sizeof(char), bytesToWrite, dest_writer);
        free(utf8Buffer);

        if (bytesWritten < bytesToWrite) {
            fprintf(stderr, "Error: Could not write all data in file_reader_copy.\n");
            return false;
        }
    }
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
        fprintf(stderr, "Error: Invalid arguments in file_reader_read_lines.\n");
        return false;
    }

    *buffer = malloc(num_lines * sizeof(char*));
    if (!*buffer) {
        fprintf(stderr, "Error: Memory allocation failed in file_reader_read_lines.\n");
        return false;
    }

    size_t lines_read = 0;
    char line_buffer[1024]; // Adjust the size as needed

    while (lines_read < num_lines && !feof(reader->file_reader)) {
        if (file_reader_read_line(line_buffer, sizeof(line_buffer), reader)) {
            (*buffer)[lines_read] = string_strdup(line_buffer);
            if (!(*buffer)[lines_read]) {
                fprintf(stderr, "Error: Memory allocation failed for line in file_reader_read_lines.\n");
                // Free previously allocated lines
                for (size_t i = 0; i < lines_read; ++i) {
                    free((*buffer)[i]);
                }
                free(*buffer);
                return false;
            }
            lines_read++;
        }
    }
    return lines_read == num_lines;
}
