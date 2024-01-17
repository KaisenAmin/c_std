#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    FILE* file = fopen("./sources/data.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    int value1;
    float value2;
    char str[100];

    int scanned = fmt_fscan(file, "%d %f %s", &value1, &value2, str);
    if (scanned < 3) {
        printf("Error or end of file reached with %d items scanned.\n", scanned);
    } 
    else {
        fmt_printf("Read values: %d, %f, %s\n", value1, value2, str);
    }

    fclose(file);
    return 0;
}
