#include <stdlib.h>
#include "fmt/fmt.h"
#include "encoding/encoding.h"


int main() {
    const char* input = "Hello, Base91!";
    size_t input_length = strlen(input);

    // Encoding
    char* encoded = encoding_base91_encode((const uint8_t*)input, input_length);
    if (!encoded) {
        fmt_fprintf(stderr, "Encoding failed.\n");
        return 1;
    }

    fmt_printf("Encoded: %s\n", encoded);

    // Decoding
    size_t decoded_length;
    uint8_t* decoded = encoding_base91_decode(encoded, &decoded_length);
    if (!decoded) {
        fmt_fprintf(stderr, "Decoding failed.\n");
        free(encoded);
        return 1;
	}
    fmt_printf("Decoded: %.*s\n", (int)decoded_length, decoded);

    free(encoded);
    free(decoded);
    return 0;
}