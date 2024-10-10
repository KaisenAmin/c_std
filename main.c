#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *originalStr = string_create("Hello");
    String *hexStr = string_to_hex(originalStr);

    fmt_printf("Original String: %s\n", originalStr->dataStr);
    fmt_printf("Hexadecimal Representation: %s\n", hexStr->dataStr);

    string_deallocate(originalStr);
    string_deallocate(hexStr);

    return 0;
}