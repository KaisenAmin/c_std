# Crypto Library in C

Author: Amin Tahmasebi  
Release Date: 2024  
License: GNU General Public License v3.0  

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

### Supported Algorithms
- `CRYPTO_MD4`: Computes an MD4 hash digest.
- `CRYPTO_MD5`: Computes an MD5 hash digest.
- `CRYPTO_SHA1`: Computes an SHA-1 hash digest.
- `CRYPTO_SHA224`: Computes an SHA-224 hash digest.
- `CRYPTO_SHA256`: Computes an SHA-256 hash digest.
- `CRYPTO_SHA384`: Computes an SHA-384 hash digest.
- `CRYPTO_SHA512`: Computes an SHA-512 hash digest.
- `CRYPTO_SHA3_224`: Placeholder for an SHA3-224 hash digest.
- `CRYPTO_SHA3_256`: Placeholder for an SHA3-256 hash digest.
- `CRYPTO_SHA3_384`: Placeholder for an SHA3-384 hash digest.
- `CRYPTO_SHA3_512`: Placeholder for an SHA3-512 hash digest.
- `CRYPTO_KECCAK_224`: Placeholder for a Keccak-224 hash digest.
- `CRYPTO_KECCAK_256`: Placeholder for a Keccak-256 hash digest.
- `CRYPTO_KECCAK_384`: Placeholder for a Keccak-384 hash digest.
- `CRYPTO_KECCAK_512`: Placeholder for a Keccak-512 hash digest.

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