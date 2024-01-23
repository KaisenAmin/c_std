#include "fmt/fmt.h"

int main() {
    FileWriter* writer = file_writer_open("./output.txt", WRITE_TEXT);

    fmt_fprintf(writer->file_writer, "Writing to a file: %s\n", "Hello, برنامه نویسان");
    fmt_fprint(writer->file_writer, "More text: ", "This is an example", " using fmt_fprint", FMT_END_ARGS);

    file_writer_close(writer);
    return 0;
}