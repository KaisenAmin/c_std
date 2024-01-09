#include "crypto/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* exampleData = "Hello, World!";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_BLAKE2S_256, &hashLength);

    printf("CRYPTO_RIPEMD160(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}