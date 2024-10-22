#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "hello.*goodbye";
    const char* test_string = "hello world!\ngoodbye world!";

    Regex* regex = regex_compile(pattern, REGEX_DOTALL);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Dotall match found: %.*s\n", match.length, match.start);
    } 
    else {
        fmt_printf("No match found.\n");
    }

    regex_deallocate(regex);
    return 0;
}