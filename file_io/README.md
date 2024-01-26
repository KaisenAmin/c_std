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

- `file_writer_open(filename, mode)`: Opens a file for writing in the specified mode.
- `file_writer_append(filename, mode)`: Opens an existing file for appending, or creates it if it doesn't exist.
- `file_writer_close(writer)`: Closes the given FileWriter.
- `file_writer_get_position(writer)`: Returns the current position in the file.
- `file_writer_write(buffer, size, count, writer)`: Writes data to the file.
- `file_writer_write_line(buffer, size, writer)`: Writes a line of text to the file.
- `file_writer_is_open(writer)`: Checks if the FileWriter is open.
- `file_writer_flush(writer)`: Flushes the FileWriter's buffer.
- `file_writer_set_encoding(writer, encoding)`: Sets the character encoding for writing.
- `file_writer_copy(src_writer, dest_writer)`: Copies content from one FileWriter to another.
- `file_writer_get_file_name(writer)`: Retrieves the file path associated with the FileWriter.
- `file_writer_get_encoding(writer)`: Gets the encoding type of the FileWriter.
- `file_writer_write_fmt(writer, format, ...)`: Writes formatted data to the file.
- `file_writer_get_size(writer)`: Gets the size of the file.
- `file_writer_lock(writer)`: Locks the file to prevent other processes from modifying it.
- `file_writer_unlock(writer)`: Unlocks the file.
- `file_writer_seek(writer, offset, cursor_pos)`: Moves the file pointer to a specific location.
- `file_writer_truncate(writer, size)`: Truncates the file to a specified size.
- `file_writer_write_batch(writer, buffers, sizes, count)`: Writes multiple buffers in a single operation.
- `file_writer_append_fmt(writer, format, ...)`: Appends formatted text to a file.

### FileReader Functions

- `file_reader_open(filename, mode)`: Opens a file for reading in the specified mode.
- `file_reader_close(reader)`: Closes the FileReader.
- `file_reader_get_position(reader)`: Returns the current position in the file.
- `file_reader_is_open(reader)`: Checks if the FileReader is open.
- `file_reader_set_encoding(reader, encoding)`: Sets the character encoding for reading.
- `file_reader_get_file_name(reader)`: Retrieves the file path associated with the FileReader.
- `file_reader_seek(reader, offset, cursor_pos)`: Moves the file pointer to a specific location.
- `file_reader_eof(reader)`: Checks if the end of the file has been reached.
- `file_reader_get_size(reader)`: Gets the size of the file.
- `file_reader_read(buffer, size, count, reader)`: Reads data from the file.
- `file_reader_read_line(buffer, size, reader)`: Reads a line of text from the file.
- `file_reader_read_fmt(reader, format, ...)`: Reads formatted data from the file.
- `file_reader_copy(src_reader, dest_writer)`: Copies content from a FileReader to a FileWriter.
- `file_reader_read_lines(reader, buffer, num_lines)`: Reads a specified number of lines from the file.


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

## Example 3 : Using `file_writer_write_line`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include "string/string.h"

int main() {
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_UNICODE);
    String* buffer = string_create("سلام به همه دوستان و برنامه نویسان عزیز\n"); 

    // or you can use file_writer_is_open(writer)
    if (writer->is_open) { 
        size_t written = file_writer_write_line((char*)string_c_str(buffer), string_length(buffer), writer);

        if (written) {
            fmt_print("write in file was successfully.\n", FMT_END_ARGS);
        }
        else {
            fmt_print("Can not write to file.\n", FMT_END_ARGS);
        }
        file_writer_close(writer);
    }
    else {
        fmt_print("file is opened\n", FMT_END_ARGS);
    }
    return 0;
}
```

## Example 4 : Using most of the FileWriter functions 

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include "string/string.h"
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
    fmt_println("Current position in the file:", string_from_int_cstr((int)position), FMT_END_ARGS);

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

    // Append another line to the file
    char* additionalString = "اضافه کردن متن به فایل\n"; // "Adding text to the file" in Persian
    if (!file_writer_write_line(additionalString, strlen(additionalString), writer)) {
        fmt_printf("Failed to write line to file.\n");
        file_writer_close(writer);
        return 1;
    }

    // Close the file
    if (!file_writer_close(writer)) {
        fmt_printf("Failed to close the file writer.\n");
    }

    return 0;
}
```

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

## Example 11 : how to truncate the file in `FileWriter` with `file_writer_truncate`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include <string.h>

int main() {
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_TEXT);
    char* message = "Hello C/C++ Programmers in all over the world.";

    if (!file_writer_write_line(message, strlen(message), writer)) {
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

## Example 12 : how to write multiple buffer in `FileWriter` with `file_writer_write_batch`

```c
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include "string/string.h"

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

## Example 16 : how to read line as formated text in `FileReader` with `file_reader_read_fmt`

```c
#include "file_io/file_reader.h"
#include "fmt/fmt.h"

int main() {
    FileReader* reader = file_reader_open("./sources/text_uni.txt", READ_UNICODE);
    int intValue;
    double doubleValue;
    char strValue[100];

    // Assuming the file contains a line formatted like "int: %d, double: %lf, string: %s"
    size_t itemsRead = file_reader_read_fmt(reader, "int: %d, double: %lf, string: %99s", &intValue, &doubleValue, strValue);
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

## Example : how to read number of lines from file in `FileReader` with `file_reader_read_lines`

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