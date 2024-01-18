
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

    if (!writer) {
        fmt_printf("Error: failed to open file for appending text.\n");
        return -1;
    }

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

    if (!writer) {
        fmt_printf("Error: failed to open file for appending text.\n");
        return -1;
    }
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

    if (!writer) {
        fmt_printf("Error: failed to open file for appending text.\n");
        return -1;
    }
    
    fmt_printf("filePath is : %s\n", file_writer_get_file_name(writer)); // or writer->file_path
    fmt_printf("Encoding Type is : %s\n", file_writer_get_encoding(writer));

    file_writer_close(writer);
    return 0;
}
```