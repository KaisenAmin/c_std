# Secrets Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

The Secrets library in C provides cryptographically secure random number generation for applications that require security features, such as managing secrets, passwords, authentication tokens, and other sensitive data. This library should be used when security and cryptographic strength are required, as opposed to standard random number generation.

This library is designed to be easy to integrate into C projects and provides essential functionality for secure random number generation, equivalent to Python’s `secrets` module. It is fully cross-platform, working on both Linux and Windows.

## Key Features

- **Secure Random Integer Generation**: Generates secure random integers within specified ranges.
- **Secure Random Bits**: Generates random bits securely.
- **Token Generation**: Functions to generate random tokens in different formats, such as hexadecimal, byte arrays, and URL-safe tokens.
- **Constant-Time Comparison**: Provides secure comparison functions to mitigate timing attacks.

## Installation and Compilation

To use this library, include `secrets.h` in your project and compile the source files with your C compiler. For GCC, the following command can be used:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./secrets/secrets.c
```

## Documentation

The documentation includes detailed descriptions of all the functions provided by the library, along with their usage examples. It covers secure token generation and secure random number generation.

### Function Descriptions

### `void secrets_token_bytes(unsigned char *buffer, size_t size)`

- **Purpose**: Generate cryptographically secure random bytes.
- **Parameters**:
  - `buffer`: Pointer to the byte array where random bytes will be stored.
  - `size`: Number of bytes to generate.
- **Return**: No return value.
- **Details**:
  - This function generates random bytes using platform-specific methods.
  - On **Windows**, it uses `BCryptGenRandom`, and on **Unix-based systems**, it reads from `/dev/urandom`.
  - If any error occurs during random byte generation, the function logs the error and exits the program.
  - The generated random bytes are stored in the provided `buffer`.

---

### `int secrets_randbelow(int n)`

- **Purpose**: Generate a cryptographically secure random integer in the range [0, n).
- **Parameters**:
  - `n`: Upper limit (exclusive).
- **Return**: A cryptographically secure random integer between 0 and `n-1`.
- **Details**:
  - This function first calls `secrets_token_bytes` to generate random bytes.
  - It converts the bytes to an integer and then calculates a random number in the range `[0, n)` using modulo operation.
  - Logs both the input and the generated random number.

---

### `void secrets_token_hex(char *buffer, size_t nbytes)`

- **Purpose**: Generate a cryptographically secure random token in hexadecimal format.
- **Parameters**:
  - `buffer`: Pointer to the array where the hexadecimal token will be stored.
  - `nbytes`: Number of bytes to generate, each byte is converted to two hex digits.
- **Return**: No return value.
- **Details**:
  - The function generates `nbytes` random bytes using `secrets_token_bytes`.
  - It converts each byte to its hexadecimal representation (two characters per byte).
  - The result is stored in the provided `buffer`.
  - Allocates memory dynamically for the random bytes and frees it after use.

---

### `void secrets_token_urlsafe(char *buffer, size_t nbytes)`

- **Purpose**: Generate a cryptographically secure random URL-safe token.
- **Parameters**:
  - `buffer`: A pointer to the array where the Base64-encoded URL-safe token will be stored.
  - `nbytes`: The number of random bytes to encode as a URL-safe token.
- **Return**: No return value.
- **Details**:
  - This function generates `nbytes` random bytes using `secrets_token_bytes`.
  - It converts each byte to a Base64 URL-safe character using a predefined table (`A-Z`, `a-z`, `0-9`, `-`, and `_`).
  - The resulting token is stored in the provided `buffer`.
  - Allocates memory dynamically for the random bytes and frees it after use.

---

### `int secrets_compare_digest(const unsigned char *a, const unsigned char *b, size_t length)`

- **Purpose**: Compare two byte sequences in constant time to prevent timing attacks.
- **Parameters**:
  - `a`: Pointer to the first byte sequence.
  - `b`: Pointer to the second byte sequence.
  - `length`: The number of bytes to compare.
- **Return**: `1` if the sequences are equal, `0` otherwise.
- **Details**:
  - This function compares two byte sequences in constant time to avoid timing attacks. It loops over the sequences and performs a bitwise XOR for each byte, accumulating the result.
  - Even if the sequences differ early on, the function continues to process the entire length, ensuring the time taken does not depend on when the difference is found.
  - Logs the comparison result (`1` for equal, `0` for unequal).

---

### `void* secrets_choice(const void* seq, size_t size, size_t elem_size)`

- **Purpose**: Select a random element from a sequence (array) using cryptographically secure randomness.
- **Parameters**:
  - `seq`: Pointer to the sequence (array) of elements.
  - `size`: The number of elements in the sequence.
  - `elem_size`: Size of each element in bytes.
- **Return**: A pointer to the randomly selected element or `NULL` if the sequence is empty.
- **Details**:
  - This function chooses a random element from a sequence using cryptographically secure randomness.
  - It first checks if the sequence is empty (if `size == 0`) and logs an error if so.
  - It uses `secrets_randbelow` to generate a random index and returns the element at the calculated index.
  - Logs the randomly selected index.

---

### `unsigned int secrets_randbits(int k)`

- **Purpose**: Generate a non-negative integer with exactly `k` random bits.
- **Parameters**:
  - `k`: The number of random bits to generate (must be between 1 and 32).
- **Return**: A random integer with exactly `k` random bits.
- **Details**:
  - This function generates a random integer with `k` bits using cryptographically secure randomness.
  - If `k` is outside the valid range (1 to 32), it logs an error and exits the program.
  - It calls `secrets_token_bytes` to generate random bytes, masks the extra bits using `(1U << k) - 1`, and returns the result.
  - Logs the number of bits requested and the generated random value.

---

## Examples

## Example 1 : how to use `secrets_choice`

```c
#include "fmt/fmt.h"
#include "secrets/secrets.h"

int main() {
    int int_array[] = {10, 20, 30, 40, 50};
    size_t int_size = sizeof(int_array) / sizeof(int_array[0]);

    int *random_int = (int *)secrets_choice(int_array, int_size, sizeof(int));
    fmt_printf("Randomly chosen integer: %d\n", *random_int);

    const char* string_array[] = {"apple", "banana", "cherry", "date", "elderberry"};
    size_t string_size = sizeof(string_array) / sizeof(string_array[0]);

    const char **random_string = (const char **)secrets_choice(string_array, string_size, sizeof(char *));
    fmt_printf("Randomly chosen string: %s\n", *random_string);

    return 0;
}
```
**Result is Random**
```
Randomly chosen integer: 40
Randomly chosen string: cherry
```

---

## Example 2 : Generate random bits with `secrets_randbits`

```c
#include "fmt/fmt.h"
#include "secrets/secrets.h"

int main() {
    // Generate a random integer with 8 random bits
    unsigned int random_8bits = secrets_randbits(8);
    fmt_printf("Random 8-bit integer: %lu\n", random_8bits);

    // Generate a random integer with 16 random bits
    unsigned int random_16bits = secrets_randbits(16);
    fmt_printf("Random 16-bit integer: %lu\n", random_16bits);

    // Generate a random integer with 32 random bits
    unsigned int random_32bits = secrets_randbits(32);
    fmt_printf("Random 32-bit integer: %lu\n", random_32bits);

    return 0;
}
```
**Result is Random**
```
Random 8-bit integer: 135
Random 16-bit integer: 15441
Random 32-bit integer: 2482988267
```

---

## Example 3 : Generate Secure password with std_string and secrets libs 

```c
#include "fmt/fmt.h"
#include "secrets/secrets.h"
#include "string/std_string.h"
#include <string.h>
#include <stdlib.h>

#define PASSWORD_LENGTH 8

int main() {
    size_t ascii_len = string_length_cstr(STRING_ASCII_LETTERS);
    size_t digits_len = string_length_cstr(STRING_DIGITS);
    char* alphabet = (char*) malloc(sizeof(char) * (ascii_len + digits_len));

    if (!alphabet) {
        fmt_fprintf(stderr, "Error: Can not allocate memory");
        exit(EXIT_FAILURE);
    }

    strcpy(alphabet, STRING_ASCII_LETTERS);
    strcat(alphabet, STRING_DIGITS);
    
    char password[PASSWORD_LENGTH + 1];  
    size_t alphabet_len = string_length_cstr(alphabet);

    for (size_t i = 0; i < PASSWORD_LENGTH; i++) {
        char* random_char = (char*)secrets_choice(alphabet, alphabet_len, sizeof(char));
        password[i] = *random_char;
    }
    
    password[PASSWORD_LENGTH] = '\0'; 
    fmt_printf("Generated Password: %s\n", password);

    free(alphabet);
    return 0;
}
```
**Result is Random**
```
Generated Password: 6MheGa42
```

---

## Example 4: Generate a ten-character alphanumeric password with at least one lowercase character, at least one uppercase character, and at least three digits

```c
#include "fmt/fmt.h"
#include "secrets/secrets.h"
#include "string/std_string.h"
#include <ctype.h> 
#include <stdlib.h>
#include <string.h>

#define PASSWORD_LENGTH 10
#define MIN_DIGITS 3

bool check_password_requirements(const char* password) {
    int has_lower = 0, has_upper = 0, digit_count = 0;

    for (size_t i = 0; i < PASSWORD_LENGTH; i++) {
        if (islower(password[i])) {
            has_lower = 1;
        }
        if (isupper(password[i])) {
            has_upper = 1;
        }
        if (isdigit(password[i])) {
            digit_count++;
        }
    }

    return (has_lower && has_upper && digit_count >= MIN_DIGITS);
}

int main() {
    size_t ascii_len = string_length_cstr(STRING_ASCII_LETTERS);
    size_t digits_len = string_length_cstr(STRING_DIGITS);
    char* alphabet = (char*) malloc(sizeof(char) * (ascii_len + digits_len + 1));

    if (!alphabet) {
        fmt_fprintf(stderr, "Error: Cannot allocate memory\n");
        exit(EXIT_FAILURE);
    }

    strcpy(alphabet, STRING_ASCII_LETTERS);
    strcat(alphabet, STRING_DIGITS);

    char password[PASSWORD_LENGTH + 1];  
    size_t alphabet_len = string_length_cstr(alphabet);

    while (1) {
        for (size_t i = 0; i < PASSWORD_LENGTH; i++) {
            char* random_char = (char*)secrets_choice(alphabet, alphabet_len, sizeof(char));
            password[i] = *random_char;
        }
        password[PASSWORD_LENGTH] = '\0'; 

        if (check_password_requirements(password)) {
            break;
        }
    }

    fmt_printf("Generated Password: %s\n", password);

    free(alphabet);
    return 0;
}
```
**Result is Random**
```
Generated Password: Q35HIAJbu9
```

---

## Example 5: Generate a hard-to-guess temporary URL containing a security token suitable for password recovery applications

```c
#include "fmt/fmt.h"
#include "secrets/secrets.h"
#include <string.h>
#include <stdlib.h>

#define TOKEN_BYTES 16
#define URL_BASE "https://example.com/reset="

int main() {
    char token[TOKEN_BYTES + 1];

    secrets_token_urlsafe(token, TOKEN_BYTES);
    token[TOKEN_BYTES] = '\0'; 

    size_t url_length = strlen(URL_BASE) + TOKEN_BYTES + 1;
    char* full_url = (char*) malloc(url_length * sizeof(char));

    if (!full_url) {
        fmt_fprintf(stderr, "Error: Cannot allocate memory for the URL\n");
        exit(EXIT_FAILURE);
    }

    strcpy(full_url, URL_BASE);
    strcat(full_url, token);

    fmt_printf("Generated URL: %s\n", full_url);

    free(full_url);
    return 0;
}
```
**Result is Random**
```
Generated URL: https://example.com/reset=FcChO2MzCwkkgwPf
```

---

## Example 6: securely compare two byte sequences in constant time with `secrets_compare_digest`

```c
#include "fmt/fmt.h"
#include "secrets/secrets.h"


int main() {
    const unsigned char digest1[] = {0x5a, 0xf4, 0x1e, 0x9c, 0x12, 0x56, 0xa3, 0xb9};
    const unsigned char digest2[] = {0x5a, 0xf4, 0x1e, 0x9c, 0x12, 0x56, 0xa3, 0xb9}; 
    const unsigned char digest3[] = {0x6b, 0xa3, 0x5c, 0x9d, 0x12, 0x57, 0xa4, 0xb8};  

    size_t length = sizeof(digest1);  
    int result1 = secrets_compare_digest(digest1, digest2, length);

    if (result1) {
        fmt_printf("digest1 and digest2 are equal.\n");
    } 
    else {
        fmt_printf("digest1 and digest2 are not equal.\n");
    }

    int result2 = secrets_compare_digest(digest1, digest3, length);
    if (result2) {
        fmt_printf("digest1 and digest3 are equal.\n");
    } 
    else {
        fmt_printf("digest1 and digest3 are not equal.\n");
    }

    return 0;
}
```
**Result**
```
digest1 and digest2 are equal.
digest1 and digest3 are not equal.
```

---

## Example 7: how to generate a cryptographically secure random byte array with `secrets_token_bytes`

```c
#include "fmt/fmt.h"
#include "secrets/secrets.h"

#define TOKEN_BYTES 16 


int main() {
    unsigned char buffer[TOKEN_BYTES];  

    secrets_token_bytes(buffer, TOKEN_BYTES);
    fmt_printf("Generated Random Bytes: ");

    for (size_t i = 0; i < TOKEN_BYTES; i++) {
        fmt_printf("%02x", buffer[i]);
    }
    fmt_printf("\n");

    return 0;
}
```
**Result is Random**
```
Generated Random Bytes: a888fa763cb53abb443e38474efa7a46
```

---

## Example 8 : how to generate a cryptographically secure random token in hexadecimal format with `secrets_token_hex`

```c
#include "fmt/fmt.h"
#include "secrets/secrets.h"
#include <stdlib.h>

#define TOKEN_BYTES 16 

int main() {
    char* hex_token = (char*)malloc((TOKEN_BYTES * 2) + 1);  // Allocate memory for the hex token (each byte = 2 hex digits)

    if (!hex_token) {
        fmt_fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    secrets_token_hex(hex_token, TOKEN_BYTES);
    fmt_printf("Generated Hex Token: %s\n", hex_token);

    free(hex_token);
    return 0;
}
```
**Result is Random**
```
Generated Hex Token: e257327c82d4a296c66c48a16f8d78a9
```

---

## Example 9 : generate cryptographically secure random integers below a specified upper bound with `secrets_randbelow`

```c
#include "fmt/fmt.h"
#include "secrets/secrets.h"

int main() {
    int upper_bound = 100; 
    int random_value = secrets_randbelow(upper_bound);

    fmt_printf("Random number below %d: %d\n", upper_bound, random_value);

    return 0;
}
```
**Result is Random**
```
Random number below 100: 19
```

---

## Example 10 : Generate 5 secrets passwords with some conditions and use Vector and String for save them 

```c
#include "fmt/fmt.h"
#include "secrets/secrets.h"
#include "vector/vector.h"
#include "string/std_string.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define PASSWORD_LENGTH 10
#define NUM_PASSWORDS 5

bool check_password_requirements(const char* password) {
    int has_lower = 0, has_upper = 0, digit_count = 0;

    for (size_t i = 0; i < PASSWORD_LENGTH; i++) {
        if (islower(password[i])) {
            has_lower = 1;
        }
        if (isupper(password[i])) {
            has_upper = 1;
        }
        if (isdigit(password[i])) {
            digit_count++;
        }
    }

    return (has_lower && has_upper && digit_count >= 3);
}

int main() {
    Vector* passwords = vector_create(sizeof(String*));
    size_t ascii_len = string_length_cstr(STRING_ASCII_LETTERS);
    size_t digits_len = string_length_cstr(STRING_DIGITS);
    char* alphabet = (char*)malloc(sizeof(char) * (ascii_len + digits_len + 1));

    if (!alphabet) {
        fmt_fprintf(stderr, "Error: Cannot allocate memory\n");
        exit(EXIT_FAILURE);
    }

    strcpy(alphabet, STRING_ASCII_LETTERS);
    strcat(alphabet, STRING_DIGITS);

    size_t alphabet_len = string_length_cstr(alphabet);

    for (size_t i = 0; i < NUM_PASSWORDS; i++) {
        String* password = string_create("");

        while (1) {
            string_clear(password);

            for (size_t j = 0; j < PASSWORD_LENGTH; j++) {
                char* random_char = (char*)secrets_choice(alphabet, alphabet_len, sizeof(char));
                string_push_back(password, *random_char);
            }
            if (check_password_requirements(string_c_str(password))) {
                break; 
            }
        }

        vector_push_back(passwords, &password);
    }

    fmt_printf("Generated Secure Passwords:\n");
    for (size_t i = 0; i < vector_size(passwords); i++) {
        String** password = (String**)vector_at(passwords, i);
        fmt_printf("%zu: %s\n", i + 1, string_c_str(*password));
    }

    for (size_t i = 0; i < vector_size(passwords); i++) {
        String** password = (String**)vector_at(passwords, i);
        string_deallocate(*password);
    }

    vector_deallocate(passwords);
    free(alphabet);
    return 0;
}
```
**Result is Random**
```
1: zKkI2U7I2A
2: s2s2A2qrBF
3: jape59f9Vt
4: i52cU6v7aU
5: mAH6q3cri9
```

---

## License

This project is open-source and available under [ISC License].