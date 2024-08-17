/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Crypto
*/

#include "crypto.h"
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
        printf("No hash data to print.\n");
        return;
    }

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
    if (!out_len || !key || !plaintext) return NULL;

    // Pad length to 8 byte boundary
    size_t padded_len = (len + DES_BLOCK_SIZE - 1) & ~(DES_BLOCK_SIZE - 1);
    *out_len = padded_len;
    uint8_t* padded_plaintext = calloc(padded_len, sizeof(uint8_t));
    if (!padded_plaintext) return NULL;
    memcpy(padded_plaintext, plaintext, len);

    uint8_t* ciphertext = malloc(padded_len);
    if (!ciphertext) {
        free(padded_plaintext);
        return NULL;
    }

    DES_cblock keyBlock;
    memcpy(keyBlock, key, DES_BLOCK_SIZE);
    DES_key_schedule keysched;
    DES_set_odd_parity(&keyBlock);
    DES_set_key_checked(&keyBlock, &keysched);

    DES_cblock ivec;
    if (iv) {
        memcpy(ivec, iv, DES_BLOCK_SIZE);
    } 
    else {
        memset(ivec, 0, DES_BLOCK_SIZE); // Default IV to all zeros if none provided
    }

    for (size_t i = 0; i < padded_len; i += DES_BLOCK_SIZE) {
        DES_cblock block;
        memcpy(block, padded_plaintext + i, DES_BLOCK_SIZE);
        switch (mode) {
            case CRYPTO_MODE_ECB:
                DES_ecb_encrypt(&block, (DES_cblock*)(ciphertext + i), &keysched, DES_ENCRYPT);
                break;
            case CRYPTO_MODE_CBC:
                DES_ncbc_encrypt(block, ciphertext + i, DES_BLOCK_SIZE, &keysched, &ivec, DES_ENCRYPT);
                break;
            case CRYPTO_MODE_CFB:{
                // Initialize IV to all zeros
                DES_cblock ivec = {0};
                DES_cfb_encrypt(padded_plaintext, ciphertext, 8, padded_len, &keysched, &ivec, DES_ENCRYPT);
            }
            break;
            case CRYPTO_MODE_OFB:
                DES_ofb_encrypt(block, ciphertext + i, 8, DES_BLOCK_SIZE, &keysched, &ivec);
                break;
            default:
                free(ciphertext);
                free(padded_plaintext);
                return NULL;
        }
    }

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
    if (!out_len || !key || !ciphertext) return NULL;

    // In decryption, padding isn't removed here. It should be handled after decryption.
    *out_len = len;
    uint8_t* plaintext = malloc(len);
    if (!plaintext) return NULL;

    DES_cblock keyBlock;
    memcpy(keyBlock, key, DES_BLOCK_SIZE);
    DES_key_schedule keysched;
    DES_set_odd_parity(&keyBlock);
    DES_set_key_checked(&keyBlock, &keysched);

    DES_cblock ivec;
    if (iv) {
        memcpy(ivec, iv, DES_BLOCK_SIZE);
    } 
    else {
        memset(ivec, 0, DES_BLOCK_SIZE); // Default IV to all zeros if none provided
    }

    for (size_t i = 0; i < len; i += DES_BLOCK_SIZE) {
        DES_cblock block;
        memcpy(block, ciphertext + i, DES_BLOCK_SIZE);
        switch (mode) {
            case CRYPTO_MODE_ECB:
                DES_ecb_encrypt(&block, (DES_cblock*)(plaintext + i), &keysched, DES_DECRYPT);
                break;
            case CRYPTO_MODE_CBC:
                DES_ncbc_encrypt(block, plaintext + i, DES_BLOCK_SIZE, &keysched, &ivec, DES_DECRYPT);
                break;
            case CRYPTO_MODE_CFB:{
                // For CFB, decryption is the same as encryption
                DES_cfb_encrypt(ciphertext, plaintext, 8, len, &keysched, &ivec, DES_ENCRYPT);
            }
            break;
            case CRYPTO_MODE_OFB:
                // OFB mode decryption is the same as encryption
                DES_ofb_encrypt(block, plaintext + i, 8, DES_BLOCK_SIZE, &keysched, &ivec);
                break;
            default:
                free(plaintext);
                return NULL;
        }
    }

    return plaintext; // Caller is responsible for freeing this memory
}

#if defined(_WIN32) || defined(_WIN64)
/**
 * @brief Generates a random Initialization Vector (IV) using Windows Cryptographic API.
 *
 * This function generates a random IV using the Windows Cryptographic API, specifically
 * the CryptAcquireContext and CryptGenRandom functions.
 *
 * @param iv Pointer to a buffer where the generated IV will be stored.
 * @param length The length of the IV to generate, in bytes.
 *
 * @note This function will terminate the program if it fails to acquire a cryptographic context
 *       or generate random data.
 */
#include <Windows.h>
#include <Wincrypt.h>

void crypto_generate_random_iv(uint8_t *iv, size_t length) {
    HCRYPTPROV hCryptProv;
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        fprintf(stderr, "CryptAcquireContext failed: %lu\n", GetLastError());
        exit(EXIT_FAILURE);
    }
    if (!CryptGenRandom(hCryptProv, length, iv)) {
        fprintf(stderr, "CryptGenRandom failed: %lu\n", GetLastError());
        CryptReleaseContext(hCryptProv, 0);
        exit(EXIT_FAILURE);
    }
    CryptReleaseContext(hCryptProv, 0);
}

#else
/**
 * @brief Generates a random Initialization Vector (IV) using the Unix-like system's `/dev/urandom` source.
 *
 * This function generates a random IV by reading from the `/dev/urandom` file, which provides
 * random data generated by the operating system.
 *
 * @param iv Pointer to a buffer where the generated IV will be stored.
 * @param length The length of the IV to generate, in bytes.
 *
 * @note This function will terminate the program if it fails to open `/dev/urandom` or read the requested
 *       number of bytes.
 */
#include <fcntl.h>
#include <unistd.h>

void crypto_generate_random_iv(uint8_t *iv, size_t length) {
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        perror("open /dev/urandom");
        exit(EXIT_FAILURE);
    }
    ssize_t read_bytes = read(fd, iv, length);
    if (read_bytes != (ssize_t)length) {
        perror("read from /dev/urandom");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}

#endif
