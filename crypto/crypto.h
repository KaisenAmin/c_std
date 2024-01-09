#ifndef CRYPTO_H_
#define CRYPTO_H_

#include <stddef.h>
#include <stdint.h>

enum HashAlgorithm {
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
};

uint8_t* crypto_hash_data(const uint8_t* data, size_t length, enum HashAlgorithm algorithm, size_t *outLength);
void crypto_print_hash(const uint8_t* hash, size_t length);

#endif 