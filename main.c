#include "fmt/fmt.h"
#include "string/string.h"

// persian
void println() {
    for (int index = 0; index < 10; index++) {
        fmt_println(string_from_int_cstr(index), ":", "سلام دنیا");
    }
}

// japanese
void print() {
    for (int index = 0; index < 10; index++) {
        fmt_print(string_from_int_cstr(index), ":", "ああ、 --");
    }
}

int main() {
    println();
    fmt_println("-----------------");
    print();

    return 0;
}