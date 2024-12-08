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

### FileWriter Functions

### `FileWriter* file_writer_open(const char* filename, const WriteMode mode)`

- **Purpose**:  
  The `file_writer_open` function is used to open a file for writing based on the specified `WriteMode`. This function supports writing in various modes, such as text, binary, Unicode, buffered, unbuffered, and appending, allowing flexibility in how the file is written.

- **Parameters**:
  - `filename`: The name of the file to open for writing.
  - `mode`: The mode in which the file should be opened, specified by the `WriteMode` enum. The modes include:
    - `WRITE_TEXT`: Open in text mode.
    - `WRITE_BINARY`: Open in binary mode.
    - `WRITE_UNICODE`: Open in Unicode mode (UTF-8).
    - `WRITE_BUFFERED`: Open in buffered mode for optimized writing.
    - `WRITE_UNBUFFERED`: Open in unbuffered mode for direct writing.
    - `WRITE_APPEND`: Open in append mode to write data at the end of the file.

- **Return Type**:  
  Returns a pointer to the `FileWriter` structure on success, or `NULL` on failure (e.g., invalid `filename` or failure to open the file).

---

### `FileWriter* file_writer_append(const char* filename, const WriteMode mode)`

- **Purpose**:  
  The `file_writer_append` function is used to open a file for appending data. This function allows appending new data to the end of an existing file, or creates a new file if it does not already exist. The function supports writing in different modes, such as text, binary, and Unicode.

- **Parameters**:
  - `filename`: The name of the file to open for appending.
  - `mode`: The mode in which the file should be opened, specified by the `WriteMode` enum. The available modes include:
    - `WRITE_TEXT`: Open in text mode for appending.
    - `WRITE_BINARY`: Open in binary mode for appending.
    - `WRITE_UNICODE`: Open in Unicode mode (UTF-8) for appending.
    - `WRITE_BUFFERED`: Open in buffered mode for optimized writing.
    - `WRITE_UNBUFFERED`: Open in unbuffered mode for direct writing.
    - `WRITE_APPEND`: Open specifically for appending data.

- **Return Type**:  
  Returns a pointer to the `FileWriter` structure on success, or `NULL` on failure (e.g., invalid `filename` or failure to open the file).

---

### `bool file_writer_close(FileWriter* writer)`

- **Purpose**:  
  The `file_writer_close` function closes the file associated with the given `FileWriter` structure. It ensures that all buffered data is properly flushed to the file before closing, preventing data loss.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure.

- **Return Type**:  
  Returns `true` if the file was successfully closed, or `false` if an error occurred (e.g., if the file was already closed or `fclose` failed).

---

### `size_t file_writer_get_position(FileWriter* writer)`

- **Purpose**:  
  The `file_writer_get_position` function retrieves the current position of the file pointer within the file associated with the `FileWriter`. This is useful for tracking the position in the file during writing operations.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure.

- **Return Type**:  
  Returns the current file pointer position as a `size_t`. If an error occurs, the function returns `(size_t)-1`.

---

### `size_t file_writer_write(void* buffer, size_t size, size_t count, FileWriter* writer)`

- **Purpose**:  
  The `file_writer_write` function writes data from a buffer to the file associated with the `FileWriter`. It handles different writing modes, including binary and Unicode (UTF-16 and UTF-32), and can perform necessary encoding conversions before writing.

- **Parameters**:
  - `buffer`: The data buffer to write from.
  - `size`: The size of each element in the buffer.
  - `count`: The number of elements to write.
  - `writer`: A pointer to the `FileWriter` structure.

- **Return Type**:  
  Returns the number of elements successfully written. If an error occurs (e.g., invalid `FileWriter` structure or buffer), the function returns `0`.

---

### `bool file_writer_write_line(char* buffer, size_t size, FileWriter* writer)`

- **Purpose**:  
  The `file_writer_write_line` function writes a line of text to the file associated with the `FileWriter`, automatically adding a newline character at the end. It handles Unicode text, ensuring proper encoding (e.g., UTF-16 on Windows).

- **Parameters**:
  - `buffer`: The buffer containing the line of text to write.
  - `size`: The size of the text line in bytes.
  - `writer`: A pointer to the `FileWriter` structure.

- **Return Type**:  
  Returns `true` if the line was successfully written, or `false` if there was an error.

---

### `bool file_writer_is_open(FileWriter* writer)`

- **Purpose**:  
  The `file_writer_is_open` function checks whether the file associated with the `FileWriter` structure is currently open. This is useful to verify the state of the file before performing any write operations.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure.

- **Return Type**:  
  Returns `true` if the file is open, or `false` if it is not open or if an error occurred.

---

### `bool file_writer_flush(FileWriter* writer)`

- **Purpose**:  
  The `file_writer_flush` function ensures that all buffered data is physically written to the file. It is useful in scenarios where immediate persistence of data to the file is necessary.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure.

- **Return Type**:  
  Returns `true` if the flush operation was successful, or `false` if an error occurred (e.g., invalid `FileWriter` or failed `fflush`).

---

### `bool file_writer_set_encoding(FileWriter* writer, const WriteEncodingType encoding)`

- **Purpose**:  
  The `file_writer_set_encoding` function configures the `FileWriter` to use the specified character encoding for writing data. It supports different encodings, such as UTF-16 and UTF-32.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure.
  - `encoding`: The desired `WriteEncodingType` (e.g., `WRITE_ENCODING_UTF16` or `WRITE_ENCODING_UTF32`).

- **Return Type**:  
  Returns `true` if the encoding was successfully set, or `false` if there was an error (e.g., invalid `FileWriter` or invalid encoding type).

---

### `bool file_writer_copy(FileWriter* src_writer, FileWriter* dest_writer)`

- **Purpose**:  
  The `file_writer_copy` function copies the entire content of the file managed by the `src_writer` to the file managed by the `dest_writer`. This function is useful for duplicating files or transferring content between different file streams.

- **Parameters**:
  - `src_writer`: A pointer to the source `FileWriter` structure.
  - `dest_writer`: A pointer to the destination `FileWriter` structure.

- **Return Type**:  
  Returns `true` if the file content was successfully copied, or `false` if an error occurred (e.g., file reading/writing failure or invalid `FileWriter` structures).

---

### `const char* file_writer_get_file_name(FileWriter* writer)`

- **Purpose**:  
  The `file_writer_get_file_name` function retrieves the absolute path of the file associated with the `FileWriter` structure. This allows the user to check which file the `FileWriter` is currently operating on.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure.

- **Return Type**:  
  Returns a constant character pointer to the file path if the `FileWriter` is valid, or `NULL` if there is an error.

---

### `const char* file_writer_get_encoding(FileWriter* writer)`

- **Purpose**:  
  The `file_writer_get_encoding` function retrieves the encoding type currently set for the `FileWriter` (e.g., UTF-16 or UTF-32). This allows the user to confirm how data is being written to the file in terms of encoding.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure.

- **Return Type**:  
  Returns a constant character pointer representing the encoding type if valid, or `NULL` if there is an error.

---

### `size_t file_writer_write_fmt(FileWriter* writer, const char* format, ...)`

- **Purpose**:  
  The `file_writer_write_fmt` function writes formatted data to the file associated with the `FileWriter`, similar to the behavior of `fprintf`. It formats the data according to the provided format string and writes it to the file.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure.
  - `format`: A format string (similar to `printf`), followed by the values to format.

- **Return Type**:  
  Returns the number of characters successfully written, or `0` if an error occurs (e.g., invalid `FileWriter` or formatting failure).

---

### `size_t file_writer_get_size(FileWriter* writer)`

- **Purpose**:  
  The `file_writer_get_size` function retrieves the current size of the file associated with the `FileWriter`. It ensures that any pending writes are flushed to the file, then seeks to the end of the file to calculate the size.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure.

- **Return Type**:  
  Returns the size of the file in bytes, or `0` if an error occurs (e.g., failed `fseek` or invalid `FileWriter`).

---

### `bool file_writer_lock(FileWriter* writer)`

- **Purpose**:  
  The `file_writer_lock` function locks the file associated with the `FileWriter` structure to prevent other processes from modifying it while the lock is held. This ensures exclusive access to the file for writing.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure representing the file.

- **Return Type**:  
  Returns `true` if the file is successfully locked, or `false` if an error occurred.

---

### `bool file_writer_unlock(FileWriter* writer)`

- **Purpose**:  
  The `file_writer_unlock` function releases the lock on the file associated with the `FileWriter`, allowing other processes to access it again. This function should be called after the file operations are completed.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure representing the file.

- **Return Type**:  
  Returns `true` if the file is successfully unlocked, or `false` if an error occurred.

---

### `bool file_writer_seek(FileWriter* writer, long offset, const CursorPosition cursor_pos)`

- **Purpose**:  
  The `file_writer_seek` function moves the file pointer to a specific location, enabling random access writing. The position can be set relative to the beginning, end, or current position of the file.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure representing the file.
  - `offset`: The offset in bytes to move the file pointer.
  - `cursor_pos`: The reference position for the offset (`POS_BEGIN`, `POS_END`, or `POS_CURRENT`).

- **Return Type**:  
  Returns `true` if the file pointer was successfully moved, or `false` if there was an error.

---

### `bool file_writer_truncate(FileWriter* writer, size_t size)`

- **Purpose**:  
  The `file_writer_truncate` function truncates the file associated with the `FileWriter` to the specified size. If the new size is smaller than the current file size, the file is shortened. If it is larger, the file is extended, and the added space is filled with zeros.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure representing the file.
  - `size`: The size to which the file should be truncated (in bytes).

- **Return Type**:  
  Returns `true` if the file was successfully truncated, or `false` if an error occurred.

---

### `bool file_writer_write_batch(FileWriter* writer, const void** buffers, const size_t* sizes, size_t count)`

- **Purpose**:  
  The `file_writer_write_batch` function writes multiple buffers to a file in a single operation, potentially optimizing I/O operations by reducing the number of system calls. It handles different encoding types (e.g., UTF-16, UTF-32) and converts the data as needed before writing.

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure representing the file.
  - `buffers`: An array of pointers to the data buffers to be written.
  - `sizes`: An array of sizes corresponding to each buffer.
  - `count`: The number of buffers to be written.

- **Return Type**:  
  Returns `true` if all buffers were successfully written, or `false` if an error occurred.

---

### `bool file_writer_append_fmt(FileWriter* writer, const char* format, ...)`

- **Purpose**:  
  The `file_writer_append_fmt` function formats a string according to the specified format string and appends the resulting text to the file associated with the `FileWriter`. The `FileWriter` must be in append mode (`WRITE_APPEND`).

- **Parameters**:
  - `writer`: A pointer to the `FileWriter` structure representing the file.
  - `format`: A format string (similar to `printf`), followed by additional arguments corresponding to the format.

- **Return Type**:  
  Returns `true` if the formatted text was successfully appended, or `false` if an error occurred.

---

### FileReader Functions

### `FileReader* file_reader_open(const char* filename, const ReadMode mode)` 

- **Purpose**:  
  The `FileReader` function is designed to open and manage reading operations on files using various modes, such as text, binary, Unicode, buffered, unbuffered, and line-by-line reading. This flexibility allows for optimized file reading based on the specific needs of the user. The function is part of a broader framework aimed at providing robust file management capabilities in C.

- **Parameters**:
  - `filename`: The path of the file to open.
  - `mode`: Specifies how the file should be opened, as defined by the `ReadMode` enum. Modes include:
    - `READ_TEXT`: Opens the file in text mode.
    - `READ_BINARY`: Opens the file in binary mode.
    - `READ_UNICODE`: Opens the file for Unicode text reading (on Windows, it may involve encoding conversion to UTF-16).
    - `READ_BUFFERED`: Opens the file with buffered reading for optimized performance.
    - `READ_UNBUFFERED`: Opens the file without buffering, performing direct read operations.
    - `READ_LINE`: Opens the file for line-by-line reading, useful for text files.

- **Return Type**:  
  Returns a pointer to a `FileReader` structure on success, or `NULL` on failure.

---

### `bool file_reader_close(FileReader* reader)`

- **Purpose**:  
  The `file_reader_close` function is responsible for properly closing the file associated with a `FileReader` structure. It ensures that any resources allocated for the file (such as memory and file handles) are released to prevent resource leaks.

- **Parameters**:
  - `reader`: A pointer to the `FileReader` structure that holds information about the open file and its state.

- **Return Type**:  
  Returns `true` if the file was successfully closed, or `false` if an error occurred (e.g., invalid `FileReader` structure or failure to close the file).

---

### `size_t file_reader_get_position(FileReader* reader)`

- **Purpose**:  
  The `file_reader_get_position` function retrieves the current position of the file pointer within the file associated with the `FileReader` structure. This allows the user to track or resume reading from the current position.

- **Parameters**:
  - `reader`: A pointer to the `FileReader` structure that holds information about the open file.

- **Return Type**:  
  Returns the current file pointer position as a `size_t`. If an error occurs (such as an invalid `FileReader` structure), the function returns `(size_t)-1`.

---

### `bool file_reader_is_open(FileReader* reader)`

- **Purpose**:  
  The `file_reader_is_open` function checks whether the file associated with the given `FileReader` structure is currently open. This helps the user ensure that the file is still available for reading operations.

- **Parameters**:
  - `reader`: A pointer to the `FileReader` structure that holds information about the open file.

- **Return Type**:  
  Returns `true` if the file is open, or `false` if it is not, or if an error occurs (e.g., invalid `FileReader` structure).

---

### `bool file_reader_set_encoding(FileReader* reader, const ReadEncodingType encoding)`

- **Purpose**:  
  The `file_reader_set_encoding` function sets the encoding type for reading the file associated with the `FileReader` structure. This allows the user to specify how the file should be interpreted when reading text in different encodings.

- **Parameters**:
  - `reader`: A pointer to the `FileReader` structure that holds information about the file.
  - `encoding`: Specifies the encoding type to set, defined by the `ReadEncodingType` enum (e.g., `READ_ENCODING_UTF16` or `READ_ENCODING_UTF32`).

- **Return Type**:  
  Returns `true` if the encoding was successfully set, or `false` if an error occurred (e.g., invalid `FileReader` structure or invalid encoding type).

---

### `const char* file_reader_get_file_name(FileReader* reader)`

- **Purpose**:  
  The `file_reader_get_file_name` function retrieves the absolute file path associated with the `FileReader` structure. This allows the user to access the full file path of the currently opened file for reference or logging purposes.

- **Parameters**:
  - `reader`: A pointer to the `FileReader` structure that holds information about the file.

- **Return Type**:  
  Returns the absolute file path as a constant string if successful, or `NULL` if there is an error (e.g., invalid `FileReader` structure or `file_path` is `NULL`).

---

### `bool file_reader_seek(FileReader* reader, long offset, const CursorPosition cursor_pos)`

- **Purpose**:  
  The `file_reader_seek` function moves the file pointer to a specific location within the file. This allows for random access reading, where the user can skip to a particular position in the file to begin reading from there.

- **Parameters**:
  - `reader`: A pointer to the `FileReader` structure.
  - `offset`: The number of bytes to move the file pointer relative to the `cursor_pos`.
  - `cursor_pos`: The reference point from which to calculate the offset. It is defined by the `CursorPosition` enum with values like:
    - `POS_BEGIN`: Move relative to the beginning of the file.
    - `POS_END`: Move relative to the end of the file.
    - `POS_CURRENT`: Move relative to the current position of the file pointer.

- **Return Type**:  
  Returns `true` if the file pointer was successfully moved, or `false` if there was an error (e.g., invalid `FileReader` structure or failed `fseek`).

---

### `bool file_reader_eof(FileReader* reader)`

- **Purpose**:  
  The `file_reader_eof` function checks if the file pointer has reached the end of the file. This is useful when reading through a file to determine when there is no more data to read.

- **Parameters**:
  - `reader`: A pointer to the `FileReader` structure.

- **Return Type**:  
  Returns `true` if the file pointer has reached the end of the file, or `false` if not or if an error occurs (e.g., invalid `FileReader`).

---

### `size_t file_reader_get_size(FileReader* reader)`

- **Purpose**:  
  The `file_reader_get_size` function returns the total size of the file in bytes. This is useful for determining the size of the file for reading or processing operations.

- **Parameters**:
  - `reader`: A pointer to the `FileReader` structure.

- **Return Type**:  
  Returns the size of the file in bytes, or `0` if an error occurred (e.g., invalid `FileReader` or failed `fseek`).

---

### `size_t file_reader_read(void* buffer, size_t size, size_t count, FileReader* reader)`

- **Purpose**:  
  The `file_reader_read` function reads data from the file into a buffer. It supports reading in both binary and text modes, handling different types of data reading efficiently.

- **Parameters**:
  - `buffer`: A pointer to the buffer where the data will be stored.
  - `size`: The size of each element to read (in bytes).
  - `count`: The number of elements to read.
  - `reader`: A pointer to the `FileReader` structure.

- **Return Type**:  
  Returns the number of elements successfully read, or `0` if an error occurs (e.g., invalid `FileReader` structure or buffer).

---

### `bool file_reader_read_line(char* buffer, size_t size, FileReader* reader)`

- **Purpose**:  
  The `file_reader_read_line` function reads a single line of text from the file associated with the `FileReader`. It supports different encoding modes, including UTF-16 and UTF-8, handling encoding conversions where necessary.

- **Parameters**:
  - `buffer`: A pointer to the buffer where the line will be stored.
  - `size`: The maximum number of characters to read, including the null terminator.
  - `reader`: A pointer to the `FileReader` structure.

- **Return Type**:  
  Returns `true` if a line was successfully read, or `false` if an error occurs (e.g., invalid `FileReader` structure or reading failure).

---

### `size_t file_reader_read_fmt(FileReader* reader, const char* format, ...)`

- **Purpose**:  
  The `file_reader_read_fmt` function reads formatted data from the file using a format string, similar to how `scanf` works. This is useful for reading structured data in a predefined format.

- **Parameters**:
  - `reader`: A pointer to the `FileReader` structure.
  - `format`: A format string specifying how to interpret the data.
  - (varargs): A variable number of arguments representing the variables that will receive the formatted data.

- **Return Type**:  
  Returns the number of items successfully read, or `0` if an error occurs (e.g., invalid `FileReader` structure or formatting failure).

---

### `bool file_reader_copy(FileReader* src_reader, FileWriter* dest_writer)`

- **Purpose**:  
  The `file_reader_copy` function copies the contents of a source file (associated with the `FileReader`) to a destination file (associated with the `FileWriter`). It handles encoding conversion if necessary, ensuring that UTF-16 content is converted to UTF-8 before writing.

- **Parameters**:
  - `src_reader`: A pointer to the `FileReader` structure for the source file.
  - `dest_writer`: A pointer to the `FileWriter` structure for the destination file.

- **Return Type**:  
  Returns `true` if the copy operation was successful, or `false` if an error occurred (e.g., failed reading, writing, or conversion).

---

### `bool file_reader_read_lines(FileReader* reader, char*** buffer, size_t num_lines)`

- **Purpose**:  
  The `file_reader_read_lines` function reads multiple lines of text from a file, storing each line in a dynamically allocated array of strings. This function allows the user to extract a specified number of lines for further processing.

- **Parameters**:
  - `reader`: A pointer to the `FileReader` structure associated with the file.
  - `buffer`: A pointer to an array of strings where the lines will be stored. The memory for this buffer is allocated dynamically within the function.
  - `num_lines`: The number of lines to read from the file.

- **Return Type**:  
  Returns `true` if the specified number of lines was successfully read, or `false` if an error occurred (e.g., memory allocation failure, invalid `FileReader`, or file reading issues). The buffer will contain the lines that were successfully read.
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
    printf("Written %zu item(s) to Unicode file.\n", unicodeWritten);

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
    printf("Written %zu item(s) to Unicode file.\n", unicodeWritten);
    printf("Cursor position is %zu\n", file_writer_get_position(writer));
    

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
        size_t written = file_writer_write_line((char*)string_c_str(buffer), string_length(buffer), writer);

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

int main() {
    const char* unicodeString = "سلام به همه دوستان\n"; // "Hello to all friends" in Persian

    // Open a new file for writing in Unicode mode
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
    fmt_println("Current position in the file:", string_from_int_cstr((int)position));

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
Failed to open file for appending.
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
Its Correct 46
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
Size of file is 88
```

---

## Example 7 : get file_path in `FileWriter` with `file_writer_get_file_name` also file encoding with `file_writer_get_encoding`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"

int main() {
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_APPEND);

    fmt_printf("filePath is : %s\n", file_writer_get_file_name(writer)); // or writer->file_path
    fmt_printf("Encoding Type is : %s\n", file_writer_get_encoding(writer));

    file_writer_close(writer);
    return 0;
}
```
**Result**
```
filePath is : ./text_uni.txt
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
Read 382 bytes from Unicode file.
Content:
Hello, this is a test text.سلام، این یک متن آزمایشی است.Name: John Doe, Age: 30, Occupation: Software Developer
This is another line with number: 123.456000
Name: John Doe, Age: 30, Occupation: Software Developer
This is another line with number: 123.456000
Name: John Doe, Age: 30, Occupation: Software Developer
This is another line with number: 123.456000
```

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
Line: Name: John Doe, Age: 30, Occupation: Software Developer
Line: This is another line with number: 123.456000
Line: Name: John Doe, Age: 30, Occupation: Software Developer
Line: This is another line with number: 123.456000
```

---

## Example 16 : how to read line as formated text in `FileReader` with `file_reader_read_fmt`

```c
#include "file_io/file_reader.h"
#include "fmt/fmt.h"

int main() {
    FileReader* reader = file_reader_open("./sources/text_uni.txt", READ_UNICODE);
    int intValue;
    double doubleValue;
    char strValue[100];

    // Assuming the file contains a line formatted like "int: %d, double: %lf, string: %s" int: 10, double: 20.2, string: amin tahmasebi
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
            free(lines[i]); // Free each line
        }
        free(lines); // Free the array of lines
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

Line 3: Name: John Doe, Age: 30, Occupation: Software Developer

Line 4: This is another line with number: 123.456000

Line 5: Name: John Doe, Age: 30, Occupation: Software Developer
```

---

## License

This project is open-source and available under [ISC License].