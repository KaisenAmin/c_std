

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