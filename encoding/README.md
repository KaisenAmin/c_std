# Encoding Library in C

**Author:** amin tahmasebi
**Release Date:** 2024
**License:** ISC License


The Encoding library is a versatile tool designed for efficient and user-friendly encoding and decoding operations. While it currently supports Base64 encoding and decoding, the library is structured to accommodate additional encoding methods in the future. The Base64 functionality allows for conversion between binary data and a character set comprising 64 different characters, making it ideal for data storage and transfer over media primarily handling textual content. The library's extensible architecture ensures it can be adapted to include other encoding types such as Base16, Base32, and URL encoding, catering to a wide range of applications and data formats.

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

### `char* encoding_base64_encode(const char* input, size_t length)`: 
   - **Description**: Encodes a given binary data into Base64 format. 
   - **Parameters**: 
     - `input`: Pointer to the binary data to be encoded.
     - `length`: The length of the input data.
   - **Returns**: A dynamically allocated string containing the Base64 encoded representation.

---

### `char* encoding_base64_decode(const char* input, size_t length)`:
   - **Description**: Decodes a Base64 encoded string back to its original binary format.
   - **Parameters**: 
     - `input`: Pointer to the Base64 encoded string.
     - `length`: The length of the encoded string.
   - **Returns**: A dynamically allocated string containing the decoded binary data.

---

### `char* encoding_url_encode(const char* input, size_t length)`:
   - **Description**: Encodes a given string into a URL-friendly format, converting special characters into percent-encoded representations.
   - **Parameters**: 
     - `input`: Pointer to the string to be URL encoded.
     - `length`: The length of the input string.
   - **Returns**: A dynamically allocated string containing the URL encoded representation.

---

### `char* encoding_url_decode(const char* input, size_t lenght)`:
   - **Description**: Decodes a percent-encoded URL string back to its original format, interpreting percent-encoded characters.
   - **Parameters**: 
     - `input`: Pointer to the URL encoded string.
     - `length`: The length of the encoded string.
   - **Returns**: A dynamically allocated string containing the decoded string.

---

### `char* encoding_base32_encode(const char* input, size_t length)`:
   - **Description**: Encodes a given binary data into Base32 format, which is useful in scenarios where case-insensitivity is beneficial.
   - **Parameters**: 
     - `input`: Pointer to the binary data to be encoded.
     - `length`: The length of the input data.
   - **Returns**: A dynamically allocated string containing the Base32 encoded representation.

---

### `char* encoding_base32_decode(const char* input, size_t length)`:
   - **Description**: Decodes a Base32 encoded string back to its original binary format.
   - **Parameters**: 
     - `input`: Pointer to the Base32 encoded string.
     - `length`: The length of the encoded string.
   - **Returns**: A dynamically allocated string containing the decoded binary data.

---

### `char* encoding_base16_encode(const char* input, size_t length)`:
   - **Description**: Encodes binary data into Base16 (hexadecimal) format, which represents each byte of data as two hexadecimal digits.
   - **Parameters**: 
     - `input`: Pointer to the binary data to be encoded.
     - `length`: The length of the input data.
   - **Returns**: A dynamically allocated string containing the Base16 encoded representation.
---

### `char* encoding_base16_decode(const char* input, size_t length)`:
   - **Description**: Decodes a Base16 (hexadecimal) encoded string back to its original binary format.
   - **Parameters**: 
     - `input`: Pointer to the Base16 encoded string.
     - `length`: The length of the encoded string.
   - **Returns**: A dynamically allocated string containing the decoded binary data.

---

### `char* encoding_base58_encode(const void *data, size_t binsz)`:
   - **Description**: Encodes binary data into Base58 format, commonly used in cryptocurrencies such as Bitcoin.
   - **Parameters**: 
     - `data`: Pointer to the binary data to be encoded.
     - `binsz`: The length of the input data.
   - **Returns**: A dynamically allocated string containing the Base58 encoded representation.
---

### `char* encoding_base58_decode(const char *b58, size_t *binszp)`:
   - **Description**: Decodes a Base58 encoded string back to its original binary format.
   - **Parameters**: 
     - `b58`: Pointer to the Base58 encoded string.
     - `binszp`: Pointer to a size_t that will be set to the size of the decoded binary data.
   - **Returns**: A dynamically allocated string containing the decoded binary data.
---

### `char* encododing_base85_encode(const uint8_t* input, size_t length)`:
   - **Description**: Encodes binary data into Base85 (also known as ASCII85) format, often used in Adobe's PostScript and PDF formats.
   - **Parameters**: 
     - `input`: Pointer to the binary data to be encoded.
     - `length`: The length of the input data.
   - **Returns**: A dynamically allocated string containing the Base85 encoded representation.
---

### `uint8_t* encododing_base85_decode(const char* input, size_t length)`:
   - **Description**: Decodes a Base85 encoded string back to its original binary format.
   - **Parameters**: 
     - `input`: Pointer to the Base85 encoded string.
     - `length`: The length of the encoded string.
   - **Returns**: A dynamically allocated string containing the decoded binary data.

---

### `encoding_base91_encode`:
   - **Description**: Encodes binary data into Base91 format, which is more efficient in terms of character use compared to Base64 and Base85.
   - **Parameters**: 
     - `data`: Pointer to the binary data to be encoded.
     - `length`: The length of the input data.
   - **Returns**: A dynamically allocated string containing the Base91 encoded representation.

---

### `uint8_t* encoding_base91_decode(const char* encoded, size_t* decoded_length)`:
   - **Description**: Decodes a Base91 encoded string back to its original binary format.
   - **Parameters**: 
     - `encoded`: Pointer to the Base91 encoded string.
     - `decoded_length`: Pointer to a size_t that will be set to the size of the decoded binary data.
   - **Returns**: A dynamically allocated string containing the decoded binary data.

---

### `uint16_t* encoding_utf8_to_utf16(const uint8_t* input, size_t length)`:
   - **Description**: Converts a UTF-8 encoded string to UTF-16.
   - **Parameters**: 
     - `input`: Pointer to the UTF-8 encoded string.
     - `length`: The length of the input string.
   - **Returns**: A dynamically allocated UTF-16 encoded string.

---

### `uint32_t* encoding_utf8_to_utf32(const uint8_t* input, size_t length)`:
   - **Description**: Converts a UTF-8 encoded string to UTF-32.
   - **Parameters**: 
     - `input`: Pointer to the UTF-8 encoded string.
     - `length`: The length of the input string.
   - **Returns**: A dynamically allocated UTF-32 encoded string.

---

### `uint8_t* encoding_utf16_to_utf8(const uint16_t* input, size_t length)`:
   - **Description**: Converts a UTF-16 encoded string to UTF-8.
   - **Parameters**: 
     - `input`: Pointer to the UTF-16 encoded string.
     - `length`: The length of the input string.
   - **Returns**: A dynamically allocated UTF-8 encoded string.

---

### `uint32_t* encoding_utf16_to_utf32(const uint16_t* input, size_t length)`:
   - **Description**: Converts a UTF-16 encoded string to UTF-32.
   - **Parameters**: 
     - `input`: Pointer to the UTF-16 encoded string.
     - `length`: The length of the input string.
   - **Returns**: A dynamically allocated UTF-32 encoded string.

---

### `uint8_t* encoding_utf32_to_utf8(const uint32_t* input, size_t length)`:
   - **Description**: Converts a UTF-32 encoded string to UTF-8.
   - **Parameters**: 
     - `input`: Pointer to the UTF-32 encoded string.
     - `length`: The length of the input string.
   - **Returns**: A dynamically allocated UTF-8 encoded string.

---

### `uint16_t* encoding_utf32_to_utf16(const uint32_t* input, size_t length)`:
   - **Description**: Converts a UTF-32 encoded string to UTF-16.
   - **Parameters**: 
     - `input`: Pointer to the UTF-32 encoded string.
     - `length`: The length of the input string.
   - **Returns**: A dynamically allocated UTF-16 encoded string.

---

### `bool encoding_is_utf8(const uint8_t* input, size_t length)`:
   - **Description**: Checks whether a given byte sequence is valid UTF-8.
   - **Parameters**: 
     - `input`: Pointer to the byte sequence to check.
     - `length`: The length of the input byte sequence.
   - **Returns**: `true` if the sequence is valid UTF-8, `false` otherwise.
---

### `bool encoding_is_utf8_string(const uint8_t** input, size_t length)`:
   - **Description**: Checks whether a given string is a valid UTF-8 string.
   - **Parameters**: 
     - `input`: Pointer to a pointer to the string to check.
     - `length`: The length of the input string.
   - **Returns**: `true` if the string is valid UTF-8, `false` otherwise.

---

### `void encoding_hex_dump(const void *data, size_t size)`:
   - **Description**: Produces a hexadecimal dump of the given data, displaying both the hex values and their ASCII representation.
   - **Parameters**: 
     - `data`: Pointer to the data to dump.
     - `size`: The size of the data.
   - **Returns**: Void. The function prints the hexadecimal dump to standard output.

---

 Character Conversion
### `wchar_t* encoding_utf8_to_wchar(const char* utf8Str)` (Windows only):
   - **Description**: Converts a UTF-8 encoded string to a wide character string (`wchar_t`).
   - **Parameters**: 
     - `utf8Str`: Pointer to the UTF-8 encoded string.
   - **Returns**: A dynamically allocated wide character string.
---

### `char* encoding_wchar_to_utf8(const wchar_t* wstr)`:
   - **Description**: Converts a wide character string (`wchar_t`) to a UTF-8 encoded string.
   - **Parameters**: 
     - `wstr`: Pointer to the wide character string.
   - **Returns**: A dynamically allocated UTF-8 encoded string.

---

### `void encoding_initialize(void)`:
   - **Description**: Initializes the encoding library, setting the locale for character encoding. This function is typically called at the beginning of the program.
   - **Parameters**: None.
   - **Returns**: Void.

---


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
**Result:**
```
Encoded: SGVsbG8sIFdvcmxkIQ==
Decoded: Hello, World!
```

---

### Example 2: URL Encoding

In this example, we will encode a URL or a string with special characters using `encoding_url_encode`.

```c
#include "encoding/encoding.h"
#include "string/std_string.h"
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
**Result:**
```
Original: Hello World! This is a test/example?
URL Encoded: Hello%20World%21%20This%20is%20a%20test%2Fexample%3F
```

---

### Example 3: URL Decoding `encoding_url_decode`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

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
**Result:**
```
URL Encoded: Hello%20World%21%20This%20is%20a%20test%2Fexample%3F
Decoded: Hello World! This is a test/example?
```

---

## Examples 4 : use `encoding_base32_encode`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

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
**Result:**
```
Original: Hello, World!
Base32 Encoded: JBSWY3DPFQQFO33SNRSCC===
```

---

## Example 5 : use `encoding_base32_decode`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

int main() {
    const char* text_to_decode = "ORSXG5A=";  // Base32 encoded version of "test"
    char* decoded = encoding_base32_decode(text_to_decode, strlen(text_to_decode));

    if (decoded) {
        fmt_printf("Base32 Encoded: %s\n", text_to_decode);
        fmt_printf("Decoded: %s\n", decoded);
        free(decoded);
    } 
    else {
        fmt_printf("Failed to decode from Base32.\n");
    }
    return 0;
}
```
**Result:**
```
Base32 Encoded: ORSXG5A=
Decoded: test
```

---

## Example 6 : use `encoding_base16_encode`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

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
**Result:**
```
Original: Hello, World!
Base16 Encoded: 48656C6C6F2C20576F726C6421
```

---

## Example 7 : use `encoding_base32_decode`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

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
**Result:**
```
Base16 Encoded: 48656C6C6F2C20576F726C6421
Decoded: Hello, World!
```

---

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
**Result:**
```
UTF-16 String: D83D DE00 D83D DE03 D83D DE04 D83D DE06 D83D DE09 D83D DE0A \n
```

---

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
    fmt_printf("\n");

    free(utf16_string);
    return 0;
}
```
**Result:**
```
UTF-16 String: D83D DE00 D83D DE03 D83D DE04 D83D DE06 D83D DE09 D83D DE0A 
UTF-32 String: 1F600 1F603 1F604 1F606 1F609 1F60A
```

---

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
**Result:**
```
UTF-8 Encoded String: Hello World
```

---

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
**Result:**
```
UTF-8 Encoded String: Hello World
```

---

## Example 12 : Check data is valid UTF-8 or not with `encoding_is_utf8`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    // Valid UTF-8 string
    const uint8_t valid_utf8[] = {0xE2, 0x82, 0xAC}; // € symbol in UTF-8
    size_t length1 = sizeof(valid_utf8);

    if (encoding_is_utf8(valid_utf8, length1)) {
        fmt_printf("String 1 is valid UTF-8.\n");
    } 
    else {
        fmt_printf("String 1 is not valid UTF-8.\n");
    }

    //  Invalid UTF-8 string
    const uint8_t invalid_utf8[] = {0xE2, 0x28, 0xA1}; // Invalid sequence
    size_t length2 = sizeof(invalid_utf8);

    if (encoding_is_utf8(invalid_utf8, length2)) {
        fmt_printf("String 2 is valid UTF-8.\n");
    } 
    else {
        fmt_printf("String 2 is not valid UTF-8.\n");
    }

    // ASCII string (also valid UTF-8)
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
**Result:**
```
String 1 is valid UTF-8.
String 2 is not valid UTF-8.
String 3 is not valid UTF-8.
```

---

## Example 13 : Check data is valid UTF-8 String or not with `encoding_is_utf8_string`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"

int main() {
    // Valid UTF-8 string
    const uint8_t valid_utf8[] = {0xE2, 0x82, 0xAC, 0}; // € symbol in UTF-8
    const uint8_t* valid_ptr = valid_utf8;

    if (encoding_is_utf8_string(&valid_ptr, sizeof(valid_utf8))) {
        fmt_printf("Example 1 is valid UTF-8.\n");
    } 
    else {
        fmt_printf("Example 1 is not valid UTF-8.\n");
    }

    // Invalid UTF-8 string
    const uint8_t invalid_utf8[] = {0xE2, 0x28, 0xA1, 0}; // Invalid sequence
    const uint8_t* invalid_ptr = invalid_utf8;

    if (encoding_is_utf8_string(&invalid_ptr, sizeof(invalid_utf8))) {
        fmt_printf("Example 2 is valid UTF-8.\n");
    } 
    else {
        fmt_printf("Example 2 is not valid UTF-8.\n");
    }

    //ASCII string (also valid UTF-8)
    const uint8_t ascii_utf8[] = "Hello, World!"; // ASCII is a subset of UTF-8
    const uint8_t* ascii_ptr = ascii_utf8;

    if (encoding_is_utf8_string(&ascii_ptr, sizeof(ascii_utf8) - 1)) { 
        fmt_printf("Example 3 is valid UTF-8.\n");
    } 
    else {
        fmt_printf("Example 3 is not valid UTF-8.\n");
    }
    return 0;
}
```
**Result:**
```
Example 1 is valid UTF-8.
Error: Invalid UTF-8 encoding detected in encoding_is_utf8_string.
Example 2 is not valid UTF-8.
Example 3 is valid UTF-8.
```

---

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
**Result:**
```
UTF-16 Encoded String: 0048 0065 006C 006C 006F 002C 0020 0055 0054 0046 002D 0031 0036 0021 0020 D83D DE0A
```

---

### Example 15 :  `encoding_utf8_to_utf32`

```c
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
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
**Result:**
```
UTF-32 Encoded String: 00000048 00000065 0000006C 0000006C 0000006F 0000002C 00000020 00000055 00000054 00000046 0000002D 00000033 00000032 00000021 00000020 0001F30D
```

---

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
**Result:**
```
00000000  61 6d 69 6e                                      |amin|
```

---

## Example 17 : encode and decode data with Using `encoding_base85_encode` and `encoding_base85_decode`

```c
#include <stdlib.h>
#include <string.h>
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
**Result:**
```
Encoded ASCII85: 87cURD]it9AH
Decoded: Hello one
```

---

## Example 18 : encode and decode data with `encoding_base58_encode` and `encode_base58_decode`

```c
#include <stdlib.h>
#include <string.h>
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
**Result:**
```
Base58 Encoded: 72k1xXWG59fYdzSNoA
Decoded Data: Hello, World!
```

---

## Example 19 : encode and decode data Using `encoding_base91_encode` and `encodinل_base91_decode`

```c
#include <stdlib.h>
#include <string.h>
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
**Result:**
```
Encoded: >OwJh>}AdcN[>081Q
Decoded: Hello, Base91!
```


---

## License

This project is open-source and available under [ISC License].