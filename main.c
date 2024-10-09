#include "fmt/fmt.h"
#include "string/std_string.h"


int main() {
    String* str1 = string_create("     ");
    String* str2 = string_create("");

    fmt_printf("%s is space %d\n", string_c_str(str1), string_is_space(str1));
    fmt_printf("%s is space %d\n", string_c_str(str2), string_is_space(str2));

    string_deallocate(str2);
    string_deallocate(str1);

    return 0;
}