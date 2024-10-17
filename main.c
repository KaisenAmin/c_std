#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    char* currentPath = dir_current_path();

    if (currentPath) {
        fmt_printf("Current directory path: '%s'\n", currentPath);
        free(currentPath);
    } 
    else {
        fmt_printf("Failed to get current directory path.\n");
    }
    
    return 0;
}