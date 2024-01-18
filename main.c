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

