#include "fmt/fmt.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
    char* input = NULL;
    printf("Enter a string: ");

    if (fmt_scan(&input) == 0) {
        fmt_printf("You entered: %s\n", input);
        free(input);
    } 
    else {
        fmt_printf("Error reading input.\n");
    }
    return 0;
}