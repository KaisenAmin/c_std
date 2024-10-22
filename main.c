#include "algorithm/algorithm.h"
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *source = string_create("Hello C Programmers");
    String *dest = string_create("");

    fmt_printf("Source is -> %s\n", string_c_str(source));
    
    string_reserve(dest, string_length(source));
    algorithm_copy(string_c_str(source), string_length(source) + 1, sizeof(char), dest->dataStr);

    const char* value = string_c_str(dest);
    fmt_printf("Destination is -> %s\n", value);

    string_deallocate(source);
    string_deallocate(dest);

    return 0;
}