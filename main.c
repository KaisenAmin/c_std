#include "fmt/fmt.h"
#include "string/std_string.h"


int main() {
    String* str1 = string_create("Hello!Areyou#1?");
    String* str2 = string_create("Hello!\nAre you #1?");

    fmt_printf("'%s' is printable %d\n", string_c_str(str1), string_is_printable(str1));
    fmt_printf("'%s' is printable %d\n", string_c_str(str2), string_is_printable(str2));

    string_deallocate(str1);
    string_deallocate(str2);

    return 0;
}