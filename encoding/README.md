# Encoding Library in C

**Author:** amin tahmasebi
**Release Date:** 2024
**License:** ISC License


The Encoding library in C is a versatile tool designed for efficient and user-friendly encoding and decoding operations. While it currently supports Base64 encoding and decoding, the library is structured to accommodate additional encoding methods in the future. The Base64 functionality allows for conversion between binary data and a character set comprising 64 different characters, making it ideal for data storage and transfer over media primarily handling textual content. The library's extensible architecture ensures it can be adapted to include other encoding types such as Base16, Base32, and URL encoding, catering to a wide range of applications and data formats.

## Compilation

To compile the Encoding library with your main program, you can use the following GCC command. Make sure to include any other necessary library files:

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main main.c encoding/encoding.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as indicated in the project.

## Usage

To use the Encoding library in your project, include the `encoding.h` header file in your source code:

```c
#include "encoding/encoding.h"
```

## Function Descriptions

### Base64 Encoding and Decoding
- `encoding_base64_encode`: Encodes a given string into Base64 format. It takes a pointer to the input string and its length, returning a dynamically allocated encoded string.

- `encoding_base64_decode`: Decodes a Base64 encoded string back to its original format. It takes a pointer to the encoded string and its length, returning a dynamically allocated decoded string.

Certainly! Here are brief explanations for the URL encode and decode functions to add to your function descriptions:

### URL Encoding and Decoding
- `encoding_url_encode`: Encodes a given string into a URL-friendly format. It converts special characters into percent-encoded representations, taking a pointer to the input string and its length, and returns a dynamically allocated encoded string.

- `encoding_url_decode`: Decodes a percent-encoded URL string back to its original format. It interprets percent-encoded characters and converts them back to their original representations, taking a pointer to the encoded string and its length, and returns a dynamically allocated decoded string.

### Base32 Encoding and Decoding
- `encoding_base32_encode`: This function encodes a given string into Base32 format. It takes a pointer to the input string and its length. The function returns a dynamically allocated string containing the Base32 encoded representation. Base32 encoding is useful for cases where case-insensitive encoding is beneficial, such as in email and URL encoding.

- `encoding_base32_decode`: Decodes a Base32 encoded string back to its original format. It takes a pointer to the encoded string and its length, returning a dynamically allocated decoded string. The function handles the Base32 decoding process, converting the encoded characters back to the original binary data.

### Base16 (Hex) Encoding and Decoding
- `encoding_base16_encode`: Encodes a given string into Base16 (hexadecimal) format. It accepts a pointer to the input string and its length, returning a dynamically allocated string with the Base16 encoded representation. Base16 encoding represents binary data in an ASCII string format by translating each byte of data into two hexadecimal digits.

- `encoding_base16_decode`: Decodes a Base16 (hexadecimal) encoded string back to its original binary form. It requires a pointer to the encoded string and its length, and returns a dynamically allocated decoded string. This function is useful for converting data encoded in hex back to its original binary state.


## Example 1: Base64 Encoding and Decoding
This example demonstrates encoding a string to Base64 and then decoding it back to its original format.

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* text = "Hello, World!";
    char* encoded = encoding_base64_encode(text, strlen(text));
    
    if (encoded) {
        fmt_printf("Encoded: %s\n", encoded);

        char* decoded = encoding_base64_decode(encoded, strlen(encoded));
        if (decoded) {
            fmt_printf("Decoded: %s\n", decoded);
            free(decoded);
        }
        free(encoded);
    }
    return 0;
}
```
### Example 2: URL Encoding

In this example, we will encode a URL or a string with special characters using `encoding_url_encode`.

```c
#include "encoding/encoding.h"
#include "string/string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    String *text_to_encode = string_create("Hello World! This is a test/example?");
    char* encoded = encoding_url_encode(string_c_str(text_to_encode), string_length(text_to_encode));

    if (encoded) {
        fmt_printf("Original: %s\n", string_c_str(text_to_encode));
        fmt_printf("URL Encoded: %s\n", encoded);
        free(encoded);
    } 
    else {
        fmt_printf("Failed to encode URL.\n");
    }

    string_deallocate(text_to_encode);
    return 0;
}
```

### Example 3: URL Decoding `encoding_url_decode`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"

int main() {
    const char* text_to_decode = "Hello%20World%21%20This%20is%20a%20test%2Fexample%3F";
    char* decoded = encoding_url_decode(text_to_decode, strlen(text_to_decode));
    if (decoded) {
        fmt_printf("URL Encoded: %s\n", text_to_decode);
        fmt_printf("Decoded: %s\n", decoded);
        free(decoded);
    } 
    else {
        fmt_printf("Failed to decode URL.\n");
    }
    return 0;
}
```

## Examples 4 : use `encoding_base32_encode`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* text_to_encode = "Hello, World!";
    char* encoded = encoding_base32_encode(text_to_encode, strlen(text_to_encode));

    if (encoded) {
        fmt_printf("Original: %s\n", text_to_encode);
        fmt_printf("Base32 Encoded: %s\n", encoded);
        free(encoded);
    } 
    else {
        fmt_printf("Failed to encode in Base32.\n");
    }
    return 0;
}
```

## Example 5 : use `encoding_base32_decode`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* text_to_decode = "ORSXG5A=";  // Base32 encoded version of "test"
    char* decoded = encoding_base32_decode(text_to_decode, strlen(text_to_decode));

    if (decoded) {
        fmt_printf("Base32 Encoded: %s\n", text_to_decode);
        fmt_printf("Decoded: %s\n", decoded);
        free(decoded);
    } else {
        fmt_printf("Failed to decode from Base32.\n");
    }
    return 0;
}
```

## Example 6 : use `encoding_base16_encode`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* text = "Hello, World!";
    char* encoded = encoding_base16_encode(text, strlen(text));

    if (encoded) {
        fmt_printf("Original: %s\n", text);
        fmt_printf("Base16 Encoded: %s\n", encoded);
        free(encoded);
    }
    return 0;
}
```

## Example 7 : use `encoding_base32_decode`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* encodedText = "48656C6C6F2C20576F726C6421";
    char* decoded = encoding_base16_decode(encodedText, strlen(encodedText));
    
    if (decoded) {
        fmt_printf("Base16 Encoded: %s\n", encodedText);
        fmt_printf("Decoded: %s\n", decoded);
        free(decoded);
    }
    return 0;
}
```

## Example 8 : use `encoding_utf32_to_utf16`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    // UTF-32 string containing emojis
    uint32_t utf32_string[] = {0x1F600, 0x1F603, 0x1F604, 0x1F606, 0x1F609, 0x1F60A, 0x0000}; // Null-terminated

    size_t length = 0;
    while (utf32_string[length] != 0) {
        length++;
    }

    uint16_t* utf16_string = encoding_utf32_to_utf16(utf32_string, length);

    // Print UTF-16 string as hexadecimal values
    fmt_printf("UTF-16 String: ");
    for (size_t i = 0; utf16_string[i] != 0; i++) {
        fmt_printf("%04X ", utf16_string[i]);
    }
    fmt_printf("\\n");

    free(utf16_string);
    return 0;
}
```

## Example 9 : convert utf16 to utf32 with `encoding_utf16_to_utf32`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>


int main() {
    // UTF-32 string containing emojis
    uint32_t utf32_string[] = {0x1F600, 0x1F603, 0x1F604, 0x1F606, 0x1F609, 0x1F60A, 0x0000}; // Null-terminated

    size_t length = 0;
    while (utf32_string[length] != 0) {
        length++;
    }

    uint16_t* utf16_string = encoding_utf32_to_utf16(utf32_string, length);
    size_t index = 0;
    while (utf16_string[index] != 0) {
        index++;
    }
    // Print UTF-16 string as hexadecimal values
    fmt_printf("UTF-16 String: ");
    for (size_t i = 0; utf16_string[i] != 0; i++) {
        fmt_printf("%04X ", utf16_string[i]);
    }
    fmt_printf("\n");

    uint32_t* calc = encoding_utf16_to_utf32(utf16_string, index);

    fmt_printf("UTF-32 String: ");
    for (size_t i = 0; calc[i] != 0; i++) {
        fmt_printf("%0X ", calc[i]);
    }

    free(utf16_string);
    return 0;
}
```

### Example 10: Using `encoding_utf16_to_utf8`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    // UTF-16 encoded string (Example: "Hello World")
    const uint16_t utf16_string[] = {0x0048, 0x0065, 0x006C, 0x006C, 0x006F, 0x0020, 0x0057, 0x006F, 0x0072, 0x006C, 0x0064, 0x0000};
    size_t length = sizeof(utf16_string) / sizeof(uint16_t) - 1; // Exclude null terminator
    uint8_t* utf8_string = encoding_utf16_to_utf8(utf16_string, length);

    if (utf8_string) {
        fmt_printf("UTF-8 Encoded String: %s\n", utf8_string);
        free(utf8_string); 
    } 
    else {
        fmt_printf("Failed to encode UTF-16 to UTF-8.\n");
    }
    return 0;
}
```

### Example 11: Using `encoding_utf32_to_utf8`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    // UTF-32 encoded string (Example: "Hello World")
    const uint32_t utf32_string[] = {0x0000048, 0x0000065, 0x000006C, 0x000006C, 0x000006F, 0x0000020, 0x0000057, 0x000006F, 0x0000072, 0x000006C, 0x0000064, 0x0000000};
    size_t length = sizeof(utf32_string) / sizeof(uint32_t) - 1; // Exclude null terminator
    uint8_t* utf8_string = encoding_utf32_to_utf8(utf32_string, length);

    if (utf8_string) {
        fmt_printf("UTF-8 Encoded String: %s\n", utf8_string);
        free(utf8_string); 
    } 
    else {
        fmt_printf("Failed to encode UTF-32 to UTF-8.\n");
    }
    return 0;
}
```

## Example 12 : Check data is valid UTF-8 or not with `encoding_is_utf8`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    // Example 1: Valid UTF-8 string
    const uint8_t valid_utf8[] = {0xE2, 0x82, 0xAC}; // € symbol in UTF-8
    size_t length1 = sizeof(valid_utf8);

    if (encoding_is_utf8(valid_utf8, length1)) {
        fmt_printf("String 1 is valid UTF-8.\n");
    } 
    else {
        fmt_printf("String 1 is not valid UTF-8.\n");
    }

    // Example 2: Invalid UTF-8 string
    const uint8_t invalid_utf8[] = {0xE2, 0x28, 0xA1}; // Invalid sequence
    size_t length2 = sizeof(invalid_utf8);

    if (encoding_is_utf8(invalid_utf8, length2)) {
        fmt_printf("String 2 is valid UTF-8.\n");
    } 
    else {
        fmt_printf("String 2 is not valid UTF-8.\n");
    }

    // Example 3: ASCII string (also valid UTF-8)
    const uint8_t ascii_utf8[] = "Hello, World!";
    size_t length3 = sizeof(ascii_utf8) - 1; // Exclude null terminator

    if (encoding_is_utf8(ascii_utf8, length3)) {
        fmt_printf("String 3 is valid UTF-8.\n");
    } 
    else {
        fmt_printf("String 3 is not valid UTF-8.\n");
    }
    return 0;
}
```
## Example 13 : Check data is valid UTF-8 String or not with `encoding_is_utf8_string`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"

int main() {
    // Example 1: Valid UTF-8 string
    const uint8_t valid_utf8[] = {0xE2, 0x82, 0xAC, 0}; // € symbol in UTF-8
    const uint8_t* valid_ptr = valid_utf8;

    if (encoding_is_utf8_string(&valid_ptr, sizeof(valid_utf8))) {
        fmt_printf("Example 1 is valid UTF-8.\n");
    } 
    else {
        fmt_printf("Example 1 is not valid UTF-8.\n");
    }

    // Example 2: Invalid UTF-8 string
    const uint8_t invalid_utf8[] = {0xE2, 0x28, 0xA1, 0}; // Invalid sequence
    const uint8_t* invalid_ptr = invalid_utf8;

    if (encoding_is_utf8_string(&invalid_ptr, sizeof(invalid_utf8))) {
        fmt_printf("Example 2 is valid UTF-8.\n");
    } 
    else {
        fmt_printf("Example 2 is not valid UTF-8.\n");
    }

    // Example 3: ASCII string (also valid UTF-8)
    const uint8_t ascii_utf8[] = "Hello, World!"; // ASCII is a subset of UTF-8
    const uint8_t* ascii_ptr = ascii_utf8;

    if (encoding_is_utf8_string(&ascii_ptr, sizeof(ascii_utf8) - 1)) { // -1 to exclude null terminator
        fmt_printf("Example 3 is valid UTF-8.\n");
    } 
    else {
        fmt_printf("Example 3 is not valid UTF-8.\n");
    }
    return 0;
}
```

## Example 14 : `encoding_utf8_to_utf16`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    // UTF-8 encoded string
    const uint8_t utf_8_string[] = "Hello, UTF-16! 😊";
    size_t utf_8_length = sizeof(utf_8_string) - 1; // Exclude null terminator
    uint16_t* utf_16_string = encoding_utf8_to_utf16(utf_8_string, utf_8_length);

    if (utf_16_string) {
        fmt_printf("UTF-16 Encoded String: ");
        for (size_t i = 0; utf_16_string[i] != 0; ++i) {
            fmt_printf("%04X ", utf_16_string[i]);
        }
        fmt_printf("\n");
        free(utf_16_string);
    } 
    else {
        fmt_printf("Failed to convert to UTF-16.\n");
    }
    return 0;
}
```

### Example 15 :  `encoding_utf8_to_utf32`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    // UTF-8 encoded string
    const uint8_t utf_8_string[] = "Hello, UTF-32! 🌍";
    size_t utf_8_length = sizeof(utf_8_string) - 1; // Exclude null terminator
    uint32_t* utf_32_string = encoding_utf8_to_utf32(utf_8_string, utf_8_length);
    
    if (utf_32_string) {
        fmt_printf("UTF-32 Encoded String: ");
        for (size_t i = 0; utf_32_string[i] != 0; ++i) {
            fmt_printf("%08X ", utf_32_string[i]);
        }
        fmt_printf("\n");
        free(utf_32_string);
    } 
    else {
        fmt_printf("Failed to convert to UTF-32.\n");
    }
    return 0;
}
```

## Example 16 : `encoding_hex_dump`

```c
#include "encoding/encoding.h"

int main() {
    const char *data = "amin";
    size_t data_size = sizeof("amin") - 1;

    encoding_hex_dump(data, data_size);
    
    return 0;
}
```

## Example 17 : encode and decode data with Using `encoding_base85_encode` and `encoding_base85_decode`

```c
#include <stdlib.h>
#include "fmt/fmt.h"
#include "encoding/encoding.h"

int main() {
    const uint8_t data[] = "Hello one";
    size_t data_size = sizeof(data) - 1; // Exclude null terminator
    char* encoded = encododing_base85_encode(data, data_size);

    if (encoded) {
        fmt_printf("Encoded ASCII85: %s\n", encoded);

        const size_t input_len = strlen(encoded);
        uint8_t* decoded_result = encododing_base85_decode(encoded, input_len);
        if (decoded_result) {
            fmt_printf("Decoded: %s\n", decoded_result);
            free(decoded_result);
        } 
        else {
            fmt_printf("Decoding failed.\n");
        }
        free(encoded);
    } 
    else {
        fmt_printf("Encoding failed.\n");
    }
    return 0;
}
```

## Example 18 : encode and decode data with `encoding_base58_encode` and `encode_base58_decode`

```c
#include <stdlib.h>
#include "encoding/encoding.h"
#include "fmt/fmt.h"

int main() {
    const char *data = "Hello, World!";
    size_t binsz = strlen(data);
    char *b58 = encoding_base58_encode(data, binsz);
	
    fmt_printf("Base58 Encoded: %s\n", b58);

    size_t decoded_size;
    char *decoded = encoding_base58_decode(b58, &decoded_size);
    if (decoded) {
        fmt_printf("Decoded Data: %.*s\n", (int)decoded_size, decoded);
        free(decoded);
    }

    free(b58);
    return 0;
}
```

## Example 19 : encode and decode data Using `encoding_base91_encode` and `encodinل_base91_decode`

```c
#include <stdlib.h>
#include "fmt/fmt.h"
#include "encoding/encoding.h"


int main() {
    const char* input = "Hello, Base91!";
    size_t input_length = strlen(input);

    // Encoding
    char* encoded = encoding_base91_encode((const uint8_t*)input, input_length);
    if (!encoded) {
        fmt_fprintf(stderr, "Encoding failed.\n");
        return 1;
    }

    fmt_printf("Encoded: %s\n", encoded);

    // Decoding
    size_t decoded_length;
    uint8_t* decoded = encoding_base91_decode(encoded, &decoded_length);
    if (!decoded) {
        fmt_fprintf(stderr, "Decoding failed.\n");
        free(encoded);
        return 1;
	}
    fmt_printf("Decoded: %.*s\n", (int)decoded_length, decoded);

    free(encoded);
    free(decoded);
    return 0;
}
```