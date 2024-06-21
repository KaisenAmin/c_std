#include "fmt/fmt.h"

int main() {
    FileWriter* writer = file_writer_open("./output.txt", WRITE_TEXT);

    fmt_fprint(writer->file_writer, "This is text in a file");
    fmt_fprint(stdout, "Hello, World! ", "こんにちは ", "سلام دنیا");
    fmt_fprint(stderr, "This is an error message on stderr");

    file_writer_close(writer);
    return 0;
}