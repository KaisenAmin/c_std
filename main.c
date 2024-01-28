#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* filePath = "C:\\Users\\Science\\Desktop\\new_one.txt";
    char ownerBuffer[256];  // Buffer to store the owner's name

    if (dir_get_file_owner(filePath, ownerBuffer, sizeof(ownerBuffer))) {
        fmt_printf("The owner of '%s' is: %s\n", filePath, ownerBuffer);
    } 
    else {
        fmt_printf("Failed to retrieve the file owner for '%s'\n", filePath);
    }
    return 0;
}