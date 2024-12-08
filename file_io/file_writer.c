/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class FileWriter
*/

#if defined(_WIN32) || defined(_WIN64) 
#include <windows.h>
#include <io.h> // _get_osfhandle
#else
#define _GNU_SOURCE

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif 

#include <stdlib.h>
#include <string.h>
#include "file_writer.h"
#include "../encoding/encoding.h"
#include "../string/std_string.h"


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
        exit(-1);
    }

    FileWriter* writer = (FileWriter*) malloc(sizeof(FileWriter));
    if (!writer) {
        FILE_WRITER_LOG("[file_writer_open] Error: Cannot allocate memory for FileWriter.");
        exit(-1);
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

    if (!wMode) {
        FILE_WRITER_LOG("[file_writer_open] Error: Cannot convert mode to wchar.");
        exit(-1);
    }
    if (!wFileName) {
        FILE_WRITER_LOG("[file_writer_open] Error: Cannot convert filename to wchar.");
        exit(-1);
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
        exit(-1);
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
    if (writer->file_writer == NULL) {
        FILE_WRITER_LOG("[file_writer_close] Warning: The file is already NULL, no need to close.");
        return false;
    }
    if (fclose(writer->file_writer)) {
        FILE_WRITER_LOG("[file_writer_close] Error: Failed to close the file.");
        return false;
    }
    writer->is_open = false;
    FILE_WRITER_LOG("[file_writer_close] File successfully closed.");
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
            // Convert UTF-8 to UTF-32 and then write
            uint32_t* utf32Buffer = encoding_utf8_to_utf32((const uint8_t*)buffer, size * count);
            if (!utf32Buffer) {
                FILE_WRITER_LOG("[file_writer_write] Error: Conversion to UTF-32 failed.");
                return 0;
            }
            written = fwrite(utf32Buffer, sizeof(uint32_t), wcslen((wchar_t*)utf32Buffer), writer->file_writer);
            free(utf32Buffer);
            FILE_WRITER_LOG("[file_writer_write] Successfully wrote %zu UTF-32 elements.", written);
            break;
        }

        case WRITE_ENCODING_UTF16: {
            #if defined(_WIN32) || defined(_WIN64)
            // For Windows, if mode requires UTF-16, convert and write
            if (writer->mode == WRITE_UNICODE || writer->mode == WRITE_APPEND) {
                wchar_t* wBuffer = encoding_utf8_to_wchar((const char*)buffer);
                if (!wBuffer) {
                    FILE_WRITER_LOG("[file_writer_write] Error: Conversion to wchar_t failed.");
                    return 0;
                }
                written = fwrite(wBuffer, sizeof(wchar_t), wcslen(wBuffer), writer->file_writer);
                free(wBuffer);
                FILE_WRITER_LOG("[file_writer_write] Successfully wrote %zu UTF-16 elements in Windows mode.", written);
            } 
            else {
                // For non-Unicode modes, write directly
                written = fwrite(buffer, size, count, writer->file_writer);
                FILE_WRITER_LOG("[file_writer_write] Successfully wrote %zu elements without conversion.", written);
            }
            #else
            // For non-Windows systems, convert UTF-8 to UTF-16 if required and write
            if (writer->encoding == WRITE_ENCODING_UTF16) {
                uint16_t* utf16Buffer = encoding_utf8_to_utf16((const uint8_t*)buffer, size * count);
                if (!utf16Buffer) {
                    FILE_WRITER_LOG("[file_writer_write] Error: Conversion to UTF-16 failed.");
                    return 0;
                }
                written = fwrite(utf16Buffer, sizeof(uint16_t), wcslen((wchar_t*)utf16Buffer), writer->file_writer);
                free(utf16Buffer);
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

        // Other encoding types or default handling can be added here
        default:
            // For safety, default to direct writing for undefined encoding types
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

    size_t written = 0;
    size_t elementToWriteSize = size;

    #if defined(_WIN32) || defined(_WIN64)
    if (writer->mode == WRITE_UNICODE) {
        wchar_t* wBuffer = encoding_utf8_to_wchar(buffer);
        if (!wBuffer) {
            FILE_WRITER_LOG("[file_writer_write_line] Error: Cannot convert buffer to wchar.");
            return false;
        }

        elementToWriteSize = wcslen(wBuffer);
        written = fwrite(wBuffer, sizeof(wchar_t), elementToWriteSize, writer->file_writer);
        free(wBuffer);
        FILE_WRITER_LOG("[file_writer_write_line] Successfully wrote %zu UTF-16 elements.", written);
    } 
    else {
        written = fwrite(buffer, sizeof(char), elementToWriteSize, writer->file_writer);
        FILE_WRITER_LOG("[file_writer_write_line] Successfully wrote %zu elements.", written);
    }
    #else 
    written = fwrite(buffer, sizeof(char), size, writer->file_writer);
    FILE_WRITER_LOG("[file_writer_write_line] Successfully wrote %zu elements.", written);
    #endif 

    if (written < elementToWriteSize) {
        FILE_WRITER_LOG("[file_writer_write_line] Error: Could not write the entire buffer.");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    if (writer->mode == WRITE_UNICODE) {
        wchar_t newLine[] = L"\n";
        written = fwrite(newLine, sizeof(wchar_t), 1, writer->file_writer);
        FILE_WRITER_LOG("[file_writer_write_line] Wrote newline (UTF-16).");
    } else {
        char newLine[] = "\n";
        written = fwrite(newLine, sizeof(char), 1, writer->file_writer);
        FILE_WRITER_LOG("[file_writer_write_line] Wrote newline.");
    }

    #else 
    char newLine[] = "\n";
    written = fwrite(newLine, sizeof(char), 1, writer->file_writer);
    FILE_WRITER_LOG("[file_writer_write_line] Wrote newline.");
    #endif 

    return written == 1;
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

    FILE* src_file = fopen(src_writer->file_path, "rb");
    if (!src_file) {
        FILE_WRITER_LOG("[file_writer_copy] Error: Cannot reopen source file for reading.");
        return false;
    }

    FILE* dest_file = fopen(dest_writer->file_path, "wb");
    if (!dest_file) {
        FILE_WRITER_LOG("[file_writer_copy] Error: Cannot reopen destination file for writing.");
        fclose(src_file);
        return false;
    }

    char buffer[4096];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, sizeof(char), sizeof(buffer), src_file))) {
        if (fwrite(buffer, sizeof(char), bytes_read, dest_file) != bytes_read) {
            FILE_WRITER_LOG("[file_writer_copy] Error: Failed to write data to the destination file.");
            fclose(src_file);
            fclose(dest_file);
            return false;
        }
    }

    fclose(src_file);
    fclose(dest_file);

    FILE_WRITER_LOG("[file_writer_copy] File content copied successfully.");
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

    char buffer[2048]; // Adjust buffer size as necessary
    vsnprintf(buffer, sizeof(buffer), format, args);

    // Write the formatted string to the file
    size_t written = fwrite(buffer, 1, strlen(buffer), writer->file_writer);
    fflush(writer->file_writer);

    va_end(args);

    if (written != strlen(buffer)) {
        FILE_WRITER_LOG("[file_writer_write_fmt] Error: Failed to write full data to the file.");
    } 
    else {
        FILE_WRITER_LOG("[file_writer_write_fmt] Successfully written %zu characters to the file.", written);
    }

    return written;
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

    size_t current_position = file_writer_get_position(writer);
    if (fseek(writer->file_writer, 0, SEEK_END) != 0) {
        FILE_WRITER_LOG("[file_writer_get_size] Error: Failed to seek to end of file.");
        return 0;
    }

    size_t size = file_writer_get_position(writer);
    if (fseek(writer->file_writer, current_position, SEEK_SET) != 0) {
        FILE_WRITER_LOG("[file_writer_get_size] Error: Failed to return to original position.");
    }

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

    for (size_t i = 0; i < count; ++i) {
        const void* buffer = buffers[i];
        size_t size = sizes[i];
        all_bytes += size;

        if (!buffer || size == 0) {
            FILE_WRITER_LOG("[file_writer_write_batch] Error: Invalid buffer or size at index %zu.", i);
            continue;
        }

        size_t written = 0;
        void* convertedBuffer = NULL; 
        size_t convertedSize = 0;     

        // Convert buffer based on encoding
        switch (writer->encoding) {
            case WRITE_ENCODING_UTF32: {
                uint32_t* utf32Buffer = encoding_utf8_to_utf32((const uint8_t*)buffer, size);
                if (!utf32Buffer) {
                    FILE_WRITER_LOG("[file_writer_write_batch] Error: Conversion to UTF-32 failed at index %zu.", i);
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
                    FILE_WRITER_LOG("[file_writer_write_batch] Error: Conversion to wchar_t failed at index %zu.", i);
                    continue;
                }
                convertedBuffer = wBuffer;
                convertedSize = wcslen(wBuffer) * sizeof(wchar_t);
                #else
                uint16_t* utf16Buffer = encoding_utf8_to_utf16((const uint8_t*)buffer, size);
                if (!utf16Buffer) {
                    FILE_WRITER_LOG("[file_writer_write_batch] Error: Conversion to UTF-16 failed at index %zu.", i);
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
            FILE_WRITER_LOG("[file_writer_write_batch] Error: Partial or failed write at index %zu.", i);
            return false;
        }

        total_written += written;
    }

    if (writer->mode == WRITE_UNICODE) {
        all_bytes *= 2; // Because we use wchar_t in Unicode
    }

    FILE_WRITER_LOG("[file_writer_write_batch] Successfully wrote %zu bytes.", total_written);
    return total_written == all_bytes;
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

    char buffer[2048]; 
    vsnprintf(buffer, sizeof(buffer), format, args);

    size_t written = file_writer_write(buffer, strlen(buffer), 1, writer);

    va_end(args);

    FILE_WRITER_LOG("[file_writer_append_fmt] Successfully appended %zu characters to the file.", written);
    return written > 0;
}

