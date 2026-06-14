/**
 * @file jwt.c
 * @author Amin Tahmasebi
 * @brief Implementation of the JWT module: HS256/384/512 and RS256/384/512
 *        signing/verification with full RFC 7519 claim validation.
 *
 * Backed by OpenSSL for the cryptography (HMAC, EVP_DigestSign/Verify,
 * CRYPTO_memcmp) and by the in-tree json / string / encoding modules
 * for everything else.
 */

#include "jwt.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <ctype.h>

#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/bn.h>
#include <openssl/objects.h>
#include <openssl/rsa.h>
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
#include <openssl/param_build.h>
#include <openssl/core_names.h>
#endif

#include "../string/std_string.h"

/* =================================================================== */
/* Internal types                                                       */
/* =================================================================== */

typedef enum {
    JWT_KEY_KIND_HMAC,
    JWT_KEY_KIND_RSA_PRIVATE,
    JWT_KEY_KIND_RSA_PUBLIC,
    JWT_KEY_KIND_EC_PRIVATE,
    JWT_KEY_KIND_EC_PUBLIC
} JwtKeyKind;

struct JwtKey {
    JwtKeyKind kind;
    /* HMAC: raw secret bytes (owned). */
    unsigned char* secret;
    size_t         secret_len;
    /* RSA: EVP_PKEY (owned). */
    EVP_PKEY*      pkey;
};

struct JwtToken {
    JwtAlgorithm   alg;
    JsonElement*   header;          /* JSON object — owned */
    JsonElement*   payload;         /* JSON object — owned */
    unsigned char* signature;       /* raw bytes — owned (may be NULL when building) */
    size_t         signature_len;
    char*          signing_input;   /* "<b64u_header>.<b64u_payload>" — owned (only set after decode) */
    size_t         signing_input_len;
};

/* =================================================================== */
/* Internal: base64url codec                                             */
/* =================================================================== */

/* RFC 4648 §5: "URL and Filename safe" base64 alphabet with no padding. */
static const char k_b64u_alphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

/* Reverse table for decoding. -1 = invalid. */
static const signed char k_b64u_decode_table[256] = {
    /* 0..31 */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /* 32..63: ' '..'?' */
    -1,-1,-1,-1,-1,-1,-1,-1,  -1,-1,-1,/*+*/62,-1,/*-*/62,-1,/*/=>*/63,
    /*0..9*/52,53,54,55,56,57,58,59, 60,61,-1,-1,-1,-1,-1,-1,
    /* 64..95: '@'..'_' */
    -1, 0, 1, 2, 3, 4, 5, 6,   7, 8, 9,10,11,12,13,14,
    15,16,17,18,19,20,21,22,  23,24,25,-1,-1,-1,-1,/*_*/63,
    /* 96..127: '`'..DEL */
    -1,26,27,28,29,30,31,32,  33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,  49,50,51,-1,-1,-1,-1,-1,
    /* 128..255 */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

/* Encode `in_len` bytes of input to base64url. Output is malloc'd, NUL-
 * terminated, length returned in *out_len (excluding NUL). NULL on OOM. */
static char* jwt_base64url_encode(const unsigned char* in, size_t in_len, size_t* out_len) {
    /* Output length: ceil(in_len / 3) * 4 chars, then strip padding. The
       no-padding length is exactly ceil(in_len * 4 / 3). */
    size_t triples = in_len / 3;
    size_t rem     = in_len % 3;
    size_t out_n   = triples * 4 + (rem ? rem + 1 : 0);
    char* out = (char*)malloc(out_n + 1);
    if (!out) return NULL;

    size_t i, o = 0;
    for (i = 0; i + 3 <= in_len; i += 3) {
        uint32_t v = ((uint32_t)in[i] << 16) | ((uint32_t)in[i+1] << 8) | in[i+2];
        out[o++] = k_b64u_alphabet[(v >> 18) & 0x3F];
        out[o++] = k_b64u_alphabet[(v >> 12) & 0x3F];
        out[o++] = k_b64u_alphabet[(v >>  6) & 0x3F];
        out[o++] = k_b64u_alphabet[ v        & 0x3F];
    }
    if (rem == 1) {
        uint32_t v = (uint32_t)in[i] << 16;
        out[o++] = k_b64u_alphabet[(v >> 18) & 0x3F];
        out[o++] = k_b64u_alphabet[(v >> 12) & 0x3F];
    } else if (rem == 2) {
        uint32_t v = ((uint32_t)in[i] << 16) | ((uint32_t)in[i+1] << 8);
        out[o++] = k_b64u_alphabet[(v >> 18) & 0x3F];
        out[o++] = k_b64u_alphabet[(v >> 12) & 0x3F];
        out[o++] = k_b64u_alphabet[(v >>  6) & 0x3F];
    }
    out[o] = '\0';
    if (out_len) *out_len = o;
    return out;
}

/* Decode `in_len` base64url chars (with or without "=" padding; tolerates
 * stray "+" / "/" from buggy producers). Returns malloc'd bytes; length
 * in *out_len. NULL on invalid input or OOM. */
static unsigned char* jwt_base64url_decode(const char* in, size_t in_len, size_t* out_len) {
    /* Strip any trailing '=' padding. */
    while (in_len > 0 && in[in_len - 1] == '=') in_len--;

    /* Validate every character. */
    for (size_t i = 0; i < in_len; ++i) {
        if (k_b64u_decode_table[(unsigned char)in[i]] < 0) {
            return NULL;
        }
    }

    /* Output size: floor((4 * in_len) / 3) — exact for no-pad input. */
    size_t out_cap = (in_len / 4) * 3;
    size_t tail = in_len % 4;
    if (tail == 1) return NULL;           /* invalid: 1 char doesn't carry a byte */
    if (tail == 2) out_cap += 1;
    else if (tail == 3) out_cap += 2;

    unsigned char* out = (unsigned char*)malloc(out_cap ? out_cap : 1);
    if (!out) return NULL;

    size_t i, o = 0;
    for (i = 0; i + 4 <= in_len; i += 4) {
        uint32_t v =
            ((uint32_t)k_b64u_decode_table[(unsigned char)in[i  ]] << 18) |
            ((uint32_t)k_b64u_decode_table[(unsigned char)in[i+1]] << 12) |
            ((uint32_t)k_b64u_decode_table[(unsigned char)in[i+2]] <<  6) |
             (uint32_t)k_b64u_decode_table[(unsigned char)in[i+3]];
        out[o++] = (unsigned char)((v >> 16) & 0xFF);
        out[o++] = (unsigned char)((v >>  8) & 0xFF);
        out[o++] = (unsigned char)( v        & 0xFF);
    }
    if (tail == 2) {
        uint32_t v =
            ((uint32_t)k_b64u_decode_table[(unsigned char)in[i  ]] << 18) |
            ((uint32_t)k_b64u_decode_table[(unsigned char)in[i+1]] << 12);
        out[o++] = (unsigned char)((v >> 16) & 0xFF);
    } else if (tail == 3) {
        uint32_t v =
            ((uint32_t)k_b64u_decode_table[(unsigned char)in[i  ]] << 18) |
            ((uint32_t)k_b64u_decode_table[(unsigned char)in[i+1]] << 12) |
            ((uint32_t)k_b64u_decode_table[(unsigned char)in[i+2]] <<  6);
        out[o++] = (unsigned char)((v >> 16) & 0xFF);
        out[o++] = (unsigned char)((v >>  8) & 0xFF);
    }
    if (out_len) *out_len = o;
    return out;
}

/* =================================================================== */
/* Internal: algorithm dispatch                                         */
/* =================================================================== */

/* Returns the OpenSSL EVP_MD for the digest portion of `a`, or NULL for
   JWT_ALG_NONE / unsupported algorithms. */
static const EVP_MD* jwt_evp_md_for(JwtAlgorithm a) {
    switch (a) {
        case JWT_ALG_HS256:
        case JWT_ALG_RS256:
        case JWT_ALG_ES256:
        case JWT_ALG_PS256: return EVP_sha256();
        case JWT_ALG_HS384:
        case JWT_ALG_RS384:
        case JWT_ALG_ES384:
        case JWT_ALG_PS384: return EVP_sha384();
        case JWT_ALG_HS512:
        case JWT_ALG_RS512:
        case JWT_ALG_ES512:
        case JWT_ALG_PS512: return EVP_sha512();
        default: return NULL;
    }
}

/**
 * @brief Return the JOSE-mandated raw R||S signature length for an ES* alg.
 *
 * JWS represents ECDSA signatures as the fixed-size concatenation of the
 * R and S big-endian integers, each padded to the curve's byte length:
 *   ES256 (P-256, n≈256 bits) → 32 + 32 = 64 bytes
 *   ES384 (P-384, n≈384 bits) → 48 + 48 = 96 bytes
 *   ES512 (P-521, n=521 bits) → 66 + 66 = 132 bytes  (note 521, not 512)
 *
 * Returns 0 for non-EC algorithms — callers must guard with jwt_alg_is_ec.
 */
static size_t jwt_ec_raw_sig_len(JwtAlgorithm a) {
    switch (a) {
        case JWT_ALG_ES256: return 64;
        case JWT_ALG_ES384: return 96;
        case JWT_ALG_ES512: return 132;
        default: return 0;
    }
}

/**
 * @brief Per-curve coordinate byte length (= raw sig len / 2).
 */
static size_t jwt_ec_coord_len(JwtAlgorithm a) {
    return jwt_ec_raw_sig_len(a) / 2;
}

/**
 * @brief Minimum HMAC secret length recommended by RFC 7518 §3.2 for an alg.
 *
 * RFC 7518 mandates that the HMAC key length be at least the digest
 * output: HS256→32, HS384→48, HS512→64 bytes. Returns 0 for non-HMAC.
 */
static size_t jwt_hmac_min_key_len(JwtAlgorithm a) {
    switch (a) {
        case JWT_ALG_HS256: return 32;
        case JWT_ALG_HS384: return 48;
        case JWT_ALG_HS512: return 64;
        default: return 0;
    }
}

/* =================================================================== */
/* Introspection helpers                                                */
/* =================================================================== */

/**
 * @brief Human-readable label for a `JwtStatus` code.
 *
 * Returns a static string (`"JWT_SUCCESS"`, `"JWT_ERR_EXPIRED"`, …)
 * suitable for logs. Unknown values return `"JWT_UNKNOWN_CODE"`.
 *
 * @param s Status returned by any `jwt_*` function.
 * @return Static string. Never NULL.
 */
const char* jwt_status_to_string(JwtStatus s) {
    switch (s) {
        case JWT_SUCCESS:                return "JWT_SUCCESS";
        case JWT_ERR_INVALID_FORMAT:     return "JWT_ERR_INVALID_FORMAT";
        case JWT_ERR_INVALID_BASE64:     return "JWT_ERR_INVALID_BASE64";
        case JWT_ERR_INVALID_JSON:       return "JWT_ERR_INVALID_JSON";
        case JWT_ERR_INVALID_ALG:        return "JWT_ERR_INVALID_ALG";
        case JWT_ERR_INVALID_KEY:        return "JWT_ERR_INVALID_KEY";
        case JWT_ERR_INVALID_SIGNATURE:  return "JWT_ERR_INVALID_SIGNATURE";
        case JWT_ERR_EXPIRED:            return "JWT_ERR_EXPIRED";
        case JWT_ERR_NOT_YET_VALID:      return "JWT_ERR_NOT_YET_VALID";
        case JWT_ERR_INVALID_ISSUER:     return "JWT_ERR_INVALID_ISSUER";
        case JWT_ERR_INVALID_AUDIENCE:   return "JWT_ERR_INVALID_AUDIENCE";
        case JWT_ERR_INVALID_SUBJECT:    return "JWT_ERR_INVALID_SUBJECT";
        case JWT_ERR_MISSING_CLAIM:      return "JWT_ERR_MISSING_CLAIM";
        case JWT_ERR_CRYPTO:             return "JWT_ERR_CRYPTO";
        case JWT_ERR_MEMORY:             return "JWT_ERR_MEMORY";
        case JWT_ERR_CRIT_UNSUPPORTED:   return "JWT_ERR_CRIT_UNSUPPORTED";
        case JWT_ERR_INVALID_TYP:        return "JWT_ERR_INVALID_TYP";
        case JWT_ERR_TOKEN_TOO_LARGE:    return "JWT_ERR_TOKEN_TOO_LARGE";
        case JWT_ERR_WEAK_KEY:           return "JWT_ERR_WEAK_KEY";
        case JWT_ERR_GENERIC:            return "JWT_ERR_GENERIC";
        default:                         return "JWT_UNKNOWN_CODE";
    }
}

/**
 * @brief Canonical wire-format name of a JWT algorithm.
 *
 * Returns `"none"`, `"HS256"`, `"RS256"`, etc. — the strings RFC 7518
 * mandates inside the JOSE header's `alg` field. Unknown values return
 * `"unknown"`.
 *
 * @param a Algorithm enum.
 * @return Static string. Never NULL.
 */
const char* jwt_alg_to_string(JwtAlgorithm a) {
    switch (a) {
        case JWT_ALG_NONE:  return "none";
        case JWT_ALG_HS256: return "HS256";
        case JWT_ALG_HS384: return "HS384";
        case JWT_ALG_HS512: return "HS512";
        case JWT_ALG_RS256: return "RS256";
        case JWT_ALG_RS384: return "RS384";
        case JWT_ALG_RS512: return "RS512";
        case JWT_ALG_ES256: return "ES256";
        case JWT_ALG_ES384: return "ES384";
        case JWT_ALG_ES512: return "ES512";
        case JWT_ALG_PS256: return "PS256";
        case JWT_ALG_PS384: return "PS384";
        case JWT_ALG_PS512: return "PS512";
        default:            return "unknown";
    }
}

/**
 * @brief Parse a JWT algorithm name string into the enum.
 *
 * Recognizes "none", "HS256", "HS384", "HS512", "RS256", "RS384",
 * "RS512" (case-sensitive — RFC 7518 is case-sensitive). Anything else
 * returns `JWT_ALG_NONE`. NULL input also returns `JWT_ALG_NONE`.
 *
 * @param s Algorithm name as it appears in a JOSE header.
 * @return Enum value, or `JWT_ALG_NONE` on no match.
 */
JwtAlgorithm jwt_alg_from_string(const char* s) {
    if (!s) return JWT_ALG_NONE;
    if (strcmp(s, "HS256") == 0) return JWT_ALG_HS256;
    if (strcmp(s, "HS384") == 0) return JWT_ALG_HS384;
    if (strcmp(s, "HS512") == 0) return JWT_ALG_HS512;
    if (strcmp(s, "RS256") == 0) return JWT_ALG_RS256;
    if (strcmp(s, "RS384") == 0) return JWT_ALG_RS384;
    if (strcmp(s, "RS512") == 0) return JWT_ALG_RS512;
    if (strcmp(s, "ES256") == 0) return JWT_ALG_ES256;
    if (strcmp(s, "ES384") == 0) return JWT_ALG_ES384;
    if (strcmp(s, "ES512") == 0) return JWT_ALG_ES512;
    if (strcmp(s, "PS256") == 0) return JWT_ALG_PS256;
    if (strcmp(s, "PS384") == 0) return JWT_ALG_PS384;
    if (strcmp(s, "PS512") == 0) return JWT_ALG_PS512;
    if (strcmp(s, "none")  == 0) return JWT_ALG_NONE;
    return JWT_ALG_NONE;
}

/**
 * @brief Whether the algorithm uses an HMAC (symmetric) key.
 * @return true for HS256/HS384/HS512, false otherwise.
 */
bool jwt_alg_is_hmac(JwtAlgorithm a) {
    return a == JWT_ALG_HS256 || a == JWT_ALG_HS384 || a == JWT_ALG_HS512;
}

/**
 * @brief Whether the algorithm is RSA-PKCS#1-v1.5 (RS256/384/512).
 *
 * Note this does NOT include the PS* family — those also use RSA keys
 * but apply PSS padding instead. Use `jwt_alg_uses_rsa_key` if you need
 * to ask "does this alg consume an RSA key, regardless of padding".
 *
 * @return true for RS256/RS384/RS512, false otherwise.
 */
bool jwt_alg_is_rsa(JwtAlgorithm a) {
    return a == JWT_ALG_RS256 || a == JWT_ALG_RS384 || a == JWT_ALG_RS512;
}

/**
 * @brief Whether the algorithm is RSASSA-PSS (PS256/384/512, RFC 7518 §3.5).
 *
 * PS* uses the same RSA key as RS* (same EVP_PKEY type) but applies PSS
 * padding with MGF1 over the same digest, salt length = digest length.
 *
 * @return true for PS256/PS384/PS512.
 */
bool jwt_alg_is_rsa_pss(JwtAlgorithm a) {
    return a == JWT_ALG_PS256 || a == JWT_ALG_PS384 || a == JWT_ALG_PS512;
}

/**
 * @brief Whether the algorithm consumes an RSA-shaped key (RS* or PS*).
 *
 * Useful when deciding which key constructor to call from a JWK whose
 * `kty=RSA` — the same key works for both families.
 *
 * @return true for any RS* or PS* algorithm.
 */
bool jwt_alg_uses_rsa_key(JwtAlgorithm a) {
    return jwt_alg_is_rsa(a) || jwt_alg_is_rsa_pss(a);
}

/**
 * @brief Whether the algorithm uses an ECDSA (elliptic-curve) key.
 * @return true for ES256/ES384/ES512, false otherwise.
 */
bool jwt_alg_is_ec(JwtAlgorithm a) {
    return a == JWT_ALG_ES256 || a == JWT_ALG_ES384 || a == JWT_ALG_ES512;
}

/**
 * @brief Whether `pkey` is a curve whose order length matches `alg`.
 *
 * RFC 7518 §3.4 binds each ES* alg to a specific curve:
 *   ES256 → P-256, ES384 → P-384, ES512 → P-521.
 *
 * Returns true if the binding holds, false otherwise. Used by the encoder
 * and verifier to reject mismatched curve/alg pairs *before* doing crypto.
 */
static bool jwt_ec_curve_matches_alg(EVP_PKEY* pkey, JwtAlgorithm alg) {
    if (!pkey) return false;
    int nid_expected = NID_undef;
    switch (alg) {
        case JWT_ALG_ES256: nid_expected = NID_X9_62_prime256v1; break;
        case JWT_ALG_ES384: nid_expected = NID_secp384r1;        break;
        case JWT_ALG_ES512: nid_expected = NID_secp521r1;        break;
        default: return false;
    }
    int nid_actual = NID_undef;
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    char curve_name[80];
    size_t curve_name_len = 0;
    if (EVP_PKEY_get_group_name(pkey, curve_name, sizeof(curve_name), &curve_name_len) == 1) {
        nid_actual = OBJ_txt2nid(curve_name);
    }
#else
    EC_KEY* ec = EVP_PKEY_get0_EC_KEY(pkey);
    if (ec) {
        const EC_GROUP* g = EC_KEY_get0_group(ec);
        if (g) nid_actual = EC_GROUP_get_curve_name(g);
    }
#endif
    return nid_actual == nid_expected;
}

/* =================================================================== */
/* Keys                                                                 */
/* =================================================================== */

/**
 * @brief Build an HMAC signing/verification key from raw secret bytes.
 *
 * The same key object is usable for both signing and verification. The
 * secret is copied — the caller may free the source buffer immediately
 * after this returns.
 *
 * @param secret Raw secret bytes (any binary content).
 * @param secret_len Length in bytes. May be 0 (degenerate but legal).
 * @return Newly-allocated key, or NULL on OOM / NULL secret with len>0.
 */
JwtKey* jwt_key_hmac(const unsigned char* secret, size_t secret_len) {
    if (!secret && secret_len > 0) {
        JWT_LOG("[jwt_key_hmac]: NULL secret with non-zero length");
        return NULL;
    }

    JwtKey* k = (JwtKey*)calloc(1, sizeof(JwtKey));
    if (!k) {
        JWT_LOG("[jwt_key_hmac]: calloc(JwtKey) failed");
        return NULL;
    }
    k->kind = JWT_KEY_KIND_HMAC;
    k->secret = (unsigned char*)malloc(secret_len ? secret_len : 1);
    if (!k->secret) {
        JWT_LOG("[jwt_key_hmac]: malloc(secret) failed");
        free(k);
        return NULL;
    }
    if (secret_len) memcpy(k->secret, secret, secret_len);
    k->secret_len = secret_len;
    JWT_LOG("[jwt_key_hmac]: built HMAC key (%zu bytes)", secret_len);
    return k;
}

/* Common helper for the two RSA-from-PEM builders. */
static JwtKey* jwt_key_rsa_from_bio(BIO* bio, bool is_private) {
    EVP_PKEY* pkey = is_private
        ? PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL)
        : PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
    if (!pkey) {
        JWT_LOG("RSA PEM parse failed");
        return NULL;
    }
    /* Reject non-RSA keys so an EC PEM can't masquerade as RSA and
       silently end up signing with the wrong primitive. */
    int t =
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
        EVP_PKEY_get_base_id(pkey);
#else
        EVP_PKEY_base_id(pkey);
#endif
    if (t != EVP_PKEY_RSA) {
        JWT_LOG("RSA loader fed a non-RSA key (key type=%d)", t);
        EVP_PKEY_free(pkey);
        return NULL;
    }
    JwtKey* k = (JwtKey*)calloc(1, sizeof(JwtKey));
    if (!k) { EVP_PKEY_free(pkey); return NULL; }
    k->kind = is_private ? JWT_KEY_KIND_RSA_PRIVATE : JWT_KEY_KIND_RSA_PUBLIC;
    k->pkey = pkey;
    return k;
}

/**
 * @brief Load an RSA private key from a PEM-formatted memory buffer.
 *
 * Accepts both PKCS#1 (`-----BEGIN RSA PRIVATE KEY-----`) and PKCS#8
 * (`-----BEGIN PRIVATE KEY-----`) envelopes. Used for signing RS256/384/512
 * tokens. Returns NULL on parse failure or OOM.
 *
 * @param pem Pointer to PEM bytes.
 * @param pem_len Length of `pem` in bytes (does not need a trailing NUL).
 * @return Newly-allocated key, or NULL.
 */
JwtKey* jwt_key_rsa_private_pem(const char* pem, size_t pem_len) {
    if (!pem || pem_len == 0) {
        JWT_LOG("[jwt_key_rsa_private_pem]: NULL or empty PEM buffer");
        return NULL;
    }
    BIO* bio = BIO_new_mem_buf(pem, (int)pem_len);
    if (!bio) {
        JWT_LOG("[jwt_key_rsa_private_pem]: BIO_new_mem_buf failed");
        return NULL;
    }
    JwtKey* k = jwt_key_rsa_from_bio(bio, true);
    BIO_free(bio);
    if (!k) JWT_LOG("[jwt_key_rsa_private_pem]: PEM parse failed");
    return k;
}

/**
 * @brief Load an RSA public key from a PEM-formatted memory buffer.
 *
 * Accepts the SubjectPublicKeyInfo form (`-----BEGIN PUBLIC KEY-----`).
 * Used for verifying RS256/384/512 tokens. Returns NULL on parse failure.
 *
 * @param pem Pointer to PEM bytes.
 * @param pem_len Length in bytes.
 * @return Newly-allocated key, or NULL.
 */
JwtKey* jwt_key_rsa_public_pem(const char* pem, size_t pem_len) {
    if (!pem || pem_len == 0) {
        JWT_LOG("[jwt_key_rsa_public_pem]: NULL or empty PEM buffer");
        return NULL;
    }
    BIO* bio = BIO_new_mem_buf(pem, (int)pem_len);
    if (!bio) {
        JWT_LOG("[jwt_key_rsa_public_pem]: BIO_new_mem_buf failed");
        return NULL;
    }
    JwtKey* k = jwt_key_rsa_from_bio(bio, false);
    BIO_free(bio);
    if (!k) JWT_LOG("[jwt_key_rsa_public_pem]: PEM parse failed");
    return k;
}

/* Helper: load a whole file into memory. Caller frees *out. */
static bool jwt_slurp_file(const char* path, char** out, size_t* out_len) {
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return false; }
    long n = ftell(f);
    if (n < 0) { fclose(f); return false; }
    rewind(f);
    char* buf = (char*)malloc((size_t)n + 1);
    if (!buf) { fclose(f); return false; }
    size_t r = fread(buf, 1, (size_t)n, f);
    fclose(f);
    if (r != (size_t)n) { free(buf); return false; }
    buf[n] = '\0';
    *out = buf;
    *out_len = (size_t)n;
    return true;
}

/**
 * @brief Convenience: load an RSA private key from a PEM file on disk.
 *
 * Slurps the entire file, then delegates to `jwt_key_rsa_private_pem`.
 *
 * @param path Filesystem path to a PEM file.
 * @return Newly-allocated key, or NULL on read / parse failure.
 */
JwtKey* jwt_key_rsa_private_file(const char* path) {
    if (!path) {
        JWT_LOG("[jwt_key_rsa_private_file]: NULL path");
        return NULL;
    }
    char* buf = NULL; size_t n = 0;
    if (!jwt_slurp_file(path, &buf, &n)) {
        JWT_LOG("[jwt_key_rsa_private_file]: failed to read '%s'", path);
        return NULL;
    }
    JwtKey* k = jwt_key_rsa_private_pem(buf, n);
    free(buf);
    return k;
}

/**
 * @brief Convenience: load an RSA public key from a PEM file on disk.
 *
 * Slurps the entire file, then delegates to `jwt_key_rsa_public_pem`.
 *
 * @param path Filesystem path to a PEM file.
 * @return Newly-allocated key, or NULL on read / parse failure.
 */
JwtKey* jwt_key_rsa_public_file(const char* path) {
    if (!path) {
        JWT_LOG("[jwt_key_rsa_public_file]: NULL path");
        return NULL;
    }
    char* buf = NULL; size_t n = 0;
    if (!jwt_slurp_file(path, &buf, &n)) {
        JWT_LOG("[jwt_key_rsa_public_file]: failed to read '%s'", path);
        return NULL;
    }
    JwtKey* k = jwt_key_rsa_public_pem(buf, n);
    free(buf);
    return k;
}

/* Common helper for the two EC-from-PEM builders. */
static JwtKey* jwt_key_ec_from_bio(BIO* bio, bool is_private) {
    EVP_PKEY* pkey = is_private
        ? PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL)
        : PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
    if (!pkey) {
        JWT_LOG("EC PEM parse failed");
        return NULL;
    }
    /* Reject non-EC keys masquerading via the EC entrypoints. */
    int t =
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
        EVP_PKEY_get_base_id(pkey);
#else
        EVP_PKEY_base_id(pkey);
#endif
    if (t != EVP_PKEY_EC) {
        JWT_LOG("EC loader fed a non-EC key");
        EVP_PKEY_free(pkey);
        return NULL;
    }
    JwtKey* k = (JwtKey*)calloc(1, sizeof(JwtKey));
    if (!k) { EVP_PKEY_free(pkey); return NULL; }
    k->kind = is_private ? JWT_KEY_KIND_EC_PRIVATE : JWT_KEY_KIND_EC_PUBLIC;
    k->pkey = pkey;
    return k;
}

/**
 * @brief Load an EC private key from a PEM-formatted memory buffer.
 *
 * Accepts both SEC1 (`-----BEGIN EC PRIVATE KEY-----`) and PKCS#8
 * (`-----BEGIN PRIVATE KEY-----`) envelopes. The curve is read from the
 * key; the JWT algorithm chosen at encode time must match it. Used for
 * **signing** ES256/384/512 tokens.
 *
 * @param pem      Pointer to PEM bytes.
 * @param pem_len  Length in bytes (no trailing NUL needed).
 * @return Newly-allocated key, or NULL on parse failure / non-EC key / OOM.
 */
JwtKey* jwt_key_ec_private_pem(const char* pem, size_t pem_len) {
    if (!pem || pem_len == 0) {
        JWT_LOG("[jwt_key_ec_private_pem]: NULL/empty PEM");
        return NULL;
    }
    BIO* bio = BIO_new_mem_buf(pem, (int)pem_len);
    if (!bio) return NULL;
    JwtKey* k = jwt_key_ec_from_bio(bio, true);
    BIO_free(bio);
    return k;
}

/**
 * @brief Load an EC public key from a PEM-formatted memory buffer.
 *
 * Accepts the SubjectPublicKeyInfo envelope (`-----BEGIN PUBLIC KEY-----`).
 * Used for **verifying** ES256/384/512 tokens.
 *
 * @param pem      Pointer to PEM bytes.
 * @param pem_len  Length in bytes.
 * @return Newly-allocated key, or NULL.
 */
JwtKey* jwt_key_ec_public_pem(const char* pem, size_t pem_len) {
    if (!pem || pem_len == 0) {
        JWT_LOG("[jwt_key_ec_public_pem]: NULL/empty PEM");
        return NULL;
    }
    BIO* bio = BIO_new_mem_buf(pem, (int)pem_len);
    if (!bio) return NULL;
    JwtKey* k = jwt_key_ec_from_bio(bio, false);
    BIO_free(bio);
    return k;
}

/**
 * @brief Convenience: load an EC private key from a PEM file on disk.
 * @param path Filesystem path to a PEM file.
 * @return Newly-allocated key, or NULL on read / parse failure.
 */
JwtKey* jwt_key_ec_private_file(const char* path) {
    if (!path) return NULL;
    char* buf = NULL; size_t n = 0;
    if (!jwt_slurp_file(path, &buf, &n)) return NULL;
    JwtKey* k = jwt_key_ec_private_pem(buf, n);
    free(buf);
    return k;
}

/**
 * @brief Convenience: load an EC public key from a PEM file on disk.
 * @param path Filesystem path to a PEM file.
 * @return Newly-allocated key, or NULL on read / parse failure.
 */
JwtKey* jwt_key_ec_public_file(const char* path) {
    if (!path) return NULL;
    char* buf = NULL; size_t n = 0;
    if (!jwt_slurp_file(path, &buf, &n)) return NULL;
    JwtKey* k = jwt_key_ec_public_pem(buf, n);
    free(buf);
    return k;
}

/**
 * @brief Release a JwtKey and zero its sensitive bytes.
 *
 * HMAC secrets are wiped with `OPENSSL_cleanse` before `free()` so they
 * don't linger on the heap. NULL is a safe no-op.
 *
 * @param key Key to release.
 */
void jwt_key_deallocate(JwtKey* key) {
    if (!key) {
        JWT_LOG("[jwt_key_deallocate]: NULL key (no-op)");
        return;
    }
    if (key->secret) {
        OPENSSL_cleanse(key->secret, key->secret_len);
        free(key->secret);
    }
    if (key->pkey) EVP_PKEY_free(key->pkey);
    free(key);
    JWT_LOG("[jwt_key_deallocate]: key released");
}

/* =================================================================== */
/* JWKS (RFC 7517) — JSON Web Key Set                                   */
/* =================================================================== */

typedef struct {
    char*   kid;     /* may be NULL */
    char*   alg;     /* may be NULL — e.g. "RS256", "ES256", "HS256" */
    JwtKey* key;     /* owned by the set */
} JwksEntry;

struct JwtKeySet {
    JwksEntry* entries;
    size_t     count;
    size_t     cap;
};

/* Decode a base64url-encoded BIGNUM-style integer (modulus, exponent,
   EC coordinate). Returns a malloc'd byte buffer; caller frees. */
static unsigned char* jwks_decode_b64u_field(JsonElement* obj, const char* name,
                                             size_t* out_len) {
    JsonElement* e = json_get_element(obj, name);
    if (!e || e->type != JSON_STRING || !e->value.string_val) return NULL;
    return jwt_base64url_decode(e->value.string_val,
                                strlen(e->value.string_val), out_len);
}

/* Build an RSA public EVP_PKEY from raw modulus + exponent bytes.
   Uses OpenSSL 3.x's OSSL_PARAM_BLD API; on older OpenSSL falls back to
   the deprecated RSA_new() / EVP_PKEY_assign_RSA() path. */
static EVP_PKEY* jwks_make_rsa_pubkey(const unsigned char* n_bytes, size_t n_len,
                                      const unsigned char* e_bytes, size_t e_len) {
    if (!n_bytes || !e_bytes || n_len == 0 || e_len == 0) return NULL;
    BIGNUM* bn_n = BN_bin2bn(n_bytes, (int)n_len, NULL);
    BIGNUM* bn_e = BN_bin2bn(e_bytes, (int)e_len, NULL);
    if (!bn_n || !bn_e) {
        if (bn_n) BN_free(bn_n);
        if (bn_e) BN_free(bn_e);
        return NULL;
    }
    EVP_PKEY* pkey = NULL;

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    OSSL_PARAM_BLD* bld = OSSL_PARAM_BLD_new();
    if (bld &&
        OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_N, bn_n) &&
        OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_E, bn_e)) {
        OSSL_PARAM* params = OSSL_PARAM_BLD_to_param(bld);
        EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_from_name(NULL, "RSA", NULL);
        if (pctx && params) {
            if (EVP_PKEY_fromdata_init(pctx) == 1) {
                EVP_PKEY_fromdata(pctx, &pkey, EVP_PKEY_PUBLIC_KEY, params);
            }
        }
        if (params) OSSL_PARAM_free(params);
        if (pctx)   EVP_PKEY_CTX_free(pctx);
    }
    if (bld) OSSL_PARAM_BLD_free(bld);
#else
    RSA* rsa = RSA_new();
    if (rsa && RSA_set0_key(rsa, bn_n, bn_e, NULL) == 1) {
        pkey = EVP_PKEY_new();
        if (!pkey || EVP_PKEY_assign_RSA(pkey, rsa) != 1) {
            if (pkey) EVP_PKEY_free(pkey);
            RSA_free(rsa);
            pkey = NULL;
        }
        /* RSA now owned by pkey on success; do not free bn_n/bn_e. */
        bn_n = bn_e = NULL;
    } else if (rsa) {
        RSA_free(rsa);
    }
#endif

    if (bn_n) BN_free(bn_n);
    if (bn_e) BN_free(bn_e);
    return pkey;
}

/* Build an EC public EVP_PKEY from the curve name (e.g. "P-256") plus
   raw X and Y coordinates. */
static EVP_PKEY* jwks_make_ec_pubkey(const char* crv,
                                     const unsigned char* x_bytes, size_t x_len,
                                     const unsigned char* y_bytes, size_t y_len) {
    if (!crv || !x_bytes || !y_bytes) return NULL;

    /* Map JOSE curve names (RFC 7518 §6.2.1.1) to OpenSSL names. */
    const char* ossl_name = NULL;
    int          nid       = NID_undef;
    size_t       coord_len = 0;
    if      (strcmp(crv, "P-256") == 0) { ossl_name = "P-256"; nid = NID_X9_62_prime256v1; coord_len = 32; }
    else if (strcmp(crv, "P-384") == 0) { ossl_name = "P-384"; nid = NID_secp384r1;        coord_len = 48; }
    else if (strcmp(crv, "P-521") == 0) { ossl_name = "P-521"; nid = NID_secp521r1;        coord_len = 66; }
    else return NULL;
    (void)nid;  /* used only in the OpenSSL 1.x EC_KEY_new_by_curve_name branch */

    /* Coordinates must be exactly the curve byte length. */
    if (x_len != coord_len || y_len != coord_len) return NULL;

    /* Build the uncompressed point: 0x04 || X || Y. */
    size_t point_len = 1 + coord_len * 2;
    unsigned char* point = (unsigned char*)malloc(point_len);
    if (!point) return NULL;
    point[0] = 0x04;
    memcpy(point + 1,             x_bytes, coord_len);
    memcpy(point + 1 + coord_len, y_bytes, coord_len);

    EVP_PKEY* pkey = NULL;

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
    OSSL_PARAM_BLD* bld = OSSL_PARAM_BLD_new();
    if (bld &&
        OSSL_PARAM_BLD_push_utf8_string(bld, OSSL_PKEY_PARAM_GROUP_NAME, ossl_name, 0) &&
        OSSL_PARAM_BLD_push_octet_string(bld, OSSL_PKEY_PARAM_PUB_KEY, point, point_len)) {
        OSSL_PARAM* params = OSSL_PARAM_BLD_to_param(bld);
        EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
        if (pctx && params) {
            if (EVP_PKEY_fromdata_init(pctx) == 1) {
                EVP_PKEY_fromdata(pctx, &pkey, EVP_PKEY_PUBLIC_KEY, params);
            }
        }
        if (params) OSSL_PARAM_free(params);
        if (pctx)   EVP_PKEY_CTX_free(pctx);
    }
    if (bld) OSSL_PARAM_BLD_free(bld);
#else
    EC_KEY* ec = EC_KEY_new_by_curve_name(nid);
    if (ec) {
        const EC_GROUP* g = EC_KEY_get0_group(ec);
        EC_POINT* pt = EC_POINT_new(g);
        if (pt && EC_POINT_oct2point(g, pt, point, point_len, NULL) == 1 &&
            EC_KEY_set_public_key(ec, pt) == 1) {
            pkey = EVP_PKEY_new();
            if (!pkey || EVP_PKEY_assign_EC_KEY(pkey, ec) != 1) {
                if (pkey) EVP_PKEY_free(pkey);
                EC_KEY_free(ec);
                pkey = NULL;
            }
            ec = NULL; /* owned by pkey now */
        }
        if (pt) EC_POINT_free(pt);
        if (ec) EC_KEY_free(ec);
    }
    (void)nid;
#endif

    free(point);
    return pkey;
}

/* Build a single JwksEntry from a parsed JWK object. Returns false (and
   leaves *out untouched) if the entry should be skipped — e.g. unknown
   kty, missing required fields, use=enc. The set-level parser keeps
   going so one bad entry doesn't poison the whole set. */
static bool jwks_build_entry(JsonElement* jwk, JwksEntry* out) {
    memset(out, 0, sizeof(*out));

    JsonElement* use_e = json_get_element(jwk, "use");
    if (use_e && use_e->type == JSON_STRING && use_e->value.string_val &&
        strcmp(use_e->value.string_val, "enc") == 0) {
        JWT_LOG("[jwks]: skipping use=enc entry");
        return false;
    }

    JsonElement* kty_e = json_get_element(jwk, "kty");
    if (!kty_e || kty_e->type != JSON_STRING || !kty_e->value.string_val) {
        JWT_LOG("[jwks]: skipping entry without kty");
        return false;
    }
    const char* kty = kty_e->value.string_val;

    JwtKey* k = NULL;

    if (strcmp(kty, "RSA") == 0) {
        size_t n_len = 0, e_len = 0;
        unsigned char* n_bytes = jwks_decode_b64u_field(jwk, "n", &n_len);
        unsigned char* e_bytes = jwks_decode_b64u_field(jwk, "e", &e_len);
        if (!n_bytes || !e_bytes) {
            free(n_bytes); free(e_bytes);
            JWT_LOG("[jwks]: RSA entry missing or invalid n/e");
            return false;
        }
        EVP_PKEY* pkey = jwks_make_rsa_pubkey(n_bytes, n_len, e_bytes, e_len);
        free(n_bytes); free(e_bytes);
        if (!pkey) {
            JWT_LOG("[jwks]: failed to build RSA EVP_PKEY");
            return false;
        }
        k = (JwtKey*)calloc(1, sizeof(JwtKey));
        if (!k) { EVP_PKEY_free(pkey); return false; }
        k->kind = JWT_KEY_KIND_RSA_PUBLIC;
        k->pkey = pkey;

    } else if (strcmp(kty, "EC") == 0) {
        JsonElement* crv_e = json_get_element(jwk, "crv");
        if (!crv_e || crv_e->type != JSON_STRING || !crv_e->value.string_val) {
            JWT_LOG("[jwks]: EC entry missing crv");
            return false;
        }
        size_t x_len = 0, y_len = 0;
        unsigned char* x_bytes = jwks_decode_b64u_field(jwk, "x", &x_len);
        unsigned char* y_bytes = jwks_decode_b64u_field(jwk, "y", &y_len);
        if (!x_bytes || !y_bytes) {
            free(x_bytes); free(y_bytes);
            JWT_LOG("[jwks]: EC entry missing x/y");
            return false;
        }
        EVP_PKEY* pkey = jwks_make_ec_pubkey(crv_e->value.string_val,
                                             x_bytes, x_len, y_bytes, y_len);
        free(x_bytes); free(y_bytes);
        if (!pkey) {
            JWT_LOG("[jwks]: failed to build EC EVP_PKEY (crv=%s)",
                    crv_e->value.string_val);
            return false;
        }
        k = (JwtKey*)calloc(1, sizeof(JwtKey));
        if (!k) { EVP_PKEY_free(pkey); return false; }
        k->kind = JWT_KEY_KIND_EC_PUBLIC;
        k->pkey = pkey;

    } else if (strcmp(kty, "oct") == 0) {
        size_t k_len = 0;
        unsigned char* k_bytes = jwks_decode_b64u_field(jwk, "k", &k_len);
        if (!k_bytes) {
            JWT_LOG("[jwks]: oct entry missing k");
            return false;
        }
        k = jwt_key_hmac(k_bytes, k_len);
        /* k_bytes was copied inside jwt_key_hmac. Wipe before free. */
        OPENSSL_cleanse(k_bytes, k_len);
        free(k_bytes);
        if (!k) {
            JWT_LOG("[jwks]: failed to build HMAC key from oct entry");
            return false;
        }

    } else {
        JWT_LOG("[jwks]: skipping unsupported kty='%s'", kty);
        return false;
    }

    /* Optional metadata. */
    JsonElement* kid_e = json_get_element(jwk, "kid");
    if (kid_e && kid_e->type == JSON_STRING && kid_e->value.string_val) {
        out->kid = string_strdup(kid_e->value.string_val);
    }
    JsonElement* alg_e = json_get_element(jwk, "alg");
    if (alg_e && alg_e->type == JSON_STRING && alg_e->value.string_val) {
        out->alg = string_strdup(alg_e->value.string_val);
    }
    out->key = k;
    return true;
}

/* Grow the entries array geometrically. */
static bool jwks_reserve(JwtKeySet* set, size_t need) {
    if (set->cap >= need) return true;
    size_t cap = set->cap ? set->cap : 4;
    while (cap < need) cap *= 2;
    JwksEntry* e = (JwksEntry*)realloc(set->entries, cap * sizeof(JwksEntry));
    if (!e) return false;
    set->entries = e;
    set->cap = cap;
    return true;
}

/**
 * @brief Parse a JWKS document into a `JwtKeySet`.
 *
 * The input is the JSON object an OIDC IdP publishes at its
 * `/.well-known/jwks.json` endpoint. The top-level shape must be
 * `{"keys": [...]}`; each member of `keys` is a JWK (RFC 7517 §4).
 * Supported `kty`:
 *   - `RSA` — fields `n`, `e` (base64url big-endian, no padding)
 *   - `EC`  — fields `crv` (`P-256` / `P-384` / `P-521`), `x`, `y`
 *   - `oct` — field `k` (base64url-encoded HMAC secret)
 *
 * Entries with `use=enc`, missing required fields, or an unsupported
 * `kty` / `crv` are silently skipped — they do NOT cause the whole
 * parse to fail. This is deliberate: real-world IdPs sometimes publish
 * keys this verifier doesn't speak (Ed25519, etc.), and we'd rather
 * load the good ones than reject the whole set.
 *
 * @param jwks_json Null-terminated JSON. Need not be the file shape exactly —
 *                  any top-level object with a `keys` array works.
 * @return A newly-allocated keyset (possibly empty if no entries were
 *         parsable), or NULL on JSON parse failure / OOM. Caller frees
 *         with `jwt_keyset_deallocate`.
 */
JwtKeySet* jwt_keyset_from_json(const char* jwks_json) {
    if (!jwks_json) {
        JWT_LOG("[jwt_keyset_from_json]: NULL input");
        return NULL;
    }
    JsonElement* root = json_parse(jwks_json);
    if (!root || root->type != JSON_OBJECT) {
        if (root) json_deallocate(root);
        JWT_LOG("[jwt_keyset_from_json]: JSON parse failed or root is not an object");
        return NULL;
    }
    JsonElement* keys = json_get_element(root, "keys");
    if (!keys || keys->type != JSON_ARRAY) {
        json_deallocate(root);
        JWT_LOG("[jwt_keyset_from_json]: missing top-level 'keys' array");
        return NULL;
    }

    JwtKeySet* set = (JwtKeySet*)calloc(1, sizeof(JwtKeySet));
    if (!set) { json_deallocate(root); return NULL; }

    size_t n = json_array_size(keys);
    if (!jwks_reserve(set, n)) {
        json_deallocate(root);
        jwt_keyset_deallocate(set);
        return NULL;
    }

    for (size_t i = 0; i < n; ++i) {
        char idx[24]; snprintf(idx, sizeof(idx), "%zu", i);
        JsonElement* jwk = json_get_element(keys, idx);
        if (!jwk || jwk->type != JSON_OBJECT) continue;
        JwksEntry e;
        if (jwks_build_entry(jwk, &e)) {
            set->entries[set->count++] = e;
        }
    }

    json_deallocate(root);
    JWT_LOG("[jwt_keyset_from_json]: parsed %zu key(s)", set->count);
    return set;
}

/**
 * @brief Slurp a JWKS file from disk and parse it.
 *
 * Thin wrapper over `jwt_keyset_from_json`.
 *
 * @param path Filesystem path to a JWKS JSON document.
 * @return Newly-allocated keyset or NULL.
 */
JwtKeySet* jwt_keyset_from_file(const char* path) {
    if (!path) return NULL;
    char* buf = NULL; size_t n = 0;
    if (!jwt_slurp_file(path, &buf, &n)) {
        JWT_LOG("[jwt_keyset_from_file]: failed to read '%s'", path);
        return NULL;
    }
    /* Need NUL-terminated text for json_parse. */
    buf[n] = '\0';
    JwtKeySet* set = jwt_keyset_from_json(buf);
    free(buf);
    return set;
}

/**
 * @brief Look up a key by its `kid`. Returns a borrowed pointer.
 *
 * The returned pointer is owned by the set — do NOT call
 * `jwt_key_deallocate` on it. NULL if `kid` is NULL or no match exists.
 *
 * @param set Parsed keyset.
 * @param kid kid value to match (case-sensitive).
 * @return Borrowed JwtKey* or NULL.
 */
const JwtKey* jwt_keyset_lookup(const JwtKeySet* set, const char* kid) {
    if (!set || !kid) return NULL;
    for (size_t i = 0; i < set->count; ++i) {
        if (set->entries[i].kid && strcmp(set->entries[i].kid, kid) == 0) {
            return set->entries[i].key;
        }
    }
    return NULL;
}

/** @brief Number of keys successfully parsed. */
size_t jwt_keyset_size(const JwtKeySet* set) {
    return set ? set->count : 0;
}

/** @brief Borrowed key at index `i`, or NULL if out of range. */
const JwtKey* jwt_keyset_at(const JwtKeySet* set, size_t i) {
    return (set && i < set->count) ? set->entries[i].key : NULL;
}

/** @brief `kid` of the entry at index `i`, or NULL if missing / out of range. */
const char* jwt_keyset_kid_at(const JwtKeySet* set, size_t i) {
    return (set && i < set->count) ? set->entries[i].kid : NULL;
}

/** @brief `alg` of the entry at index `i`, or NULL if missing / out of range. */
const char* jwt_keyset_alg_at(const JwtKeySet* set, size_t i) {
    return (set && i < set->count) ? set->entries[i].alg : NULL;
}

/**
 * @brief Release the keyset and every key inside it.
 *
 * NULL is a safe no-op.
 *
 * @param set Set to release.
 */
void jwt_keyset_deallocate(JwtKeySet* set) {
    if (!set) return;
    for (size_t i = 0; i < set->count; ++i) {
        free(set->entries[i].kid);
        free(set->entries[i].alg);
        jwt_key_deallocate(set->entries[i].key);
    }
    free(set->entries);
    free(set);
}

/* =================================================================== */
/* Builder                                                              */
/* =================================================================== */

/**
 * @brief Allocate a new JwtToken and seed its JOSE header.
 *
 * Sets `typ = "JWT"` and `alg = <name(alg)>` on the header object. The
 * payload object is created empty — populate it with the
 * `jwt_set_*` helpers, then call `jwt_encode` with a signing key.
 *
 * @param alg The algorithm this token will be signed with.
 * @return Newly-allocated token, or NULL on OOM.
 */
JwtToken* jwt_create(JwtAlgorithm alg) {
    JwtToken* t = (JwtToken*)calloc(1, sizeof(JwtToken));
    if (!t) {
        JWT_LOG("[jwt_create]: calloc(JwtToken) failed");
        return NULL;
    }
    t->alg = alg;
    t->header  = json_create(JSON_OBJECT);
    t->payload = json_create(JSON_OBJECT);
    if (!t->header || !t->payload) {
        JWT_LOG("[jwt_create]: header/payload allocation failed");
        jwt_deallocate(t);
        return NULL;
    }

    JsonElement* typ = json_create(JSON_STRING);
    JsonElement* algv = json_create(JSON_STRING);
    if (!typ || !algv) {
        JWT_LOG("[jwt_create]: header value allocation failed");
        jwt_deallocate(t);
        if (typ)  json_deallocate(typ);
        if (algv) json_deallocate(algv);
        return NULL;
    }
    typ->value.string_val  = string_strdup("JWT");
    algv->value.string_val = string_strdup(jwt_alg_to_string(alg));
    if (!typ->value.string_val || !algv->value.string_val) {
        JWT_LOG("[jwt_create]: header value strdup failed");
        json_deallocate(typ); json_deallocate(algv); jwt_deallocate(t);
        return NULL;
    }
    json_add_to_object(t->header, "typ", typ);
    json_add_to_object(t->header, "alg", algv);
    JWT_LOG("[jwt_create]: token created (alg=%s)", jwt_alg_to_string(alg));
    return t;
}

/* Internal helper: set a string-valued claim on a target object.
   Removes any existing claim with the same name first so repeated calls
   don't accumulate duplicates and don't leak the prior value. */
static bool jwt_obj_set_string(JsonElement* obj, const char* name, const char* value) {
    if (!obj || !name || !value) return false;
    JsonElement* v = json_create(JSON_STRING);
    if (!v) return false;
    v->value.string_val = string_strdup(value);
    if (!v->value.string_val) { json_deallocate(v); return false; }
    if (json_get_element(obj, name) != NULL) {
        json_remove_element(obj, name);
    }
    if (!json_add_to_object(obj, name, v)) { json_deallocate(v); return false; }
    return true;
}

static bool jwt_obj_set_number(JsonElement* obj, const char* name, double value) {
    if (!obj || !name) return false;
    JsonElement* v = json_create(JSON_NUMBER);
    if (!v) return false;
    v->value.number_val = value;
    if (json_get_element(obj, name) != NULL) {
        json_remove_element(obj, name);
    }
    if (!json_add_to_object(obj, name, v)) { json_deallocate(v); return false; }
    return true;
}

static bool jwt_obj_set_bool(JsonElement* obj, const char* name, bool value) {
    if (!obj || !name) return false;
    JsonElement* v = json_create(JSON_BOOL);
    if (!v) return false;
    v->value.bool_val = value;
    if (json_get_element(obj, name) != NULL) {
        json_remove_element(obj, name);
    }
    if (!json_add_to_object(obj, name, v)) { json_deallocate(v); return false; }
    return true;
}

/**
 * @brief Set the `iss` (issuer) standard claim. NULL inputs are no-ops.
 */
void jwt_set_issuer(JwtToken* t, const char* iss) {
    if (!t || !iss) return;
    jwt_obj_set_string(t->payload, "iss", iss);
}

/**
 * @brief Set the `sub` (subject) standard claim. NULL inputs are no-ops.
 */
void jwt_set_subject(JwtToken* t, const char* sub) {
    if (!t || !sub) return;
    jwt_obj_set_string(t->payload, "sub", sub);
}

/**
 * @brief Set the `aud` (audience) standard claim as a single string.
 *
 * For multiple audiences use `jwt_set_audiences`, which writes a JSON
 * array (RFC 7519 §4.1.3 permits either form).
 */
void jwt_set_audience(JwtToken* t, const char* aud) {
    if (!t || !aud) return;
    jwt_obj_set_string(t->payload, "aud", aud);
}

/**
 * @brief Set the `aud` claim as a JSON array of strings.
 *
 * @param t   Token to modify.
 * @param auds Array of audience names.
 * @param n   Number of names. n == 0 → no-op.
 */
void jwt_set_audiences(JwtToken* t, const char** auds, size_t n) {
    if (!t || !auds || n == 0) return;
    JsonElement* arr = json_create(JSON_ARRAY);
    if (!arr) return;
    for (size_t i = 0; i < n; ++i) {
        if (!auds[i]) continue;
        JsonElement* s = json_create(JSON_STRING);
        if (!s) { json_deallocate(arr); return; }
        s->value.string_val = string_strdup(auds[i]);
        if (!s->value.string_val) { json_deallocate(s); json_deallocate(arr); return; }
        json_add_to_array(arr, s);
    }
    if (json_get_element(t->payload, "aud") != NULL) {
        json_remove_element(t->payload, "aud");
    }
    json_add_to_object(t->payload, "aud", arr);
}

/**
 * @brief Set the `exp` (expiration) claim. Unix seconds.
 */
void jwt_set_expiration(JwtToken* t, int64_t exp_unix) {
    if (!t) return;
    jwt_obj_set_number(t->payload, "exp", (double)exp_unix);
}

/**
 * @brief Set the `nbf` (not-before) claim. Unix seconds.
 */
void jwt_set_not_before(JwtToken* t, int64_t nbf_unix) {
    if (!t) return;
    jwt_obj_set_number(t->payload, "nbf", (double)nbf_unix);
}

/**
 * @brief Set the `iat` (issued-at) claim. Unix seconds.
 */
void jwt_set_issued_at(JwtToken* t, int64_t iat_unix) {
    if (!t) return;
    jwt_obj_set_number(t->payload, "iat", (double)iat_unix);
}

/**
 * @brief Set the `jti` (JWT ID) standard claim — unique per-token ID.
 */
void jwt_set_jwt_id(JwtToken* t, const char* jti) {
    if (!t || !jti) return;
    jwt_obj_set_string(t->payload, "jti", jti);
}

/**
 * @brief Set `exp` to `seconds_from_now` seconds after the current time.
 *
 * Convenience wrapper over `jwt_set_expiration` that computes the absolute
 * timestamp from `time(NULL)`, so callers express a time-to-live instead of
 * an absolute epoch. A non-positive argument yields an already-expired token.
 *
 * @param t                Token to modify. NULL is a no-op.
 * @param seconds_from_now Lifetime in seconds added to the current time.
 */
void jwt_set_expires_in(JwtToken* t, int64_t seconds_from_now) {
    if (!t) return;
    jwt_obj_set_number(t->payload, "exp", (double)((int64_t)time(NULL) + seconds_from_now));
}

/**
 * @brief Set the `iat` (issued-at) claim to the current time.
 *
 * Convenience wrapper over `jwt_set_issued_at` using `time(NULL)`.
 *
 * @param t Token to modify. NULL is a no-op.
 */
void jwt_set_issued_now(JwtToken* t) {
    if (!t) return;
    jwt_obj_set_number(t->payload, "iat", (double)(int64_t)time(NULL));
}

/**
 * @brief Set a custom string-valued claim in the payload.
 * @return true on success, false on NULL inputs / OOM.
 */
bool jwt_set_claim_string(JwtToken* t, const char* name, const char* value) {
    if (!t) return false;
    return jwt_obj_set_string(t->payload, name, value);
}

/**
 * @brief Set a custom integer claim. Stored as a JSON number.
 */
bool jwt_set_claim_int(JwtToken* t, const char* name, int64_t value) {
    if (!t) return false;
    return jwt_obj_set_number(t->payload, name, (double)value);
}

/**
 * @brief Set a custom floating-point claim. Stored as a JSON number.
 */
bool jwt_set_claim_double(JwtToken* t, const char* name, double value) {
    if (!t) return false;
    return jwt_obj_set_number(t->payload, name, value);
}

/**
 * @brief Set a custom boolean claim. Stored as a JSON bool.
 */
bool jwt_set_claim_bool(JwtToken* t, const char* name, bool value) {
    if (!t) return false;
    return jwt_obj_set_bool(t->payload, name, value);
}

/**
 * @brief Set a custom claim with an arbitrary JSON value (array, object, …).
 *
 * Takes ownership of `value` on success. On failure (NULL inputs / OOM)
 * the caller still owns `value` and is responsible for freeing it.
 */
bool jwt_set_claim_json(JwtToken* t, const char* name, JsonElement* value) {
    if (!t || !name || !value) return false;
    if (json_get_element(t->payload, name) != NULL) {
        json_remove_element(t->payload, name);
    }
    return json_add_to_object(t->payload, name, value);
}

/**
 * @brief Set a custom header parameter (e.g. `kid`).
 *
 * The standard `typ` and `alg` headers are already populated by
 * `jwt_create`; this is for everything else (`kid`, `cty`, …).
 */
bool jwt_set_header_string(JwtToken* t, const char* name, const char* value) {
    if (!t) return false;
    return jwt_obj_set_string(t->header, name, value);
}

/* =================================================================== */
/* Encoding                                                             */
/* =================================================================== */

/* Format an integer-valued JSON claim without scientific notation.
   json_serialize already produces an integer when (double)floor(v)==v,
   so we don't need a separate path. */

/**
 * @brief Convert an OpenSSL DER-encoded ECDSA_SIG to JWS raw R||S form.
 *
 * JWS (RFC 7515) wants ECDSA signatures as the fixed-length concatenation
 * of the R and S big-endian integers, each zero-padded on the LEFT to the
 * curve's coordinate size. OpenSSL's EVP_DigestSignFinal hands us DER:
 *   SEQUENCE { INTEGER R, INTEGER S }
 * with R/S minimum-length encoded. We extract the two BIGNUMs and re-pad.
 *
 * @param der_sig  DER blob written by EVP_DigestSignFinal.
 * @param der_len  Length of `der_sig`.
 * @param coord_len Curve coordinate length in bytes (32/48/66).
 * @param out      Receives a malloc'd buffer of size 2*coord_len. Caller frees.
 * @param out_len  Receives 2*coord_len on success.
 * @return true on success.
 */
static bool jwt_ecdsa_der_to_raw(const unsigned char* der_sig, size_t der_len,
                                 size_t coord_len,
                                 unsigned char** out, size_t* out_len) {
    const unsigned char* p = der_sig;
    ECDSA_SIG* sig = d2i_ECDSA_SIG(NULL, &p, (long)der_len);
    if (!sig) {
        JWT_LOG("[jwt_ecdsa_der_to_raw]: d2i_ECDSA_SIG failed");
        return false;
    }
    const BIGNUM* r = NULL;
    const BIGNUM* s = NULL;
    ECDSA_SIG_get0(sig, &r, &s);
    if (!r || !s) { ECDSA_SIG_free(sig); return false; }

    size_t raw_len = coord_len * 2;
    unsigned char* raw = (unsigned char*)calloc(1, raw_len);
    if (!raw) { ECDSA_SIG_free(sig); return false; }

    /* BN_bn2binpad left-pads with zeros to exactly coord_len bytes. */
    if (BN_bn2binpad(r, raw,             (int)coord_len) != (int)coord_len ||
        BN_bn2binpad(s, raw + coord_len, (int)coord_len) != (int)coord_len) {
        JWT_LOG("[jwt_ecdsa_der_to_raw]: BN_bn2binpad failed");
        free(raw); ECDSA_SIG_free(sig); return false;
    }
    ECDSA_SIG_free(sig);
    *out = raw;
    *out_len = raw_len;
    return true;
}

/**
 * @brief Convert a JWS raw R||S signature back to OpenSSL DER form.
 *
 * Inverse of jwt_ecdsa_der_to_raw. The two halves are interpreted as
 * big-endian unsigned integers; OpenSSL then re-encodes them as ASN.1
 * INTEGERs inside a DER SEQUENCE.
 *
 * @param raw       Raw R||S buffer of length 2*coord_len.
 * @param raw_len   Must equal 2*coord_len; otherwise NULL returned.
 * @param coord_len Curve coordinate length.
 * @param out       Receives a malloc'd DER buffer. Caller frees.
 * @param out_len   Receives the DER length.
 * @return true on success.
 */
static bool jwt_ecdsa_raw_to_der(const unsigned char* raw, size_t raw_len,
                                 size_t coord_len,
                                 unsigned char** out, size_t* out_len) {
    if (raw_len != coord_len * 2) {
        JWT_LOG("[jwt_ecdsa_raw_to_der]: raw_len=%zu, expected=%zu",
                raw_len, coord_len * 2);
        return false;
    }
    BIGNUM* r = BN_bin2bn(raw,             (int)coord_len, NULL);
    BIGNUM* s = BN_bin2bn(raw + coord_len, (int)coord_len, NULL);
    ECDSA_SIG* sig = ECDSA_SIG_new();
    if (!r || !s || !sig) {
        BN_free(r); BN_free(s); if (sig) ECDSA_SIG_free(sig);
        return false;
    }
    /* ECDSA_SIG_set0 takes ownership of r and s; do NOT free them after. */
    if (ECDSA_SIG_set0(sig, r, s) != 1) {
        BN_free(r); BN_free(s); ECDSA_SIG_free(sig);
        return false;
    }
    int der_len = i2d_ECDSA_SIG(sig, NULL);
    if (der_len <= 0) { ECDSA_SIG_free(sig); return false; }
    unsigned char* der = (unsigned char*)malloc((size_t)der_len);
    if (!der) { ECDSA_SIG_free(sig); return false; }
    unsigned char* p = der;
    int actual = i2d_ECDSA_SIG(sig, &p);
    ECDSA_SIG_free(sig);
    if (actual <= 0) { free(der); return false; }
    *out = der;
    *out_len = (size_t)actual;
    return true;
}

/* Sign the signing input with the supplied key + algorithm. On success,
   writes a malloc'd signature buffer to *sig (caller frees) and its
   length to *sig_len. Returns true on success. */
static bool jwt_sign(JwtAlgorithm alg, const JwtKey* key,
                     const unsigned char* signing_input, size_t signing_input_len,
                     unsigned char** sig, size_t* sig_len) {
    if (alg == JWT_ALG_NONE) {
        *sig = NULL;
        *sig_len = 0;
        return true;
    }

    const EVP_MD* md = jwt_evp_md_for(alg);
    if (!md) return false;

    if (jwt_alg_is_hmac(alg)) {
        if (!key || key->kind != JWT_KEY_KIND_HMAC) return false;
        unsigned char buf[EVP_MAX_MD_SIZE];
        unsigned int   buf_len = 0;
        if (!HMAC(md, key->secret, (int)key->secret_len,
                  signing_input, signing_input_len, buf, &buf_len)) {
            JWT_LOG("HMAC() failed");
            return false;
        }
        unsigned char* out = (unsigned char*)malloc(buf_len);
        if (!out) return false;
        memcpy(out, buf, buf_len);
        *sig = out;
        *sig_len = buf_len;
        return true;
    }

    if (jwt_alg_is_rsa(alg) || jwt_alg_is_rsa_pss(alg)) {
        if (!key || key->kind != JWT_KEY_KIND_RSA_PRIVATE) return false;
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) return false;
        bool ok = false;
        unsigned char* out = NULL;
        size_t out_len = 0;
        EVP_PKEY_CTX* pctx = NULL;

        if (EVP_DigestSignInit(ctx, &pctx, md, NULL, key->pkey) != 1) goto rsa_sign_done;
        if (jwt_alg_is_rsa_pss(alg)) {
            /* RFC 7518 §3.5: PS* uses PSS padding, MGF1 over the same
               digest, salt length equal to the digest output. */
            if (EVP_PKEY_CTX_set_rsa_padding(pctx, RSA_PKCS1_PSS_PADDING) <= 0) goto rsa_sign_done;
            if (EVP_PKEY_CTX_set_rsa_pss_saltlen(pctx, RSA_PSS_SALTLEN_DIGEST) <= 0) goto rsa_sign_done;
            if (EVP_PKEY_CTX_set_rsa_mgf1_md(pctx, md) <= 0) goto rsa_sign_done;
        }
        if (EVP_DigestSignUpdate(ctx, signing_input, signing_input_len) != 1) goto rsa_sign_done;
        if (EVP_DigestSignFinal(ctx, NULL, &out_len) != 1) goto rsa_sign_done;

        out = (unsigned char*)malloc(out_len);
        if (!out) goto rsa_sign_done;
        if (EVP_DigestSignFinal(ctx, out, &out_len) != 1) { free(out); out = NULL; goto rsa_sign_done; }
        ok = true;

    rsa_sign_done:
        EVP_MD_CTX_free(ctx);
        if (!ok) { return false; }
        *sig = out;
        *sig_len = out_len;
        return true;
    }

    if (jwt_alg_is_ec(alg)) {
        if (!key || key->kind != JWT_KEY_KIND_EC_PRIVATE) return false;
        if (!jwt_ec_curve_matches_alg(key->pkey, alg)) {
            JWT_LOG("[jwt_sign]: EC key curve doesn't match %s", jwt_alg_to_string(alg));
            return false;
        }
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) return false;
        bool ok = false;
        unsigned char* der = NULL;
        size_t der_len = 0;

        if (EVP_DigestSignInit(ctx, NULL, md, NULL, key->pkey) != 1) goto ec_sign_done;
        if (EVP_DigestSignUpdate(ctx, signing_input, signing_input_len) != 1) goto ec_sign_done;
        if (EVP_DigestSignFinal(ctx, NULL, &der_len) != 1) goto ec_sign_done;

        der = (unsigned char*)malloc(der_len);
        if (!der) goto ec_sign_done;
        if (EVP_DigestSignFinal(ctx, der, &der_len) != 1) { free(der); der = NULL; goto ec_sign_done; }

        /* Convert DER signature into the JOSE raw R||S form. */
        unsigned char* raw = NULL;
        size_t raw_len = 0;
        if (!jwt_ecdsa_der_to_raw(der, der_len, jwt_ec_coord_len(alg),
                                  &raw, &raw_len)) {
            free(der);
            goto ec_sign_done;
        }
        free(der); der = NULL;
        *sig = raw;
        *sig_len = raw_len;
        ok = true;

    ec_sign_done:
        EVP_MD_CTX_free(ctx);
        return ok;
    }

    return false;
}

/* Verify the signature using the verification key. Returns true if the
   signature is valid. */
static bool jwt_verify_signature(JwtAlgorithm alg, const JwtKey* key,
                                 const unsigned char* signing_input, size_t signing_input_len,
                                 const unsigned char* sig, size_t sig_len) {
    if (alg == JWT_ALG_NONE) {
        return sig_len == 0;
    }

    const EVP_MD* md = jwt_evp_md_for(alg);
    if (!md) return false;

    if (jwt_alg_is_hmac(alg)) {
        if (!key || key->kind != JWT_KEY_KIND_HMAC) return false;
        unsigned char buf[EVP_MAX_MD_SIZE];
        unsigned int   buf_len = 0;
        if (!HMAC(md, key->secret, (int)key->secret_len,
                  signing_input, signing_input_len, buf, &buf_len)) {
            return false;
        }
        if (buf_len != sig_len) return false;
        /* Constant-time compare — never use plain memcmp on signature bytes. */
        return CRYPTO_memcmp(buf, sig, buf_len) == 0;
    }

    if (jwt_alg_is_rsa(alg) || jwt_alg_is_rsa_pss(alg)) {
        if (!key || key->kind != JWT_KEY_KIND_RSA_PUBLIC) return false;
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        if (!ctx) return false;
        bool ok = false;
        EVP_PKEY_CTX* pctx = NULL;
        if (EVP_DigestVerifyInit(ctx, &pctx, md, NULL, key->pkey) != 1) goto rsa_verify_done;
        if (jwt_alg_is_rsa_pss(alg)) {
            if (EVP_PKEY_CTX_set_rsa_padding(pctx, RSA_PKCS1_PSS_PADDING) <= 0) goto rsa_verify_done;
            if (EVP_PKEY_CTX_set_rsa_pss_saltlen(pctx, RSA_PSS_SALTLEN_DIGEST) <= 0) goto rsa_verify_done;
            if (EVP_PKEY_CTX_set_rsa_mgf1_md(pctx, md) <= 0) goto rsa_verify_done;
        }
        if (EVP_DigestVerifyUpdate(ctx, signing_input, signing_input_len) != 1) goto rsa_verify_done;
        ok = (EVP_DigestVerifyFinal(ctx, sig, sig_len) == 1);
    rsa_verify_done:
        EVP_MD_CTX_free(ctx);
        return ok;
    }

    if (jwt_alg_is_ec(alg)) {
        if (!key || key->kind != JWT_KEY_KIND_EC_PUBLIC) return false;
        if (!jwt_ec_curve_matches_alg(key->pkey, alg)) {
            JWT_LOG("[jwt_verify_signature]: EC key curve doesn't match %s",
                    jwt_alg_to_string(alg));
            return false;
        }
        /* JOSE delivers ECDSA sigs as raw R||S of fixed length. Anything
           else is malformed — reject without touching OpenSSL. */
        if (sig_len != jwt_ec_raw_sig_len(alg)) {
            JWT_LOG("[jwt_verify_signature]: ES* sig length %zu, expected %zu",
                    sig_len, jwt_ec_raw_sig_len(alg));
            return false;
        }
        unsigned char* der = NULL;
        size_t der_len = 0;
        if (!jwt_ecdsa_raw_to_der(sig, sig_len, jwt_ec_coord_len(alg),
                                  &der, &der_len)) {
            return false;
        }
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        bool ok = false;
        if (!ctx) { free(der); return false; }
        if (EVP_DigestVerifyInit(ctx, NULL, md, NULL, key->pkey) != 1) goto ec_verify_done;
        if (EVP_DigestVerifyUpdate(ctx, signing_input, signing_input_len) != 1) goto ec_verify_done;
        ok = (EVP_DigestVerifyFinal(ctx, der, der_len) == 1);
    ec_verify_done:
        EVP_MD_CTX_free(ctx);
        free(der);
        return ok;
    }

    return false;
}

/**
 * @brief Encode the token to the compact serialization "h.p.s".
 *
 * Serializes header and payload JSON, base64url-encodes both, joins
 * with `.`, signs the joined "h.p" string with the supplied key, and
 * appends the base64url-encoded signature. For `JWT_ALG_NONE` the
 * trailing signature segment is empty (no dot is omitted — the format
 * still requires three segments per RFC 7519 §3).
 *
 * Validates the key shape against the token's algorithm: HMAC keys
 * with RSA algs (or vice versa) return NULL up front.
 *
 * @param t           Fully-populated token.
 * @param signing_key Key matching `jwt_alg_is_hmac(alg)` / `jwt_alg_is_rsa(alg)`.
 *                    NULL allowed iff `alg == JWT_ALG_NONE`.
 * @return Newly-allocated NUL-terminated string. Caller frees with free().
 */
char* jwt_encode(const JwtToken* t, const JwtKey* signing_key) {
    if (!t) {
        JWT_LOG("[jwt_encode]: NULL token");
        return NULL;
    }
    JWT_LOG("[jwt_encode]: encoding (alg=%s)", jwt_alg_to_string(t->alg));

    /* Reject obviously-wrong key shape before doing any work. */
    if (t->alg != JWT_ALG_NONE) {
        if (!signing_key) {
            JWT_LOG("[jwt_encode]: alg=%s but signing_key is NULL", jwt_alg_to_string(t->alg));
            return NULL;
        }
        if (jwt_alg_is_hmac(t->alg) && signing_key->kind != JWT_KEY_KIND_HMAC) {
            JWT_LOG("[jwt_encode]: HMAC algorithm requires HMAC key");
            return NULL;
        }
        if (jwt_alg_uses_rsa_key(t->alg) && signing_key->kind != JWT_KEY_KIND_RSA_PRIVATE) {
            JWT_LOG("[jwt_encode]: %s requires RSA private key",
                    jwt_alg_to_string(t->alg));
            return NULL;
        }
        if (jwt_alg_is_ec(t->alg)) {
            if (signing_key->kind != JWT_KEY_KIND_EC_PRIVATE) {
                JWT_LOG("[jwt_encode]: EC algorithm requires EC private key");
                return NULL;
            }
            if (!jwt_ec_curve_matches_alg(signing_key->pkey, t->alg)) {
                JWT_LOG("[jwt_encode]: EC key curve doesn't match algorithm %s",
                        jwt_alg_to_string(t->alg));
                return NULL;
            }
        }
    }

    char* hdr_json = NULL;
    char* pl_json  = NULL;
    char* hdr_b64  = NULL;
    char* pl_b64   = NULL;
    char* sig_b64  = NULL;
    unsigned char* sig = NULL;
    size_t sig_len = 0;
    char* out = NULL;

    hdr_json = json_serialize(t->header);
    pl_json  = json_serialize(t->payload);
    if (!hdr_json || !pl_json) {
        JWT_LOG("[jwt_encode]: json_serialize failed");
        goto done;
    }

    hdr_b64 = jwt_base64url_encode((const unsigned char*)hdr_json, strlen(hdr_json), NULL);
    pl_b64  = jwt_base64url_encode((const unsigned char*)pl_json,  strlen(pl_json),  NULL);
    if (!hdr_b64 || !pl_b64) {
        JWT_LOG("[jwt_encode]: base64url_encode failed for header/payload");
        goto done;
    }

    size_t hl = strlen(hdr_b64), pl = strlen(pl_b64);
    size_t signing_len = hl + 1 + pl;
    char* signing_input = (char*)malloc(signing_len + 1);
    if (!signing_input) goto done;
    memcpy(signing_input, hdr_b64, hl);
    signing_input[hl] = '.';
    memcpy(signing_input + hl + 1, pl_b64, pl);
    signing_input[signing_len] = '\0';

    if (!jwt_sign(t->alg, signing_key,
                  (const unsigned char*)signing_input, signing_len,
                  &sig, &sig_len)) {
        JWT_LOG("[jwt_encode]: signing failed");
        free(signing_input);
        goto done;
    }

    if (sig_len > 0) {
        sig_b64 = jwt_base64url_encode(sig, sig_len, NULL);
        if (!sig_b64) { free(signing_input); goto done; }
    } else {
        sig_b64 = (char*)malloc(1);
        if (!sig_b64) { free(signing_input); goto done; }
        sig_b64[0] = '\0';
    }

    size_t sb = strlen(sig_b64);
    out = (char*)malloc(signing_len + 1 + sb + 1);
    if (!out) { JWT_LOG("[jwt_encode]: final malloc failed"); free(signing_input); goto done; }
    memcpy(out, signing_input, signing_len);
    out[signing_len] = '.';
    memcpy(out + signing_len + 1, sig_b64, sb);
    out[signing_len + 1 + sb] = '\0';

    free(signing_input);

done:
    free(hdr_json);
    free(pl_json);
    free(hdr_b64);
    free(pl_b64);
    free(sig_b64);
    if (sig) { OPENSSL_cleanse(sig, sig_len); free(sig); }
    if (out) {
        JWT_LOG("[jwt_encode]: encoded token length=%zu", strlen(out));
    }
    return out;
}

/* =================================================================== */
/* Decoding                                                             */
/* =================================================================== */


/**
 * @brief Parse a compact-serialization JWT without verifying its signature.
 *
 * Returns a populated token whose header / payload / signature bytes /
 * preserved signing input are all set. The returned token is NOT
 * trustworthy until `jwt_verify` returns `JWT_SUCCESS`. Useful for
 * reading the `kid` header parameter to pick a verification key.
 *
 * @param compact_jwt Null-terminated "h.p.s" string.
 * @return Newly-allocated token, or NULL on any parse failure / OOM.
 */
JwtToken* jwt_decode(const char* compact_jwt) {
    if (!compact_jwt) {
        JWT_LOG("[jwt_decode]: NULL input");
        return NULL;
    }
    const char* dot1 = strchr(compact_jwt, '.');
    if (!dot1) { JWT_LOG("[jwt_decode]: missing first '.'"); return NULL; }
    const char* dot2 = strchr(dot1 + 1, '.');
    if (!dot2) { JWT_LOG("[jwt_decode]: missing second '.'"); return NULL; }
    if (strchr(dot2 + 1, '.') != NULL) {
        JWT_LOG("[jwt_decode]: more than two '.' separators");
        return NULL;
    }

    size_t hlen = (size_t)(dot1 - compact_jwt);
    size_t plen = (size_t)(dot2 - (dot1 + 1));
    size_t slen = strlen(dot2 + 1);

    if (hlen == 0 || plen == 0) {
        JWT_LOG("[jwt_decode]: empty header or payload segment");
        return NULL;
    }

    JwtToken* t = (JwtToken*)calloc(1, sizeof(JwtToken));
    if (!t) { JWT_LOG("[jwt_decode]: calloc(JwtToken) failed"); return NULL; }

    /* Preserve the signing input bytes for later verification. */
    size_t si_len = hlen + 1 + plen;
    t->signing_input = (char*)malloc(si_len + 1);
    if (!t->signing_input) { JWT_LOG("[jwt_decode]: signing_input malloc failed"); jwt_deallocate(t); return NULL; }
    memcpy(t->signing_input, compact_jwt, si_len);
    t->signing_input[si_len] = '\0';
    t->signing_input_len = si_len;

    /* Decode header. */
    size_t dec_n = 0;
    unsigned char* hdec = jwt_base64url_decode(compact_jwt, hlen, &dec_n);
    if (!hdec) { JWT_LOG("[jwt_decode]: header base64url decode failed"); jwt_deallocate(t); return NULL; }
    char* hjson = (char*)malloc(dec_n + 1);
    if (!hjson) { JWT_LOG("[jwt_decode]: header JSON copy malloc failed"); free(hdec); jwt_deallocate(t); return NULL; }
    memcpy(hjson, hdec, dec_n); hjson[dec_n] = '\0';
    free(hdec);
    t->header = json_parse(hjson);
    free(hjson);
    if (!t->header || t->header->type != JSON_OBJECT) {
        JWT_LOG("[jwt_decode]: header JSON parse failed or not an object");
        jwt_deallocate(t); return NULL;
    }

    /* Decode payload. */
    unsigned char* pdec = jwt_base64url_decode(dot1 + 1, plen, &dec_n);
    if (!pdec) { JWT_LOG("[jwt_decode]: payload base64url decode failed"); jwt_deallocate(t); return NULL; }
    char* pjson = (char*)malloc(dec_n + 1);
    if (!pjson) { JWT_LOG("[jwt_decode]: payload JSON copy malloc failed"); free(pdec); jwt_deallocate(t); return NULL; }
    memcpy(pjson, pdec, dec_n); pjson[dec_n] = '\0';
    free(pdec);
    t->payload = json_parse(pjson);
    free(pjson);
    if (!t->payload || t->payload->type != JSON_OBJECT) {
        JWT_LOG("[jwt_decode]: payload JSON parse failed or not an object");
        jwt_deallocate(t); return NULL;
    }

    /* Decode signature (may legitimately be empty for alg=none). */
    if (slen > 0) {
        unsigned char* sdec = jwt_base64url_decode(dot2 + 1, slen, &dec_n);
        if (!sdec) { JWT_LOG("[jwt_decode]: signature base64url decode failed"); jwt_deallocate(t); return NULL; }
        t->signature = sdec;
        t->signature_len = dec_n;
    }

    /* Read alg from the header. */
    JsonElement* alg = json_get_element(t->header, "alg");
    if (!alg || alg->type != JSON_STRING || !alg->value.string_val) {
        JWT_LOG("[jwt_decode]: header missing 'alg' string");
        jwt_deallocate(t); return NULL;
    }
    t->alg = jwt_alg_from_string(alg->value.string_val);
    /* If the header says something the enum doesn't know about, alg
       becomes JWT_ALG_NONE — that's fine. Verification will reject it
       because JWT_ALG_NONE won't be in the caller's allow-list. */
    JWT_LOG("[jwt_decode]: decoded token (alg=%s, sig_len=%zu)",
            jwt_alg_to_string(t->alg), t->signature_len);

    return t;
}

/* =================================================================== */
/* Verification                                                         */
/* =================================================================== */

/* Safely convert a JSON number (double) to int64. Rejects NaN, +/-Inf, and any
   value outside the representable int64 range — a direct (int64_t) cast of such
   a value is undefined behavior in C. A token can legally carry "exp": 1e308
   (valid JSON), so the time-claim path MUST guard before casting; otherwise a
   huge `nbf` can collapse to INT64_MIN and wrongly pass the not-before gate.
   NaN fails every comparison, so the range test rejects it without <math.h>.
   2^63 (9223372036854775808.0) and -2^63 are exactly representable as double. */
static bool jwtp_json_num_to_int64(double v, int64_t* out) {
    if (!(v >= -9223372036854775808.0 && v < 9223372036854775808.0)) {
        return false;
    }
    *out = (int64_t)v;
    return true;
}

/* Is `alg` in opts->allowed_algs? */
static bool jwt_alg_allowed(JwtAlgorithm alg, const JwtAlgorithm* allowed, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        if (allowed[i] == alg) return true;
    }
    return false;
}

/* Does the token's audience set contain `expected`? `aud` may be a JSON
   string or a JSON array of strings. */
static bool jwt_audience_matches(JsonElement* aud, const char* expected) {
    if (!aud || !expected) return false;
    if (aud->type == JSON_STRING && aud->value.string_val) {
        return strcmp(aud->value.string_val, expected) == 0;
    }
    if (aud->type == JSON_ARRAY) {
        size_t n = json_array_size(aud);
        for (size_t i = 0; i < n; ++i) {
            char key[24];
            snprintf(key, sizeof(key), "%zu", i);
            JsonElement* item = json_get_element(aud, key);
            if (item && item->type == JSON_STRING && item->value.string_val &&
                strcmp(item->value.string_val, expected) == 0) {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Verify a decoded token against a key + validation options.
 *
 * Performs (in order):
 *   1. Algorithm allow-list check (rejects unsupported / "none" unless
 *      explicitly opted in).
 *   2. Algorithm-key-type binding check (HMAC alg requires HMAC key, RSA
 *      alg requires RSA public key).
 *   3. Constant-time signature verification.
 *   4. `exp`, `nbf`, `iat` checks honoring `leeway_seconds`.
 *   5. `iss`, `aud`, `sub` checks if the corresponding `expected_*`
 *      options are set.
 *
 * `opts->allowed_algs` MUST be non-NULL — if it is NULL, the function
 * returns `JWT_ERR_INVALID_ALG` without inspecting the token. This is
 * the canonical defense against the historical "none" attack.
 *
 * @param t                Token returned by `jwt_decode`.
 * @param verification_key Key whose type matches `t->alg`.
 * @param opts             Validation options.
 * @return `JWT_SUCCESS` on full validation; a specific `JWT_ERR_*` otherwise.
 */
JwtStatus jwt_verify(const JwtToken* t,
                     const JwtKey* verification_key,
                     const JwtValidationOptions* opts) {
    if (!t || !opts) {
        JWT_LOG("[jwt_verify]: NULL token or opts");
        return JWT_ERR_GENERIC;
    }

    /* 1. Algorithm allow-list — mandatory. */
    if (!opts->allowed_algs || opts->allowed_algs_count == 0) {
        JWT_LOG("[jwt_verify]: opts->allowed_algs is NULL/empty (mandatory)");
        return JWT_ERR_INVALID_ALG;
    }
    if (!jwt_alg_allowed(t->alg, opts->allowed_algs, opts->allowed_algs_count)) {
        JWT_LOG("[jwt_verify]: token alg=%s not in allow-list", jwt_alg_to_string(t->alg));
        return JWT_ERR_INVALID_ALG;
    }

    /* 2. Key-shape binding. */
    if (t->alg != JWT_ALG_NONE) {
        if (!verification_key) {
            JWT_LOG("[jwt_verify]: NULL verification key for alg=%s", jwt_alg_to_string(t->alg));
            return JWT_ERR_INVALID_KEY;
        }
        if (jwt_alg_is_hmac(t->alg) && verification_key->kind != JWT_KEY_KIND_HMAC) {
            JWT_LOG("[jwt_verify]: HMAC alg requires HMAC key (algorithm confusion blocked)");
            return JWT_ERR_INVALID_KEY;
        }
        if (jwt_alg_uses_rsa_key(t->alg) && verification_key->kind != JWT_KEY_KIND_RSA_PUBLIC) {
            JWT_LOG("[jwt_verify]: %s requires RSA public key",
                    jwt_alg_to_string(t->alg));
            return JWT_ERR_INVALID_KEY;
        }
        if (jwt_alg_is_ec(t->alg) && verification_key->kind != JWT_KEY_KIND_EC_PUBLIC) {
            JWT_LOG("[jwt_verify]: EC alg requires EC public key");
            return JWT_ERR_INVALID_KEY;
        }
    }

    /* 2a. RFC 7515 §4.1.11 — `crit` header.
       The producer marks listed header parameters as "must understand".
       This implementation supports no extensions, so any non-empty crit
       must be rejected. Empty / non-array / non-string-member crit are
       all malformed per the RFC. */
    JsonElement* crit = json_get_element(t->header, "crit");
    if (crit) {
        if (crit->type != JSON_ARRAY) {
            JWT_LOG("[jwt_verify]: 'crit' is not a JSON array");
            return JWT_ERR_CRIT_UNSUPPORTED;
        }
        size_t n = json_array_size(crit);
        if (n == 0) {
            /* "Producers MUST NOT use the empty list '[]' as the 'crit' value." */
            JWT_LOG("[jwt_verify]: 'crit' is empty (forbidden by RFC 7515)");
            return JWT_ERR_CRIT_UNSUPPORTED;
        }
        /* We don't implement any header-parameter extensions, so any name
           listed in `crit` is by definition unsupported. */
        JWT_LOG("[jwt_verify]: 'crit' present with %zu unsupported extension(s)", n);
        return JWT_ERR_CRIT_UNSUPPORTED;
    }

    /* 2b. Header `typ` enforcement (RFC 9068 access tokens want `at+jwt`). */
    if (opts->expected_typ) {
        JsonElement* typ = json_get_element(t->header, "typ");
        if (!typ || typ->type != JSON_STRING || !typ->value.string_val ||
            strcmp(typ->value.string_val, opts->expected_typ) != 0) {
            JWT_LOG("[jwt_verify]: typ mismatch (expected '%s')", opts->expected_typ);
            return JWT_ERR_INVALID_TYP;
        }
    }

    /* 2c. RFC 7518 §3.2 — HMAC secret length floor (opt-in).
       Verifies BEFORE running HMAC so weak secrets are rejected loudly. */
    if (opts->reject_weak_hmac_key && jwt_alg_is_hmac(t->alg)) {
        size_t min = jwt_hmac_min_key_len(t->alg);
        if (verification_key && verification_key->kind == JWT_KEY_KIND_HMAC &&
            verification_key->secret_len < min) {
            JWT_LOG("[jwt_verify]: HMAC secret too short (%zu < %zu) for %s",
                    verification_key->secret_len, min, jwt_alg_to_string(t->alg));
            return JWT_ERR_WEAK_KEY;
        }
    }

    /* 3. Signature. */
    if (!jwt_verify_signature(t->alg, verification_key,
                              (const unsigned char*)t->signing_input,
                              t->signing_input_len,
                              t->signature, t->signature_len)) {
        JWT_LOG("[jwt_verify]: signature verification failed");
        return JWT_ERR_INVALID_SIGNATURE;
    }

    /* 4. Time-based claims. */
    int64_t now = opts->now_unix ? opts->now_unix : (int64_t)time(NULL);
    int64_t leeway = opts->leeway_seconds;

    JsonElement* exp = json_get_element(t->payload, "exp");
    if (exp) {
        int64_t exp_v;
        if (exp->type != JSON_NUMBER || !jwtp_json_num_to_int64(exp->value.number_val, &exp_v)) {
            JWT_LOG("[jwt_verify]: exp claim is not a representable number");
            return JWT_ERR_INVALID_FORMAT;
        }
        /* Compare without arithmetic on the token-controlled value (exp_v + leeway
           could overflow for a hostile exp): expired iff exp_v < now - leeway. */
        if (exp_v < now - leeway) {
            JWT_LOG("[jwt_verify]: token expired (exp=%lld now=%lld leeway=%lld)",
                    (long long)exp_v, (long long)now, (long long)leeway);
            return JWT_ERR_EXPIRED;
        }
    } else if (opts->require_exp) {
        JWT_LOG("[jwt_verify]: require_exp set but exp claim missing");
        return JWT_ERR_MISSING_CLAIM;
    }

    JsonElement* nbf = json_get_element(t->payload, "nbf");
    if (nbf) {
        int64_t nbf_v;
        if (nbf->type != JSON_NUMBER || !jwtp_json_num_to_int64(nbf->value.number_val, &nbf_v)) {
            JWT_LOG("[jwt_verify]: nbf claim is not a representable number");
            return JWT_ERR_INVALID_FORMAT;
        }
        /* not-yet-valid iff nbf_v > now + leeway (no arithmetic on nbf_v). */
        if (nbf_v > now + leeway) {
            JWT_LOG("[jwt_verify]: token not yet valid (nbf=%lld now=%lld leeway=%lld)",
                    (long long)nbf_v, (long long)now, (long long)leeway);
            return JWT_ERR_NOT_YET_VALID;
        }
    } else if (opts->require_nbf) {
        JWT_LOG("[jwt_verify]: require_nbf set but nbf claim missing");
        return JWT_ERR_MISSING_CLAIM;
    }

    JsonElement* iat = json_get_element(t->payload, "iat");
    if (iat) {
        if (iat->type != JSON_NUMBER) {
            JWT_LOG("[jwt_verify]: iat claim is not a number");
            return JWT_ERR_INVALID_FORMAT;
        }
        /* iat-in-the-future is sometimes flagged, but RFC doesn't mandate it.
           We treat it as informational; no rejection. */
    } else if (opts->require_iat) {
        JWT_LOG("[jwt_verify]: require_iat set but iat claim missing");
        return JWT_ERR_MISSING_CLAIM;
    }

    /* 5. Issuer / subject / audience. */
    if (opts->expected_iss) {
        JsonElement* iss = json_get_element(t->payload, "iss");
        if (!iss || iss->type != JSON_STRING || !iss->value.string_val ||
            strcmp(iss->value.string_val, opts->expected_iss) != 0) {
            JWT_LOG("[jwt_verify]: issuer mismatch (expected '%s')", opts->expected_iss);
            return JWT_ERR_INVALID_ISSUER;
        }
    }
    if (opts->expected_sub) {
        JsonElement* sub = json_get_element(t->payload, "sub");
        if (!sub || sub->type != JSON_STRING || !sub->value.string_val ||
            strcmp(sub->value.string_val, opts->expected_sub) != 0) {
            JWT_LOG("[jwt_verify]: subject mismatch (expected '%s')", opts->expected_sub);
            return JWT_ERR_INVALID_SUBJECT;
        }
    }
    if (opts->expected_aud && opts->expected_aud_count > 0) {
        JsonElement* aud = json_get_element(t->payload, "aud");
        bool ok = false;
        if (aud) {
            for (size_t i = 0; i < opts->expected_aud_count && !ok; ++i) {
                if (opts->expected_aud[i] && jwt_audience_matches(aud, opts->expected_aud[i])) {
                    ok = true;
                }
            }
        }
        if (!ok) {
            JWT_LOG("[jwt_verify]: audience mismatch (no expected aud matched)");
            return JWT_ERR_INVALID_AUDIENCE;
        }
    }

    JWT_LOG("[jwt_verify]: token verified (alg=%s)", jwt_alg_to_string(t->alg));
    return JWT_SUCCESS;
}

/**
 * @brief Decode and verify in one call.
 *
 * On success the parsed token is returned via *out and the caller owns
 * it (release with `jwt_deallocate`). On any failure *out is set to
 * NULL and the appropriate `JWT_ERR_*` code is returned.
 *
 * @param compact_jwt      Compact-serialized JWT.
 * @param verification_key Verification key.
 * @param opts             Validation options.
 * @param out              Receives the parsed token on JWT_SUCCESS only.
 * @return Status code.
 */
JwtStatus jwt_decode_and_verify(const char* compact_jwt,
                                const JwtKey* verification_key,
                                const JwtValidationOptions* opts,
                                JwtToken** out) {
    if (out) *out = NULL;
    if (!compact_jwt || !opts) {
        JWT_LOG("[jwt_decode_and_verify]: NULL input or opts");
        return JWT_ERR_GENERIC;
    }
    /* DoS guard: refuse oversized inputs before allocating anything. We
       use strnlen-style scanning so we don't walk a multi-MB attacker
       string when the cap is set to a small value. */
    if (opts->max_token_bytes > 0) {
        size_t cap = opts->max_token_bytes;
        for (size_t i = 0; i <= cap; ++i) {
            if (compact_jwt[i] == '\0') { cap = i; break; }
            if (i == opts->max_token_bytes) {
                JWT_LOG("[jwt_decode_and_verify]: token exceeds max_token_bytes=%zu",
                        opts->max_token_bytes);
                return JWT_ERR_TOKEN_TOO_LARGE;
            }
        }
        (void)cap;
    }
    JwtToken* t = jwt_decode(compact_jwt);
    if (!t) {
        JWT_LOG("[jwt_decode_and_verify]: jwt_decode failed");
        return JWT_ERR_INVALID_FORMAT;
    }
    JwtStatus rc = jwt_verify(t, verification_key, opts);
    if (rc != JWT_SUCCESS) {
        JWT_LOG("[jwt_decode_and_verify]: jwt_verify returned %s", jwt_status_to_string(rc));
        jwt_deallocate(t);
        return rc;
    }
    if (out) *out = t;
    else jwt_deallocate(t);
    JWT_LOG("[jwt_decode_and_verify]: success");
    return JWT_SUCCESS;
}

/* Returns true if `key` is at least the right SHAPE for `alg`. This is
   not "the key will verify" — that's signature work — it's "trying this
   key is not categorically wrong". Used by the multi-key paths to skip
   keys that can't possibly match before burning crypto cycles. */
static bool jwt_key_shape_matches_alg(const JwtKey* key, JwtAlgorithm alg) {
    if (!key) return false;
    if (alg == JWT_ALG_NONE) return true;  /* no key needed */
    if (jwt_alg_is_hmac(alg))      return key->kind == JWT_KEY_KIND_HMAC;
    if (jwt_alg_uses_rsa_key(alg)) return key->kind == JWT_KEY_KIND_RSA_PUBLIC;
    if (jwt_alg_is_ec(alg)) {
        if (key->kind != JWT_KEY_KIND_EC_PUBLIC) return false;
        return jwt_ec_curve_matches_alg(key->pkey, alg);
    }
    return false;
}

/**
 * @brief Decode + verify against an ordered list of candidate keys.
 *
 * Designed for key rotation. The verifier walks `keys` in order; for
 * each non-NULL key whose shape matches the token's algorithm it runs
 * the full verification pipeline. The FIRST key that produces a valid
 * signature AND passes every claim check wins.
 *
 * Errors that don't depend on the key (`INVALID_FORMAT`,
 * `TOKEN_TOO_LARGE`, `INVALID_ALG`, `CRIT_UNSUPPORTED`, `INVALID_TYP`,
 * claim mismatches, time-based rejections) short-circuit immediately —
 * trying another key wouldn't change the answer.
 *
 * If no key matches, the function returns the most informative status
 * it saw — preferring `INVALID_SIGNATURE` over `INVALID_KEY`, both of
 * which signal "your keys don't speak to this token".
 *
 * @param compact_jwt      Serialized JWT.
 * @param keys             Array of candidate keys; NULL slots are skipped.
 * @param key_count        Length of `keys`.
 * @param opts             Validation options (`allowed_algs` mandatory as usual).
 * @param out              Receives the verified token on `JWT_SUCCESS`.
 * @return Verification status.
 */
JwtStatus jwt_decode_and_verify_with_keys(const char* compact_jwt,
                                          const JwtKey* const* keys,
                                          size_t key_count,
                                          const JwtValidationOptions* opts,
                                          JwtToken** out) {
    if (out) *out = NULL;
    if (!compact_jwt || !opts || (!keys && key_count > 0)) {
        JWT_LOG("[jwt_decode_and_verify_with_keys]: NULL input/opts/keys");
        return JWT_ERR_GENERIC;
    }
    /* Size guard up-front, same as the single-key path. */
    if (opts->max_token_bytes > 0) {
        for (size_t i = 0; i <= opts->max_token_bytes; ++i) {
            if (compact_jwt[i] == '\0') break;
            if (i == opts->max_token_bytes) return JWT_ERR_TOKEN_TOO_LARGE;
        }
    }
    JwtToken* t = jwt_decode(compact_jwt);
    if (!t) return JWT_ERR_INVALID_FORMAT;

    JwtStatus last_rc = JWT_ERR_INVALID_SIGNATURE;
    bool any_tried   = false;

    for (size_t i = 0; i < key_count; ++i) {
        const JwtKey* k = keys[i];
        if (!k) continue;
        if (!jwt_key_shape_matches_alg(k, t->alg)) {
            JWT_LOG("[jwt_decode_and_verify_with_keys]: key[%zu] shape mismatch for %s",
                    i, jwt_alg_to_string(t->alg));
            continue;
        }
        any_tried = true;
        JwtStatus rc = jwt_verify(t, k, opts);
        if (rc == JWT_SUCCESS) {
            if (out) *out = t; else jwt_deallocate(t);
            JWT_LOG("[jwt_decode_and_verify_with_keys]: success on key[%zu]", i);
            return JWT_SUCCESS;
        }
        /* Key-independent failures don't get retried. */
        switch (rc) {
            case JWT_ERR_INVALID_ALG:
            case JWT_ERR_CRIT_UNSUPPORTED:
            case JWT_ERR_INVALID_TYP:
            case JWT_ERR_EXPIRED:
            case JWT_ERR_NOT_YET_VALID:
            case JWT_ERR_MISSING_CLAIM:
            case JWT_ERR_INVALID_ISSUER:
            case JWT_ERR_INVALID_AUDIENCE:
            case JWT_ERR_INVALID_SUBJECT:
            case JWT_ERR_INVALID_FORMAT:
            case JWT_ERR_TOKEN_TOO_LARGE:
            case JWT_ERR_WEAK_KEY:
                jwt_deallocate(t);
                return rc;
            default:
                last_rc = rc;
                break;
        }
    }

    jwt_deallocate(t);
    return any_tried ? last_rc : JWT_ERR_INVALID_KEY;
}

/**
 * @brief Decode + verify using a `JwtKeySet`, picking the right key by `kid`.
 *
 * If the token's header carries a `kid`, the matching keyset entry is
 * tried first (this is the fast path for properly-rotated IdPs). If
 * `kid` is absent OR no entry matches, the function falls back to
 * trying every key in the set whose shape is compatible with the
 * token's algorithm — covering pathological cases where the IdP forgot
 * to publish `kid`s.
 *
 * @param compact_jwt Serialized JWT.
 * @param set         Parsed keyset (e.g. from `jwt_keyset_from_json`).
 * @param opts        Validation options.
 * @param out         Receives the verified token on `JWT_SUCCESS`.
 * @return Verification status.
 */
JwtStatus jwt_decode_and_verify_with_keyset(const char* compact_jwt,
                                            const JwtKeySet* set,
                                            const JwtValidationOptions* opts,
                                            JwtToken** out) {
    if (out) *out = NULL;
    if (!compact_jwt || !opts || !set) {
        JWT_LOG("[jwt_decode_and_verify_with_keyset]: NULL input");
        return JWT_ERR_GENERIC;
    }

    /* Peek at the header (untrusted!) to read kid. */
    JwtToken* peeked = jwt_decode(compact_jwt);
    const char* kid = peeked ? jwt_get_header_string(peeked, "kid") : NULL;
    const JwtKey* primary = kid ? jwt_keyset_lookup(set, kid) : NULL;
    if (peeked) jwt_deallocate(peeked);

    /* Build a candidate list: primary key first (if any), then everything
       else in declared order. */
    const size_t n = set->count;
    const JwtKey** candidates = (const JwtKey**)calloc(n + 1, sizeof(JwtKey*));
    if (!candidates) return JWT_ERR_MEMORY;
    size_t k = 0;
    if (primary) candidates[k++] = primary;
    for (size_t i = 0; i < n; ++i) {
        if (set->entries[i].key && set->entries[i].key != primary) {
            candidates[k++] = set->entries[i].key;
        }
    }
    JwtStatus rc = jwt_decode_and_verify_with_keys(compact_jwt, candidates, k,
                                                   opts, out);
    free(candidates);
    return rc;
}

/* =================================================================== */
/* Accessors                                                            */
/* =================================================================== */

/** @brief Return the algorithm declared in the token's header. */
JwtAlgorithm jwt_get_algorithm(const JwtToken* t) {
    return t ? t->alg : JWT_ALG_NONE;
}

/* Helper: fetch a string-valued payload claim. */
static const char* jwt_obj_get_string(JsonElement* obj, const char* name) {
    if (!obj) return NULL;
    JsonElement* e = json_get_element(obj, name);
    if (!e || e->type != JSON_STRING) return NULL;
    return e->value.string_val;
}

/** @brief Return the `iss` claim string, or NULL if absent. */
const char* jwt_get_issuer(const JwtToken* t) {
    return t ? jwt_obj_get_string(t->payload, "iss") : NULL;
}
/** @brief Return the `sub` claim string, or NULL if absent. */
const char* jwt_get_subject(const JwtToken* t) {
    return t ? jwt_obj_get_string(t->payload, "sub") : NULL;
}
/** @brief Return the `jti` claim string, or NULL if absent. */
const char* jwt_get_jwt_id(const JwtToken* t) {
    return t ? jwt_obj_get_string(t->payload, "jti") : NULL;
}

static int64_t jwt_obj_get_unix(JsonElement* obj, const char* name, bool* present) {
    if (present) *present = false;
    if (!obj) return 0;
    JsonElement* e = json_get_element(obj, name);
    if (!e || e->type != JSON_NUMBER) return 0;
    int64_t v;
    /* An out-of-range / non-finite value isn't a usable Unix timestamp — report
       it as absent rather than performing an undefined (int64_t) cast. */
    if (!jwtp_json_num_to_int64(e->value.number_val, &v)) return 0;
    if (present) *present = true;
    return v;
}

/** @brief Return the `exp` claim (Unix seconds). *present is set true/false. */
int64_t jwt_get_expiration(const JwtToken* t, bool* present) {
    return t ? jwt_obj_get_unix(t->payload, "exp", present)
             : (present ? (*present = false, 0) : 0);
}
/** @brief Return the `nbf` claim (Unix seconds). *present is set true/false. */
int64_t jwt_get_not_before(const JwtToken* t, bool* present) {
    return t ? jwt_obj_get_unix(t->payload, "nbf", present)
             : (present ? (*present = false, 0) : 0);
}
/** @brief Return the `iat` claim (Unix seconds). *present is set true/false. */
int64_t jwt_get_issued_at(const JwtToken* t, bool* present) {
    return t ? jwt_obj_get_unix(t->payload, "iat", present)
             : (present ? (*present = false, 0) : 0);
}

/**
 * @brief Number of audiences in the `aud` claim.
 *
 * RFC 7519 §4.1.3 permits `aud` to be either a single string or an array of
 * strings; this presents both uniformly: 1 for a single string, the array
 * length for an array, and 0 if `aud` is absent or not a string/array.
 *
 * @param t Decoded token.
 * @return Audience count.
 */
size_t jwt_get_audience_count(const JwtToken* t) {
    if (!t) return 0;
    JsonElement* aud = json_get_element(t->payload, "aud");
    if (!aud) return 0;
    if (aud->type == JSON_STRING) return aud->value.string_val ? 1 : 0;
    if (aud->type == JSON_ARRAY)  return json_array_size(aud);
    return 0;
}

/**
 * @brief The `index`-th audience string, or NULL if out of range.
 *
 * Works whether `aud` is a single string (only `index == 0` is valid) or an
 * array. The returned pointer is borrowed (owned by the token).
 *
 * @param t     Decoded token.
 * @param index Zero-based audience index.
 * @return Borrowed audience string, or NULL if absent / out of range.
 */
const char* jwt_get_audience_at(const JwtToken* t, size_t index) {
    if (!t) return NULL;
    JsonElement* aud = json_get_element(t->payload, "aud");
    if (!aud) return NULL;
    if (aud->type == JSON_STRING) {
        return index == 0 ? aud->value.string_val : NULL;
    }
    if (aud->type == JSON_ARRAY) {
        char key[24];
        snprintf(key, sizeof(key), "%zu", index);
        JsonElement* item = json_get_element(aud, key);
        if (item && item->type == JSON_STRING) return item->value.string_val;
    }
    return NULL;
}

/** @brief Fetch a custom string claim by name. Returns NULL if absent. */
const char* jwt_get_claim_string(const JwtToken* t, const char* name) {
    return (t && name) ? jwt_obj_get_string(t->payload, name) : NULL;
}

/**
 * @brief Fetch a custom integer claim by name.
 * @return true if the claim is present and numeric; false otherwise.
 */
bool jwt_get_claim_int(const JwtToken* t, const char* name, int64_t* out) {
    if (!t || !name) return false;
    JsonElement* e = json_get_element(t->payload, name);
    if (!e || e->type != JSON_NUMBER) return false;
    if (out) *out = (int64_t)e->value.number_val;
    return true;
}

/**
 * @brief Fetch a custom floating-point claim by name.
 * @return true on present-and-numeric, false otherwise.
 */
bool jwt_get_claim_double(const JwtToken* t, const char* name, double* out) {
    if (!t || !name) return false;
    JsonElement* e = json_get_element(t->payload, name);
    if (!e || e->type != JSON_NUMBER) return false;
    if (out) *out = e->value.number_val;
    return true;
}

/**
 * @brief Fetch a custom boolean claim by name.
 * @return true on present-and-bool, false otherwise.
 */
bool jwt_get_claim_bool(const JwtToken* t, const char* name, bool* out) {
    if (!t || !name) return false;
    JsonElement* e = json_get_element(t->payload, name);
    if (!e || e->type != JSON_BOOL) return false;
    if (out) *out = e->value.bool_val;
    return true;
}

/** @brief Return the raw header JSON tree (read-only; owned by token). */
const JsonElement* jwt_get_header(const JwtToken* t) {
    return t ? t->header : NULL;
}
/** @brief Return the raw payload JSON tree (read-only; owned by token). */
const JsonElement* jwt_get_payload(const JwtToken* t) {
    return t ? t->payload : NULL;
}

/**
 * @brief Fetch a header parameter (e.g. `kid`) by name. NULL if absent.
 */
const char* jwt_get_header_string(const JwtToken* t, const char* name) {
    return (t && name) ? jwt_obj_get_string(t->header, name) : NULL;
}

/* =================================================================== */
/* Lifecycle                                                            */
/* =================================================================== */

/**
 * @brief Release every allocation owned by a token. NULL is a safe no-op.
 *
 * Wipes the signature bytes with `OPENSSL_cleanse` before free so any
 * stack-tracing dump after a verify failure doesn't leak signatures.
 */
void jwt_deallocate(JwtToken* t) {
    if (!t) {
        JWT_LOG("[jwt_deallocate]: NULL token (no-op)");
        return;
    }
    if (t->header)  json_deallocate(t->header);
    if (t->payload) json_deallocate(t->payload);
    if (t->signature) {
        OPENSSL_cleanse(t->signature, t->signature_len);
        free(t->signature);
    }
    free(t->signing_input);
    free(t);
    JWT_LOG("[jwt_deallocate]: token released");
}
