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
