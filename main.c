#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str1 = string_create("Hello");
    String* str2 = string_create("World");

    if (string_is_equal(str1, str2)) {
        fmt_printf("Strings are equal\n");
    }   
    if (string_is_less(str1, str2)) { 
        fmt_printf("String 1 is less than String 2\n");
    }
    if (string_is_greater(str1, str2)) {
        fmt_printf("String1 is greater that String2\n");
    }
    if (string_is_less_or_equal(str1, str2)) {
        fmt_printf("String1 is less than or equal String2\n");
    }
    if (string_is_greater_or_equal(str1, str2)) {
        fmt_printf("String1 is greater than or equal String2\n");
    }

    string_deallocate(str1);
    string_deallocate(str2);

    return 0;
}