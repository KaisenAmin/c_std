#include "encoding/encoding.h"
#include "string/string.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    String *text_to_encode = string_create("salam salam/ halet chetore");
    char* encoded = encoding_url_encode(string_c_str(text_to_encode), string_length(text_to_encode));

    if (encoded) {
        printf("Original: %s\n", string_c_str(text_to_encode));
        printf("URL Encoded: %s\n", encoded);
        free(encoded);
    } else {
        printf("Failed to encode URL.\n");
    }

    string_deallocate(text_to_encode);
    return 0;
}