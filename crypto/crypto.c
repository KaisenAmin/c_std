/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Crypto
*/


#include <limits.h>
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

/* MD4, MD5, MDC2, DES and the HMAC_CTX API are deprecated since OpenSSL 3.0,
   but this module offers them on purpose (legacy formats, interop). Silence
   that one diagnostic here, for every compiler and build system. */
#if defined(_MSC_VER)
#pragma warning(disable: 4996)
#elif defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#if defined(_WIN32)
#include <windows.h>
#include <wchar.h>
#endif

/* fopen for a UTF-8 path. Windows' narrow fopen reads the path in the ANSI
   code page, so a non-ASCII UTF-8 name only opened when the system code
   page happened to be UTF-8; convert it and use _wfopen instead. */
static FILE* cryp_fopen_utf8(const char* path, const char* mode) {
#if defined(_WIN32)
    wchar_t wmode[8];
    size_t i = 0;
    for (; mode[i] && i < 7; ++i) {
        wmode[i] = (wchar_t)(unsigned char)mode[i];
    }
    wmode[i] = L'\0';
    int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, NULL, 0);
    if (n <= 0) {
        return NULL;
    }
    wchar_t* wpath = (wchar_t*)malloc((size_t)n * sizeof(wchar_t));
    if (!wpath) {
        return NULL;
    }
    FILE* f = NULL;
    if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wpath, n) > 0) {
        f = _wfopen(wpath, wmode);
    }
    free(wpath);
    return f;
#else
    return fopen(path, mode);
#endif
}

/* EVP_*Update take an int length: larger buffers go through in pieces (the
   (int) casts truncated anything of 2 GiB or more). `out` may be NULL for
   AAD. *written receives the total output. */
static int cryp_evp_update(EVP_CIPHER_CTX* ctx, int encrypt, uint8_t* out, size_t* written,
                           const uint8_t* in, size_t len) {
    size_t total = 0;
    while (len > 0) {
        int chunk = (len > (size_t)(INT_MAX / 2)) ? INT_MAX / 2 : (int)len;
        int w = 0;
        int rc = encrypt ? EVP_EncryptUpdate(ctx, out ? out + total : NULL, &w, in, chunk)
                         : EVP_DecryptUpdate(ctx, out ? out + total : NULL, &w, in, chunk);
        if (rc != 1) {
            return 0;
        }
        total += (size_t)w;
        in += chunk;
        len -= (size_t)chunk;
    }
    *written = total;
    return 1;
}

#ifndef DES_BLOCK_SIZE
#define DES_BLOCK_SIZE 8
#endif
#define CRYP_AEAD_MIN_TAG 12   /* shortest accepted AEAD tag (NIST SP 800-38D) */

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
            /* EVP_DigestFinal_ex writes an unsigned int: casting the size_t*
               broke strict aliasing and wrote the wrong half on big-endian */
            unsigned int sha3_len = 0;
            if (!mdctx || !EVP_DigestInit_ex(mdctx, md_type, NULL) ||
                !EVP_DigestUpdate(mdctx, data, length) ||
                !EVP_DigestFinal_ex(mdctx, md, &sha3_len)) {
                CRYPTO_LOG("[crypto_hash_data] Failed to compute SHA3 hash");
                free(md);
                EVP_MD_CTX_free(mdctx);
                *outLength = 0;
                return NULL;
            }
            *outLength = sha3_len;

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
            unsigned int md_len = 0;
            if (!mdctx || !EVP_DigestInit_ex(mdctx, md_type, NULL) ||
                !EVP_DigestUpdate(mdctx, data, length) ||
                !EVP_DigestFinal_ex(mdctx, md, &md_len)) {
                CRYPTO_LOG("[crypto_hash_data] Failed to compute hash");
                free(md);
                EVP_MD_CTX_free(mdctx);
                *outLength = 0;
                return NULL;
            }
            *outLength = md_len;

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
 * ECB and CBC pad the plaintext with PKCS#7 (1 to 8 bytes, each holding the pad length), so
 * crypto_des_decrypt gives back exactly the original bytes. CFB and OFB are stream modes: the
 * ciphertext has the plaintext's length. (Zero padding was used before, so decryption could not
 * know the original length and lost trailing zero bytes.)
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

    bool blockMode = (mode == CRYPTO_MODE_ECB || mode == CRYPTO_MODE_CBC);
    if (!blockMode && mode != CRYPTO_MODE_CFB && mode != CRYPTO_MODE_OFB) {
        CRYPTO_LOG("[crypto_des_encrypt] Unknown mode");
        return NULL;
    }
    if (blockMode && len > SIZE_MAX - DES_BLOCK_SIZE) {
        return NULL;
    }
    /* PKCS#7: always 1..8 bytes of padding in block modes */
    size_t padded_len = blockMode ? (len / DES_BLOCK_SIZE + 1) * DES_BLOCK_SIZE : len;
    *out_len = padded_len;

    CRYPTO_LOG("[crypto_des_encrypt] Allocating memory for padded plaintext (size: %zu)", padded_len);
    uint8_t* padded_plaintext = (uint8_t*) calloc(padded_len ? padded_len : 1, sizeof(uint8_t));
    if (!padded_plaintext) {
        CRYPTO_LOG("[crypto_des_encrypt] Memory allocation failed for padded plaintext");
        return NULL;
    }
    if (len) {
        memcpy(padded_plaintext, plaintext, len);
    }
    if (blockMode) {
        memset(padded_plaintext + len, (int)(padded_len - len), padded_len - len);
    }

    CRYPTO_LOG("[crypto_des_encrypt] Allocating memory for ciphertext (size: %zu)", padded_len);
    uint8_t* ciphertext = (uint8_t*) malloc(padded_len ? padded_len : 1);
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
 * In ECB and CBC the ciphertext length must be a multiple of 8 and the PKCS#7 padding added by
 * crypto_des_encrypt is checked and removed (NULL if it is malformed, e.g. a wrong key). The
 * returned buffer is NUL-terminated (the terminator is not counted in *out_len).
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
    /* ECB and CBC work on whole 8-byte blocks: a ciphertext whose length is
       not a multiple of 8 was read and written a full block past the end of
       the len-byte buffers. Such input cannot be valid DES output. */
    if ((mode == CRYPTO_MODE_ECB || mode == CRYPTO_MODE_CBC) && (len == 0 || len % DES_BLOCK_SIZE != 0)) {
        CRYPTO_LOG("[crypto_des_decrypt] Invalid ciphertext length %zu for a block mode", len);
        return NULL;
    }
    if (len == 0) {
        CRYPTO_LOG("[crypto_des_decrypt] Empty ciphertext");
        return NULL;
    }
    if (mode != CRYPTO_MODE_ECB && mode != CRYPTO_MODE_CBC && mode != CRYPTO_MODE_CFB && mode != CRYPTO_MODE_OFB) {
        CRYPTO_LOG("[crypto_des_decrypt] Unknown mode");
        return NULL;
    }
    *out_len = len;
    uint8_t* plaintext = (uint8_t*) malloc(len + 1);
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
    if (mode == CRYPTO_MODE_ECB || mode == CRYPTO_MODE_CBC) {
        uint8_t pad = plaintext[len - 1];
        bool padOk = pad >= 1 && pad <= DES_BLOCK_SIZE;
        for (size_t p = 0; padOk && p < pad; ++p) {
            padOk = plaintext[len - 1 - p] == pad;
        }
        if (!padOk) {
            CRYPTO_LOG("[crypto_des_decrypt] Invalid PKCS#7 padding (wrong key or corrupted data)");
            free(plaintext);
            *out_len = 0;
            return NULL;
        }
        *out_len = len - pad;
    }
    plaintext[*out_len] = '\0';

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

bool crypto_generate_random_iv(uint8_t *iv, size_t length) {
    CRYPTO_LOG("[crypto_generate_random_iv] enter (iv=%p, length=%zu)", (void*)iv, length);
    if (!iv || length == 0 || length > (size_t)0xFFFFFFFFu) {
        CRYPTO_LOG("[crypto_generate_random_iv] invalid args: iv=%p, length=%zu", (void*)iv, length);
        return false;
    }

    HCRYPTPROV hCryptProv;

    CRYPTO_LOG("[crypto_generate_random_iv] Acquiring cryptographic context");
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        // Project convention: a library never aborts the process. On failure,
        // fall back to a deterministic zero IV and let the caller proceed.
        CRYPTO_LOG("[crypto_generate_random_iv] CryptAcquireContext failed: %lu — zero IV", GetLastError());
        memset(iv, 0, length);
        return false;
    }

    CRYPTO_LOG("[crypto_generate_random_iv] Generating random bytes");
    if (!CryptGenRandom(hCryptProv, (DWORD)length, iv)) {
        CRYPTO_LOG("[crypto_generate_random_iv] CryptGenRandom failed: %lu — zero IV", GetLastError());
        memset(iv, 0, length);
        CryptReleaseContext(hCryptProv, 0);
        return false;
    }

    CRYPTO_LOG("[crypto_generate_random_iv] Releasing cryptographic context");
    CryptReleaseContext(hCryptProv, 0);
    CRYPTO_LOG("[crypto_generate_random_iv] exit -> success (length=%zu)", length);
    return true;
}

#else

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

bool crypto_generate_random_iv(uint8_t *iv, size_t length) {
    CRYPTO_LOG("[crypto_generate_random_iv] enter (iv=%p, length=%zu)", (void*)iv, length);
    if (!iv || length == 0) {
        CRYPTO_LOG("[crypto_generate_random_iv] invalid args: iv=%p, length=%zu", (void*)iv, length);
        return false;
    }

    CRYPTO_LOG("[crypto_generate_random_iv] Opening /dev/urandom");
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        // Report the failure (zero IV) — never abort the process.
        CRYPTO_LOG("[crypto_generate_random_iv] Failed to open /dev/urandom — zero IV");
        memset(iv, 0, length);
        return false;
    }

    CRYPTO_LOG("[crypto_generate_random_iv] Reading random data");
    /* read() may return fewer bytes than asked or be interrupted: loop. */
    size_t got = 0;
    while (got < length) {
        ssize_t r = read(fd, iv + got, length - got);
        if (r > 0) {
            got += (size_t)r;
        }
        else if (r < 0 && errno == EINTR) {
            continue;
        }
        else {
            break;
        }
    }
    close(fd);
    if (got != length) {
        CRYPTO_LOG("[crypto_generate_random_iv] Failed to read from /dev/urandom — zero IV");
        memset(iv, 0, length);
        return false;
    }
    CRYPTO_LOG("[crypto_generate_random_iv] exit -> success (length=%zu)", length);
    return true;
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
    /* EVP_EncodeBlock writes 4 chars per 3 input bytes + NUL, and takes an
       int length: refuse what does not fit (the int size used to overflow). */
    if (length > (size_t)INT_MAX / 4 * 3) {
        return NULL;
    }
    size_t b64_len = 4 * ((length + 2) / 3) + 1;
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
    FILE* f = cryp_fopen_utf8(path, "rb");
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
    if (ferror(f)) {
        /* a read error is not EOF: the digest would cover only a prefix */
        EVP_MD_CTX_free(ctx);
        fclose(f);
        *outLength = 0;
        return NULL;
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

    if (key_len > (size_t)INT_MAX || !HMAC(md, key, (int)key_len, data, data_len, mac, &mac_len)) {
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
 * zero-filled and false is returned — the library never
 * aborts the process. The function returns false in that case: treat the
 * buffer as unusable (never use it as a key, salt or nonce).
 *
 * @param buffer  Destination buffer.
 * @param length  Number of bytes to fill.
 *
 * @code
 * uint8_t key[32];
 * crypto_random_bytes(key, sizeof(key));
 * @endcode
 */
bool crypto_random_bytes(uint8_t* buffer, size_t length) {
    if (!buffer) {
        return false;
    }
    /* RAND_bytes takes an int: fill large buffers in chunks instead of
       truncating the length. */
    size_t done = 0;
    while (done < length) {
        size_t chunk = length - done;
        if (chunk > (size_t)INT_MAX) {
            chunk = (size_t)INT_MAX;
        }
        if (RAND_bytes(buffer + done, (int)chunk) != 1) {
            /* Report the failure: silently handing back zeros made keys, salts
               and nonces predictable while the caller believed they were random. */
            CRYPTO_LOG("[crypto_random_bytes] RAND_bytes failed — buffer zeroed, returning false");
            memset(buffer, 0, length);
            return false;
        }
        done += chunk;
    }
    return true;
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

    int decoded = (b64_len <= (size_t)INT_MAX) ? EVP_DecodeBlock(out, (const unsigned char*)b64, (int)b64_len) : -1;
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
    if (password_len > (size_t)INT_MAX || salt_len > (size_t)INT_MAX || key_len > (size_t)INT_MAX) {
        return NULL;   /* PKCS5_PBKDF2_HMAC takes int lengths */
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
    FILE* f = cryp_fopen_utf8(path, "rb");
    if (!f) {
        CRYPTO_LOG("[crypto_hmac_file] cannot open %s", path);
        *outLength = 0;
        return NULL;
    }

    HMAC_CTX* ctx = (key_len <= (size_t)INT_MAX) ? HMAC_CTX_new() : NULL;   /* HMAC takes an int key length */
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
    if (ferror(f)) {
        HMAC_CTX_free(ctx);
        fclose(f);
        *outLength = 0;
        return NULL;
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
    if (!key || (!plaintext && len > 0) || len > SIZE_MAX - CRYPTO_AES_BLOCK_SIZE) {
        return NULL;
    }
    if (!iv && mode != CRYPTO_MODE_ECB) {
        /* OpenSSL would use an all-zero IV: identical plaintexts would give
           identical ciphertexts, and CTR/OFB would reuse the keystream */
        CRYPTO_LOG("[crypto_aes_encrypt] an IV is required for this mode");
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
    int ok = 0, finl = 0;
    size_t outl = 0;
    if (EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv) == 1 &&
        cryp_evp_update(ctx, 1, out, &outl, plaintext, len) &&
        EVP_EncryptFinal_ex(ctx, out + outl, &finl) == 1) {
        ok = 1;
    }
    EVP_CIPHER_CTX_free(ctx);
    if (!ok) {
        free(out);
        return NULL;
    }
    *out_len = outl + (size_t)finl;
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
    if (!key || (!ciphertext && len > 0) || len > SIZE_MAX - CRYPTO_AES_BLOCK_SIZE) {
        return NULL;
    }
    if (!iv && mode != CRYPTO_MODE_ECB) {
        return NULL;   /* as in crypto_aes_encrypt */
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
    int ok = 0, finl = 0;
    size_t outl = 0;
    if (EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv) == 1 &&
        cryp_evp_update(ctx, 0, out, &outl, ciphertext, len) &&
        EVP_DecryptFinal_ex(ctx, out + outl, &finl) == 1) {
        ok = 1;
    }
    EVP_CIPHER_CTX_free(ctx);
    if (!ok) {
        free(out);
        return NULL;
    }
    *out_len = outl + (size_t)finl;
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
    int ok = 0, finl = 0;
    size_t outl = 0, tmp = 0;
    if (iv_len <= 64 && tag_len <= 64 &&
        EVP_EncryptInit_ex(ctx, cipher, NULL, NULL, NULL) == 1 &&
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, (int)iv_len, NULL) == 1 &&
        EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv) == 1) {
        ok = 1;
        if (ok && aad && aad_len > 0 && !cryp_evp_update(ctx, 1, NULL, &tmp, aad, aad_len)) {
            ok = 0;
        }
        if (ok && !cryp_evp_update(ctx, 1, out, &outl, pt, len)) {
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
    *out_len = outl + (size_t)finl;
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
    /* OpenSSL compares only tag_len bytes, so a 1-byte "tag" would be an
       8-bit authenticator. Accept 12..16 bytes (NIST SP 800-38D). */
    if (tag_len < CRYP_AEAD_MIN_TAG || tag_len > 16) {
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
    int ok = 0, finl = 0;
    size_t outl = 0, tmp = 0;
    if (iv_len <= 64 &&
        EVP_DecryptInit_ex(ctx, cipher, NULL, NULL, NULL) == 1 &&
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, (int)iv_len, NULL) == 1 &&
        EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv) == 1) {
        ok = 1;
        if (ok && aad && aad_len > 0 && !cryp_evp_update(ctx, 0, NULL, &tmp, aad, aad_len)) {
            ok = 0;
        }
        if (ok && !cryp_evp_update(ctx, 0, out, &outl, ct, len)) {
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
    *out_len = outl + (size_t)finl;
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
 * @param tag_len   Tag length in bytes (12..16; 16 recommended).
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
    if (!c || !key || !iv || iv_len == 0 || !tag || tag_len < CRYP_AEAD_MIN_TAG || tag_len > 16 || (!plaintext && len > 0)) {
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
 * @param tag_len    Tag length in bytes (12..16; shorter tags are refused).
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
 * @param tag_len   Tag length in bytes (12..16; 16 recommended).
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
    if (key_len != 32 || !key || !iv || iv_len == 0 || !tag || tag_len < CRYP_AEAD_MIN_TAG || tag_len > 16 || (!plaintext && len > 0)) {
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
 * @param tag_len    Tag length in bytes (12..16; shorter tags are refused).
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
    if (iterations > CRYP_PWE_MAX_ITERS) {
        /* crypto_password_decrypt refuses such blobs (to bound the work a
           hostile blob can demand), so the data could never be decrypted */
        return NULL;
    }

    uint8_t salt[16], iv[12], tag[16];
    if (!crypto_random_bytes(salt, sizeof salt) || !crypto_random_bytes(iv, sizeof iv)) {
        CRYPTO_LOG("[crypto_password_encrypt] CSPRNG failure — refusing to encrypt with a predictable salt/nonce");
        return NULL;
    }

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


/* ================================================================== */
/* Non-cryptographic hashes and checksums                             */
/* ================================================================== */
/*
 * Fast, portable C (no OpenSSL), for hash tables, checksums, sharding,
 * deduplication and file integrity checks - NOT for security: an attacker
 * can construct collisions for all of them. The results are identical on
 * every platform: inputs are read as little-endian words whatever the CPU,
 * and unaligned input is fine. Build with -DCRYPTO_HASH_PORTABLE to use only
 * the plain-C paths (no SSE4.2 CRC instruction, no SSE2 / AVX2 XXH3 loop, no
 * 128-bit multiply).
 *
 * FNV-1a, MurmurHash3 and xxHash are implemented as static inline
 * functions at the end of crypto.h (dependency-free, shared with hashmap);
 * the functions below wrap them. The CRCs are implemented here.
 */

#if !defined(CRYPTO_HASH_PORTABLE) && (defined(__SSE4_2__) || (defined(_MSC_VER) && defined(__AVX__))) && \
    (defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86))
#  define CRYP_HAVE_SSE42_CRC 1
#  include <nmmintrin.h>
#endif

/* ------------------------------------------------------------------ */
/* FNV-1a                                                             */
/* ------------------------------------------------------------------ */

/**
 * @brief Continues an FNV-1a 32-bit hash over `length` more bytes.
 *
 * Start from CRYPTO_FNV1A_32_INIT; feeding the input in pieces gives the
 * same result as hashing it at once. FNV-1a is tiny and good for short keys
 * (identifiers, small strings); for long input use XXH3 / XXH64.
 *
 * @param hash   The hash so far (CRYPTO_FNV1A_32_INIT to start).
 * @param data   The bytes (may be NULL when length is 0).
 * @param length Number of bytes.
 * @return The updated hash.
 */
uint32_t crypto_fnv1a_32_update(uint32_t hash, const void* data, size_t length) {
    return cryp_fnv1a_32_update(hash, data, length);
}

/** @brief FNV-1a, 32-bit, of `length` bytes. */
uint32_t crypto_fnv1a_32(const void* data, size_t length) {
    return cryp_fnv1a_32_update(CRYPTO_FNV1A_32_INIT, data, length);
}

/** @brief Continues an FNV-1a 64-bit hash (start from CRYPTO_FNV1A_64_INIT). */
uint64_t crypto_fnv1a_64_update(uint64_t hash, const void* data, size_t length) {
    return cryp_fnv1a_64_update(hash, data, length);
}

/** @brief FNV-1a, 64-bit, of `length` bytes. */
uint64_t crypto_fnv1a_64(const void* data, size_t length) {
    return cryp_fnv1a_64_update(CRYPTO_FNV1A_64_INIT, data, length);
}


/* ------------------------------------------------------------------ */
/* MurmurHash3 (Austin Appleby)                                       */
/* ------------------------------------------------------------------ */

/**
 * @brief MurmurHash3_x86_32: 32-bit hash with a seed. The same values as the
 *        reference implementation and Python's mmh3.hash (as unsigned).
 *
 * @param data   The bytes (may be NULL when length is 0).
 * @param length Number of bytes (the algorithm mixes in length mod 2^32).
 * @param seed   Seed; different seeds give independent hash functions.
 */
uint32_t crypto_murmur3_32(const void* data, size_t length, uint32_t seed) {
    return cryp_murmur3_32(data, length, seed);
}

/**
 * @brief MurmurHash3_x64_128: 128-bit hash with a seed, written to out[0]
 *        (low 64 bits) and out[1] (high 64 bits) - Python's mmh3.hash64
 *        returns the same two halves (as signed), mmh3.hash128 their
 *        combination out[1] << 64 | out[0]. Does nothing if `out` is NULL.
 */
void crypto_murmur3_128(const void* data, size_t length, uint32_t seed, uint64_t out[2]) {
    cryp_murmur3_128(data, length, seed, out);
}


/* ------------------------------------------------------------------ */
/* xxHash (Yann Collet): XXH32, XXH64, XXH3-64                        */
/* ------------------------------------------------------------------ */

/**
 * @brief XXH32: 32-bit xxHash with a seed (identical to the reference
 *        XXH32()). Fast on 32-bit CPUs; on 64-bit ones prefer XXH3 / XXH64.
 */
uint32_t crypto_xxh32(const void* data, size_t length, uint32_t seed) {
    return cryp_xxh32(data, length, seed);
}

/**
 * @brief XXH64: 64-bit xxHash with a seed (identical to the reference XXH64()).
 */
uint64_t crypto_xxh64(const void* data, size_t length, uint64_t seed) {
    return cryp_xxh64(data, length, seed);
}

/**
 * @brief XXH3, 64-bit, with a seed (identical to the reference
 *        XXH3_64bits_withSeed(); seed 0 equals XXH3_64bits()). The fastest
 *        hash here for short keys - a few nanoseconds - and over 10 GB/s on
 *        long input (SSE2; more with AVX2).
 */
uint64_t crypto_xxh3_64(const void* data, size_t length, uint64_t seed) {
    return cryp_xxh3_64(data, length, seed);
}


/* ------------------------------------------------------------------ */
/* CRC-32 and CRC-32C                                                 */
/* ------------------------------------------------------------------ */

/* CRC-32 (IEEE 802.3, zlib, PNG, gzip): slicing-by-8 tables, reflected polynomial 0xEDB88320 (generated). */
static const uint32_t cryp_crc32_table[8][256] = {
    {
        0x00000000u, 0x77073096u, 0xEE0E612Cu, 0x990951BAu, 0x076DC419u, 0x706AF48Fu, 0xE963A535u, 0x9E6495A3u,
        0x0EDB8832u, 0x79DCB8A4u, 0xE0D5E91Eu, 0x97D2D988u, 0x09B64C2Bu, 0x7EB17CBDu, 0xE7B82D07u, 0x90BF1D91u,
        0x1DB71064u, 0x6AB020F2u, 0xF3B97148u, 0x84BE41DEu, 0x1ADAD47Du, 0x6DDDE4EBu, 0xF4D4B551u, 0x83D385C7u,
        0x136C9856u, 0x646BA8C0u, 0xFD62F97Au, 0x8A65C9ECu, 0x14015C4Fu, 0x63066CD9u, 0xFA0F3D63u, 0x8D080DF5u,
        0x3B6E20C8u, 0x4C69105Eu, 0xD56041E4u, 0xA2677172u, 0x3C03E4D1u, 0x4B04D447u, 0xD20D85FDu, 0xA50AB56Bu,
        0x35B5A8FAu, 0x42B2986Cu, 0xDBBBC9D6u, 0xACBCF940u, 0x32D86CE3u, 0x45DF5C75u, 0xDCD60DCFu, 0xABD13D59u,
        0x26D930ACu, 0x51DE003Au, 0xC8D75180u, 0xBFD06116u, 0x21B4F4B5u, 0x56B3C423u, 0xCFBA9599u, 0xB8BDA50Fu,
        0x2802B89Eu, 0x5F058808u, 0xC60CD9B2u, 0xB10BE924u, 0x2F6F7C87u, 0x58684C11u, 0xC1611DABu, 0xB6662D3Du,
        0x76DC4190u, 0x01DB7106u, 0x98D220BCu, 0xEFD5102Au, 0x71B18589u, 0x06B6B51Fu, 0x9FBFE4A5u, 0xE8B8D433u,
        0x7807C9A2u, 0x0F00F934u, 0x9609A88Eu, 0xE10E9818u, 0x7F6A0DBBu, 0x086D3D2Du, 0x91646C97u, 0xE6635C01u,
        0x6B6B51F4u, 0x1C6C6162u, 0x856530D8u, 0xF262004Eu, 0x6C0695EDu, 0x1B01A57Bu, 0x8208F4C1u, 0xF50FC457u,
        0x65B0D9C6u, 0x12B7E950u, 0x8BBEB8EAu, 0xFCB9887Cu, 0x62DD1DDFu, 0x15DA2D49u, 0x8CD37CF3u, 0xFBD44C65u,
        0x4DB26158u, 0x3AB551CEu, 0xA3BC0074u, 0xD4BB30E2u, 0x4ADFA541u, 0x3DD895D7u, 0xA4D1C46Du, 0xD3D6F4FBu,
        0x4369E96Au, 0x346ED9FCu, 0xAD678846u, 0xDA60B8D0u, 0x44042D73u, 0x33031DE5u, 0xAA0A4C5Fu, 0xDD0D7CC9u,
        0x5005713Cu, 0x270241AAu, 0xBE0B1010u, 0xC90C2086u, 0x5768B525u, 0x206F85B3u, 0xB966D409u, 0xCE61E49Fu,
        0x5EDEF90Eu, 0x29D9C998u, 0xB0D09822u, 0xC7D7A8B4u, 0x59B33D17u, 0x2EB40D81u, 0xB7BD5C3Bu, 0xC0BA6CADu,
        0xEDB88320u, 0x9ABFB3B6u, 0x03B6E20Cu, 0x74B1D29Au, 0xEAD54739u, 0x9DD277AFu, 0x04DB2615u, 0x73DC1683u,
        0xE3630B12u, 0x94643B84u, 0x0D6D6A3Eu, 0x7A6A5AA8u, 0xE40ECF0Bu, 0x9309FF9Du, 0x0A00AE27u, 0x7D079EB1u,
        0xF00F9344u, 0x8708A3D2u, 0x1E01F268u, 0x6906C2FEu, 0xF762575Du, 0x806567CBu, 0x196C3671u, 0x6E6B06E7u,
        0xFED41B76u, 0x89D32BE0u, 0x10DA7A5Au, 0x67DD4ACCu, 0xF9B9DF6Fu, 0x8EBEEFF9u, 0x17B7BE43u, 0x60B08ED5u,
        0xD6D6A3E8u, 0xA1D1937Eu, 0x38D8C2C4u, 0x4FDFF252u, 0xD1BB67F1u, 0xA6BC5767u, 0x3FB506DDu, 0x48B2364Bu,
        0xD80D2BDAu, 0xAF0A1B4Cu, 0x36034AF6u, 0x41047A60u, 0xDF60EFC3u, 0xA867DF55u, 0x316E8EEFu, 0x4669BE79u,
        0xCB61B38Cu, 0xBC66831Au, 0x256FD2A0u, 0x5268E236u, 0xCC0C7795u, 0xBB0B4703u, 0x220216B9u, 0x5505262Fu,
        0xC5BA3BBEu, 0xB2BD0B28u, 0x2BB45A92u, 0x5CB36A04u, 0xC2D7FFA7u, 0xB5D0CF31u, 0x2CD99E8Bu, 0x5BDEAE1Du,
        0x9B64C2B0u, 0xEC63F226u, 0x756AA39Cu, 0x026D930Au, 0x9C0906A9u, 0xEB0E363Fu, 0x72076785u, 0x05005713u,
        0x95BF4A82u, 0xE2B87A14u, 0x7BB12BAEu, 0x0CB61B38u, 0x92D28E9Bu, 0xE5D5BE0Du, 0x7CDCEFB7u, 0x0BDBDF21u,
        0x86D3D2D4u, 0xF1D4E242u, 0x68DDB3F8u, 0x1FDA836Eu, 0x81BE16CDu, 0xF6B9265Bu, 0x6FB077E1u, 0x18B74777u,
        0x88085AE6u, 0xFF0F6A70u, 0x66063BCAu, 0x11010B5Cu, 0x8F659EFFu, 0xF862AE69u, 0x616BFFD3u, 0x166CCF45u,
        0xA00AE278u, 0xD70DD2EEu, 0x4E048354u, 0x3903B3C2u, 0xA7672661u, 0xD06016F7u, 0x4969474Du, 0x3E6E77DBu,
        0xAED16A4Au, 0xD9D65ADCu, 0x40DF0B66u, 0x37D83BF0u, 0xA9BCAE53u, 0xDEBB9EC5u, 0x47B2CF7Fu, 0x30B5FFE9u,
        0xBDBDF21Cu, 0xCABAC28Au, 0x53B39330u, 0x24B4A3A6u, 0xBAD03605u, 0xCDD70693u, 0x54DE5729u, 0x23D967BFu,
        0xB3667A2Eu, 0xC4614AB8u, 0x5D681B02u, 0x2A6F2B94u, 0xB40BBE37u, 0xC30C8EA1u, 0x5A05DF1Bu, 0x2D02EF8Du
    },
    {
        0x00000000u, 0x191B3141u, 0x32366282u, 0x2B2D53C3u, 0x646CC504u, 0x7D77F445u, 0x565AA786u, 0x4F4196C7u,
        0xC8D98A08u, 0xD1C2BB49u, 0xFAEFE88Au, 0xE3F4D9CBu, 0xACB54F0Cu, 0xB5AE7E4Du, 0x9E832D8Eu, 0x87981CCFu,
        0x4AC21251u, 0x53D92310u, 0x78F470D3u, 0x61EF4192u, 0x2EAED755u, 0x37B5E614u, 0x1C98B5D7u, 0x05838496u,
        0x821B9859u, 0x9B00A918u, 0xB02DFADBu, 0xA936CB9Au, 0xE6775D5Du, 0xFF6C6C1Cu, 0xD4413FDFu, 0xCD5A0E9Eu,
        0x958424A2u, 0x8C9F15E3u, 0xA7B24620u, 0xBEA97761u, 0xF1E8E1A6u, 0xE8F3D0E7u, 0xC3DE8324u, 0xDAC5B265u,
        0x5D5DAEAAu, 0x44469FEBu, 0x6F6BCC28u, 0x7670FD69u, 0x39316BAEu, 0x202A5AEFu, 0x0B07092Cu, 0x121C386Du,
        0xDF4636F3u, 0xC65D07B2u, 0xED705471u, 0xF46B6530u, 0xBB2AF3F7u, 0xA231C2B6u, 0x891C9175u, 0x9007A034u,
        0x179FBCFBu, 0x0E848DBAu, 0x25A9DE79u, 0x3CB2EF38u, 0x73F379FFu, 0x6AE848BEu, 0x41C51B7Du, 0x58DE2A3Cu,
        0xF0794F05u, 0xE9627E44u, 0xC24F2D87u, 0xDB541CC6u, 0x94158A01u, 0x8D0EBB40u, 0xA623E883u, 0xBF38D9C2u,
        0x38A0C50Du, 0x21BBF44Cu, 0x0A96A78Fu, 0x138D96CEu, 0x5CCC0009u, 0x45D73148u, 0x6EFA628Bu, 0x77E153CAu,
        0xBABB5D54u, 0xA3A06C15u, 0x888D3FD6u, 0x91960E97u, 0xDED79850u, 0xC7CCA911u, 0xECE1FAD2u, 0xF5FACB93u,
        0x7262D75Cu, 0x6B79E61Du, 0x4054B5DEu, 0x594F849Fu, 0x160E1258u, 0x0F152319u, 0x243870DAu, 0x3D23419Bu,
        0x65FD6BA7u, 0x7CE65AE6u, 0x57CB0925u, 0x4ED03864u, 0x0191AEA3u, 0x188A9FE2u, 0x33A7CC21u, 0x2ABCFD60u,
        0xAD24E1AFu, 0xB43FD0EEu, 0x9F12832Du, 0x8609B26Cu, 0xC94824ABu, 0xD05315EAu, 0xFB7E4629u, 0xE2657768u,
        0x2F3F79F6u, 0x362448B7u, 0x1D091B74u, 0x04122A35u, 0x4B53BCF2u, 0x52488DB3u, 0x7965DE70u, 0x607EEF31u,
        0xE7E6F3FEu, 0xFEFDC2BFu, 0xD5D0917Cu, 0xCCCBA03Du, 0x838A36FAu, 0x9A9107BBu, 0xB1BC5478u, 0xA8A76539u,
        0x3B83984Bu, 0x2298A90Au, 0x09B5FAC9u, 0x10AECB88u, 0x5FEF5D4Fu, 0x46F46C0Eu, 0x6DD93FCDu, 0x74C20E8Cu,
        0xF35A1243u, 0xEA412302u, 0xC16C70C1u, 0xD8774180u, 0x9736D747u, 0x8E2DE606u, 0xA500B5C5u, 0xBC1B8484u,
        0x71418A1Au, 0x685ABB5Bu, 0x4377E898u, 0x5A6CD9D9u, 0x152D4F1Eu, 0x0C367E5Fu, 0x271B2D9Cu, 0x3E001CDDu,
        0xB9980012u, 0xA0833153u, 0x8BAE6290u, 0x92B553D1u, 0xDDF4C516u, 0xC4EFF457u, 0xEFC2A794u, 0xF6D996D5u,
        0xAE07BCE9u, 0xB71C8DA8u, 0x9C31DE6Bu, 0x852AEF2Au, 0xCA6B79EDu, 0xD37048ACu, 0xF85D1B6Fu, 0xE1462A2Eu,
        0x66DE36E1u, 0x7FC507A0u, 0x54E85463u, 0x4DF36522u, 0x02B2F3E5u, 0x1BA9C2A4u, 0x30849167u, 0x299FA026u,
        0xE4C5AEB8u, 0xFDDE9FF9u, 0xD6F3CC3Au, 0xCFE8FD7Bu, 0x80A96BBCu, 0x99B25AFDu, 0xB29F093Eu, 0xAB84387Fu,
        0x2C1C24B0u, 0x350715F1u, 0x1E2A4632u, 0x07317773u, 0x4870E1B4u, 0x516BD0F5u, 0x7A468336u, 0x635DB277u,
        0xCBFAD74Eu, 0xD2E1E60Fu, 0xF9CCB5CCu, 0xE0D7848Du, 0xAF96124Au, 0xB68D230Bu, 0x9DA070C8u, 0x84BB4189u,
        0x03235D46u, 0x1A386C07u, 0x31153FC4u, 0x280E0E85u, 0x674F9842u, 0x7E54A903u, 0x5579FAC0u, 0x4C62CB81u,
        0x8138C51Fu, 0x9823F45Eu, 0xB30EA79Du, 0xAA1596DCu, 0xE554001Bu, 0xFC4F315Au, 0xD7626299u, 0xCE7953D8u,
        0x49E14F17u, 0x50FA7E56u, 0x7BD72D95u, 0x62CC1CD4u, 0x2D8D8A13u, 0x3496BB52u, 0x1FBBE891u, 0x06A0D9D0u,
        0x5E7EF3ECu, 0x4765C2ADu, 0x6C48916Eu, 0x7553A02Fu, 0x3A1236E8u, 0x230907A9u, 0x0824546Au, 0x113F652Bu,
        0x96A779E4u, 0x8FBC48A5u, 0xA4911B66u, 0xBD8A2A27u, 0xF2CBBCE0u, 0xEBD08DA1u, 0xC0FDDE62u, 0xD9E6EF23u,
        0x14BCE1BDu, 0x0DA7D0FCu, 0x268A833Fu, 0x3F91B27Eu, 0x70D024B9u, 0x69CB15F8u, 0x42E6463Bu, 0x5BFD777Au,
        0xDC656BB5u, 0xC57E5AF4u, 0xEE530937u, 0xF7483876u, 0xB809AEB1u, 0xA1129FF0u, 0x8A3FCC33u, 0x9324FD72u
    },
    {
        0x00000000u, 0x01C26A37u, 0x0384D46Eu, 0x0246BE59u, 0x0709A8DCu, 0x06CBC2EBu, 0x048D7CB2u, 0x054F1685u,
        0x0E1351B8u, 0x0FD13B8Fu, 0x0D9785D6u, 0x0C55EFE1u, 0x091AF964u, 0x08D89353u, 0x0A9E2D0Au, 0x0B5C473Du,
        0x1C26A370u, 0x1DE4C947u, 0x1FA2771Eu, 0x1E601D29u, 0x1B2F0BACu, 0x1AED619Bu, 0x18ABDFC2u, 0x1969B5F5u,
        0x1235F2C8u, 0x13F798FFu, 0x11B126A6u, 0x10734C91u, 0x153C5A14u, 0x14FE3023u, 0x16B88E7Au, 0x177AE44Du,
        0x384D46E0u, 0x398F2CD7u, 0x3BC9928Eu, 0x3A0BF8B9u, 0x3F44EE3Cu, 0x3E86840Bu, 0x3CC03A52u, 0x3D025065u,
        0x365E1758u, 0x379C7D6Fu, 0x35DAC336u, 0x3418A901u, 0x3157BF84u, 0x3095D5B3u, 0x32D36BEAu, 0x331101DDu,
        0x246BE590u, 0x25A98FA7u, 0x27EF31FEu, 0x262D5BC9u, 0x23624D4Cu, 0x22A0277Bu, 0x20E69922u, 0x2124F315u,
        0x2A78B428u, 0x2BBADE1Fu, 0x29FC6046u, 0x283E0A71u, 0x2D711CF4u, 0x2CB376C3u, 0x2EF5C89Au, 0x2F37A2ADu,
        0x709A8DC0u, 0x7158E7F7u, 0x731E59AEu, 0x72DC3399u, 0x7793251Cu, 0x76514F2Bu, 0x7417F172u, 0x75D59B45u,
        0x7E89DC78u, 0x7F4BB64Fu, 0x7D0D0816u, 0x7CCF6221u, 0x798074A4u, 0x78421E93u, 0x7A04A0CAu, 0x7BC6CAFDu,
        0x6CBC2EB0u, 0x6D7E4487u, 0x6F38FADEu, 0x6EFA90E9u, 0x6BB5866Cu, 0x6A77EC5Bu, 0x68315202u, 0x69F33835u,
        0x62AF7F08u, 0x636D153Fu, 0x612BAB66u, 0x60E9C151u, 0x65A6D7D4u, 0x6464BDE3u, 0x662203BAu, 0x67E0698Du,
        0x48D7CB20u, 0x4915A117u, 0x4B531F4Eu, 0x4A917579u, 0x4FDE63FCu, 0x4E1C09CBu, 0x4C5AB792u, 0x4D98DDA5u,
        0x46C49A98u, 0x4706F0AFu, 0x45404EF6u, 0x448224C1u, 0x41CD3244u, 0x400F5873u, 0x4249E62Au, 0x438B8C1Du,
        0x54F16850u, 0x55330267u, 0x5775BC3Eu, 0x56B7D609u, 0x53F8C08Cu, 0x523AAABBu, 0x507C14E2u, 0x51BE7ED5u,
        0x5AE239E8u, 0x5B2053DFu, 0x5966ED86u, 0x58A487B1u, 0x5DEB9134u, 0x5C29FB03u, 0x5E6F455Au, 0x5FAD2F6Du,
        0xE1351B80u, 0xE0F771B7u, 0xE2B1CFEEu, 0xE373A5D9u, 0xE63CB35Cu, 0xE7FED96Bu, 0xE5B86732u, 0xE47A0D05u,
        0xEF264A38u, 0xEEE4200Fu, 0xECA29E56u, 0xED60F461u, 0xE82FE2E4u, 0xE9ED88D3u, 0xEBAB368Au, 0xEA695CBDu,
        0xFD13B8F0u, 0xFCD1D2C7u, 0xFE976C9Eu, 0xFF5506A9u, 0xFA1A102Cu, 0xFBD87A1Bu, 0xF99EC442u, 0xF85CAE75u,
        0xF300E948u, 0xF2C2837Fu, 0xF0843D26u, 0xF1465711u, 0xF4094194u, 0xF5CB2BA3u, 0xF78D95FAu, 0xF64FFFCDu,
        0xD9785D60u, 0xD8BA3757u, 0xDAFC890Eu, 0xDB3EE339u, 0xDE71F5BCu, 0xDFB39F8Bu, 0xDDF521D2u, 0xDC374BE5u,
        0xD76B0CD8u, 0xD6A966EFu, 0xD4EFD8B6u, 0xD52DB281u, 0xD062A404u, 0xD1A0CE33u, 0xD3E6706Au, 0xD2241A5Du,
        0xC55EFE10u, 0xC49C9427u, 0xC6DA2A7Eu, 0xC7184049u, 0xC25756CCu, 0xC3953CFBu, 0xC1D382A2u, 0xC011E895u,
        0xCB4DAFA8u, 0xCA8FC59Fu, 0xC8C97BC6u, 0xC90B11F1u, 0xCC440774u, 0xCD866D43u, 0xCFC0D31Au, 0xCE02B92Du,
        0x91AF9640u, 0x906DFC77u, 0x922B422Eu, 0x93E92819u, 0x96A63E9Cu, 0x976454ABu, 0x9522EAF2u, 0x94E080C5u,
        0x9FBCC7F8u, 0x9E7EADCFu, 0x9C381396u, 0x9DFA79A1u, 0x98B56F24u, 0x99770513u, 0x9B31BB4Au, 0x9AF3D17Du,
        0x8D893530u, 0x8C4B5F07u, 0x8E0DE15Eu, 0x8FCF8B69u, 0x8A809DECu, 0x8B42F7DBu, 0x89044982u, 0x88C623B5u,
        0x839A6488u, 0x82580EBFu, 0x801EB0E6u, 0x81DCDAD1u, 0x8493CC54u, 0x8551A663u, 0x8717183Au, 0x86D5720Du,
        0xA9E2D0A0u, 0xA820BA97u, 0xAA6604CEu, 0xABA46EF9u, 0xAEEB787Cu, 0xAF29124Bu, 0xAD6FAC12u, 0xACADC625u,
        0xA7F18118u, 0xA633EB2Fu, 0xA4755576u, 0xA5B73F41u, 0xA0F829C4u, 0xA13A43F3u, 0xA37CFDAAu, 0xA2BE979Du,
        0xB5C473D0u, 0xB40619E7u, 0xB640A7BEu, 0xB782CD89u, 0xB2CDDB0Cu, 0xB30FB13Bu, 0xB1490F62u, 0xB08B6555u,
        0xBBD72268u, 0xBA15485Fu, 0xB853F606u, 0xB9919C31u, 0xBCDE8AB4u, 0xBD1CE083u, 0xBF5A5EDAu, 0xBE9834EDu
    },
    {
        0x00000000u, 0xB8BC6765u, 0xAA09C88Bu, 0x12B5AFEEu, 0x8F629757u, 0x37DEF032u, 0x256B5FDCu, 0x9DD738B9u,
        0xC5B428EFu, 0x7D084F8Au, 0x6FBDE064u, 0xD7018701u, 0x4AD6BFB8u, 0xF26AD8DDu, 0xE0DF7733u, 0x58631056u,
        0x5019579Fu, 0xE8A530FAu, 0xFA109F14u, 0x42ACF871u, 0xDF7BC0C8u, 0x67C7A7ADu, 0x75720843u, 0xCDCE6F26u,
        0x95AD7F70u, 0x2D111815u, 0x3FA4B7FBu, 0x8718D09Eu, 0x1ACFE827u, 0xA2738F42u, 0xB0C620ACu, 0x087A47C9u,
        0xA032AF3Eu, 0x188EC85Bu, 0x0A3B67B5u, 0xB28700D0u, 0x2F503869u, 0x97EC5F0Cu, 0x8559F0E2u, 0x3DE59787u,
        0x658687D1u, 0xDD3AE0B4u, 0xCF8F4F5Au, 0x7733283Fu, 0xEAE41086u, 0x525877E3u, 0x40EDD80Du, 0xF851BF68u,
        0xF02BF8A1u, 0x48979FC4u, 0x5A22302Au, 0xE29E574Fu, 0x7F496FF6u, 0xC7F50893u, 0xD540A77Du, 0x6DFCC018u,
        0x359FD04Eu, 0x8D23B72Bu, 0x9F9618C5u, 0x272A7FA0u, 0xBAFD4719u, 0x0241207Cu, 0x10F48F92u, 0xA848E8F7u,
        0x9B14583Du, 0x23A83F58u, 0x311D90B6u, 0x89A1F7D3u, 0x1476CF6Au, 0xACCAA80Fu, 0xBE7F07E1u, 0x06C36084u,
        0x5EA070D2u, 0xE61C17B7u, 0xF4A9B859u, 0x4C15DF3Cu, 0xD1C2E785u, 0x697E80E0u, 0x7BCB2F0Eu, 0xC377486Bu,
        0xCB0D0FA2u, 0x73B168C7u, 0x6104C729u, 0xD9B8A04Cu, 0x446F98F5u, 0xFCD3FF90u, 0xEE66507Eu, 0x56DA371Bu,
        0x0EB9274Du, 0xB6054028u, 0xA4B0EFC6u, 0x1C0C88A3u, 0x81DBB01Au, 0x3967D77Fu, 0x2BD27891u, 0x936E1FF4u,
        0x3B26F703u, 0x839A9066u, 0x912F3F88u, 0x299358EDu, 0xB4446054u, 0x0CF80731u, 0x1E4DA8DFu, 0xA6F1CFBAu,
        0xFE92DFECu, 0x462EB889u, 0x549B1767u, 0xEC277002u, 0x71F048BBu, 0xC94C2FDEu, 0xDBF98030u, 0x6345E755u,
        0x6B3FA09Cu, 0xD383C7F9u, 0xC1366817u, 0x798A0F72u, 0xE45D37CBu, 0x5CE150AEu, 0x4E54FF40u, 0xF6E89825u,
        0xAE8B8873u, 0x1637EF16u, 0x048240F8u, 0xBC3E279Du, 0x21E91F24u, 0x99557841u, 0x8BE0D7AFu, 0x335CB0CAu,
        0xED59B63Bu, 0x55E5D15Eu, 0x47507EB0u, 0xFFEC19D5u, 0x623B216Cu, 0xDA874609u, 0xC832E9E7u, 0x708E8E82u,
        0x28ED9ED4u, 0x9051F9B1u, 0x82E4565Fu, 0x3A58313Au, 0xA78F0983u, 0x1F336EE6u, 0x0D86C108u, 0xB53AA66Du,
        0xBD40E1A4u, 0x05FC86C1u, 0x1749292Fu, 0xAFF54E4Au, 0x322276F3u, 0x8A9E1196u, 0x982BBE78u, 0x2097D91Du,
        0x78F4C94Bu, 0xC048AE2Eu, 0xD2FD01C0u, 0x6A4166A5u, 0xF7965E1Cu, 0x4F2A3979u, 0x5D9F9697u, 0xE523F1F2u,
        0x4D6B1905u, 0xF5D77E60u, 0xE762D18Eu, 0x5FDEB6EBu, 0xC2098E52u, 0x7AB5E937u, 0x680046D9u, 0xD0BC21BCu,
        0x88DF31EAu, 0x3063568Fu, 0x22D6F961u, 0x9A6A9E04u, 0x07BDA6BDu, 0xBF01C1D8u, 0xADB46E36u, 0x15080953u,
        0x1D724E9Au, 0xA5CE29FFu, 0xB77B8611u, 0x0FC7E174u, 0x9210D9CDu, 0x2AACBEA8u, 0x38191146u, 0x80A57623u,
        0xD8C66675u, 0x607A0110u, 0x72CFAEFEu, 0xCA73C99Bu, 0x57A4F122u, 0xEF189647u, 0xFDAD39A9u, 0x45115ECCu,
        0x764DEE06u, 0xCEF18963u, 0xDC44268Du, 0x64F841E8u, 0xF92F7951u, 0x41931E34u, 0x5326B1DAu, 0xEB9AD6BFu,
        0xB3F9C6E9u, 0x0B45A18Cu, 0x19F00E62u, 0xA14C6907u, 0x3C9B51BEu, 0x842736DBu, 0x96929935u, 0x2E2EFE50u,
        0x2654B999u, 0x9EE8DEFCu, 0x8C5D7112u, 0x34E11677u, 0xA9362ECEu, 0x118A49ABu, 0x033FE645u, 0xBB838120u,
        0xE3E09176u, 0x5B5CF613u, 0x49E959FDu, 0xF1553E98u, 0x6C820621u, 0xD43E6144u, 0xC68BCEAAu, 0x7E37A9CFu,
        0xD67F4138u, 0x6EC3265Du, 0x7C7689B3u, 0xC4CAEED6u, 0x591DD66Fu, 0xE1A1B10Au, 0xF3141EE4u, 0x4BA87981u,
        0x13CB69D7u, 0xAB770EB2u, 0xB9C2A15Cu, 0x017EC639u, 0x9CA9FE80u, 0x241599E5u, 0x36A0360Bu, 0x8E1C516Eu,
        0x866616A7u, 0x3EDA71C2u, 0x2C6FDE2Cu, 0x94D3B949u, 0x090481F0u, 0xB1B8E695u, 0xA30D497Bu, 0x1BB12E1Eu,
        0x43D23E48u, 0xFB6E592Du, 0xE9DBF6C3u, 0x516791A6u, 0xCCB0A91Fu, 0x740CCE7Au, 0x66B96194u, 0xDE0506F1u
    },
    {
        0x00000000u, 0x3D6029B0u, 0x7AC05360u, 0x47A07AD0u, 0xF580A6C0u, 0xC8E08F70u, 0x8F40F5A0u, 0xB220DC10u,
        0x30704BC1u, 0x0D106271u, 0x4AB018A1u, 0x77D03111u, 0xC5F0ED01u, 0xF890C4B1u, 0xBF30BE61u, 0x825097D1u,
        0x60E09782u, 0x5D80BE32u, 0x1A20C4E2u, 0x2740ED52u, 0x95603142u, 0xA80018F2u, 0xEFA06222u, 0xD2C04B92u,
        0x5090DC43u, 0x6DF0F5F3u, 0x2A508F23u, 0x1730A693u, 0xA5107A83u, 0x98705333u, 0xDFD029E3u, 0xE2B00053u,
        0xC1C12F04u, 0xFCA106B4u, 0xBB017C64u, 0x866155D4u, 0x344189C4u, 0x0921A074u, 0x4E81DAA4u, 0x73E1F314u,
        0xF1B164C5u, 0xCCD14D75u, 0x8B7137A5u, 0xB6111E15u, 0x0431C205u, 0x3951EBB5u, 0x7EF19165u, 0x4391B8D5u,
        0xA121B886u, 0x9C419136u, 0xDBE1EBE6u, 0xE681C256u, 0x54A11E46u, 0x69C137F6u, 0x2E614D26u, 0x13016496u,
        0x9151F347u, 0xAC31DAF7u, 0xEB91A027u, 0xD6F18997u, 0x64D15587u, 0x59B17C37u, 0x1E1106E7u, 0x23712F57u,
        0x58F35849u, 0x659371F9u, 0x22330B29u, 0x1F532299u, 0xAD73FE89u, 0x9013D739u, 0xD7B3ADE9u, 0xEAD38459u,
        0x68831388u, 0x55E33A38u, 0x124340E8u, 0x2F236958u, 0x9D03B548u, 0xA0639CF8u, 0xE7C3E628u, 0xDAA3CF98u,
        0x3813CFCBu, 0x0573E67Bu, 0x42D39CABu, 0x7FB3B51Bu, 0xCD93690Bu, 0xF0F340BBu, 0xB7533A6Bu, 0x8A3313DBu,
        0x0863840Au, 0x3503ADBAu, 0x72A3D76Au, 0x4FC3FEDAu, 0xFDE322CAu, 0xC0830B7Au, 0x872371AAu, 0xBA43581Au,
        0x9932774Du, 0xA4525EFDu, 0xE3F2242Du, 0xDE920D9Du, 0x6CB2D18Du, 0x51D2F83Du, 0x167282EDu, 0x2B12AB5Du,
        0xA9423C8Cu, 0x9422153Cu, 0xD3826FECu, 0xEEE2465Cu, 0x5CC29A4Cu, 0x61A2B3FCu, 0x2602C92Cu, 0x1B62E09Cu,
        0xF9D2E0CFu, 0xC4B2C97Fu, 0x8312B3AFu, 0xBE729A1Fu, 0x0C52460Fu, 0x31326FBFu, 0x7692156Fu, 0x4BF23CDFu,
        0xC9A2AB0Eu, 0xF4C282BEu, 0xB362F86Eu, 0x8E02D1DEu, 0x3C220DCEu, 0x0142247Eu, 0x46E25EAEu, 0x7B82771Eu,
        0xB1E6B092u, 0x8C869922u, 0xCB26E3F2u, 0xF646CA42u, 0x44661652u, 0x79063FE2u, 0x3EA64532u, 0x03C66C82u,
        0x8196FB53u, 0xBCF6D2E3u, 0xFB56A833u, 0xC6368183u, 0x74165D93u, 0x49767423u, 0x0ED60EF3u, 0x33B62743u,
        0xD1062710u, 0xEC660EA0u, 0xABC67470u, 0x96A65DC0u, 0x248681D0u, 0x19E6A860u, 0x5E46D2B0u, 0x6326FB00u,
        0xE1766CD1u, 0xDC164561u, 0x9BB63FB1u, 0xA6D61601u, 0x14F6CA11u, 0x2996E3A1u, 0x6E369971u, 0x5356B0C1u,
        0x70279F96u, 0x4D47B626u, 0x0AE7CCF6u, 0x3787E546u, 0x85A73956u, 0xB8C710E6u, 0xFF676A36u, 0xC2074386u,
        0x4057D457u, 0x7D37FDE7u, 0x3A978737u, 0x07F7AE87u, 0xB5D77297u, 0x88B75B27u, 0xCF1721F7u, 0xF2770847u,
        0x10C70814u, 0x2DA721A4u, 0x6A075B74u, 0x576772C4u, 0xE547AED4u, 0xD8278764u, 0x9F87FDB4u, 0xA2E7D404u,
        0x20B743D5u, 0x1DD76A65u, 0x5A7710B5u, 0x67173905u, 0xD537E515u, 0xE857CCA5u, 0xAFF7B675u, 0x92979FC5u,
        0xE915E8DBu, 0xD475C16Bu, 0x93D5BBBBu, 0xAEB5920Bu, 0x1C954E1Bu, 0x21F567ABu, 0x66551D7Bu, 0x5B3534CBu,
        0xD965A31Au, 0xE4058AAAu, 0xA3A5F07Au, 0x9EC5D9CAu, 0x2CE505DAu, 0x11852C6Au, 0x562556BAu, 0x6B457F0Au,
        0x89F57F59u, 0xB49556E9u, 0xF3352C39u, 0xCE550589u, 0x7C75D999u, 0x4115F029u, 0x06B58AF9u, 0x3BD5A349u,
        0xB9853498u, 0x84E51D28u, 0xC34567F8u, 0xFE254E48u, 0x4C059258u, 0x7165BBE8u, 0x36C5C138u, 0x0BA5E888u,
        0x28D4C7DFu, 0x15B4EE6Fu, 0x521494BFu, 0x6F74BD0Fu, 0xDD54611Fu, 0xE03448AFu, 0xA794327Fu, 0x9AF41BCFu,
        0x18A48C1Eu, 0x25C4A5AEu, 0x6264DF7Eu, 0x5F04F6CEu, 0xED242ADEu, 0xD044036Eu, 0x97E479BEu, 0xAA84500Eu,
        0x4834505Du, 0x755479EDu, 0x32F4033Du, 0x0F942A8Du, 0xBDB4F69Du, 0x80D4DF2Du, 0xC774A5FDu, 0xFA148C4Du,
        0x78441B9Cu, 0x4524322Cu, 0x028448FCu, 0x3FE4614Cu, 0x8DC4BD5Cu, 0xB0A494ECu, 0xF704EE3Cu, 0xCA64C78Cu
    },
    {
        0x00000000u, 0xCB5CD3A5u, 0x4DC8A10Bu, 0x869472AEu, 0x9B914216u, 0x50CD91B3u, 0xD659E31Du, 0x1D0530B8u,
        0xEC53826Du, 0x270F51C8u, 0xA19B2366u, 0x6AC7F0C3u, 0x77C2C07Bu, 0xBC9E13DEu, 0x3A0A6170u, 0xF156B2D5u,
        0x03D6029Bu, 0xC88AD13Eu, 0x4E1EA390u, 0x85427035u, 0x9847408Du, 0x531B9328u, 0xD58FE186u, 0x1ED33223u,
        0xEF8580F6u, 0x24D95353u, 0xA24D21FDu, 0x6911F258u, 0x7414C2E0u, 0xBF481145u, 0x39DC63EBu, 0xF280B04Eu,
        0x07AC0536u, 0xCCF0D693u, 0x4A64A43Du, 0x81387798u, 0x9C3D4720u, 0x57619485u, 0xD1F5E62Bu, 0x1AA9358Eu,
        0xEBFF875Bu, 0x20A354FEu, 0xA6372650u, 0x6D6BF5F5u, 0x706EC54Du, 0xBB3216E8u, 0x3DA66446u, 0xF6FAB7E3u,
        0x047A07ADu, 0xCF26D408u, 0x49B2A6A6u, 0x82EE7503u, 0x9FEB45BBu, 0x54B7961Eu, 0xD223E4B0u, 0x197F3715u,
        0xE82985C0u, 0x23755665u, 0xA5E124CBu, 0x6EBDF76Eu, 0x73B8C7D6u, 0xB8E41473u, 0x3E7066DDu, 0xF52CB578u,
        0x0F580A6Cu, 0xC404D9C9u, 0x4290AB67u, 0x89CC78C2u, 0x94C9487Au, 0x5F959BDFu, 0xD901E971u, 0x125D3AD4u,
        0xE30B8801u, 0x28575BA4u, 0xAEC3290Au, 0x659FFAAFu, 0x789ACA17u, 0xB3C619B2u, 0x35526B1Cu, 0xFE0EB8B9u,
        0x0C8E08F7u, 0xC7D2DB52u, 0x4146A9FCu, 0x8A1A7A59u, 0x971F4AE1u, 0x5C439944u, 0xDAD7EBEAu, 0x118B384Fu,
        0xE0DD8A9Au, 0x2B81593Fu, 0xAD152B91u, 0x6649F834u, 0x7B4CC88Cu, 0xB0101B29u, 0x36846987u, 0xFDD8BA22u,
        0x08F40F5Au, 0xC3A8DCFFu, 0x453CAE51u, 0x8E607DF4u, 0x93654D4Cu, 0x58399EE9u, 0xDEADEC47u, 0x15F13FE2u,
        0xE4A78D37u, 0x2FFB5E92u, 0xA96F2C3Cu, 0x6233FF99u, 0x7F36CF21u, 0xB46A1C84u, 0x32FE6E2Au, 0xF9A2BD8Fu,
        0x0B220DC1u, 0xC07EDE64u, 0x46EAACCAu, 0x8DB67F6Fu, 0x90B34FD7u, 0x5BEF9C72u, 0xDD7BEEDCu, 0x16273D79u,
        0xE7718FACu, 0x2C2D5C09u, 0xAAB92EA7u, 0x61E5FD02u, 0x7CE0CDBAu, 0xB7BC1E1Fu, 0x31286CB1u, 0xFA74BF14u,
        0x1EB014D8u, 0xD5ECC77Du, 0x5378B5D3u, 0x98246676u, 0x852156CEu, 0x4E7D856Bu, 0xC8E9F7C5u, 0x03B52460u,
        0xF2E396B5u, 0x39BF4510u, 0xBF2B37BEu, 0x7477E41Bu, 0x6972D4A3u, 0xA22E0706u, 0x24BA75A8u, 0xEFE6A60Du,
        0x1D661643u, 0xD63AC5E6u, 0x50AEB748u, 0x9BF264EDu, 0x86F75455u, 0x4DAB87F0u, 0xCB3FF55Eu, 0x006326FBu,
        0xF135942Eu, 0x3A69478Bu, 0xBCFD3525u, 0x77A1E680u, 0x6AA4D638u, 0xA1F8059Du, 0x276C7733u, 0xEC30A496u,
        0x191C11EEu, 0xD240C24Bu, 0x54D4B0E5u, 0x9F886340u, 0x828D53F8u, 0x49D1805Du, 0xCF45F2F3u, 0x04192156u,
        0xF54F9383u, 0x3E134026u, 0xB8873288u, 0x73DBE12Du, 0x6EDED195u, 0xA5820230u, 0x2316709Eu, 0xE84AA33Bu,
        0x1ACA1375u, 0xD196C0D0u, 0x5702B27Eu, 0x9C5E61DBu, 0x815B5163u, 0x4A0782C6u, 0xCC93F068u, 0x07CF23CDu,
        0xF6999118u, 0x3DC542BDu, 0xBB513013u, 0x700DE3B6u, 0x6D08D30Eu, 0xA65400ABu, 0x20C07205u, 0xEB9CA1A0u,
        0x11E81EB4u, 0xDAB4CD11u, 0x5C20BFBFu, 0x977C6C1Au, 0x8A795CA2u, 0x41258F07u, 0xC7B1FDA9u, 0x0CED2E0Cu,
        0xFDBB9CD9u, 0x36E74F7Cu, 0xB0733DD2u, 0x7B2FEE77u, 0x662ADECFu, 0xAD760D6Au, 0x2BE27FC4u, 0xE0BEAC61u,
        0x123E1C2Fu, 0xD962CF8Au, 0x5FF6BD24u, 0x94AA6E81u, 0x89AF5E39u, 0x42F38D9Cu, 0xC467FF32u, 0x0F3B2C97u,
        0xFE6D9E42u, 0x35314DE7u, 0xB3A53F49u, 0x78F9ECECu, 0x65FCDC54u, 0xAEA00FF1u, 0x28347D5Fu, 0xE368AEFAu,
        0x16441B82u, 0xDD18C827u, 0x5B8CBA89u, 0x90D0692Cu, 0x8DD55994u, 0x46898A31u, 0xC01DF89Fu, 0x0B412B3Au,
        0xFA1799EFu, 0x314B4A4Au, 0xB7DF38E4u, 0x7C83EB41u, 0x6186DBF9u, 0xAADA085Cu, 0x2C4E7AF2u, 0xE712A957u,
        0x15921919u, 0xDECECABCu, 0x585AB812u, 0x93066BB7u, 0x8E035B0Fu, 0x455F88AAu, 0xC3CBFA04u, 0x089729A1u,
        0xF9C19B74u, 0x329D48D1u, 0xB4093A7Fu, 0x7F55E9DAu, 0x6250D962u, 0xA90C0AC7u, 0x2F987869u, 0xE4C4ABCCu
    },
    {
        0x00000000u, 0xA6770BB4u, 0x979F1129u, 0x31E81A9Du, 0xF44F2413u, 0x52382FA7u, 0x63D0353Au, 0xC5A73E8Eu,
        0x33EF4E67u, 0x959845D3u, 0xA4705F4Eu, 0x020754FAu, 0xC7A06A74u, 0x61D761C0u, 0x503F7B5Du, 0xF64870E9u,
        0x67DE9CCEu, 0xC1A9977Au, 0xF0418DE7u, 0x56368653u, 0x9391B8DDu, 0x35E6B369u, 0x040EA9F4u, 0xA279A240u,
        0x5431D2A9u, 0xF246D91Du, 0xC3AEC380u, 0x65D9C834u, 0xA07EF6BAu, 0x0609FD0Eu, 0x37E1E793u, 0x9196EC27u,
        0xCFBD399Cu, 0x69CA3228u, 0x582228B5u, 0xFE552301u, 0x3BF21D8Fu, 0x9D85163Bu, 0xAC6D0CA6u, 0x0A1A0712u,
        0xFC5277FBu, 0x5A257C4Fu, 0x6BCD66D2u, 0xCDBA6D66u, 0x081D53E8u, 0xAE6A585Cu, 0x9F8242C1u, 0x39F54975u,
        0xA863A552u, 0x0E14AEE6u, 0x3FFCB47Bu, 0x998BBFCFu, 0x5C2C8141u, 0xFA5B8AF5u, 0xCBB39068u, 0x6DC49BDCu,
        0x9B8CEB35u, 0x3DFBE081u, 0x0C13FA1Cu, 0xAA64F1A8u, 0x6FC3CF26u, 0xC9B4C492u, 0xF85CDE0Fu, 0x5E2BD5BBu,
        0x440B7579u, 0xE27C7ECDu, 0xD3946450u, 0x75E36FE4u, 0xB044516Au, 0x16335ADEu, 0x27DB4043u, 0x81AC4BF7u,
        0x77E43B1Eu, 0xD19330AAu, 0xE07B2A37u, 0x460C2183u, 0x83AB1F0Du, 0x25DC14B9u, 0x14340E24u, 0xB2430590u,
        0x23D5E9B7u, 0x85A2E203u, 0xB44AF89Eu, 0x123DF32Au, 0xD79ACDA4u, 0x71EDC610u, 0x4005DC8Du, 0xE672D739u,
        0x103AA7D0u, 0xB64DAC64u, 0x87A5B6F9u, 0x21D2BD4Du, 0xE47583C3u, 0x42028877u, 0x73EA92EAu, 0xD59D995Eu,
        0x8BB64CE5u, 0x2DC14751u, 0x1C295DCCu, 0xBA5E5678u, 0x7FF968F6u, 0xD98E6342u, 0xE86679DFu, 0x4E11726Bu,
        0xB8590282u, 0x1E2E0936u, 0x2FC613ABu, 0x89B1181Fu, 0x4C162691u, 0xEA612D25u, 0xDB8937B8u, 0x7DFE3C0Cu,
        0xEC68D02Bu, 0x4A1FDB9Fu, 0x7BF7C102u, 0xDD80CAB6u, 0x1827F438u, 0xBE50FF8Cu, 0x8FB8E511u, 0x29CFEEA5u,
        0xDF879E4Cu, 0x79F095F8u, 0x48188F65u, 0xEE6F84D1u, 0x2BC8BA5Fu, 0x8DBFB1EBu, 0xBC57AB76u, 0x1A20A0C2u,
        0x8816EAF2u, 0x2E61E146u, 0x1F89FBDBu, 0xB9FEF06Fu, 0x7C59CEE1u, 0xDA2EC555u, 0xEBC6DFC8u, 0x4DB1D47Cu,
        0xBBF9A495u, 0x1D8EAF21u, 0x2C66B5BCu, 0x8A11BE08u, 0x4FB68086u, 0xE9C18B32u, 0xD82991AFu, 0x7E5E9A1Bu,
        0xEFC8763Cu, 0x49BF7D88u, 0x78576715u, 0xDE206CA1u, 0x1B87522Fu, 0xBDF0599Bu, 0x8C184306u, 0x2A6F48B2u,
        0xDC27385Bu, 0x7A5033EFu, 0x4BB82972u, 0xEDCF22C6u, 0x28681C48u, 0x8E1F17FCu, 0xBFF70D61u, 0x198006D5u,
        0x47ABD36Eu, 0xE1DCD8DAu, 0xD034C247u, 0x7643C9F3u, 0xB3E4F77Du, 0x1593FCC9u, 0x247BE654u, 0x820CEDE0u,
        0x74449D09u, 0xD23396BDu, 0xE3DB8C20u, 0x45AC8794u, 0x800BB91Au, 0x267CB2AEu, 0x1794A833u, 0xB1E3A387u,
        0x20754FA0u, 0x86024414u, 0xB7EA5E89u, 0x119D553Du, 0xD43A6BB3u, 0x724D6007u, 0x43A57A9Au, 0xE5D2712Eu,
        0x139A01C7u, 0xB5ED0A73u, 0x840510EEu, 0x22721B5Au, 0xE7D525D4u, 0x41A22E60u, 0x704A34FDu, 0xD63D3F49u,
        0xCC1D9F8Bu, 0x6A6A943Fu, 0x5B828EA2u, 0xFDF58516u, 0x3852BB98u, 0x9E25B02Cu, 0xAFCDAAB1u, 0x09BAA105u,
        0xFFF2D1ECu, 0x5985DA58u, 0x686DC0C5u, 0xCE1ACB71u, 0x0BBDF5FFu, 0xADCAFE4Bu, 0x9C22E4D6u, 0x3A55EF62u,
        0xABC30345u, 0x0DB408F1u, 0x3C5C126Cu, 0x9A2B19D8u, 0x5F8C2756u, 0xF9FB2CE2u, 0xC813367Fu, 0x6E643DCBu,
        0x982C4D22u, 0x3E5B4696u, 0x0FB35C0Bu, 0xA9C457BFu, 0x6C636931u, 0xCA146285u, 0xFBFC7818u, 0x5D8B73ACu,
        0x03A0A617u, 0xA5D7ADA3u, 0x943FB73Eu, 0x3248BC8Au, 0xF7EF8204u, 0x519889B0u, 0x6070932Du, 0xC6079899u,
        0x304FE870u, 0x9638E3C4u, 0xA7D0F959u, 0x01A7F2EDu, 0xC400CC63u, 0x6277C7D7u, 0x539FDD4Au, 0xF5E8D6FEu,
        0x647E3AD9u, 0xC209316Du, 0xF3E12BF0u, 0x55962044u, 0x90311ECAu, 0x3646157Eu, 0x07AE0FE3u, 0xA1D90457u,
        0x579174BEu, 0xF1E67F0Au, 0xC00E6597u, 0x66796E23u, 0xA3DE50ADu, 0x05A95B19u, 0x34414184u, 0x92364A30u
    },
    {
        0x00000000u, 0xCCAA009Eu, 0x4225077Du, 0x8E8F07E3u, 0x844A0EFAu, 0x48E00E64u, 0xC66F0987u, 0x0AC50919u,
        0xD3E51BB5u, 0x1F4F1B2Bu, 0x91C01CC8u, 0x5D6A1C56u, 0x57AF154Fu, 0x9B0515D1u, 0x158A1232u, 0xD92012ACu,
        0x7CBB312Bu, 0xB01131B5u, 0x3E9E3656u, 0xF23436C8u, 0xF8F13FD1u, 0x345B3F4Fu, 0xBAD438ACu, 0x767E3832u,
        0xAF5E2A9Eu, 0x63F42A00u, 0xED7B2DE3u, 0x21D12D7Du, 0x2B142464u, 0xE7BE24FAu, 0x69312319u, 0xA59B2387u,
        0xF9766256u, 0x35DC62C8u, 0xBB53652Bu, 0x77F965B5u, 0x7D3C6CACu, 0xB1966C32u, 0x3F196BD1u, 0xF3B36B4Fu,
        0x2A9379E3u, 0xE639797Du, 0x68B67E9Eu, 0xA41C7E00u, 0xAED97719u, 0x62737787u, 0xECFC7064u, 0x205670FAu,
        0x85CD537Du, 0x496753E3u, 0xC7E85400u, 0x0B42549Eu, 0x01875D87u, 0xCD2D5D19u, 0x43A25AFAu, 0x8F085A64u,
        0x562848C8u, 0x9A824856u, 0x140D4FB5u, 0xD8A74F2Bu, 0xD2624632u, 0x1EC846ACu, 0x9047414Fu, 0x5CED41D1u,
        0x299DC2EDu, 0xE537C273u, 0x6BB8C590u, 0xA712C50Eu, 0xADD7CC17u, 0x617DCC89u, 0xEFF2CB6Au, 0x2358CBF4u,
        0xFA78D958u, 0x36D2D9C6u, 0xB85DDE25u, 0x74F7DEBBu, 0x7E32D7A2u, 0xB298D73Cu, 0x3C17D0DFu, 0xF0BDD041u,
        0x5526F3C6u, 0x998CF358u, 0x1703F4BBu, 0xDBA9F425u, 0xD16CFD3Cu, 0x1DC6FDA2u, 0x9349FA41u, 0x5FE3FADFu,
        0x86C3E873u, 0x4A69E8EDu, 0xC4E6EF0Eu, 0x084CEF90u, 0x0289E689u, 0xCE23E617u, 0x40ACE1F4u, 0x8C06E16Au,
        0xD0EBA0BBu, 0x1C41A025u, 0x92CEA7C6u, 0x5E64A758u, 0x54A1AE41u, 0x980BAEDFu, 0x1684A93Cu, 0xDA2EA9A2u,
        0x030EBB0Eu, 0xCFA4BB90u, 0x412BBC73u, 0x8D81BCEDu, 0x8744B5F4u, 0x4BEEB56Au, 0xC561B289u, 0x09CBB217u,
        0xAC509190u, 0x60FA910Eu, 0xEE7596EDu, 0x22DF9673u, 0x281A9F6Au, 0xE4B09FF4u, 0x6A3F9817u, 0xA6959889u,
        0x7FB58A25u, 0xB31F8ABBu, 0x3D908D58u, 0xF13A8DC6u, 0xFBFF84DFu, 0x37558441u, 0xB9DA83A2u, 0x7570833Cu,
        0x533B85DAu, 0x9F918544u, 0x111E82A7u, 0xDDB48239u, 0xD7718B20u, 0x1BDB8BBEu, 0x95548C5Du, 0x59FE8CC3u,
        0x80DE9E6Fu, 0x4C749EF1u, 0xC2FB9912u, 0x0E51998Cu, 0x04949095u, 0xC83E900Bu, 0x46B197E8u, 0x8A1B9776u,
        0x2F80B4F1u, 0xE32AB46Fu, 0x6DA5B38Cu, 0xA10FB312u, 0xABCABA0Bu, 0x6760BA95u, 0xE9EFBD76u, 0x2545BDE8u,
        0xFC65AF44u, 0x30CFAFDAu, 0xBE40A839u, 0x72EAA8A7u, 0x782FA1BEu, 0xB485A120u, 0x3A0AA6C3u, 0xF6A0A65Du,
        0xAA4DE78Cu, 0x66E7E712u, 0xE868E0F1u, 0x24C2E06Fu, 0x2E07E976u, 0xE2ADE9E8u, 0x6C22EE0Bu, 0xA088EE95u,
        0x79A8FC39u, 0xB502FCA7u, 0x3B8DFB44u, 0xF727FBDAu, 0xFDE2F2C3u, 0x3148F25Du, 0xBFC7F5BEu, 0x736DF520u,
        0xD6F6D6A7u, 0x1A5CD639u, 0x94D3D1DAu, 0x5879D144u, 0x52BCD85Du, 0x9E16D8C3u, 0x1099DF20u, 0xDC33DFBEu,
        0x0513CD12u, 0xC9B9CD8Cu, 0x4736CA6Fu, 0x8B9CCAF1u, 0x8159C3E8u, 0x4DF3C376u, 0xC37CC495u, 0x0FD6C40Bu,
        0x7AA64737u, 0xB60C47A9u, 0x3883404Au, 0xF42940D4u, 0xFEEC49CDu, 0x32464953u, 0xBCC94EB0u, 0x70634E2Eu,
        0xA9435C82u, 0x65E95C1Cu, 0xEB665BFFu, 0x27CC5B61u, 0x2D095278u, 0xE1A352E6u, 0x6F2C5505u, 0xA386559Bu,
        0x061D761Cu, 0xCAB77682u, 0x44387161u, 0x889271FFu, 0x825778E6u, 0x4EFD7878u, 0xC0727F9Bu, 0x0CD87F05u,
        0xD5F86DA9u, 0x19526D37u, 0x97DD6AD4u, 0x5B776A4Au, 0x51B26353u, 0x9D1863CDu, 0x1397642Eu, 0xDF3D64B0u,
        0x83D02561u, 0x4F7A25FFu, 0xC1F5221Cu, 0x0D5F2282u, 0x079A2B9Bu, 0xCB302B05u, 0x45BF2CE6u, 0x89152C78u,
        0x50353ED4u, 0x9C9F3E4Au, 0x121039A9u, 0xDEBA3937u, 0xD47F302Eu, 0x18D530B0u, 0x965A3753u, 0x5AF037CDu,
        0xFF6B144Au, 0x33C114D4u, 0xBD4E1337u, 0x71E413A9u, 0x7B211AB0u, 0xB78B1A2Eu, 0x39041DCDu, 0xF5AE1D53u,
        0x2C8E0FFFu, 0xE0240F61u, 0x6EAB0882u, 0xA201081Cu, 0xA8C40105u, 0x646E019Bu, 0xEAE10678u, 0x264B06E6u
    }
};

#if !defined(CRYP_HAVE_SSE42_CRC)
/* CRC-32C (Castagnoli): slicing-by-8 tables, reflected polynomial 0x82F63B78 (generated). */
static const uint32_t cryp_crc32c_table[8][256] = {
    {
        0x00000000u, 0xF26B8303u, 0xE13B70F7u, 0x1350F3F4u, 0xC79A971Fu, 0x35F1141Cu, 0x26A1E7E8u, 0xD4CA64EBu,
        0x8AD958CFu, 0x78B2DBCCu, 0x6BE22838u, 0x9989AB3Bu, 0x4D43CFD0u, 0xBF284CD3u, 0xAC78BF27u, 0x5E133C24u,
        0x105EC76Fu, 0xE235446Cu, 0xF165B798u, 0x030E349Bu, 0xD7C45070u, 0x25AFD373u, 0x36FF2087u, 0xC494A384u,
        0x9A879FA0u, 0x68EC1CA3u, 0x7BBCEF57u, 0x89D76C54u, 0x5D1D08BFu, 0xAF768BBCu, 0xBC267848u, 0x4E4DFB4Bu,
        0x20BD8EDEu, 0xD2D60DDDu, 0xC186FE29u, 0x33ED7D2Au, 0xE72719C1u, 0x154C9AC2u, 0x061C6936u, 0xF477EA35u,
        0xAA64D611u, 0x580F5512u, 0x4B5FA6E6u, 0xB93425E5u, 0x6DFE410Eu, 0x9F95C20Du, 0x8CC531F9u, 0x7EAEB2FAu,
        0x30E349B1u, 0xC288CAB2u, 0xD1D83946u, 0x23B3BA45u, 0xF779DEAEu, 0x05125DADu, 0x1642AE59u, 0xE4292D5Au,
        0xBA3A117Eu, 0x4851927Du, 0x5B016189u, 0xA96AE28Au, 0x7DA08661u, 0x8FCB0562u, 0x9C9BF696u, 0x6EF07595u,
        0x417B1DBCu, 0xB3109EBFu, 0xA0406D4Bu, 0x522BEE48u, 0x86E18AA3u, 0x748A09A0u, 0x67DAFA54u, 0x95B17957u,
        0xCBA24573u, 0x39C9C670u, 0x2A993584u, 0xD8F2B687u, 0x0C38D26Cu, 0xFE53516Fu, 0xED03A29Bu, 0x1F682198u,
        0x5125DAD3u, 0xA34E59D0u, 0xB01EAA24u, 0x42752927u, 0x96BF4DCCu, 0x64D4CECFu, 0x77843D3Bu, 0x85EFBE38u,
        0xDBFC821Cu, 0x2997011Fu, 0x3AC7F2EBu, 0xC8AC71E8u, 0x1C661503u, 0xEE0D9600u, 0xFD5D65F4u, 0x0F36E6F7u,
        0x61C69362u, 0x93AD1061u, 0x80FDE395u, 0x72966096u, 0xA65C047Du, 0x5437877Eu, 0x4767748Au, 0xB50CF789u,
        0xEB1FCBADu, 0x197448AEu, 0x0A24BB5Au, 0xF84F3859u, 0x2C855CB2u, 0xDEEEDFB1u, 0xCDBE2C45u, 0x3FD5AF46u,
        0x7198540Du, 0x83F3D70Eu, 0x90A324FAu, 0x62C8A7F9u, 0xB602C312u, 0x44694011u, 0x5739B3E5u, 0xA55230E6u,
        0xFB410CC2u, 0x092A8FC1u, 0x1A7A7C35u, 0xE811FF36u, 0x3CDB9BDDu, 0xCEB018DEu, 0xDDE0EB2Au, 0x2F8B6829u,
        0x82F63B78u, 0x709DB87Bu, 0x63CD4B8Fu, 0x91A6C88Cu, 0x456CAC67u, 0xB7072F64u, 0xA457DC90u, 0x563C5F93u,
        0x082F63B7u, 0xFA44E0B4u, 0xE9141340u, 0x1B7F9043u, 0xCFB5F4A8u, 0x3DDE77ABu, 0x2E8E845Fu, 0xDCE5075Cu,
        0x92A8FC17u, 0x60C37F14u, 0x73938CE0u, 0x81F80FE3u, 0x55326B08u, 0xA759E80Bu, 0xB4091BFFu, 0x466298FCu,
        0x1871A4D8u, 0xEA1A27DBu, 0xF94AD42Fu, 0x0B21572Cu, 0xDFEB33C7u, 0x2D80B0C4u, 0x3ED04330u, 0xCCBBC033u,
        0xA24BB5A6u, 0x502036A5u, 0x4370C551u, 0xB11B4652u, 0x65D122B9u, 0x97BAA1BAu, 0x84EA524Eu, 0x7681D14Du,
        0x2892ED69u, 0xDAF96E6Au, 0xC9A99D9Eu, 0x3BC21E9Du, 0xEF087A76u, 0x1D63F975u, 0x0E330A81u, 0xFC588982u,
        0xB21572C9u, 0x407EF1CAu, 0x532E023Eu, 0xA145813Du, 0x758FE5D6u, 0x87E466D5u, 0x94B49521u, 0x66DF1622u,
        0x38CC2A06u, 0xCAA7A905u, 0xD9F75AF1u, 0x2B9CD9F2u, 0xFF56BD19u, 0x0D3D3E1Au, 0x1E6DCDEEu, 0xEC064EEDu,
        0xC38D26C4u, 0x31E6A5C7u, 0x22B65633u, 0xD0DDD530u, 0x0417B1DBu, 0xF67C32D8u, 0xE52CC12Cu, 0x1747422Fu,
        0x49547E0Bu, 0xBB3FFD08u, 0xA86F0EFCu, 0x5A048DFFu, 0x8ECEE914u, 0x7CA56A17u, 0x6FF599E3u, 0x9D9E1AE0u,
        0xD3D3E1ABu, 0x21B862A8u, 0x32E8915Cu, 0xC083125Fu, 0x144976B4u, 0xE622F5B7u, 0xF5720643u, 0x07198540u,
        0x590AB964u, 0xAB613A67u, 0xB831C993u, 0x4A5A4A90u, 0x9E902E7Bu, 0x6CFBAD78u, 0x7FAB5E8Cu, 0x8DC0DD8Fu,
        0xE330A81Au, 0x115B2B19u, 0x020BD8EDu, 0xF0605BEEu, 0x24AA3F05u, 0xD6C1BC06u, 0xC5914FF2u, 0x37FACCF1u,
        0x69E9F0D5u, 0x9B8273D6u, 0x88D28022u, 0x7AB90321u, 0xAE7367CAu, 0x5C18E4C9u, 0x4F48173Du, 0xBD23943Eu,
        0xF36E6F75u, 0x0105EC76u, 0x12551F82u, 0xE03E9C81u, 0x34F4F86Au, 0xC69F7B69u, 0xD5CF889Du, 0x27A40B9Eu,
        0x79B737BAu, 0x8BDCB4B9u, 0x988C474Du, 0x6AE7C44Eu, 0xBE2DA0A5u, 0x4C4623A6u, 0x5F16D052u, 0xAD7D5351u
    },
    {
        0x00000000u, 0x13A29877u, 0x274530EEu, 0x34E7A899u, 0x4E8A61DCu, 0x5D28F9ABu, 0x69CF5132u, 0x7A6DC945u,
        0x9D14C3B8u, 0x8EB65BCFu, 0xBA51F356u, 0xA9F36B21u, 0xD39EA264u, 0xC03C3A13u, 0xF4DB928Au, 0xE7790AFDu,
        0x3FC5F181u, 0x2C6769F6u, 0x1880C16Fu, 0x0B225918u, 0x714F905Du, 0x62ED082Au, 0x560AA0B3u, 0x45A838C4u,
        0xA2D13239u, 0xB173AA4Eu, 0x859402D7u, 0x96369AA0u, 0xEC5B53E5u, 0xFFF9CB92u, 0xCB1E630Bu, 0xD8BCFB7Cu,
        0x7F8BE302u, 0x6C297B75u, 0x58CED3ECu, 0x4B6C4B9Bu, 0x310182DEu, 0x22A31AA9u, 0x1644B230u, 0x05E62A47u,
        0xE29F20BAu, 0xF13DB8CDu, 0xC5DA1054u, 0xD6788823u, 0xAC154166u, 0xBFB7D911u, 0x8B507188u, 0x98F2E9FFu,
        0x404E1283u, 0x53EC8AF4u, 0x670B226Du, 0x74A9BA1Au, 0x0EC4735Fu, 0x1D66EB28u, 0x298143B1u, 0x3A23DBC6u,
        0xDD5AD13Bu, 0xCEF8494Cu, 0xFA1FE1D5u, 0xE9BD79A2u, 0x93D0B0E7u, 0x80722890u, 0xB4958009u, 0xA737187Eu,
        0xFF17C604u, 0xECB55E73u, 0xD852F6EAu, 0xCBF06E9Du, 0xB19DA7D8u, 0xA23F3FAFu, 0x96D89736u, 0x857A0F41u,
        0x620305BCu, 0x71A19DCBu, 0x45463552u, 0x56E4AD25u, 0x2C896460u, 0x3F2BFC17u, 0x0BCC548Eu, 0x186ECCF9u,
        0xC0D23785u, 0xD370AFF2u, 0xE797076Bu, 0xF4359F1Cu, 0x8E585659u, 0x9DFACE2Eu, 0xA91D66B7u, 0xBABFFEC0u,
        0x5DC6F43Du, 0x4E646C4Au, 0x7A83C4D3u, 0x69215CA4u, 0x134C95E1u, 0x00EE0D96u, 0x3409A50Fu, 0x27AB3D78u,
        0x809C2506u, 0x933EBD71u, 0xA7D915E8u, 0xB47B8D9Fu, 0xCE1644DAu, 0xDDB4DCADu, 0xE9537434u, 0xFAF1EC43u,
        0x1D88E6BEu, 0x0E2A7EC9u, 0x3ACDD650u, 0x296F4E27u, 0x53028762u, 0x40A01F15u, 0x7447B78Cu, 0x67E52FFBu,
        0xBF59D487u, 0xACFB4CF0u, 0x981CE469u, 0x8BBE7C1Eu, 0xF1D3B55Bu, 0xE2712D2Cu, 0xD69685B5u, 0xC5341DC2u,
        0x224D173Fu, 0x31EF8F48u, 0x050827D1u, 0x16AABFA6u, 0x6CC776E3u, 0x7F65EE94u, 0x4B82460Du, 0x5820DE7Au,
        0xFBC3FAF9u, 0xE861628Eu, 0xDC86CA17u, 0xCF245260u, 0xB5499B25u, 0xA6EB0352u, 0x920CABCBu, 0x81AE33BCu,
        0x66D73941u, 0x7575A136u, 0x419209AFu, 0x523091D8u, 0x285D589Du, 0x3BFFC0EAu, 0x0F186873u, 0x1CBAF004u,
        0xC4060B78u, 0xD7A4930Fu, 0xE3433B96u, 0xF0E1A3E1u, 0x8A8C6AA4u, 0x992EF2D3u, 0xADC95A4Au, 0xBE6BC23Du,
        0x5912C8C0u, 0x4AB050B7u, 0x7E57F82Eu, 0x6DF56059u, 0x1798A91Cu, 0x043A316Bu, 0x30DD99F2u, 0x237F0185u,
        0x844819FBu, 0x97EA818Cu, 0xA30D2915u, 0xB0AFB162u, 0xCAC27827u, 0xD960E050u, 0xED8748C9u, 0xFE25D0BEu,
        0x195CDA43u, 0x0AFE4234u, 0x3E19EAADu, 0x2DBB72DAu, 0x57D6BB9Fu, 0x447423E8u, 0x70938B71u, 0x63311306u,
        0xBB8DE87Au, 0xA82F700Du, 0x9CC8D894u, 0x8F6A40E3u, 0xF50789A6u, 0xE6A511D1u, 0xD242B948u, 0xC1E0213Fu,
        0x26992BC2u, 0x353BB3B5u, 0x01DC1B2Cu, 0x127E835Bu, 0x68134A1Eu, 0x7BB1D269u, 0x4F567AF0u, 0x5CF4E287u,
        0x04D43CFDu, 0x1776A48Au, 0x23910C13u, 0x30339464u, 0x4A5E5D21u, 0x59FCC556u, 0x6D1B6DCFu, 0x7EB9F5B8u,
        0x99C0FF45u, 0x8A626732u, 0xBE85CFABu, 0xAD2757DCu, 0xD74A9E99u, 0xC4E806EEu, 0xF00FAE77u, 0xE3AD3600u,
        0x3B11CD7Cu, 0x28B3550Bu, 0x1C54FD92u, 0x0FF665E5u, 0x759BACA0u, 0x663934D7u, 0x52DE9C4Eu, 0x417C0439u,
        0xA6050EC4u, 0xB5A796B3u, 0x81403E2Au, 0x92E2A65Du, 0xE88F6F18u, 0xFB2DF76Fu, 0xCFCA5FF6u, 0xDC68C781u,
        0x7B5FDFFFu, 0x68FD4788u, 0x5C1AEF11u, 0x4FB87766u, 0x35D5BE23u, 0x26772654u, 0x12908ECDu, 0x013216BAu,
        0xE64B1C47u, 0xF5E98430u, 0xC10E2CA9u, 0xD2ACB4DEu, 0xA8C17D9Bu, 0xBB63E5ECu, 0x8F844D75u, 0x9C26D502u,
        0x449A2E7Eu, 0x5738B609u, 0x63DF1E90u, 0x707D86E7u, 0x0A104FA2u, 0x19B2D7D5u, 0x2D557F4Cu, 0x3EF7E73Bu,
        0xD98EEDC6u, 0xCA2C75B1u, 0xFECBDD28u, 0xED69455Fu, 0x97048C1Au, 0x84A6146Du, 0xB041BCF4u, 0xA3E32483u
    },
    {
        0x00000000u, 0xA541927Eu, 0x4F6F520Du, 0xEA2EC073u, 0x9EDEA41Au, 0x3B9F3664u, 0xD1B1F617u, 0x74F06469u,
        0x38513EC5u, 0x9D10ACBBu, 0x773E6CC8u, 0xD27FFEB6u, 0xA68F9ADFu, 0x03CE08A1u, 0xE9E0C8D2u, 0x4CA15AACu,
        0x70A27D8Au, 0xD5E3EFF4u, 0x3FCD2F87u, 0x9A8CBDF9u, 0xEE7CD990u, 0x4B3D4BEEu, 0xA1138B9Du, 0x045219E3u,
        0x48F3434Fu, 0xEDB2D131u, 0x079C1142u, 0xA2DD833Cu, 0xD62DE755u, 0x736C752Bu, 0x9942B558u, 0x3C032726u,
        0xE144FB14u, 0x4405696Au, 0xAE2BA919u, 0x0B6A3B67u, 0x7F9A5F0Eu, 0xDADBCD70u, 0x30F50D03u, 0x95B49F7Du,
        0xD915C5D1u, 0x7C5457AFu, 0x967A97DCu, 0x333B05A2u, 0x47CB61CBu, 0xE28AF3B5u, 0x08A433C6u, 0xADE5A1B8u,
        0x91E6869Eu, 0x34A714E0u, 0xDE89D493u, 0x7BC846EDu, 0x0F382284u, 0xAA79B0FAu, 0x40577089u, 0xE516E2F7u,
        0xA9B7B85Bu, 0x0CF62A25u, 0xE6D8EA56u, 0x43997828u, 0x37691C41u, 0x92288E3Fu, 0x78064E4Cu, 0xDD47DC32u,
        0xC76580D9u, 0x622412A7u, 0x880AD2D4u, 0x2D4B40AAu, 0x59BB24C3u, 0xFCFAB6BDu, 0x16D476CEu, 0xB395E4B0u,
        0xFF34BE1Cu, 0x5A752C62u, 0xB05BEC11u, 0x151A7E6Fu, 0x61EA1A06u, 0xC4AB8878u, 0x2E85480Bu, 0x8BC4DA75u,
        0xB7C7FD53u, 0x12866F2Du, 0xF8A8AF5Eu, 0x5DE93D20u, 0x29195949u, 0x8C58CB37u, 0x66760B44u, 0xC337993Au,
        0x8F96C396u, 0x2AD751E8u, 0xC0F9919Bu, 0x65B803E5u, 0x1148678Cu, 0xB409F5F2u, 0x5E273581u, 0xFB66A7FFu,
        0x26217BCDu, 0x8360E9B3u, 0x694E29C0u, 0xCC0FBBBEu, 0xB8FFDFD7u, 0x1DBE4DA9u, 0xF7908DDAu, 0x52D11FA4u,
        0x1E704508u, 0xBB31D776u, 0x511F1705u, 0xF45E857Bu, 0x80AEE112u, 0x25EF736Cu, 0xCFC1B31Fu, 0x6A802161u,
        0x56830647u, 0xF3C29439u, 0x19EC544Au, 0xBCADC634u, 0xC85DA25Du, 0x6D1C3023u, 0x8732F050u, 0x2273622Eu,
        0x6ED23882u, 0xCB93AAFCu, 0x21BD6A8Fu, 0x84FCF8F1u, 0xF00C9C98u, 0x554D0EE6u, 0xBF63CE95u, 0x1A225CEBu,
        0x8B277743u, 0x2E66E53Du, 0xC448254Eu, 0x6109B730u, 0x15F9D359u, 0xB0B84127u, 0x5A968154u, 0xFFD7132Au,
        0xB3764986u, 0x1637DBF8u, 0xFC191B8Bu, 0x595889F5u, 0x2DA8ED9Cu, 0x88E97FE2u, 0x62C7BF91u, 0xC7862DEFu,
        0xFB850AC9u, 0x5EC498B7u, 0xB4EA58C4u, 0x11ABCABAu, 0x655BAED3u, 0xC01A3CADu, 0x2A34FCDEu, 0x8F756EA0u,
        0xC3D4340Cu, 0x6695A672u, 0x8CBB6601u, 0x29FAF47Fu, 0x5D0A9016u, 0xF84B0268u, 0x1265C21Bu, 0xB7245065u,
        0x6A638C57u, 0xCF221E29u, 0x250CDE5Au, 0x804D4C24u, 0xF4BD284Du, 0x51FCBA33u, 0xBBD27A40u, 0x1E93E83Eu,
        0x5232B292u, 0xF77320ECu, 0x1D5DE09Fu, 0xB81C72E1u, 0xCCEC1688u, 0x69AD84F6u, 0x83834485u, 0x26C2D6FBu,
        0x1AC1F1DDu, 0xBF8063A3u, 0x55AEA3D0u, 0xF0EF31AEu, 0x841F55C7u, 0x215EC7B9u, 0xCB7007CAu, 0x6E3195B4u,
        0x2290CF18u, 0x87D15D66u, 0x6DFF9D15u, 0xC8BE0F6Bu, 0xBC4E6B02u, 0x190FF97Cu, 0xF321390Fu, 0x5660AB71u,
        0x4C42F79Au, 0xE90365E4u, 0x032DA597u, 0xA66C37E9u, 0xD29C5380u, 0x77DDC1FEu, 0x9DF3018Du, 0x38B293F3u,
        0x7413C95Fu, 0xD1525B21u, 0x3B7C9B52u, 0x9E3D092Cu, 0xEACD6D45u, 0x4F8CFF3Bu, 0xA5A23F48u, 0x00E3AD36u,
        0x3CE08A10u, 0x99A1186Eu, 0x738FD81Du, 0xD6CE4A63u, 0xA23E2E0Au, 0x077FBC74u, 0xED517C07u, 0x4810EE79u,
        0x04B1B4D5u, 0xA1F026ABu, 0x4BDEE6D8u, 0xEE9F74A6u, 0x9A6F10CFu, 0x3F2E82B1u, 0xD50042C2u, 0x7041D0BCu,
        0xAD060C8Eu, 0x08479EF0u, 0xE2695E83u, 0x4728CCFDu, 0x33D8A894u, 0x96993AEAu, 0x7CB7FA99u, 0xD9F668E7u,
        0x9557324Bu, 0x3016A035u, 0xDA386046u, 0x7F79F238u, 0x0B899651u, 0xAEC8042Fu, 0x44E6C45Cu, 0xE1A75622u,
        0xDDA47104u, 0x78E5E37Au, 0x92CB2309u, 0x378AB177u, 0x437AD51Eu, 0xE63B4760u, 0x0C158713u, 0xA954156Du,
        0xE5F54FC1u, 0x40B4DDBFu, 0xAA9A1DCCu, 0x0FDB8FB2u, 0x7B2BEBDBu, 0xDE6A79A5u, 0x3444B9D6u, 0x91052BA8u
    },
    {
        0x00000000u, 0xDD45AAB8u, 0xBF672381u, 0x62228939u, 0x7B2231F3u, 0xA6679B4Bu, 0xC4451272u, 0x1900B8CAu,
        0xF64463E6u, 0x2B01C95Eu, 0x49234067u, 0x9466EADFu, 0x8D665215u, 0x5023F8ADu, 0x32017194u, 0xEF44DB2Cu,
        0xE964B13Du, 0x34211B85u, 0x560392BCu, 0x8B463804u, 0x924680CEu, 0x4F032A76u, 0x2D21A34Fu, 0xF06409F7u,
        0x1F20D2DBu, 0xC2657863u, 0xA047F15Au, 0x7D025BE2u, 0x6402E328u, 0xB9474990u, 0xDB65C0A9u, 0x06206A11u,
        0xD725148Bu, 0x0A60BE33u, 0x6842370Au, 0xB5079DB2u, 0xAC072578u, 0x71428FC0u, 0x136006F9u, 0xCE25AC41u,
        0x2161776Du, 0xFC24DDD5u, 0x9E0654ECu, 0x4343FE54u, 0x5A43469Eu, 0x8706EC26u, 0xE524651Fu, 0x3861CFA7u,
        0x3E41A5B6u, 0xE3040F0Eu, 0x81268637u, 0x5C632C8Fu, 0x45639445u, 0x98263EFDu, 0xFA04B7C4u, 0x27411D7Cu,
        0xC805C650u, 0x15406CE8u, 0x7762E5D1u, 0xAA274F69u, 0xB327F7A3u, 0x6E625D1Bu, 0x0C40D422u, 0xD1057E9Au,
        0xABA65FE7u, 0x76E3F55Fu, 0x14C17C66u, 0xC984D6DEu, 0xD0846E14u, 0x0DC1C4ACu, 0x6FE34D95u, 0xB2A6E72Du,
        0x5DE23C01u, 0x80A796B9u, 0xE2851F80u, 0x3FC0B538u, 0x26C00DF2u, 0xFB85A74Au, 0x99A72E73u, 0x44E284CBu,
        0x42C2EEDAu, 0x9F874462u, 0xFDA5CD5Bu, 0x20E067E3u, 0x39E0DF29u, 0xE4A57591u, 0x8687FCA8u, 0x5BC25610u,
        0xB4868D3Cu, 0x69C32784u, 0x0BE1AEBDu, 0xD6A40405u, 0xCFA4BCCFu, 0x12E11677u, 0x70C39F4Eu, 0xAD8635F6u,
        0x7C834B6Cu, 0xA1C6E1D4u, 0xC3E468EDu, 0x1EA1C255u, 0x07A17A9Fu, 0xDAE4D027u, 0xB8C6591Eu, 0x6583F3A6u,
        0x8AC7288Au, 0x57828232u, 0x35A00B0Bu, 0xE8E5A1B3u, 0xF1E51979u, 0x2CA0B3C1u, 0x4E823AF8u, 0x93C79040u,
        0x95E7FA51u, 0x48A250E9u, 0x2A80D9D0u, 0xF7C57368u, 0xEEC5CBA2u, 0x3380611Au, 0x51A2E823u, 0x8CE7429Bu,
        0x63A399B7u, 0xBEE6330Fu, 0xDCC4BA36u, 0x0181108Eu, 0x1881A844u, 0xC5C402FCu, 0xA7E68BC5u, 0x7AA3217Du,
        0x52A0C93Fu, 0x8FE56387u, 0xEDC7EABEu, 0x30824006u, 0x2982F8CCu, 0xF4C75274u, 0x96E5DB4Du, 0x4BA071F5u,
        0xA4E4AAD9u, 0x79A10061u, 0x1B838958u, 0xC6C623E0u, 0xDFC69B2Au, 0x02833192u, 0x60A1B8ABu, 0xBDE41213u,
        0xBBC47802u, 0x6681D2BAu, 0x04A35B83u, 0xD9E6F13Bu, 0xC0E649F1u, 0x1DA3E349u, 0x7F816A70u, 0xA2C4C0C8u,
        0x4D801BE4u, 0x90C5B15Cu, 0xF2E73865u, 0x2FA292DDu, 0x36A22A17u, 0xEBE780AFu, 0x89C50996u, 0x5480A32Eu,
        0x8585DDB4u, 0x58C0770Cu, 0x3AE2FE35u, 0xE7A7548Du, 0xFEA7EC47u, 0x23E246FFu, 0x41C0CFC6u, 0x9C85657Eu,
        0x73C1BE52u, 0xAE8414EAu, 0xCCA69DD3u, 0x11E3376Bu, 0x08E38FA1u, 0xD5A62519u, 0xB784AC20u, 0x6AC10698u,
        0x6CE16C89u, 0xB1A4C631u, 0xD3864F08u, 0x0EC3E5B0u, 0x17C35D7Au, 0xCA86F7C2u, 0xA8A47EFBu, 0x75E1D443u,
        0x9AA50F6Fu, 0x47E0A5D7u, 0x25C22CEEu, 0xF8878656u, 0xE1873E9Cu, 0x3CC29424u, 0x5EE01D1Du, 0x83A5B7A5u,
        0xF90696D8u, 0x24433C60u, 0x4661B559u, 0x9B241FE1u, 0x8224A72Bu, 0x5F610D93u, 0x3D4384AAu, 0xE0062E12u,
        0x0F42F53Eu, 0xD2075F86u, 0xB025D6BFu, 0x6D607C07u, 0x7460C4CDu, 0xA9256E75u, 0xCB07E74Cu, 0x16424DF4u,
        0x106227E5u, 0xCD278D5Du, 0xAF050464u, 0x7240AEDCu, 0x6B401616u, 0xB605BCAEu, 0xD4273597u, 0x09629F2Fu,
        0xE6264403u, 0x3B63EEBBu, 0x59416782u, 0x8404CD3Au, 0x9D0475F0u, 0x4041DF48u, 0x22635671u, 0xFF26FCC9u,
        0x2E238253u, 0xF36628EBu, 0x9144A1D2u, 0x4C010B6Au, 0x5501B3A0u, 0x88441918u, 0xEA669021u, 0x37233A99u,
        0xD867E1B5u, 0x05224B0Du, 0x6700C234u, 0xBA45688Cu, 0xA345D046u, 0x7E007AFEu, 0x1C22F3C7u, 0xC167597Fu,
        0xC747336Eu, 0x1A0299D6u, 0x782010EFu, 0xA565BA57u, 0xBC65029Du, 0x6120A825u, 0x0302211Cu, 0xDE478BA4u,
        0x31035088u, 0xEC46FA30u, 0x8E647309u, 0x5321D9B1u, 0x4A21617Bu, 0x9764CBC3u, 0xF54642FAu, 0x2803E842u
    },
    {
        0x00000000u, 0x38116FACu, 0x7022DF58u, 0x4833B0F4u, 0xE045BEB0u, 0xD854D11Cu, 0x906761E8u, 0xA8760E44u,
        0xC5670B91u, 0xFD76643Du, 0xB545D4C9u, 0x8D54BB65u, 0x2522B521u, 0x1D33DA8Du, 0x55006A79u, 0x6D1105D5u,
        0x8F2261D3u, 0xB7330E7Fu, 0xFF00BE8Bu, 0xC711D127u, 0x6F67DF63u, 0x5776B0CFu, 0x1F45003Bu, 0x27546F97u,
        0x4A456A42u, 0x725405EEu, 0x3A67B51Au, 0x0276DAB6u, 0xAA00D4F2u, 0x9211BB5Eu, 0xDA220BAAu, 0xE2336406u,
        0x1BA8B557u, 0x23B9DAFBu, 0x6B8A6A0Fu, 0x539B05A3u, 0xFBED0BE7u, 0xC3FC644Bu, 0x8BCFD4BFu, 0xB3DEBB13u,
        0xDECFBEC6u, 0xE6DED16Au, 0xAEED619Eu, 0x96FC0E32u, 0x3E8A0076u, 0x069B6FDAu, 0x4EA8DF2Eu, 0x76B9B082u,
        0x948AD484u, 0xAC9BBB28u, 0xE4A80BDCu, 0xDCB96470u, 0x74CF6A34u, 0x4CDE0598u, 0x04EDB56Cu, 0x3CFCDAC0u,
        0x51EDDF15u, 0x69FCB0B9u, 0x21CF004Du, 0x19DE6FE1u, 0xB1A861A5u, 0x89B90E09u, 0xC18ABEFDu, 0xF99BD151u,
        0x37516AAEu, 0x0F400502u, 0x4773B5F6u, 0x7F62DA5Au, 0xD714D41Eu, 0xEF05BBB2u, 0xA7360B46u, 0x9F2764EAu,
        0xF236613Fu, 0xCA270E93u, 0x8214BE67u, 0xBA05D1CBu, 0x1273DF8Fu, 0x2A62B023u, 0x625100D7u, 0x5A406F7Bu,
        0xB8730B7Du, 0x806264D1u, 0xC851D425u, 0xF040BB89u, 0x5836B5CDu, 0x6027DA61u, 0x28146A95u, 0x10050539u,
        0x7D1400ECu, 0x45056F40u, 0x0D36DFB4u, 0x3527B018u, 0x9D51BE5Cu, 0xA540D1F0u, 0xED736104u, 0xD5620EA8u,
        0x2CF9DFF9u, 0x14E8B055u, 0x5CDB00A1u, 0x64CA6F0Du, 0xCCBC6149u, 0xF4AD0EE5u, 0xBC9EBE11u, 0x848FD1BDu,
        0xE99ED468u, 0xD18FBBC4u, 0x99BC0B30u, 0xA1AD649Cu, 0x09DB6AD8u, 0x31CA0574u, 0x79F9B580u, 0x41E8DA2Cu,
        0xA3DBBE2Au, 0x9BCAD186u, 0xD3F96172u, 0xEBE80EDEu, 0x439E009Au, 0x7B8F6F36u, 0x33BCDFC2u, 0x0BADB06Eu,
        0x66BCB5BBu, 0x5EADDA17u, 0x169E6AE3u, 0x2E8F054Fu, 0x86F90B0Bu, 0xBEE864A7u, 0xF6DBD453u, 0xCECABBFFu,
        0x6EA2D55Cu, 0x56B3BAF0u, 0x1E800A04u, 0x269165A8u, 0x8EE76BECu, 0xB6F60440u, 0xFEC5B4B4u, 0xC6D4DB18u,
        0xABC5DECDu, 0x93D4B161u, 0xDBE70195u, 0xE3F66E39u, 0x4B80607Du, 0x73910FD1u, 0x3BA2BF25u, 0x03B3D089u,
        0xE180B48Fu, 0xD991DB23u, 0x91A26BD7u, 0xA9B3047Bu, 0x01C50A3Fu, 0x39D46593u, 0x71E7D567u, 0x49F6BACBu,
        0x24E7BF1Eu, 0x1CF6D0B2u, 0x54C56046u, 0x6CD40FEAu, 0xC4A201AEu, 0xFCB36E02u, 0xB480DEF6u, 0x8C91B15Au,
        0x750A600Bu, 0x4D1B0FA7u, 0x0528BF53u, 0x3D39D0FFu, 0x954FDEBBu, 0xAD5EB117u, 0xE56D01E3u, 0xDD7C6E4Fu,
        0xB06D6B9Au, 0x887C0436u, 0xC04FB4C2u, 0xF85EDB6Eu, 0x5028D52Au, 0x6839BA86u, 0x200A0A72u, 0x181B65DEu,
        0xFA2801D8u, 0xC2396E74u, 0x8A0ADE80u, 0xB21BB12Cu, 0x1A6DBF68u, 0x227CD0C4u, 0x6A4F6030u, 0x525E0F9Cu,
        0x3F4F0A49u, 0x075E65E5u, 0x4F6DD511u, 0x777CBABDu, 0xDF0AB4F9u, 0xE71BDB55u, 0xAF286BA1u, 0x9739040Du,
        0x59F3BFF2u, 0x61E2D05Eu, 0x29D160AAu, 0x11C00F06u, 0xB9B60142u, 0x81A76EEEu, 0xC994DE1Au, 0xF185B1B6u,
        0x9C94B463u, 0xA485DBCFu, 0xECB66B3Bu, 0xD4A70497u, 0x7CD10AD3u, 0x44C0657Fu, 0x0CF3D58Bu, 0x34E2BA27u,
        0xD6D1DE21u, 0xEEC0B18Du, 0xA6F30179u, 0x9EE26ED5u, 0x36946091u, 0x0E850F3Du, 0x46B6BFC9u, 0x7EA7D065u,
        0x13B6D5B0u, 0x2BA7BA1Cu, 0x63940AE8u, 0x5B856544u, 0xF3F36B00u, 0xCBE204ACu, 0x83D1B458u, 0xBBC0DBF4u,
        0x425B0AA5u, 0x7A4A6509u, 0x3279D5FDu, 0x0A68BA51u, 0xA21EB415u, 0x9A0FDBB9u, 0xD23C6B4Du, 0xEA2D04E1u,
        0x873C0134u, 0xBF2D6E98u, 0xF71EDE6Cu, 0xCF0FB1C0u, 0x6779BF84u, 0x5F68D028u, 0x175B60DCu, 0x2F4A0F70u,
        0xCD796B76u, 0xF56804DAu, 0xBD5BB42Eu, 0x854ADB82u, 0x2D3CD5C6u, 0x152DBA6Au, 0x5D1E0A9Eu, 0x650F6532u,
        0x081E60E7u, 0x300F0F4Bu, 0x783CBFBFu, 0x402DD013u, 0xE85BDE57u, 0xD04AB1FBu, 0x9879010Fu, 0xA0686EA3u
    },
    {
        0x00000000u, 0xEF306B19u, 0xDB8CA0C3u, 0x34BCCBDAu, 0xB2F53777u, 0x5DC55C6Eu, 0x697997B4u, 0x8649FCADu,
        0x6006181Fu, 0x8F367306u, 0xBB8AB8DCu, 0x54BAD3C5u, 0xD2F32F68u, 0x3DC34471u, 0x097F8FABu, 0xE64FE4B2u,
        0xC00C303Eu, 0x2F3C5B27u, 0x1B8090FDu, 0xF4B0FBE4u, 0x72F90749u, 0x9DC96C50u, 0xA975A78Au, 0x4645CC93u,
        0xA00A2821u, 0x4F3A4338u, 0x7B8688E2u, 0x94B6E3FBu, 0x12FF1F56u, 0xFDCF744Fu, 0xC973BF95u, 0x2643D48Cu,
        0x85F4168Du, 0x6AC47D94u, 0x5E78B64Eu, 0xB148DD57u, 0x370121FAu, 0xD8314AE3u, 0xEC8D8139u, 0x03BDEA20u,
        0xE5F20E92u, 0x0AC2658Bu, 0x3E7EAE51u, 0xD14EC548u, 0x570739E5u, 0xB83752FCu, 0x8C8B9926u, 0x63BBF23Fu,
        0x45F826B3u, 0xAAC84DAAu, 0x9E748670u, 0x7144ED69u, 0xF70D11C4u, 0x183D7ADDu, 0x2C81B107u, 0xC3B1DA1Eu,
        0x25FE3EACu, 0xCACE55B5u, 0xFE729E6Fu, 0x1142F576u, 0x970B09DBu, 0x783B62C2u, 0x4C87A918u, 0xA3B7C201u,
        0x0E045BEBu, 0xE13430F2u, 0xD588FB28u, 0x3AB89031u, 0xBCF16C9Cu, 0x53C10785u, 0x677DCC5Fu, 0x884DA746u,
        0x6E0243F4u, 0x813228EDu, 0xB58EE337u, 0x5ABE882Eu, 0xDCF77483u, 0x33C71F9Au, 0x077BD440u, 0xE84BBF59u,
        0xCE086BD5u, 0x213800CCu, 0x1584CB16u, 0xFAB4A00Fu, 0x7CFD5CA2u, 0x93CD37BBu, 0xA771FC61u, 0x48419778u,
        0xAE0E73CAu, 0x413E18D3u, 0x7582D309u, 0x9AB2B810u, 0x1CFB44BDu, 0xF3CB2FA4u, 0xC777E47Eu, 0x28478F67u,
        0x8BF04D66u, 0x64C0267Fu, 0x507CEDA5u, 0xBF4C86BCu, 0x39057A11u, 0xD6351108u, 0xE289DAD2u, 0x0DB9B1CBu,
        0xEBF65579u, 0x04C63E60u, 0x307AF5BAu, 0xDF4A9EA3u, 0x5903620Eu, 0xB6330917u, 0x828FC2CDu, 0x6DBFA9D4u,
        0x4BFC7D58u, 0xA4CC1641u, 0x9070DD9Bu, 0x7F40B682u, 0xF9094A2Fu, 0x16392136u, 0x2285EAECu, 0xCDB581F5u,
        0x2BFA6547u, 0xC4CA0E5Eu, 0xF076C584u, 0x1F46AE9Du, 0x990F5230u, 0x763F3929u, 0x4283F2F3u, 0xADB399EAu,
        0x1C08B7D6u, 0xF338DCCFu, 0xC7841715u, 0x28B47C0Cu, 0xAEFD80A1u, 0x41CDEBB8u, 0x75712062u, 0x9A414B7Bu,
        0x7C0EAFC9u, 0x933EC4D0u, 0xA7820F0Au, 0x48B26413u, 0xCEFB98BEu, 0x21CBF3A7u, 0x1577387Du, 0xFA475364u,
        0xDC0487E8u, 0x3334ECF1u, 0x0788272Bu, 0xE8B84C32u, 0x6EF1B09Fu, 0x81C1DB86u, 0xB57D105Cu, 0x5A4D7B45u,
        0xBC029FF7u, 0x5332F4EEu, 0x678E3F34u, 0x88BE542Du, 0x0EF7A880u, 0xE1C7C399u, 0xD57B0843u, 0x3A4B635Au,
        0x99FCA15Bu, 0x76CCCA42u, 0x42700198u, 0xAD406A81u, 0x2B09962Cu, 0xC439FD35u, 0xF08536EFu, 0x1FB55DF6u,
        0xF9FAB944u, 0x16CAD25Du, 0x22761987u, 0xCD46729Eu, 0x4B0F8E33u, 0xA43FE52Au, 0x90832EF0u, 0x7FB345E9u,
        0x59F09165u, 0xB6C0FA7Cu, 0x827C31A6u, 0x6D4C5ABFu, 0xEB05A612u, 0x0435CD0Bu, 0x308906D1u, 0xDFB96DC8u,
        0x39F6897Au, 0xD6C6E263u, 0xE27A29B9u, 0x0D4A42A0u, 0x8B03BE0Du, 0x6433D514u, 0x508F1ECEu, 0xBFBF75D7u,
        0x120CEC3Du, 0xFD3C8724u, 0xC9804CFEu, 0x26B027E7u, 0xA0F9DB4Au, 0x4FC9B053u, 0x7B757B89u, 0x94451090u,
        0x720AF422u, 0x9D3A9F3Bu, 0xA98654E1u, 0x46B63FF8u, 0xC0FFC355u, 0x2FCFA84Cu, 0x1B736396u, 0xF443088Fu,
        0xD200DC03u, 0x3D30B71Au, 0x098C7CC0u, 0xE6BC17D9u, 0x60F5EB74u, 0x8FC5806Du, 0xBB794BB7u, 0x544920AEu,
        0xB206C41Cu, 0x5D36AF05u, 0x698A64DFu, 0x86BA0FC6u, 0x00F3F36Bu, 0xEFC39872u, 0xDB7F53A8u, 0x344F38B1u,
        0x97F8FAB0u, 0x78C891A9u, 0x4C745A73u, 0xA344316Au, 0x250DCDC7u, 0xCA3DA6DEu, 0xFE816D04u, 0x11B1061Du,
        0xF7FEE2AFu, 0x18CE89B6u, 0x2C72426Cu, 0xC3422975u, 0x450BD5D8u, 0xAA3BBEC1u, 0x9E87751Bu, 0x71B71E02u,
        0x57F4CA8Eu, 0xB8C4A197u, 0x8C786A4Du, 0x63480154u, 0xE501FDF9u, 0x0A3196E0u, 0x3E8D5D3Au, 0xD1BD3623u,
        0x37F2D291u, 0xD8C2B988u, 0xEC7E7252u, 0x034E194Bu, 0x8507E5E6u, 0x6A378EFFu, 0x5E8B4525u, 0xB1BB2E3Cu
    },
    {
        0x00000000u, 0x68032CC8u, 0xD0065990u, 0xB8057558u, 0xA5E0C5D1u, 0xCDE3E919u, 0x75E69C41u, 0x1DE5B089u,
        0x4E2DFD53u, 0x262ED19Bu, 0x9E2BA4C3u, 0xF628880Bu, 0xEBCD3882u, 0x83CE144Au, 0x3BCB6112u, 0x53C84DDAu,
        0x9C5BFAA6u, 0xF458D66Eu, 0x4C5DA336u, 0x245E8FFEu, 0x39BB3F77u, 0x51B813BFu, 0xE9BD66E7u, 0x81BE4A2Fu,
        0xD27607F5u, 0xBA752B3Du, 0x02705E65u, 0x6A7372ADu, 0x7796C224u, 0x1F95EEECu, 0xA7909BB4u, 0xCF93B77Cu,
        0x3D5B83BDu, 0x5558AF75u, 0xED5DDA2Du, 0x855EF6E5u, 0x98BB466Cu, 0xF0B86AA4u, 0x48BD1FFCu, 0x20BE3334u,
        0x73767EEEu, 0x1B755226u, 0xA370277Eu, 0xCB730BB6u, 0xD696BB3Fu, 0xBE9597F7u, 0x0690E2AFu, 0x6E93CE67u,
        0xA100791Bu, 0xC90355D3u, 0x7106208Bu, 0x19050C43u, 0x04E0BCCAu, 0x6CE39002u, 0xD4E6E55Au, 0xBCE5C992u,
        0xEF2D8448u, 0x872EA880u, 0x3F2BDDD8u, 0x5728F110u, 0x4ACD4199u, 0x22CE6D51u, 0x9ACB1809u, 0xF2C834C1u,
        0x7AB7077Au, 0x12B42BB2u, 0xAAB15EEAu, 0xC2B27222u, 0xDF57C2ABu, 0xB754EE63u, 0x0F519B3Bu, 0x6752B7F3u,
        0x349AFA29u, 0x5C99D6E1u, 0xE49CA3B9u, 0x8C9F8F71u, 0x917A3FF8u, 0xF9791330u, 0x417C6668u, 0x297F4AA0u,
        0xE6ECFDDCu, 0x8EEFD114u, 0x36EAA44Cu, 0x5EE98884u, 0x430C380Du, 0x2B0F14C5u, 0x930A619Du, 0xFB094D55u,
        0xA8C1008Fu, 0xC0C22C47u, 0x78C7591Fu, 0x10C475D7u, 0x0D21C55Eu, 0x6522E996u, 0xDD279CCEu, 0xB524B006u,
        0x47EC84C7u, 0x2FEFA80Fu, 0x97EADD57u, 0xFFE9F19Fu, 0xE20C4116u, 0x8A0F6DDEu, 0x320A1886u, 0x5A09344Eu,
        0x09C17994u, 0x61C2555Cu, 0xD9C72004u, 0xB1C40CCCu, 0xAC21BC45u, 0xC422908Du, 0x7C27E5D5u, 0x1424C91Du,
        0xDBB77E61u, 0xB3B452A9u, 0x0BB127F1u, 0x63B20B39u, 0x7E57BBB0u, 0x16549778u, 0xAE51E220u, 0xC652CEE8u,
        0x959A8332u, 0xFD99AFFAu, 0x459CDAA2u, 0x2D9FF66Au, 0x307A46E3u, 0x58796A2Bu, 0xE07C1F73u, 0x887F33BBu,
        0xF56E0EF4u, 0x9D6D223Cu, 0x25685764u, 0x4D6B7BACu, 0x508ECB25u, 0x388DE7EDu, 0x808892B5u, 0xE88BBE7Du,
        0xBB43F3A7u, 0xD340DF6Fu, 0x6B45AA37u, 0x034686FFu, 0x1EA33676u, 0x76A01ABEu, 0xCEA56FE6u, 0xA6A6432Eu,
        0x6935F452u, 0x0136D89Au, 0xB933ADC2u, 0xD130810Au, 0xCCD53183u, 0xA4D61D4Bu, 0x1CD36813u, 0x74D044DBu,
        0x27180901u, 0x4F1B25C9u, 0xF71E5091u, 0x9F1D7C59u, 0x82F8CCD0u, 0xEAFBE018u, 0x52FE9540u, 0x3AFDB988u,
        0xC8358D49u, 0xA036A181u, 0x1833D4D9u, 0x7030F811u, 0x6DD54898u, 0x05D66450u, 0xBDD31108u, 0xD5D03DC0u,
        0x8618701Au, 0xEE1B5CD2u, 0x561E298Au, 0x3E1D0542u, 0x23F8B5CBu, 0x4BFB9903u, 0xF3FEEC5Bu, 0x9BFDC093u,
        0x546E77EFu, 0x3C6D5B27u, 0x84682E7Fu, 0xEC6B02B7u, 0xF18EB23Eu, 0x998D9EF6u, 0x2188EBAEu, 0x498BC766u,
        0x1A438ABCu, 0x7240A674u, 0xCA45D32Cu, 0xA246FFE4u, 0xBFA34F6Du, 0xD7A063A5u, 0x6FA516FDu, 0x07A63A35u,
        0x8FD9098Eu, 0xE7DA2546u, 0x5FDF501Eu, 0x37DC7CD6u, 0x2A39CC5Fu, 0x423AE097u, 0xFA3F95CFu, 0x923CB907u,
        0xC1F4F4DDu, 0xA9F7D815u, 0x11F2AD4Du, 0x79F18185u, 0x6414310Cu, 0x0C171DC4u, 0xB412689Cu, 0xDC114454u,
        0x1382F328u, 0x7B81DFE0u, 0xC384AAB8u, 0xAB878670u, 0xB66236F9u, 0xDE611A31u, 0x66646F69u, 0x0E6743A1u,
        0x5DAF0E7Bu, 0x35AC22B3u, 0x8DA957EBu, 0xE5AA7B23u, 0xF84FCBAAu, 0x904CE762u, 0x2849923Au, 0x404ABEF2u,
        0xB2828A33u, 0xDA81A6FBu, 0x6284D3A3u, 0x0A87FF6Bu, 0x17624FE2u, 0x7F61632Au, 0xC7641672u, 0xAF673ABAu,
        0xFCAF7760u, 0x94AC5BA8u, 0x2CA92EF0u, 0x44AA0238u, 0x594FB2B1u, 0x314C9E79u, 0x8949EB21u, 0xE14AC7E9u,
        0x2ED97095u, 0x46DA5C5Du, 0xFEDF2905u, 0x96DC05CDu, 0x8B39B544u, 0xE33A998Cu, 0x5B3FECD4u, 0x333CC01Cu,
        0x60F48DC6u, 0x08F7A10Eu, 0xB0F2D456u, 0xD8F1F89Eu, 0xC5144817u, 0xAD1764DFu, 0x15121187u, 0x7D113D4Fu
    },
    {
        0x00000000u, 0x493C7D27u, 0x9278FA4Eu, 0xDB448769u, 0x211D826Du, 0x6821FF4Au, 0xB3657823u, 0xFA590504u,
        0x423B04DAu, 0x0B0779FDu, 0xD043FE94u, 0x997F83B3u, 0x632686B7u, 0x2A1AFB90u, 0xF15E7CF9u, 0xB86201DEu,
        0x847609B4u, 0xCD4A7493u, 0x160EF3FAu, 0x5F328EDDu, 0xA56B8BD9u, 0xEC57F6FEu, 0x37137197u, 0x7E2F0CB0u,
        0xC64D0D6Eu, 0x8F717049u, 0x5435F720u, 0x1D098A07u, 0xE7508F03u, 0xAE6CF224u, 0x7528754Du, 0x3C14086Au,
        0x0D006599u, 0x443C18BEu, 0x9F789FD7u, 0xD644E2F0u, 0x2C1DE7F4u, 0x65219AD3u, 0xBE651DBAu, 0xF759609Du,
        0x4F3B6143u, 0x06071C64u, 0xDD439B0Du, 0x947FE62Au, 0x6E26E32Eu, 0x271A9E09u, 0xFC5E1960u, 0xB5626447u,
        0x89766C2Du, 0xC04A110Au, 0x1B0E9663u, 0x5232EB44u, 0xA86BEE40u, 0xE1579367u, 0x3A13140Eu, 0x732F6929u,
        0xCB4D68F7u, 0x827115D0u, 0x593592B9u, 0x1009EF9Eu, 0xEA50EA9Au, 0xA36C97BDu, 0x782810D4u, 0x31146DF3u,
        0x1A00CB32u, 0x533CB615u, 0x8878317Cu, 0xC1444C5Bu, 0x3B1D495Fu, 0x72213478u, 0xA965B311u, 0xE059CE36u,
        0x583BCFE8u, 0x1107B2CFu, 0xCA4335A6u, 0x837F4881u, 0x79264D85u, 0x301A30A2u, 0xEB5EB7CBu, 0xA262CAECu,
        0x9E76C286u, 0xD74ABFA1u, 0x0C0E38C8u, 0x453245EFu, 0xBF6B40EBu, 0xF6573DCCu, 0x2D13BAA5u, 0x642FC782u,
        0xDC4DC65Cu, 0x9571BB7Bu, 0x4E353C12u, 0x07094135u, 0xFD504431u, 0xB46C3916u, 0x6F28BE7Fu, 0x2614C358u,
        0x1700AEABu, 0x5E3CD38Cu, 0x857854E5u, 0xCC4429C2u, 0x361D2CC6u, 0x7F2151E1u, 0xA465D688u, 0xED59ABAFu,
        0x553BAA71u, 0x1C07D756u, 0xC743503Fu, 0x8E7F2D18u, 0x7426281Cu, 0x3D1A553Bu, 0xE65ED252u, 0xAF62AF75u,
        0x9376A71Fu, 0xDA4ADA38u, 0x010E5D51u, 0x48322076u, 0xB26B2572u, 0xFB575855u, 0x2013DF3Cu, 0x692FA21Bu,
        0xD14DA3C5u, 0x9871DEE2u, 0x4335598Bu, 0x0A0924ACu, 0xF05021A8u, 0xB96C5C8Fu, 0x6228DBE6u, 0x2B14A6C1u,
        0x34019664u, 0x7D3DEB43u, 0xA6796C2Au, 0xEF45110Du, 0x151C1409u, 0x5C20692Eu, 0x8764EE47u, 0xCE589360u,
        0x763A92BEu, 0x3F06EF99u, 0xE44268F0u, 0xAD7E15D7u, 0x572710D3u, 0x1E1B6DF4u, 0xC55FEA9Du, 0x8C6397BAu,
        0xB0779FD0u, 0xF94BE2F7u, 0x220F659Eu, 0x6B3318B9u, 0x916A1DBDu, 0xD856609Au, 0x0312E7F3u, 0x4A2E9AD4u,
        0xF24C9B0Au, 0xBB70E62Du, 0x60346144u, 0x29081C63u, 0xD3511967u, 0x9A6D6440u, 0x4129E329u, 0x08159E0Eu,
        0x3901F3FDu, 0x703D8EDAu, 0xAB7909B3u, 0xE2457494u, 0x181C7190u, 0x51200CB7u, 0x8A648BDEu, 0xC358F6F9u,
        0x7B3AF727u, 0x32068A00u, 0xE9420D69u, 0xA07E704Eu, 0x5A27754Au, 0x131B086Du, 0xC85F8F04u, 0x8163F223u,
        0xBD77FA49u, 0xF44B876Eu, 0x2F0F0007u, 0x66337D20u, 0x9C6A7824u, 0xD5560503u, 0x0E12826Au, 0x472EFF4Du,
        0xFF4CFE93u, 0xB67083B4u, 0x6D3404DDu, 0x240879FAu, 0xDE517CFEu, 0x976D01D9u, 0x4C2986B0u, 0x0515FB97u,
        0x2E015D56u, 0x673D2071u, 0xBC79A718u, 0xF545DA3Fu, 0x0F1CDF3Bu, 0x4620A21Cu, 0x9D642575u, 0xD4585852u,
        0x6C3A598Cu, 0x250624ABu, 0xFE42A3C2u, 0xB77EDEE5u, 0x4D27DBE1u, 0x041BA6C6u, 0xDF5F21AFu, 0x96635C88u,
        0xAA7754E2u, 0xE34B29C5u, 0x380FAEACu, 0x7133D38Bu, 0x8B6AD68Fu, 0xC256ABA8u, 0x19122CC1u, 0x502E51E6u,
        0xE84C5038u, 0xA1702D1Fu, 0x7A34AA76u, 0x3308D751u, 0xC951D255u, 0x806DAF72u, 0x5B29281Bu, 0x1215553Cu,
        0x230138CFu, 0x6A3D45E8u, 0xB179C281u, 0xF845BFA6u, 0x021CBAA2u, 0x4B20C785u, 0x906440ECu, 0xD9583DCBu,
        0x613A3C15u, 0x28064132u, 0xF342C65Bu, 0xBA7EBB7Cu, 0x4027BE78u, 0x091BC35Fu, 0xD25F4436u, 0x9B633911u,
        0xA777317Bu, 0xEE4B4C5Cu, 0x350FCB35u, 0x7C33B612u, 0x866AB316u, 0xCF56CE31u, 0x14124958u, 0x5D2E347Fu,
        0xE54C35A1u, 0xAC704886u, 0x7734CFEFu, 0x3E08B2C8u, 0xC451B7CCu, 0x8D6DCAEBu, 0x56294D82u, 0x1F1530A5u
    }
};
#endif

static uint32_t cryp_crc_slice8(const uint32_t t[8][256], uint32_t crc, const unsigned char* p, size_t len) {
    crc = ~crc;
    while (len >= 8) {
        uint32_t one = cryp_read32(p) ^ crc;
        uint32_t two = cryp_read32(p + 4);
        crc = t[7][one & 0xFFu] ^ t[6][(one >> 8) & 0xFFu] ^ t[5][(one >> 16) & 0xFFu] ^ t[4][one >> 24] ^
              t[3][two & 0xFFu] ^ t[2][(two >> 8) & 0xFFu] ^ t[1][(two >> 16) & 0xFFu] ^ t[0][two >> 24];
        p += 8;
        len -= 8;
    }
    while (len--) {
        crc = (crc >> 8) ^ t[0][(crc ^ *p++) & 0xFFu];
    }
    return ~crc;
}

/**
 * @brief Continues a CRC-32 (IEEE 802.3: the checksum of zlib, gzip, PNG,
 *        ZIP and Ethernet) over `length` more bytes. Start from 0: feeding
 *        the data in pieces gives the same result as at once, and the value
 *        equals zlib's crc32(crc, data, length) and Python's zlib.crc32.
 */
uint32_t crypto_crc32_update(uint32_t crc, const void* data, size_t length) {
    if (data == NULL) {
        return crc;
    }
    return cryp_crc_slice8(cryp_crc32_table, crc, (const unsigned char*)data, length);
}

/** @brief CRC-32 (zlib / PNG / gzip) of `length` bytes. */
uint32_t crypto_crc32(const void* data, size_t length) {
    return crypto_crc32_update(0, data, length);
}

/**
 * @brief Continues a CRC-32C (Castagnoli: iSCSI, ext4, Btrfs, SCTP,
 *        LevelDB / RocksDB) over `length` more bytes; start from 0. Uses the
 *        CPU's crc32 instruction when the build targets SSE4.2
 *        (-msse4.2 / -march=native / /arch:AVX), else slicing-by-8 tables.
 */
uint32_t crypto_crc32c_update(uint32_t crc, const void* data, size_t length) {
    if (data == NULL) {
        return crc;
    }
    const unsigned char* p = (const unsigned char*)data;
#if defined(CRYP_HAVE_SSE42_CRC)
    uint32_t c = ~crc;
#  if defined(__x86_64__) || defined(_M_X64)
    uint64_t c64 = c;
    while (length >= 8) {
        c64 = _mm_crc32_u64(c64, cryp_read64(p));
        p += 8;
        length -= 8;
    }
    c = (uint32_t)c64;
#  endif
    while (length >= 4) {
        c = _mm_crc32_u32(c, cryp_read32(p));
        p += 4;
        length -= 4;
    }
    while (length--) {
        c = _mm_crc32_u8(c, *p++);
    }
    return ~c;
#else
    return cryp_crc_slice8(cryp_crc32c_table, crc, p, length);
#endif
}

/** @brief CRC-32C (Castagnoli) of `length` bytes. */
uint32_t crypto_crc32c(const void* data, size_t length) {
    return crypto_crc32c_update(0, data, length);
}
