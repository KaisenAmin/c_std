# FileReader and FileWriter Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2024
**License:** ISC License

## Overview

This library provides a comprehensive set of functions for reading from and writing to files in various modes and encodings. It's designed for flexibility and ease of use in handling both text and binary data, with special support for Unicode (UTF-16 and UTF-32) encoding. The library simplifies file operations in C, offering high-level abstractions for common tasks.

## Key Features

- **Multiple Modes:** Support for reading and writing in text, binary, buffered, unbuffered, and Unicode modes.
- **Encoding Support:** Handles UTF-16 and UTF-32 encodings, with automatic conversion for Unicode operations.
- **Utility Functions:** Offers functions for copying data between files, locking files for exclusive access, and more.

## Function Descriptions

**FileWriter Functions**

### `FileWriter* file_writer_open(const char* filename, const WriteMode mode)`
**Purpose**: Opens a file for writing using the specified mode, creating it if it does not exist or truncating it if it does (except in append mode).
**Parameters**:
- `filename`: Path to the file to open for writing.
- `mode`: Write mode (`WRITE_TEXT`, `WRITE_BINARY`, `WRITE_UNICODE`, `WRITE_BUFFERED`, `WRITE_UNBUFFERED`, or `WRITE_APPEND`).
**Return Value**: A `FileWriter*` on success, or `NULL` on failure.
**Usage Case**: Use this to create or overwrite a file and obtain a writer handle before calling any write functions.

---

### `FileWriter* file_writer_append(const char* filename, const WriteMode mode)`
**Purpose**: Opens a file in append mode so new data is added at the end without erasing existing content, creating the file if it does not exist.
**Parameters**:
- `filename`: Path to the file to open for appending.
- `mode`: Write mode (`WRITE_TEXT`, `WRITE_BINARY`, `WRITE_UNICODE`, `WRITE_BUFFERED`, `WRITE_UNBUFFERED`, or `WRITE_APPEND`).
**Return Value**: A `FileWriter*` on success, or `NULL` on failure.
**Usage Case**: Use this when you need to add data to the end of an existing file without losing its current contents.

---

### `bool file_writer_close(FileWriter* writer)`
**Purpose**: Flushes all buffered data, closes the underlying file, and frees the `FileWriter` struct.
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure to close.
**Return Value**: `true` on success, `false` if the file was already closed or `fclose` failed.
**Usage Case**: Always call this when you are done writing to properly release file resources and ensure all data is persisted.

---

### `bool file_writer_is_open(FileWriter* writer)`
**Purpose**: Returns whether the file associated with the writer is currently open.
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure to check.
**Return Value**: `true` if the file is open, `false` otherwise.
**Usage Case**: Use this as a guard before performing write operations to confirm the writer is in a valid open state.

---

### `bool file_writer_flush(FileWriter* writer)`
**Purpose**: Forces all buffered data to be written to disk immediately.
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure to flush.
**Return Value**: `true` on success, `false` if `fflush` failed or `writer` is invalid.
**Usage Case**: Use this when you need data durability at a specific point without closing the file.

---

### `size_t file_writer_write(void* buffer, size_t size, size_t count, FileWriter* writer)`
**Purpose**: Writes `count` elements of `size` bytes each from `buffer` to the file, handling encoding conversion for Unicode mode.
**Parameters**:
- `buffer`: Pointer to the data to write.
- `size`: Size in bytes of each element.
- `count`: Number of elements to write.
- `writer`: A pointer to the `FileWriter` structure.
**Return Value**: The number of elements written, or `0` on error.
**Usage Case**: Use this as the primary raw write function for arbitrary binary or text data.

---

### `bool file_writer_write_line(char* buffer, size_t size, FileWriter* writer)`
**Purpose**: Writes `size` bytes from `buffer` to the file and appends a newline character, handling UTF-16 encoding on Windows in Unicode mode.
**Parameters**:
- `buffer`: Pointer to the text to write.
- `size`: Number of bytes from `buffer` to write.
- `writer`: A pointer to the `FileWriter` structure.
**Return Value**: `true` on success, `false` on error.
**Usage Case**: Use this when writing line-oriented text content and you want the newline appended automatically.

---

### `size_t file_writer_write_fmt(FileWriter* writer, const char* format, ...)`
**Purpose**: Writes a `printf`-style formatted string to the file.
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure.
- `format`: A `printf`-style format string followed by corresponding arguments.
**Return Value**: The number of characters written, or `0` on error.
**Usage Case**: Use this to write structured or human-readable formatted text without manually constructing the string first.

---

### `bool file_writer_write_batch(FileWriter* writer, const void** buffers, const size_t* sizes, size_t count)`
**Purpose**: Writes `count` separate buffers to the file in a single batch, reducing system-call overhead and handling encoding conversion as needed.
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure.
- `buffers`: Array of pointers to the data buffers to write.
- `sizes`: Array of byte sizes corresponding to each buffer.
- `count`: Number of buffers to write.
**Return Value**: `true` if all buffers were written, `false` on any error.
**Usage Case**: Use this to efficiently write multiple discontiguous data chunks in one operation.

---

### `bool file_writer_append_fmt(FileWriter* writer, const char* format, ...)`
**Purpose**: Formats a string with `printf`-style specifiers and appends it to the file; the `FileWriter` must be in `WRITE_APPEND` mode.
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure in append mode.
- `format`: A `printf`-style format string followed by corresponding arguments.
**Return Value**: `true` on success, `false` on error.
**Usage Case**: Use this to append formatted text entries (such as log lines) to an existing file.

---

### `bool file_writer_copy(FileWriter* src_writer, FileWriter* dest_writer)`
**Purpose**: Copies the entire content of the file managed by `src_writer` into the file managed by `dest_writer`.
**Parameters**:
- `src_writer`: A pointer to the source `FileWriter` structure.
- `dest_writer`: A pointer to the destination `FileWriter` structure.
**Return Value**: `true` on success, `false` if any read or write step fails.
**Usage Case**: Use this to duplicate a file's content into another file using writer handles you already have open.

---

### `size_t file_writer_get_position(FileWriter* writer)`
**Purpose**: Returns the current byte offset of the file pointer.
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure.
**Return Value**: The current file pointer position as `size_t`, or `(size_t)-1` on error.
**Usage Case**: Use this to track how many bytes have been written or to record a position for later seeking.

---

### `size_t file_writer_get_size(FileWriter* writer)`
**Purpose**: Flushes pending writes and returns the total file size in bytes.
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure.
**Return Value**: The file size in bytes, or `0` on error.
**Usage Case**: Use this to determine the full size of the file at any point during writing.

---

### `bool file_writer_seek(FileWriter* writer, long offset, const CursorPosition cursor_pos)`
**Purpose**: Moves the file pointer by `offset` bytes relative to `cursor_pos` (`POS_BEGIN`, `POS_END`, or `POS_CURRENT`).
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure.
- `offset`: Number of bytes to move the file pointer.
- `cursor_pos`: Reference position for the offset (`POS_BEGIN`, `POS_END`, or `POS_CURRENT`).
**Return Value**: `true` on success, `false` on error.
**Usage Case**: Use this to enable random-access writing by repositioning the file pointer before a write.

---

### `bool file_writer_truncate(FileWriter* writer, size_t size)`
**Purpose**: Truncates or extends the file to exactly `size` bytes, with extended space zero-filled.
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure.
- `size`: The target file size in bytes.
**Return Value**: `true` on success, `false` on error.
**Usage Case**: Use this to shrink a file to discard trailing data or to pre-allocate space by extending it.

---

### `bool file_writer_lock(FileWriter* writer)`
**Purpose**: Acquires an exclusive lock on the file, preventing other processes from modifying it.
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure.
**Return Value**: `true` if the lock was obtained, `false` on error.
**Usage Case**: Use this before a critical write section to ensure no other process can modify the file concurrently.

---

### `bool file_writer_unlock(FileWriter* writer)`
**Purpose**: Releases the exclusive lock held on the file.
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure.
**Return Value**: `true` on success, `false` on error.
**Usage Case**: Use this after a locked write section is complete to allow other processes to access the file again.

---

### `bool file_writer_set_encoding(FileWriter* writer, const WriteEncodingType encoding)`
**Purpose**: Sets the character encoding used for subsequent writes (`WRITE_ENCODING_UTF16` or `WRITE_ENCODING_UTF32`).
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure.
- `encoding`: The desired encoding type (`WRITE_ENCODING_UTF16` or `WRITE_ENCODING_UTF32`).
**Return Value**: `true` on success, `false` on error.
**Usage Case**: Use this to switch the encoding mid-stream when writing multi-encoding files.

---

### `const char* file_writer_get_encoding(FileWriter* writer)`
**Purpose**: Returns a string describing the current encoding type (e.g. `"ENCODING_UTF16"`).
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure.
**Return Value**: A string describing the current encoding, or `NULL` on error.
**Usage Case**: Use this to confirm or log the active encoding before performing encoding-sensitive write operations.

---

### `const char* file_writer_get_file_name(FileWriter* writer)`
**Purpose**: Returns the file path string stored in the `FileWriter` struct.
**Parameters**:
- `writer`: A pointer to the `FileWriter` structure.
**Return Value**: The file path string, or `NULL` if `writer` is invalid.
**Usage Case**: Use this to retrieve or log the path of the file currently managed by the writer.

---

**FileReader Functions**

### `FileReader* file_reader_open(const char* filename, const ReadMode mode)`
**Purpose**: Opens a file for reading using the specified mode, returning a reader handle.
**Parameters**:
- `filename`: Path to the file to open for reading.
- `mode`: Read mode (`READ_TEXT`, `READ_BINARY`, `READ_UNICODE`, `READ_BUFFERED`, `READ_UNBUFFERED`, or `READ_LINE`).
**Return Value**: A `FileReader*` on success, or `NULL` if the file cannot be opened.
**Usage Case**: Use this to obtain a reader handle before calling any read functions.

---

### `bool file_reader_close(FileReader* reader)`
**Purpose**: Closes the file and frees the `FileReader` struct.
**Parameters**:
- `reader`: A pointer to the `FileReader` structure to close.
**Return Value**: `true` on success, `false` on error.
**Usage Case**: Always call this when you are done reading to release file handles and allocated memory.

---

### `bool file_reader_is_open(FileReader* reader)`
**Purpose**: Returns whether the file associated with the reader is currently open.
**Parameters**:
- `reader`: A pointer to the `FileReader` structure to check.
**Return Value**: `true` if the file is open, `false` otherwise.
**Usage Case**: Use this as a guard before reading to confirm the reader is in a valid open state.

---

### `bool file_reader_eof(FileReader* reader)`
**Purpose**: Returns whether the file pointer has reached the end of the file.
**Parameters**:
- `reader`: A pointer to the `FileReader` structure.
**Return Value**: `true` if at end of file, `false` otherwise.
**Usage Case**: Use this in read loops to detect when there is no more data to consume.

---

### `size_t file_reader_read(void* buffer, size_t size, size_t count, FileReader* reader)`
**Purpose**: Reads `count` elements of `size` bytes each from the file into `buffer`.
**Parameters**:
- `buffer`: Pointer to the destination buffer.
- `size`: Size in bytes of each element.
- `count`: Number of elements to read.
- `reader`: A pointer to the `FileReader` structure.
**Return Value**: The number of elements read, or `0` on error.
**Usage Case**: Use this as the primary raw read function for arbitrary binary or text data.

---

### `bool file_reader_read_line(char* buffer, size_t size, FileReader* reader)`
**Purpose**: Reads one line of text (up to `size - 1` chars plus null terminator) into `buffer`, handling UTF-16/UTF-8 conversion where needed.
**Parameters**:
- `buffer`: Pointer to the destination buffer for the line.
- `size`: Maximum number of characters to read, including the null terminator.
- `reader`: A pointer to the `FileReader` structure.
**Return Value**: `true` if a line was read, `false` at EOF or on error.
**Usage Case**: Use this to process a text file one line at a time.

---

### `size_t file_reader_read_fmt(FileReader* reader, const char* format, ...)`
**Purpose**: Reads formatted data from the file using a `scanf`-style format string.
**Parameters**:
- `reader`: A pointer to the `FileReader` structure.
- `format`: A `scanf`-style format string followed by pointers to the variables to fill.
**Return Value**: The number of items successfully matched, or `0` on error.
**Usage Case**: Use this to parse structured data from a file in a known format without manual string splitting.

---

### `bool file_reader_read_lines(FileReader* reader, char*** buffer, size_t num_lines)`
**Purpose**: Reads exactly `num_lines` lines from the file into a dynamically allocated array of strings stored at `*buffer`; the caller must `free` each string and the array.
**Parameters**:
- `reader`: A pointer to the `FileReader` structure.
- `buffer`: Output parameter that receives a pointer to the allocated array of line strings.
- `num_lines`: Number of lines to read.
**Return Value**: `true` on success, `false` on error.
**Usage Case**: Use this to bulk-read a fixed number of lines into memory for batch processing.

---

### `bool file_reader_copy(FileReader* src_reader, FileWriter* dest_writer)`
**Purpose**: Copies the full content of the source file into the destination file, converting encoding if necessary.
**Parameters**:
- `src_reader`: A pointer to the source `FileReader` structure.
- `dest_writer`: A pointer to the destination `FileWriter` structure.
**Return Value**: `true` on success, `false` on error.
**Usage Case**: Use this to duplicate or migrate file content using already-open reader and writer handles.

---

### `size_t file_reader_get_position(FileReader* reader)`
**Purpose**: Returns the current byte offset of the file pointer.
**Parameters**:
- `reader`: A pointer to the `FileReader` structure.
**Return Value**: The current file pointer position as `size_t`, or `(size_t)-1` on error.
**Usage Case**: Use this to record the current position so it can be restored later with `file_reader_seek`.

---

### `size_t file_reader_get_size(FileReader* reader)`
**Purpose**: Returns the total file size in bytes.
**Parameters**:
- `reader`: A pointer to the `FileReader` structure.
**Return Value**: The file size in bytes, or `0` on error.
**Usage Case**: Use this to know how much data to allocate or how many bytes remain to read.

---

### `bool file_reader_seek(FileReader* reader, long offset, const CursorPosition cursor_pos)`
**Purpose**: Moves the file pointer by `offset` bytes relative to `cursor_pos`.
**Parameters**:
- `reader`: A pointer to the `FileReader` structure.
- `offset`: Number of bytes to move the file pointer.
- `cursor_pos`: Reference position for the offset (`POS_BEGIN`, `POS_END`, or `POS_CURRENT`).
**Return Value**: `true` on success, `false` on error.
**Usage Case**: Use this to implement random-access reading or to rewind to a previously saved position.

---

### `bool file_reader_set_encoding(FileReader* reader, const ReadEncodingType encoding)`
**Purpose**: Sets the encoding type for reading (`READ_ENCODING_UTF16` or `READ_ENCODING_UTF32`).
**Parameters**:
- `reader`: A pointer to the `FileReader` structure.
- `encoding`: The desired encoding type (`READ_ENCODING_UTF16` or `READ_ENCODING_UTF32`).
**Return Value**: `true` on success, `false` on error.
**Usage Case**: Use this when the file uses a non-default encoding and you need correct character interpretation during reads.

---

### `const char* file_reader_get_file_name(FileReader* reader)`
**Purpose**: Returns the absolute file path stored in the `FileReader` struct.
**Parameters**:
- `reader`: A pointer to the `FileReader` structure.
**Return Value**: The absolute file path string, or `NULL` if `reader` is invalid.
**Usage Case**: Use this to retrieve or log the path of the file currently managed by the reader.

---

## Compilation

For compiling C programs using this library, standard C compilation commands apply, ensuring the `file_io`, `fmt`, `encoding`, and `string` modules are correctly referenced.


## Example 1 : How to write in file in `unicode mode` with `file_writer_write` and `file_writer_open`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include <string.h>

int main() {
    FileWriter* writer = file_writer_open("./sources/test_uni.txt", WRITE_UNICODE);
    const char* unicodeContent = "سلام به همه دوستان.\n"; // Example in Persian

    size_t unicodeWritten = file_writer_write((void*)unicodeContent, strlen(unicodeContent), sizeof(char), writer);
    fmt_printf("Written %zu item(s) to Unicode file.\n", unicodeWritten);

    file_writer_close(writer);
    return 0;
}
```
**Result**
```
Written 20 item(s) to Unicode file.
```

---

## Example 2 : how to get cursor position of file with `file_writer_get_position`

This confirms that each character in your string is taking up more than one byte due to UTF-8 encoding, explaining why the file position in your earlier message was greater than the number of characters in the string.
```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include <string.h>

int main() {
    FileWriter* writer = file_writer_open("./sources/test_uni.txt", WRITE_UNICODE);
    const char* unicodeContent = "سلام امین\n"; // Example in Persian

    size_t unicodeWritten = file_writer_write((void*)unicodeContent, strlen(unicodeContent), sizeof(char), writer);
    fmt_printf("Written %zu item(s) to Unicode file.\n", unicodeWritten);
    fmt_printf("Cursor position is %zu\n", file_writer_get_position(writer));
    

    file_writer_close(writer);
    return 0;
}
```
**Result**
```
Written 10 item(s) to Unicode file.
Cursor position is 24
```

---

## Example 3 : Using `file_writer_write_line`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include "string/std_string.h"

int main() {
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_UNICODE);
    String* buffer = string_create("سلام به همه دوستان و برنامه نویسان عزیز\n"); 

    // or you can use file_writer_is_open(writer)
    if (writer->is_open) { 
        bool written = file_writer_write_line((char*)string_c_str(buffer), string_length(buffer), writer);

        if (written) {
            fmt_print("write in file was successfully.\n");
        }
        else {
            fmt_print("Can not write to file.\n");
        }
        file_writer_close(writer);
    }
    else {
        fmt_print("file is opened\n");
    }

    string_deallocate(buffer);
    return 0;
}
```
**Result**
```
write in file was successfully.
```

---

## Example 4 : Using most of the FileWriter functions 

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include "string/std_string.h"
#include <string.h>
#include <stdlib.h>


int main() {
    const char* unicodeString = "سلام به همه دوستان\n"; // "Hello to all friends" in Persian
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_UNICODE);

    if (!writer || !file_writer_is_open(writer)) {
        fmt_printf("Failed to open file for writing.\n");
        return 1;
    }

    // Write the Unicode string to the file
    size_t written = file_writer_write((void*)unicodeString, strlen(unicodeString), sizeof(char), writer);
    if (written == 0) {
        fmt_printf("Failed to write to file.\n");
        file_writer_close(writer);

        return 1;
    }

    // Flush the buffer to ensure all data is written
    if (!file_writer_flush(writer)) {
        fmt_printf("Failed to flush the file writer.\n");
        return -1;
    }

    // Get and print the current position in the file
    size_t position = file_writer_get_position(writer);
    char* positionStr = string_from_int_cstr((int)position);
    fmt_println("Current position in the file:", positionStr);
    free(positionStr);

    // Close the file
    if (!file_writer_close(writer)) {
        fmt_printf("Failed to close the file writer.\n");
        return -1;
    }

    // Open the same file in append mode
    writer = file_writer_append("./sources/example_unicode.txt", WRITE_UNICODE);
    if (!writer || !file_writer_is_open(writer)) {
        fmt_printf("Failed to open file for appending.\n");
        return 1;
    }

    const char* additionalString = "اضافه کردن متن به فایل\n"; 
    if (!file_writer_write_line((char*)additionalString, strlen(additionalString), writer)) {
        fmt_printf("Failed to write line to file.\n");
        file_writer_close(writer);

        return 1;
    }

    if (!file_writer_close(writer)) {
        fmt_printf("Failed to close the file writer.\n");
    }

    return 0;
}
```
**Result**
```
Current position in the file: 38
```

---

## Example 5: how to use `file_writer_write_fmt` in `FileWriter`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"

int main() {
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_APPEND);
    size_t write = file_writer_write_fmt(writer, "Hello %s your age is %d and programming in %s", "امین", 27, "C");
    
    if (write > 0) {
        fmt_printf("Its Correct %zu\n", write);
    }
    else {
        fmt_printf("Failed to write formated str\n");
    }

    file_writer_close(writer);
    return 0;
}
```
**Result**
```
Its Correct 50
```

---

## Example 6 : get file size in `FileWriter` with `file_writer_get_size`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"

int main() {
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_APPEND);

    size_t file_size = file_writer_get_size(writer);
    fmt_printf("Size of file is %zu", file_size);

    file_writer_close(writer);
    return 0;
}
```
**Result**
```
Size of file is 113
```
> Note: The exact size reflects the accumulated bytes written by Examples 3–5 in sequence.

---

## Example 7 : get file_path in `FileWriter` with `file_writer_get_file_name` also file encoding with `file_writer_get_encoding`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include <stdlib.h>

int main() {
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_APPEND);

    fmt_printf("filePath is : %s\n", file_writer_get_file_name(writer)); // or writer->file_path
    char* encoding = file_writer_get_encoding(writer);
    fmt_printf("Encoding Type is : %s\n", encoding);

    free(encoding);
    file_writer_close(writer);

    return 0;
}
```
**Result**
```
filePath is : ./sources/text_uni.txt
Encoding Type is : ENCODING_UTF16
```

---

## Example 8 : how to copy inforamation of file to an other file in `FileWriter` with `file_writer_copy`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"

int main() {
    // We must open the source file in append mode so that the information we want to copy is not deleted.
    FileWriter* src_writer = file_writer_open("./sources/text_uni.txt", WRITE_APPEND);
    FileWriter* dest_writer = file_writer_open("./sources/destination_file.txt", WRITE_UNICODE);

    if (file_writer_copy(src_writer, dest_writer)) {
        fmt_printf("File copy successful.\n");
    } 
    else {
        fmt_printf("File copy failed.\n");
    }

    file_writer_close(src_writer);
    file_writer_close(dest_writer);

    return 0;
}
```
**Result**
```
File copy successful.
```

---

## Example 9 : how to lock and unlock file for prevent other proccess to modifying it with `file_writer_lock & unlock`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include <string.h>

int main() {
    FileWriter* writer = file_writer_open("./sources/locked_file.txt", WRITE_APPEND);

    // Attempt to lock the file
    if (!file_writer_lock(writer)) {
        fmt_printf("Error: failed to lock the file.\n");
        file_writer_close(writer);

        return -1;
    }

    const char* content = "This is a test content.\n";
    if (!file_writer_write((void*)content, strlen(content), sizeof(char), writer)) {
        fmt_printf("Error: failed to write to the file.\n");
    }

    // Unlock the file after writing
    if (!file_writer_unlock(writer)) {
        fmt_printf("Error: failed to unlock the file.\n");
    }

    if (!file_writer_close(writer)) {
        fmt_printf("Error: failed to close the file writer.\n");
    }

    return 0;
}
```
**Result**
```
(no console output — file is locked, written to, unlocked, and closed)
```

---

## Example 10 : how to move cursor position in `FileWriter` with `file_writer_seek`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"

int main() {
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_APPEND);
    
    if (file_writer_seek(writer, 10, POS_BEGIN)) {
        fmt_printf("Successfully moved the file cursor.\n");
    }
    else {
        fmt_printf("Failed to move the file cursor.\n");
    }

    file_writer_close(writer);
    return 0;
}
```
**Result**
```
Successfully moved the file cursor.
```

---

## Example 11 : how to truncate the file in `FileWriter` with `file_writer_truncate`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include <string.h>


int main() {
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_TEXT);
    const char* message = "Hello C/C++ Programmers in all over the world.";

    if (!file_writer_write_line((char*)message, strlen(message), writer)) {
        fmt_printf("Failed to write line in file\n");
        return -1;
    }

    size_t trunc_size = 10;
    if (file_writer_truncate(writer, trunc_size)) {
        fmt_printf("File Successfully truncated to %zu bytes.\n", trunc_size);
    }
    else {
        fmt_printf("Failed to truncate the file.\n");
    }

    file_writer_close(writer);
    return 0;
}
```
**Result**
```
File Successfully truncated to 10 bytes.
```

---

## Example 12 : how to write multiple buffer in `FileWriter` with `file_writer_write_batch`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include "string/std_string.h"

int main() {
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_UNICODE);

    const char* englishText = "Hello, this is a test text.";
    const char* persianText = "سلام، این یک متن آزمایشی است."; // Unicode content
    const void* buffers[2] = { englishText, persianText };
    size_t sizes[2] = { string_length_cstr(englishText), string_length_utf8(persianText) };
    
    // Write both buffers in a single batch operation
    if (!file_writer_write_batch(writer, buffers, sizes, 2)) {
        fmt_printf("Failed to write buffers to file.\n");
        file_writer_close(writer);

        return -1;
    }

    fmt_printf("Successfully wrote buffers to file.\n");

    file_writer_close(writer);
    return 0;
}
```
**Result**
```
Successfully wrote buffers to file.
```

---

## Example 13 : append formated text to file in `FileWriter` with `file_writer_append_fmt`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"

int main() {
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_APPEND);

    // Append formatted text to the file for appending with fmt writer must be in WRITE_APPEND mode
    if (!file_writer_append_fmt(writer, "Name: %s, Age: %d, Occupation: %s\n", "John Doe", 30, "Software Developer")) {
        fmt_printf("Failed to append formatted text to the file.\n");
        file_writer_close(writer);

        return -1;
    }
    if (!file_writer_append_fmt(writer, "This is another line with number: %f\n", 123.456)) {
        fmt_printf("Failed to append second line to the file.\n");
        file_writer_close(writer);

        return -1;
    }

    fmt_printf("Successfully appended formatted text to the file.\n");

    file_writer_close(writer);
    return 0;
}
```
**Result**
```
Successfully appended formatted text to the file.
```

---

## Example 14 : how to reade from file in `FileReader` with `file_reader_open` and `file_reader_read`

```c
#include "file_io/file_reader.h"
#include "fmt/fmt.h"

int main() {
    FileReader* reader = file_reader_open("./sources/text_uni.txt", READ_UNICODE);
    char buffer[1024] = {0};
    size_t read = file_reader_read(buffer, sizeof(char), sizeof(buffer) - 1, reader);

    if (read > 0) {
        fmt_printf("Read %zu bytes from Unicode file.\n", read);
        fmt_printf("Content:\n%s\n", buffer);
    } 
    else {
        fmt_printf("No data read from the file.\n");
    }

    file_reader_close(reader);
    return 0;
}
```
**Result**
```
Read 180 bytes from Unicode file.
Content:
Hello, this is a test text.سلام، این یک متن آزمایشی است.Name: John Doe, Age: 30, Occupation: Software Developer
This is another line with number: 123.456000
```
> Note: Exact content depends on what Examples 12 and 13 wrote to the file.

---

## Example 15 : how to read line by line in `FileReader` with `file_reader_read_line`

```c
#include "file_io/file_reader.h"
#include "fmt/fmt.h"

int main() {
    FileReader* reader = file_reader_open("./sources/text_uni.txt", READ_UNICODE);
    char buffer[1024];
    
    while (file_reader_read_line(buffer, sizeof(buffer), reader)) {
        fmt_printf("Line: %s\n", buffer);
    }

    file_reader_close(reader);
    return 0;
}
```
**Result**
```
Line: Hello, this is a test text.سلام، این یک متن آزمایشی است.Name: John Doe, Age: 30, Occupation: Software Developer

Line: This is another line with number: 123.456000
```

---

## Example 16 : how to read line as formated text in `FileReader` with `file_reader_read_fmt`

```c
#include "file_io/file_writer.h"
#include "file_io/file_reader.h"
#include "fmt/fmt.h"

int main() {
    /* Create a file with known formatted content first */
    FileWriter* writer = file_writer_open("./sources/formatted_data.txt", WRITE_TEXT);
    file_writer_write_fmt(writer, "int: %d, double: %lf, string: %s\n", 10, 20.2, "amin tahmasebi");
    file_writer_close(writer);

    /* Now read it back with a format string */
    FileReader* reader = file_reader_open("./sources/formatted_data.txt", READ_TEXT);
    int intValue;
    double doubleValue;
    char strValue[100];

    size_t itemsRead = file_reader_read_fmt(reader, "int: %d, double: %lf, string: %[^\n]s", &intValue, &doubleValue, strValue);
    if (itemsRead > 0) {
        fmt_printf("Read values: int = %d, double = %lf, string = %s\n", intValue, doubleValue, strValue);
    } 
    else {
        fmt_printf("Failed to read formatted data from file.\n");
    }

    file_reader_close(reader);
    return 0;
}
```
**Result**
```
Read values: int = 10, double = 20.200000, string = amin tahmasebi
```

---

## Example 17 : how to read number of lines from file in `FileReader` with `file_reader_read_lines`

```c
#include "file_io/file_reader.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    FileReader* reader = file_reader_open("./sources/text_uni.txt", READ_UNICODE);
    char** lines;
    size_t num_lines = 5; // Number of lines to read

    if (file_reader_read_lines(reader, &lines, num_lines)) {
        for (size_t i = 0; i < num_lines; ++i) {
            fmt_printf("Line %zu: %s\n", i + 1, lines[i]);
            free(lines[i]); 
        }
        free(lines); 
    } 
    else {
        fmt_printf("Failed to read lines.\n");
    }

    file_reader_close(reader);
    return 0;
}
```
**Result**
```
Line 1: Hello, this is a test text.سلام، این یک متن آزمایشی است.Name: John Doe, Age: 30, Occupation: Software Developer

Line 2: This is another line with number: 123.456000
```

---

## Example 18 : serialize a JSON object to a file then read and parse it back with `json` and `FileWriter`/`FileReader`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include "file_io/file_reader.h"
#include "json/json.h"
#include <stdlib.h>
#include <string.h>

int main(void) {
    JsonElement* root = json_create(JSON_OBJECT);

    JsonElement* name = json_create(JSON_STRING);
    name->value.string_val = strdup("c_std");
    json_add_to_object(root, "library", name);

    JsonElement* version = json_create(JSON_NUMBER);
    version->value.number_val = 2;
    json_add_to_object(root, "version", version);

    /* Serialize to text and persist it with the FileWriter. */
    char* serialized = json_serialize(root);
    FileWriter* writer = file_writer_open("config.json", WRITE_TEXT);

    file_writer_write(serialized, sizeof(char), strlen(serialized), writer);
    file_writer_close(writer);
    free(serialized);
    json_deallocate(root);

    /* Read the raw bytes back with the FileReader. */
    FileReader* reader = file_reader_open("config.json", READ_TEXT);
    char buffer[256] = {0};
    size_t read = file_reader_read(buffer, sizeof(char), sizeof(buffer) - 1, reader);
    file_reader_close(reader);
    fmt_printf("Read %zu bytes back from config.json\n", read);

    /* Parse the text we just read and pull a field out of it. */
    JsonElement* parsed = json_parse(buffer);
    JsonElement* lib = json_get_element(parsed, "library");
    fmt_printf("library = %s\n", lib->value.string_val);
    json_deallocate(parsed);

    remove("config.json");
    return 0;
}
```
**Result**
```
Read 34 bytes back from config.json
library = c_std
```

---

## Example 19 : base64-encode data to a file then read and decode it back with `encoding` and `FileWriter`/`FileReader`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include "file_io/file_reader.h"
#include "encoding/encoding.h"
#include <stdlib.h>
#include <string.h>

int main(void) {
    const char* secret = "Attack at dawn";

    /* Base64-encode the payload and store the encoded text in a file. */
    char* encoded = encoding_base64_encode(secret, strlen(secret));
    FileWriter* writer = file_writer_open("payload.b64", WRITE_TEXT);

    file_writer_write_line(encoded, strlen(encoded), writer);
    file_writer_close(writer);
    fmt_printf("Stored base64: %s\n", encoded);
    free(encoded);

    /* Read the encoded line back and decode it to recover the original. */
    FileReader* reader = file_reader_open("payload.b64", READ_TEXT);
    char line[128] = {0};
    file_reader_read_line(line, sizeof(line), reader);
    file_reader_close(reader);

    char* decoded = encoding_base64_decode(line, strlen(line));
    fmt_printf("Decoded text: %s\n", decoded);
    free(decoded);

    remove("payload.b64");
    return 0;
}
```
**Result**
```
Stored base64: QXR0YWNrIGF0IGRhd24=
Decoded text: Attack at dawn
```

---

## Example 20 : write each `Vector` element as a line then read the lines back with `vector` and `FileWriter`/`FileReader`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include "file_io/file_reader.h"
#include "vector/vector.h"
#include <stdlib.h>

int main(void) {
    Vector* vec = vector_create(sizeof(int));
    int values[] = {10, 20, 30, 40};

    for (size_t i = 0; i < 4; ++i) {
        vector_push_back(vec, &values[i]);
    }

    /* Write each element as its own line in the file. */
    FileWriter* writer = file_writer_open("numbers.txt", WRITE_TEXT);
    for (size_t i = 0; i < vector_size(vec); ++i) {
        int* item = (int*)vector_at(vec, i);
        file_writer_write_fmt(writer, "%d\n", *item);
    }
    file_writer_close(writer);
    vector_deallocate(vec);

    /* Read the lines back and accumulate a running total. */
    FileReader* reader = file_reader_open("numbers.txt", READ_TEXT);
    char line[32];
    int sum = 0;

    while (file_reader_read_line(line, sizeof(line), reader)) {
        sum += atoi(line);
    }
    file_reader_close(reader);

    fmt_printf("Sum of values read from file: %d\n", sum);

    remove("numbers.txt");
    return 0;
}
```
**Result**
```
Sum of values read from file: 100
```

---

## Example 21 : transform a `String`, persist it, and load it back with `string` and `FileWriter`/`FileReader`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include "file_io/file_reader.h"
#include "string/std_string.h"
#include <stdlib.h>

int main(void) {
    /* Build a String, uppercase it, and persist the result. */
    String* msg = string_create("hello from the string module");
    char* upper = string_to_upper(msg);

    FileWriter* writer = file_writer_open("note.txt", WRITE_TEXT);
    file_writer_write_line(upper, string_length_cstr(upper), writer);
    file_writer_close(writer);
    free(upper);
    string_deallocate(msg);

    /* Read the line back into a fresh String and inspect it. */
    FileReader* reader = file_reader_open("note.txt", READ_TEXT);
    char line[128] = {0};
    file_reader_read_line(line, sizeof(line), reader);
    file_reader_close(reader);

    String* loaded = string_create(line);
    fmt_printf("Loaded line: %s\n", string_c_str(loaded));
    fmt_printf("Length: %zu, is_upper: %s\n",
               string_length(loaded),
               string_is_upper(loaded) ? "true" : "false");
    string_deallocate(loaded);

    remove("note.txt");
    return 0;
}
```
**Result**
```
Loaded line: HELLO FROM THE STRING MODULE
Length: 28, is_upper: true
```

---

## Example 22 : write a file, hash it on disk, and store the digest in a sidecar file with `crypto` and `FileWriter`/`FileReader`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include "file_io/file_reader.h"
#include "crypto/crypto.h"
#include <stdlib.h>
#include <string.h>

int main(void) {
    const char* content = "integrity check";

    FileWriter* writer = file_writer_open("data.bin", WRITE_TEXT);
    file_writer_write((void*)content, sizeof(char), strlen(content), writer);
    file_writer_close(writer);

    /* Hash the file on disk and store the hex digest in a sidecar file. */
    size_t hash_len = 0;
    uint8_t* hash = crypto_hash_file("data.bin", CRYPTO_SHA256, &hash_len);
    char* hex = crypto_hash_to_hex(hash, hash_len);

    FileWriter* sumWriter = file_writer_open("data.sha256", WRITE_TEXT);
    file_writer_write_line(hex, strlen(hex), sumWriter);
    file_writer_close(sumWriter);

    free(hash);
    free(hex);

    FileReader* reader = file_reader_open("data.sha256", READ_TEXT);
    char digest[128] = {0};
    file_reader_read_line(digest, sizeof(digest), reader);
    file_reader_close(reader);

    fmt_printf("SHA-256 of data.bin:\n%s\n", digest);

    remove("data.bin");
    remove("data.sha256");
    
    return 0;
}
```
**Result**
```
SHA-256 of data.bin:
5e6cc2c43757fa2635c2a23fdb3c3d643648e04b6ddf8bc2e03a5300e4220876
```

---

## License

This project is open-source and available under [ISC License].