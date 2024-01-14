#ifndef CRYPTO_H_
#define CRYPTO_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef DES_BLOCK_SIZE
#define DES_BLOCK_SIZE 8
#endif
typedef enum {
    CRYPTO_MD4,
    CRYPTO_MD5,
    CRYPTO_SHA1,
    CRYPTO_SHA224,
    CRYPTO_SHA256,
    CRYPTO_SHA384,
    CRYPTO_SHA512,
    CRYPTO_SHA3_224,
    CRYPTO_SHA3_256,
    CRYPTO_SHA3_384,
    CRYPTO_SHA3_512,
    CRYPTO_SHAKE_128,
    CRYPTO_SHAKE_256,
    CRYPTO_BLAKE2B_512,
    CRYPTO_BLAKE2S_256,
    CRYPTO_MDC2,
    CRYPTO_RIPEMD_160,
    CRYPTO_SHA512_224,
} HashAlgorithm ;

typedef enum {
    CRYPTO_MODE_ECB,
    CRYPTO_MODE_CBC,
    CRYPTO_MODE_CFB,
    CRYPTO_MODE_OFB,
} CryptoMode;

uint8_t* crypto_hash_data(const uint8_t* data, size_t length, HashAlgorithm algorithm, size_t *outLength);

void crypto_print_hash(const uint8_t* hash, size_t length);
void crypto_generate_random_iv(uint8_t *iv, size_t length);

void* crypto_des_encrypt(const uint8_t* plaintext, size_t len, const uint8_t* key, const uint8_t* iv, CryptoMode mode, size_t* out_len);
void* crypto_des_decrypt(const uint8_t* ciphertext, size_t len, const uint8_t* key, const uint8_t* iv, CryptoMode mode, size_t* out_len);



#endif 