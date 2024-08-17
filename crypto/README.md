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


## Example 1 : use `MD4` algorithm `crypto_hash_data` and `crypto_print_hash`
```c
#include "crypto/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* exampleData = "The quick brown fox jumps over the lazy dog";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_MD4, &hashLength);

    printf("MD4(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```

## Example 2 : how to use `MD5`

```c

#include "crypto/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* exampleData = "سلام";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_MD5, &hashLength);

    printf("MD5(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}

```

## Example 3 : How to Use `SHA-1`

```c
#include "crypto/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* exampleData = "Hello World";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA1, &hashLength);

    printf("SHA-1(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```

## Example 4 :`SHA-224`

```c
#include "crypto/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* exampleData = "Hello World";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA224, &hashLength);

    printf("SHA-224(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```

## Example 5 : `SHA-256`

```c
#include "crypto/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* exampleData = "Hello World";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA256, &hashLength);

    printf("SHA-256(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```

## Example 6 : `SHA-384`

```c
#include "crypto/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* exampleData = "امین طهماسبی";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA384, &hashLength);

    printf("SHA-384(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```

## Example 7 : `SHA-512`

```c
#include "crypto/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* exampleData = "امین طهماسبی";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA512, &hashLength);

    printf("SHA-512(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```

## Example 8 : `SHA3-224`

```c
#include "crypto/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* exampleData = "امین طهماسبی";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA3_224, &hashLength);

    printf("SHA3-224(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```

## Example 9 : How to use `SHA3-256`

```c
#include "crypto/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* exampleData = "امین طهماسبی";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA3_256, &hashLength);

    printf("SHA-256(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```

## Example 10 : How to use `SHA3-384`

```c
#include "crypto/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* exampleData = "امین طهماسبی";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA3_384, &hashLength);

    printf("SHA3-384(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```

## Example 11 : How to use `SHA3-512`

```c
#include "crypto/crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* exampleData = "امین طهماسبی";
    size_t dataLength = strlen(exampleData);
    size_t hashLength = 0;
    uint8_t* hash = crypto_hash_data((const uint8_t*)exampleData, dataLength, CRYPTO_SHA3_512, &hashLength);

    printf("SHA3-512(\"%s\") = ", exampleData);
    crypto_print_hash(hash, hashLength);

    free(hash);
    return 0;
}
```

## Example 12 : How to use `OFB` mode with `crypto_des_encrypt`

```c
#include "crypto/crypto.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int main() {
    const uint8_t key[8] = "yourkey"; // 8 bytes key, assuming it is padded with zeros if less than 8 bytes
    const uint8_t plaintext[] = "Hello World"; // Your plaintext
    const uint8_t iv[DES_BLOCK_SIZE] = {0}; // The IV, if required by the mode
    size_t lenPlain = strlen((const char *)plaintext);
    size_t outLen;

    // Encrypt
    uint8_t* encrypted = (uint8_t*)crypto_des_encrypt(plaintext, lenPlain, key, iv, CRYPTO_MODE_OFB, &outLen);
    if (!encrypted) {
        perror("Encryption failed");
        return 1;
    }

    printf("Encrypted text: ");
    crypto_print_hash(encrypted, outLen);

    // Decrypt
    uint8_t* decrypted = (uint8_t*)crypto_des_decrypt(encrypted, outLen, key, iv, CRYPTO_MODE_OFB, &outLen);
    if (!decrypted) {
        perror("Decryption failed");
        free(encrypted); // Remember to free the allocated memory
        return 1;
    }

    printf("Decrypted text: ");
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

## Example 13 : How to use `ECB` mode with `crypto_des_encrypt`

```c

#include "crypto/crypto.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

int main() {
    const uint8_t key[8] = "yourkey"; // 8 bytes key
    const uint8_t plaintext[] = "Hello World"; 
    size_t outLen;

    // Encrypt with ECB
    uint8_t* encrypted_ecb = (uint8_t*)crypto_des_encrypt(plaintext, strlen((const char *)plaintext), key, NULL, CRYPTO_MODE_ECB, &outLen);
    printf("ECB Encrypted: ");
    crypto_print_hash(encrypted_ecb, outLen);

    // Decrypt with ECB
    uint8_t* decrypted_ecb = (uint8_t*)crypto_des_decrypt(encrypted_ecb, outLen, key, NULL, CRYPTO_MODE_ECB, &outLen);
    printf("ECB Decrypted: %s\n", decrypted_ecb);

    free(encrypted_ecb);
    free(decrypted_ecb);
    return 0;
}

```

## Example 14 : Generate random IV for `CBC` mode `crypto_des_encrypt`

```c
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "crypto/crypto.h"

int main() {
    const uint8_t key[8] = "yourkey"; // 8 bytes key
    uint8_t iv[DES_BLOCK_SIZE]; // IV for CBC mode
    crypto_generate_random_iv(iv, DES_BLOCK_SIZE); // Generate a random IV for testing
    
    const uint8_t plaintext[] = "Hello World"; // Your plaintext
    size_t outLen;

    // Encrypt with CBC
    uint8_t* encrypted_cbc = (uint8_t*)crypto_des_encrypt(plaintext, strlen((const char *)plaintext), key, iv, CRYPTO_MODE_CBC, &outLen);
    printf("CBC Encrypted: ");
    crypto_print_hash(encrypted_cbc, outLen);

    // Decrypt with CBC
    uint8_t* decrypted_cbc = (uint8_t*)crypto_des_decrypt(encrypted_cbc, outLen, key, iv, CRYPTO_MODE_CBC, &outLen);
    printf("CBC Decrypted: %s\n", decrypted_cbc);

    // Cleanup
    free(encrypted_cbc);
    free(decrypted_cbc);

    return 0;
}

```