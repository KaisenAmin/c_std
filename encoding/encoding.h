#ifndef ENCODING_H
#define ENCODING_H

#include <stddef.h>
#include <stdint.h>

char* encoding_base64_encode(const char* input, size_t length);
char* encoding_base64_decode(const char* input, size_t length);
char* encoding_url_encode(const char* input, size_t length);
char* encoding_url_decode(const char* input, size_t lenght);
char* encoding_base32_encode(const char* input, size_t length);
char* encoding_base32_decode(const char* input, size_t length);
char* encoding_base16_encode(const char* input, size_t length);
char* encoding_base16_decode(const char* input, size_t length);
uint16_t* encoding_utf32_to_utf16(const uint32_t* input, size_t length);
uint32_t* encoding_utf16_to_utf32(const uint16_t* input, size_t length);


#endif // ENCODING_H