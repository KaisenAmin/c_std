#include "fmt/fmt.h"
#include "string/string.h"

int main() {
    const char* str = "سلام دنیا";
    const char* cstr = "Hello World";
    
    fmt_printf("Size of ASCII string is %zu\n", string_length_cstr(cstr));
    fmt_printf("Size of unicode string is %zu\n", string_length_utf8(str));

    return 0;
}