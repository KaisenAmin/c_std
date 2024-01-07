#include "encoding.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char* encoding_base64_encode(const char* input, size_t length) {
    size_t output_length = 4 * ((length + 2) / 3);
    char* encoded = malloc(output_length + 1); // +1 for null terminator

    if (!encoded) {
        perror("Can not allocate memory for encoded");
        return NULL;
    }

    size_t i, j;
    for (i = 0, j = 0; i < length;) {
        uint32_t octet_a = i < length ? (unsigned char)input[i++] : 0;
        uint32_t octet_b = i < length ? (unsigned char)input[i++] : 0;
        uint32_t octet_c = i < length ? (unsigned char)input[i++] : 0;
        uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        encoded[j++] = base64_chars[(triple >> 18) & 0x3F];
        encoded[j++] = base64_chars[(triple >> 12) & 0x3F];
        encoded[j++] = base64_chars[(triple >> 6) & 0x3F];
        encoded[j++] = base64_chars[triple & 0x3F];
    }

    for (size_t k = 0; k < (3 - length % 3) % 3; k++) {
        encoded[output_length - 1 - k] = '=';
    }
    encoded[output_length] = '\0';

    return encoded;
}

char* encoding_base64_decode(const char* input, size_t length) {
    if (length % 4 != 0) {
        return NULL;
    }

    size_t output_length = length / 4 * 3;
    if (input[length - 1] == '=')  {
        output_length--;
    }
    if (input[length - 2] == '=') {
        output_length--;
    }

    char* decoded = malloc(output_length + 1);
    if (!decoded) {
        perror("Can not allocate memory for decoded");
        return NULL;
    }
    static const unsigned char d[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0, 
        0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0, 
        0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
    };

    for (size_t i = 0, j = 0; i < length;) {
        uint32_t sextet_a = input[i] == '=' ? 0 & i++ : d[(unsigned char)input[i++]];
        uint32_t sextet_b = input[i] == '=' ? 0 & i++ : d[(unsigned char)input[i++]];
        uint32_t sextet_c = input[i] == '=' ? 0 & i++ : d[(unsigned char)input[i++]];
        uint32_t sextet_d = input[i] == '=' ? 0 & i++ : d[(unsigned char)input[i++]];
        uint32_t triple = (sextet_a << 18) + (sextet_b << 12) + (sextet_c << 6) + sextet_d;

        if (j < output_length) {
            decoded[j++] = (triple >> 16) & 0xFF;
        }
        if (j < output_length) {
            decoded[j++] = (triple >> 8) & 0xFF;
        }
        if (j < output_length) {
            decoded[j++] = triple & 0xFF;
        }
    }
    decoded[output_length] = '\0';

    return decoded;
}

char* encoding_url_encode(const char* input, size_t length) {
    char* result = malloc(3 * length + 1); // Worst case scenario, every character needs encoding
    if (!result) {
        perror("Can not allocate memory for URL encoded string");
        return NULL;
    }

    size_t result_index = 0;
    for (size_t i = 0; i < length; ++i) {
        char ch = input[i];
        if (isalnum(ch) || ch == '-' || ch == '.' || ch == '_' || ch == '~') {
            result[result_index++] = ch;
        } else {
            static const char hex[] = "0123456789ABCDEF";
            result[result_index++] = '%';
            result[result_index++] = hex[(ch >> 4) & 0x0F];
            result[result_index++] = hex[ch & 0x0F];
        }
    }
    result[result_index] = '\0';
    return result;
}

char* encoding_url_decode(const char* input, size_t length) {
    char* result = malloc(length + 1); // Decoded string will be equal or smaller in size
    if (!result) {
        perror("Can not allocate memory for URL decoded string");
        return NULL;
    }

    size_t result_index = 0;
    for (size_t i = 0; i < length; ++i) {
        char ch = input[i];
        if (ch == '%') {
            if (i + 2 >= length) {
                free(result);
                return NULL;
            }

            static const char hex[] = "0123456789ABCDEF";
            char hi = input[++i];
            char lo = input[++i];
            int hi_index = strchr(hex, toupper(hi)) - hex;
            int lo_index = strchr(hex, toupper(lo)) - hex;

            if (hi_index < 0 || hi_index >= 16 || lo_index < 0 || lo_index >= 16) {
                free(result);
                return NULL;
            }

            result[result_index++] = (char)((hi_index << 4) + lo_index);
        } else if (ch == '+') {
            result[result_index++] = ' ';
        } else {
            result[result_index++] = ch;
        }
    }
    result[result_index] = '\0';
    return result;
}