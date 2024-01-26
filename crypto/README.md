# Crypto Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License  

## Overview

The Crypto library in C is a lightweight and versatile cryptographic hashing library for C programming, providing an interface for creating a variety of hash digests. It is designed to be a counterpart to the cryptographic capabilities found in higher-level languages like C++ and Python, offering C developers a straightforward approach to securing information.

This library serves as a bridge for developers familiar with Qt's `QCryptographicHash` class or similar high-level cryptographic APIs, allowing the use of these in a C environment.

## Key Features

- **Multiple Hashing Algorithms:** Supports a range of hashing algorithms including MD4, MD5, SHA-1, SHA-2 (SHA-224, SHA-256, SHA-384, SHA-512), and placeholders for SHA-3 and Keccak variants.
- **Easy to Use:** Simplifies the hashing process with a straightforward API.
- **Memory Management:** Allocates and frees memory dynamically to handle hash data, ensuring flexibility with data sizes.
- **OpenSSL Integration:** Utilizes the robust OpenSSL library for cryptographic operations.

## Installation and Compilation

To use the Crypto library, include `crypto.h` in your project. You must have OpenSSL installed on your system as this library depends on it for the cryptographic functions.

Compile your C files using a command similar to:

```bash
gcc -std=c11 -O3 -Wall -Wextra -o mycrypto main.c crypto.c -lcrypto
```

Ensure that your development environment is set up with the necessary tools and libraries, such as GCC and OpenSSL.

## Documentation

Detailed documentation is provided within the header files, outlining the purpose and usage of each function, as well as the supported hashing algorithms. The documentation is intended to help users quickly integrate the library's functionalities into their projects.


## Function Descriptions

### Hashing Operations
- `crypto_hash_data(const uint8_t*, size_t, enum HashAlgorithm, size_t*)`: Computes a hash digest based on the selected algorithm for the provided data.
- `crypto_print_hash(const uint8_t*, size_t)`: Prints the hash digest in a hexadecimal format.

To include the additional hash algorithms you've implemented in your Crypto library documentation, you can expand the "Supported Algorithms" section as follows:

### Supported Algorithms for hashing
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

### Compilation
```bash
gcc -std=c17 -o mycrypto main.c crypto.c -lcrypto
```

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