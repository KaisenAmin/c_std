/**
 * @author Amin Tahmasebi
 * @date 2026
 * @class UUID
 *
 * RFC 9562 UUID implementation. MD5 (for v3) and SHA-1 (for v5) are
 * embedded below as small public-domain-style routines so the module
 * needs no OpenSSL; randomness for v4/v7 comes from the c_std `secrets`
 * CSPRNG. The whole file is allocation-free.
 */


/* Expose clock_gettime via <time.h> in strict-ANSI mode (-std=c17) on both
   MinGW and glibc. We deliberately avoid <windows.h>, which would typedef its
   own `UUID` (= GUID) and clash with ours. */


#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "uuid.h"
#include "../secrets/secrets.h"

#include <string.h>
#include <stdint.h>
#include <time.h>


/* ================================================================== */
/* Embedded MD5  (RFC 1321 — used by UUIDv3)                          */
/* ================================================================== */


typedef struct {
    uint32_t      state[4];
    uint64_t      count;        /* total message length in bytes */
    unsigned char buffer[64];
} Md5Ctx;


static void md5_transform(uint32_t state[4], const unsigned char block[64]) {
    static const uint32_t K[64] = {
        0xd76aa478u,0xe8c7b756u,0x242070dbu,0xc1bdceeeu,0xf57c0fafu,0x4787c62au,0xa8304613u,0xfd469501u,
        0x698098d8u,0x8b44f7afu,0xffff5bb1u,0x895cd7beu,0x6b901122u,0xfd987193u,0xa679438eu,0x49b40821u,
        0xf61e2562u,0xc040b340u,0x265e5a51u,0xe9b6c7aau,0xd62f105du,0x02441453u,0xd8a1e681u,0xe7d3fbc8u,
        0x21e1cde6u,0xc33707d6u,0xf4d50d87u,0x455a14edu,0xa9e3e905u,0xfcefa3f8u,0x676f02d9u,0x8d2a4c8au,
        0xfffa3942u,0x8771f681u,0x6d9d6122u,0xfde5380cu,0xa4beea44u,0x4bdecfa9u,0xf6bb4b60u,0xbebfbc70u,
        0x289b7ec6u,0xeaa127fau,0xd4ef3085u,0x04881d05u,0xd9d4d039u,0xe6db99e5u,0x1fa27cf8u,0xc4ac5665u,
        0xf4292244u,0x432aff97u,0xab9423a7u,0xfc93a039u,0x655b59c3u,0x8f0ccc92u,0xffeff47du,0x85845dd1u,
        0x6fa87e4fu,0xfe2ce6e0u,0xa3014314u,0x4e0811a1u,0xf7537e82u,0xbd3af235u,0x2ad7d2bbu,0xeb86d391u
    };
    static const int S[64] = {
        7,12,17,22, 7,12,17,22, 7,12,17,22, 7,12,17,22,
        5, 9,14,20, 5, 9,14,20, 5, 9,14,20, 5, 9,14,20,
        4,11,16,23, 4,11,16,23, 4,11,16,23, 4,11,16,23,
        6,10,15,21, 6,10,15,21, 6,10,15,21, 6,10,15,21
    };

    uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
    uint32_t m[16];

    for (int i = 0; i < 16; i++) {
        m[i] = (uint32_t)block[i*4]            | ((uint32_t)block[i*4+1] << 8)
             | ((uint32_t)block[i*4+2] << 16)  | ((uint32_t)block[i*4+3] << 24);   /* little-endian */
    }

    for (int i = 0; i < 64; i++) {
        uint32_t f;
        int g;

        if (i < 16) { 
            f = (b & c) | (~b & d);        
            g = i; 
        }
        else if (i < 32) { 
            f = (d & b) | (~d & c);        
            g = (5*i + 1) & 15; 
        }
        else if (i < 48) { 
            f = b ^ c ^ d;                 
            g = (3*i + 5) & 15; 
        }
        else { 
            f = c ^ (b | ~d);              
            g = (7*i) & 15; 
        }

        f = f + a + K[i] + m[g];
        a = d; d = c; c = b;
        b = b + ((f << S[i]) | (f >> (32 - S[i])));
    }

    state[0] += a; state[1] += b; state[2] += c; state[3] += d;
}


static void md5_init(Md5Ctx* ctx) {
    ctx->state[0] = 0x67452301u; ctx->state[1] = 0xefcdab89u;
    ctx->state[2] = 0x98badcfeu; ctx->state[3] = 0x10325476u;
    ctx->count = 0;
}


static void md5_update(Md5Ctx* ctx, const unsigned char* data, size_t len) {
    size_t have = (size_t)(ctx->count & 63);
    ctx->count += len;
    size_t i = 0;
    if (have) {
        size_t need = 64 - have;

        if (len < need) { 
            memcpy(ctx->buffer + have, data, len); 
            return; 
        }

        memcpy(ctx->buffer + have, data, need);
        md5_transform(ctx->state, ctx->buffer);
        i = need;
    }
    for (; i + 64 <= len; i += 64) {
        md5_transform(ctx->state, data + i);
    }
    if (i < len) {
        memcpy(ctx->buffer, data + i, len - i);
    }
}


static void md5_final(Md5Ctx* ctx, unsigned char out[16]) {
    uint64_t bits = ctx->count * 8;
    size_t have = (size_t)(ctx->count & 63);
    unsigned char pad = 0x80;
    md5_update(ctx, &pad, 1);
    unsigned char zero = 0;

    while ((ctx->count & 63) != 56) {
        md5_update(ctx, &zero, 1);
    }

    unsigned char lenbuf[8];
    for (int i = 0; i < 8; i++) {
        lenbuf[i] = (unsigned char)(bits >> (8 * i));   /* little-endian length */
    }
    md5_update(ctx, lenbuf, 8);
    (void)have;

    for (int i = 0; i < 4; i++) {
        out[i*4]   = (unsigned char)(ctx->state[i]);
        out[i*4+1] = (unsigned char)(ctx->state[i] >> 8);
        out[i*4+2] = (unsigned char)(ctx->state[i] >> 16);
        out[i*4+3] = (unsigned char)(ctx->state[i] >> 24);
    }
}


/* ================================================================== */
/* Embedded SHA-1  (FIPS 180 — used by UUIDv5)                        */
/* ================================================================== */


typedef struct {
    uint32_t      state[5];
    uint64_t      count;
    unsigned char buffer[64];
} Sha1Ctx;


static uint32_t sha1_rol(uint32_t v, int b) {
    return (v << b) | (v >> (32 - b));
}


static void sha1_transform(uint32_t state[5], const unsigned char block[64]) {
    uint32_t w[80];

    for (int i = 0; i < 16; i++) {
        w[i] = ((uint32_t)block[i*4] << 24) | ((uint32_t)block[i*4+1] << 16)
             | ((uint32_t)block[i*4+2] << 8) | (uint32_t)block[i*4+3];          /* big-endian */
    }
    for (int i = 16; i < 80; i++) {
        w[i] = sha1_rol(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
    }

    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];
    for (int i = 0; i < 80; i++) {
        uint32_t f, k;

        if (i < 20)      { 
            f = (b & c) | (~b & d);            
            k = 0x5A827999u; 
        }
        else if (i < 40) { 
            f = b ^ c ^ d;                     
            k = 0x6ED9EBA1u; 
        }
        else if (i < 60) { 
            f = (b & c) | (b & d) | (c & d);   
            k = 0x8F1BBCDCu; 
        }
        else { 
            f = b ^ c ^ d;                     
            k = 0xCA62C1D6u; 
        }

        uint32_t tmp = sha1_rol(a, 5) + f + e + k + w[i];
        e = d; d = c; c = sha1_rol(b, 30); b = a; a = tmp;
    }

    state[0] += a; state[1] += b; state[2] += c; state[3] += d; state[4] += e;
}


static void sha1_init(Sha1Ctx* ctx) {
    ctx->state[0] = 0x67452301u; ctx->state[1] = 0xEFCDAB89u;
    ctx->state[2] = 0x98BADCFEu; ctx->state[3] = 0x10325476u;
    ctx->state[4] = 0xC3D2E1F0u;
    ctx->count = 0;
}


static void sha1_update(Sha1Ctx* ctx, const unsigned char* data, size_t len) {
    size_t have = (size_t)(ctx->count & 63);
    ctx->count += len;
    size_t i = 0;

    if (have) {
        size_t need = 64 - have;

        if (len < need) { 
            memcpy(ctx->buffer + have, data, len); 
            return; 
        }

        memcpy(ctx->buffer + have, data, need);
        sha1_transform(ctx->state, ctx->buffer);
        i = need;
    }
    for (; i + 64 <= len; i += 64) {
        sha1_transform(ctx->state, data + i);
    }
    if (i < len) {
        memcpy(ctx->buffer, data + i, len - i);
    }
}


static void sha1_final(Sha1Ctx* ctx, unsigned char out[20]) {
    uint64_t bits = ctx->count * 8;
    unsigned char pad = 0x80;
    sha1_update(ctx, &pad, 1);
    unsigned char zero = 0;

    while ((ctx->count & 63) != 56) {
        sha1_update(ctx, &zero, 1);
    }

    unsigned char lenbuf[8];
    for (int i = 0; i < 8; i++) {
        lenbuf[i] = (unsigned char)(bits >> (8 * (7 - i)));   /* big-endian length */
    }

    sha1_update(ctx, lenbuf, 8);

    for (int i = 0; i < 5; i++) {
        out[i*4]   = (unsigned char)(ctx->state[i] >> 24);
        out[i*4+1] = (unsigned char)(ctx->state[i] >> 16);
        out[i*4+2] = (unsigned char)(ctx->state[i] >> 8);
        out[i*4+3] = (unsigned char)(ctx->state[i]);
    }
}


/* Current Unix time as a timespec; false if the clock is unavailable. C11
   timespec_get is used where it exists (MSVC has no clock_gettime). */
static int uuid_now_timespec(struct timespec* ts) {
#if defined(TIME_UTC)
    return timespec_get(ts, TIME_UTC) == TIME_UTC;
#else
    return clock_gettime(CLOCK_REALTIME, ts) == 0;
#endif
}


static unsigned long long uuid_now_unix_ms(void) {
    /* Current Unix time in milliseconds — for UUIDv7. */
    struct timespec ts;

    if (!uuid_now_timespec(&ts)) {
        return 0;
    }

    return (unsigned long long)ts.tv_sec * 1000ULL + (unsigned long long)ts.tv_nsec / 1000000ULL;
}


/* splitmix64 finalizer: a bijection on 64-bit values. */
static uint64_t uuid_mix64(uint64_t z) {
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}


/* Process-wide call counter for the fallback below. */
#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>
static uint64_t uuid_counter_next(void) {
    static volatile __int64 counter;
    return (uint64_t)_InterlockedIncrement64(&counter);
}
#else
static uint64_t uuid_counter_next(void) {
    static uint64_t counter;
    return __atomic_add_fetch(&counter, 1, __ATOMIC_RELAXED);
}
#endif


/* Fills u->bytes[from..16) with random bytes from the secrets CSPRNG.
 *
 * If the OS generator fails, secrets_token_bytes leaves zeros, which would
 * make every UUID identical. The fallback instead fills the bytes from the
 * clock and addresses, and writes a bijective mix of a process-wide counter
 * into the 8 bytes listed in `unique_pos` (bytes the version/variant stamp
 * never touches). That is not cryptographically random, but no two UUIDs
 * from one process can be equal. */
static void uuid_fill_random(Uuid* u, size_t from, const unsigned char unique_pos[8]) {
    if (secrets_token_bytes(u->bytes + from, 16 - from)) {
        return;
    }
    UUID_LOG("[uuid_fill_random] CSPRNG failed, using the counter-based fallback");

    static const char key_anchor = 0;    /* its address is the per-process key */
    uint64_t key = uuid_mix64((uint64_t)(uintptr_t)&key_anchor);
    uint64_t ctr = uuid_counter_next();

    struct timespec ts;
    uint64_t t = 0;
    if (uuid_now_timespec(&ts)) {
        t = (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;
    }
    uint64_t s = t ^ ((uint64_t)(uintptr_t)u << 16) ^ key;
    uint64_t r = 0;
    for (size_t i = from; i < 16; i++) {
        if ((i - from) % 8 == 0) {
            s += 0x9E3779B97F4A7C15ULL;
            r = uuid_mix64(s ^ ctr);
        }
        u->bytes[i] = (unsigned char)(r >> (8 * ((i - from) % 8)));
    }

    uint64_t unique = uuid_mix64(ctr + key);
    for (int i = 0; i < 8; i++) {
        u->bytes[unique_pos[i]] = (unsigned char)(unique >> (8 * i));
    }
}


/* Process-wide uuid_v7 state, under a tiny spinlock (the critical section
   only compares and bumps two integers). */
#if defined(_MSC_VER) && !defined(__clang__)
static volatile long uuid_v7_lock_word;
static void uuid_v7_lock(void) {
    while (_InterlockedExchange(&uuid_v7_lock_word, 1) != 0) {
        /* spin */
    }
}
static void uuid_v7_unlock(void) {
    _InterlockedExchange(&uuid_v7_lock_word, 0);
}
#else
static unsigned char uuid_v7_lock_word;
static void uuid_v7_lock(void) {
    while (__atomic_test_and_set(&uuid_v7_lock_word, __ATOMIC_ACQUIRE)) {
        /* spin */
    }
}
static void uuid_v7_unlock(void) {
    __atomic_clear(&uuid_v7_lock_word, __ATOMIC_RELEASE);
}
#endif
static unsigned long long uuid_v7_last_ms;
static uint64_t           uuid_v7_counter;    /* 42 bits: rand_a (12) + top of rand_b (30) */

#define UUID_V7_COUNTER_MAX 0x3FFFFFFFFFFULL  /* 2^42 - 1 */


/* Stamp the version nibble (octet 6) and the RFC 4122 variant bits (octet 8). */
static void uuid_set_version_variant(Uuid* u, int version) {
    u->bytes[6] = (unsigned char)((u->bytes[6] & 0x0F) | (version << 4));
    u->bytes[8] = (unsigned char)((u->bytes[8] & 0x3F) | 0x80);
}


static int uuid_hex_val(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }

    return -1;
}


/* Case-insensitive prefix test (avoids the non-standard strncasecmp). */
static int uuid_ci_prefix(const char* s, const char* prefix) {
    while (*prefix) {
        char a = *s, b = *prefix;

        if (a >= 'A' && a <= 'Z') {
            a = (char)(a + 32);
        }
        if (b >= 'A' && b <= 'Z') {
            b = (char)(b + 32);
        }
        if (a != b) {
            return 0;
        }

        s++; prefix++;
    }
    return 1;
}


/**
 * @brief Generate a version-4 (fully random) UUID (RFC 9562 §5.4).
 *
 * All 122 free bits are filled from the cross-platform `secrets` CSPRNG
 * (BCryptGenRandom on Windows, getrandom()/`/dev/urandom` on POSIX); the
 * 4-bit version field is then set to 4 and the 2-bit variant field to the
 * RFC 4122/9562 value. Two calls are astronomically unlikely to collide
 * (~1 in 2^122).
 *
 * @return A freshly generated v4 UUID, returned by value.
 *
 * @note Allocation-free and thread-safe — it keeps no shared mutable state.
 * @warning Fine as an identifier, but these 122 random bits are not a
 *          substitute for a key, token, or nonce; use the `secrets` module
 *          for security-sensitive material.
 * @see uuid_v7 for a time-ordered alternative.
 *
 */
Uuid uuid_v4(void) {
    static const unsigned char unique_pos[8] = {0, 1, 2, 3, 4, 5, 7, 9};
    Uuid u;
    uuid_fill_random(&u, 0, unique_pos);
    uuid_set_version_variant(&u, 4);

    UUID_LOG("[uuid_v4] generated");
    return u;
}


/**
 * @brief Generate a version-7 (Unix-time-ordered) UUID (RFC 9562 §5.7).
 *
 * Layout: octets 0–5 hold the current Unix timestamp in **milliseconds** as
 * a 48-bit big-endian integer, followed by the 4-bit version (7), 12 random
 * bits, the 2-bit variant, and 62 more random bits (from the `secrets`
 * CSPRNG). Because the most-significant bytes are the timestamp, v7 UUIDs
 * are k-sortable: ordering them lexicographically (see @ref uuid_compare)
 * orders them by creation time, which keeps B-tree index inserts local.
 *
 * @return A freshly generated v7 UUID, returned by value.
 *
 * Within one process every uuid_v7() is greater (by uuid_compare) than the
 * previous one, also within one millisecond and across threads: the 12-bit
 * rand_a and the top 30 bits of rand_b hold a 42-bit counter (RFC 9562
 * §6.2 method 1, like Python 3.14's uuid7) that starts at a random 41-bit
 * value in each new millisecond and is incremented otherwise. If the clock
 * goes backwards, the last timestamp is kept; if the counter runs out, the
 * timestamp is advanced by 1 ms. The last 32 bits stay random. (Without the
 * counter about half of the UUIDs made within one millisecond compared
 * lower than the one before.)
 *
 * @note RFC 9562 recommends v7 over v1/v6 for new applications. Across
 *       processes, ordering holds at millisecond granularity only.
 *       Allocation-free and thread-safe.
 * @see uuid_v7_timestamp_ms to read the embedded timestamp back; uuid_v4.
 *
 */
Uuid uuid_v7(void) {
    Uuid u;
    static const unsigned char unique_pos[8] = {7, 9, 10, 11, 12, 13, 14, 15};
    uuid_fill_random(&u, 6, unique_pos);   /* rand_a (12 bits) + rand_b (62 bits) */

    /* A fresh random 41-bit counter start (top bit clear leaves room for
       2^41 increments), from bytes that the counter overwrites anyway. */
    uint64_t seed = 0;
    for (int i = 6; i < 12; i++) {
        seed = (seed << 8) | u.bytes[i];
    }
    seed &= UUID_V7_COUNTER_MAX >> 1;

    unsigned long long now = uuid_now_unix_ms();   /* read outside the lock */
    uuid_v7_lock();
    if (now > uuid_v7_last_ms) {
        uuid_v7_last_ms = now;
        uuid_v7_counter = seed;
    }
    else if (uuid_v7_counter < UUID_V7_COUNTER_MAX) {
        uuid_v7_counter++;            /* same millisecond, or the clock went back */
    }
    else {
        uuid_v7_last_ms++;            /* counter exhausted: borrow the next ms */
        uuid_v7_counter = seed;
    }
    unsigned long long ms = uuid_v7_last_ms;
    uint64_t counter = uuid_v7_counter;
    uuid_v7_unlock();

    u.bytes[0] = (unsigned char)(ms >> 40);
    u.bytes[1] = (unsigned char)(ms >> 32);
    u.bytes[2] = (unsigned char)(ms >> 24);
    u.bytes[3] = (unsigned char)(ms >> 16);
    u.bytes[4] = (unsigned char)(ms >> 8);
    u.bytes[5] = (unsigned char)(ms);

    uint64_t hi = counter >> 30;              /* 12 bits -> rand_a */
    uint64_t lo = counter & 0x3FFFFFFFULL;    /* 30 bits -> rand_b, after the variant */
    u.bytes[6]  = (unsigned char)(hi >> 8);   /* the version nibble is stamped below */
    u.bytes[7]  = (unsigned char)(hi);
    u.bytes[8]  = (unsigned char)(lo >> 24);  /* top 2 bits: the variant, stamped below */
    u.bytes[9]  = (unsigned char)(lo >> 16);
    u.bytes[10] = (unsigned char)(lo >> 8);
    u.bytes[11] = (unsigned char)(lo);
    uuid_set_version_variant(&u, 7);

    UUID_LOG("[uuid_v7] generated (ts_ms=%llu)", ms);
    return u;
}


/**
 * @brief Generate a version-3 (name-based, MD5) UUID (RFC 9562 §5.3).
 *
 * Computes `MD5(namespace_id.bytes || name)`, copies the 16-byte digest in,
 * and stamps the version (3) and variant fields. The result is fully
 * **deterministic**: the same namespace and name always produce the same
 * UUID — ideal for stable IDs derived from a known string such as a URL, a
 * file path, or an account name.
 *
 * @param namespace_id The namespace UUID, typically one of
 *                     uuid_namespace_dns() / _url() / _oid() / _x500().
 * @param name         The name to hash within that namespace. A `NULL` name
 *                     is treated as the empty string.
 * @return The name-based v3 UUID, returned by value.
 *
 * @note Reproduces Python's `uuid.uuid3()` byte-for-byte, e.g.
 *       `uuid3(NAMESPACE_DNS, "python.org")` ==
 *       `6fa459ea-ee8a-3ca4-894e-db77e160355e`.
 * @warning Prefer @ref uuid_v5 (SHA-1) for new code; v3 exists only to
 *          interoperate with existing MD5-based UUIDs.
 * @see uuid_v5, uuid_namespace_dns
 */
Uuid uuid_v3(Uuid namespace_id, const char* name) {
    if (!name) {
        name = "";
    }
    unsigned char digest[16];
    Md5Ctx ctx;

    md5_init(&ctx);
    md5_update(&ctx, namespace_id.bytes, 16);
    md5_update(&ctx, (const unsigned char*)name, strlen(name));
    md5_final(&ctx, digest);

    Uuid u;
    memcpy(u.bytes, digest, 16);
    uuid_set_version_variant(&u, 3);

    UUID_LOG("[uuid_v3] generated for name '%s'", name);
    return u;
}


/**
 * @brief Generate a version-5 (name-based, SHA-1) UUID (RFC 9562 §5.5).
 *
 * Like @ref uuid_v3, but hashes with SHA-1 and uses the first 16 bytes of
 * the 20-byte digest. Deterministic, and the recommended name-based version.
 *
 * @param namespace_id The namespace UUID (e.g. uuid_namespace_url()).
 * @param name         The name to hash. A `NULL` name is treated as the
 *                     empty string.
 * @return The name-based v5 UUID, returned by value.
 *
 * @note Reproduces Python's `uuid.uuid5()` byte-for-byte, e.g.
 *       `uuid5(NAMESPACE_DNS, "python.org")` ==
 *       `886313e1-3b8a-5372-9b90-0c9aee199e5d`.
 * @see uuid_v3, uuid_namespace_dns
 */
Uuid uuid_v5(Uuid namespace_id, const char* name) {
    if (!name) {
        name = "";
    }

    unsigned char digest[20];
    Sha1Ctx ctx;

    sha1_init(&ctx);
    sha1_update(&ctx, namespace_id.bytes, 16);
    sha1_update(&ctx, (const unsigned char*)name, strlen(name));
    sha1_final(&ctx, digest);

    Uuid u;
    memcpy(u.bytes, digest, 16);   /* first 128 bits of the SHA-1 digest */
    uuid_set_version_variant(&u, 5);

    UUID_LOG("[uuid_v5] generated for name '%s'", name);
    return u;
}


/* ================================================================== */
/* Special values                                                     */
/* ================================================================== */


/**
 * @brief Return the Nil UUID — all 128 bits zero (RFC 9562 §5.9).
 *
 * Formats as `00000000-0000-0000-0000-000000000000`.
 *
 * @return The Nil UUID, returned by value.
 *
 * @note Handy as a sentinel "no UUID" value (e.g. a struct's default before
 *       a real UUID is assigned); test for it with @ref uuid_is_nil.
 * @see uuid_is_nil, uuid_max
 */
Uuid uuid_nil(void) {
    Uuid u;
    memset(u.bytes, 0x00, sizeof(u.bytes));

    return u;
}


/**
 * @brief Return the Max UUID — all 128 bits one (RFC 9562 §5.10).
 *
 * Formats as `ffffffff-ffff-ffff-ffff-ffffffffffff`.
 *
 * @return The Max UUID, returned by value.
 *
 * @note A convenient inclusive upper bound for UUID range comparisons; test
 *       for it with @ref uuid_is_max.
 * @see uuid_is_max, uuid_nil
 */
Uuid uuid_max(void) {
    Uuid u;
    memset(u.bytes, 0xFF, sizeof(u.bytes));
    return u;
}


/* DNS / URL / OID / X.500 differ only in octet index 3 (10/11/12/14). */
static Uuid uuid_make_namespace(unsigned char octet3) {
    Uuid u = {{ 0x6b,0xa7,0xb8,0x00, 0x9d,0xad, 0x11,0xd1, 0x80,0xb4, 0x00,0xc0,0x4f,0xd4,0x30,0xc8 }};
    u.bytes[3] = octet3;

    return u;
}


/**
 * @brief Return the predefined DNS namespace UUID (RFC 9562 Appendix C):
 *        `6ba7b810-9dad-11d1-80b4-00c04fd430c8`.
 * @return The DNS namespace UUID, for use as the `namespace_id` of a
 *         name-based UUID whose name is a fully-qualified domain name.
 * @see uuid_v3, uuid_v5
 */
Uuid uuid_namespace_dns(void)  {
    return uuid_make_namespace(0x10);
}


/**
 * @brief Return the predefined URL namespace UUID (RFC 9562 Appendix C):
 *        `6ba7b811-9dad-11d1-80b4-00c04fd430c8`.
 * @return The URL namespace UUID, for names that are URLs.
 * @see uuid_v3, uuid_v5
 */
Uuid uuid_namespace_url(void)  {
    return uuid_make_namespace(0x11);
}


/**
 * @brief Return the predefined ISO OID namespace UUID (RFC 9562 Appendix C):
 *        `6ba7b812-9dad-11d1-80b4-00c04fd430c8`.
 * @return The OID namespace UUID, for names that are ISO object identifiers.
 * @see uuid_v3, uuid_v5
 */
Uuid uuid_namespace_oid(void)  {
    return uuid_make_namespace(0x12);
}


/**
 * @brief Return the predefined X.500 namespace UUID (RFC 9562 Appendix C):
 *        `6ba7b814-9dad-11d1-80b4-00c04fd430c8`.
 * @return The X.500 namespace UUID, for names that are X.500 DNs.
 * @see uuid_v3, uuid_v5
 */
Uuid uuid_namespace_x500(void) {
    return uuid_make_namespace(0x14);
}



/**
 * @brief Write the canonical lowercase text form of @p u into @p out.
 *
 * Produces the 36-character form `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx`
 * followed by a NUL terminator (37 bytes total). No allocation is performed.
 *
 * @param u   The UUID to format.
 * @param out Destination buffer of at least `UUID_STR_LEN` (37) bytes.
 * @return None.
 *
 * @warning Passing a buffer smaller than `UUID_STR_LEN` is undefined
 *          behaviour — the function always writes exactly 37 bytes.
 * @see uuid_from_string, uuid_to_urn
 */
void uuid_to_string(Uuid u, char out[UUID_STR_LEN]) {
    static const char hex[] = "0123456789abcdef";
    int pos = 0;

    for (int i = 0; i < 16; i++) {
        if (i == 4 || i == 6 || i == 8 || i == 10) {
            out[pos++] = '-';
        }

        out[pos++] = hex[u.bytes[i] >> 4];
        out[pos++] = hex[u.bytes[i] & 0x0F];
    }

    out[pos] = '\0';
}


/**
 * @brief Write the URN form `urn:uuid:xxxxxxxx-…` of @p u into @p out.
 *
 * Produces the 45-character `urn:uuid:` prefixed form plus a NUL terminator
 * (46 bytes total). No allocation is performed.
 *
 * @param u   The UUID to format.
 * @param out Destination buffer of at least `UUID_URN_LEN` (46) bytes.
 * @return None.
 *
 * @warning Passing a buffer smaller than `UUID_URN_LEN` is undefined
 *          behaviour.
 * @see uuid_to_string
 */
void uuid_to_urn(Uuid u, char out[UUID_URN_LEN]) {
    memcpy(out, "urn:uuid:", 9);
    uuid_to_string(u, out + 9);
}


/**
 * @brief Parse a UUID from its text form into @p out.
 *
 * Accepts, case-insensitively, all of these forms:
 *   - canonical hyphenated: `6ba7b810-9dad-11d1-80b4-00c04fd430c8`
 *   - bare 32 hex digits:   `6ba7b8109dad11d180b400c04fd430c8`
 *   - brace-wrapped:        `{6ba7b810-…-00c04fd430c8}`
 *   - URN:                  `urn:uuid:6ba7b810-…-00c04fd430c8`
 * Hyphens are optional and ignored wherever they appear.
 *
 * @param str The string to parse. May be `NULL` (the call then fails).
 * @param out Receives the parsed UUID on success; left untouched on failure.
 * @return `true` on success; `false` if @p str or @p out is `NULL`, or the
 *         input does not reduce to exactly 32 hexadecimal digits.
 *
 * @note Surrounding whitespace is NOT trimmed and causes a parse failure.
 * @see uuid_to_string
 *
 */
bool uuid_from_string(const char* str, Uuid* out) {
    if (!str || !out) {
        return false;
    }

    const char* p = str;
    if (uuid_ci_prefix(p, "urn:uuid:")) {
        p += 9;
    }
    size_t len = strlen(p);
    if (len >= 2 && p[0] == '{' && p[len - 1] == '}') {
        p += 1;
        len -= 2;
    }

    unsigned char bytes[16];
    int nib = 0;
    int hi = 0;

    for (size_t i = 0; i < len; i++) {
        char c = p[i];
        if (c == '-') {
            continue;
        }

        int v = uuid_hex_val(c);
        if (v < 0 || nib >= 32) {
            return false;
        }
        if ((nib & 1) == 0) {
            hi = v;
        }
        else {
            bytes[nib / 2] = (unsigned char)((hi << 4) | v);
        }
        nib++;
    }

    if (nib != 32) {
        return false;
    }

    memcpy(out->bytes, bytes, 16);
    return true;
}


/**
 * @brief Copy the 16 raw bytes of @p u (big-endian field order) into @p out.
 *
 * The compact binary form, suitable for a 16-byte BLOB column or wire
 * transfer.
 *
 * @param u   The UUID.
 * @param out Destination buffer of at least 16 bytes.
 * @return None.
 *
 * @see uuid_from_bytes
 */
void uuid_to_bytes(Uuid u, unsigned char out[16]) {
    memcpy(out, u.bytes, 16);
}


/**
 * @brief Build a UUID from 16 raw bytes in big-endian field order.
 *
 * The bytes are stored verbatim, so this round-trips exactly with
 * @ref uuid_to_bytes and can represent any 128-bit value.
 *
 * @param in Source buffer of 16 bytes.
 * @return The UUID holding those bytes, returned by value.
 *
 * @warning No version/variant fixing is applied, so the result may not be a
 *          valid RFC 9562 UUID if @p in did not already encode one. Use the
 *          generators (uuid_v4 etc.) to produce spec-compliant UUIDs.
 * @see uuid_to_bytes
 */
Uuid uuid_from_bytes(const unsigned char in[16]) {
    Uuid u;
    memcpy(u.bytes, in, 16);
    
    return u;
}


/**
 * @brief Return the version number of @p u.
 *
 * Reads the 4-bit version field (the high nibble of octet 6).
 *
 * @param u The UUID.
 * @return The version, 0–15 (4, 7, 3, or 5 for UUIDs produced by this
 *         library; 0 for the Nil UUID).
 * @see uuid_variant
 */
int uuid_version(Uuid u) {
    return (u.bytes[6] >> 4) & 0x0F;
}


/**
 * @brief Return the variant of @p u.
 *
 * Classifies the leading bits of octet 8.
 *
 * @param u The UUID.
 * @return One of `UUID_VARIANT_NCS`, `UUID_VARIANT_RFC4122`,
 *         `UUID_VARIANT_MICROSOFT`, or `UUID_VARIANT_FUTURE`. Every UUID
 *         produced by this library is `UUID_VARIANT_RFC4122`.
 * @see uuid_version
 */
UuidVariant cstd_uuid_variant(Uuid u) {   /* uuid_variant (header macro) */
    unsigned char b = u.bytes[8];

    if ((b & 0x80) == 0x00) {
        return UUID_VARIANT_NCS;
    }
    if ((b & 0xC0) == 0x80) {
        return UUID_VARIANT_RFC4122;
    }
    if ((b & 0xE0) == 0xC0) {
        return UUID_VARIANT_MICROSOFT;
    }

    return UUID_VARIANT_FUTURE;
}


/**
 * @brief Test whether @p u is the Nil UUID (all 16 bytes zero).
 * @param u The UUID.
 * @return `true` if @p u is the Nil UUID, otherwise `false`.
 * @see uuid_nil
 */
bool uuid_is_nil(Uuid u) {
    for (int i = 0; i < 16; i++) {
        if (u.bytes[i] != 0x00) {
            return false;
        }
    }

    return true;
}


/**
 * @brief Test whether @p u is the Max UUID (all 16 bytes 0xFF).
 * @param u The UUID.
 * @return `true` if @p u is the Max UUID, otherwise `false`.
 * @see uuid_max
 */
bool uuid_is_max(Uuid u) {
    for (int i = 0; i < 16; i++) {
        if (u.bytes[i] != 0xFF) {
            return false;
        }
    }

    return true;
}


/**
 * @brief Test two UUIDs for byte-for-byte equality.
 * @param a The first UUID.
 * @param b The second UUID.
 * @return `true` if @p a and @p b are identical, otherwise `false`.
 * @see uuid_compare
 */
bool uuid_equal(Uuid a, Uuid b) {
    return memcmp(a.bytes, b.bytes, 16) == 0;
}


/**
 * @brief Order two UUIDs lexicographically by their 16 big-endian bytes.
 *
 * Uses the same sign convention as `memcmp`/`strcmp`. For v7 UUIDs this byte
 * order is also chronological order, so sorting them groups records by
 * creation time.
 *
 * @param a The first UUID.
 * @param b The second UUID.
 * @return A negative value if `a < b`, `0` if they are equal, a positive
 *         value if `a > b`.
 * @see uuid_equal
 *
 */
int cstd_uuid_compare(Uuid a, Uuid b) {   /* uuid_compare (header macro) */
    int c = memcmp(a.bytes, b.bytes, 16);

    return (c > 0) - (c < 0);
}


/**
 * @brief Extract the embedded creation timestamp from a version-7 UUID.
 *
 * Reads octets 0–5 as a 48-bit big-endian integer — the Unix timestamp in
 * milliseconds that @ref uuid_v7 wrote.
 *
 * @param u The UUID (expected to be v7).
 * @return The timestamp in milliseconds since the Unix epoch, or `0` if
 *         @p u is not a version-7 UUID.
 * @see uuid_v7
 */
unsigned long long uuid_v7_timestamp_ms(Uuid u) {
    if (uuid_version(u) != 7) {
        return 0;
    }

    unsigned long long ms = 0;
    for (int i = 0; i < 6; i++) {
        ms = (ms << 8) | u.bytes[i];
    }

    return ms;
}
