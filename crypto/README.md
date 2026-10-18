# Crypto Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License  

## Overview

The Crypto Library in C is a comprehensive cryptographic library designed for developers who require secure and efficient hashing and encryption capabilities in their C programs. It provides a straightforward API for a variety of cryptographic operations, including hashing with multiple algorithms and encryption/decryption using DES in various modes. The library is built on top of the OpenSSL library, ensuring robust and widely trusted cryptographic functionality. It also contains fast non-cryptographic hashes and checksums (FNV-1a, MurmurHash3, xxHash, CRC-32/CRC-32C), written in portable C, for hash tables, fingerprints and integrity checks.

This library is particularly useful for developers who are familiar with cryptographic libraries in higher-level languages like C++ or Python and wish to leverage similar capabilities in C.

## Key Features

- **Multiple Hashing Algorithms:** Supports a wide range of hashing algorithms, including MD4, MD5, SHA-1, SHA-2 (SHA-224, SHA-256, SHA-384, SHA-512), SHA-3, SHAKE variants, BLAKE2, MDC2, and RIPEMD-160.
- **Fast Non-Cryptographic Hashes:** FNV-1a (32/64), MurmurHash3 (32/128), xxHash (XXH32, XXH64, XXH3) and CRC-32 / CRC-32C, bit-identical to the reference implementations, with SSE2/AVX2/SSE4.2 acceleration where the build allows it. See [Non-Cryptographic Hashes and Checksums](#non-cryptographic-hashes-and-checksums-14).
- **DES Encryption/Decryption:** Provides DES encryption and decryption in various modes (ECB, CBC, CFB, OFB), allowing for flexibility depending on the security needs.
- **Cross-Platform Support:** The library can be compiled and used on both Windows and Unix-like systems, with platform-specific random number generation.
- **OpenSSL Integration:** Utilizes the OpenSSL library for cryptographic operations, ensuring reliability and security.
- **Memory Management:** Manages memory dynamically to accommodate varying sizes of data and cryptographic outputs.

## Installation and Compilation

### Prerequisites

- **OpenSSL:** Ensure that OpenSSL is installed on your system. This library depends on OpenSSL for cryptographic functions.
- **GCC:** A C compiler such as GCC is required to compile the library and your C programs.

### Compilation

To compile the Crypto library along with your C program, use a command similar to the following:

```bash
gcc -std=c11 -O3 -Wall -Wextra -o mycrypto main.c crypto.c -lcrypto
```

This command compiles `main.c` and `crypto.c` and links the OpenSSL library (`-lcrypto`). Ensure that OpenSSL is properly installed and accessible in your system's library path.

## Usage

### Hashing Operations

The library provides a variety of hashing algorithms that can be used to generate cryptographic hash digests from data. The primary function for this is `crypto_hash_data`, which computes a hash digest based on the selected algorithm.

### Supported Algorithms for Hashing

- `CRYPTO_MD4`: Computes an MD4 hash digest.
- `CRYPTO_MD5`: Computes an MD5 hash digest.
- `CRYPTO_SHA1`: Computes an SHA-1 hash digest.
- `CRYPTO_SHA224`: Computes an SHA-224 hash digest.
- `CRYPTO_SHA256`: Computes an SHA-256 hash digest.
- `CRYPTO_SHA384`: Computes an SHA-384 hash digest.
- `CRYPTO_SHA512`: Computes an SHA-512 hash digest.
- `CRYPTO_SHA3_224`: Computes an SHA3-224 hash digest.
- `CRYPTO_SHA3_256`: Computes an SHA3-256 hash digest.
- `CRYPTO_SHA3_384`: Computes an SHA3-384 hash digest.
- `CRYPTO_SHA3_512`: Computes an SHA3-512 hash digest.
- `CRYPTO_SHAKE_128`: Computes a SHAKE-128 hash digest.
- `CRYPTO_SHAKE_256`: Computes a SHAKE-256 hash digest.
- `CRYPTO_BLAKE2B_512`: Computes a BLAKE2b-512 hash digest.
- `CRYPTO_BLAKE2S_256`: Computes a BLAKE2s-256 hash digest.
- `CRYPTO_MDC2`: Computes an MDC2 hash digest.
- `CRYPTO_RIPEMD_160`: Computes an RIPEMD-160 hash digest.
- `CRYPTO_SHA512_224`: Computes an SHA-512/224 hash digest.

### DES Encryption/Decryption

The library supports DES encryption and decryption in several modes: ECB, CBC, CFB, and OFB. The functions `crypto_des_encrypt` and `crypto_des_decrypt` handle encryption and decryption, respectively.

### Supported Modes for DES

- `CRYPTO_MODE_ECB`: Electronic Codebook mode.
- `CRYPTO_MODE_CBC`: Cipher Block Chaining mode.
- `CRYPTO_MODE_CFB`: Cipher Feedback mode.
- `CRYPTO_MODE_OFB`: Output Feedback mode.

### Function Descriptions 

### `uint8_t* crypto_hash_data(const uint8_t* data, size_t length, HashAlgorithm algorithm, size_t *outLength)`

**Purpose**: This function computes a cryptographic hash of the provided data using a specified cryptographic algorithm (e.g., MD5, SHA-256, SHA3-512). The result is returned as a dynamically allocated array of bytes, with the length of the hash stored in the `outLength` parameter.

**Parameters**:
  - `data`: A pointer to the input data that needs to be hashed.
  - `length`: The length (in bytes) of the input data.
  - `algorithm`: Specifies which cryptographic hash algorithm to use (e.g., `CRYPTO_MD5`, `CRYPTO_SHA256`).
  - `outLength`: A pointer to a variable where the length of the computed hash will be stored.

**Return Value**: A pointer to the computed hash (as a byte array), or `NULL` if an error occurs (e.g., memory allocation failure or unsupported algorithm). The caller is responsible for freeing the memory allocated for the hash.

**Usage Case**: Use as the core primitive for all hashing needs. The function supports multiple cryptographic algorithms, including MD5, SHA-256, SHA3, SHAKE, Blake2, RIPEMD, and others. Depending on the selected algorithm, it either directly calls lower-level hashing functions (like `MD5`, `SHA256`) or uses OpenSSL's `EVP` interfaces for more complex algorithms like SHA3 and Blake2. The function performs error checking, ensuring the `outLength` pointer is valid and that memory allocation succeeds. If any error occurs during the hashing process, the function logs the error, cleans up any allocated resources, and returns `NULL`.

---

### `void crypto_print_hash(const uint8_t* hash, size_t length)`
**Purpose**: This function prints cryptographic hash data as a hexadecimal string.

**Parameters**:
  - `hash`: A pointer to the byte array containing the hash data.
  - `length`: The number of bytes in the hash data.

**Return Value**: None.

**Usage Case**: Use to quickly dump a digest to stdout during development or debugging. The function converts each byte of the hash into two hexadecimal characters and prints the result. If the `hash` parameter is `NULL`, it logs an error message and prints "No hash data to print."

### `void* crypto_des_encrypt(const uint8_t* plaintext, size_t len, const uint8_t* key, const uint8_t* iv, CryptoMode mode, size_t* out_len)`
**Purpose**: Encrypts the given plaintext using the DES (Data Encryption Standard) algorithm in various modes (ECB, CBC, CFB, OFB).

**Parameters**:
  - `plaintext`: A pointer to the plaintext data that needs to be encrypted.
  - `len`: The length of the plaintext data in bytes.
  - `key`: A pointer to the encryption key (must be 8 bytes for DES).
  - `iv`: A pointer to the initialization vector (IV) for modes that require it (CBC, CFB, OFB). If `NULL`, an IV of all zeros is used.
  - `mode`: The mode of operation for DES (e.g., `CRYPTO_MODE_ECB`, `CRYPTO_MODE_CBC`).
  - `out_len`: A pointer to a variable that will store the length of the encrypted data.

**Return Value**: A pointer to the encrypted data (ciphertext) or `NULL` if an error occurs. The caller is responsible for freeing the memory allocated for the ciphertext.

**Usage Case**: Use to encrypt data with DES in one of four modes (ECB, CBC, CFB, OFB). In ECB and CBC the plaintext gets PKCS#7 padding (1 to 8 bytes, so the ciphertext is always longer than the plaintext and a multiple of 8); CFB and OFB are stream modes and the ciphertext has the plaintext's length. (Older versions zero-padded every mode, so decryption could not recover the original length or trailing zero bytes; such old ECB/CBC ciphertexts are rejected by the padding check of the new `crypto_des_decrypt`.) Error handling ensures that null inputs for critical parameters (e.g., `plaintext`, `key`, `out_len`) lead to early returns with error messages logged. Memory is allocated for the padded plaintext and ciphertext, which must be freed by the caller.

---

### `void* crypto_des_decrypt(const uint8_t* ciphertext, size_t len, const uint8_t* key, const uint8_t* iv, CryptoMode mode, size_t* out_len)`
**Purpose**: Decrypts the provided ciphertext using the DES (Data Encryption Standard) algorithm in various modes (ECB, CBC, CFB, or OFB).

**Parameters**:
  - `ciphertext`: A pointer to the ciphertext data that needs to be decrypted.
  - `len`: The length of the ciphertext data in bytes.
  - `key`: A pointer to the decryption key (must be 8 bytes for DES).
  - `iv`: A pointer to the initialization vector (IV) for modes that require it (CBC, CFB, OFB). If `NULL`, an IV of all zeros is used.
  - `mode`: The mode of operation for DES (e.g., `CRYPTO_MODE_ECB`, `CRYPTO_MODE_CBC`).
  - `out_len`: A pointer to a variable that will store the length of the decrypted data.

**Return Value**: A pointer to the decrypted data (plaintext), or `NULL` if an error occurs — including, in ECB/CBC, malformed PKCS#7 padding (usually a wrong key or IV). `*out_len` is the original plaintext length, and the buffer is NUL-terminated (the terminator is not counted), so text can be printed with `%s`. The caller is responsible for freeing the memory allocated for the decrypted data.

**Usage Case**: Use to decrypt DES ciphertext produced by `crypto_des_encrypt`. The ciphertext is processed in blocks of 8 bytes (the DES block size), and memory is allocated for the plaintext. If an initialization vector (IV) is not provided, the function defaults to using an IV of all zeros. Memory allocated for the plaintext must be freed by the caller. If any input parameters are invalid (e.g., `ciphertext`, `key`, or `out_len` is `NULL`), the function logs an error and returns `NULL`.

---

### `bool crypto_generate_random_iv(uint8_t *iv, size_t length)`
**Purpose**: Generates a cryptographically secure random Initialization Vector (IV) using platform-specific methods (CryptGenRandom on Windows, /dev/urandom on POSIX).

**Parameters**:
  - `iv`: Pointer to the buffer where the generated IV will be stored.
  - `length`: The length in bytes of the IV to generate.

**Return Value**: `true` on success; `false` for invalid arguments or when the system CSPRNG fails (the IV is then zero-filled and must not be used).

**Usage Case**: Use before encrypting with CBC, CFB, or OFB mode to produce a unique, unpredictable IV. The function terminates the program if the platform CSPRNG fails, preventing use of an uninitialized IV.

---

## Additional Helpers (8)

These cover the most common production scenarios that aren't covered
by the raw hash / DES primitives above: hashing strings and files,
serializing digests, computing HMACs, comparing secrets safely, and
generating CSPRNG bytes.

### `uint8_t* crypto_hash_string(const char* str, HashAlgorithm algorithm, size_t* outLength)`
**Purpose**: Hash a NUL-terminated C string. Convenience wrapper around `crypto_hash_data` that supplies `strlen(str)` for you.

**Parameters**:
  - `str`: NUL-terminated input string. NULL → returns NULL.
  - `algorithm`: Hash algorithm to use (same enum set as `crypto_hash_data`).
  - `outLength`: Receives the hash length on success. Must be non-NULL.

**Return Value**: Heap-allocated hash bytes the caller must `free`, or NULL on any error.

**Usage Case**: Use instead of `crypto_hash_data` when your input is already a C string, to avoid computing `strlen` manually.

---

### `char* crypto_hash_to_hex(const uint8_t* hash, size_t length)`
**Purpose**: Convert a binary hash to a lowercase hex string of length `2 * length`.

**Parameters**:
  - `hash`: Hash bytes to encode.
  - `length`: Number of bytes in `hash`.

**Return Value**: Heap-allocated NUL-terminated hex string the caller must `free`, or NULL on NULL input or allocation failure.

**Usage Case**: Use to display or store a digest as a human-readable string for logging, file output, or comparison with published test vectors.

---

### `char* crypto_hash_to_base64(const uint8_t* hash, size_t length)`
**Purpose**: Convert a binary hash (or any byte buffer) to a base64-encoded string. Wraps OpenSSL's `EVP_EncodeBlock`; result is NUL-terminated with no line breaks.

**Parameters**:
  - `hash`: Bytes to encode.
  - `length`: Number of bytes.

**Return Value**: Heap-allocated base64 string the caller must `free`, or NULL on failure.

**Usage Case**: Use for storing or transmitting digests in config files, HTTP headers, or JSON where binary data is not allowed.

---

### `uint8_t* crypto_hash_file(const char* path, HashAlgorithm algorithm, size_t* outLength)`
**Purpose**: Stream-hash the contents of a file. Reads in 64 KiB chunks and never holds the whole file in memory. A read error part-way through returns `NULL` (it used to return the digest of the prefix that was read).

**Parameters**:
  - `path`: Filesystem path in UTF-8 (on Windows it is opened with the wide-character API, so non-ASCII names work whatever the system code page). NULL → returns NULL.
  - `algorithm`: Any `HashAlgorithm` value (SHAKE-128/256 not supported by this wrapper).
  - `outLength`: Receives the hash length on success.

**Return Value**: Heap-allocated digest the caller must `free`, or NULL on error (missing file, bad algorithm, etc.).

**Usage Case**: Use to hash large files (disk images, release archives) without loading them into RAM.

---

### `uint8_t* crypto_hmac(const uint8_t* data, size_t data_len, const uint8_t* key, size_t key_len, HashAlgorithm algorithm, size_t* outLength)`
**Purpose**: Compute an HMAC (Hash-based Message Authentication Code) — the standard primitive for symmetric-key message authentication.

**Parameters**:
  - `data`, `data_len`: Message bytes.
  - `key`, `key_len`: HMAC key (any length; long keys are hashed internally).
  - `algorithm`: Hash algorithm driving the HMAC (HMAC-SHA256 is the common production choice).
  - `outLength`: Receives the MAC length on success.

**Return Value**: Heap-allocated MAC bytes the caller must `free`, or NULL on bad input or unsupported algorithm.

**Usage Case**: Use to authenticate messages, sign API payloads, or verify webhook deliveries. Pair with `crypto_verify_hmac` for constant-time MAC verification.

---

### `bool crypto_constant_time_equal(const void* a, const void* b, size_t length)`
**Purpose**: Constant-time byte equality. Compares `length` bytes from `a` and `b`; runtime does NOT depend on the position of the first differing byte.

**Parameters**:
  - `a`, `b`: Pointers to the two buffers.
  - `length`: Number of bytes to compare.

**Return Value**: `true` if all `length` bytes are equal, `false` otherwise. Returns `false` if either pointer is NULL. Zero length returns `true`.

**Usage Case**: Always use this instead of `memcmp` when comparing MACs, tokens, or any secret-derived value to prevent timing side-channel attacks.

---

### `bool crypto_random_bytes(uint8_t* buffer, size_t length)`
**Purpose**: Fill a buffer with cryptographically-secure random bytes, wrapping OpenSSL's `RAND_bytes`.

**Parameters**:
  - `buffer`: Destination buffer.
  - `length`: Number of bytes to fill.

**Return Value**: `true` on success. `false` for a NULL buffer or when the CSPRNG fails; on failure the buffer is zero-filled and must not be used as a key, salt or nonce (the function never aborts the process).

**Usage Case**: Use to generate keys, IVs, nonces, session tokens, or any value that must be unpredictable.

---

### `const char* crypto_hash_algorithm_name(HashAlgorithm algorithm)`
**Purpose**: Return a human-readable name for a `HashAlgorithm` enum value. The returned pointer is to a static, immutable string.

**Parameters**:
  - `algorithm`: The enum value to look up.

**Return Value**: A constant string (e.g. `"SHA-256"`, `"BLAKE2b-512"`, `"RIPEMD-160"`). Unknown enum values return `"UNKNOWN"`. Never NULL.

**Usage Case**: Use for log messages, error reporting, and any display context where the enum value needs a printable name.

---

## Additional Helpers, Batch 2 (5)

Round-trip decoders, password-based key derivation, and safe
verification wrappers — the helpers most production code reaches for
the day after it's deployed.

### `uint8_t* crypto_hash_from_hex(const char* hex, size_t* outLength)`
**Purpose**: Inverse of `crypto_hash_to_hex` — parse a lowercase or uppercase hex string back to a byte array.

**Parameters**:
  - `hex`: NUL-terminated hex string with an even number of characters. NULL → NULL.
  - `outLength`: Receives the decoded byte length on success.

**Return Value**: Heap-allocated bytes the caller must `free`, or NULL on bad input (odd length, invalid hex digit, NULL).

**Usage Case**: Use when hashes arrive as strings (config files, HTTP headers, signed manifests) and you need to compare them against a freshly computed digest.

---

### `uint8_t* crypto_hash_from_base64(const char* b64, size_t* outLength)`
**Purpose**: Inverse of `crypto_hash_to_base64` — decode a base64 string back to raw bytes.

**Parameters**:
  - `b64`: NUL-terminated base64 string, padded to a multiple of 4 with `=`. NULL → NULL.
  - `outLength`: Receives the decoded byte length.

**Return Value**: Heap-allocated bytes the caller must `free`, or NULL on error.

**Usage Case**: Use when digests or MACs arrive as base64 (JSON payloads, HTTP headers) and need to be decoded for byte-level comparison.

---

### `uint8_t* crypto_pbkdf2(const char* password, size_t password_len, const uint8_t* salt, size_t salt_len, int iterations, size_t key_len, HashAlgorithm algorithm)`
**Purpose**: Password-Based Key Derivation Function 2 (PBKDF2). Stretches a low-entropy password into a high-entropy key using iterated HMAC.

**Parameters**:
  - `password`, `password_len`: Password bytes (need not be NUL-terminated).
  - `salt`, `salt_len`: Per-user random salt (at least 16 bytes recommended).
  - `iterations`: Work factor; OWASP recommends at least 600 000 for HMAC-SHA256 (as of 2023). Must be > 0.
  - `key_len`: Desired output key length in bytes (e.g. 32 for a 256-bit key).
  - `algorithm`: Hash algorithm driving the inner HMAC; typically `CRYPTO_SHA256` or `CRYPTO_SHA512`.

**Return Value**: Heap-allocated derived key the caller must `free`, or NULL on bad input or unsupported algorithm.

**Usage Case**: Store `(salt, iterations, derived_key)` in the user database instead of the raw password. On login, re-derive with the stored salt+iterations and compare with `crypto_constant_time_equal`.

---

### `bool crypto_verify_hmac(const uint8_t* data, size_t data_len, const uint8_t* key, size_t key_len, const uint8_t* mac, size_t mac_len, HashAlgorithm algorithm)`
**Purpose**: Verify an incoming MAC in a single safe call — computes the expected HMAC over `data` with `key`, then constant-time compares it against `mac`.

**Parameters**:
  - `data`, `data_len`: Message bytes.
  - `key`, `key_len`: Shared HMAC key.
  - `mac`, `mac_len`: Received MAC to verify.
  - `algorithm`: Hash algorithm.

**Return Value**: `true` if the MAC verifies; `false` on length mismatch, content mismatch, or any NULL input.

**Usage Case**: Use for every incoming MAC validation (API signatures, JWT, webhook delivery). Combines `crypto_hmac` and `crypto_constant_time_equal` into one call so there is no way to accidentally use `memcmp`.

---

### `uint8_t* crypto_hmac_file(const char* path, const uint8_t* key, size_t key_len, HashAlgorithm algorithm, size_t* outLength)`
**Purpose**: Stream-compute HMAC over a file's contents. Reads the file in 64 KiB chunks via `HMAC_Update` — never holds the whole file in memory. A read error part-way through returns `NULL`.

**Parameters**:
  - `path`: Filesystem path in UTF-8 (wide-character API on Windows). NULL → NULL.
  - `key`, `key_len`: Shared HMAC key.
  - `algorithm`: Hash algorithm. SHAKE variants are not supported.
  - `outLength`: Receives the MAC length on success.

**Return Value**: Heap-allocated MAC bytes the caller must `free`, or NULL on any error.

**Usage Case**: Use to sign or verify release artifacts, large log files, or any file whose integrity you need to check without loading it fully into RAM.

---

### `uint8_t* crypto_aes_encrypt(const uint8_t* plaintext, size_t len, const uint8_t* key, size_t key_len, const uint8_t* iv, CryptoMode mode, size_t* out_len)`

**Purpose**: AES encryption with a modern, hardware-accelerated cipher. `key_len` selects the variant — 16 → AES-128, 24 → AES-192, 32 → AES-256.

**Parameters**: `plaintext`/`len` input (any length, including 2 GiB and more); `key`/`key_len` the key; `iv` a 16-byte IV — required for CBC/CFB/OFB/CTR (`NULL` is rejected: OpenSSL would use an all-zero IV, giving identical ciphertexts for identical messages and keystream reuse in CTR/OFB), ignored for `CRYPTO_MODE_ECB`; `mode` one of ECB/CBC/CFB/OFB/CTR; `out_len` receives the ciphertext length.

**Return Value**: Heap-allocated ciphertext (caller `free`s), or `NULL` on error (e.g. an invalid key length or a missing IV).CBC/ECB apply PKCS#7 padding so the output is rounded up to the 16-byte block; CFB/OFB/CTR are stream modes whose output length equals the input length.

**Usage Case**: General-purpose symmetric encryption when you manage integrity separately (or don't need it). When in doubt, use `crypto_aes_gcm_encrypt` instead.

---

### `uint8_t* crypto_aes_decrypt(const uint8_t* ciphertext, size_t len, const uint8_t* key, size_t key_len, const uint8_t* iv, CryptoMode mode, size_t* out_len)`

**Purpose**: Inverse of `crypto_aes_encrypt` (same key, IV and mode). As there, the IV is required for every mode except ECB.

**Return Value**: Heap-allocated plaintext (caller `free`s) and `*out_len`, or `NULL` on error — which for CBC/ECB includes a PKCS#7 **padding failure** (a wrong key or corrupted ciphertext). Note that CBC is *not* authenticated: a successful decrypt does not prove the ciphertext wasn't tampered with — use an AEAD mode for that.

---

### `uint8_t* crypto_aes_gcm_encrypt(const uint8_t* plaintext, size_t len, const uint8_t* key, size_t key_len, const uint8_t* iv, size_t iv_len, const uint8_t* aad, size_t aad_len, uint8_t* tag, size_t tag_len, size_t* out_len)`

**Purpose**: **Authenticated encryption (AEAD)** with AES-GCM — the industry standard (used by TLS). Provides confidentiality *and* integrity/authenticity in one pass.

**Parameters**: `key_len` ∈ {16,24,32}; `iv` is the nonce (**12 bytes recommended, and it MUST be unique for every message encrypted under the same key**); `aad`/`aad_len` is *Additional Authenticated Data* (may be `NULL`) — authenticated but not encrypted (e.g. a header/version); on return the `tag_len`-byte (12 to 16; use 16) authentication tag is written to `tag`; `out_len` receives the ciphertext length (== plaintext length).

**Return Value**: Heap-allocated ciphertext (caller `free`s), or `NULL` on error.

**Usage Case**: Encrypting messages, tokens, session data, files — anywhere you need to detect tampering. Store/transmit the `iv` and `tag` alongside the ciphertext.

---

### `uint8_t* crypto_aes_gcm_decrypt(const uint8_t* ciphertext, size_t len, const uint8_t* key, size_t key_len, const uint8_t* iv, size_t iv_len, const uint8_t* aad, size_t aad_len, const uint8_t* tag, size_t tag_len, size_t* out_len)`

**Purpose**: Authenticated decryption. The tag is verified **before** the plaintext is accepted. `tag_len` must be 12 to 16: shorter values are refused, because OpenSSL would compare only that many bytes (a 1-byte prefix of the tag used to be accepted).

**Return Value**: Heap-allocated plaintext, or **`NULL` if the tag does not verify** — i.e. the ciphertext, IV, AAD or key is wrong or was tampered with. Always check for `NULL`; never use the output of a failed decrypt.

---

### `uint8_t* crypto_chacha20_poly1305_encrypt(...)` / `crypto_chacha20_poly1305_decrypt(...)`

**Purpose**: ChaCha20-Poly1305 AEAD (RFC 8439) — a modern alternative to AES-GCM that is fast in software on CPUs without AES-NI. Same parameter shape and contract as the AES-GCM pair above, but the **key must be 32 bytes**, the nonce 12 bytes and the tag 16 bytes.

**Return Value**: As for AES-GCM; decrypt returns `NULL` on authentication failure.

**Usage Case**: Mobile/embedded targets, or anywhere a constant-time software cipher is preferred. Functionally interchangeable with AES-GCM.

---

### `uint8_t* crypto_password_encrypt(const uint8_t* plaintext, size_t len, const char* password, int iterations, size_t* out_len)`

**Purpose**: High-level, hard-to-misuse password-based encryption. Derives a key from `password` with **PBKDF2-HMAC-SHA256** (a fresh random 16-byte salt per call) and encrypts with **AES-256-GCM** (a fresh random nonce per call), returning **one self-describing blob**: `magic | iterations | salt | nonce | tag | ciphertext`.

**Parameters**: `plaintext`/`len`; `password` (NUL-terminated); `iterations` (PBKDF2 cost; `<= 0` uses a sane default of 200,000; more than 10,000,000 is refused with `NULL`, because `crypto_password_decrypt` refuses such blobs to bound the work a hostile blob can demand); `out_len` receives the blob length.

**Return Value**: Heap-allocated blob (caller `free`s), or `NULL` on error.

**Usage Case**: Encrypting a file, config secret, or backup with a passphrase — without having to manage salts, IVs and tags yourself.

---

### `uint8_t* crypto_password_decrypt(const uint8_t* blob, size_t blob_len, const char* password, size_t* out_len)`

**Purpose**: Reverse of `crypto_password_encrypt`.

**Return Value**: The original plaintext, or **`NULL` on a wrong password, a bad/foreign blob format, or any tampering** (the GCM tag makes all three indistinguishable to an attacker).

---

### `void crypto_secure_zero(void* ptr, size_t length)`

**Purpose**: Wipe sensitive bytes (keys, derived keys, decrypted plaintext) so the compiler cannot optimize the clear away (wraps `OPENSSL_cleanse`). `NULL`-safe.

**Usage Case**: Call on key material and secret buffers immediately before `free`ing them, to limit how long secrets linger in memory.

---

## Non-Cryptographic Hashes and Checksums (14)

Fast hashes for **hash tables, sharding, caches, Bloom filters, deduplication and integrity checks against accidental corruption**: FNV-1a, MurmurHash3, xxHash (XXH32, XXH64, XXH3) and CRC-32 / CRC-32C. They are written in plain C inside `crypto.c` (no OpenSSL calls), allocate nothing, keep no global state and are thread-safe.

> **Not for security.** Anyone can deliberately construct collisions for all of these functions. For passwords, signatures, MACs or tamper detection use `crypto_hash_data` (SHA-256, BLAKE2, ...), `crypto_hmac` or the AEAD ciphers above. A random seed (from `crypto_random_bytes`) makes accidental worst cases unlikely, but none of these resists a deliberate hash-flooding attack the way a keyed hash such as SipHash does; MurmurHash3 even has seed-independent collisions.

**Common contract**

- `data` may point anywhere (no alignment needed). `data == NULL` is treated as empty input, whatever `length` says.
- The values are the same on every CPU, OS and compiler (input is read as little-endian words), and equal those of the reference implementations: zlib `crc32`, the `xxhsum` tool / Python `xxhash` (libxxhash 0.8), SMHasher's MurmurHash3 / Python `mmh3`, and the published FNV test vectors. Verified on GCC, Clang and MSVC, with and without SIMD, under AddressSanitizer and UBSan.
- The `_update` functions continue a running value, so data can be fed in pieces (file chunks, network packets); the result equals hashing it all at once. Start CRCs from `0` and FNV from `CRYPTO_FNV1A_32_INIT` / `CRYPTO_FNV1A_64_INIT`.
- Hardware acceleration is chosen at compile time: CRC-32C uses the SSE4.2 `crc32` instruction when the build targets it (`-msse4.2`, `-march=native`, or MSVC `/arch:AVX` and above); XXH3's long-input loop uses SSE2 on every x86-64 and AVX2 when the build targets it (`-mavx2`, `/arch:AVX2`). Define `CRYPTO_HASH_PORTABLE` to force the plain-C paths. Results never depend on these options.
- FNV-1a, MurmurHash3 and xxHash are implemented as `static inline` functions at the end of `crypto.h` (internal `cryp_` prefix, not part of the API) that call nothing in `crypto.c` or OpenSSL. That is how `hashmap` uses XXH3 for `hashmap_hash_string` without depending on the crypto module. CRC-32 / CRC-32C live in `crypto.c`.

**Which one?**

| Need | Use |
| --- | --- |
| Hash-table keys, general fingerprints | `crypto_xxh3_64` (fastest on short keys; on large buffers on par with XXH64, faster with AVX2) |
| Smallest possible code, short keys | `crypto_fnv1a_32` / `crypto_fnv1a_64` |
| Fingerprints of large data / files | `crypto_xxh3_64` or `crypto_xxh64` |
| Several independent hashes (Bloom filters, consistent hashing) | `crypto_murmur3_32` / `crypto_murmur3_128` with different seeds, or `crypto_xxh3_64` with different seeds |
| Compatibility with existing data | `crypto_crc32` (zlib, gzip, PNG, ZIP, Ethernet), `crypto_crc32c` (iSCSI, ext4, Btrfs, SCTP, LevelDB/RocksDB), `crypto_murmur3_*` (`mmh3`, Guava), `crypto_xxh*` (`xxhsum`) |
| Checksum of a file or packet | `crypto_crc32` / `crypto_crc32c` for standard formats, otherwise `crypto_xxh64` |

**Speed** (Intel Core i5-10400F, one core, MinGW GCC 14 `-O2`; throughput on 64 MiB, time per call for short keys):

| Function | 64 MiB | 16-byte key | 100-byte key |
| --- | ---: | ---: | ---: |
| `crypto_fnv1a_32` / `crypto_fnv1a_64` | 0.98 GB/s | 8.6 ns | 82 ns |
| `crypto_murmur3_32` | 2.9 GB/s | 5.8 ns | 26 ns |
| `crypto_murmur3_128` | 6.6 GB/s | 6.8 ns | 18.5 ns |
| `crypto_xxh32` | 6.5 GB/s | 6.0 ns | 16.5 ns |
| `crypto_xxh64` | 11.3 GB/s | 5.8 ns | 14.6 ns |
| `crypto_xxh3_64` (SSE2 / with `-mavx2`) | 11.0 / 14.6 GB/s | 3.8 ns | 10.6 ns |
| `crypto_crc32` | 2.5 GB/s | 6.2 ns | 36 ns |
| `crypto_crc32c` (tables / with `-msse4.2`) | 2.5 / 8.9 GB/s | 6.2 / 2.8 ns | 36 / 6.5 ns |
| for comparison: zlib `crc32` (MSYS2 build) | 1.9 GB/s | 18.8 ns | 129 ns |

---

### `uint32_t crypto_fnv1a_32(const void* data, size_t length)` / `uint32_t crypto_fnv1a_32_update(uint32_t hash, const void* data, size_t length)`

**Purpose**: FNV-1a, 32-bit: one XOR and one multiply per byte. `crypto_fnv1a_32(d, n)` is `crypto_fnv1a_32_update(CRYPTO_FNV1A_32_INIT, d, n)`.

**Parameters**: `data`/`length` the bytes; `hash` the running value (`CRYPTO_FNV1A_32_INIT` to start).

**Return Value**: The hash (`0x811C9DC5` for empty input).

**Usage Case**: Short identifiers and small strings where code size matters more than speed; hashing a key that is made of several parts without concatenating them.

---

### `uint64_t crypto_fnv1a_64(const void* data, size_t length)` / `uint64_t crypto_fnv1a_64_update(uint64_t hash, const void* data, size_t length)`

**Purpose**: FNV-1a, 64-bit. Same as the 32-bit pair with a 64-bit state; start from `CRYPTO_FNV1A_64_INIT`.

**Return Value**: The hash (`0xCBF29CE484222325` for empty input).

---

### `uint32_t crypto_murmur3_32(const void* data, size_t length, uint32_t seed)`

**Purpose**: MurmurHash3_x86_32 (Austin Appleby). Equals Python's `mmh3.hash(data, seed, signed=False)`.

**Parameters**: `seed` selects one of 2^32 different hash functions.

**Return Value**: The 32-bit hash.

**Usage Case**: Bloom filters and other structures that need several independent hashes of one key; compatibility with systems that store MurmurHash3 values.

---

### `void crypto_murmur3_128(const void* data, size_t length, uint32_t seed, uint64_t out[2])`

**Purpose**: MurmurHash3_x64_128. Writes the low 64 bits to `out[0]` and the high 64 bits to `out[1]` (Python's `mmh3.hash64` returns the same two halves as signed numbers; `mmh3.hash128` is `out[1] << 64 | out[0]`). Does nothing when `out` is `NULL`.

**Usage Case**: A 128-bit fingerprint where collisions must be negligible across billions of items (deduplication keys, cache keys), or two 64-bit hashes from one call.

---

### `uint32_t crypto_xxh32(const void* data, size_t length, uint32_t seed)`

**Purpose**: XXH32. Equals `xxhsum -H0` and Python's `xxhash.xxh32_intdigest(data, seed)`.

**Usage Case**: A 32-bit hash for 32-bit targets, or compatibility with formats that use XXH32 (LZ4 frame checksums).

---

### `uint64_t crypto_xxh64(const void* data, size_t length, uint64_t seed)`

**Purpose**: XXH64. Equals `xxhsum -H1` and Python's `xxhash.xxh64_intdigest(data, seed)`.

**Usage Case**: Fast 64-bit checksums of large data (Zstandard's content checksum is the low 32 bits of XXH64).

---

### `uint64_t crypto_xxh3_64(const void* data, size_t length, uint64_t seed)`

**Purpose**: XXH3, 64-bit: the fastest function in this group on short keys, and on large buffers as fast as XXH64 with SSE2 and about 30% faster with AVX2. Equals `xxhsum -H3` and Python's `xxhash.xxh3_64_intdigest(data, seed)` (seed `0` is the default secret).

**Usage Case**: The default choice for hash tables (`hashmap_hash_string` uses it; see Example 36) and fingerprints.

---

### `uint32_t crypto_crc32(const void* data, size_t length)` / `uint32_t crypto_crc32_update(uint32_t crc, const void* data, size_t length)`

**Purpose**: CRC-32 (IEEE 802.3, reflected polynomial `0xEDB88320`): the checksum of zlib, gzip, PNG, ZIP and Ethernet. `crypto_crc32_update(crc, d, n)` equals zlib's `crc32(crc, d, n)`; start from `0`.

**Return Value**: The CRC (`0xCBF43926` for `"123456789"`).

**Usage Case**: Reading or writing those formats; detecting accidental corruption of stored or transmitted data. Implemented with slicing-by-8 tables (on the test machine about 30% faster than the MSYS2 zlib build on large buffers and 3x faster on short ones).

---

### `uint32_t crypto_crc32c(const void* data, size_t length)` / `uint32_t crypto_crc32c_update(uint32_t crc, const void* data, size_t length)`

**Purpose**: CRC-32C (Castagnoli, reflected polynomial `0x82F63B78`), used by iSCSI, ext4, Btrfs, SCTP and LevelDB/RocksDB. Start from `0`. Uses the CPU's `crc32` instruction when the build targets SSE4.2.

**Return Value**: The CRC (`0xE3069283` for `"123456789"`).

**Usage Case**: Those formats, or a new checksum where SSE4.2 is available (it is then the fastest checksum here on short input).

---

## Example Programs

The library includes several example programs demonstrating how to use the various cryptographic functions:

1. **MD4 Hash Example:** Computes and prints an MD4 hash of a string.
2. **MD5 Hash Example:** Computes and prints an MD5 hash of a string.
3. **SHA-1 Hash Example:** Computes and prints a SHA-1 hash of a string.
4. **SHA-224 Hash Example:** Computes and prints a SHA-224 hash of a string.
5. **SHA-256 Hash Example:** Computes and prints a SHA-256 hash of a string.
6. **SHA-384 Hash Example:** Computes and prints a SHA-384 hash of a string.
7. **SHA-512 Hash Example:** Computes and prints a SHA-512 hash of a string.
8. **SHA3-224 Hash Example:** Computes and prints a SHA3-224 hash of a string.
9. **SHA3-256 Hash Example:** Computes and prints a SHA3-256 hash of a string.
10. **SHA3-384 Hash Example:** Computes and prints a SHA3-384 hash of a string.
11. **SHA3-512 Hash Example:** Computes and prints a SHA3-512 hash of a string.
12. **DES Encryption in OFB Mode Example:** Encrypts and decrypts a string using DES in OFB mode.
13. **DES Encryption in ECB Mode Example:** Encrypts and decrypts a string using DES in ECB mode.
14. **Random IV Generation and CBC Encryption Example:** Demonstrates random IV generation and DES encryption/decryption in CBC mode.

Examples 15–31 cover the helpers, HMAC, PBKDF2, AES and the AEAD ciphers; Examples 32–36 cover the non-cryptographic hashes (FNV-1a, MurmurHash3, xxHash, CRC-32/CRC-32C, and XXH3 in `hashmap`, with a seeded variant for untrusted keys).

---

## Example 1 : use `MD4` algorithm `crypto_hash_data` and `crypto_print_hash`
```c
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const char* exampleData = "The quick brown fox jumps over the lazy dog";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_MD4, &hashLength);

    fmt_printf("MD4(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```
**Result**
```
MD4("The quick brown fox jumps over the lazy dog") = 1bee69a46ba811185c194762abaeae90
```

---

## Example 2 : how to use `MD5`

```c
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const char* exampleData = "سلام";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_MD5, &hashLength);

    fmt_printf("MD5(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```
**Result**
```
MD5("سلام") = 78903c575b0dda53c4a7644a2dd36d0e
```

---

## Example 3 : How to Use `SHA-1`

```c
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const char* exampleData = "Hello World";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA1, &hashLength);

    fmt_printf("SHA-1(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```
**Result**
```
SHA-1("Hello World") = 0a4d55a8d778e5022fab701977c5d840bbc486d0
```

---

## Example 4 :`SHA-224`

```c
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const char* exampleData = "Hello World";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA224, &hashLength);

    fmt_printf("SHA-224(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```
**Result**
```
SHA-224("Hello World") = c4890faffdb0105d991a461e668e276685401b02eab1ef4372795047
```

---

## Example 5 : `SHA-256`

```c
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const char* exampleData = "Hello World";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA256, &hashLength);

    fmt_printf("SHA-256(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```
**Result**
```
SHA-256("Hello World") = a591a6d40bf420404a011733cfb7b190d62c65bf0bcda32b57b277d9ad9f146e
```

---

## Example 6 : `SHA-384`

```c
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const char* exampleData = "امین طهماسبی";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA384, &hashLength);

    fmt_printf("SHA-384(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```
**Result**
```
SHA-384("امین طهماسبی") = 638af16caec28354e9683d50e8504038c5af7c3f0313e54366c608552127bad296d2b139b96926eedc2dfa5187125c87
```

---

## Example 7 : `SHA-512`

```c
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const char* exampleData = "امین طهماسبی";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA512, &hashLength);

    fmt_printf("SHA-512(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```
**Result**
```
SHA-512("امین طهماسبی") = aa49758be6a253211043b32d327f7084182a04bbbdc0c26a608d5f2674f0e06a60806077d2a1b7fc12e97db1a840ad4b603182123082af50c7a3d1867a222cb0
```

---

## Example 8 : `SHA3-224`

```c
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const char* exampleData = "امین طهماسبی";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA3_224, &hashLength);

    fmt_printf("SHA3-224(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```
**Result**
```
SHA3-224("امین طهماسبی") = ddb8b6722b535a95aebf14b11648b60296330b82d8292a5880b4884f
```

---

## Example 9 : How to use `SHA3-256`


```c
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const char* exampleData = "امین طهماسبی";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA3_256, &hashLength);

    fmt_printf("SHA3-256(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```
**Result**
```
SHA3-256("امین طهماسبی") = 1886c3f57e943ae493fa42a2dbcaeb8e672d3131e0229014d00175e214402117
```

---

## Example 10 : How to use `SHA3-384`

```c
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const char* exampleData = "امین طهماسبی";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA3_384, &hashLength);

    fmt_printf("SHA3-384(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```
**Result**
```
SHA3-384("امین طهماسبی") = 1e33aa77d69c1db1b55606d707cafc6cc622bebd58561727a715d6969342f1e4a77dddf2a4c8a4f4136a2046feb54750
```

---

## Example 11 : How to use `SHA3-512`

```c
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const char* exampleData = "امین طهماسبی";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA3_512, &hashLength);

    fmt_printf("SHA3-512(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```
**Result**
```
SHA3-512("امین طهماسبی") = 77953bcbd41ec669833036e52d64c3d409bae8164af841735b113e3e9344b170cceda6640fcfb85f7a59cb6d1472773f8eca426fde80ca4ff6fe555cb9ff5d1f
```

---

## Example 12 : How to use `OFB` mode with `crypto_des_encrypt`

```c
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const uint8_t key[8] = "yourkey"; // 8 bytes key, assuming it is padded with zeros if less than 8 bytes
    const uint8_t plaintext[] = "Hello World"; // Your plaintext
    const uint8_t iv[DES_BLOCK_SIZE] = {0}; // The IV, if required by the mode
    size_t lenPlain = strlen((const char *)plaintext);
    size_t outLen;

    // Encrypt
    uint8_t* encrypted = (uint8_t*)crypto_des_encrypt(plaintext, lenPlain, key, iv, CRYPTO_MODE_OFB, &outLen);
    if (!encrypted) {
        fmt_fprintf(stderr, "Encryption failed");
        return 1;
    }

    fmt_printf("Encrypted text: ");
    crypto_print_hash(encrypted, outLen);

    // Decrypt
    uint8_t* decrypted = (uint8_t*)crypto_des_decrypt(encrypted, outLen, key, iv, CRYPTO_MODE_OFB, &outLen);
    if (!decrypted) {
        fmt_fprintf(stderr, "Decryption failed");
        free(encrypted); // Remember to free the allocated memory
        return 1;
    }

    fmt_printf("Decrypted text: ");
    for (size_t i = 0; i < outLen; i++) {
        putchar(decrypted[i]);
    }
    putchar('\n');

    // Cleanup
    free(encrypted);
    free(decrypted);
    return 0;
}

```
**Result**
```
Encrypted text: bbc09bb6b902b921ba2a31
Decrypted text: Hello World
```

---

## Example 13 : How to use `ECB` mode with `crypto_des_encrypt`

```c
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "crypto/crypto.h"
#include "fmt/fmt.h"


int main() {
    const uint8_t key[8] = "yourkey"; // 8 bytes key
    const uint8_t plaintext[] = "Hello World"; 
    size_t outLen;

    // Encrypt with ECB
    uint8_t* encrypted_ecb = (uint8_t*)crypto_des_encrypt(plaintext, strlen((const char *)plaintext), key, NULL, CRYPTO_MODE_ECB, &outLen);
    fmt_printf("ECB Encrypted: ");
    crypto_print_hash(encrypted_ecb, outLen);

    // Decrypt with ECB
    uint8_t* decrypted_ecb = (uint8_t*)crypto_des_decrypt(encrypted_ecb, outLen, key, NULL, CRYPTO_MODE_ECB, &outLen);
    fmt_printf("ECB Decrypted: %s\n", decrypted_ecb);

    free(encrypted_ecb);
    free(decrypted_ecb);
    return 0;
}

```
**Result**
```
ECB Encrypted: 92c71e5265020383f0e1c12cdd18f594
ECB Decrypted: Hello World
```

---

## Example 14 : Generate random IV for `CBC` mode `crypto_des_encrypt`

```c
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "fmt/fmt.h"
#include "crypto/crypto.h"

int main() {
    const uint8_t key[8] = "yourkey"; // 8 bytes key
    uint8_t iv[DES_BLOCK_SIZE]; // IV for CBC mode
    crypto_generate_random_iv(iv, DES_BLOCK_SIZE); // Generate a random IV for testing
    
    const uint8_t plaintext[] = "Hello World"; // Your plaintext
    size_t outLen;

    // Encrypt with CBC
    uint8_t* encrypted_cbc = (uint8_t*)crypto_des_encrypt(plaintext, strlen((const char *)plaintext), key, iv, CRYPTO_MODE_CBC, &outLen);
    fmt_printf("CBC Encrypted: ");
    crypto_print_hash(encrypted_cbc, outLen);

    // Decrypt with CBC
    uint8_t* decrypted_cbc = (uint8_t*)crypto_des_decrypt(encrypted_cbc, outLen, key, iv, CRYPTO_MODE_CBC, &outLen);
    fmt_printf("CBC Decrypted: %s\n", decrypted_cbc);

    // Cleanup
    free(encrypted_cbc);
    free(decrypted_cbc);

    return 0;
}

```
**Result**
```
CBC Encrypted: <varies — random IV used each run>
CBC Decrypted: Hello World
```

---

## Example 15 : `crypto_hash_string` — hash a NUL-terminated string

Convenience wrapper that fills in `strlen` for you. Verified against the
published `SHA-256("abc")` test vector.

```c
#include <stdio.h>
#include <stdlib.h>
#include "crypto/crypto.h"

int main(void) {
    size_t n = 0;
    uint8_t* h = crypto_hash_string("abc", CRYPTO_SHA256, &n);
    crypto_print_hash(h, n);
    /* ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad */
    free(h);
    return 0;
}
```
**Result**
```
ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
```

---

## Example 16 : `crypto_hash_to_hex` — pretty-print a digest

`crypto_print_hash` writes to stdout. When you need the hex AS a
string — for logging, comparison, or storing in a file — use this.

```c
#include <stdio.h>
#include <stdlib.h>
#include "crypto/crypto.h"

int main(void) {
    size_t n = 0;
    uint8_t* h = crypto_hash_string("hello", CRYPTO_SHA256, &n);
    char* hex = crypto_hash_to_hex(h, n);
    printf("sha256(hello) = %s\n", hex);
    /* sha256(hello) = 2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824 */
    free(hex);
    free(h);
    return 0;
}
```
**Result**
```
sha256(hello) = 2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824
```

---

## Example 17 : `crypto_hash_to_base64` — base64-encode a digest

Use this for storing digests in JSON, HTTP headers, or anywhere
ASCII-only is required and you want fewer characters than hex.

```c
#include <stdio.h>
#include <stdlib.h>
#include "crypto/crypto.h"

int main(void) {
    size_t n = 0;
    uint8_t* h = crypto_hash_string("hello", CRYPTO_SHA256, &n);
    char* b64 = crypto_hash_to_base64(h, n);
    printf("base64 = %s\n", b64);
    /* base64 = LPJNul+wow4m6DsqxbninhsWHlwfp0JecwQzYpOLmCQ= */
    free(b64);
    free(h);
    return 0;
}
```
**Result**
```
base64 = LPJNul+wow4m6DsqxbninhsWHlwfp0JecwQzYpOLmCQ=
```

---

## Example 18 : `crypto_hash_file` — streaming hash of a file

Reads the file in 64 KiB chunks — safe for arbitrarily large files.
Pair with `crypto_hash_to_hex` to print the result the way `sha256sum`
does.

```c
#include <stdio.h>
#include <stdlib.h>
#include "crypto/crypto.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <path>\n", argv[0]);
        return 1;
    }

    size_t n = 0;
    uint8_t* h = crypto_hash_file(argv[1], CRYPTO_SHA256, &n);
    if (!h) {
        fprintf(stderr, "failed to hash '%s'\n", argv[1]);
        return 1;
    }

    char* hex = crypto_hash_to_hex(h, n);
    printf("%s  %s\n", hex, argv[1]);
    free(hex);
    free(h);
    return 0;
}
```
**Result**
```
(requires a file path as command-line argument)
<sha256hex>  <path>
```

---

## Example 19 : `crypto_hmac` — message authentication

HMAC-SHA256 is the production-grade primitive for "did this message
come from someone who knows our shared secret?" Verified against the
canonical test vector for key=`"key"` and msg=`"The quick brown fox..."`.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"

int main(void) {
    const char* msg = "The quick brown fox jumps over the lazy dog";
    const char* key = "key";
    size_t mac_len = 0;

    uint8_t* mac = crypto_hmac((const uint8_t*)msg, strlen(msg),
                               (const uint8_t*)key, strlen(key),
                               CRYPTO_SHA256, &mac_len);

    char* hex = crypto_hash_to_hex(mac, mac_len);
    printf("HMAC-SHA256 = %s\n", hex);
    /* HMAC-SHA256 = f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8 */
    free(hex);
    free(mac);
    return 0;
}
```
**Result**
```
HMAC-SHA256 = f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8
```

---

## Example 20 : `crypto_constant_time_equal` — timing-safe MAC compare

The single most important security-critical helper here. **Always** use
this — never `memcmp` — when comparing MACs, tokens, hashes, or any
secret-derived value. `memcmp` short-circuits on the first mismatching
byte, leaking timing information that lets remote attackers brute-force
the secret one byte at a time.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"

/* Verify an incoming request: re-compute the MAC and compare it
   timing-safely against the one the client supplied. */
static bool verify(const char* body, const char* key, const uint8_t* received_mac) {
    size_t n = 0;
    uint8_t* expected = crypto_hmac((const uint8_t*)body, strlen(body),
                                    (const uint8_t*)key,  strlen(key),
                                    CRYPTO_SHA256, &n);
    bool ok = crypto_constant_time_equal(expected, received_mac, n);
    free(expected);
    return ok;
}

int main(void) {
    const char* body = "amount=100";
    const char* key  = "shared_secret";

    /* Pretend client computed and sent this same MAC. */
    size_t n = 0;
    uint8_t* sent = crypto_hmac((const uint8_t*)body, strlen(body),
                                (const uint8_t*)key,  strlen(key),
                                CRYPTO_SHA256, &n);

    printf("verify: %s\n", verify(body, key, sent) ? "ok" : "REJECT");
    /* verify: ok */

    /* Flip one byte → reject. */
    sent[0] ^= 0xFF;
    printf("verify: %s\n", verify(body, key, sent) ? "ok" : "REJECT");
    /* verify: REJECT */

    free(sent);
    return 0;
}
```
**Result**
```
verify: ok
verify: REJECT
```

---

## Example 21 : `crypto_random_bytes` — generate a fresh secret key

The general-purpose CSPRNG entry point. Use it for keys, IVs, nonces,
session tokens, anything that has to be unpredictable.

```c
#include <stdio.h>
#include <stdlib.h>
#include "crypto/crypto.h"

int main(void) {
    /* Generate a 32-byte (256-bit) key. */
    uint8_t key[32];
    crypto_random_bytes(key, sizeof(key));

    char* hex = crypto_hash_to_hex(key, sizeof(key));
    printf("new key: %s\n", hex);
    /* Output is different on every run. */
    free(hex);
    return 0;
}
```
**Result**
```
new key: <64 random hex characters — different on every run>
```

---

## Example 22 : `crypto_hash_algorithm_name` — pretty-print the algorithm

Useful for log messages and error reporting. Returns a static string
so it does not need to be freed.

```c
#include <stdio.h>
#include "crypto/crypto.h"

int main(void) {
    HashAlgorithm algos[] = {
        CRYPTO_MD5, CRYPTO_SHA1, CRYPTO_SHA256,
        CRYPTO_SHA3_256, CRYPTO_BLAKE2B_512, CRYPTO_RIPEMD_160
    };
    for (size_t i = 0; i < sizeof(algos) / sizeof(algos[0]); ++i) {
        printf("algorithm %2d -> %s\n", algos[i], crypto_hash_algorithm_name(algos[i]));
    }
    /* algorithm  1 -> MD5
       algorithm  2 -> SHA-1
       algorithm  4 -> SHA-256
       ... */
    return 0;
}
```
**Result**
```
algorithm  1 -> MD5
algorithm  2 -> SHA-1
algorithm  4 -> SHA-256
algorithm  8 -> SHA3-256
algorithm 13 -> BLAKE2b-512
algorithm 16 -> RIPEMD-160
```

---

## Example 23 : `crypto_hash_from_hex` — parse a hash string

Pairs with `crypto_hash_to_hex` for a clean round-trip. Useful when
hashes arrive as strings (config files, HTTP headers, signed manifests).

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "crypto/crypto.h"

int main(void) {
    /* Pretend we read this from a release manifest. */
    const char* expected_hex =
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";

    size_t en = 0;
    uint8_t* expected = crypto_hash_from_hex(expected_hex, &en);

    /* Compute the actual hash. */
    size_t an = 0;
    uint8_t* actual = crypto_hash_string("abc", CRYPTO_SHA256, &an);

    bool match = (en == an) && crypto_constant_time_equal(expected, actual, en);
    printf("hash match: %s\n", match ? "yes" : "NO");
    /* hash match: yes */

    free(expected); free(actual);
    return 0;
}
```
**Result**
```
hash match: yes
```

---

## Example 24 : `crypto_hash_from_base64` — decode a base64 digest

Inverse of `crypto_hash_to_base64`. The base64 form is more compact
than hex (33% vs 100% overhead), so it's a popular choice for
transmitting digests in JSON / HTTP headers.

```c
#include <stdio.h>
#include <stdlib.h>
#include "crypto/crypto.h"

int main(void) {
    const char* b64 = "AAECAwQFBgcICQoLDA0ODw==";  // bytes 0x00..0x0F

    size_t n = 0;
    uint8_t* bytes = crypto_hash_from_base64(b64, &n);
    printf("decoded %zu bytes: ", n);
    for (size_t i = 0; i < n; ++i) printf("%02x ", bytes[i]);
    printf("\n");
    /* decoded 16 bytes: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f */

    free(bytes);
    return 0;
}
```
**Result**
```
decoded 16 bytes: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 
```

---

## Example 25 : `crypto_pbkdf2` — store and verify a password

The right way to store passwords. Generate a random salt per user,
PBKDF2-stretch the password with a high iteration count, store
`(salt, iterations, derived_key)`. On login, re-run PBKDF2 with the
stored salt+iterations and constant-time-compare the result.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "crypto/crypto.h"

int main(void) {
    /* === Registration === */
    uint8_t salt[16];
    crypto_random_bytes(salt, sizeof(salt));
    int iters = 600000;            /* OWASP min for HMAC-SHA256 (2023) */

    const char* password = "hunter2";
    uint8_t* stored = crypto_pbkdf2(password, strlen(password),
                                    salt, sizeof(salt),
                                    iters, 32, CRYPTO_SHA256);
    /* In real code: write (salt, iters, stored) to your user DB. */

    /* === Login === */
    const char* attempt = "hunter2";   // try "wrongpass" for a failed login
    uint8_t* derived = crypto_pbkdf2(attempt, strlen(attempt),
                                     salt, sizeof(salt),
                                     iters, 32, CRYPTO_SHA256);

    bool ok = crypto_constant_time_equal(stored, derived, 32);
    printf("login: %s\n", ok ? "accepted" : "rejected");

    free(stored); free(derived);
    return 0;
}
```
**Result**
```
login: accepted
```

---

## Example 26 : `crypto_verify_hmac` — one-call safe MAC verification

The canonical "validate an incoming signed request" pattern. Combines
`crypto_hmac` and `crypto_constant_time_equal` into one call so there's
no way to accidentally use `memcmp` and leak timing.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"

int main(void) {
    const char* body = "transfer_amount=42";
    const char* secret = "shared_signing_key";

    /* Client side: compute the MAC over the body. */
    size_t mac_len = 0;
    uint8_t* mac = crypto_hmac((const uint8_t*)body, strlen(body),
                               (const uint8_t*)secret, strlen(secret),
                               CRYPTO_SHA256, &mac_len);

    /* Server side: verify in one safe call. */
    bool ok = crypto_verify_hmac((const uint8_t*)body, strlen(body),
                                 (const uint8_t*)secret, strlen(secret),
                                 mac, mac_len, CRYPTO_SHA256);
    printf("verify: %s\n", ok ? "ok" : "REJECT");        /* ok */

    /* Tamper with the body — verification must fail. */
    const char* tampered = "transfer_amount=99999";
    ok = crypto_verify_hmac((const uint8_t*)tampered, strlen(tampered),
                            (const uint8_t*)secret, strlen(secret),
                            mac, mac_len, CRYPTO_SHA256);
    printf("verify (tampered): %s\n", ok ? "ok" : "REJECT");    /* REJECT */

    free(mac);
    return 0;
}
```
**Result**
```
verify: ok
verify (tampered): REJECT
```

---

## Example 27 : `crypto_hmac_file` — sign or verify a release archive

Streaming HMAC over a file. Print it like `sha256sum` does, store it
alongside the artifact, and verify on the consumer side.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"


int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <path> <key>\n", argv[0]);
        return 1;
    }

    size_t n = 0;
    uint8_t* mac = crypto_hmac_file(argv[1],
                                    (const uint8_t*)argv[2],
                                    (size_t)strlen(argv[2]),
                                    CRYPTO_SHA256, &n);
    if (!mac) {
        fprintf(stderr, "failed to MAC '%s'\n", argv[1]);
        return 1;
    }

    char* hex = crypto_hash_to_hex(mac, n);
    printf("%s  %s\n", hex, argv[1]);

    free(hex); free(mac);
    return 0;
}
```
**Result**
```
(requires a file path and key as command-line arguments)
<hmac-sha256-hex>  <path>
```

---

## Example 28 : `crypto_aes_gcm_encrypt` / `crypto_aes_gcm_decrypt` — authenticated encryption

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"

int main(void) {
    const char* msg = "Authenticated secret payload";
    const char* aad = "context:v1";              /* authenticated, not encrypted */

    uint8_t key[32], iv[CRYPTO_GCM_IV_SIZE], tag[CRYPTO_GCM_TAG_SIZE];
    crypto_random_bytes(key, sizeof key);         /* in practice: derive or load a key */
    crypto_random_bytes(iv,  sizeof iv);          /* nonce: MUST be unique per message  */

    size_t ct_len = 0;
    uint8_t* ct = crypto_aes_gcm_encrypt((const uint8_t*)msg, strlen(msg), key, sizeof key,
                                         iv, sizeof iv, (const uint8_t*)aad, strlen(aad),
                                         tag, sizeof tag, &ct_len);
    printf("ciphertext: %zu bytes, tag: %zu bytes\n", ct_len, sizeof tag);

    size_t pt_len = 0;
    uint8_t* pt = crypto_aes_gcm_decrypt(ct, ct_len, key, sizeof key, iv, sizeof iv,
                                         (const uint8_t*)aad, strlen(aad), tag, sizeof tag, &pt_len);
    printf("decrypted: %.*s\n", (int)pt_len, (const char*)pt);

    /* Tampering is detected: flip one byte -> decrypt returns NULL. */
    ct[0] ^= 1;
    uint8_t* bad = crypto_aes_gcm_decrypt(ct, ct_len, key, sizeof key, iv, sizeof iv,
                                          (const uint8_t*)aad, strlen(aad), tag, sizeof tag, &pt_len);
    printf("tampered decrypt: %s\n", bad ? "ACCEPTED (bug!)" : "rejected (NULL)");

    crypto_secure_zero(key, sizeof key);
    free(ct); free(pt); free(bad);
    return 0;
}
```
**Result**
```
ciphertext: 28 bytes, tag: 16 bytes
decrypted: Authenticated secret payload
tampered decrypt: rejected (NULL)
```

---

## Example 29 : `crypto_chacha20_poly1305_encrypt` / `_decrypt` — modern AEAD

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"

int main(void) {
    const char* msg = "ChaCha20-Poly1305 is great without AES-NI";

    uint8_t key[CRYPTO_CHACHA_KEY_SIZE];          /* must be 32 bytes */
    uint8_t iv[CRYPTO_CHACHA_IV_SIZE];            /* 12-byte nonce    */
    uint8_t tag[CRYPTO_CHACHA_TAG_SIZE];
    crypto_random_bytes(key, sizeof key);
    crypto_random_bytes(iv,  sizeof iv);

    size_t ct_len = 0;
    uint8_t* ct = crypto_chacha20_poly1305_encrypt((const uint8_t*)msg, strlen(msg),
                                                   key, sizeof key, iv, sizeof iv,
                                                   NULL, 0, tag, sizeof tag, &ct_len);

    size_t pt_len = 0;
    uint8_t* pt = crypto_chacha20_poly1305_decrypt(ct, ct_len, key, sizeof key, iv, sizeof iv,
                                                   NULL, 0, tag, sizeof tag, &pt_len);
    printf("decrypted (%zu bytes): %.*s\n", pt_len, (int)pt_len, (const char*)pt);

    crypto_secure_zero(key, sizeof key);
    free(ct); free(pt);
    return 0;
}
```
**Result**
```
decrypted (41 bytes): ChaCha20-Poly1305 is great without AES-NI
```

---

## Example 30 : `crypto_password_encrypt` / `crypto_password_decrypt` — encrypt with a passphrase

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"

int main(void) {
    const char* secret   = "API_KEY=sk-live-abc123";
    const char* password = "correct horse battery staple";

    /* One call: PBKDF2-HMAC-SHA256 derives a key, AES-256-GCM encrypts, and a
       self-describing blob (salt + nonce + tag + ciphertext) comes back. */
    size_t blob_len = 0;
    uint8_t* blob = crypto_password_encrypt((const uint8_t*)secret, strlen(secret),
                                            password, 200000, &blob_len);
    printf("blob: %zu bytes\n", blob_len);

    size_t out_len = 0;
    uint8_t* pt = crypto_password_decrypt(blob, blob_len, password, &out_len);
    printf("decrypted: %.*s\n", (int)out_len, (const char*)pt);

    /* A wrong password (or any tampering) is rejected with NULL. */
    uint8_t* wrong = crypto_password_decrypt(blob, blob_len, "guess", &out_len);
    printf("wrong password: %s\n", wrong ? "ACCEPTED (bug!)" : "rejected (NULL)");

    free(blob); free(pt); free(wrong);
    return 0;
}
```
**Result**
```
blob: 74 bytes
decrypted: API_KEY=sk-live-abc123
wrong password: rejected (NULL)
```

---

## Example 31 : `crypto_aes_encrypt` / `crypto_aes_decrypt` — AES-256-CBC round-trip

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto/crypto.h"

int main(void) {
    const char* msg = "Block-cipher round trip";

    uint8_t key[32], iv[CRYPTO_AES_BLOCK_SIZE];
    crypto_random_bytes(key, sizeof key);
    crypto_generate_random_iv(iv, sizeof iv);

    size_t ct_len = 0, pt_len = 0;
    uint8_t* ct = crypto_aes_encrypt((const uint8_t*)msg, strlen(msg), key, sizeof key,
                                     iv, CRYPTO_MODE_CBC, &ct_len);
    uint8_t* pt = crypto_aes_decrypt(ct, ct_len, key, sizeof key, iv, CRYPTO_MODE_CBC, &pt_len);

    printf("AES-256-CBC: %zu -> %zu bytes, plaintext=\"%.*s\"\n",
           ct_len, pt_len, (int)pt_len, (const char*)pt);   /* ct is PKCS#7-padded to 32 */

    crypto_secure_zero(key, sizeof key);
    free(ct); free(pt);
    return 0;
}
```
**Result**
```
AES-256-CBC: 32 -> 23 bytes, plaintext="Block-cipher round trip"
```

---

## Example 32 : `crypto_fnv1a_32` / `crypto_fnv1a_64` — tiny hashes for short keys

```c
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "crypto/crypto.h"

int main(void) {
    const char* key = "foobar";
    size_t n = strlen(key);

    printf("fnv1a_32(\"%s\") = %08" PRIx32 "\n", key, crypto_fnv1a_32(key, n));
    printf("fnv1a_64(\"%s\") = %016" PRIx64 "\n", key, crypto_fnv1a_64(key, n));

    /* Hash a key made of several parts without concatenating them. */
    uint64_t h = CRYPTO_FNV1A_64_INIT;
    h = crypto_fnv1a_64_update(h, "foo", 3);
    h = crypto_fnv1a_64_update(h, "bar", 3);
    printf("in two parts       = %016" PRIx64 "\n", h);

    /* Bucket of the key in a table of 16 buckets. */
    printf("bucket (of 16)     = %" PRIu32 "\n", crypto_fnv1a_32(key, n) & 15u);
    return 0;
}
```
**Result**
```
fnv1a_32("foobar") = bf9cf968
fnv1a_64("foobar") = 85944171f73967e8
in two parts       = 85944171f73967e8
bucket (of 16)     = 8
```

---

## Example 33 : `crypto_murmur3_32` / `crypto_murmur3_128` — seeded hashes, Bloom filter bits

```c
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "crypto/crypto.h"

int main(void) {
    const char* msg = "The quick brown fox jumps over the lazy dog";
    size_t n = strlen(msg);

    printf("murmur3_32 (seed 0x9747b28c) = %08" PRIx32 "\n", crypto_murmur3_32(msg, n, 0x9747B28Cu));
    printf("murmur3_32 (seed 0)          = %08" PRIx32 "\n", crypto_murmur3_32(msg, n, 0));

    uint64_t h[2];
    crypto_murmur3_128(msg, n, 0, h);
    printf("murmur3_128 (seed 0)         = %016" PRIx64 " %016" PRIx64 "\n", h[0], h[1]);

    /* A Bloom filter needs k independent hashes: one algorithm, k seeds. */
    for (uint32_t seed = 1; seed <= 3; ++seed) {
        printf("bit for seed %" PRIu32 " (of 1024)    = %" PRIu32 "\n", seed, crypto_murmur3_32(msg, n, seed) % 1024u);
    }
    return 0;
}
```
**Result**
```
murmur3_32 (seed 0x9747b28c) = 2fa826cd
murmur3_32 (seed 0)          = 2e4ff723
murmur3_128 (seed 0)         = e34bbc7bbc071b6c 7a433ca9c49a9347
bit for seed 1 (of 1024)    = 551
bit for seed 2 (of 1024)    = 651
bit for seed 3 (of 1024)    = 372
```

---

## Example 34 : `crypto_xxh32` / `crypto_xxh64` / `crypto_xxh3_64` — xxHash

The values are the same as `xxhsum -H0` / `-H1` / `-H3` and Python's `xxhash` module print.

```c
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "crypto/crypto.h"

int main(void) {
    printf("xxh32  (\"abc\", seed 0)  = %08" PRIx32 "\n", crypto_xxh32("abc", 3, 0));
    printf("xxh64  (\"abc\", seed 0)  = %016" PRIx64 "\n", crypto_xxh64("abc", 3, 0));
    printf("xxh3_64(\"abc\", seed 0)  = %016" PRIx64 "\n", crypto_xxh3_64("abc", 3, 0));
    printf("xxh3_64(\"abc\", seed 42) = %016" PRIx64 "\n", crypto_xxh3_64("abc", 3, 42));

    /* Fingerprint 1 MiB of data; a single flipped bit changes the whole hash. */
    size_t n = (size_t)1 << 20;
    unsigned char* buf = malloc(n);
    if (buf == NULL) {
        return 1;
    }
    for (size_t i = 0; i < n; ++i) {
        buf[i] = (unsigned char)(i * 31u);
    }
    printf("xxh3_64(1 MiB)          = %016" PRIx64 "\n", crypto_xxh3_64(buf, n, 0));
    buf[n / 2] ^= 1;
    printf("after flipping one bit  = %016" PRIx64 "\n", crypto_xxh3_64(buf, n, 0));

    free(buf);
    return 0;
}
```
**Result**
```
xxh32  ("abc", seed 0)  = 32d153ff
xxh64  ("abc", seed 0)  = 44bc2cf5ad770999
xxh3_64("abc", seed 0)  = 78af5f94892f3950
xxh3_64("abc", seed 42) = d8438def21bbdcc3
xxh3_64(1 MiB)          = 8aa721cade95685b
after flipping one bit  = 68b03668de36d716
```

---

## Example 35 : `crypto_crc32` / `crypto_crc32c` — checksums, fed in pieces

```c
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "crypto/crypto.h"

int main(void) {
    /* "123456789" is the standard check input for CRCs. */
    printf("crc32 (\"123456789\") = %08" PRIx32 "   (zlib, gzip, PNG, ZIP)\n", crypto_crc32("123456789", 9));
    printf("crc32c(\"123456789\") = %08" PRIx32 "   (iSCSI, ext4, Btrfs)\n", crypto_crc32c("123456789", 9));

    /* Data that arrives in pieces (file chunks, packets): start from 0 and
       keep updating; the result equals the one-shot checksum. */
    const char* parts[] = { "1234", "56", "789" };
    uint32_t crc = 0, crcc = 0;
    for (size_t i = 0; i < sizeof parts / sizeof parts[0]; ++i) {
        crc  = crypto_crc32_update(crc, parts[i], strlen(parts[i]));
        crcc = crypto_crc32c_update(crcc, parts[i], strlen(parts[i]));
    }
    printf("in 3 pieces          = %08" PRIx32 " / %08" PRIx32 "\n", crc, crcc);

    /* Detect corruption: the receiver recomputes the CRC and compares. */
    char packet[] = "transfer 100 coins to bob";
    uint32_t sent = crypto_crc32(packet, strlen(packet));
    packet[9] = '9';                                   /* a byte changed in transit */
    printf("corrupted packet     : %s\n", crypto_crc32(packet, strlen(packet)) == sent ? "not detected" : "detected");
    return 0;
}
```
**Result**
```
crc32 ("123456789") = cbf43926   (zlib, gzip, PNG, ZIP)
crc32c("123456789") = e3069283   (iSCSI, ext4, Btrfs)
in 3 pieces          = cbf43926 / e3069283
corrupted packet     : detected
```

---

## Example 36 : `hashmap` and XXH3 — the default string hash, and a seeded one for untrusted keys

`hashmap_hash_string` already is XXH3 with seed 0 (the static inline code in `crypto.h`, so hashmap needs no OpenSSL). When keys come from untrusted input (HTTP parameters, user names, file names), a secret per-process seed makes precomputed lists of colliding keys useless. It raises the bar rather than closing the door, because XXH3 is not a cryptographic keyed hash such as SipHash. `hashmap_create` takes any `size_t (*)(const void*)`, so the seeded hash plugs straight in.

```c
#include <stdio.h>
#include <string.h>
#include "crypto/crypto.h"
#include "hashmap/hashmap.h"

static uint64_t hash_seed;   /* secret, random per process */

/* HashFunc for NUL-terminated string keys, keyed with hash_seed. */
static size_t seeded_string_hash(const void* key) {
    const char* s = (const char*)key;
    return (size_t)crypto_xxh3_64(s, strlen(s), hash_seed);
}

int main(void) {
    const char* key = "alice";
    printf("hashmap_hash_string == crypto_xxh3_64(seed 0): %s\n",
           hashmap_hash_string(key) == (size_t)crypto_xxh3_64(key, strlen(key), 0) ? "yes" : "no");

    if (!crypto_random_bytes((uint8_t*)&hash_seed, sizeof hash_seed)) {
        return 1;
    }

    /* The keys and values are string literals here, so nothing is freed. */
    HashMap* roles = hashmap_create(seeded_string_hash, hashmap_compare_string, NULL, NULL);
    hashmap_insert(roles, (void*)"alice", (void*)"admin");
    hashmap_insert(roles, (void*)"bob",   (void*)"user");
    hashmap_insert(roles, (void*)"carol", (void*)"guest");

    printf("size : %zu\n", hashmap_size(roles));
    printf("bob  : %s\n", (const char*)hashmap_at(roles, (void*)"bob"));

    hashmap_deallocate(roles);
    return 0;
}
```
**Result**
```
hashmap_hash_string == crypto_xxh3_64(seed 0): yes
size : 3
bob  : user
```

---

## License

This project is open-source and available under [ISC License].