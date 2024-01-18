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

