#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include "string/string.h"

int main() {
    FileWriter* writer = file_writer_open("./sources/text_uni.txt", WRITE_UNICODE);
    String* buffer = string_create("سلام به همه دوستان و برنامه نویسان عزیز\n"); 
    size_t size = string_length(buffer);

    // or you can use file_writer_is_open(writer)
    if (writer->is_open) { 
        size_t written = file_writer_write_line((char*)string_c_str(buffer), size, writer);

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
