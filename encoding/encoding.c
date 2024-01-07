#include "encoding.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char base32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=";
static const char base16_chars[] = "0123456789ABCDEF";
static const int halfShift  = 10; /* used for shifting by 10 bits */
// static const uint8_t firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

static const unsigned int halfBase = 0x0010000UL;
static const unsigned int halfMask = 0x3FFUL;

#define UNI_REPLACEMENT_CHAR (uint32_t)0x0000FFFD
#define UNI_SUR_HIGH_START  (unsigned int)0xD800
#define UNI_SUR_HIGH_END    (unsigned int)0xDBFF
#define UNI_SUR_LOW_START   (unsigned int)0xDC00
#define UNI_SUR_LOW_END     (unsigned int)0xDFFF
#define UNI_MAX_BMP (unsigned int)0x0000FFFF
#define UNI_MAX_UTF16 (unsigned int)0x0010FFFF
#define UNI_MAX_UTF32 (unsigned int)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32 (unsigned int)0x0010FFFF
#define false      0
#define true        1

static int decode_char(unsigned char c) {
	char retval = -1;

	if (c >= 'A' && c <= 'Z') {
		retval = c - 'A';
    }
	if (c >= '2' && c <= '7') {
		retval = c - '2' + 26;
    }

	assert(retval == -1 || ((retval & 0x1F) == retval));
	return  retval;
}

/**
 * Given a block id between 0 and 7 inclusive, this will return the index of
 * the octet in which this block starts. For example, given 3 it will return 1
 * because block 3 starts in octet 1:
 *
 * +--------+--------+
 * | ......<|.3 >....|
 * +--------+--------+
 *  octet 1 | octet 2
 */
static int get_octet(int block) {
	assert(block >= 0 && block < 8);
	return (block*5) / 8;
}

/**
 * Given a block id between 0 and 7 inclusive, this will return how many bits
 * we can drop at the end of the octet in which this block starts. 
 * For example, given block 0 it will return 3 because there are 3 bits
 * we don't care about at the end:
 *
 *  +--------+-
 *  |< 0 >...|
 *  +--------+-
 *
 * Given block 1, it will return -2 because there
 * are actually two bits missing to have a complete block:
 *
 *  +--------+-
 *  |.....< 1|..
 *  +--------+-
 **/
static int get_offset(int block) {
	assert(block >= 0 && block < 8);
	return (8 - 5 - (5*block) % 8);
}

/**
 * Like "b >> offset" but it will do the right thing with negative offset.
 * We need this as bitwise shifting by a negative offset is undefined
 * behavior.
 */
static unsigned char shift_right(unsigned char byte, signed char offset) {
	if (offset > 0) {
		return byte >>  offset;
    }
    else {
		return byte << -offset;
    }
}

static unsigned char shift_left(unsigned char byte, signed char offset) {
	return shift_right(byte, - offset);
}

static int decode_sequence(const unsigned char *coded, unsigned char *plain) {
	assert(CHAR_BIT == 8);
	assert(coded && plain);

	plain[0] = 0;
	for (int block = 0; block < 8; block++) {
		int offset = get_offset(block);
		int octet = get_octet(block);

		int c = decode_char(coded[block]);
		if (c < 0) {
			return octet;
        }

		plain[octet] |= shift_left(c, offset);
		if (offset < 0) {  // does this block overflows to next octet?
			assert(octet < 4);
			plain[octet+1] = shift_left(c, 8 + offset);
		}
	}
	return 5;
}

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

char* encoding_base32_encode(const char* input, size_t length) {
    size_t output_length = ((length + 4) / 5) * 8; // Output length including padding
    char* encoded = malloc(output_length + 1);

    if (!encoded) {
        perror("Cannot allocate memory for encoded string");
        return NULL;
    }

    size_t input_index = 0;
    size_t output_index = 0;
    size_t bit_count = 0;
    uint32_t buffer = 0;

    while (input_index < length) {
        // Fill the buffer with up to 5 bytes
        buffer = (buffer << 8) | (uint8_t)input[input_index++];
        bit_count += 8;

        // While we have more than 5 bits, process the buffer
        while (bit_count >= 5) {
            encoded[output_index++] = base32[(buffer >> (bit_count - 5)) & 0x1F];
            bit_count -= 5;
        }
    }

    // Process any remaining bits in the buffer
    if (bit_count > 0) {
        encoded[output_index++] = base32[(buffer << (5 - bit_count)) & 0x1F];
    }

    // Add padding
    while (output_index < output_length) {
        encoded[output_index++] = '=';
    }

    encoded[output_index] = '\0';
    return encoded;
}

char* encoding_base32_decode(const char* input, size_t length) {
    if (length % 8 != 0) {
        return NULL; // Invalid input length
    }

    size_t olength = (length / 8) * 5;
    unsigned char* result = malloc(olength + 1);
    if (!result) {
        perror("Cannot allocate memory for Base32 decoded string");
        return NULL;
    }

    size_t i = 0, j = 0;
    while (i < length) {
        if (input[i] == '=') {
            break; // Padding character
        }

        int n = decode_sequence((const unsigned char*)&input[i], &result[j]);
        if (n < 5) { // Less than 5 bytes decoded, indicates padding or end of input
            j += n;
            break;
        }

        i += 8;
        j += 5;
    }

    result[j] = '\0';
    return (char*)result;
}

char* encoding_base16_encode(const char* input, size_t length) {
    size_t output_length = length * 2;
    char* encoded = malloc(output_length + 1);

    if (!encoded) {
        perror("Cannot allocate memory for Base16 encoded string");
        return NULL;
    }

    for (size_t i = 0, j = 0; i < length; ++i) {
        uint8_t ch = (uint8_t)input[i];
        encoded[j++] = base16_chars[(ch & 0xF0) >> 4];
        encoded[j++] = base16_chars[ch & 0x0F];
    }

    encoded[output_length] = '\0';
    return encoded;
}

char* encoding_base16_decode(const char* input, size_t length) {
    static const unsigned char base16_decode[128] ={
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
     };

    if (length % 2 != 0) {
        return NULL; // Invalid input length
    }

    size_t olength = length / 2;
    char* decoded = malloc(olength + 1);

    if (!decoded) {
        perror("Cannot allocate memory for Base16 decoded string");
        return NULL;
    }

    for (size_t i = 0, j = 0; i < length;) {
        uint8_t a = base16_decode[(unsigned char)input[i++]];
        uint8_t b = base16_decode[(unsigned char)input[i++]];
        
        if (a == 0xFF || b == 0xFF) {
            free(decoded);
            return NULL; // Invalid character
        }

        decoded[j++] = (a << 4) | b;
    }

    decoded[olength] = '\0';
    return decoded;
}

uint16_t* encoding_utf32_to_utf16(const uint32_t* input, size_t length) {
    if (input == NULL || length == 0) {
        return NULL;
    }

    // Allocate maximum possible size (each UTF-32 character might become two UTF-16 characters)
    uint16_t* output = malloc(sizeof(uint16_t) * (length * 2 + 1));
    if (!output) return NULL;

    size_t j = 0;
    for (size_t i = 0; i < length; ++i) {
        uint32_t ch = input[i];

        if (ch > UNI_MAX_LEGAL_UTF32) {
            // Handling invalid character
            free(output);
            return NULL;
        }

        if (ch <= UNI_MAX_BMP) {
            // Character can be directly converted to single UTF-16 unit
            output[j++] = (uint16_t)ch;
        } else if (ch > UNI_MAX_BMP && ch <= UNI_MAX_UTF16) {
            // Convert character to surrogate pair
            ch -= halfBase;
            output[j++] = (uint16_t)((ch >> halfShift) + UNI_SUR_HIGH_START);
            output[j++] = (uint16_t)((ch & halfMask) + UNI_SUR_LOW_START);
        }
    }

    // Null-terminate the UTF-16 string
    output[j] = 0;
    return output;
}

uint32_t* encoding_utf16_to_utf32(const uint16_t* input, size_t length) {
    if (input == NULL || length == 0) {
        return NULL;
    }

    // Allocate memory for the worst-case scenario (all characters are non-surrogates)
    uint32_t* output = malloc(sizeof(uint32_t) * (length + 1));
    if (!output) return NULL;

    size_t j = 0;
    for (size_t i = 0; i < length; ++i) {
        uint32_t ch = input[i];

        // Check for high surrogate
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            // Ensure there's a following character for the low surrogate
            if (i + 1 < length) {
                uint32_t ch2 = input[i + 1];
                // Check for low surrogate and construct the full code point
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ch = ((ch - UNI_SUR_HIGH_START) << 10) + (ch2 - UNI_SUR_LOW_START) + 0x10000;
                    i++; // Skip the low surrogate
                } else {
                    // Invalid sequence, you might want to handle the error differently
                    free(output);
                    return NULL;
                }
            } else {
                // High surrogate without a following low surrogate
                free(output);
                return NULL;
            }
        }

        output[j++] = ch;
    }

    // Null-terminate the output
    output[j] = 0;
    return output;
}

