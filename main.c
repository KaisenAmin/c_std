#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello, World!");

    // rfind example
    int rfindPos = string_rfind(myString, "o,", string_length(myString) - 1);
    fmt_printf("Last 'o' found at position: %d\n", rfindPos);

    int findFirstOfPos = string_find_first_of(myString, "World", 0);
    fmt_printf("First occurrence of 'World' found at position: %d\n", findFirstOfPos);

    // find_last_of example
    int findLastOfPos = string_find_last_of(myString, "World", string_length(myString) - 1);
    fmt_printf("Last occurrence of 'World' found at position: %d\n", findLastOfPos);

    // Deallocate and clean up
    string_clear(myString);
    string_deallocate(myString);

    return 0;
}