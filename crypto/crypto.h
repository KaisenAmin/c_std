/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Crypto
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in crypto.c (file-level convention).
 * The exception is the static inline implementation of the
 * non-cryptographic hashes at the end of this file, which other modules
 * (hashmap) use without linking crypto or OpenSSL.
 *
 * Hash, HMAC, PBKDF2 and a small DES symmetric-cipher pair built on
 * OpenSSL EVP, plus fast non-cryptographic hashes and checksums (FNV-1a,
 * MurmurHash3, xxHash, CRC-32 / CRC-32C) in plain C.
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
    #define CRYPTO_LOG(...) \
        do { fprintf(stderr, "[CRYPTO LOG] "); fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)
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
    CRYPTO_MODE_OFB,
    CRYPTO_MODE_CTR        /* counter mode (AES only; a stream mode, no padding) */
} CryptoMode;

/* Recommended sizes for the AEAD helpers below. */
#define CRYPTO_AES_BLOCK_SIZE   16
#define CRYPTO_GCM_IV_SIZE      12   /* 96-bit nonce: the GCM-preferred IV length */
#define CRYPTO_GCM_TAG_SIZE     16   /* full 128-bit authentication tag           */
#define CRYPTO_CHACHA_KEY_SIZE  32
#define CRYPTO_CHACHA_IV_SIZE   12
#define CRYPTO_CHACHA_TAG_SIZE  16


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
/* AES — modern symmetric cipher (prefer this over DES)               */
/* ------------------------------------------------------------------ */
uint8_t*     crypto_aes_encrypt                (const uint8_t* plaintext, size_t len, const uint8_t* key, size_t key_len, const uint8_t* iv, CryptoMode mode, size_t* out_len);
uint8_t*     crypto_aes_decrypt                (const uint8_t* ciphertext, size_t len, const uint8_t* key, size_t key_len, const uint8_t* iv, CryptoMode mode, size_t* out_len);


/* ------------------------------------------------------------------ */
/* Authenticated encryption (AEAD): confidentiality + integrity       */
/* ------------------------------------------------------------------ */

uint8_t*     crypto_aes_gcm_encrypt            (const uint8_t* plaintext, size_t len, const uint8_t* key, size_t key_len,
                                                const uint8_t* iv, size_t iv_len, const uint8_t* aad, size_t aad_len,
                                                uint8_t* tag, size_t tag_len, size_t* out_len);
uint8_t*     crypto_aes_gcm_decrypt            (const uint8_t* ciphertext, size_t len, const uint8_t* key, size_t key_len,
                                                const uint8_t* iv, size_t iv_len, const uint8_t* aad, size_t aad_len,
                                                const uint8_t* tag, size_t tag_len, size_t* out_len);

uint8_t*     crypto_chacha20_poly1305_encrypt  (const uint8_t* plaintext, size_t len, const uint8_t* key, size_t key_len,
                                                const uint8_t* iv, size_t iv_len, const uint8_t* aad, size_t aad_len,
                                                uint8_t* tag, size_t tag_len, size_t* out_len);
uint8_t*     crypto_chacha20_poly1305_decrypt  (const uint8_t* ciphertext, size_t len, const uint8_t* key, size_t key_len,
                                                const uint8_t* iv, size_t iv_len, const uint8_t* aad, size_t aad_len,
                                                const uint8_t* tag, size_t tag_len, size_t* out_len);


/* ------------------------------------------------------------------ */
/* Password-based encryption (PBKDF2-HMAC-SHA256 + AES-256-GCM)       */
/* ------------------------------------------------------------------ */

uint8_t*     crypto_password_encrypt           (const uint8_t* plaintext, size_t len, const char* password, int iterations, size_t* out_len);
uint8_t*     crypto_password_decrypt           (const uint8_t* blob, size_t blob_len, const char* password, size_t* out_len);


/* ------------------------------------------------------------------ */
/* Random                                                             */
/* ------------------------------------------------------------------ */

bool         crypto_random_bytes               (uint8_t* buffer, size_t length);
bool         crypto_generate_random_iv         (uint8_t* iv, size_t length);


/* ------------------------------------------------------------------ */
/* Misc                                                               */
/* ------------------------------------------------------------------ */

bool         crypto_constant_time_equal        (const void* a, const void* b, size_t length);
const char*  crypto_hash_algorithm_name        (HashAlgorithm algorithm);
void         crypto_secure_zero                (void* ptr, size_t length);


/* ------------------------------------------------------------------ */
/* Non-cryptographic hashes and checksums                             */
/* ------------------------------------------------------------------ */
/* Fast hashes for hash tables, checksums, sharding and deduplication -
   NOT for security (collisions can be constructed). Plain C, no OpenSSL;
   the same values on every platform and as the reference implementations
   (zlib crc32, xxHash, MurmurHash3, FNV). */

#define CRYPTO_FNV1A_32_INIT  0x811C9DC5u                      /* FNV-1a offset basis */
#define CRYPTO_FNV1A_64_INIT  UINT64_C(0xCBF29CE484222325)

uint32_t     crypto_fnv1a_32                   (const void* data, size_t length);
uint32_t     crypto_fnv1a_32_update            (uint32_t hash, const void* data, size_t length);
uint64_t     crypto_fnv1a_64                   (const void* data, size_t length);
uint64_t     crypto_fnv1a_64_update            (uint64_t hash, const void* data, size_t length);

uint32_t     crypto_murmur3_32                 (const void* data, size_t length, uint32_t seed);
void         crypto_murmur3_128                (const void* data, size_t length, uint32_t seed, uint64_t out[2]);

uint32_t     crypto_xxh32                      (const void* data, size_t length, uint32_t seed);
uint64_t     crypto_xxh64                      (const void* data, size_t length, uint64_t seed);
uint64_t     crypto_xxh3_64                    (const void* data, size_t length, uint64_t seed);

uint32_t     crypto_crc32                      (const void* data, size_t length);
uint32_t     crypto_crc32_update               (uint32_t crc, const void* data, size_t length);
uint32_t     crypto_crc32c                     (const void* data, size_t length);
uint32_t     crypto_crc32c_update              (uint32_t crc, const void* data, size_t length);


/* ------------------------------------------------------------------ */
/* Debug                                                              */
/* ------------------------------------------------------------------ */

void         crypto_print_hash                 (const uint8_t* hash, size_t length);


#ifdef __cplusplus
}
#endif


/* ================================================================== */
/* Implementation of the non-cryptographic hashes (internal)          */
/* ================================================================== */
/*
 * FNV-1a, MurmurHash3 and xxHash (XXH32, XXH64, XXH3-64) are implemented
 * here as static inline functions with the internal prefix `cryp_`: they
 * use nothing from crypto.c and no OpenSSL, so other modules can use them
 * by including this header alone, without linking the crypto module -
 * hashmap_hash_string is cryp_xxh3_64. Call the public crypto_* functions
 * declared above (crypto.c wraps these); the cryp_* names are not part of
 * the API. CRC-32 / CRC-32C (with their tables) live in crypto.c.
 *
 * Inputs are read as little-endian words on every CPU, so the results are
 * the same everywhere, and unaligned input is fine. With
 * CRYPTO_HASH_PORTABLE defined only plain C is used (no SSE2 / AVX2 XXH3
 * loop, no 128-bit multiply intrinsics); the results do not change.
 * Kept outside the extern "C" block: the intrinsics headers are C++-aware.
 */

#include <string.h>   /* memcpy */

#if !defined(CRYPTO_HASH_PORTABLE) && \
    ((defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || \
     defined(_M_X64) || defined(_M_IX86) || defined(_M_ARM64) || defined(_M_ARM))
#  define CRYP_LITTLE_ENDIAN 1
#endif

#if !defined(CRYPTO_HASH_PORTABLE) && defined(_MSC_VER) && (defined(_M_X64) || defined(_M_ARM64))
#  include <intrin.h>   /* _umul128 / __umulh */
#endif

/* Stands in for a NULL input of length 0: pointer arithmetic on NULL (even
   NULL + 0) is undefined behaviour in C. */
static const unsigned char cryp_empty[1] = { 0 };

static inline uint32_t cryp_read32(const unsigned char* p) {
#if defined(CRYP_LITTLE_ENDIAN)
    uint32_t v;
    memcpy(&v, p, sizeof v);           /* one unaligned load */
    return v;
#else
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
#endif
}

static inline uint64_t cryp_read64(const unsigned char* p) {
#if defined(CRYP_LITTLE_ENDIAN)
    uint64_t v;
    memcpy(&v, p, sizeof v);
    return v;
#else
    return (uint64_t)cryp_read32(p) | ((uint64_t)cryp_read32(p + 4) << 32);
#endif
}

static inline uint32_t cryp_rotl32(uint32_t x, unsigned r) {
    return (x << r) | (x >> (32u - r));          /* r is a constant in 1..31 */
}

static inline uint64_t cryp_rotl64(uint64_t x, unsigned r) {
    return (x << r) | (x >> (64u - r));          /* r is a constant in 1..63 */
}

static inline uint32_t cryp_swap32(uint32_t x) {
    return ((x << 24) & 0xFF000000u) | ((x << 8) & 0x00FF0000u) | ((x >> 8) & 0x0000FF00u) | (x >> 24);
}

static inline uint64_t cryp_swap64(uint64_t x) {
    return ((uint64_t)cryp_swap32((uint32_t)x) << 32) | (uint64_t)cryp_swap32((uint32_t)(x >> 32));
}

/* 64 x 64 -> 128-bit product, folded to 64 bits (low ^ high). */
static inline uint64_t cryp_mul128_fold64(uint64_t a, uint64_t b) {
#if !defined(CRYPTO_HASH_PORTABLE) && defined(__SIZEOF_INT128__)
    __extension__ typedef unsigned __int128 cryp_u128;
    cryp_u128 p = (cryp_u128)a * b;
    return (uint64_t)p ^ (uint64_t)(p >> 64);
#elif !defined(CRYPTO_HASH_PORTABLE) && defined(_MSC_VER) && defined(_M_X64)
    uint64_t hi;
    uint64_t lo = _umul128(a, b, &hi);
    return lo ^ hi;
#elif !defined(CRYPTO_HASH_PORTABLE) && defined(_MSC_VER) && defined(_M_ARM64)
    return (a * b) ^ __umulh(a, b);
#else
    uint64_t lo_lo = (a & 0xFFFFFFFFu) * (b & 0xFFFFFFFFu);
    uint64_t hi_lo = (a >> 32) * (b & 0xFFFFFFFFu);
    uint64_t lo_hi = (a & 0xFFFFFFFFu) * (b >> 32);
    uint64_t hi_hi = (a >> 32) * (b >> 32);
    uint64_t cross = (lo_lo >> 32) + (hi_lo & 0xFFFFFFFFu) + lo_hi;
    uint64_t upper = (hi_lo >> 32) + (cross >> 32) + hi_hi;
    uint64_t lower = (cross << 32) | (lo_lo & 0xFFFFFFFFu);
    return lower ^ upper;
#endif
}


/* ------------------------------------------------------------------ */
/* FNV-1a                                                             */
/* ------------------------------------------------------------------ */

/* FNV-1a 32-bit: continue `hash` over `length` bytes (NULL data = no bytes). */
static inline uint32_t cryp_fnv1a_32_update(uint32_t hash, const void* data, size_t length) {
    const unsigned char* p = (const unsigned char*)data;
    if (p == NULL) {
        return hash;
    }
    for (size_t i = 0; i < length; ++i) {
        hash ^= p[i];
        hash *= 16777619u;                       /* FNV prime 2^24 + 2^8 + 0x93 */
    }
    return hash;
}

/* FNV-1a 64-bit: continue `hash` over `length` bytes (NULL data = no bytes). */
static inline uint64_t cryp_fnv1a_64_update(uint64_t hash, const void* data, size_t length) {
    const unsigned char* p = (const unsigned char*)data;
    if (p == NULL) {
        return hash;
    }
    for (size_t i = 0; i < length; ++i) {
        hash ^= p[i];
        hash *= UINT64_C(1099511628211);         /* FNV prime 2^40 + 2^8 + 0xB3 */
    }
    return hash;
}


/* ------------------------------------------------------------------ */
/* MurmurHash3 (Austin Appleby)                                       */
/* ------------------------------------------------------------------ */

static inline uint32_t cryp_fmix32(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85EBCA6Bu;
    h ^= h >> 13;
    h *= 0xC2B2AE35u;
    h ^= h >> 16;
    return h;
}

static inline uint64_t cryp_fmix64(uint64_t k) {
    k ^= k >> 33;
    k *= UINT64_C(0xFF51AFD7ED558CCD);
    k ^= k >> 33;
    k *= UINT64_C(0xC4CEB9FE1A85EC53);
    k ^= k >> 33;
    return k;
}

/* MurmurHash3_x86_32. */
static inline uint32_t cryp_murmur3_32(const void* data, size_t length, uint32_t seed) {
    const unsigned char* p = (const unsigned char*)data;
    if (p == NULL) {
        p = cryp_empty;          /* NULL + 0 is undefined behaviour in C */
        length = 0;
    }
    const uint32_t c1 = 0xCC9E2D51u, c2 = 0x1B873593u;
    uint32_t h1 = seed;
    size_t nblocks = length / 4;
    for (size_t i = 0; i < nblocks; ++i) {
        uint32_t k1 = cryp_read32(p + 4 * i);
        k1 *= c1;
        k1 = cryp_rotl32(k1, 15);
        k1 *= c2;
        h1 ^= k1;
        h1 = cryp_rotl32(h1, 13);
        h1 = h1 * 5u + 0xE6546B64u;
    }
    const unsigned char* tail = p + 4 * nblocks;
    uint32_t k1 = 0;
    switch (length & 3u) {
        case 3: k1 ^= (uint32_t)tail[2] << 16;   /* fall through */
        case 2: k1 ^= (uint32_t)tail[1] << 8;    /* fall through */
        case 1: k1 ^= (uint32_t)tail[0];
                k1 *= c1;
                k1 = cryp_rotl32(k1, 15);
                k1 *= c2;
                h1 ^= k1;
                break;
        default: break;
    }
    h1 ^= (uint32_t)length;
    return cryp_fmix32(h1);
}

/* MurmurHash3_x64_128: out[0] = low 64 bits, out[1] = high 64 bits. */
static inline void cryp_murmur3_128(const void* data, size_t length, uint32_t seed, uint64_t out[2]) {
    if (out == NULL) {
        return;
    }
    const unsigned char* p = (const unsigned char*)data;
    if (p == NULL) {
        p = cryp_empty;          /* NULL + 0 is undefined behaviour in C */
        length = 0;
    }
    const uint64_t c1 = UINT64_C(0x87C37B91114253D5), c2 = UINT64_C(0x4CF5AD432745937F);
    uint64_t h1 = seed, h2 = seed;
    size_t nblocks = length / 16;
    for (size_t i = 0; i < nblocks; ++i) {
        uint64_t k1 = cryp_read64(p + 16 * i);
        uint64_t k2 = cryp_read64(p + 16 * i + 8);
        k1 *= c1; k1 = cryp_rotl64(k1, 31); k1 *= c2; h1 ^= k1;
        h1 = cryp_rotl64(h1, 27); h1 += h2; h1 = h1 * 5u + 0x52DCE729u;
        k2 *= c2; k2 = cryp_rotl64(k2, 33); k2 *= c1; h2 ^= k2;
        h2 = cryp_rotl64(h2, 31); h2 += h1; h2 = h2 * 5u + 0x38495AB5u;
    }
    const unsigned char* tail = p + 16 * nblocks;
    uint64_t k1 = 0, k2 = 0;
    switch (length & 15u) {
        case 15: k2 ^= (uint64_t)tail[14] << 48;  /* fall through */
        case 14: k2 ^= (uint64_t)tail[13] << 40;  /* fall through */
        case 13: k2 ^= (uint64_t)tail[12] << 32;  /* fall through */
        case 12: k2 ^= (uint64_t)tail[11] << 24;  /* fall through */
        case 11: k2 ^= (uint64_t)tail[10] << 16;  /* fall through */
        case 10: k2 ^= (uint64_t)tail[9] << 8;    /* fall through */
        case 9:  k2 ^= (uint64_t)tail[8];
                 k2 *= c2; k2 = cryp_rotl64(k2, 33); k2 *= c1; h2 ^= k2;
                 /* fall through */
        case 8:  k1 ^= (uint64_t)tail[7] << 56;   /* fall through */
        case 7:  k1 ^= (uint64_t)tail[6] << 48;   /* fall through */
        case 6:  k1 ^= (uint64_t)tail[5] << 40;   /* fall through */
        case 5:  k1 ^= (uint64_t)tail[4] << 32;   /* fall through */
        case 4:  k1 ^= (uint64_t)tail[3] << 24;   /* fall through */
        case 3:  k1 ^= (uint64_t)tail[2] << 16;   /* fall through */
        case 2:  k1 ^= (uint64_t)tail[1] << 8;    /* fall through */
        case 1:  k1 ^= (uint64_t)tail[0];
                 k1 *= c1; k1 = cryp_rotl64(k1, 31); k1 *= c2; h1 ^= k1;
                 break;
        default: break;
    }
    h1 ^= (uint64_t)length;
    h2 ^= (uint64_t)length;
    h1 += h2;
    h2 += h1;
    h1 = cryp_fmix64(h1);
    h2 = cryp_fmix64(h2);
    h1 += h2;
    h2 += h1;
    out[0] = h1;
    out[1] = h2;
}


/* ------------------------------------------------------------------ */
/* xxHash (Yann Collet): XXH32, XXH64, XXH3-64                        */
/* ------------------------------------------------------------------ */

#define CRYP_P32_1 0x9E3779B1u
#define CRYP_P32_2 0x85EBCA77u
#define CRYP_P32_3 0xC2B2AE3Du
#define CRYP_P32_4 0x27D4EB2Fu
#define CRYP_P32_5 0x165667B1u
#define CRYP_P64_1 UINT64_C(0x9E3779B185EBCA87)
#define CRYP_P64_2 UINT64_C(0xC2B2AE3D27D4EB4F)
#define CRYP_P64_3 UINT64_C(0x165667B19E3779F9)
#define CRYP_P64_4 UINT64_C(0x85EBCA77C2B2AE63)
#define CRYP_P64_5 UINT64_C(0x27D4EB2F165667C5)

static inline uint32_t cryp_xxh32_round(uint32_t acc, uint32_t input) {
    acc += input * CRYP_P32_2;
    acc = cryp_rotl32(acc, 13);
    return acc * CRYP_P32_1;
}

/* XXH32 (= reference XXH32()). */
static inline uint32_t cryp_xxh32(const void* data, size_t length, uint32_t seed) {
    const unsigned char* p = (const unsigned char*)data;
    if (p == NULL) {
        p = cryp_empty;          /* NULL + 0 is undefined behaviour in C */
        length = 0;
    }
    const unsigned char* end = p + length;
    uint32_t h32;
    if (length >= 16) {
        const unsigned char* limit = end - 16;
        uint32_t v1 = seed + CRYP_P32_1 + CRYP_P32_2;
        uint32_t v2 = seed + CRYP_P32_2;
        uint32_t v3 = seed;
        uint32_t v4 = seed - CRYP_P32_1;
        do {
            v1 = cryp_xxh32_round(v1, cryp_read32(p));
            v2 = cryp_xxh32_round(v2, cryp_read32(p + 4));
            v3 = cryp_xxh32_round(v3, cryp_read32(p + 8));
            v4 = cryp_xxh32_round(v4, cryp_read32(p + 12));
            p += 16;
        } while (p <= limit);
        h32 = cryp_rotl32(v1, 1) + cryp_rotl32(v2, 7) + cryp_rotl32(v3, 12) + cryp_rotl32(v4, 18);
    }
    else {
        h32 = seed + CRYP_P32_5;
    }
    h32 += (uint32_t)length;
    while ((size_t)(end - p) >= 4) {
        h32 += cryp_read32(p) * CRYP_P32_3;
        h32 = cryp_rotl32(h32, 17) * CRYP_P32_4;
        p += 4;
    }
    while (p < end) {
        h32 += (uint32_t)(*p) * CRYP_P32_5;
        h32 = cryp_rotl32(h32, 11) * CRYP_P32_1;
        ++p;
    }
    h32 ^= h32 >> 15;
    h32 *= CRYP_P32_2;
    h32 ^= h32 >> 13;
    h32 *= CRYP_P32_3;
    h32 ^= h32 >> 16;
    return h32;
}

static inline uint64_t cryp_xxh64_round(uint64_t acc, uint64_t input) {
    acc += input * CRYP_P64_2;
    acc = cryp_rotl64(acc, 31);
    return acc * CRYP_P64_1;
}

static inline uint64_t cryp_xxh64_merge(uint64_t acc, uint64_t val) {
    acc ^= cryp_xxh64_round(0, val);
    return acc * CRYP_P64_1 + CRYP_P64_4;
}

static inline uint64_t cryp_xxh64_avalanche(uint64_t h) {
    h ^= h >> 33;
    h *= CRYP_P64_2;
    h ^= h >> 29;
    h *= CRYP_P64_3;
    h ^= h >> 32;
    return h;
}

/* XXH64 (= reference XXH64()). */
static inline uint64_t cryp_xxh64(const void* data, size_t length, uint64_t seed) {
    const unsigned char* p = (const unsigned char*)data;
    if (p == NULL) {
        p = cryp_empty;          /* NULL + 0 is undefined behaviour in C */
        length = 0;
    }
    const unsigned char* end = p + length;
    uint64_t h64;
    if (length >= 32) {
        const unsigned char* limit = end - 32;
        uint64_t v1 = seed + CRYP_P64_1 + CRYP_P64_2;
        uint64_t v2 = seed + CRYP_P64_2;
        uint64_t v3 = seed;
        uint64_t v4 = seed - CRYP_P64_1;
        do {
            v1 = cryp_xxh64_round(v1, cryp_read64(p));
            v2 = cryp_xxh64_round(v2, cryp_read64(p + 8));
            v3 = cryp_xxh64_round(v3, cryp_read64(p + 16));
            v4 = cryp_xxh64_round(v4, cryp_read64(p + 24));
            p += 32;
        } while (p <= limit);
        h64 = cryp_rotl64(v1, 1) + cryp_rotl64(v2, 7) + cryp_rotl64(v3, 12) + cryp_rotl64(v4, 18);
        h64 = cryp_xxh64_merge(h64, v1);
        h64 = cryp_xxh64_merge(h64, v2);
        h64 = cryp_xxh64_merge(h64, v3);
        h64 = cryp_xxh64_merge(h64, v4);
    }
    else {
        h64 = seed + CRYP_P64_5;
    }
    h64 += (uint64_t)length;
    while ((size_t)(end - p) >= 8) {
        h64 ^= cryp_xxh64_round(0, cryp_read64(p));
        h64 = cryp_rotl64(h64, 27) * CRYP_P64_1 + CRYP_P64_4;
        p += 8;
    }
    if ((size_t)(end - p) >= 4) {
        h64 ^= (uint64_t)cryp_read32(p) * CRYP_P64_1;
        h64 = cryp_rotl64(h64, 23) * CRYP_P64_2 + CRYP_P64_3;
        p += 4;
    }
    while (p < end) {
        h64 ^= (uint64_t)(*p) * CRYP_P64_5;
        h64 = cryp_rotl64(h64, 11) * CRYP_P64_1;
        ++p;
    }
    return cryp_xxh64_avalanche(h64);
}

/* XXH3's default 192-byte secret. */
static const unsigned char cryp_xxh3_secret[192] = {
    0xb8, 0xfe, 0x6c, 0x39, 0x23, 0xa4, 0x4b, 0xbe, 0x7c, 0x01, 0x81, 0x2c, 0xf7, 0x21, 0xad, 0x1c,
    0xde, 0xd4, 0x6d, 0xe9, 0x83, 0x90, 0x97, 0xdb, 0x72, 0x40, 0xa4, 0xa4, 0xb7, 0xb3, 0x67, 0x1f,
    0xcb, 0x79, 0xe6, 0x4e, 0xcc, 0xc0, 0xe5, 0x78, 0x82, 0x5a, 0xd0, 0x7d, 0xcc, 0xff, 0x72, 0x21,
    0xb8, 0x08, 0x46, 0x74, 0xf7, 0x43, 0x24, 0x8e, 0xe0, 0x35, 0x90, 0xe6, 0x81, 0x3a, 0x26, 0x4c,
    0x3c, 0x28, 0x52, 0xbb, 0x91, 0xc3, 0x00, 0xcb, 0x88, 0xd0, 0x65, 0x8b, 0x1b, 0x53, 0x2e, 0xa3,
    0x71, 0x64, 0x48, 0x97, 0xa2, 0x0d, 0xf9, 0x4e, 0x38, 0x19, 0xef, 0x46, 0xa9, 0xde, 0xac, 0xd8,
    0xa8, 0xfa, 0x76, 0x3f, 0xe3, 0x9c, 0x34, 0x3f, 0xf9, 0xdc, 0xbb, 0xc7, 0xc7, 0x0b, 0x4f, 0x1d,
    0x8a, 0x51, 0xe0, 0x4b, 0xcd, 0xb4, 0x59, 0x31, 0xc8, 0x9f, 0x7e, 0xc9, 0xd9, 0x78, 0x73, 0x64,
    0xea, 0xc5, 0xac, 0x83, 0x34, 0xd3, 0xeb, 0xc3, 0xc5, 0x81, 0xa0, 0xff, 0xfa, 0x13, 0x63, 0xeb,
    0x17, 0x0d, 0xdd, 0x51, 0xb7, 0xf0, 0xda, 0x49, 0xd3, 0x16, 0x55, 0x26, 0x29, 0xd4, 0x68, 0x9e,
    0x2b, 0x16, 0xbe, 0x58, 0x7d, 0x47, 0xa1, 0xfc, 0x8f, 0xf8, 0xb8, 0xd1, 0x7a, 0xd0, 0x31, 0xce,
    0x45, 0xcb, 0x3a, 0x8f, 0x95, 0x16, 0x04, 0x28, 0xaf, 0xd7, 0xfb, 0xca, 0xbb, 0x4b, 0x40, 0x7e,
};

#define CRYP_XXH3_STRIPE_LEN   64
#define CRYP_XXH3_SECRET_SIZE  192
#define CRYP_XXH3_MIDSIZE_MAX  240
#define CRYP_PRIME_MX1 UINT64_C(0x165667919E3779F9)
#define CRYP_PRIME_MX2 UINT64_C(0x9FB21C651E98DF25)

static inline uint64_t cryp_xxh3_avalanche(uint64_t h) {
    h ^= h >> 37;
    h *= CRYP_PRIME_MX1;
    h ^= h >> 32;
    return h;
}

static inline uint64_t cryp_xxh3_rrmxmx(uint64_t h, uint64_t len) {
    h ^= cryp_rotl64(h, 49) ^ cryp_rotl64(h, 24);
    h *= CRYP_PRIME_MX2;
    h ^= (h >> 35) + len;
    h *= CRYP_PRIME_MX2;
    return h ^ (h >> 28);
}

static inline uint64_t cryp_xxh3_mix16(const unsigned char* in, const unsigned char* sec, uint64_t seed) {
    uint64_t lo = cryp_read64(in);
    uint64_t hi = cryp_read64(in + 8);
    return cryp_mul128_fold64(lo ^ (cryp_read64(sec) + seed), hi ^ (cryp_read64(sec + 8) - seed));
}

/* The long-input loop of XXH3 keeps 8 x 64-bit accumulators. They live in
 * AVX2 registers when the build targets AVX2, in SSE2 registers on every
 * x86-64 (SSE2 is part of the baseline), and in plain uint64_t elsewhere -
 * same results, the vector forms just process a 64-byte stripe in 2 or 4
 * instructions per step (the reference xxHash does the same). */
#if !defined(CRYPTO_HASH_PORTABLE) && defined(__AVX2__)
#  define CRYP_XXH3_AVX2 1
#  include <immintrin.h>
#elif !defined(CRYPTO_HASH_PORTABLE) && \
      (defined(__SSE2__) || defined(_M_X64) || defined(_M_AMD64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))
#  define CRYP_XXH3_SSE2 1
#  include <emmintrin.h>
#endif

#if defined(CRYP_XXH3_AVX2)
typedef struct { __m256i v[2]; } CrypXxh3Acc;

static inline void cryp_xxh3_acc_load(CrypXxh3Acc* a, const uint64_t init[8]) {
    a->v[0] = _mm256_loadu_si256((const __m256i*)(const void*)init);
    a->v[1] = _mm256_loadu_si256((const __m256i*)(const void*)(init + 4));
}

static inline void cryp_xxh3_acc_store(const CrypXxh3Acc* a, uint64_t out[8]) {
    _mm256_storeu_si256((__m256i*)(void*)out, a->v[0]);
    _mm256_storeu_si256((__m256i*)(void*)(out + 4), a->v[1]);
}

static inline void cryp_xxh3_accumulate512(CrypXxh3Acc* a, const unsigned char* in, const unsigned char* sec) {
    for (int i = 0; i < 2; ++i) {
        __m256i d  = _mm256_loadu_si256((const __m256i*)(const void*)(in + 32 * i));
        __m256i k  = _mm256_loadu_si256((const __m256i*)(const void*)(sec + 32 * i));
        __m256i dk = _mm256_xor_si256(d, k);
        __m256i pr = _mm256_mul_epu32(dk, _mm256_shuffle_epi32(dk, _MM_SHUFFLE(0, 3, 0, 1)));   /* lo32 * hi32 */
        __m256i sw = _mm256_shuffle_epi32(d, _MM_SHUFFLE(1, 0, 3, 2));                          /* swap 64-bit lanes */
        a->v[i] = _mm256_add_epi64(a->v[i], _mm256_add_epi64(pr, sw));
    }
}

static inline void cryp_xxh3_scramble(CrypXxh3Acc* a, const unsigned char* sec) {
    const __m256i prime = _mm256_set1_epi32((int)CRYP_P32_1);
    for (int i = 0; i < 2; ++i) {
        __m256i x = _mm256_xor_si256(a->v[i], _mm256_srli_epi64(a->v[i], 47));
        x = _mm256_xor_si256(x, _mm256_loadu_si256((const __m256i*)(const void*)(sec + 32 * i)));
        __m256i lo = _mm256_mul_epu32(x, prime);
        __m256i hi = _mm256_mul_epu32(_mm256_shuffle_epi32(x, _MM_SHUFFLE(0, 3, 0, 1)), prime);
        a->v[i] = _mm256_add_epi64(lo, _mm256_slli_epi64(hi, 32));
    }
}
#elif defined(CRYP_XXH3_SSE2)
typedef struct { __m128i v[4]; } CrypXxh3Acc;

static inline void cryp_xxh3_acc_load(CrypXxh3Acc* a, const uint64_t init[8]) {
    for (int i = 0; i < 4; ++i) {
        a->v[i] = _mm_loadu_si128((const __m128i*)(const void*)(init + 2 * i));
    }
}

static inline void cryp_xxh3_acc_store(const CrypXxh3Acc* a, uint64_t out[8]) {
    for (int i = 0; i < 4; ++i) {
        _mm_storeu_si128((__m128i*)(void*)(out + 2 * i), a->v[i]);
    }
}

static inline void cryp_xxh3_accumulate512(CrypXxh3Acc* a, const unsigned char* in, const unsigned char* sec) {
    for (int i = 0; i < 4; ++i) {
        __m128i d  = _mm_loadu_si128((const __m128i*)(const void*)(in + 16 * i));
        __m128i k  = _mm_loadu_si128((const __m128i*)(const void*)(sec + 16 * i));
        __m128i dk = _mm_xor_si128(d, k);
        __m128i pr = _mm_mul_epu32(dk, _mm_shuffle_epi32(dk, _MM_SHUFFLE(0, 3, 0, 1)));   /* lo32 * hi32 */
        __m128i sw = _mm_shuffle_epi32(d, _MM_SHUFFLE(1, 0, 3, 2));                       /* swap 64-bit lanes */
        a->v[i] = _mm_add_epi64(a->v[i], _mm_add_epi64(pr, sw));
    }
}

static inline void cryp_xxh3_scramble(CrypXxh3Acc* a, const unsigned char* sec) {
    const __m128i prime = _mm_set1_epi32((int)CRYP_P32_1);
    for (int i = 0; i < 4; ++i) {
        __m128i x = _mm_xor_si128(a->v[i], _mm_srli_epi64(a->v[i], 47));
        x = _mm_xor_si128(x, _mm_loadu_si128((const __m128i*)(const void*)(sec + 16 * i)));
        __m128i lo = _mm_mul_epu32(x, prime);
        __m128i hi = _mm_mul_epu32(_mm_shuffle_epi32(x, _MM_SHUFFLE(0, 3, 0, 1)), prime);
        a->v[i] = _mm_add_epi64(lo, _mm_slli_epi64(hi, 32));
    }
}
#else
typedef struct { uint64_t v[8]; } CrypXxh3Acc;

static inline void cryp_xxh3_acc_load(CrypXxh3Acc* a, const uint64_t init[8]) {
    memcpy(a->v, init, sizeof a->v);
}

static inline void cryp_xxh3_acc_store(const CrypXxh3Acc* a, uint64_t out[8]) {
    memcpy(out, a->v, sizeof a->v);
}

/* 64-byte stripe into the 8 accumulators. */
static inline void cryp_xxh3_accumulate512(CrypXxh3Acc* a, const unsigned char* in, const unsigned char* sec) {
    for (unsigned i = 0; i < 8; ++i) {
        uint64_t v = cryp_read64(in + 8 * i);
        uint64_t key = v ^ cryp_read64(sec + 8 * i);
        a->v[i ^ 1] += v;
        a->v[i] += (key & 0xFFFFFFFFu) * (key >> 32);
    }
}

static inline void cryp_xxh3_scramble(CrypXxh3Acc* a, const unsigned char* sec) {
    for (unsigned i = 0; i < 8; ++i) {
        uint64_t x = a->v[i];
        x ^= x >> 47;
        x ^= cryp_read64(sec + 8 * i);
        a->v[i] = x * CRYP_P32_1;
    }
}
#endif

static inline uint64_t cryp_xxh3_long(const unsigned char* in, size_t len, const unsigned char* sec) {
    static const uint64_t init[8] = { CRYP_P32_3, CRYP_P64_1, CRYP_P64_2, CRYP_P64_3, CRYP_P64_4, CRYP_P32_2, CRYP_P64_5, CRYP_P32_1 };
    CrypXxh3Acc a;
    cryp_xxh3_acc_load(&a, init);
    const size_t stripes_per_block = (CRYP_XXH3_SECRET_SIZE - CRYP_XXH3_STRIPE_LEN) / 8;     /* 16 */
    const size_t block_len = CRYP_XXH3_STRIPE_LEN * stripes_per_block;                       /* 1024 */
    size_t nb_blocks = (len - 1) / block_len;
    for (size_t n = 0; n < nb_blocks; ++n) {
        for (size_t s = 0; s < stripes_per_block; ++s) {
            cryp_xxh3_accumulate512(&a, in + n * block_len + s * CRYP_XXH3_STRIPE_LEN, sec + s * 8);
        }
        cryp_xxh3_scramble(&a, sec + CRYP_XXH3_SECRET_SIZE - CRYP_XXH3_STRIPE_LEN);
    }
    size_t nb_stripes = ((len - 1) - block_len * nb_blocks) / CRYP_XXH3_STRIPE_LEN;
    for (size_t s = 0; s < nb_stripes; ++s) {
        cryp_xxh3_accumulate512(&a, in + nb_blocks * block_len + s * CRYP_XXH3_STRIPE_LEN, sec + s * 8);
    }
    /* the last stripe (may overlap the previous ones) */
    cryp_xxh3_accumulate512(&a, in + len - CRYP_XXH3_STRIPE_LEN, sec + CRYP_XXH3_SECRET_SIZE - CRYP_XXH3_STRIPE_LEN - 7);
    uint64_t acc[8];
    cryp_xxh3_acc_store(&a, acc);
    uint64_t result = (uint64_t)len * CRYP_P64_1;
    for (unsigned i = 0; i < 4; ++i) {
        result += cryp_mul128_fold64(acc[2 * i] ^ cryp_read64(sec + 11 + 16 * i),
                                     acc[2 * i + 1] ^ cryp_read64(sec + 11 + 16 * i + 8));
    }
    return cryp_xxh3_avalanche(result);
}

/* XXH3-64 (= reference XXH3_64bits_withSeed(); seed 0 = XXH3_64bits()). */
static inline uint64_t cryp_xxh3_64(const void* data, size_t length, uint64_t seed) {
    const unsigned char* in = (const unsigned char*)data;
    if (in == NULL) {
        in = cryp_empty;
        length = 0;
    }
    const unsigned char* sec = cryp_xxh3_secret;
    uint64_t len = (uint64_t)length;

    if (length <= 16) {
        if (length > 8) {
            uint64_t bitflip1 = (cryp_read64(sec + 24) ^ cryp_read64(sec + 32)) + seed;
            uint64_t bitflip2 = (cryp_read64(sec + 40) ^ cryp_read64(sec + 48)) - seed;
            uint64_t lo = cryp_read64(in) ^ bitflip1;
            uint64_t hi = cryp_read64(in + length - 8) ^ bitflip2;
            uint64_t acc = len + cryp_swap64(lo) + hi + cryp_mul128_fold64(lo, hi);
            return cryp_xxh3_avalanche(acc);
        }
        if (length >= 4) {
            uint64_t s = seed ^ ((uint64_t)cryp_swap32((uint32_t)seed) << 32);
            uint32_t in1 = cryp_read32(in);
            uint32_t in2 = cryp_read32(in + length - 4);
            uint64_t bitflip = (cryp_read64(sec + 8) ^ cryp_read64(sec + 16)) - s;
            uint64_t in64 = (uint64_t)in2 + ((uint64_t)in1 << 32);
            return cryp_xxh3_rrmxmx(in64 ^ bitflip, len);
        }
        if (length > 0) {
            uint32_t c1 = in[0], c2 = in[length >> 1], c3 = in[length - 1];
            uint32_t combined = (c1 << 16) | (c2 << 24) | c3 | ((uint32_t)length << 8);
            uint64_t bitflip = (uint64_t)(cryp_read32(sec) ^ cryp_read32(sec + 4)) + seed;
            return cryp_xxh64_avalanche((uint64_t)combined ^ bitflip);
        }
        return cryp_xxh64_avalanche(seed ^ (cryp_read64(sec + 56) ^ cryp_read64(sec + 64)));
    }
    if (length <= 128) {
        uint64_t acc = len * CRYP_P64_1;
        if (length > 32) {
            if (length > 64) {
                if (length > 96) {
                    acc += cryp_xxh3_mix16(in + 48, sec + 96, seed);
                    acc += cryp_xxh3_mix16(in + length - 64, sec + 112, seed);
                }
                acc += cryp_xxh3_mix16(in + 32, sec + 64, seed);
                acc += cryp_xxh3_mix16(in + length - 48, sec + 80, seed);
            }
            acc += cryp_xxh3_mix16(in + 16, sec + 32, seed);
            acc += cryp_xxh3_mix16(in + length - 32, sec + 48, seed);
        }
        acc += cryp_xxh3_mix16(in, sec, seed);
        acc += cryp_xxh3_mix16(in + length - 16, sec + 16, seed);
        return cryp_xxh3_avalanche(acc);
    }
    if (length <= CRYP_XXH3_MIDSIZE_MAX) {
        uint64_t acc = len * CRYP_P64_1;
        size_t rounds = length / 16;
        for (size_t i = 0; i < 8; ++i) {
            acc += cryp_xxh3_mix16(in + 16 * i, sec + 16 * i, seed);
        }
        uint64_t acc_end = cryp_xxh3_mix16(in + length - 16, sec + 136 - 17, seed);
        acc = cryp_xxh3_avalanche(acc);
        for (size_t i = 8; i < rounds; ++i) {
            acc_end += cryp_xxh3_mix16(in + 16 * i, sec + 16 * (i - 8) + 3, seed);
        }
        return cryp_xxh3_avalanche(acc + acc_end);
    }
    if (seed == 0) {
        return cryp_xxh3_long(in, length, sec);
    }
    /* a seeded long hash uses a secret derived from the seed */
    unsigned char custom[CRYP_XXH3_SECRET_SIZE];
    for (size_t i = 0; i < CRYP_XXH3_SECRET_SIZE / 16; ++i) {
        uint64_t lo = cryp_read64(sec + 16 * i) + seed;
        uint64_t hi = cryp_read64(sec + 16 * i + 8) - seed;
        for (unsigned b = 0; b < 8; ++b) {
            custom[16 * i + b]     = (unsigned char)(lo >> (8 * b));
            custom[16 * i + 8 + b] = (unsigned char)(hi >> (8 * b));
        }
    }
    return cryp_xxh3_long(in, length, custom);
}

#endif 
