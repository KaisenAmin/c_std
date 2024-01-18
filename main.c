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