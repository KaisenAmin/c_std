# Crypto Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License  

## Overview

The Crypto Library in C is a comprehensive cryptographic library designed for developers who require secure and efficient hashing and encryption capabilities in their C programs. It provides a straightforward API for a variety of cryptographic operations, including hashing with multiple algorithms and encryption/decryption using DES in various modes. The library is built on top of the OpenSSL library, ensuring robust and widely trusted cryptographic functionality.

This library is particularly useful for developers who are familiar with cryptographic libraries in higher-level languages like C++ or Python and wish to leverage similar capabilities in C.

## Key Features

- **Multiple Hashing Algorithms:** Supports a wide range of hashing algorithms, including MD4, MD5, SHA-1, SHA-2 (SHA-224, SHA-256, SHA-384, SHA-512), SHA-3, SHAKE variants, BLAKE2, MDC2, and RIPEMD-160.
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

**Returns**: A pointer to the computed hash (as a byte array), or `NULL` if an error occurs (e.g., memory allocation failure or unsupported algorithm). The caller is responsible for freeing the memory allocated for the hash.

**Details**:
- The function supports multiple cryptographic algorithms, including MD5, SHA-256, SHA3, SHAKE, Blake2, RIPEMD, and others.
- Depending on the selected algorithm, it either directly calls lower-level hashing functions (like `MD5`, `SHA256`) or uses OpenSSL's `EVP` interfaces for more complex algorithms like SHA3 and Blake2.
- The function performs error checking, ensuring the `outLength` pointer is valid and that memory allocation succeeds.
- If any error occurs during the hashing process, the function logs the error, cleans up any allocated resources, and returns `NULL`.

---

### `void crypto_print_hash(const uint8_t* hash, size_t length)`
**Purpose**: This function prints cryptographic hash data as a hexadecimal string.

**Parameters**:
- `hash`: A pointer to the byte array containing the hash data.
- `length`: The number of bytes in the hash data.

**Details**:
- The function converts each byte of the hash into two hexadecimal characters and prints the result.
- If the `hash` parameter is `NULL`, it logs an error message and prints "No hash data to print."

### `void* crypto_des_encrypt(const uint8_t* plaintext, size_t len, const uint8_t* key, const uint8_t* iv, CryptoMode mode, size_t* out_len)`
**Purpose**: Encrypts the given plaintext using the DES (Data Encryption Standard) algorithm in various modes (ECB, CBC, CFB, OFB).

**Parameters**:
- `plaintext`: A pointer to the plaintext data that needs to be encrypted.
- `len`: The length of the plaintext data in bytes.
- `key`: A pointer to the encryption key (must be 8 bytes for DES).
- `iv`: A pointer to the initialization vector (IV) for modes that require it (CBC, CFB, OFB). If `NULL`, an IV of all zeros is used.
- `mode`: The mode of operation for DES (e.g., `CRYPTO_MODE_ECB`, `CRYPTO_MODE_CBC`).
- `out_len`: A pointer to a variable that will store the length of the encrypted data.

**Returns**: A pointer to the encrypted data (ciphertext) or `NULL` if an error occurs. The caller is responsible for freeing the memory allocated for the ciphertext.

**Details**:
- The plaintext is padded to ensure its length is a multiple of the DES block size (8 bytes).
- The function uses DES encryption based on the mode of operation specified.
  - ECB (Electronic Codebook)
  - CBC (Cipher Block Chaining)
  - CFB (Cipher Feedback)
  - OFB (Output Feedback)
- Error handling ensures that null inputs for critical parameters (e.g., `plaintext`, `key`, `out_len`) lead to early returns with error messages logged.
- Memory is allocated for the padded plaintext and ciphertext, which must be freed by the caller.

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

**Returns**: A pointer to the decrypted data (plaintext), or `NULL` if an error occurs. The caller is responsible for freeing the memory allocated for the decrypted data.

**Details**:
- The function decrypts the provided ciphertext using DES based on the specified mode of operation:
  - ECB (Electronic Codebook)
  - CBC (Cipher Block Chaining)
  - CFB (Cipher Feedback)
  - OFB (Output Feedback)
- The ciphertext is processed in blocks of 8 bytes (the DES block size), and memory is allocated for the plaintext.
- If an initialization vector (IV) is not provided, the function defaults to using an IV of all zeros.
- Memory allocated for the plaintext must be freed by the caller.
- If any input parameters are invalid (e.g., `ciphertext`, `key`, or `out_len` is `NULL`), the function logs an error and returns `NULL`.

---

### `void crypto_generate_random_iv(uint8_t *iv, size_t length)`
**Purpose**: Generates a random Initialization Vector (IV) for cryptographic purposes, using platform-specific methods.

**Parameters**:
- `iv`: A pointer to the buffer where the generated IV will be stored.
- `length`: The length (in bytes) of the IV to generate.

**Platform-specific Implementations**:
1. **Windows (using Cryptographic API)**:
   - Acquires a cryptographic context via the `CryptAcquireContext` function.
   - Generates random bytes using the `CryptGenRandom` function.
   - Releases the cryptographic context.
   
   **Error Handling**:
   - If the cryptographic context cannot be acquired or if random bytes cannot be generated, the function logs the error and terminates the program.

2. **Unix-like systems (using `/dev/urandom`)**:
   - Opens the `/dev/urandom` file to read cryptographically secure random bytes.
   - Reads the specified number of random bytes into the IV buffer.
   - Closes the file once done.
   
   **Error Handling**:
   - If the file cannot be opened or the read operation fails, the function logs the error and terminates the program.

**Notes**:
- The function ensures platform compatibility by using preprocessor directives to choose the appropriate method based on whether the platform is Windows or Unix-like.
- Both implementations use cryptographically secure random sources to generate IVs, ensuring that the IVs are suitable for use in encryption algorithms.
- If an error occurs (e.g., failure to open `/dev/urandom` or to acquire a cryptographic context), the program will terminate to prevent usage of insecure or uninitialized data.

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

    fmt_printf("SHA-256(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```
**Result**
```
SHA-256("امین طهماسبی") = 1886c3f57e943ae493fa42a2dbcaeb8e672d3131e0229014d00175e214402117
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
Encrypted text: bbc09bb6b902b921ba2a316cabfa1019
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
ECB Encrypted: 92c71e526502038398375d833b4af95d
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
CBC Encrypted: 5248fc59b5a3056881a0337d50d011c2
CBC Decrypted: Hello World
```

---

## License

This project is open-source and available under [ISC License].