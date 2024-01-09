#include "crypto.h"
#include <stdlib.h>
#include <string.h>
#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <openssl/mdc2.h>

uint8_t* crypto_hash_data(const uint8_t* data, size_t length, enum HashAlgorithm algorithm, size_t *outLength) {
    // Ensure outLength is a valid pointer
    if (!outLength) {
        return NULL;
    }

    *outLength = 0;
    uint8_t* md = NULL;
    EVP_MD_CTX *mdctx;
    const EVP_MD *md_type;
    unsigned char md_value[EVP_MAX_MD_SIZE];

    switch (algorithm) {
        case CRYPTO_MD4:
            md = (uint8_t*)malloc(MD4_DIGEST_LENGTH);
            if (md && MD4(data, length, md)) {
                *outLength = MD4_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_MD5:
            md = (uint8_t*)malloc(MD5_DIGEST_LENGTH);
            if (md && MD5(data, length, md)) {
                *outLength = MD5_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_SHA1:
            md = (uint8_t*)malloc(SHA_DIGEST_LENGTH);
            if (md && SHA1(data, length, md)) {
                *outLength = SHA_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_SHA224:
            md = (uint8_t*)malloc(SHA224_DIGEST_LENGTH);
            if (md && SHA224(data, length, md)) {
                *outLength = SHA224_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_SHA256:
            md = (uint8_t*)malloc(SHA256_DIGEST_LENGTH);
            if (md && SHA256(data, length, md)) {
                *outLength = SHA256_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_SHA384:
            md = (uint8_t*)malloc(SHA384_DIGEST_LENGTH);
            if (md && SHA384(data, length, md)) {
                *outLength = SHA384_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_SHA512:
            md = (uint8_t*)malloc(SHA512_DIGEST_LENGTH);
            if (md && SHA512(data, length, md)) {
                *outLength = SHA512_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_SHA3_224:
            md_type = EVP_sha3_224();
            *outLength = SHA224_DIGEST_LENGTH;  // Set the hash length for SHA3-224
            md = (uint8_t*)malloc(*outLength); // Allocate memory for the hash
            if (!md) {
                perror("Failed to allocate memory for hash");
                return NULL;
            }
            break;
        case CRYPTO_SHA3_256:
            md_type = EVP_sha3_256();
            *outLength = SHA256_DIGEST_LENGTH;
            md = (uint8_t*)malloc(*outLength);
            if (!md) {
                perror("Failed to allocate memory for hash");
                return NULL;
            }
            break;
        case CRYPTO_SHA3_384:
            md_type = EVP_sha3_384();
            *outLength = SHA384_DIGEST_LENGTH;
            md = (uint8_t*)malloc(*outLength);
            if (!md) {
                perror("Failed to allocate memory for hash");
                return NULL;
            }
            break;
        case CRYPTO_SHA3_512:
            md_type = EVP_sha3_512();
            
            *outLength = SHA512_DIGEST_LENGTH;
            md = (uint8_t*)malloc(*outLength);
            if (!md) {
                perror("Failed to allocate memory for hash");
                return NULL;
            }
            break;
        case CRYPTO_SHAKE_128:
            md_type = EVP_shake128();
            *outLength = 32; // Set this to the desired output length for SHAKE128
            break;
        case CRYPTO_SHAKE_256:
            md_type = EVP_shake256();
            *outLength = 64; // Set this to the desired output length for SHAKE256
            break;
        case CRYPTO_BLAKE2B_512:
            md_type = EVP_blake2b512();
            *outLength = 64; // Blake2b512 produces a 512-bit hash (64 bytes)
            break;
        case CRYPTO_BLAKE2S_256:
            md_type = EVP_blake2s256();
            *outLength = 32; // Blake2s256 produces a 256-bit hash (32 bytes)
            break;
        case CRYPTO_MDC2:
            *outLength = MDC2_DIGEST_LENGTH; // MDC2_DIGEST_LENGTH should be 16
            md = (uint8_t*)malloc(*outLength);
            if (!md) {
                perror("Failed to allocate memory for hash");
                return NULL;
            }
            if (!MDC2(data, length, md)) {
                perror("Failed to compute MDC2 hash");
                free(md);
                return NULL;
            }
            break;
        case CRYPTO_RIPEMD_160:
            md_type = EVP_ripemd160();
            *outLength = RIPEMD160_DIGEST_LENGTH; // RIPEMD160_DIGEST_LENGTH should be 20
            break;
        case CRYPTO_SHA512_224:
            md_type = EVP_sha512_224();
            *outLength = 28; // SHA512-224 produces a 224-bit hash (28 bytes)
            break;
        default:
            perror("Unsupported Algorithm");
            break;
    }

    if (algorithm == CRYPTO_RIPEMD_160 || algorithm == CRYPTO_SHA512_224 || algorithm == CRYPTO_BLAKE2B_512 || 
                                            algorithm == CRYPTO_BLAKE2S_256) {
        md = (uint8_t*)malloc(*outLength);
            if (!md) {
                perror("Failed to allocate memory for hash");
                return NULL;
            }

            mdctx = EVP_MD_CTX_new();
            if (!mdctx || !EVP_DigestInit_ex(mdctx, md_type, NULL) ||
                !EVP_DigestUpdate(mdctx, data, length) ||
                !EVP_DigestFinal_ex(mdctx, md, (unsigned int *)outLength)) {
                perror("Failed to compute hash");
                free(md);
                EVP_MD_CTX_free(mdctx);
                return NULL;
            }

            EVP_MD_CTX_free(mdctx);
    }
    else if (algorithm == CRYPTO_SHAKE_128 || algorithm == CRYPTO_SHAKE_256) {
        // SHAKE-specific handling
        md = (uint8_t*)malloc(*outLength);
        if (!md) {
            perror("Failed to allocate memory for hash");
            return NULL;
        }

        EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
        if (!mdctx || !EVP_DigestInit_ex(mdctx, md_type, NULL) ||
            !EVP_DigestUpdate(mdctx, data, length) ||
            !EVP_DigestFinalXOF(mdctx, md, *outLength)) {
            perror("Failed to compute SHAKE hash");
            free(md);
            EVP_MD_CTX_free(mdctx);
            return NULL;
        }

        EVP_MD_CTX_free(mdctx);
    }
    else if (algorithm >= CRYPTO_SHA3_224 && algorithm <= CRYPTO_SHA3_512) {
        // Create and initialize the context
        if ((mdctx = EVP_MD_CTX_new()) == NULL) {
            perror("Failed to create EVP_MD_CTX");
            free(md);
            return NULL;
        }

        // Perform the hash operation
        if (1 != EVP_DigestInit_ex(mdctx, md_type, NULL) ||
            1 != EVP_DigestUpdate(mdctx, data, length) ||
            1 != EVP_DigestFinal_ex(mdctx, md_value, (unsigned int *)outLength)) {
            perror("Failed to compute hash");
            EVP_MD_CTX_free(mdctx);
            free(md);
            return NULL;
        }

        memcpy(md, md_value, *outLength);
        EVP_MD_CTX_free(mdctx);
    }

    return md; // Caller is responsible for freeing this memory
}

void crypto_print_hash(const uint8_t* hash, size_t length) {
    if (hash == NULL) {
        printf("No hash data to print.\n");
        return;
    }

    for (size_t i = 0; i < length; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}
