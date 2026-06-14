/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Crypto
*/


#include <stdlib.h>
#include <string.h>
#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <openssl/mdc2.h>
#include <openssl/des.h>
#include <openssl/err.h>
#include <openssl/hmac.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
#include "crypto.h"

#ifndef DES_BLOCK_SIZE
#define DES_BLOCK_SIZE 8
#endif

/**
 * @brief Computes a cryptographic hash of the provided data using the specified algorithm.
 *
 * This function takes input data and computes its hash using one of the supported cryptographic
 * algorithms (e.g., MD5, SHA-256, SHA3-512). The resulting hash is returned as a dynamically allocated 
 * array of bytes, and the length of the hash is stored in the `outLength` parameter.
 *
 * The caller is responsible for freeing the memory allocated for the hash.
 *
 * @param data Pointer to the input data to be hashed.
 * @param length Length of the input data in bytes.
 * @param algorithm The hash algorithm to use (e.g., CRYPTO_MD5, CRYPTO_SHA256).
 * @param[out] outLength Pointer to a size_t variable where the length of the computed hash will be stored.
 *
 * @return Pointer to the computed hash (as a byte array), or NULL if an error occurs. The caller must free the returned memory.
 *
 * @note The `outLength` parameter must be a valid pointer. If it is NULL, the function will return NULL.
 */
uint8_t* crypto_hash_data(const uint8_t* data, size_t length, HashAlgorithm algorithm, size_t *outLength) {
    if (!outLength) {
        CRYPTO_LOG("[crypto_hash_data] Invalid outLength pointer");
        return NULL;
    }

    CRYPTO_LOG("[crypto_hash_data] Starting hash computation for algorithm %d", algorithm);
    *outLength = 0;
    uint8_t* md = NULL;
    EVP_MD_CTX *mdctx;
    const EVP_MD *md_type;
    // unsigned char md_value[EVP_MAX_MD_SIZE];

    switch (algorithm) {
        case CRYPTO_MD4:
            CRYPTO_LOG("[crypto_hash_data] Using MD4 algorithm");
            md = (uint8_t*)malloc(MD4_DIGEST_LENGTH);
            if (md && MD4(data, length, md)) {
                *outLength = MD4_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_MD5:
            CRYPTO_LOG("[crypto_hash_data] Using MD5 algorithm");
            md = (uint8_t*)malloc(MD5_DIGEST_LENGTH);
            if (md && MD5(data, length, md)) {
                *outLength = MD5_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_SHA1:
            CRYPTO_LOG("[crypto_hash_data] Using SHA1 algorithm");
            md = (uint8_t*)malloc(SHA_DIGEST_LENGTH);
            if (md && SHA1(data, length, md)) {
                *outLength = SHA_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_SHA224:
            CRYPTO_LOG("[crypto_hash_data] Using SHA224 algorithm");
            md = (uint8_t*)malloc(SHA224_DIGEST_LENGTH);
            if (md && SHA224(data, length, md)) {
                *outLength = SHA224_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_SHA256:
            CRYPTO_LOG("[crypto_hash_data] Using SHA256 algorithm");
            md = (uint8_t*)malloc(SHA256_DIGEST_LENGTH);
            if (md && SHA256(data, length, md)) {
                *outLength = SHA256_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_SHA384:
            CRYPTO_LOG("[crypto_hash_data] Using SHA384 algorithm");
            md = (uint8_t*)malloc(SHA384_DIGEST_LENGTH);
            if (md && SHA384(data, length, md)) {
                *outLength = SHA384_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_SHA512:
            CRYPTO_LOG("[crypto_hash_data] Using SHA512 algorithm");
            md = (uint8_t*)malloc(SHA512_DIGEST_LENGTH);
            if (md && SHA512(data, length, md)) {
                *outLength = SHA512_DIGEST_LENGTH;
            }
            break;
        case CRYPTO_SHA3_224:
        case CRYPTO_SHA3_256:
        case CRYPTO_SHA3_384:
        case CRYPTO_SHA3_512:
            CRYPTO_LOG("[crypto_hash_data] Using SHA3 algorithm");
            switch (algorithm) {
                case CRYPTO_SHA3_224:
                    md_type = EVP_sha3_224();
                    *outLength = SHA224_DIGEST_LENGTH;
                    break;
                case CRYPTO_SHA3_256:
                    md_type = EVP_sha3_256();
                    *outLength = SHA256_DIGEST_LENGTH;
                    break;
                case CRYPTO_SHA3_384:
                    md_type = EVP_sha3_384();
                    *outLength = SHA384_DIGEST_LENGTH;
                    break;
                case CRYPTO_SHA3_512:
                    md_type = EVP_sha3_512();
                    *outLength = SHA512_DIGEST_LENGTH;
                    break;
                default:
                    CRYPTO_LOG("[crypto_hash_data] Unsupported algorithm");
                    return NULL;
            }
            md = (uint8_t*)malloc(*outLength);
            if (!md) {
                CRYPTO_LOG("[crypto_hash_data] Failed to allocate memory for hash");
                return NULL;
            }

            mdctx = EVP_MD_CTX_new();
            if (!mdctx || !EVP_DigestInit_ex(mdctx, md_type, NULL) ||
                !EVP_DigestUpdate(mdctx, data, length) ||
                !EVP_DigestFinal_ex(mdctx, md, (unsigned int *)outLength)) {
                CRYPTO_LOG("[crypto_hash_data] Failed to compute SHA3 hash");
                free(md);
                EVP_MD_CTX_free(mdctx);
                return NULL;
            }

            EVP_MD_CTX_free(mdctx);
            break;

        case CRYPTO_SHAKE_128:
        case CRYPTO_SHAKE_256:
            CRYPTO_LOG("[crypto_hash_data] Using SHAKE algorithm");
            switch (algorithm) {
                case CRYPTO_SHAKE_128:
                    md_type = EVP_shake128();
                    *outLength = 32; // desired length for SHAKE128
                    break;
                case CRYPTO_SHAKE_256:
                    md_type = EVP_shake256();
                    *outLength = 64; // desired length for SHAKE256
                    break;
                default:
                    CRYPTO_LOG("[crypto_hash_data] Unsupported algorithm");
                    return NULL;
            }
            md = (uint8_t*)malloc(*outLength);
            if (!md) {
                CRYPTO_LOG("[crypto_hash_data] Failed to allocate memory for SHAKE hash");
                return NULL;
            }

            mdctx = EVP_MD_CTX_new();
            if (!mdctx || !EVP_DigestInit_ex(mdctx, md_type, NULL) ||
                !EVP_DigestUpdate(mdctx, data, length) ||
                !EVP_DigestFinalXOF(mdctx, md, *outLength)) {
                CRYPTO_LOG("[crypto_hash_data] Failed to compute SHAKE hash");
                free(md);
                EVP_MD_CTX_free(mdctx);
                return NULL;
            }

            EVP_MD_CTX_free(mdctx);
            break;

        case CRYPTO_BLAKE2B_512:
        case CRYPTO_BLAKE2S_256:
        case CRYPTO_RIPEMD_160:
        case CRYPTO_SHA512_224:
            CRYPTO_LOG("[crypto_hash_data] Using EVP-based algorithm");
            switch (algorithm) {
                case CRYPTO_BLAKE2B_512:
                    md_type = EVP_blake2b512();
                    *outLength = 64;
                    break;
                case CRYPTO_BLAKE2S_256:
                    md_type = EVP_blake2s256();
                    *outLength = 32;
                    break;
                case CRYPTO_RIPEMD_160:
                    md_type = EVP_ripemd160();
                    *outLength = RIPEMD160_DIGEST_LENGTH;
                    break;
                case CRYPTO_SHA512_224:
                    md_type = EVP_sha512_224();
                    *outLength = 28;
                    break;
                default:
                    CRYPTO_LOG("[crypto_hash_data] Unsupported algorithm");
                    return NULL;
            }
            md = (uint8_t*)malloc(*outLength);
            if (!md) {
                CRYPTO_LOG("[crypto_hash_data] Failed to allocate memory for hash");
                return NULL;
            }

            mdctx = EVP_MD_CTX_new();
            if (!mdctx || !EVP_DigestInit_ex(mdctx, md_type, NULL) ||
                !EVP_DigestUpdate(mdctx, data, length) ||
                !EVP_DigestFinal_ex(mdctx, md, (unsigned int *)outLength)) {
                CRYPTO_LOG("[crypto_hash_data] Failed to compute hash");
                free(md);
                EVP_MD_CTX_free(mdctx);
                return NULL;
            }

            EVP_MD_CTX_free(mdctx);
            break;

        case CRYPTO_MDC2:
#ifdef OPENSSL_NO_MDC2
            /* MDC2 is a legacy digest that many OpenSSL builds (e.g. the default
               Ubuntu/Debian package) compile out via OPENSSL_NO_MDC2. When it is
               unavailable, report failure instead of failing to compile. */
            CRYPTO_LOG("[crypto_hash_data] MDC2 is not available in this OpenSSL build");
            *outLength = 0;
            return NULL;
#else
            CRYPTO_LOG("[crypto_hash_data] Using MDC2 algorithm");
            md = (uint8_t*)malloc(MDC2_DIGEST_LENGTH);
            if (!md) {
                CRYPTO_LOG("[crypto_hash_data] Failed to allocate memory for MDC2 hash");
                return NULL;
            }
            if (!MDC2(data, length, md)) {
                CRYPTO_LOG("[crypto_hash_data] Failed to compute MDC2 hash");
                free(md);
                return NULL;
            }
            *outLength = MDC2_DIGEST_LENGTH;
            break;
#endif

        default:
            CRYPTO_LOG("[crypto_hash_data] Unsupported algorithm");
            return NULL;
    }

    CRYPTO_LOG("[crypto_hash_data] Hash computation completed successfully");
    return md; 
}

/**
 * @brief Prints a cryptographic hash in hexadecimal format.
 *
 * This function prints the provided hash data as a hexadecimal string. Each byte of the hash is
 * printed as two hexadecimal characters.
 *
 * @param hash Pointer to the hash data to be printed.
 * @param length Length of the hash data in bytes.
 *
 * @note If the `hash` parameter is NULL, the function prints "No hash data to print."
 */
void crypto_print_hash(const uint8_t* hash, size_t length) {
    if (hash == NULL) {
        CRYPTO_LOG("[crypto_print_hash] No hash data to print.");
        printf("No hash data to print.\n");
        return;
    }

    CRYPTO_LOG("[crypto_print_hash] Printing hash data of length %zu", length);
    for (size_t i = 0; i < length; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

/**
 * @brief Encrypts plaintext using the DES (Data Encryption Standard) algorithm.
 *
 * This function encrypts the provided plaintext using DES with the specified mode of operation (ECB, CBC, CFB, or OFB).
 * The plaintext is padded to ensure its length is a multiple of the DES block size (8 bytes).
 * 
 * @param plaintext Pointer to the input plaintext data to be encrypted.
 * @param len Length of the plaintext data in bytes.
 * @param key Pointer to the key used for encryption (must be 8 bytes for DES).
 * @param iv Pointer to the initialization vector (IV) for modes that require it (CBC, CFB, OFB). If NULL, a default IV of all zeros is used.
 * @param mode The mode of operation (CRYPTO_MODE_ECB, CRYPTO_MODE_CBC, CRYPTO_MODE_CFB, CRYPTO_MODE_OFB).
 * @param out_len Pointer to a size_t variable where the length of the encrypted data will be stored.
 * 
 * @return Pointer to the encrypted data, or NULL if an error occurs. The caller is responsible for freeing this memory.
 */
void* crypto_des_encrypt(const uint8_t* plaintext, size_t len, const uint8_t* key, const uint8_t* iv, CryptoMode mode, size_t* out_len) {
    if (!out_len || !key || !plaintext) {
        CRYPTO_LOG("[crypto_des_encrypt] Invalid input: out_len, key, or plaintext is NULL");
        return NULL;
    }

    CRYPTO_LOG("[crypto_des_encrypt] Starting encryption process");

    // Pad length to 8 byte boundary
    size_t padded_len = (len + DES_BLOCK_SIZE - 1) & ~(DES_BLOCK_SIZE - 1);
    *out_len = padded_len;

    CRYPTO_LOG("[crypto_des_encrypt] Allocating memory for padded plaintext (size: %zu)", padded_len);
    uint8_t* padded_plaintext = (uint8_t*) calloc(padded_len, sizeof(uint8_t));
    if (!padded_plaintext) {
        CRYPTO_LOG("[crypto_des_encrypt] Memory allocation failed for padded plaintext");
        return NULL;
    }
    memcpy(padded_plaintext, plaintext, len);

    CRYPTO_LOG("[crypto_des_encrypt] Allocating memory for ciphertext (size: %zu)", padded_len);
    uint8_t* ciphertext = (uint8_t*) malloc(padded_len);
    if (!ciphertext) {
        CRYPTO_LOG("[crypto_des_encrypt] Memory allocation failed for ciphertext");
        free(padded_plaintext);
        return NULL;
    }

    DES_cblock keyBlock;
    memcpy(keyBlock, key, DES_BLOCK_SIZE);
    DES_key_schedule keysched;

    CRYPTO_LOG("[crypto_des_encrypt] Setting odd parity for the key");
    DES_set_odd_parity(&keyBlock);

    CRYPTO_LOG("[crypto_des_encrypt] Checking and setting the key");
    if (DES_set_key_checked(&keyBlock, &keysched) != 0) {
        CRYPTO_LOG("[crypto_des_encrypt] Failed to set the key");
        free(ciphertext);
        free(padded_plaintext);
        return NULL;
    }

    DES_cblock ivec;
    if (iv) {
        CRYPTO_LOG("[crypto_des_encrypt] Using provided IV");
        memcpy(ivec, iv, DES_BLOCK_SIZE);
    } 
    else {
        CRYPTO_LOG("[crypto_des_encrypt] No IV provided, using default (all zeros)");
        memset(ivec, 0, DES_BLOCK_SIZE); // Default IV to all zeros if none provided
    }

    // CFB and OFB are stream-style operations that consume the entire buffer
    // in a single OpenSSL call; encrypting block-by-block inside a loop (as
    // the previous code did) re-encrypted the whole buffer N times with a
    // mutated IV per iteration, producing garbage.
    if (mode == CRYPTO_MODE_CFB) {
        CRYPTO_LOG("[crypto_des_encrypt] Encrypting whole buffer in CFB mode");
        DES_cfb_encrypt(padded_plaintext, ciphertext, 8, (long)padded_len, &keysched, &ivec, DES_ENCRYPT);
    }
    else if (mode == CRYPTO_MODE_OFB) {
        CRYPTO_LOG("[crypto_des_encrypt] Encrypting whole buffer in OFB mode");
        DES_ofb_encrypt(padded_plaintext, ciphertext, 8, (long)padded_len, &keysched, &ivec);
    }
    else {
        // ECB and CBC operate on one block per iteration.
        for (size_t i = 0; i < padded_len; i += DES_BLOCK_SIZE) {
            DES_cblock block;
            memcpy(block, padded_plaintext + i, DES_BLOCK_SIZE);

            switch (mode) {
                case CRYPTO_MODE_ECB:
                    CRYPTO_LOG("[crypto_des_encrypt] Encrypting block in ECB mode");
                    DES_ecb_encrypt(&block, (DES_cblock*)(ciphertext + i), &keysched, DES_ENCRYPT);
                    break;
                case CRYPTO_MODE_CBC:
                    CRYPTO_LOG("[crypto_des_encrypt] Encrypting block in CBC mode");
                    DES_ncbc_encrypt(block, ciphertext + i, DES_BLOCK_SIZE, &keysched, &ivec, DES_ENCRYPT);
                    break;
                default:
                    CRYPTO_LOG("[crypto_des_encrypt] Unknown mode, encryption failed");
                    free(ciphertext);
                    free(padded_plaintext);
                    return NULL;
            }
        }
    }

    CRYPTO_LOG("[crypto_des_encrypt] Encryption process completed successfully");

    free(padded_plaintext);
    return ciphertext; // Caller is responsible for freeing this memory
}

/**
 * @brief Decrypts ciphertext using the DES (Data Encryption Standard) algorithm.
 *
 * This function decrypts the provided ciphertext using DES with the specified mode of operation (ECB, CBC, CFB, or OFB).
 * The function assumes that the ciphertext length is a multiple of the DES block size (8 bytes).
 * 
 * @param ciphertext Pointer to the input ciphertext data to be decrypted.
 * @param len Length of the ciphertext data in bytes.
 * @param key Pointer to the key used for decryption (must be 8 bytes for DES).
 * @param iv Pointer to the initialization vector (IV) for modes that require it (CBC, CFB, OFB). If NULL, a default IV of all zeros is used.
 * @param mode The mode of operation (CRYPTO_MODE_ECB, CRYPTO_MODE_CBC, CRYPTO_MODE_CFB, CRYPTO_MODE_OFB).
 * @param out_len Pointer to a size_t variable where the length of the decrypted data will be stored.
 * 
 * @return Pointer to the decrypted data, or NULL if an error occurs. The caller is responsible for freeing this memory.
 */
void* crypto_des_decrypt(const uint8_t* ciphertext, size_t len, const uint8_t* key, const uint8_t* iv, CryptoMode mode, size_t* out_len) {
    if (!out_len || !key || !ciphertext) {
        CRYPTO_LOG("[crypto_des_decrypt] Invalid input: out_len, key, or ciphertext is NULL");
        return NULL;
    }

    CRYPTO_LOG("[crypto_des_decrypt] Starting decryption");
    *out_len = len;
    uint8_t* plaintext = (uint8_t*) malloc(len);
    if (!plaintext) {
        CRYPTO_LOG("[crypto_des_decrypt] Failed to allocate memory for plaintext");
        return NULL;
    }

    DES_cblock keyBlock;
    memcpy(keyBlock, key, DES_BLOCK_SIZE);
    DES_key_schedule keysched;

    CRYPTO_LOG("[crypto_des_decrypt] Setting odd parity for the key");
    DES_set_odd_parity(&keyBlock);

    CRYPTO_LOG("[crypto_des_decrypt] Checking and setting key");
    if (DES_set_key_checked(&keyBlock, &keysched) != 0) {
        CRYPTO_LOG("[crypto_des_decrypt] Failed to set the key");
        free(plaintext);
        return NULL;
    }

    DES_cblock ivec;
    if (iv) {
        CRYPTO_LOG("[crypto_des_decrypt] Using provided IV");
        memcpy(ivec, iv, DES_BLOCK_SIZE);
    } 
    else {
        CRYPTO_LOG("[crypto_des_decrypt] No IV provided, using default (all zeros)");
        memset(ivec, 0, DES_BLOCK_SIZE); // Default IV to all zeros if none provided
    }

    // Same single-call handling for CFB/OFB as in encrypt — the per-block
    // loop version corrupted the result by re-running on the full buffer
    // with mutated IV every iteration. Additionally CFB decrypt previously
    // passed DES_ENCRYPT for the direction argument, which is wrong.
    if (mode == CRYPTO_MODE_CFB) {
        CRYPTO_LOG("[crypto_des_decrypt] Decrypting whole buffer in CFB mode");
        DES_cfb_encrypt(ciphertext, plaintext, 8, (long)len, &keysched, &ivec, DES_DECRYPT);
    }
    else if (mode == CRYPTO_MODE_OFB) {
        // OFB is symmetric — same operation for encrypt and decrypt.
        CRYPTO_LOG("[crypto_des_decrypt] Decrypting whole buffer in OFB mode");
        DES_ofb_encrypt(ciphertext, plaintext, 8, (long)len, &keysched, &ivec);
    }
    else {
        for (size_t i = 0; i < len; i += DES_BLOCK_SIZE) {
            DES_cblock block;
            memcpy(block, ciphertext + i, DES_BLOCK_SIZE);

            switch (mode) {
                case CRYPTO_MODE_ECB:
                    CRYPTO_LOG("[crypto_des_decrypt] Decrypting block in ECB mode");
                    DES_ecb_encrypt(&block, (DES_cblock*)(plaintext + i), &keysched, DES_DECRYPT);
                    break;
                case CRYPTO_MODE_CBC:
                    CRYPTO_LOG("[crypto_des_decrypt] Decrypting block in CBC mode");
                    DES_ncbc_encrypt(block, plaintext + i, DES_BLOCK_SIZE, &keysched, &ivec, DES_DECRYPT);
                    break;
                default:
                    CRYPTO_LOG("[crypto_des_decrypt] Unknown mode, decryption failed");
                    free(plaintext);
                    return NULL;
            }
        }
    }

    CRYPTO_LOG("[crypto_des_decrypt] Decryption completed successfully");
    return plaintext; // Caller is responsible for freeing this memory
}

/**
 * @brief Generates a random Initialization Vector (IV).
 *
 * Cross-platform implementation: on Windows uses the Win32 Cryptographic
 * API (`CryptAcquireContext` / `CryptGenRandom`); on POSIX systems reads
 * from `/dev/urandom`. On failure the buffer is zero-filled — the
 * library never aborts the process.
 *
 * @param iv     Pointer to a buffer where the generated IV will be stored.
 * @param length The length of the IV to generate, in bytes.
 */
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#include <Wincrypt.h>

void crypto_generate_random_iv(uint8_t *iv, size_t length) {
    CRYPTO_LOG("[crypto_generate_random_iv] enter (iv=%p, length=%zu)", (void*)iv, length);
    if (!iv || length == 0) {
        CRYPTO_LOG("[crypto_generate_random_iv] invalid args: iv=%p, length=%zu", (void*)iv, length);
        return;
    }

    HCRYPTPROV hCryptProv;

    CRYPTO_LOG("[crypto_generate_random_iv] Acquiring cryptographic context");
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        // Project convention: a library never aborts the process. On failure,
        // fall back to a deterministic zero IV and let the caller proceed.
        CRYPTO_LOG("[crypto_generate_random_iv] CryptAcquireContext failed: %lu — zero IV", GetLastError());
        memset(iv, 0, length);
        return;
    }

    CRYPTO_LOG("[crypto_generate_random_iv] Generating random bytes");
    if (!CryptGenRandom(hCryptProv, (DWORD)length, iv)) {
        CRYPTO_LOG("[crypto_generate_random_iv] CryptGenRandom failed: %lu — zero IV", GetLastError());
        memset(iv, 0, length);
        CryptReleaseContext(hCryptProv, 0);
        return;
    }

    CRYPTO_LOG("[crypto_generate_random_iv] Releasing cryptographic context");
    CryptReleaseContext(hCryptProv, 0);
    CRYPTO_LOG("[crypto_generate_random_iv] exit -> success (length=%zu)", length);
}

#else

#include <fcntl.h>
#include <unistd.h>

void crypto_generate_random_iv(uint8_t *iv, size_t length) {
    CRYPTO_LOG("[crypto_generate_random_iv] enter (iv=%p, length=%zu)", (void*)iv, length);
    if (!iv || length == 0) {
        CRYPTO_LOG("[crypto_generate_random_iv] invalid args: iv=%p, length=%zu", (void*)iv, length);
        return;
    }

    CRYPTO_LOG("[crypto_generate_random_iv] Opening /dev/urandom");
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        // Fall back to zero IV — never abort the process.
        CRYPTO_LOG("[crypto_generate_random_iv] Failed to open /dev/urandom — zero IV");
        memset(iv, 0, length);
        return;
    }

    CRYPTO_LOG("[crypto_generate_random_iv] Reading random data");
    ssize_t read_bytes = read(fd, iv, length);
    close(fd);
    if (read_bytes != (ssize_t)length) {
        CRYPTO_LOG("[crypto_generate_random_iv] Failed to read from /dev/urandom — zero IV");
        memset(iv, 0, length);
        return;
    }

    CRYPTO_LOG("[crypto_generate_random_iv] Successfully generated IV");
    CRYPTO_LOG("[crypto_generate_random_iv] exit -> success (length=%zu)", length);
}

#endif


/* Translate our HashAlgorithm enum into the matching `EVP_MD*` so the
   helpers below don't each carry their own switch. Returns NULL on
   unsupported / unknown values. */
static const EVP_MD* crypto_evp_md(HashAlgorithm a) {
    switch (a) {
        case CRYPTO_MD4:          return EVP_md4();
        case CRYPTO_MD5:          return EVP_md5();
        case CRYPTO_SHA1:         return EVP_sha1();
        case CRYPTO_SHA224:       return EVP_sha224();
        case CRYPTO_SHA256:       return EVP_sha256();
        case CRYPTO_SHA384:       return EVP_sha384();
        case CRYPTO_SHA512:       return EVP_sha512();
        case CRYPTO_SHA3_224:     return EVP_sha3_224();
        case CRYPTO_SHA3_256:     return EVP_sha3_256();
        case CRYPTO_SHA3_384:     return EVP_sha3_384();
        case CRYPTO_SHA3_512:     return EVP_sha3_512();
        case CRYPTO_BLAKE2B_512:  return EVP_blake2b512();
        case CRYPTO_BLAKE2S_256:  return EVP_blake2s256();
        case CRYPTO_RIPEMD_160:   return EVP_ripemd160();
        case CRYPTO_SHA512_224:   return EVP_sha512_224();
#ifndef OPENSSL_NO_MDC2
        case CRYPTO_MDC2:         return EVP_mdc2();
#endif
        /* SHAKE has variable output length; not addressable by this
           wrapper. */
        case CRYPTO_SHAKE_128:
        case CRYPTO_SHAKE_256:
        default:                  return NULL;
    }
}

/**
 * @brief Hash a NUL-terminated C string.
 *
 * Convenience wrapper around `crypto_hash_data` that supplies
 * `strlen(str)` for you. NULL input is rejected (returns NULL).
 *
 * @param str        Null-terminated input string. NULL → NULL.
 * @param algorithm  Same `HashAlgorithm` set as `crypto_hash_data`.
 * @param outLength  Receives the hash length on success. Required (non-NULL).
 * @return           Heap-allocated hash bytes (caller frees), or NULL.
 *
 * @code
 * size_t n = 0;
 * uint8_t* h = crypto_hash_string("hello", CRYPTO_SHA256, &n);
 * crypto_print_hash(h, n);       // 2cf24dba5fb0a30e26e83b2ac5b9e29e1b...
 * free(h);
 * @endcode
 */
uint8_t* crypto_hash_string(const char* str, HashAlgorithm algorithm, size_t* outLength) {
    if (!str || !outLength) {
        if (outLength) *outLength = 0;
        return NULL;
    }
    return crypto_hash_data((const uint8_t*)str, strlen(str), algorithm, outLength);
}

/**
 * @brief Convert a binary hash to a lowercase hexadecimal string.
 *
 * Returns a freshly-allocated NUL-terminated string of length
 * `2 * length` (caller frees with `free()`).
 *
 * @param hash    Hash bytes.
 * @param length  Number of bytes in `hash`.
 * @return        Heap string, or NULL on bad input / OOM.
 *
 * @code
 * size_t n = 0;
 * uint8_t* h = crypto_hash_string("abc", CRYPTO_SHA256, &n);
 * char* hex = crypto_hash_to_hex(h, n);
 * printf("sha256(abc) = %s\n", hex);
 * // ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
 * free(hex); free(h);
 * @endcode
 */
char* crypto_hash_to_hex(const uint8_t* hash, size_t length) {
    CRYPTO_LOG("[crypto_hash_to_hex]: enter (hash=%p, length=%zu)", (const void*)hash, length);
    if (!hash) {
        CRYPTO_LOG("[crypto_hash_to_hex]: NULL hash -> NULL");
        return NULL;
    }
    char* out = (char*)malloc(length * 2 + 1);
    if (!out) {
        CRYPTO_LOG("[crypto_hash_to_hex]: malloc failed -> NULL");
        return NULL;
    }
    static const char digits[] = "0123456789abcdef";
    for (size_t i = 0; i < length; ++i) {
        out[i * 2]     = digits[(hash[i] >> 4) & 0x0F];
        out[i * 2 + 1] = digits[ hash[i]       & 0x0F];
    }
    out[length * 2] = '\0';
    CRYPTO_LOG("[crypto_hash_to_hex]: exit -> %p (length=%zu)", (void*)out, length * 2);
    return out;
}


/**
 * @brief Convert a binary hash (or any byte buffer) to a base64 string.
 *
 * Wraps OpenSSL's `EVP_EncodeBlock`. The result is NUL-terminated and
 * does NOT include line breaks. Useful for storing digests / MACs in
 * config files, HTTP headers, or JSON.
 *
 * @param hash    Bytes to encode.
 * @param length  Number of bytes.
 * @return        Heap-allocated base64 string (caller frees), or NULL.
 *
 * @code
 * size_t n = 0;
 * uint8_t* d = crypto_hash_string("hello", CRYPTO_SHA256, &n);
 * char* b64 = crypto_hash_to_base64(d, n);
 * printf("base64 = %s\n", b64);     // LPJNul+wow4m6DsqxbninhsWHlwfp0J...
 * free(b64); free(d);
 * @endcode
 */
char* crypto_hash_to_base64(const uint8_t* hash, size_t length) {
    CRYPTO_LOG("[crypto_hash_to_base64]: enter (hash=%p, length=%zu)", (const void*)hash, length);
    if (!hash) {
        CRYPTO_LOG("[crypto_hash_to_base64]: NULL hash -> NULL");
        return NULL;
    }
    /* EVP_EncodeBlock writes 4 chars per 3 input bytes + NUL. */
    int b64_len = 4 * (int)((length + 2) / 3) + 1;
    char* out = (char*)malloc(b64_len);
    if (!out) {
        CRYPTO_LOG("[crypto_hash_to_base64]: malloc failed -> NULL");
        return NULL;
    }
    int written = EVP_EncodeBlock((unsigned char*)out, hash, (int)length);
    if (written < 0) {
        free(out);
        return NULL;
    }
    out[written] = '\0';
    return out;
}


/**
 * @brief Stream-hash the contents of a file.
 *
 * Reads `path` in 64 KiB chunks and feeds each chunk to the digest
 * — never holds the whole file in memory. Returns the resulting
 * hash on the heap (caller frees).
 *
 * @param path        Filesystem path. NULL → NULL.
 * @param algorithm   Any `HashAlgorithm` supported by `crypto_evp_md`
 *                    (SHAKE_128/256 are not supported here — use
 *                    `crypto_hash_data` for variable-length output).
 * @param outLength   Receives the hash length on success.
 * @return            Heap-allocated digest, or NULL on any error.
 *
 * @code
 * size_t n = 0;
 * uint8_t* d = crypto_hash_file("./README.md", CRYPTO_SHA256, &n);
 * if (d) { crypto_print_hash(d, n); free(d); }
 * @endcode
 */
uint8_t* crypto_hash_file(const char* path, HashAlgorithm algorithm, size_t* outLength) {
    if (!path || !outLength) {
        if (outLength) *outLength = 0;
        return NULL;
    }
    const EVP_MD* md = crypto_evp_md(algorithm);
    if (!md) {
        CRYPTO_LOG("[crypto_hash_file] Algorithm %d not supported for file hashing", algorithm);
        *outLength = 0;
        return NULL;
    }
    FILE* f = fopen(path, "rb");
    if (!f) {
        CRYPTO_LOG("[crypto_hash_file] Cannot open %s", path);
        *outLength = 0;
        return NULL;
    }
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx || !EVP_DigestInit_ex(ctx, md, NULL)) {
        if (ctx) {
            EVP_MD_CTX_free(ctx);
        }
        fclose(f);
        *outLength = 0;
        return NULL;
    }
    unsigned char buf[64 * 1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        if (!EVP_DigestUpdate(ctx, buf, n)) {
            EVP_MD_CTX_free(ctx);
            fclose(f);
            *outLength = 0;
            return NULL;
        }
    }
    fclose(f);

    unsigned int hash_len = (unsigned int)EVP_MD_size(md);
    uint8_t* out = (uint8_t*)malloc(hash_len);
    if (!out) { 
        EVP_MD_CTX_free(ctx); 
        *outLength = 0; 
        return NULL; 
    }
    if (!EVP_DigestFinal_ex(ctx, out, &hash_len)) {
        free(out);
        EVP_MD_CTX_free(ctx);
        *outLength = 0;
        return NULL;
    }
    EVP_MD_CTX_free(ctx);
    *outLength = (size_t)hash_len;
    return out;
}


/**
 * @brief Compute an HMAC (Hash-based Message Authentication Code).
 *
 * HMAC is the standard primitive for symmetric-key message
 * authentication. Pair the hash algorithm with the security level
 * you want: HMAC-SHA256 (32-byte output) is the common production
 * choice.
 *
 * @param data        Bytes to authenticate.
 * @param data_len    Length of `data` in bytes.
 * @param key         HMAC key (any length; long keys are first hashed).
 * @param key_len     Length of `key` in bytes.
 * @param algorithm   Hash algorithm to drive HMAC.
 * @param outLength   Receives the MAC length on success.
 * @return            Heap-allocated MAC bytes (caller frees), or NULL.
 *
 * @code
 * const char* msg = "The quick brown fox jumps over the lazy dog";
 * const char* key = "key";
 * size_t mac_len = 0;
 * uint8_t* mac = crypto_hmac((const uint8_t*)msg, strlen(msg),
 *                            (const uint8_t*)key, strlen(key),
 *                            CRYPTO_SHA256, &mac_len);
 * char* hex = crypto_hash_to_hex(mac, mac_len);
 * printf("HMAC-SHA256 = %s\n", hex);
 * // f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8
 * free(hex); free(mac);
 * @endcode
 */
uint8_t* crypto_hmac(const uint8_t* data, size_t data_len, const uint8_t* key,  size_t key_len, HashAlgorithm algorithm, size_t* outLength) {
    if (!data || !key || !outLength) {
        if (outLength) *outLength = 0;
        return NULL;
    }
    const EVP_MD* md = crypto_evp_md(algorithm);
    if (!md) {
        CRYPTO_LOG("[crypto_hmac] Algorithm %d not supported", algorithm);
        *outLength = 0;
        return NULL;
    }
    unsigned int mac_len = (unsigned int)EVP_MD_size(md);
    uint8_t* mac = (uint8_t*)malloc(mac_len);
    if (!mac) { 
        *outLength = 0; 
        return NULL; 
    }

    if (!HMAC(md, key, (int)key_len, data, data_len, mac, &mac_len)) {
        CRYPTO_LOG("[crypto_hmac] HMAC computation failed");
        free(mac);
        *outLength = 0;
        return NULL;
    }
    *outLength = (size_t)mac_len;
    return mac;
}


/**
 * @brief Constant-time byte-equality.
 *
 * Compares `length` bytes from `a` and `b` and returns `true` iff
 * every byte matches. The runtime does NOT depend on the position of
 * the first differing byte — essential when comparing MACs, hashes,
 * or any other secret-derived value. Always compares all bytes
 * regardless of mismatches.
 *
 * @param a       Pointer to first buffer.
 * @param b       Pointer to second buffer.
 * @param length  Bytes to compare.
 * @return        `true` if equal, `false` otherwise. Returns `false`
 *                if either pointer is NULL.
 *
 * @code
 * uint8_t mac_calc[32], mac_recv[32];
 * // ... compute mac_calc, receive mac_recv ...
 * if (!crypto_constant_time_equal(mac_calc, mac_recv, 32)) {
 *     reject_request();   // timing-safe rejection
 * }
 * @endcode
 */
bool crypto_constant_time_equal(const void* a, const void* b, size_t length) {
    if (!a || !b) {
        return false;
    }

    const unsigned char* pa = (const unsigned char*)a;
    const unsigned char* pb = (const unsigned char*)b;
    unsigned char diff = 0;

    for (size_t i = 0; i < length; ++i) {
        diff |= (unsigned char)(pa[i] ^ pb[i]);
    }
    return diff == 0;
}


/**
 * @brief Fill a buffer with cryptographically-secure random bytes.
 *
 * Wraps OpenSSL's `RAND_bytes`, which on production builds is backed
 * by the platform CSPRNG (BCryptGenRandom on Windows, getrandom() /
 * /dev/urandom on POSIX). On failure (extremely rare), the buffer is
 * zero-filled and the function returns silently — the library never
 * aborts the process. Inspect the OpenSSL error queue with
 * `ERR_get_error()` if you need to know.
 *
 * @param buffer  Destination buffer.
 * @param length  Number of bytes to fill.
 *
 * @code
 * uint8_t key[32];
 * crypto_random_bytes(key, sizeof(key));
 * @endcode
 */
void crypto_random_bytes(uint8_t* buffer, size_t length) {
    if (!buffer || length == 0) {
        return;
    }
    if (RAND_bytes(buffer, (int)length) != 1) {
        CRYPTO_LOG("[crypto_random_bytes] RAND_bytes failed — zero-filling");
        memset(buffer, 0, length);
    }
}

/**
 * @brief Human-readable name for a `HashAlgorithm` value.
 *
 * Returns a static, immutable string suitable for logging and error
 * messages. Unknown values return "UNKNOWN".
 *
 * @param algorithm  Enum value.
 * @return           Constant string. Never NULL.
 *
 * @code
 * printf("Using %s\n", crypto_hash_algorithm_name(CRYPTO_SHA256));
 * // Using SHA-256
 * @endcode
 */
const char* crypto_hash_algorithm_name(HashAlgorithm algorithm) {
    switch (algorithm) {
        case CRYPTO_MD4:          return "MD4";
        case CRYPTO_MD5:          return "MD5";
        case CRYPTO_SHA1:         return "SHA-1";
        case CRYPTO_SHA224:       return "SHA-224";
        case CRYPTO_SHA256:       return "SHA-256";
        case CRYPTO_SHA384:       return "SHA-384";
        case CRYPTO_SHA512:       return "SHA-512";
        case CRYPTO_SHA3_224:     return "SHA3-224";
        case CRYPTO_SHA3_256:     return "SHA3-256";
        case CRYPTO_SHA3_384:     return "SHA3-384";
        case CRYPTO_SHA3_512:     return "SHA3-512";
        case CRYPTO_SHAKE_128:    return "SHAKE128";
        case CRYPTO_SHAKE_256:    return "SHAKE256";
        case CRYPTO_BLAKE2B_512:  return "BLAKE2b-512";
        case CRYPTO_BLAKE2S_256:  return "BLAKE2s-256";
        case CRYPTO_MDC2:         return "MDC2";
        case CRYPTO_RIPEMD_160:   return "RIPEMD-160";
        case CRYPTO_SHA512_224:   return "SHA-512/224";
        default:                  return "UNKNOWN";
    }
}


/* Parse one hex character into its 0..15 value; return -1 on miss. */
static int crypto_hex_nibble(int c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }
    return -1;
}


/**
 * @brief Inverse of `crypto_hash_to_hex`: parse a hex string back to
 *        a byte array.
 *
 * Accepts both lowercase and uppercase hex digits. The string must
 * have an even number of characters (otherwise it can't represent a
 * whole byte). Whitespace and `0x` prefixes are NOT accepted — strip
 * those upstream if you need them.
 *
 * @param hex        NUL-terminated hex string. NULL → NULL.
 * @param outLength  Receives the decoded byte length on success.
 *                   Required (non-NULL).
 * @return           Heap-allocated bytes (caller `free`s), or NULL
 *                   on bad input.
 *
 * @code
 * // Read a SHA-256 hash from a config file and use it for comparison.
 * size_t expected_n = 0;
 * uint8_t* expected = crypto_hash_from_hex(
 *     "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
 *     &expected_n);
 *
 * size_t actual_n = 0;
 * uint8_t* actual = crypto_hash_string("abc", CRYPTO_SHA256, &actual_n);
 *
 * bool match = crypto_constant_time_equal(expected, actual, expected_n);
 * free(expected); free(actual);
 * @endcode
 */
uint8_t* crypto_hash_from_hex(const char* hex, size_t* outLength) {
    if (!hex || !outLength) {
        if (outLength) *outLength = 0;
        return NULL;
    }
    size_t hex_len = strlen(hex);
    if (hex_len == 0 || (hex_len & 1)) {
        CRYPTO_LOG("[crypto_hash_from_hex] hex string has invalid length %zu", hex_len);
        *outLength = 0;
        return NULL;
    }
    size_t byte_len = hex_len / 2;
    uint8_t* out = (uint8_t*)malloc(byte_len);
    if (!out) { *outLength = 0; return NULL; }
    for (size_t i = 0; i < byte_len; ++i) {
        int hi = crypto_hex_nibble((unsigned char)hex[i * 2]);
        int lo = crypto_hex_nibble((unsigned char)hex[i * 2 + 1]);
        if (hi < 0 || lo < 0) {
            CRYPTO_LOG("[crypto_hash_from_hex] invalid hex digit at position %zu", i * 2);
            free(out);
            *outLength = 0;
            return NULL;
        }
        out[i] = (uint8_t)((hi << 4) | lo);
    }
    *outLength = byte_len;
    return out;
}


/**
 * @brief Inverse of `crypto_hash_to_base64`: decode a base64 string
 *        back to a byte array.
 *
 * Wraps OpenSSL's `EVP_DecodeBlock`. The input must be padded to a
 * multiple of 4 characters with `=` padding (the standard base64
 * encoding produced by `crypto_hash_to_base64`).
 *
 * @param b64        NUL-terminated base64 string. NULL → NULL.
 * @param outLength  Receives the decoded byte length on success.
 * @return           Heap-allocated bytes (caller `free`s), or NULL
 *                   on bad input.
 *
 * @code
 * size_t n = 0;
 * uint8_t* bytes = crypto_hash_from_base64("AAECAwQFBgcICQoLDA0ODw==", &n);
 * // bytes now holds {0x00, 0x01, ..., 0x0F}; n == 16
 * free(bytes);
 * @endcode
 */
uint8_t* crypto_hash_from_base64(const char* b64, size_t* outLength) {
    if (!b64 || !outLength) {
        if (outLength) *outLength = 0;
        return NULL;
    }
    size_t b64_len = strlen(b64);
    if (b64_len == 0 || (b64_len % 4) != 0) {
        CRYPTO_LOG("[crypto_hash_from_base64] base64 string length %zu is not multiple of 4", b64_len);
        *outLength = 0;
        return NULL;
    }
    /* Output is at most 3 * (b64_len / 4) bytes; allocate that and shrink
       the reported length to discard the `=` padding. */
    size_t alloc = (b64_len / 4) * 3;
    uint8_t* out = (uint8_t*)malloc(alloc);
    if (!out) { 
        *outLength = 0; 
        return NULL; 
    }

    int decoded = EVP_DecodeBlock(out, (const unsigned char*)b64, (int)b64_len);
    if (decoded < 0) {
        free(out);
        *outLength = 0;
        return NULL;
    }
    /* Subtract one byte for each trailing '=' in the input — EVP_DecodeBlock
       includes them in the count but they don't carry data. */
    size_t pad = 0;
    if (b64_len >= 1 && b64[b64_len - 1] == '=') {
        pad++;
    }
    if (b64_len >= 2 && b64[b64_len - 2] == '=') {
        pad++;
    }
    *outLength = (size_t)decoded - pad;
    return out;
}


/**
 * @brief Password-Based Key Derivation Function 2 (PBKDF2).
 *
 * Stretches a low-entropy password into a higher-entropy key using
 * iterated HMAC. The work factor is set by @p iterations — the
 * industry recommendation as of 2023 is at least 600 000 for
 * HMAC-SHA256 (OWASP). Higher iteration counts cost the attacker
 * proportionally more.
 *
 * @param password      Password bytes.
 * @param password_len  Length of @p password in bytes.
 * @param salt          Per-user salt. Should be at least 16 random
 *                      bytes. NULL is rejected.
 * @param salt_len      Length of @p salt in bytes.
 * @param iterations    Number of HMAC iterations. Must be > 0.
 *                      Larger values = stronger key, slower derivation.
 * @param key_len       Desired output key length in bytes.
 * @param algorithm     Hash algorithm driving the HMAC inside PBKDF2
 *                      (typically `CRYPTO_SHA256` or `CRYPTO_SHA512`).
 * @return              Heap-allocated derived key (caller `free`s),
 *                      or NULL on any error.
 *
 * @code
 * uint8_t salt[16];
 * crypto_random_bytes(salt, sizeof(salt));
 *
 * uint8_t* key = crypto_pbkdf2("hunter2", 7, salt, sizeof(salt),
 *                              600000, 32, CRYPTO_SHA256);
 * // Store (salt, iterations, key) in your user database. Re-run the same
 * // function on login to verify.
 * free(key);
 * @endcode
 */
uint8_t* crypto_pbkdf2(const char* password, size_t password_len,
                       const uint8_t* salt,  size_t salt_len,
                       int iterations, size_t key_len,
                       HashAlgorithm algorithm) {
    if (!password || !salt || iterations <= 0 || key_len == 0) {
        CRYPTO_LOG("[crypto_pbkdf2] invalid input");
        return NULL;
    }
    const EVP_MD* md = crypto_evp_md(algorithm);
    if (!md) {
        CRYPTO_LOG("[crypto_pbkdf2] algorithm %d unsupported", algorithm);
        return NULL;
    }
    uint8_t* key = (uint8_t*)malloc(key_len);
    if (!key) { 
        return NULL;
    }
    if (PKCS5_PBKDF2_HMAC(password, (int)password_len,
                          salt, (int)salt_len,
                          iterations, md,
                          (int)key_len, key) != 1) {
        CRYPTO_LOG("[crypto_pbkdf2] PKCS5_PBKDF2_HMAC failed");
        free(key);
        return NULL;
    }
    return key;
}


/**
 * @brief Verify an HMAC in a single safe call.
 *
 * Computes the expected MAC over @p data with @p key, then compares
 * it against the supplied @p mac in constant time. This is the
 * pattern you want for **every** incoming-token / incoming-MAC
 * validation — using `memcmp` instead leaks timing information that
 * lets remote attackers brute-force the MAC one byte at a time.
 *
 * @param data        Message bytes.
 * @param data_len    Length of @p data.
 * @param key         HMAC key.
 * @param key_len     Length of @p key.
 * @param mac         Received MAC to verify.
 * @param mac_len     Length of @p mac. If it doesn't match the
 *                    algorithm's output length, verification fails
 *                    immediately (length mismatch is NOT timing-safe,
 *                    but neither is rejecting it after a constant-time
 *                    compare against a buffer of the wrong size).
 * @param algorithm   Hash algorithm.
 * @return            `true` if the MAC verifies, `false` otherwise.
 *                    Any NULL input → `false`.
 *
 * @code
 * // Server side: validate a signed request.
 * bool ok = crypto_verify_hmac((const uint8_t*)body, body_len,
 *                              (const uint8_t*)secret, secret_len,
 *                              client_mac, client_mac_len,
 *                              CRYPTO_SHA256);
 * if (!ok) reject(401);
 * @endcode
 */
bool crypto_verify_hmac(const uint8_t* data, size_t data_len, const uint8_t* key,  size_t key_len, const uint8_t* mac,  size_t mac_len,
                        HashAlgorithm algorithm) {
    if (!data || !key || !mac) {
        return false;
    }
    
    size_t expected_len = 0;
    uint8_t* expected = crypto_hmac(data, data_len, key, key_len, algorithm, &expected_len);
    if (!expected) {
        return false;
    }

    bool ok = (expected_len == mac_len) && crypto_constant_time_equal(expected, mac, mac_len);
    free(expected);

    return ok;
}


/**
 * @brief Stream-compute HMAC over the contents of a file.
 *
 * Reads `path` in 64 KiB chunks via `HMAC_Update` — never holds the
 * whole file in memory. Useful for verifying signed downloads or
 * authenticating files at rest with a shared key.
 *
 * @param path        Filesystem path. NULL → NULL.
 * @param key         HMAC key.
 * @param key_len     Length of @p key.
 * @param algorithm   Hash algorithm driving HMAC. SHAKE variants are
 *                    not supported (they produce variable-length output).
 * @param outLength   Receives the MAC length on success.
 * @return            Heap-allocated MAC bytes, or NULL on any error.
 *
 * @code
 * size_t n = 0;
 * uint8_t* mac = crypto_hmac_file("./release.tar.gz",
 *                                 (const uint8_t*)"shared_key", 10,
 *                                 CRYPTO_SHA256, &n);
 * char* hex = crypto_hash_to_hex(mac, n);
 * printf("%s  release.tar.gz\n", hex);
 * free(hex); free(mac);
 * @endcode
 */
uint8_t* crypto_hmac_file(const char* path, const uint8_t* key, size_t key_len, HashAlgorithm algorithm, size_t* outLength) {
    if (!path || !key || !outLength) {
        if (outLength) *outLength = 0;
        return NULL;
    }
    const EVP_MD* md = crypto_evp_md(algorithm);
    if (!md) {
        CRYPTO_LOG("[crypto_hmac_file] algorithm %d unsupported", algorithm);
        *outLength = 0;
        return NULL;
    }
    FILE* f = fopen(path, "rb");
    if (!f) {
        CRYPTO_LOG("[crypto_hmac_file] cannot open %s", path);
        *outLength = 0;
        return NULL;
    }

    HMAC_CTX* ctx = HMAC_CTX_new();
    if (!ctx || !HMAC_Init_ex(ctx, key, (int)key_len, md, NULL)) {
        if (ctx) HMAC_CTX_free(ctx);
        fclose(f);
        *outLength = 0;
        return NULL;
    }

    unsigned char buf[64 * 1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        if (!HMAC_Update(ctx, buf, n)) {
            HMAC_CTX_free(ctx);
            fclose(f);
            *outLength = 0;
            return NULL;
        }
    }
    fclose(f);

    unsigned int mac_len = (unsigned int)EVP_MD_size(md);
    uint8_t* mac = (uint8_t*)malloc(mac_len);

    if (!mac) { 
        HMAC_CTX_free(ctx); 
        *outLength = 0; 
        return NULL; 
    }
    if (!HMAC_Final(ctx, mac, &mac_len)) {
        free(mac);
        HMAC_CTX_free(ctx);
        *outLength = 0;
        return NULL;
    }

    HMAC_CTX_free(ctx);
    *outLength = (size_t)mac_len;

    return mac;
}


/* ================================================================== */
/* AES, AEAD (GCM / ChaCha20-Poly1305), password-based encryption,    */
/* and secure-zero — production-grade additions. All OpenSSL EVP.      */
/* Internal helpers use the `cryp_` prefix (distinct from public       */
/* crypto_).                                                           */
/* ================================================================== */

/**
 * @brief Securely wipe `length` bytes at `ptr` (not optimized away).
 *
 * Use on key material / decrypted plaintext before freeing it. Wraps
 * OPENSSL_cleanse, which the compiler cannot elide. NULL-safe.
 *
 * @param ptr    Start of the buffer to wipe. NULL is a no-op.
 * @param length Number of bytes to wipe.
 */
void crypto_secure_zero(void* ptr, size_t length) {
    if (ptr && length) {
        OPENSSL_cleanse(ptr, length);
    }
}

/* Resolve an AES EVP_CIPHER from key length (16/24/32) and mode, or NULL. */
static const EVP_CIPHER* cryp_aes_cipher(size_t key_len, CryptoMode mode) {
    switch (key_len) {
        case 16:
            switch (mode) {
                case CRYPTO_MODE_ECB: return EVP_aes_128_ecb();
                case CRYPTO_MODE_CBC: return EVP_aes_128_cbc();
                case CRYPTO_MODE_CFB: return EVP_aes_128_cfb128();
                case CRYPTO_MODE_OFB: return EVP_aes_128_ofb();
                case CRYPTO_MODE_CTR: return EVP_aes_128_ctr();
                default:              return NULL;
            }
        case 24:
            switch (mode) {
                case CRYPTO_MODE_ECB: return EVP_aes_192_ecb();
                case CRYPTO_MODE_CBC: return EVP_aes_192_cbc();
                case CRYPTO_MODE_CFB: return EVP_aes_192_cfb128();
                case CRYPTO_MODE_OFB: return EVP_aes_192_ofb();
                case CRYPTO_MODE_CTR: return EVP_aes_192_ctr();
                default:              return NULL;
            }
        case 32:
            switch (mode) {
                case CRYPTO_MODE_ECB: return EVP_aes_256_ecb();
                case CRYPTO_MODE_CBC: return EVP_aes_256_cbc();
                case CRYPTO_MODE_CFB: return EVP_aes_256_cfb128();
                case CRYPTO_MODE_OFB: return EVP_aes_256_ofb();
                case CRYPTO_MODE_CTR: return EVP_aes_256_ctr();
                default:              return NULL;
            }
        default:
            return NULL;
    }
}

/**
 * @brief Encrypt with AES-128/192/256 in ECB/CBC/CFB/OFB/CTR mode.
 *
 * Prefer this over crypto_des_encrypt (DES is obsolete). key_len picks the
 * variant (16/24/32 bytes); CBC/ECB add PKCS#7 padding, the stream modes don't.
 * `iv` is 16 bytes (ignored for ECB). Returns a malloc'd buffer (caller frees)
 * and the length in *out_len, or NULL on error.
 *
 * @param plaintext Input bytes to encrypt (may be NULL only if `len == 0`).
 * @param len       Length of `plaintext` in bytes.
 * @param key       AES key. Must not be NULL.
 * @param key_len   Key length: 16, 24, or 32 bytes (AES-128/192/256).
 * @param iv        16-byte initialization vector (ignored for ECB mode).
 * @param mode      Cipher mode (CRYPTO_MODE_ECB/CBC/CFB/OFB/CTR).
 * @param out_len   Output: length of the returned ciphertext. Must not be NULL.
 * @return Newly-allocated ciphertext (caller frees), or NULL on invalid
 *         arguments / unsupported key length or mode / OpenSSL failure.
 */
uint8_t* crypto_aes_encrypt(const uint8_t* plaintext, size_t len, const uint8_t* key, size_t key_len,
                            const uint8_t* iv, CryptoMode mode, size_t* out_len) {
    if (!out_len) {
        return NULL;
    }
    *out_len = 0;
    if (!key || (!plaintext && len > 0)) {
        return NULL;
    }
    const EVP_CIPHER* cipher = cryp_aes_cipher(key_len, mode);
    if (!cipher) {
        CRYPTO_LOG("[crypto_aes_encrypt] unsupported key length / mode");
        return NULL;
    }
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return NULL;
    }
    uint8_t* out = (uint8_t*)malloc(len + CRYPTO_AES_BLOCK_SIZE);   /* +1 block for padding */
    if (!out) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    int ok = 0, outl = 0, finl = 0;
    if (EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv) == 1 &&
        EVP_EncryptUpdate(ctx, out, &outl, plaintext, (int)len) == 1 &&
        EVP_EncryptFinal_ex(ctx, out + outl, &finl) == 1) {
        ok = 1;
    }
    EVP_CIPHER_CTX_free(ctx);
    if (!ok) {
        free(out);
        return NULL;
    }
    *out_len = (size_t)(outl + finl);
    return out;
}

/**
 * @brief Decrypt AES produced by crypto_aes_encrypt (same key/iv/mode).
 *
 * Returns the plaintext (caller frees) and *out_len, or NULL on error — which
 * for CBC/ECB includes a PKCS#7 padding failure (wrong key/corrupt input).
 *
 * @param ciphertext Input bytes to decrypt (may be NULL only if `len == 0`).
 * @param len        Length of `ciphertext` in bytes.
 * @param key        AES key matching the one used to encrypt. Must not be NULL.
 * @param key_len    Key length: 16, 24, or 32 bytes.
 * @param iv         16-byte IV matching encryption (ignored for ECB mode).
 * @param mode       Cipher mode matching encryption.
 * @param out_len    Output: length of the returned plaintext. Must not be NULL.
 * @return Newly-allocated plaintext (caller frees), or NULL on invalid
 *         arguments / unsupported key length or mode / decryption (padding)
 *         failure.
 */
uint8_t* crypto_aes_decrypt(const uint8_t* ciphertext, size_t len, const uint8_t* key, size_t key_len,
                            const uint8_t* iv, CryptoMode mode, size_t* out_len) {
    if (!out_len) {
        return NULL;
    }
    *out_len = 0;
    if (!key || (!ciphertext && len > 0)) {
        return NULL;
    }
    const EVP_CIPHER* cipher = cryp_aes_cipher(key_len, mode);
    if (!cipher) {
        return NULL;
    }
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return NULL;
    }
    uint8_t* out = (uint8_t*)malloc(len + CRYPTO_AES_BLOCK_SIZE);
    if (!out) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    int ok = 0, outl = 0, finl = 0;
    if (EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv) == 1 &&
        EVP_DecryptUpdate(ctx, out, &outl, ciphertext, (int)len) == 1 &&
        EVP_DecryptFinal_ex(ctx, out + outl, &finl) == 1) {
        ok = 1;
    }
    EVP_CIPHER_CTX_free(ctx);
    if (!ok) {
        free(out);
        return NULL;
    }
    *out_len = (size_t)(outl + finl);
    return out;
}

/* Shared AEAD encrypt (GCM / ChaCha20-Poly1305): writes the tag to `tag`. */
static uint8_t* cryp_aead_seal(const EVP_CIPHER* cipher,
                               const uint8_t* pt, size_t len, const uint8_t* key,
                               const uint8_t* iv, size_t iv_len,
                               const uint8_t* aad, size_t aad_len,
                               uint8_t* tag, size_t tag_len, size_t* out_len) {
    *out_len = 0;
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return NULL;
    }
    uint8_t* out = (uint8_t*)malloc(len ? len : 1);
    if (!out) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    int ok = 0, outl = 0, finl = 0, tmp = 0;
    if (EVP_EncryptInit_ex(ctx, cipher, NULL, NULL, NULL) == 1 &&
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, (int)iv_len, NULL) == 1 &&
        EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv) == 1) {
        ok = 1;
        if (ok && aad && aad_len > 0 && EVP_EncryptUpdate(ctx, NULL, &tmp, aad, (int)aad_len) != 1) {
            ok = 0;
        }
        if (ok && EVP_EncryptUpdate(ctx, out, &outl, pt, (int)len) != 1) {
            ok = 0;
        }
        if (ok && EVP_EncryptFinal_ex(ctx, out + outl, &finl) != 1) {
            ok = 0;
        }
        if (ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, (int)tag_len, tag) != 1) {
            ok = 0;
        }
    }
    EVP_CIPHER_CTX_free(ctx);
    if (!ok) {
        free(out);
        return NULL;
    }
    *out_len = (size_t)(outl + finl);
    return out;
}

/* Shared AEAD decrypt: verifies `tag`; returns NULL (and wipes the partial
 * plaintext) on authentication failure. */
static uint8_t* cryp_aead_open(const EVP_CIPHER* cipher,
                               const uint8_t* ct, size_t len, const uint8_t* key,
                               const uint8_t* iv, size_t iv_len,
                               const uint8_t* aad, size_t aad_len,
                               const uint8_t* tag, size_t tag_len, size_t* out_len) {
    *out_len = 0;
    unsigned char tagbuf[64];
    if (tag_len == 0 || tag_len > sizeof tagbuf) {
        return NULL;
    }
    memcpy(tagbuf, tag, tag_len);                 /* avoid casting away const on ctrl */

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return NULL;
    }
    uint8_t* out = (uint8_t*)malloc(len ? len : 1);
    if (!out) {
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    int ok = 0, outl = 0, finl = 0, tmp = 0;
    if (EVP_DecryptInit_ex(ctx, cipher, NULL, NULL, NULL) == 1 &&
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, (int)iv_len, NULL) == 1 &&
        EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv) == 1) {
        ok = 1;
        if (ok && aad && aad_len > 0 && EVP_DecryptUpdate(ctx, NULL, &tmp, aad, (int)aad_len) != 1) {
            ok = 0;
        }
        if (ok && EVP_DecryptUpdate(ctx, out, &outl, ct, (int)len) != 1) {
            ok = 0;
        }
        if (ok && EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, (int)tag_len, tagbuf) != 1) {
            ok = 0;
        }
        if (ok && EVP_DecryptFinal_ex(ctx, out + outl, &finl) != 1) {
            ok = 0;                                /* tag mismatch -> not authentic */
        }
    }
    EVP_CIPHER_CTX_free(ctx);
    if (!ok) {
        if (len) {
            OPENSSL_cleanse(out, len);             /* don't leak unauthenticated data */
        }
        free(out);
        return NULL;
    }
    *out_len = (size_t)(outl + finl);
    return out;
}

static const EVP_CIPHER* cryp_gcm_cipher(size_t key_len) {
    switch (key_len) {
        case 16: return EVP_aes_128_gcm();
        case 24: return EVP_aes_192_gcm();
        case 32: return EVP_aes_256_gcm();
        default: return NULL;
    }
}

/**
 * @brief AES-GCM authenticated encryption. Writes a tag_len-byte tag to `tag`.
 *        See the header contract.
 *
 * @param plaintext Bytes to encrypt (may be NULL only if `len == 0`).
 * @param len       Length of `plaintext` in bytes.
 * @param key       AES key. Must not be NULL.
 * @param key_len   Key length: 16, 24, or 32 bytes (AES-128/192/256-GCM).
 * @param iv        Nonce / initialization vector. Must not be NULL.
 * @param iv_len    Length of `iv` in bytes (must be non-zero; 12 is standard).
 * @param aad       Optional additional authenticated data (may be NULL).
 * @param aad_len   Length of `aad` in bytes (0 if none).
 * @param tag       Output: receives the `tag_len`-byte authentication tag.
 * @param tag_len   Tag length in bytes (1..16; 16 recommended).
 * @param out_len   Output: length of the returned ciphertext. Must not be NULL.
 * @return Newly-allocated ciphertext (caller frees), or NULL on invalid
 *         arguments / unsupported key length / OpenSSL failure.
 */
uint8_t* crypto_aes_gcm_encrypt(const uint8_t* plaintext, size_t len, const uint8_t* key, size_t key_len,
                                const uint8_t* iv, size_t iv_len, const uint8_t* aad, size_t aad_len,
                                uint8_t* tag, size_t tag_len, size_t* out_len) {
    if (!out_len) {
        return NULL;
    }
    *out_len = 0;
    const EVP_CIPHER* c = cryp_gcm_cipher(key_len);
    if (!c || !key || !iv || iv_len == 0 || !tag || tag_len == 0 || tag_len > 16 || (!plaintext && len > 0)) {
        return NULL;
    }
    return cryp_aead_seal(c, plaintext, len, key, iv, iv_len, aad, aad_len, tag, tag_len, out_len);
}

/**
 * @brief AES-GCM authenticated decryption. Returns NULL if the tag does not
 *        verify (tampering / wrong key / wrong AAD). See the header contract.
 *
 * @param ciphertext Bytes to decrypt (may be NULL only if `len == 0`).
 * @param len        Length of `ciphertext` in bytes.
 * @param key        AES key matching encryption. Must not be NULL.
 * @param key_len    Key length: 16, 24, or 32 bytes.
 * @param iv         Nonce / IV matching encryption. Must not be NULL.
 * @param iv_len     Length of `iv` in bytes (must be non-zero).
 * @param aad        Additional authenticated data matching encryption (may be NULL).
 * @param aad_len    Length of `aad` in bytes (0 if none).
 * @param tag        Input: the `tag_len`-byte authentication tag to verify.
 * @param tag_len    Tag length in bytes (must be non-zero).
 * @param out_len    Output: length of the returned plaintext. Must not be NULL.
 * @return Newly-allocated plaintext (caller frees), or NULL on invalid
 *         arguments or if the tag fails to verify (tampering / wrong key / AAD).
 */
uint8_t* crypto_aes_gcm_decrypt(const uint8_t* ciphertext, size_t len, const uint8_t* key, size_t key_len,
                                const uint8_t* iv, size_t iv_len, const uint8_t* aad, size_t aad_len,
                                const uint8_t* tag, size_t tag_len, size_t* out_len) {
    if (!out_len) {
        return NULL;
    }
    *out_len = 0;
    const EVP_CIPHER* c = cryp_gcm_cipher(key_len);
    if (!c || !key || !iv || iv_len == 0 || !tag || tag_len == 0 || (!ciphertext && len > 0)) {
        return NULL;
    }
    return cryp_aead_open(c, ciphertext, len, key, iv, iv_len, aad, aad_len, tag, tag_len, out_len);
}

/**
 * @brief ChaCha20-Poly1305 AEAD encryption (RFC 8439). key=32, iv=12, tag=16.
 *
 * @param plaintext Bytes to encrypt (may be NULL only if `len == 0`).
 * @param len       Length of `plaintext` in bytes.
 * @param key       32-byte key. Must not be NULL.
 * @param key_len   Key length in bytes; must be exactly 32.
 * @param iv        Nonce. Must not be NULL (12 bytes per RFC 8439).
 * @param iv_len    Length of `iv` in bytes (must be non-zero).
 * @param aad       Optional additional authenticated data (may be NULL).
 * @param aad_len   Length of `aad` in bytes (0 if none).
 * @param tag       Output: receives the `tag_len`-byte authentication tag.
 * @param tag_len   Tag length in bytes (1..16; 16 recommended).
 * @param out_len   Output: length of the returned ciphertext. Must not be NULL.
 * @return Newly-allocated ciphertext (caller frees), or NULL on invalid
 *         arguments (e.g. `key_len != 32`) / OpenSSL failure.
 */
uint8_t* crypto_chacha20_poly1305_encrypt(const uint8_t* plaintext, size_t len, const uint8_t* key, size_t key_len,
                                          const uint8_t* iv, size_t iv_len, const uint8_t* aad, size_t aad_len,
                                          uint8_t* tag, size_t tag_len, size_t* out_len) {
    if (!out_len) {
        return NULL;
    }
    *out_len = 0;
    if (key_len != 32 || !key || !iv || iv_len == 0 || !tag || tag_len == 0 || tag_len > 16 || (!plaintext && len > 0)) {
        return NULL;
    }
    return cryp_aead_seal(EVP_chacha20_poly1305(), plaintext, len, key, iv, iv_len, aad, aad_len, tag, tag_len, out_len);
}

/**
 * @brief ChaCha20-Poly1305 AEAD decryption. Returns NULL on auth failure.
 *
 * @param ciphertext Bytes to decrypt (may be NULL only if `len == 0`).
 * @param len        Length of `ciphertext` in bytes.
 * @param key        32-byte key matching encryption. Must not be NULL.
 * @param key_len    Key length in bytes; must be exactly 32.
 * @param iv         Nonce matching encryption. Must not be NULL.
 * @param iv_len     Length of `iv` in bytes (must be non-zero).
 * @param aad        Additional authenticated data matching encryption (may be NULL).
 * @param aad_len    Length of `aad` in bytes (0 if none).
 * @param tag        Input: the `tag_len`-byte authentication tag to verify.
 * @param tag_len    Tag length in bytes (must be non-zero).
 * @param out_len    Output: length of the returned plaintext. Must not be NULL.
 * @return Newly-allocated plaintext (caller frees), or NULL on invalid
 *         arguments or if the tag fails to verify (tampering / wrong key / AAD).
 */
uint8_t* crypto_chacha20_poly1305_decrypt(const uint8_t* ciphertext, size_t len, const uint8_t* key, size_t key_len,
                                          const uint8_t* iv, size_t iv_len, const uint8_t* aad, size_t aad_len,
                                          const uint8_t* tag, size_t tag_len, size_t* out_len) {
    if (!out_len) {
        return NULL;
    }
    *out_len = 0;
    if (key_len != 32 || !key || !iv || iv_len == 0 || !tag || tag_len == 0 || (!ciphertext && len > 0)) {
        return NULL;
    }
    return cryp_aead_open(EVP_chacha20_poly1305(), ciphertext, len, key, iv, iv_len, aad, aad_len, tag, tag_len, out_len);
}

/* Self-describing password-blob layout:
 *   "C1PE"(4) | iterations(4, big-endian) | salt(16) | nonce(12) | tag(16) | ciphertext
 */
#define CRYP_PWE_HDR        (4 + 4 + 16 + 12 + 16)
#define CRYP_PWE_DEF_ITERS  200000
#define CRYP_PWE_MAX_ITERS  10000000   /* cap to bound work on a hostile blob */

/**
 * @brief Encrypt with a password: PBKDF2-HMAC-SHA256 -> AES-256-GCM, returning a
 *        single self-describing blob. See the header contract.
 *
 * The returned blob embeds the magic, iteration count, random salt, nonce, and
 * GCM tag, so `crypto_password_decrypt` needs only the blob and the password.
 *
 * @param plaintext  Bytes to encrypt (may be NULL only if `len == 0`).
 * @param len        Length of `plaintext` in bytes.
 * @param password   NUL-terminated password. Must not be NULL.
 * @param iterations PBKDF2 iteration count; values <= 0 use a secure default.
 * @param out_len    Output: length of the returned blob. Must not be NULL.
 * @return Newly-allocated self-describing blob (caller frees), or NULL on
 *         invalid arguments / key-derivation / OpenSSL failure.
 */
uint8_t* crypto_password_encrypt(const uint8_t* plaintext, size_t len, const char* password,
                                 int iterations, size_t* out_len) {
    if (!out_len) {
        return NULL;
    }
    *out_len = 0;
    if (!password || (!plaintext && len > 0)) {
        return NULL;
    }
    if (iterations <= 0) {
        iterations = CRYP_PWE_DEF_ITERS;
    }

    uint8_t salt[16], iv[12], tag[16];
    crypto_random_bytes(salt, sizeof salt);
    crypto_random_bytes(iv, sizeof iv);

    uint8_t* dk = crypto_pbkdf2(password, strlen(password), salt, sizeof salt, iterations, 32, CRYPTO_SHA256);
    if (!dk) {
        return NULL;
    }

    size_t   ct_len = 0;
    uint8_t* ct = crypto_aes_gcm_encrypt(plaintext, len, dk, 32, iv, sizeof iv, NULL, 0, tag, sizeof tag, &ct_len);
    crypto_secure_zero(dk, 32);
    free(dk);
    if (!ct) {
        return NULL;
    }

    uint8_t* blob = (uint8_t*)malloc(CRYP_PWE_HDR + ct_len);
    if (!blob) {
        free(ct);
        return NULL;
    }
    size_t o = 0;
    memcpy(blob + o, "C1PE", 4); o += 4;
    blob[o++] = (uint8_t)((unsigned)iterations >> 24);
    blob[o++] = (uint8_t)((unsigned)iterations >> 16);
    blob[o++] = (uint8_t)((unsigned)iterations >> 8);
    blob[o++] = (uint8_t)((unsigned)iterations);
    memcpy(blob + o, salt, 16); o += 16;
    memcpy(blob + o, iv, 12);   o += 12;
    memcpy(blob + o, tag, 16);  o += 16;
    memcpy(blob + o, ct, ct_len); o += ct_len;

    free(ct);
    *out_len = o;
    return blob;
}

/**
 * @brief Reverse crypto_password_encrypt. Returns NULL on a wrong password, a
 *        bad magic/format, or any tampering. See the header contract.
 *
 * @param blob     The self-describing blob produced by crypto_password_encrypt.
 * @param blob_len Length of `blob` in bytes.
 * @param password NUL-terminated password. Must not be NULL.
 * @param out_len  Output: length of the recovered plaintext. Must not be NULL.
 * @return Newly-allocated plaintext (caller frees), or NULL on a wrong password,
 *         malformed / truncated blob, unsupported iteration count, or a failed
 *         GCM tag check (tampering).
 */
uint8_t* crypto_password_decrypt(const uint8_t* blob, size_t blob_len, const char* password, size_t* out_len) {
    if (!out_len) {
        return NULL;
    }
    *out_len = 0;
    if (!blob || !password || blob_len < CRYP_PWE_HDR || memcmp(blob, "C1PE", 4) != 0) {
        return NULL;
    }
    unsigned itu = ((unsigned)blob[4] << 24) | ((unsigned)blob[5] << 16) |
                   ((unsigned)blob[6] << 8)  |  (unsigned)blob[7];
    if (itu == 0 || itu > CRYP_PWE_MAX_ITERS) {
        return NULL;
    }
    const uint8_t* salt = blob + 8;
    const uint8_t* iv   = blob + 24;
    const uint8_t* tag  = blob + 36;
    const uint8_t* ct   = blob + CRYP_PWE_HDR;
    size_t         ctl  = blob_len - CRYP_PWE_HDR;

    uint8_t* dk = crypto_pbkdf2(password, strlen(password), salt, 16, (int)itu, 32, CRYPTO_SHA256);
    if (!dk) {
        return NULL;
    }
    uint8_t* pt = crypto_aes_gcm_decrypt(ct, ctl, dk, 32, iv, 12, NULL, 0, tag, 16, out_len);
    crypto_secure_zero(dk, 32);
    free(dk);
    return pt;   /* NULL if the password is wrong (tag mismatch) */
}
