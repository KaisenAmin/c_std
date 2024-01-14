/**
 * @author Amin Tahmasebi
 * @date 2024 
 * @class Encoding
*/

#ifndef ENCODING_H
#define ENCODING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <locale.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <wchar.h>
#endif

enum ascii85_errs_e {
    ascii85_err_out_buf_too_small = -255,
    ascii85_err_in_buf_too_large,
    ascii85_err_bad_decode_char,
    ascii85_err_decode_overflow
};

typedef enum {
    conversionOK,      /* Conversion successful */
    sourceExhausted,   /* Partial character in source, but hit end */
    targetExhausted,   /* Insufficient room in target for conversion */
    sourceIllegal      /* Source sequence is illegal/malformed */
} ConversionResult;

typedef enum {
    strictConversion,
    lenientConversion
} ConversionFlags;

void encoding_hex_dump(const void *data, size_t size);
void encoding_initialize(void);

#if defined(_WIN32) || defined(_WIN64)
wchar_t* encoding_utf8_to_wchar(const char* utf8Str);
char* encoding_wchar_to_utf8(const wchar_t* wstr);
#endif 

char* encoding_base64_encode(const char* input, size_t length);
char* encoding_base64_decode(const char* input, size_t length);
char* encoding_url_encode(const char* input, size_t length);
char* encoding_url_decode(const char* input, size_t lenght);
char* encoding_base32_encode(const char* input, size_t length);
char* encoding_base32_decode(const char* input, size_t length);
char* encoding_base16_encode(const char* input, size_t length);
char* encoding_base16_decode(const char* input, size_t length);
char* encoding_base58_encode(const void *data, size_t binsz);
char* encoding_base58_decode(const char *b58, size_t *binszp);
char* encoding_base91_encode(const uint8_t* data, size_t length);
char* encododing_base85_encode(const uint8_t* input, size_t length);

uint16_t* encoding_utf8_to_utf16(const uint8_t* input, size_t length);
uint16_t* encoding_utf32_to_utf16(const uint32_t* input, size_t length);

uint32_t* encoding_utf16_to_utf32(const uint16_t* input, size_t length);
uint32_t* encoding_utf8_to_utf32(const uint8_t* input, size_t length);

uint8_t* encoding_utf16_to_utf8(const uint16_t* input, size_t length);
uint8_t* encoding_utf32_to_utf8(const uint32_t* input, size_t length);

uint8_t* encododing_base85_decode(const char* input, size_t length);
uint8_t* encoding_base91_decode(const char* encoded, size_t* decoded_length);

bool encoding_is_utf8(const uint8_t* input, size_t length);
bool encoding_is_utf8_string(const uint8_t** input, size_t length);

#endif // ENCODING_H