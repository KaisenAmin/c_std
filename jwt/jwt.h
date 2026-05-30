/**
 * @author Amin Tahmasebi
 * @date 2026
 * @class Jwt
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in jwt.c (file-level convention).
 *
 * RFC 7519 JSON Web Token (compact serialization). Supports HS256/384/512
 * and RS256/384/512.
 *
 * SECURITY MODEL
 * --------------
 *  - `jwt_verify` REQUIRES a non-NULL `allowed_algs` allow-list and rejects
 *    anything else (including the historical "none" algorithm by default).
 *    This is the single most important rule for JWT safety; the library
 *    enforces it unconditionally.
 *  - Algorithm-key-type binding is enforced: an HMAC alg with an RSA key
 *    (or vice versa) returns `JWT_ERR_INVALID_KEY` before any crypto runs.
 *  - Signature comparison is constant-time via OpenSSL `CRYPTO_memcmp`.
 *
 * ERROR MODEL
 * -----------
 *  - No global last-error state. Every function reports failure via its
 *    return value (NULL / false / `JwtStatus`).
 *  - No library function ever calls `exit()`.
 */

#ifndef JWT_H_
#define JWT_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "../json/json.h"

#ifdef __cplusplus
extern "C" {
#endif


/* #define JWT_LOGGING_ENABLE */

#ifdef JWT_LOGGING_ENABLE
    #define JWT_LOG(fmt, ...) \
        do { fprintf(stderr, "[JWT LOG]: " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define JWT_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Public types — algorithms                                          */
/* ------------------------------------------------------------------ */

typedef enum {
    JWT_ALG_NONE = 0,   /* RFC 7519 "none" — accepted only if explicitly allow-listed */
    JWT_ALG_HS256,
    JWT_ALG_HS384,
    JWT_ALG_HS512,
    JWT_ALG_RS256,
    JWT_ALG_RS384,
    JWT_ALG_RS512,
    JWT_ALG_ES256,      /* ECDSA P-256 + SHA-256 (RFC 7518 §3.4)            */
    JWT_ALG_ES384,      /* ECDSA P-384 + SHA-384                            */
    JWT_ALG_ES512,      /* ECDSA P-521 + SHA-512 (note: P-521, not P-512)   */
    JWT_ALG_PS256,      /* RSASSA-PSS + SHA-256 + MGF1-SHA-256 (§3.5)       */
    JWT_ALG_PS384,      /* RSASSA-PSS + SHA-384 + MGF1-SHA-384              */
    JWT_ALG_PS512       /* RSASSA-PSS + SHA-512 + MGF1-SHA-512              */
} JwtAlgorithm;


/* ------------------------------------------------------------------ */
/* Public types — status codes                                        */
/* ------------------------------------------------------------------ */

typedef enum {
    JWT_SUCCESS = 0,
    JWT_ERR_INVALID_FORMAT,        /* not three dot-separated segments              */
    JWT_ERR_INVALID_BASE64,
    JWT_ERR_INVALID_JSON,
    JWT_ERR_INVALID_ALG,           /* alg not in allow-list                         */
    JWT_ERR_INVALID_KEY,           /* HMAC alg with RSA key, or vice versa          */
    JWT_ERR_INVALID_SIGNATURE,     /* signature bytes don't verify                  */
    JWT_ERR_EXPIRED,               /* exp claim < now - leeway                      */
    JWT_ERR_NOT_YET_VALID,         /* nbf claim > now + leeway                      */
    JWT_ERR_INVALID_ISSUER,
    JWT_ERR_INVALID_AUDIENCE,
    JWT_ERR_INVALID_SUBJECT,
    JWT_ERR_MISSING_CLAIM,
    JWT_ERR_CRYPTO,                /* OpenSSL call failed                           */
    JWT_ERR_MEMORY,
    JWT_ERR_CRIT_UNSUPPORTED,      /* header `crit` names unknown extensions        */
    JWT_ERR_INVALID_TYP,           /* header `typ` mismatch                         */
    JWT_ERR_TOKEN_TOO_LARGE,       /* compact token exceeds opts->max_token_bytes   */
    JWT_ERR_WEAK_KEY,              /* HMAC secret shorter than digest output size   */
    JWT_ERR_GENERIC
} JwtStatus;


/* ------------------------------------------------------------------ */
/* Public types — opaque handles                                      */
/* ------------------------------------------------------------------ */

typedef struct JwtToken  JwtToken;
typedef struct JwtKey    JwtKey;
typedef struct JwtKeySet JwtKeySet;


/* ------------------------------------------------------------------ */
/* Public types — validation options                                  */
/* ------------------------------------------------------------------ */

/* All optional; NULL/0 means "don't enforce" — EXCEPT `allowed_algs`,
 * which MUST be non-NULL. The signature-verifying step refuses to run
 * without an explicit allow-list (defense against the "none" attack). */
typedef struct {
    const char*         expected_iss;
    const char**        expected_aud;
    size_t              expected_aud_count;
    const char*         expected_sub;
    int64_t             now_unix;                  /* 0 → use time(NULL)                */
    int64_t             leeway_seconds;
    bool                require_exp;
    bool                require_nbf;
    bool                require_iat;
    const JwtAlgorithm* allowed_algs;              /* REQUIRED                          */
    size_t              allowed_algs_count;

    /* Hardening knobs (all opt-in; zero defaults preserve old behavior). */
    const char*         expected_typ;              /* `typ` must equal this if non-NULL */
    size_t              max_token_bytes;           /* 0 means unlimited                 */
    bool                reject_weak_hmac_key;
} JwtValidationOptions;


/* ------------------------------------------------------------------ */
/* Key construction / destruction                                     */
/* ------------------------------------------------------------------ */

JwtKey*             jwt_key_hmac                       (const unsigned char* secret, size_t secret_len);
JwtKey*             jwt_key_rsa_private_pem            (const char* pem, size_t pem_len);
JwtKey*             jwt_key_rsa_public_pem             (const char* pem, size_t pem_len);
JwtKey*             jwt_key_rsa_private_file           (const char* path);
JwtKey*             jwt_key_rsa_public_file            (const char* path);
JwtKey*             jwt_key_ec_private_pem             (const char* pem, size_t pem_len);
JwtKey*             jwt_key_ec_public_pem              (const char* pem, size_t pem_len);
JwtKey*             jwt_key_ec_private_file            (const char* path);
JwtKey*             jwt_key_ec_public_file             (const char* path);
void                jwt_key_deallocate                 (JwtKey* key);


/* ------------------------------------------------------------------ */
/* JWKS — RFC 7517 JSON Web Key Set                                   */
/* ------------------------------------------------------------------ */

JwtKeySet*          jwt_keyset_from_json               (const char* jwks_json);
JwtKeySet*          jwt_keyset_from_file               (const char* path);
const JwtKey*       jwt_keyset_lookup                  (const JwtKeySet* set, const char* kid);
size_t              jwt_keyset_size                    (const JwtKeySet* set);
const JwtKey*       jwt_keyset_at                      (const JwtKeySet* set, size_t i);
const char*         jwt_keyset_kid_at                  (const JwtKeySet* set, size_t i);
const char*         jwt_keyset_alg_at                  (const JwtKeySet* set, size_t i);
void                jwt_keyset_deallocate              (JwtKeySet* set);


/* ------------------------------------------------------------------ */
/* Builder — construct + claim setters                                */
/* ------------------------------------------------------------------ */

JwtToken*           jwt_create                         (JwtAlgorithm alg);

/* RFC 7519 §4.1 registered claims. */
void                jwt_set_issuer                     (JwtToken* t, const char* iss);
void                jwt_set_subject                    (JwtToken* t, const char* sub);
void                jwt_set_audience                   (JwtToken* t, const char* aud);
void                jwt_set_audiences                  (JwtToken* t, const char** auds, size_t n);
void                jwt_set_expiration                 (JwtToken* t, int64_t exp_unix);
void                jwt_set_not_before                 (JwtToken* t, int64_t nbf_unix);
void                jwt_set_issued_at                  (JwtToken* t, int64_t iat_unix);
void                jwt_set_jwt_id                     (JwtToken* t, const char* jti);

/* Custom payload claims. */
bool                jwt_set_claim_string               (JwtToken* t, const char* name, const char* value);
bool                jwt_set_claim_int                  (JwtToken* t, const char* name, int64_t value);
bool                jwt_set_claim_double               (JwtToken* t, const char* name, double value);
bool                jwt_set_claim_bool                 (JwtToken* t, const char* name, bool value);
bool                jwt_set_claim_json                 (JwtToken* t, const char* name, JsonElement* value);

/* Custom header parameters (e.g. "kid"). */
bool                jwt_set_header_string              (JwtToken* t, const char* name, const char* value);


/* ------------------------------------------------------------------ */
/* Encoding (sign + serialize)                                        */
/* ------------------------------------------------------------------ */

char*               jwt_encode                         (const JwtToken* t, const JwtKey* signing_key);


/* ------------------------------------------------------------------ */
/* Parsing / verification                                             */
/* ------------------------------------------------------------------ */

JwtToken*           jwt_decode                         (const char* compact_jwt);
JwtStatus           jwt_verify                         (const JwtToken* t, const JwtKey* verification_key, const JwtValidationOptions* opts);
JwtStatus           jwt_decode_and_verify              (const char* compact_jwt, const JwtKey* verification_key, const JwtValidationOptions* opts, JwtToken** out);
JwtStatus           jwt_decode_and_verify_with_keys    (const char* compact_jwt, const JwtKey* const* keys, size_t key_count, const JwtValidationOptions* opts,
                                                        JwtToken** out);
JwtStatus           jwt_decode_and_verify_with_keyset  (const char* compact_jwt, const JwtKeySet* set, const JwtValidationOptions* opts, JwtToken** out);


/* ------------------------------------------------------------------ */
/* Claim accessors (post-decode)                                      */
/* ------------------------------------------------------------------ */

JwtAlgorithm        jwt_get_algorithm                  (const JwtToken* t);
const char*         jwt_get_issuer                     (const JwtToken* t);
const char*         jwt_get_subject                    (const JwtToken* t);
const char*         jwt_get_jwt_id                     (const JwtToken* t);
int64_t             jwt_get_expiration                 (const JwtToken* t, bool* present);
int64_t             jwt_get_not_before                 (const JwtToken* t, bool* present);
int64_t             jwt_get_issued_at                  (const JwtToken* t, bool* present);

const char*         jwt_get_claim_string               (const JwtToken* t, const char* name);
bool                jwt_get_claim_int                  (const JwtToken* t, const char* name, int64_t* out);
bool                jwt_get_claim_double               (const JwtToken* t, const char* name, double* out);
bool                jwt_get_claim_bool                 (const JwtToken* t, const char* name, bool* out);

const JsonElement*  jwt_get_header                     (const JwtToken* t);
const JsonElement*  jwt_get_payload                    (const JwtToken* t);
const char*         jwt_get_header_string              (const JwtToken* t, const char* name);


/* ------------------------------------------------------------------ */
/* Token lifecycle                                                    */
/* ------------------------------------------------------------------ */

void                jwt_deallocate                     (JwtToken* t);


/* ------------------------------------------------------------------ */
/* Introspection helpers                                              */
/* ------------------------------------------------------------------ */

const char*         jwt_status_to_string               (JwtStatus s);
const char*         jwt_alg_to_string                  (JwtAlgorithm a);
JwtAlgorithm        jwt_alg_from_string                (const char* s);
bool                jwt_alg_is_hmac                    (JwtAlgorithm a);
bool                jwt_alg_is_rsa                     (JwtAlgorithm a);    /* RSA-PKCS#1-v1.5 only */
bool                jwt_alg_is_rsa_pss                 (JwtAlgorithm a);    /* PS256/384/512        */
bool                jwt_alg_is_ec                      (JwtAlgorithm a);
bool                jwt_alg_uses_rsa_key               (JwtAlgorithm a);    /* RS* or PS*           */


#ifdef __cplusplus
}
#endif

#endif 
