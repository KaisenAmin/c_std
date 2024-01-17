
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
    String* buffer = string_create("Hello World\n"); // in future this problem will be solve that String object work with unicode also.

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