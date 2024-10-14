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

- `int secrets_randbelow(int exclusive_upper_bound)`: Generates a secure random integer in the range `[0, exclusive_upper_bound)`.
- `int secrets_randbits(int k)`: Generates a secure random integer with `k` bits of randomness.
- `void* secrets_token_bytes(size_t nbytes)`: Generates a secure random byte sequence of `nbytes` length.
- `char* secrets_token_hex(size_t nbytes)`: Generates a secure random hexadecimal string representation of `nbytes` bytes.
- `char* secrets_token_urlsafe(size_t nbytes)`: Generates a secure random URL-safe Base64-encoded string of `nbytes` bytes.
- `int secrets_compare_digest(const void* a, const void* b, size_t len)`: Performs a constant-time comparison of two byte sequences to prevent timing attacks.
- `void* secrets_choice(const void* seq, size_t size, size_t elem_size)`: Select a random element from a sequence (array) using cryptographically secure randomness.
- `unsigned int secrets_randbits(int k)`: Generate a non-negative integer with exactly k random bits.

## Usage Examples

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