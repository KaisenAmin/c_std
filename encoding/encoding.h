/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Encoding
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in encoding.c (file-level convention).
 *
 * Family of byte-level codecs:
 *   - Base16 / Base32 / Base58 / Base64 / Base85 / Base91
 *   - Percent / URL encoding
 *   - UTF-8 ↔ UTF-16 ↔ UTF-32 conversions
 *   - Windows wchar_t ↔ UTF-8 bridge
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

#ifdef __cplusplus
extern "C" {
#endif


/* #define ENCODING_LOGGING_ENABLE */

#ifdef ENCODING_LOGGING_ENABLE
    #define ENCODING_LOG(fmt, ...) \
        do { fprintf(stderr, "[ENCODING LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define ENCODING_LOG(...) do { } while (0)
#endif



/* Ascii85 decode errors (returned via the `ascii85_err_*` sentinel
 * values; the public encode/decode wrappers return NULL on failure). */
enum ascii85_errs_e {
    ascii85_err_out_buf_too_small = -255,
    ascii85_err_in_buf_too_large,
    ascii85_err_bad_decode_char,
    ascii85_err_decode_overflow
};


/* UTF-N conversion result codes. */
typedef enum {
    conversionOK,      /* Conversion successful                       */
    sourceExhausted,   /* Partial character in source, hit end        */
    targetExhausted,   /* Insufficient room in target                 */
    sourceIllegal      /* Source sequence is illegal / malformed      */
} ConversionResult;


typedef enum {
    strictConversion,
    lenientConversion
} ConversionFlags;


/* ------------------------------------------------------------------ */
/* Lifecycle                                                          */
/* ------------------------------------------------------------------ */

void        encoding_initialize              (void);


/* ------------------------------------------------------------------ */
/* Base16 / hex                                                       */
/* ------------------------------------------------------------------ */

char*       encoding_base16_encode           (const char* input, size_t length);
char*       encoding_base16_decode           (const char* input, size_t length);


/* ------------------------------------------------------------------ */
/* Base32                                                             */
/* ------------------------------------------------------------------ */

char*       encoding_base32_encode           (const char* input, size_t length);
char*       encoding_base32_decode           (const char* input, size_t length);


/* ------------------------------------------------------------------ */
/* Base58                                                             */
/* ------------------------------------------------------------------ */

char*       encoding_base58_encode           (const void* data, size_t binsz);
char*       encoding_base58_decode           (const char* b58, size_t* binszp);


/* ------------------------------------------------------------------ */
/* Base64                                                             */
/* ------------------------------------------------------------------ */

char*       encoding_base64_encode           (const char* input, size_t length);
char*       encoding_base64_decode           (const char* input, size_t length);


/* ------------------------------------------------------------------ */
/* Base85 (Ascii85)                                                   */
/* ------------------------------------------------------------------ */

char*       encoding_base85_encode           (const uint8_t* input, size_t length);
uint8_t*    encoding_base85_decode           (const char* input, size_t length);


/* ------------------------------------------------------------------ */
/* Base91                                                             */
/* ------------------------------------------------------------------ */

char*       encoding_base91_encode           (const uint8_t* data, size_t length);
uint8_t*    encoding_base91_decode           (const char* encoded, size_t* decoded_length);


/* ------------------------------------------------------------------ */
/* URL / percent-encoding                                             */
/* ------------------------------------------------------------------ */

char*       encoding_url_encode              (const char* input, size_t length);
char*       encoding_url_decode              (const char* input, size_t length);


/* ------------------------------------------------------------------ */
/* UTF-8 ↔ UTF-16 ↔ UTF-32                                            */
/* ------------------------------------------------------------------ */

uint16_t*   encoding_utf8_to_utf16           (const uint8_t*  input, size_t length);
uint32_t*   encoding_utf8_to_utf32          (const uint8_t*  input, size_t length);
uint8_t*    encoding_utf16_to_utf8           (const uint16_t* input, size_t length);
uint32_t*   encoding_utf16_to_utf32          (const uint16_t* input, size_t length);
uint16_t*   encoding_utf32_to_utf16          (const uint32_t* input, size_t length);
uint8_t*    encoding_utf32_to_utf8           (const uint32_t* input, size_t length);


/* ------------------------------------------------------------------ */
/* UTF-8 ↔ wchar_t bridge                                             */
/* ------------------------------------------------------------------ */

#if defined(_WIN32) || defined(_WIN64)
wchar_t*    encoding_utf8_to_wchar           (const char* utf8Str);
#endif
char*       encoding_wchar_to_utf8           (const wchar_t* wstr);


/* ------------------------------------------------------------------ */
/* UTF-8 validation                                                   */
/* ------------------------------------------------------------------ */

bool        encoding_is_utf8                 (const uint8_t* input, size_t length);
bool        encoding_is_utf8_string          (const uint8_t** input, size_t length);


/* ------------------------------------------------------------------ */
/* Debug                                                              */
/* ------------------------------------------------------------------ */

void        encoding_hex_dump                (const void* data, size_t size);


/* Backward-compatibility aliases for the previously misspelled API names.
 * New code should use `encoding_base85_encode` / `encoding_base85_decode`. */
#define encododing_base85_encode  encoding_base85_encode
#define encododing_base85_decode  encoding_base85_decode


#ifdef __cplusplus
}
#endif

#endif 
