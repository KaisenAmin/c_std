#include "crypto/crypto.h"
#include "fmt/fmt.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int main() {
    const uint8_t key[8] = "yourkey"; // 8 bytes key
    const uint8_t plaintext[] = "Hello World"; 
    size_t outLen;

    // Encrypt with ECB
    uint8_t* encrypted_ecb = (uint8_t*)crypto_des_encrypt(plaintext, strlen((const char *)plaintext), key, NULL, CRYPTO_MODE_ECB, &outLen);
    fmt_printf("ECB Encrypted: ");
    crypto_print_hash(encrypted_ecb, outLen);

    // Decrypt with ECB
    uint8_t* decrypted_ecb = (uint8_t*)crypto_des_decrypt(encrypted_ecb, outLen, key, NULL, CRYPTO_MODE_ECB, &outLen);
    fmt_printf("ECB Decrypted: %s\n", decrypted_ecb);

    free(encrypted_ecb);
    free(decrypted_ecb);
    return 0;
}