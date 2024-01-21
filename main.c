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