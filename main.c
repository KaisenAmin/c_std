#include "string/string.h"
#include <stdio.h>
#include <stdlib.h>


int main() {
    String* str = string_create("Hello,World,This,Is,A,Test");
    int count = 0;
    String** splits = string_split(str, ",", &count);

    for (int i = 0; i < count; i++) {
        printf("Split %d: %s\n", i, string_c_str(splits[i]));
        string_deallocate(splits[i]);
    }
    free(splits);

    string_deallocate(str);
    return 0;
}