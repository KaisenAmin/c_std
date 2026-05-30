# Jwt Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2026  
**License:** ISC License

# JWT — JSON Web Token (RFC 7519) for `c_std`

A small, fast, **secure-by-default** JWT implementation. Supports
HS256 / HS384 / HS512 (HMAC), RS256 / RS384 / RS512 (RSA-PKCS#1-v1.5),
PS256 / PS384 / PS512 (RSASSA-PSS), and ES256 / ES384 / ES512 (ECDSA on
P-256 / P-384 / P-521). Ships with a built-in **JWKS** (RFC 7517) parser
and a **multi-key verify** entry point so key rotation is one function
call. Compact serialization only. Backed by OpenSSL for the cryptography
and by the in-tree `json`, `string`, and `encoding` modules for
everything else.

```
┌─────────────────────────────────────────────────────────────┐
│  jwt_create → set claims → jwt_encode(token, key) → "h.p.s" │
│                                                             │
│  "h.p.s" → jwt_decode_and_verify(opts) → JwtToken*          │
└─────────────────────────────────────────────────────────────┘
```

---

## Security model

This library is designed so that the most-common JWT vulnerabilities are
structurally impossible:

1. **Algorithm allow-list is mandatory.** `jwt_verify` REQUIRES a non-NULL
   `opts->allowed_algs` and returns `JWT_ERR_INVALID_ALG` if you forget.
   This single rule defeats the historical *"none"-algorithm attack*.
2. **`alg=none` is rejected by default.** It will only verify if you
   explicitly include `JWT_ALG_NONE` in your allow-list.
3. **Algorithm-key-type binding is enforced.** Submitting an HS256 token
   with an RSA public key (the classic *algorithm confusion* attack)
   returns `JWT_ERR_INVALID_KEY` *before* any crypto runs. Key loaders
   also refuse cross-type PEM (an EC PEM handed to `jwt_key_rsa_*`
   returns NULL up front).
4. **EC curve / algorithm binding is enforced.** ES256 requires P-256,
   ES384 requires P-384, ES512 requires P-521. Mismatched pairs fail at
   encode time (NULL) and at verify (`JWT_ERR_INVALID_SIGNATURE`).
5. **`crit` header rejected.** RFC 7515 §4.1.11 — any header naming
   "must-understand" extensions returns `JWT_ERR_CRIT_UNSUPPORTED`,
   because this implementation supports no extensions.
6. **Constant-time signature comparison.** HMAC signature compare uses
   OpenSSL's `CRYPTO_memcmp`; ECDSA signatures are length-checked before
   being handed to OpenSSL.
7. **Standard claims are honored.** `exp`, `nbf`, `iat`, `iss`, `sub`,
   `aud` (both string and array forms), and optionally header `typ` are
   all validated when the corresponding options are set, with symmetric
   `leeway_seconds` for clock skew.
8. **DoS guards.** `opts->max_token_bytes` caps the size of tokens the
   parser will even look at. `opts->reject_weak_hmac_key` refuses HMAC
   secrets shorter than the digest output length (RFC 7518 §3.2).
9. **HMAC secrets are zeroized on free** via `OPENSSL_cleanse`.

---

## JWT Types

| Type | Description |
|---|---|
| `JwtAlgorithm` | enum: `JWT_ALG_NONE`, `JWT_ALG_HS256/384/512`, `JWT_ALG_RS256/384/512`, `JWT_ALG_PS256/384/512`, `JWT_ALG_ES256/384/512`. |
| `JwtStatus` | enum: `JWT_SUCCESS` plus 19 distinct error codes (one per failure mode). |
| `JwtToken` | opaque — holds parsed header / payload / signature / signing-input. |
| `JwtKey` | opaque — wraps either HMAC secret bytes, an `EVP_PKEY` RSA key, or an `EVP_PKEY` EC key. |
| `JwtKeySet` | opaque — a set of `JwtKey`s parsed from a JWKS (RFC 7517), each tagged with `kid` / `alg`. Owns its keys. |
| `JwtValidationOptions` | struct of validation knobs (`allowed_algs`, `expected_iss`, `expected_aud`, `expected_typ`, `leeway_seconds`, `max_token_bytes`, `reject_weak_hmac_key`, …). |

---

## JWT Function Descriptions

**Key construction**

### `JwtKey* jwt_key_hmac(const unsigned char* secret, size_t secret_len)`
**Purpose**: Builds an HMAC key by copying `secret_len` bytes from `secret`.
**Parameters**:
- `secret`: Pointer to the raw secret bytes.
- `secret_len`: Number of bytes to copy from `secret`.
**Return Value**: A newly allocated `JwtKey*`, or `NULL` on OOM or if `secret_len` is 0.
**Usage Case**: Use when creating or verifying HS256/HS384/HS512 tokens with a shared secret; the caller may free its buffer immediately after this call.

---

### `JwtKey* jwt_key_rsa_private_pem(const char* pem, size_t pem_len)`
**Purpose**: Parses a PEM RSA private key (PKCS#1 or PKCS#8) from `pem`.
**Parameters**:
- `pem`: Pointer to the PEM-encoded key data.
- `pem_len`: Length of the PEM data in bytes.
**Return Value**: A newly allocated `JwtKey*`, or `NULL` on parse failure.
**Usage Case**: Use when signing RS256/384/512 tokens on an auth server that holds the private key.

---

### `JwtKey* jwt_key_rsa_public_pem(const char* pem, size_t pem_len)`
**Purpose**: Parses a PEM SubjectPublicKeyInfo (`BEGIN PUBLIC KEY`) RSA public key.
**Parameters**:
- `pem`: Pointer to the PEM-encoded key data.
- `pem_len`: Length of the PEM data in bytes.
**Return Value**: A newly allocated `JwtKey*`, or `NULL` on parse failure.
**Usage Case**: Use when verifying RS256/384/512 tokens on any service that only needs the public key.

---

### `JwtKey* jwt_key_rsa_private_file(const char* path)`
**Purpose**: Reads a PEM file from disk and delegates to `jwt_key_rsa_private_pem`.
**Parameters**:
- `path`: Filesystem path to the PEM file.
**Return Value**: A newly allocated `JwtKey*`, or `NULL` on read or parse failure; refuses non-RSA keys.
**Usage Case**: Use as a convenience wrapper when the RSA private key is stored as a file on disk.

---

### `JwtKey* jwt_key_rsa_public_file(const char* path)`
**Purpose**: Reads a PEM file from disk and delegates to `jwt_key_rsa_public_pem`.
**Parameters**:
- `path`: Filesystem path to the PEM file.
**Return Value**: A newly allocated `JwtKey*`, or `NULL` on read or parse failure.
**Usage Case**: Use as a convenience wrapper when the RSA public key is stored as a file on disk.

---

### `JwtKey* jwt_key_ec_private_pem(const char* pem, size_t pem_len)`
**Purpose**: Parses an EC private key (SEC1 or PKCS#8 PEM).
**Parameters**:
- `pem`: Pointer to the PEM-encoded EC private key data.
- `pem_len`: Length of the PEM data in bytes.
**Return Value**: A newly allocated `JwtKey*`, or `NULL` on parse failure.
**Usage Case**: Use when signing ES256/384/512 tokens; the algorithm chosen at encode time must match the curve in the key (P-256 for ES256, P-384 for ES384, P-521 for ES512).

---

### `JwtKey* jwt_key_ec_public_pem(const char* pem, size_t pem_len)`
**Purpose**: Parses an EC SubjectPublicKeyInfo (`BEGIN PUBLIC KEY`).
**Parameters**:
- `pem`: Pointer to the PEM-encoded EC public key data.
- `pem_len`: Length of the PEM data in bytes.
**Return Value**: A newly allocated `JwtKey*`, or `NULL` on parse failure or if the key is not an EC key.
**Usage Case**: Use when verifying ES256/384/512 tokens on services that hold only the public key.

---

### `JwtKey* jwt_key_ec_private_file(const char* path)`
**Purpose**: Reads a PEM file from disk and delegates to `jwt_key_ec_private_pem`.
**Parameters**:
- `path`: Filesystem path to the PEM file.
**Return Value**: A newly allocated `JwtKey*`, or `NULL` on read or parse failure.
**Usage Case**: Use as a convenience wrapper when the EC private key is stored as a file on disk.

---

### `JwtKey* jwt_key_ec_public_file(const char* path)`
**Purpose**: Reads a PEM file from disk and delegates to `jwt_key_ec_public_pem`.
**Parameters**:
- `path`: Filesystem path to the PEM file.
**Return Value**: A newly allocated `JwtKey*`, or `NULL` on read or parse failure.
**Usage Case**: Use as a convenience wrapper when the EC public key is stored as a file on disk.

---

### `void jwt_key_deallocate(JwtKey* key)`
**Purpose**: Wipes HMAC secrets with `OPENSSL_cleanse`, frees any `EVP_PKEY`, then frees the wrapper struct.
**Parameters**:
- `key`: The key to free; `NULL` is a safe no-op.
**Return Value**: None.
**Usage Case**: Call when a key is no longer needed to securely erase secret material and release memory.

---

**JWKS**

### `JwtKeySet* jwt_keyset_from_json(const char* jwks_json)`
**Purpose**: Parses a JWKS document (`{"keys":[...]}`) into usable keys.
**Parameters**:
- `jwks_json`: Null-terminated JSON string containing the JWKS document.
**Return Value**: A newly allocated `JwtKeySet*`, or `NULL` only on JSON-level parse failure; entries with invalid material, `use=enc`, or unsupported `kty`/`crv` are silently skipped.
**Usage Case**: Use to load a provider's published JWKS for subsequent token verification; an empty-but-valid keyset is returned if all entries are skipped.

---

### `JwtKeySet* jwt_keyset_from_file(const char* path)`
**Purpose**: Reads a JWKS file from disk and delegates to `jwt_keyset_from_json`.
**Parameters**:
- `path`: Filesystem path to the JWKS JSON file.
**Return Value**: A newly allocated `JwtKeySet*`, or `NULL` on read or parse failure.
**Usage Case**: Use as a convenience wrapper when the JWKS document is stored as a local file.

---

### `const JwtKey* jwt_keyset_lookup(const JwtKeySet* set, const char* kid)`
**Purpose**: Finds the key tagged with `kid` (case-sensitive) and returns a borrowed pointer owned by the set.
**Parameters**:
- `set`: The key set to search.
- `kid`: The key identifier string to look up.
**Return Value**: A borrowed `const JwtKey*` owned by the set, or `NULL` if no match; do not call `jwt_key_deallocate` on it.
**Usage Case**: Use to select the correct verification key when the token's header contains a known `kid`.

---

### `size_t jwt_keyset_size(const JwtKeySet* set)`
**Purpose**: Returns the number of successfully parsed keys in the set.
**Parameters**:
- `set`: The key set to query; `NULL` returns 0.
**Return Value**: The count of keys in the set, or 0 if `set` is `NULL`.
**Usage Case**: Use when iterating over all keys in a keyset or checking whether any keys were loaded.

---

### `const JwtKey* jwt_keyset_at(const JwtKeySet* set, size_t i)`
**Purpose**: Returns a borrowed pointer to the key at index `i`.
**Parameters**:
- `set`: The key set to index into.
- `i`: Zero-based index of the desired key.
**Return Value**: A borrowed `const JwtKey*`, or `NULL` if `i` is out of range.
**Usage Case**: Use when iterating over all keys in a keyset by index.

---

### `const char* jwt_keyset_kid_at(const JwtKeySet* set, size_t i)`
**Purpose**: Returns the `kid` string for the key at index `i`.
**Parameters**:
- `set`: The key set to index into.
- `i`: Zero-based index of the desired key.
**Return Value**: The `kid` string, or `NULL` if absent or `i` is out of range.
**Usage Case**: Use when enumerating key identifiers in a keyset for display or diagnostic purposes.

---

### `const char* jwt_keyset_alg_at(const JwtKeySet* set, size_t i)`
**Purpose**: Returns the `alg` string for the key at index `i`.
**Parameters**:
- `set`: The key set to index into.
- `i`: Zero-based index of the desired key.
**Return Value**: The `alg` string, or `NULL` if absent or `i` is out of range.
**Usage Case**: Use when enumerating algorithm labels in a keyset for filtering or diagnostic purposes.

---

### `void jwt_keyset_deallocate(JwtKeySet* set)`
**Purpose**: Frees every key in the set and the set struct itself.
**Parameters**:
- `set`: The key set to free; `NULL` is a safe no-op.
**Return Value**: None.
**Usage Case**: Call when the keyset is no longer needed to release all associated memory.

---

**Token builder**

### `JwtToken* jwt_create(JwtAlgorithm alg)`
**Purpose**: Allocates a new token and seeds the JOSE header with `typ=JWT` and `alg=<name>`.
**Parameters**:
- `alg`: The signing algorithm to associate with this token.
**Return Value**: A newly allocated `JwtToken*`, or `NULL` on OOM.
**Usage Case**: Use as the first step when constructing a JWT to issue to a client.

---

### `void jwt_set_issuer(JwtToken* t, const char* iss)`
**Purpose**: Sets the `iss` (issuer) registered claim.
**Parameters**:
- `t`: The token to modify.
- `iss`: The issuer string value.
**Return Value**: None.
**Usage Case**: Use to identify the principal that issued the token, such as an auth server URL.

---

### `void jwt_set_subject(JwtToken* t, const char* sub)`
**Purpose**: Sets the `sub` (subject) registered claim.
**Parameters**:
- `t`: The token to modify.
- `sub`: The subject string value.
**Return Value**: None.
**Usage Case**: Use to identify the principal that is the subject of the token, such as a user ID.

---

### `void jwt_set_audience(JwtToken* t, const char* aud)`
**Purpose**: Sets the `aud` claim as a plain string (single-audience form).
**Parameters**:
- `t`: The token to modify.
- `aud`: The audience string value.
**Return Value**: None.
**Usage Case**: Use when the token is intended for exactly one recipient service.

---

### `void jwt_set_audiences(JwtToken* t, const char** auds, size_t n)`
**Purpose**: Sets the `aud` claim as a JSON array (multi-audience form).
**Parameters**:
- `t`: The token to modify.
- `auds`: Array of audience string values.
- `n`: Number of strings in `auds`.
**Return Value**: None.
**Usage Case**: Use when the token is addressed to multiple recipient services simultaneously.

---

### `void jwt_set_expiration(JwtToken* t, int64_t exp_unix)`
**Purpose**: Sets the `exp` (expiration) claim as a Unix timestamp.
**Parameters**:
- `t`: The token to modify.
- `exp_unix`: Unix timestamp after which the token must be rejected.
**Return Value**: None.
**Usage Case**: Use to bound the token's validity window and prevent indefinite reuse.

---

### `void jwt_set_not_before(JwtToken* t, int64_t nbf_unix)`
**Purpose**: Sets the `nbf` (not-before) claim as a Unix timestamp.
**Parameters**:
- `t`: The token to modify.
- `nbf_unix`: Unix timestamp before which the token must be rejected.
**Return Value**: None.
**Usage Case**: Use to delay activation of a token, e.g. for scheduled access grants.

---

### `void jwt_set_issued_at(JwtToken* t, int64_t iat_unix)`
**Purpose**: Sets the `iat` (issued-at) claim as a Unix timestamp.
**Parameters**:
- `t`: The token to modify.
- `iat_unix`: Unix timestamp recording when the token was created.
**Return Value**: None.
**Usage Case**: Use to record token creation time for auditing or relative-age checks.

---

### `void jwt_set_jwt_id(JwtToken* t, const char* jti)`
**Purpose**: Sets the `jti` (JWT ID) registered claim.
**Parameters**:
- `t`: The token to modify.
- `jti`: A unique identifier string for this token.
**Return Value**: None.
**Usage Case**: Use to assign a unique ID for nonce tracking or token revocation lists.

---

### `bool jwt_set_claim_string(JwtToken* t, const char* name, const char* value)`
**Purpose**: Sets a custom string payload claim, replacing any prior value with the same name.
**Parameters**:
- `t`: The token to modify.
- `name`: The claim name.
- `value`: The string value to set.
**Return Value**: `true` on success, `false` on OOM.
**Usage Case**: Use to embed application-specific string data such as roles or tenant IDs in the payload.

---

### `bool jwt_set_claim_int(JwtToken* t, const char* name, int64_t value)`
**Purpose**: Sets a custom integer payload claim.
**Parameters**:
- `t`: The token to modify.
- `name`: The claim name.
- `value`: The integer value to set.
**Return Value**: `true` on success, `false` on OOM.
**Usage Case**: Use to embed numeric identifiers or counters as custom claims in the payload.

---

### `bool jwt_set_claim_double(JwtToken* t, const char* name, double value)`
**Purpose**: Sets a custom floating-point payload claim.
**Parameters**:
- `t`: The token to modify.
- `name`: The claim name.
- `value`: The double value to set.
**Return Value**: `true` on success, `false` on OOM.
**Usage Case**: Use to embed fractional numeric data as custom claims in the payload.

---

### `bool jwt_set_claim_bool(JwtToken* t, const char* name, bool value)`
**Purpose**: Sets a custom boolean payload claim.
**Parameters**:
- `t`: The token to modify.
- `name`: The claim name.
- `value`: The boolean value to set.
**Return Value**: `true` on success, `false` on OOM.
**Usage Case**: Use to embed boolean flags such as feature toggles or permission bits in the payload.

---

### `bool jwt_set_claim_json(JwtToken* t, const char* name, JsonElement* value)`
**Purpose**: Attaches an arbitrary JSON value as a payload claim; the token takes ownership of `value` on success.
**Parameters**:
- `t`: The token to modify.
- `name`: The claim name.
- `value`: A `JsonElement*` to attach; on failure the caller retains ownership.
**Return Value**: `true` on success, `false` on OOM.
**Usage Case**: Use to embed nested objects or arrays as custom claims in the payload.

---

### `bool jwt_set_header_string(JwtToken* t, const char* name, const char* value)`
**Purpose**: Sets a custom JOSE header parameter.
**Parameters**:
- `t`: The token to modify.
- `name`: The header parameter name.
- `value`: The string value to set.
**Return Value**: `true` on success, `false` on OOM.
**Usage Case**: Commonly used to set `kid` for key-rotation so the verifier knows which key to use.

---

**Encoding**

### `char* jwt_encode(const JwtToken* t, const JwtKey* signing_key)`
**Purpose**: Serializes header and payload as JSON, base64url-encodes them, joins with `.`, signs with the key, and appends the base64url signature.
**Parameters**:
- `t`: The token to encode.
- `signing_key`: The key whose shape must match the token's algorithm.
**Return Value**: A malloc'd compact JWT string the caller must `free`, or `NULL` if the key shape does not match the algorithm or on OOM.
**Usage Case**: Use as the final step to produce the compact JWT string for transmission to clients.

---

**Parsing and verification**

### `JwtToken* jwt_decode(const char* compact_jwt)`
**Purpose**: Parses a compact JWT without verifying the signature.
**Parameters**:
- `compact_jwt`: Null-terminated compact JWT string (three dot-separated base64url segments).
**Return Value**: A newly allocated `JwtToken*`, or `NULL` on malformed input; the token must not be trusted until verified.
**Usage Case**: Use to read the `kid` header before key selection, then pass the result to `jwt_verify`.

---

### `JwtStatus jwt_verify(const JwtToken* t, const JwtKey* verification_key, const JwtValidationOptions* opts)`
**Purpose**: Runs the full verification pipeline: algorithm allow-list, key-shape binding, constant-time signature check, and claim validation.
**Parameters**:
- `t`: The parsed (but unverified) token to verify.
- `verification_key`: The key to verify the signature against.
- `opts`: Validation options; `opts->allowed_algs` is mandatory.
**Return Value**: `JWT_SUCCESS` or a specific `JWT_ERR_*` error code.
**Usage Case**: Use after `jwt_decode` when you need to separate key selection from full verification.

---

### `JwtStatus jwt_decode_and_verify(const char* compact_jwt, const JwtKey* verification_key, const JwtValidationOptions* opts, JwtToken** out)`
**Purpose**: One-shot parse and verify; on `JWT_SUCCESS`, `*out` holds the parsed token the caller must `jwt_deallocate`.
**Parameters**:
- `compact_jwt`: Null-terminated compact JWT string.
- `verification_key`: The key to verify the signature against.
- `opts`: Validation options; `opts->allowed_algs` is mandatory.
- `out`: Output pointer set to the parsed token on success, or `NULL` on failure.
**Return Value**: `JWT_SUCCESS` or a specific `JWT_ERR_*` error code.
**Usage Case**: Use for the common single-key verification flow to minimize boilerplate.

---

### `JwtStatus jwt_decode_and_verify_with_keys(const char* compact_jwt, const JwtKey* const* keys, size_t key_count, const JwtValidationOptions* opts, JwtToken** out)`
**Purpose**: Multi-key verify for key rotation; tries each key in order until the first shape-compatible key with a valid signature wins.
**Parameters**:
- `compact_jwt`: Null-terminated compact JWT string.
- `keys`: Array of key pointers to try in order.
- `key_count`: Number of keys in `keys`.
- `opts`: Validation options; `opts->allowed_algs` is mandatory.
- `out`: Output pointer set to the parsed token on success, or `NULL` on failure.
**Return Value**: `JWT_SUCCESS`, `JWT_ERR_INVALID_SIGNATURE` if no key verified, `JWT_ERR_INVALID_KEY` if no key was shape-compatible, or another `JWT_ERR_*` on key-independent failures.
**Usage Case**: Use during key rotation overlap windows when both old and new public keys must be accepted.

---

### `JwtStatus jwt_decode_and_verify_with_keyset(const char* compact_jwt, const JwtKeySet* set, const JwtValidationOptions* opts, JwtToken** out)`
**Purpose**: Auto-selects a key by the token's `kid` header if present and found in the set; otherwise tries every shape-compatible key.
**Parameters**:
- `compact_jwt`: Null-terminated compact JWT string.
- `set`: The key set to select from.
- `opts`: Validation options; `opts->allowed_algs` is mandatory.
- `out`: Output pointer set to the parsed token on success, or `NULL` on failure.
**Return Value**: `JWT_SUCCESS` or a specific `JWT_ERR_*` error code.
**Usage Case**: Use as the most natural pairing with `jwt_keyset_from_json` for OIDC provider verification.

---

**Claim accessors**

### `JwtAlgorithm jwt_get_algorithm(const JwtToken* t)`
**Purpose**: Returns the algorithm declared in the token's JOSE header.
**Parameters**:
- `t`: The token to query.
**Return Value**: The `JwtAlgorithm` enum value declared in the header.
**Usage Case**: Use to inspect the algorithm before deciding which key constructor to call.

---

### `const char* jwt_get_issuer(const JwtToken* t)`
**Purpose**: Returns the `iss` claim string.
**Parameters**:
- `t`: The token to query.
**Return Value**: The `iss` string, or `NULL` if absent.
**Usage Case**: Use to read the issuer after verification for application-level authorization decisions.

---

### `const char* jwt_get_subject(const JwtToken* t)`
**Purpose**: Returns the `sub` claim string.
**Parameters**:
- `t`: The token to query.
**Return Value**: The `sub` string, or `NULL` if absent.
**Usage Case**: Use to identify the authenticated user or principal represented by the token.

---

### `const char* jwt_get_jwt_id(const JwtToken* t)`
**Purpose**: Returns the `jti` claim string.
**Parameters**:
- `t`: The token to query.
**Return Value**: The `jti` string, or `NULL` if absent.
**Usage Case**: Use to retrieve the unique token ID for replay detection or revocation checks.

---

### `int64_t jwt_get_expiration(const JwtToken* t, bool* present)`
**Purpose**: Returns the `exp` claim as a Unix timestamp and signals whether the claim exists.
**Parameters**:
- `t`: The token to query.
- `present`: Set to `true` if the claim exists, `false` if absent; distinguishes "absent" from "zero".
**Return Value**: The `exp` Unix timestamp, or 0 if absent.
**Usage Case**: Use to read the expiration time for custom time-range checks beyond what the verifier enforces.

---

### `int64_t jwt_get_not_before(const JwtToken* t, bool* present)`
**Purpose**: Returns the `nbf` claim as a Unix timestamp and signals whether the claim exists.
**Parameters**:
- `t`: The token to query.
- `present`: Set to `true` if the claim exists, `false` if absent.
**Return Value**: The `nbf` Unix timestamp, or 0 if absent.
**Usage Case**: Use to read the not-before time for custom activation-window checks.

---

### `int64_t jwt_get_issued_at(const JwtToken* t, bool* present)`
**Purpose**: Returns the `iat` claim as a Unix timestamp and signals whether the claim exists.
**Parameters**:
- `t`: The token to query.
- `present`: Set to `true` if the claim exists, `false` if absent.
**Return Value**: The `iat` Unix timestamp, or 0 if absent.
**Usage Case**: Use to read the issuance time for age-based policies or audit logging.

---

### `const char* jwt_get_claim_string(const JwtToken* t, const char* name)`
**Purpose**: Returns the value of a custom string claim by name.
**Parameters**:
- `t`: The token to query.
- `name`: The claim name to look up.
**Return Value**: The string value, or `NULL` if absent or wrong type.
**Usage Case**: Use to read application-specific string claims such as roles or tenant identifiers.

---

### `bool jwt_get_claim_int(const JwtToken* t, const char* name, int64_t* out)`
**Purpose**: Writes the integer value of a custom claim to `*out`.
**Parameters**:
- `t`: The token to query.
- `name`: The claim name to look up.
- `out`: Pointer to receive the integer value.
**Return Value**: `true` on success, `false` if absent or wrong type.
**Usage Case**: Use to read numeric custom claims such as user IDs or permission bitmasks.

---

### `bool jwt_get_claim_double(const JwtToken* t, const char* name, double* out)`
**Purpose**: Writes the double value of a custom claim to `*out`.
**Parameters**:
- `t`: The token to query.
- `name`: The claim name to look up.
- `out`: Pointer to receive the double value.
**Return Value**: `true` on success, `false` if absent or wrong type.
**Usage Case**: Use to read fractional numeric custom claims from the payload.

---

### `bool jwt_get_claim_bool(const JwtToken* t, const char* name, bool* out)`
**Purpose**: Writes the boolean value of a custom claim to `*out`.
**Parameters**:
- `t`: The token to query.
- `name`: The claim name to look up.
- `out`: Pointer to receive the boolean value.
**Return Value**: `true` on success, `false` if absent or wrong type.
**Usage Case**: Use to read boolean flag claims such as feature toggles or consent bits.

---

### `const JsonElement* jwt_get_header(const JwtToken* t)`
**Purpose**: Returns a read-only pointer to the parsed JOSE header JSON tree owned by the token.
**Parameters**:
- `t`: The token to query.
**Return Value**: A read-only `const JsonElement*` owned by the token.
**Usage Case**: Use to walk arbitrary header parameters not covered by the named accessors.

---

### `const JsonElement* jwt_get_payload(const JwtToken* t)`
**Purpose**: Returns a read-only pointer to the parsed payload JSON tree owned by the token.
**Parameters**:
- `t`: The token to query.
**Return Value**: A read-only `const JsonElement*` owned by the token.
**Usage Case**: Use to walk arbitrary claims not covered by the named accessors.

---

### `const char* jwt_get_header_string(const JwtToken* t, const char* name)`
**Purpose**: Returns the value of a string header parameter.
**Parameters**:
- `t`: The token to query.
- `name`: The header parameter name to look up.
**Return Value**: The string value, or `NULL` if absent.
**Usage Case**: Commonly used to retrieve `kid` before key selection in a rotation scenario.

---

**Lifecycle and introspection**

### `void jwt_deallocate(JwtToken* t)`
**Purpose**: Frees the header, payload, signature (wiped with `OPENSSL_cleanse`), and signing input.
**Parameters**:
- `t`: The token to free; `NULL` is a safe no-op.
**Return Value**: None.
**Usage Case**: Call when a token is no longer needed to securely erase signature material and release memory.

---

### `const char* jwt_status_to_string(JwtStatus s)`
**Purpose**: Returns a human-readable status name such as `"JWT_SUCCESS"` or `"JWT_ERR_EXPIRED"`.
**Parameters**:
- `s`: The status value to convert.
**Return Value**: A static string; never `NULL`.
**Usage Case**: Use for logging, diagnostics, or error messages when a JWT operation fails.

---

### `const char* jwt_alg_to_string(JwtAlgorithm a)`
**Purpose**: Returns the RFC 7518 wire-format algorithm name such as `"HS256"`, `"RS256"`, or `"none"`.
**Parameters**:
- `a`: The algorithm enum value to convert.
**Return Value**: A static string; never `NULL`.
**Usage Case**: Use when serializing the algorithm name for logging or custom header construction.

---

### `JwtAlgorithm jwt_alg_from_string(const char* s)`
**Purpose**: Parses the RFC 7518 algorithm name and returns the corresponding enum value.
**Parameters**:
- `s`: Null-terminated RFC 7518 algorithm name string.
**Return Value**: The corresponding `JwtAlgorithm` enum value, or `JWT_ALG_NONE` for unknown strings.
**Usage Case**: Use when deserializing an algorithm name from external configuration or a JWKS `alg` field.

---

### `bool jwt_alg_is_hmac(JwtAlgorithm a)`
**Purpose**: Returns `true` if `a` is HS256, HS384, or HS512.
**Parameters**:
- `a`: The algorithm enum value to test.
**Return Value**: `true` if `a` is an HMAC algorithm, `false` otherwise.
**Usage Case**: Use to decide whether to call `jwt_key_hmac` when constructing a key for a given algorithm.

---

### `bool jwt_alg_is_rsa(JwtAlgorithm a)`
**Purpose**: Returns `true` if `a` is RS256, RS384, or RS512 (RSA-PKCS#1-v1.5 only; excludes PSS).
**Parameters**:
- `a`: The algorithm enum value to test.
**Return Value**: `true` if `a` is an RSA-PKCS#1-v1.5 algorithm, `false` otherwise.
**Usage Case**: Use to distinguish classic RSA signing from PSS variants in algorithm dispatch logic.

---

### `bool jwt_alg_is_rsa_pss(JwtAlgorithm a)`
**Purpose**: Returns `true` if `a` is PS256, PS384, or PS512.
**Parameters**:
- `a`: The algorithm enum value to test.
**Return Value**: `true` if `a` is an RSASSA-PSS algorithm, `false` otherwise.
**Usage Case**: Use to identify PSS-padded RSA algorithms required by federal or banking profiles.

---

### `bool jwt_alg_is_ec(JwtAlgorithm a)`
**Purpose**: Returns `true` if `a` is ES256, ES384, or ES512.
**Parameters**:
- `a`: The algorithm enum value to test.
**Return Value**: `true` if `a` is an ECDSA algorithm, `false` otherwise.
**Usage Case**: Use to decide whether to call `jwt_key_ec_*` constructors when building a key for a given algorithm.

---

### `bool jwt_alg_uses_rsa_key(JwtAlgorithm a)`
**Purpose**: Returns `true` if `a` uses an RSA key shape (RS* or PS*).
**Parameters**:
- `a`: The algorithm enum value to test.
**Return Value**: `true` if `a` is any RSA-family algorithm (both PKCS#1-v1.5 and PSS), `false` otherwise.
**Usage Case**: Use to decide which key constructor to call without enumerating both RS* and PS* families separately.

---

## Example 1 — HS256 sign + verify

The simplest possible flow: a service issues a token signed with a shared secret, then verifies it on the next request.

```c
#include "jwt/jwt.h"
#include "fmt/fmt.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>

int main(void) {
    const unsigned char secret[] = "shhhhhh-keep-this-server-side-only";
    JwtKey* key = jwt_key_hmac(secret, sizeof(secret) - 1);

    /* Build. */
    JwtToken* t = jwt_create(JWT_ALG_HS256);
    jwt_set_issuer(t, "auth.example.com");
    jwt_set_subject(t, "user-42");
    jwt_set_expiration(t, (int64_t)time(NULL) + 3600);   /* 1 hour */

    char* token = jwt_encode(t, key);
    jwt_deallocate(t);

    fmt_printf("%s\n", token);

    /* Verify. */
    JwtAlgorithm allowed[] = { JWT_ALG_HS256 };
    JwtValidationOptions opts = {0};
    opts.allowed_algs       = allowed;
    opts.allowed_algs_count = 1;
    opts.require_exp        = true;
    opts.leeway_seconds     = 30;

    JwtToken* parsed = NULL;
    JwtStatus rc = jwt_decode_and_verify(token, key, &opts, &parsed);
    fmt_printf("verify: %s\n", jwt_status_to_string(rc));
    if (rc == JWT_SUCCESS) {
        fmt_printf("subject: %s\n", jwt_get_subject(parsed));
        jwt_deallocate(parsed);
    }

    free(token);
    jwt_key_deallocate(key);
    
    return 0;
}
```

**Output:**
```
eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJhdXRoLmV4YW1wbGUuY29tIiwic3ViIjoidXNlci00MiIsImV4cCI6MTc4MDM4ODE5MX0.bH6t...
verify: JWT_SUCCESS
subject: user-42
```

---

## Example 2 — RS256 (sign with private, verify with public)

Real-world deployments separate signing and verification. The auth server holds the private key; every other service only has the public key.

```c
#include "jwt/jwt.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    /* Load keys from disk. Generate a pair beforehand with:
       openssl genpkey -algorithm RSA -pkeyopt rsa_keygen_bits:2048 -out priv.pem
       openssl rsa -in priv.pem -pubout -out pub.pem                                */
    JwtKey* priv = jwt_key_rsa_private_file("priv.pem");
    JwtKey* pub  = jwt_key_rsa_public_file ("pub.pem");

    JwtToken* t = jwt_create(JWT_ALG_RS256);
    jwt_set_subject(t, "alice");
    jwt_set_claim_string(t, "role", "admin");
    char* token = jwt_encode(t, priv);
    jwt_deallocate(t);

    JwtAlgorithm allowed[] = { JWT_ALG_RS256 };
    JwtValidationOptions opts = {0};
    opts.allowed_algs       = allowed;
    opts.allowed_algs_count = 1;

    JwtToken* parsed = NULL;
    if (jwt_decode_and_verify(token, pub, &opts, &parsed) == JWT_SUCCESS) {
        fmt_printf("role: %s\n", jwt_get_claim_string(parsed, "role"));
        jwt_deallocate(parsed);
    }

    free(token);
    jwt_key_deallocate(priv);
    jwt_key_deallocate(pub);

    return 0;
}
```

**Output:**
```
role: admin
```

---

## Example 3 — Custom claims (string, int, bool, nested object)

JWT payloads can carry arbitrary JSON. Below, a single token carries a user ID, role flags, and a nested permissions object.

```c
#include "jwt/jwt.h"
#include "json/json.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    JwtKey* k = jwt_key_hmac((const unsigned char*)"k", 1);

    JwtToken* t = jwt_create(JWT_ALG_HS256);
    jwt_set_claim_string(t, "name", "Alice");
    jwt_set_claim_int   (t, "id", 12345);
    jwt_set_claim_bool  (t, "verified", true);

    JsonElement* perms = json_create(JSON_OBJECT);
    JsonElement* r = json_create(JSON_BOOL); r->value.bool_val = true;
    JsonElement* w = json_create(JSON_BOOL); w->value.bool_val = false;
    json_add_to_object(perms, "read",  r);
    json_add_to_object(perms, "write", w);
    jwt_set_claim_json(t, "perms", perms);  /* token takes ownership */

    char* token = jwt_encode(t, k);
    jwt_deallocate(t);

    JwtAlgorithm allowed[] = { JWT_ALG_HS256 };
    JwtValidationOptions opts = {0};
    opts.allowed_algs = allowed; opts.allowed_algs_count = 1;

    JwtToken* parsed = NULL;
    jwt_decode_and_verify(token, k, &opts, &parsed);
    fmt_printf("name=%s id=", jwt_get_claim_string(parsed, "name"));
    int64_t id = 0; jwt_get_claim_int(parsed, "id", &id);
    fmt_printf("%lld\n", (long long)id);

    jwt_deallocate(parsed);
    free(token);
    jwt_key_deallocate(k);

    return 0;
}
```

**Output:**
```
name=Alice id=12345
```

---

## Example 4 — Validation with `exp`, `iss`, `aud`, and leeway

Real APIs reject tokens that are expired, not-yet-valid, signed by the wrong issuer, or addressed to the wrong audience. The library makes all of these declarative.

```c
#include "jwt/jwt.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    JwtKey* k = jwt_key_hmac((const unsigned char*)"k", 1);
    int64_t now = 1700000000;   /* fixed time for the test */

    /* Issue a token that expired 60 seconds ago. */
    JwtToken* t = jwt_create(JWT_ALG_HS256);
    jwt_set_issuer (t, "auth.example.com");
    jwt_set_audience(t, "billing-service");
    jwt_set_expiration(t, now - 60);
    char* token = jwt_encode(t, k);
    jwt_deallocate(t);

    JwtAlgorithm allowed[] = { JWT_ALG_HS256 };
    JwtValidationOptions opts = {0};
    opts.allowed_algs       = allowed;
    opts.allowed_algs_count = 1;
    opts.expected_iss       = "auth.example.com";
    const char* auds[]      = { "billing-service" };
    opts.expected_aud       = auds; opts.expected_aud_count = 1;
    opts.now_unix           = now;
    opts.require_exp        = true;

    JwtToken* parsed = NULL;
    JwtStatus rc = jwt_decode_and_verify(token, k, &opts, &parsed);
    fmt_printf("strict: %s\n", jwt_status_to_string(rc));   /* JWT_ERR_EXPIRED */

    /* Loosen with 2-minute leeway — now it passes. */
    opts.leeway_seconds = 120;
    rc = jwt_decode_and_verify(token, k, &opts, &parsed);
    fmt_printf("with leeway: %s\n", jwt_status_to_string(rc));   /* JWT_SUCCESS */
    if (parsed) {
        jwt_deallocate(parsed);
    }

    free(token);
    jwt_key_deallocate(k);

    return 0;
}
```

**Output:**
```
strict: JWT_ERR_EXPIRED
with leeway: JWT_SUCCESS
```

---

## Example 5 — Defending against algorithm confusion

A common attack: the service uses RS256 (public-key crypto), so the attacker reads the public key, forges a token claiming `alg=HS256`, and signs it using the public-key bytes as the HMAC secret. If the service blindly hands that key to its verifier, the forged token verifies. This library refuses the attack at two layers.

```c
#include "jwt/jwt.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    /* Server's RSA verification key (loaded from disk, etc.). */
    JwtKey* pub = jwt_key_rsa_public_file("pub.pem");

    /* Lock the verifier to RS256 only — never list both HS256 and RS256
       for the same endpoint, since the alg name comes from the token. */
    JwtAlgorithm allowed[] = { JWT_ALG_RS256 };
    JwtValidationOptions opts = {0};
    opts.allowed_algs = allowed; opts.allowed_algs_count = 1;

    /* Attacker-crafted token claiming HS256: */
    const char* forged =
        "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJzdWIiOiJoYWNrZXIifQ."
        "fake-signature-bytes";

    JwtToken* out = NULL;
    JwtStatus rc = jwt_decode_and_verify(forged, pub, &opts, &out);
    fmt_printf("forged HS256 token: %s\n", jwt_status_to_string(rc));
    /* JWT_ERR_INVALID_ALG — HS256 isn't in our allow-list. */

    /* Even if the allow-list included HS256 (e.g. for a dual-alg
       transition), the library still catches the attack at the
       key-shape check: HS256 token + RSA public key = JWT_ERR_INVALID_KEY. */
    JwtAlgorithm dual[] = { JWT_ALG_RS256, JWT_ALG_HS256 };
    opts.allowed_algs = dual; opts.allowed_algs_count = 2;
    rc = jwt_decode_and_verify(forged, pub, &opts, &out);
    fmt_printf("dual allow-list: %s\n", jwt_status_to_string(rc));
    /* JWT_ERR_INVALID_KEY — HMAC alg requires HMAC key. */

    jwt_key_deallocate(pub);
    return 0;
}
```

**Output:**
```
forged HS256 token: JWT_ERR_INVALID_ALG
dual allow-list: JWT_ERR_INVALID_KEY
```

---

## Example 6 — Key rotation via the `kid` header

When you rotate signing keys, every issued token carries a `kid` header identifying which key signed it. The verifier reads `kid` without trusting the token, looks up the corresponding public key, then runs the real verification.

```c
#include "jwt/jwt.h"
#include "fmt/fmt.h"
#include <string.h>
#include <stdlib.h>

/* Trivial keyring — in production this is your KMS / Vault / file map. */
typedef struct { const char* kid; JwtKey* key; } KidEntry;

int main(void) {
    /* Pretend we have two HMAC keys. */
    JwtKey* k_2023 = jwt_key_hmac((const unsigned char*)"old-secret", 10);
    JwtKey* k_2024 = jwt_key_hmac((const unsigned char*)"new-secret", 10);
    KidEntry ring[] = { {"key-2023", k_2023}, {"key-2024", k_2024} };

    /* Mint a token with kid=key-2024. */
    JwtToken* t = jwt_create(JWT_ALG_HS256);
    jwt_set_header_string(t, "kid", "key-2024");
    jwt_set_subject(t, "alice");
    char* token = jwt_encode(t, k_2024);
    jwt_deallocate(t);

    /* Verifier: peek at kid WITHOUT trusting the token, pick the key. */
    JwtToken* peeked = jwt_decode(token);
    const char* kid = jwt_get_header_string(peeked, "kid");
    fmt_printf("token claims kid=%s\n", kid);

    JwtKey* chosen = NULL;
    for (size_t i = 0; i < sizeof(ring)/sizeof(ring[0]); ++i) {
        if (strcmp(ring[i].kid, kid) == 0) { chosen = ring[i].key; break; }
    }
    jwt_deallocate(peeked);

    /* Now do the real verification with the chosen key. */
    JwtAlgorithm allowed[] = { JWT_ALG_HS256 };
    JwtValidationOptions opts = {0};
    opts.allowed_algs = allowed; opts.allowed_algs_count = 1;

    JwtToken* parsed = NULL;
    JwtStatus rc = jwt_decode_and_verify(token, chosen, &opts, &parsed);
    fmt_printf("verify: %s\n", jwt_status_to_string(rc));

    if (parsed) {
        jwt_deallocate(parsed);
    }

    free(token);
    jwt_key_deallocate(k_2023);
    jwt_key_deallocate(k_2024);

    return 0;
}
```

**Output:**
```
token claims kid=key-2024
verify: JWT_SUCCESS
```

---

## Example 7 — ES256 (ECDSA on P-256)

ES256 produces compact 64-byte signatures (vs. 256 for RS256 at the same security level) and is what most modern OIDC providers — Apple Sign-in, Cloudflare Access, Auth0 — use by default.

Generate a P-256 keypair beforehand:

```sh
openssl genpkey -algorithm EC -pkeyopt ec_paramgen_curve:P-256 -out ec_priv.pem
openssl ec -in ec_priv.pem -pubout -out ec_pub.pem
```

```c
#include "jwt/jwt.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <time.h>

int main(void) {
    JwtKey* priv = jwt_key_ec_private_file("ec_priv.pem");
    JwtKey* pub  = jwt_key_ec_public_file ("ec_pub.pem");

    JwtToken* t = jwt_create(JWT_ALG_ES256);
    jwt_set_issuer    (t, "https://accounts.example.com");
    jwt_set_subject   (t, "user-42");
    jwt_set_audience  (t, "billing-api");
    jwt_set_issued_at (t, (int64_t)time(NULL));
    jwt_set_expiration(t, (int64_t)time(NULL) + 3600);
    char* token = jwt_encode(t, priv);
    jwt_deallocate(t);

    fmt_printf("%s\n", token);

    JwtAlgorithm allowed[] = { JWT_ALG_ES256 };
    JwtValidationOptions opts = {0};
    opts.allowed_algs       = allowed;
    opts.allowed_algs_count = 1;
    opts.expected_iss       = "https://accounts.example.com";
    const char* auds[]      = { "billing-api" };
    opts.expected_aud       = auds; opts.expected_aud_count = 1;
    opts.require_exp        = true;

    JwtToken* parsed = NULL;
    JwtStatus rc = jwt_decode_and_verify(token, pub, &opts, &parsed);

    fmt_printf("verify: %s\n", jwt_status_to_string(rc));
    if (rc == JWT_SUCCESS) {
        fmt_printf("subject: %s\n", jwt_get_subject(parsed));
        jwt_deallocate(parsed);
    }

    free(token);
    jwt_key_deallocate(priv);
    jwt_key_deallocate(pub);

    return 0;
}
```

**Output:**
```
eyJ0eXAiOiJKV1QiLCJhbGciOiJFUzI1NiJ9.eyJpc3MiOiJodHRwczovL2FjY291bnRzLmV4YW1wbGUuY29tIiwic3ViIjoidXNlci00MiIsImF1ZCI6ImJpbGxpbmctYXBpIiwiaWF0IjoxNzgwMzg4MTkxLCJleHAiOjE3ODAzOTE3OTF9.HZ3a...
verify: JWT_SUCCESS
subject: user-42
```

> The signature segment for ES256 is always exactly **86 base64url characters** (64 raw bytes). ES384 → 128 chars (96 bytes), ES512 → 176 chars (132 bytes — note ES512 uses P-521, not P-512). Anything else for an ES* token is malformed and rejected before OpenSSL is even called.

---

## Example 8 — Hardened verify (RFC 9068 access tokens)

Every option turned on. This is what a production resource-server endpoint should look like: pinned algorithm, pinned issuer, pinned audience, pinned `typ`, weak-key rejection, and a size cap to keep bad actors from feeding multi-MB tokens to your parser.

```c
#include "jwt/jwt.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    JwtKey* pub = jwt_key_ec_public_file("idp_es256_pub.pem");

    JwtAlgorithm allowed[] = { JWT_ALG_ES256 };
    const char*  auds[]    = { "https://billing.example.com" };

    JwtValidationOptions opts = {0};
    opts.allowed_algs        = allowed;
    opts.allowed_algs_count  = 1;
    opts.expected_iss        = "https://idp.example.com";
    opts.expected_aud        = auds;
    opts.expected_aud_count  = 1;
    opts.expected_typ        = "at+jwt";     /* RFC 9068 */
    opts.require_exp         = true;
    opts.require_iat         = true;
    opts.leeway_seconds      = 30;
    opts.max_token_bytes     = 4096;         /* DoS guard */
    opts.reject_weak_hmac_key = true;        /* belt-and-suspenders even on ES256 */

    /* The token comes from the Authorization: Bearer ... header. */
    const char* token = getenv("ACCESS_TOKEN");
    if (!token) { fmt_printf("no token\n"); return 1; }

    JwtToken* parsed = NULL;
    JwtStatus rc = jwt_decode_and_verify(token, pub, &opts, &parsed);
    switch (rc) {
        case JWT_SUCCESS:
            fmt_printf("OK: sub=%s\n", jwt_get_subject(parsed));
            jwt_deallocate(parsed);
            break;
        case JWT_ERR_TOKEN_TOO_LARGE:
            fmt_printf("rejected: token too large (DoS guard tripped)\n");
            break;
        case JWT_ERR_INVALID_TYP:
            fmt_printf("rejected: header typ is not at+jwt — wrong token kind\n");
            break;
        case JWT_ERR_CRIT_UNSUPPORTED:
            fmt_printf("rejected: header lists unsupported critical extensions\n");
            break;
        case JWT_ERR_EXPIRED:
            fmt_printf("rejected: expired\n");
            break;
        default:
            fmt_printf("rejected: %s\n", jwt_status_to_string(rc));
    }

    jwt_key_deallocate(pub);

    return rc == JWT_SUCCESS ? 0 : 1;
}
```

**Output (for a forged token that drops a `crit` extension hoping the verifier ignores it):**
```
rejected: header lists unsupported critical extensions
```

---

## Example 9 — PS256 (RSASSA-PSS)

Functionally identical from the caller's perspective to RS256 — same key constructors, same PEM files, same EVP_PKEY object — but with PSS padding under the hood. Required by some federal and banking profiles; produced by Microsoft Entra ID for some flows.

Use the same RSA keypair you'd use for RS256:

```sh
openssl genpkey -algorithm RSA -pkeyopt rsa_keygen_bits:2048 -out priv.pem
openssl rsa -in priv.pem -pubout -out pub.pem
```

```c
#include "jwt/jwt.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    JwtKey* priv = jwt_key_rsa_private_file("priv.pem");
    JwtKey* pub  = jwt_key_rsa_public_file ("pub.pem");

    JwtToken* t = jwt_create(JWT_ALG_PS256);   /* only this line changes vs. RS256 */
    jwt_set_subject(t, "alice");
    char* token = jwt_encode(t, priv);
    jwt_deallocate(t);

    JwtAlgorithm allowed[] = { JWT_ALG_PS256 };
    JwtValidationOptions opts = {0};
    opts.allowed_algs       = allowed;
    opts.allowed_algs_count = 1;

    JwtToken* parsed = NULL;
    if (jwt_decode_and_verify(token, pub, &opts, &parsed) == JWT_SUCCESS) {
        fmt_printf("verified: sub=%s\n", jwt_get_subject(parsed));
        jwt_deallocate(parsed);
    }

    free(token);
    jwt_key_deallocate(priv);
    jwt_key_deallocate(pub);
    return 0;
}
```

**Output:**
```
verified: sub=alice
```

> PS\* signatures are **randomized** — two encodes of the same payload produce different signatures (unlike RS\*, HS\*, and ES\* in this implementation, which are deterministic). The verifier handles both directions. Don't rely on byte-identical tokens.

> The allow-list distinguishes PS from RS. A token signed with PS256 and verified against an allow-list of `{JWT_ALG_RS256}` returns `JWT_ERR_INVALID_ALG` — even though both algorithms consume the same RSA key, they're separate JOSE values and must not be mixed.

---

## Example 10 — JWKS verify (the OIDC pattern)

Standard OIDC providers (Auth0, Cognito, Keycloak, Azure AD, Google, Apple, ...) publish their public verification keys at a `/.well-known/jwks.json` URL. Fetch it once, cache it, hand it to the verifier — the right key is picked by `kid` automatically.

```c
#include "jwt/jwt.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <time.h>

/* JWKS with two HMAC (kty=oct) keys identified by kid.
 * In production you'd fetch this JSON from the IdP's /.well-known/jwks.json.
 * The `k` values are base64url-encoded raw secret bytes:
 *   key-2024-01: "supersecretkeythatis32byteslong!" (32 bytes)
 *   key-2024-02: "secondkey-that-is-also-32-bytes" (31 bytes, padded)   */
static const char* JWKS_JSON =
    "{\"keys\":["
      "{\"kty\":\"oct\",\"use\":\"sig\",\"alg\":\"HS256\","
       "\"kid\":\"key-2024-01\","
       "\"k\":\"c3VwZXJzZWNyZXRrZXl0aGF0aXMzMmJ5dGVzbG9uZyE\"},"
      "{\"kty\":\"oct\",\"use\":\"sig\",\"alg\":\"HS256\","
       "\"kid\":\"key-2024-02\","
       "\"k\":\"c2Vjb25ka2V5LXRoYXQtaXMtYWxzby0zMi1ieXRlcw\"}"
    "]}";

int main(void) {
    JwtKeySet* set = jwt_keyset_from_json(JWKS_JSON);

    if (!set) {
        fmt_printf("jwks parse failed\n");
        return 1;
    }

    fmt_printf("loaded %zu key(s):\n", jwt_keyset_size(set));
    for (size_t i = 0; i < jwt_keyset_size(set); ++i) {
        fmt_printf("  [%zu] kid=%s alg=%s\n", i, jwt_keyset_kid_at(set, i), jwt_keyset_alg_at(set, i));
    }

    /* Issue a token signed with key-2024-01 and tagged with that kid. */
    const unsigned char sec[] = "supersecretkeythatis32byteslong!";
    JwtKey* signing_key = jwt_key_hmac(sec, sizeof(sec) - 1);
    JwtToken* t = jwt_create(JWT_ALG_HS256);
    jwt_set_header_string(t, "kid", "key-2024-01");
    jwt_set_subject   (t, "alice@example.com");
    jwt_set_issuer    (t, "https://idp.example.com");
    jwt_set_expiration(t, (int64_t)time(NULL) + 3600);
    char* token = jwt_encode(t, signing_key);
    jwt_deallocate(t);

    /* Pinned allow-list at this endpoint. */
    JwtAlgorithm allowed[] = { JWT_ALG_HS256 };
    JwtValidationOptions opts = {0};
    opts.allowed_algs       = allowed;
    opts.allowed_algs_count = 1;
    opts.expected_iss       = "https://idp.example.com";
    opts.require_exp        = true;
    opts.max_token_bytes    = 4096;

    JwtToken* parsed = NULL;
    JwtStatus rc = jwt_decode_and_verify_with_keyset(token, set, &opts, &parsed);
    fmt_printf("verify: %s\n", jwt_status_to_string(rc));

    if (rc == JWT_SUCCESS) {
        fmt_printf("sub: %s\n", jwt_get_subject(parsed));
        jwt_deallocate(parsed);
    }

    free(token);
    jwt_key_deallocate(signing_key);
    jwt_keyset_deallocate(set);

    return 0;
}
```

**Output:**
```
loaded 2 key(s):
  [0] kid=key-2024-01 alg=HS256
  [1] kid=key-2024-02 alg=HS256
verify: JWT_SUCCESS
sub: alice@example.com
```

The keyset path automatically uses `kid` to pick the right key on the first try, then falls back to trying every shape-compatible key if `kid` is missing or unknown — useful for half-broken IdPs that publish keys without `kid`.

---

## Example 11 — Multi-key verify for rotation

When you rotate signing keys, there's an overlap window where both the old and new public keys must verify incoming tokens (the old key has to keep working until every cached token issued under it has expired). `jwt_decode_and_verify_with_keys` makes that one call:

```c
#include "jwt/jwt.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    /* The currently-deployed verification keys, in rotation order. */
    JwtKey* k_new = jwt_key_rsa_public_file("verify_2024-04.pem");
    JwtKey* k_old = jwt_key_rsa_public_file("verify_2024-01.pem");

    const JwtKey* keys[] = { k_new, k_old };
    JwtAlgorithm allowed[] = { JWT_ALG_RS256 };
    JwtValidationOptions opts = {0};
    opts.allowed_algs       = allowed;
    opts.allowed_algs_count = 1;
    opts.require_exp        = true;

    const char* token = "<incoming bearer token>";
    JwtToken* parsed = NULL;
    JwtStatus rc = jwt_decode_and_verify_with_keys(token, keys, 2, &opts, &parsed);

    fmt_printf("verify: %s\n", jwt_status_to_string(rc));
    if (rc == JWT_SUCCESS) {
        fmt_printf("sub: %s\n", jwt_get_subject(parsed));
        jwt_deallocate(parsed);
    }

    jwt_key_deallocate(k_new);
    jwt_key_deallocate(k_old);

    return 0;
}
```

The first key in the list is tried first. Key-independent failures (expiry, wrong issuer, `crit` extension, oversize token) short-circuit on the first attempt — the function won't keep trying keys for a token that's never going to verify regardless. The list may freely mix RSA, EC, and HMAC keys; shape-incompatible keys are silently skipped, so you can pass your whole keyring without curating it per endpoint.

**Output (token was signed under the rotated-out key):**
```
verify: JWT_SUCCESS
sub: alice
```

## License

This project is open-source and available under the ISC License.
