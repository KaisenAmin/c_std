# Encoding Library in C

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* text = "Hello, World!";
    char* encoded = encoding_base64_encode(text, strlen(text));
    if (encoded) {
        printf("Encoded: %s\n", encoded);

        char* decoded = encoding_base64_decode(encoded, strlen(encoded));
        if (decoded) {
            printf("Decoded: %s\n", decoded);
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
#include <stdio.h>
#include <stdlib.h>

int main() {
    String *text_to_encode = string_create("Hello World! This is a test/example?");
    char* encoded = encoding_url_encode(string_c_str(text_to_encode), string_length(text_to_encode));

    if (encoded) {
        printf("Original: %s\n", string_c_str(text_to_encode));
        printf("URL Encoded: %s\n", encoded);
        free(encoded);
    } 
    else {
        printf("Failed to encode URL.\n");
    }

    string_deallocate(text_to_encode);
    return 0;
}
```

### Example 3: URL Decoding `encoding_url_decode`

```c
#include "encoding.h"
#include <stdio.h>

int main() {
    const char* text_to_decode = "Hello%20World%21%20This%20is%20a%20test%2Fexample%3F";
    char* decoded = encoding_url_decode(text_to_decode, strlen(text_to_decode));
    if (decoded) {
        printf("URL Encoded: %s\n", text_to_decode);
        printf("Decoded: %s\n", decoded);
        free(decoded);
    } 
    else {
        printf("Failed to decode URL.\n");
    }

    return 0;
}
```

## Examples 4 : use `encoding_base32_encode`

```c
#include "encoding/encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* text_to_encode = "Hello, World!";
    char* encoded = encoding_base32_encode(text_to_encode, strlen(text_to_encode));

    if (encoded) {
        printf("Original: %s\n", text_to_encode);
        printf("Base32 Encoded: %s\n", encoded);
        free(encoded);
    } else {
        printf("Failed to encode in Base32.\n");
    }

    return 0;
}
```

## Example 5 : use `encoding_base32_decode`

```c
int main() {
    const char* text_to_decode = "ORSXG5A=";  // Base32 encoded version of "test"
    char* decoded = encoding_base32_decode(text_to_decode, strlen(text_to_decode));

    if (decoded) {
        printf("Base32 Encoded: %s\n", text_to_decode);
        printf("Decoded: %s\n", decoded);
        free(decoded);
    } else {
        printf("Failed to decode from Base32.\n");
    }

    return 0;
}
```

## Example 6 : use `encoding_base16_encode`

```c
#include "encoding/encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* text = "Hello, World!";
    char* encoded = encoding_base16_encode(text, strlen(text));
    if (encoded) {
        printf("Original: %s\n", text);
        printf("Base16 Encoded: %s\n", encoded);
        free(encoded);
    }

    return 0;
}
```

## Example 7 : use `encoding_base32_decode`

```c
#include "encoding/encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    const char* encodedText = "48656C6C6F2C20576F726C6421";
    char* decoded = encoding_base16_decode(encodedText, strlen(encodedText));
    
    if (decoded) {
        printf("Base16 Encoded: %s\n", encodedText);
        printf("Decoded: %s\n", decoded);
        free(decoded);
    }

    return 0;
}
```

## Example 8 : use `encoding_utf32_to_utf16`

```c
#include "encoding/encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main() {
    // UTF-32 string containing emojis
    uint32_t utf32_string[] = {0x1F600, 0x1F603, 0x1F604, 0x1F606, 0x1F609, 0x1F60A, 0x0000}; // Null-terminated

    size_t length = 0;
    while (utf32_string[length] != 0) {
        length++;
    }

    uint16_t* utf16_string = encoding_utf32_to_utf16(utf32_string, length);

    // Print UTF-16 string as hexadecimal values
    printf("UTF-16 String: ");
    for (size_t i = 0; utf16_string[i] != 0; i++) {
        printf("%04X ", utf16_string[i]);
    }
    printf("\\n");


    free(utf16_string);
    return 0;
}
```

## Example 9 : convert utf16 to utf32 with `encoding_utf16_to_utf32`

```c
#include "encoding/encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main() {
    // UTF-32 string containing emojis
    uint32_t utf32_string[] = {0x1F600, 0x1F603, 0x1F604, 0x1F606, 0x1F609, 0x1F60A, 0x0000}; // Null-terminated

    size_t length = 0;
    while (utf32_string[length] != 0) {
        length++;
    }

    uint16_t* utf16_string = encoding_utf32_to_utf16(utf32_string, length);
    size_t index = 0;

    while (utf16_string[index] != 0)
        index++;

    // Print UTF-16 string as hexadecimal values
    printf("UTF-16 String: ");
    for (size_t i = 0; utf16_string[i] != 0; i++) {
        printf("%04X ", utf16_string[i]);
    }
    printf("\n");

    uint32_t* calc = encoding_utf16_to_utf32(utf16_string, index);

    printf("UTF-32 String: ");
    for (size_t i = 0; calc[i] != 0; i++) {
        printf("%0X ", calc[i]);
    }

    free(utf16_string);
    return 0;
}
```