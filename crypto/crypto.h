/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Crypto
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in crypto.c (file-level convention).
 *
 * Hash, HMAC, PBKDF2 and a small DES symmetric-cipher pair built on
 * OpenSSL EVP.
 */

#ifndef CRYPTO_H_
#define CRYPTO_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define CRYPTO_LOGGING_ENABLE */

#ifdef CRYPTO_LOGGING_ENABLE
    #define CRYPTO_LOG(fmt, ...) \
        do { fprintf(stderr, "[CRYPTO LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define CRYPTO_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Limits                                                             */
/* ------------------------------------------------------------------ */

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
    CRYPTO_SHA512_224
} HashAlgorithm;


typedef enum {
    CRYPTO_MODE_ECB,
    CRYPTO_MODE_CBC,
    CRYPTO_MODE_CFB,
    CRYPTO_MODE_OFB
} CryptoMode;


/* ------------------------------------------------------------------ */
/* Hashing                                                            */
/* ------------------------------------------------------------------ */

uint8_t*     crypto_hash_data                  (const uint8_t* data, size_t length, HashAlgorithm algorithm, size_t* outLength);
uint8_t*     crypto_hash_string                (const char* str, HashAlgorithm algorithm, size_t* outLength);
uint8_t*     crypto_hash_file                  (const char* path, HashAlgorithm algorithm, size_t* outLength);


/* ------------------------------------------------------------------ */
/* Hash digest encoding / decoding                                    */
/* ------------------------------------------------------------------ */

char*        crypto_hash_to_hex                (const uint8_t* hash, size_t length);
char*        crypto_hash_to_base64             (const uint8_t* hash, size_t length);
uint8_t*     crypto_hash_from_hex              (const char* hex, size_t* outLength);
uint8_t*     crypto_hash_from_base64           (const char* b64, size_t* outLength);


/* ------------------------------------------------------------------ */
/* HMAC                                                               */
/* ------------------------------------------------------------------ */

uint8_t*     crypto_hmac                       (const uint8_t* data, size_t data_len, const uint8_t* key,  size_t key_len, HashAlgorithm algorithm, size_t* outLength);
uint8_t*     crypto_hmac_file                  (const char* path, const uint8_t* key, size_t key_len, HashAlgorithm algorithm, size_t* outLength);
bool         crypto_verify_hmac                (const uint8_t* data, size_t data_len, const uint8_t* key,  size_t key_len, const uint8_t* mac,  size_t mac_len,
                                                HashAlgorithm algorithm);


/* ------------------------------------------------------------------ */
/* Key derivation                                                     */
/* ------------------------------------------------------------------ */

uint8_t*     crypto_pbkdf2                     (const char* password, size_t password_len, const uint8_t* salt,  size_t salt_len, int iterations, size_t key_len,
                                                HashAlgorithm algorithm);


/* ------------------------------------------------------------------ */
/* DES — symmetric cipher                                             */
/* ------------------------------------------------------------------ */

void*        crypto_des_encrypt                (const uint8_t* plaintext, size_t len, const uint8_t* key, const uint8_t* iv, CryptoMode mode, size_t* out_len);
void*        crypto_des_decrypt                (const uint8_t* ciphertext, size_t len, const uint8_t* key, const uint8_t* iv, CryptoMode mode, size_t* out_len);


/* ------------------------------------------------------------------ */
/* Random                                                             */
/* ------------------------------------------------------------------ */

void         crypto_random_bytes               (uint8_t* buffer, size_t length);
void         crypto_generate_random_iv         (uint8_t* iv, size_t length);


/* ------------------------------------------------------------------ */
/* Misc                                                               */
/* ------------------------------------------------------------------ */

bool         crypto_constant_time_equal        (const void* a, const void* b, size_t length);
const char*  crypto_hash_algorithm_name        (HashAlgorithm algorithm);


/* ------------------------------------------------------------------ */
/* Debug                                                              */
/* ------------------------------------------------------------------ */

void         crypto_print_hash                 (const uint8_t* hash, size_t length);


#ifdef __cplusplus
}
#endif

#endif 
