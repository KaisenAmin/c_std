/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Encoding
*/

#include "encoding.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

bool (*b58_sha256_impl)(void *, const void *, size_t) = NULL;

#define UNI_REPLACEMENT_CHAR (uint32_t)0x0000FFFD
#define UNI_SUR_HIGH_START  (unsigned int)0xD800
#define UNI_SUR_HIGH_END    (unsigned int)0xDBFF
#define UNI_SUR_LOW_START   (unsigned int)0xDC00
#define UNI_SUR_LOW_END     (unsigned int)0xDFFF
#define UNI_MAX_BMP (unsigned int)0x0000FFFF
#define UNI_MAX_UTF16 (unsigned int)0x0010FFFF
#define UNI_MAX_UTF32 (unsigned int)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32 (unsigned int)0x0010FFFF
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define b58_almostmaxint_bits (sizeof(uint32_t) * 8)

static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const char base32[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=";
static const char base16_chars[] = "0123456789ABCDEF";
static const int halfShift  = 10; /* used for shifting by 10 bits */
static const uint8_t firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
static const char trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};
static const uint32_t offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 
                     0x03C82080UL, 0xFA082080UL, 0x82082080UL };
static const unsigned int halfBase = 0x0010000UL;
static const unsigned int halfMask = 0x3FFUL;
static const char b58digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
static const int8_t b58digits_map[] = {
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1, 0, 1, 2, 3, 4, 5, 6,  7, 8,-1,-1,-1,-1,-1,-1,
	-1, 9,10,11,12,13,14,15, 16,-1,17,18,19,20,21,-1,
	22,23,24,25,26,27,28,29, 30,31,32,-1,-1,-1,-1,-1,
	-1,33,34,35,36,37,38,39, 40,41,42,43,-1,44,45,46,
	47,48,49,50,51,52,53,54, 55,56,57,-1,-1,-1,-1,-1,
};
static const char BASE91_ALPHABET[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
    "!#$%&()*+,./:;<=>?@[]^_`{|}~\"";

static int base91_decode_value(char c) {
    for (int i = 0; i < 91; ++i) {
        if (BASE91_ALPHABET[i] == c) {
            return i;
        }
    }
    return -1; // Character not found
}

bool encoding_is_utf8(const uint8_t* input, size_t length) {
    uint8_t a;
    const uint8_t *srcptr = input+length;

    switch (length) {
    default: 
        return false;
        /* Everything else falls through when "true"... */
    case 4: 
        if ((a = (*--srcptr)) < 0x80 || a > 0xBF) {
            return false;
        }
    /* fall through */
    case 3: 
        if ((a = (*--srcptr)) < 0x80 || a > 0xBF) {
            return false;
        }
   /* fall through */
    case 2: 
        if ((a = (*--srcptr)) < 0x80 || a > 0xBF) {
            return false;
        }
    /* fall through */
    

        switch (*input) {
            case 0xE0: 
                if (a < 0xA0) {
                    return false;
                } 
                break;
            case 0xED: 
                if (a > 0x9F) {
                    return false;
                } 
                break;
            case 0xF0: 
                if (a < 0x90) {
                    return false;
                } 
                break;
            case 0xF4: 
                if (a > 0x8F) {
                    return false;
                } 
                break;
            default:   
                if (a < 0x80) {
                    return false;
                }
            /* fall through */
        }
    /* fall through */
    case 1: 
        if (*input >= 0x80 && *input < 0xC2) {
            return false;
        }
    }

    if (*input > 0xF4) {
        return false;
    }
    return true;
}

static ConversionResult ConvertUTF16toUTF8 (
        const uint16_t** sourceStart, const uint16_t* sourceEnd, 
        uint8_t** targetStart, uint8_t* targetEnd, ConversionFlags flags) {

    ConversionResult result = conversionOK;
    const uint16_t* source = *sourceStart;
    uint8_t* target = *targetStart;

    while (source < sourceEnd) {
        uint32_t ch;
        unsigned short bytesToWrite = 0;
        const uint32_t byteMask = 0xBF;
        const uint32_t byteMark = 0x80; 
        const uint16_t* oldSource = source; /* In case we have to back up because of target overflow. */
        ch = *source++;
        /* If we have a surrogate pair, convert to UTF32 first. */
        if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
            /* If the 16 bits following the high surrogate are in the source buffer... */
            if (source < sourceEnd) {
                uint32_t ch2 = *source;
                /* If it's a low surrogate, convert to UTF32. */
                if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
                    ch = ((ch - UNI_SUR_HIGH_START) << halfShift)
                        + (ch2 - UNI_SUR_LOW_START) + halfBase;
                    ++source;
                } 
                else if (flags == strictConversion) { /* it's an unpaired high surrogate */
                    --source; /* return to the illegal value itself */
                    result = sourceIllegal;
                    break;
                }
            } 
            else { /* We don't have the 16 bits following the high surrogate. */
                --source; /* return to the high surrogate */
                result = sourceExhausted;
                break;
            }
        } 
        else if (flags == strictConversion) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
                --source; /* return to the illegal value itself */
                result = sourceIllegal;
                break;
            }
        }
        /* Figure out how many bytes the result will require */
        if (ch < (uint32_t)0x80) {      
            bytesToWrite = 1;
        } 
        else if (ch < (uint32_t)0x800) {     
            bytesToWrite = 2;
        } 
        else if (ch < (uint32_t)0x10000) {   
            bytesToWrite = 3;
        } 
        else if (ch < (uint32_t)0x110000) {  
            bytesToWrite = 4;
        } 
        else {                            
            bytesToWrite = 3;
            ch = UNI_REPLACEMENT_CHAR;
        }

        target += bytesToWrite;
        if (target > targetEnd) {
            source = oldSource; /* Back up source pointer! */
            target -= bytesToWrite; result = targetExhausted; break;
        }
        switch (bytesToWrite) { /* note: everything falls through. */
            case 4: 
            *--target = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            // fall through
            case 3: 
            *--target = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            // fall through
            case 2: 
            *--target = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            // fall through
            case 1: 
            *--target =  (uint8_t)(ch | firstByteMark[bytesToWrite]);
            // fall through
        }
        target += bytesToWrite;
    }
    *sourceStart = source;
    *targetStart = target;

    return result;
}

static ConversionResult ConvertUTF32toUTF8 (
        const uint32_t** sourceStart, const uint32_t* sourceEnd, 
        uint8_t** targetStart, uint8_t* targetEnd, ConversionFlags flags) {

    ConversionResult result = conversionOK;
    const uint32_t* source = *sourceStart;
    uint8_t* target = *targetStart;

    while (source < sourceEnd) {
        uint32_t ch;
        unsigned short bytesToWrite = 0;
        const uint32_t byteMask = 0xBF;
        const uint32_t byteMark = 0x80; 
        ch = *source++;
        if (flags == strictConversion ) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                --source; /* return to the illegal value itself */
                result = sourceIllegal;
                break;
            }
        }
        /*
         * Figure out how many bytes the result will require. Turn any
         * illegally large UTF32 things (> Plane 17) into replacement chars.
         */
        if (ch < (uint32_t)0x80) {      
            bytesToWrite = 1;
        } 
        else if (ch < (uint32_t)0x800) {     
            bytesToWrite = 2;
        } 
        else if (ch < (uint32_t)0x10000) {   
            bytesToWrite = 3;
        } 
        else if (ch <= UNI_MAX_LEGAL_UTF32) {  
            bytesToWrite = 4;
        } 
        else {                            
            bytesToWrite = 3;
            ch = UNI_REPLACEMENT_CHAR;
            result = sourceIllegal;
        }
        
        target += bytesToWrite;
        if (target > targetEnd) {
            --source; /* Back up source pointer! */
            target -= bytesToWrite; result = targetExhausted; break;
        }
        switch (bytesToWrite) { /* note: everything falls through. */
            case 4: 
                *--target = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            // fall through
            case 3: 
                *--target = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            // fall through
            case 2: 
                *--target = (uint8_t)((ch | byteMark) & byteMask); ch >>= 6;
            // fall through
            case 1: 
                *--target = (uint8_t) (ch | firstByteMark[bytesToWrite]);
            // fall through
        }
        target += bytesToWrite;
    }
    *sourceStart = source;
    *targetStart = target;

    return result;
}

static ConversionResult ConvertUTF8toUTF16 (
        const uint8_t** sourceStart, const uint8_t* sourceEnd, 
        uint16_t** targetStart, uint16_t* targetEnd, ConversionFlags flags) {
    ConversionResult result = conversionOK;
    const uint8_t* source = *sourceStart;
    uint16_t* target = *targetStart;

    while (source < sourceEnd) {
        uint32_t ch = 0;
        unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
        
        if (extraBytesToRead >= sourceEnd - source) {
            result = sourceExhausted; break;
        }
        /* Do this check whether lenient or strict */
        if (!encoding_is_utf8(source, extraBytesToRead+1)) {
            result = sourceIllegal;
            break;
        }
        /*
         * The cases all fall through. See "Note A" below.
         */
        switch (extraBytesToRead) {
            case 5: 
                ch += *source++; ch <<= 6; /* remember, illegal UTF-8 */
            // fall through
            case 4: 
                ch += *source++; ch <<= 6; /* remember, illegal UTF-8 */
                // fall through
            case 3: 
                ch += *source++; ch <<= 6;
                // fall through
            case 2: 
                ch += *source++; ch <<= 6;
                // fall through
            case 1: 
                ch += *source++; ch <<= 6;
                // fall through
            case 0: 
                ch += *source++;
                // fall through
        }
        ch -= offsetsFromUTF8[extraBytesToRead];

        if (target >= targetEnd) {
            source -= (extraBytesToRead+1); /* Back up source pointer! */
            result = targetExhausted; break;
        }
        if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (flags == strictConversion) {
                    source -= (extraBytesToRead+1); /* return to the illegal value itself */
                    result = sourceIllegal;
                    break;
                } 
                else {
                    *target++ = UNI_REPLACEMENT_CHAR;
                }
            } 
            else {
                *target++ = (uint16_t)ch; /* normal case */
            }
        } 
        else if (ch > UNI_MAX_UTF16) {
            if (flags == strictConversion) {
                result = sourceIllegal;
                source -= (extraBytesToRead+1); /* return to the start */
                break; /* Bail out; shouldn't continue */
            } 
            else {
                *target++ = UNI_REPLACEMENT_CHAR;
            }
        } 
        else {
            /* target is a character in range 0xFFFF - 0x10FFFF. */
            if (target + 1 >= targetEnd) {
                source -= (extraBytesToRead+1); /* Back up source pointer! */
                result = targetExhausted; break;
            }
            ch -= halfBase;
            *target++ = (uint16_t)((ch >> halfShift) + UNI_SUR_HIGH_START);
            *target++ = (uint16_t)((ch & halfMask) + UNI_SUR_LOW_START);
        }
    }
    *sourceStart = source;
    *targetStart = target;

    return result;
}

ConversionResult ConvertUTF8toUTF32 (
        const uint8_t** sourceStart, const uint8_t* sourceEnd, 
        uint32_t** targetStart, uint32_t* targetEnd, ConversionFlags flags) {
    ConversionResult result = conversionOK;
    const uint8_t* source = *sourceStart;
    uint32_t* target = *targetStart;
    
    while (source < sourceEnd) {
        uint32_t ch = 0;
        unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
        if (extraBytesToRead >= sourceEnd - source) {
            result = sourceExhausted; break;
        }
        /* Do this check whether lenient or strict */
        if (!encoding_is_utf8(source, extraBytesToRead+1)) {
            result = sourceIllegal;
            break;
        }
        /*
         * The cases all fall through. See "Note A" below.
         */
        switch (extraBytesToRead) {
            case 5: 
                ch += *source++; ch <<= 6;
                // fall through
            case 4: 
                ch += *source++; ch <<= 6;
                // fall through
            case 3: 
                ch += *source++; ch <<= 6;
                // fall through
            case 2: 
                ch += *source++; ch <<= 6;
                // fall through
            case 1: 
                ch += *source++; ch <<= 6;
                // fall through
            case 
                0: ch += *source++;
                // fall through
        }
        ch -= offsetsFromUTF8[extraBytesToRead];

        if (target >= targetEnd) {
            source -= (extraBytesToRead+1); /* Back up the source pointer! */
            result = targetExhausted; break;
        }
        if (ch <= UNI_MAX_LEGAL_UTF32) {
            /*
             * UTF-16 surrogate values are illegal in UTF-32, and anything
             * over Plane 17 (> 0x10FFFF) is illegal.
             */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (flags == strictConversion) {
                    source -= (extraBytesToRead+1); /* return to the illegal value itself */
                    result = sourceIllegal;
                    break;
                } else {
                    *target++ = UNI_REPLACEMENT_CHAR;
                }
            } else {
                *target++ = ch;
            }
        } else { /* i.e., ch > UNI_MAX_LEGAL_UTF32 */
            result = sourceIllegal;
            *target++ = UNI_REPLACEMENT_CHAR;
        }
    }
    *sourceStart = source;
    *targetStart = target;
    return result;
}

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
        fmt_fprintf(stderr, "Error: Can not allocate memory for encoded in encoding_base64_encode.\n");
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
        fmt_fprintf(stderr, "Error: Invalid input length in encoding_base64_decode. Length must be a multiple of 4.\n");
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
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_base64_decode.\n");
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
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_url_encode.\n");
        return NULL;
    }

    size_t result_index = 0;
    for (size_t i = 0; i < length; ++i) {
        char ch = input[i];
        if (isalnum(ch) || ch == '-' || ch == '.' || ch == '_' || ch == '~') {
            result[result_index++] = ch;
        } 
        else {
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
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_url_decode.\n");
        return NULL;
    }

    size_t result_index = 0;
    for (size_t i = 0; i < length; ++i) {
        char ch = input[i];
        if (ch == '%') {
            if (i + 2 >= length) {
                fmt_fprintf(stderr, "Error: Incomplete percent-encoding in encoding_url_decode.\n");
                free(result);
                return NULL;
            }

            static const char hex[] = "0123456789ABCDEF";
            char hi = input[++i];
            char lo = input[++i];
            int hi_index = strchr(hex, toupper(hi)) - hex;
            int lo_index = strchr(hex, toupper(lo)) - hex;

            if (hi_index < 0 || hi_index >= 16 || lo_index < 0 || lo_index >= 16) {
                fmt_fprintf(stderr, "Error: Invalid hex characters in percent-encoding in encoding_url_decode.\n");
                free(result);
                return NULL;
            }

            result[result_index++] = (char)((hi_index << 4) + lo_index);
        } 
        else if (ch == '+') {
            result[result_index++] = ' ';
        } 
        else {
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
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_base32_encode.\n");
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
        fmt_fprintf(stderr, "Error: Invalid input length in encoding_base32_decode. Length must be a multiple of 8.\n");
        return NULL;
    }

    size_t olength = (length / 8) * 5;
    unsigned char* result = malloc(olength + 1);
    if (!result) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_base32_decode.\n");
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
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_base16_encode.\n");
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
    if (input == NULL) {
        fmt_fprintf(stderr, "Error: Invalid input param in encoding_base16_decode.\n");
        return NULL;
    }
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
        fmt_fprintf(stderr, "Error: Invalid input length in encoding_base16_decode.\n");
        return NULL; // Invalid input length
    }

    size_t olength = length / 2;
    char* decoded = malloc(olength + 1);

    if (!decoded) {
        fmt_fprintf(stderr, "Error: Cannot allocate memory for Base16 decoded string in encoding_base16_decode.\n");
        return NULL;
    }

    for (size_t i = 0, j = 0; i < length;) {
        uint8_t a = base16_decode[(unsigned char)input[i++]];
        uint8_t b = base16_decode[(unsigned char)input[i++]];
        
        if (a == 0xFF || b == 0xFF) {
            fmt_fprintf(stderr, "Error: Invalid Character in encoding_base16_decode.\n");
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
        fmt_fprintf(stderr, "Error: Invalid input or length in encoding_utf32_to_utf16.\n");
        return NULL;
    }

    // Allocate maximum possible size (each UTF-32 character might become two UTF-16 characters)
    uint16_t* output = malloc(sizeof(uint16_t) * (length * 2 + 1));
    if (!output) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_utf32_to_utf16.\n");
        return NULL;
    }

    size_t j = 0;
    for (size_t i = 0; i < length; ++i) {
        uint32_t ch = input[i];

        if (ch > UNI_MAX_LEGAL_UTF32) {
            fmt_fprintf(stderr, "Error: Invalid Character in encoding_utf32_to_utf16.\n");
            free(output);
            return NULL;
        }

        if (ch <= UNI_MAX_BMP) {
            // Character can be directly converted to single UTF-16 unit
            output[j++] = (uint16_t)ch;
        } 
        else if (ch > UNI_MAX_BMP && ch <= UNI_MAX_UTF16) {
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
        fmt_fprintf(stderr, "Error: Invalid input or length in encoding_utf16_to_utf32.\n");
        return NULL;
    }

    // Allocate memory for the worst-case scenario (all characters are non-surrogates)
    uint32_t* output = malloc(sizeof(uint32_t) * (length + 1));
    if (!output) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_utf16_to_utf32.\n");
        return NULL;
    }

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
                } 
                else {
                    fmt_fprintf(stderr, "Error: Invalid surrogate pair in encoding_utf16_to_utf32.\n");
                    free(output);
                    return NULL;
                }
            } 
            else {
                fmt_fprintf(stderr, "Error: Lone high surrogate without a low surrogate in encoding_utf16_to_utf32.\n");
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

uint8_t* encoding_utf16_to_utf8(const uint16_t* input, size_t length) {
    if (input == NULL || length == 0) {
        fmt_fprintf(stderr, "Error: Invalid input or length in encoding_utf16_to_utf8.\n");
        return NULL;
    }
    // Estimate maximum output size (4 bytes per UTF-16 character)
    size_t maxOutLength = length * 4;
    uint8_t* output = (uint8_t*)malloc(maxOutLength);
    if (!output) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_utf16_to_utf8.\n");
        return NULL;
    }

    const uint16_t* sourceStart = input;
    const uint16_t* sourceEnd = input + length;
    uint8_t* targetStart = output;
    uint8_t* targetEnd = output + maxOutLength;

    ConversionResult result = ConvertUTF16toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);

    if (result != conversionOK) {
        fmt_fprintf(stderr, "Error: Conversion from UTF-16 to UTF-8 failed in encoding_utf16_to_utf8.\n");
        free(output);
        return NULL;
    }

    // Resize the output to the actual UTF-8 string length
    size_t actualLength = targetStart - output;
    uint8_t* resizedOutput = (uint8_t*)realloc(output, actualLength + 1);
    // if (resizedOutput) {
    //     resizedOutput[actualLength] = '\0';
    //     return resizedOutput;
    // }

    // return output;

    if (!resizedOutput) {
        fmt_fprintf(stderr, "Error: Reallocation failed in encoding_utf16_to_utf8.\n");
        free(output);
        return NULL;
    }
    resizedOutput[actualLength] = '\0';

    return resizedOutput;
}

uint8_t* encoding_utf32_to_utf8(const uint32_t* input, size_t length) {
    if (input == NULL || length == 0) {
        fmt_fprintf(stderr, "Error: Invalid input or length in encoding_utf32_to_utf8.\n");
        return NULL;
    }

    // Estimate maximum output size (4 bytes per UTF-32 character)
    size_t maxOutLength = length * 4;
    uint8_t* output = (uint8_t*)malloc(maxOutLength);
    if (!output) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_utf32_to_utf8.\n");
        return NULL;
    }

    const uint32_t* sourceStart = input;
    const uint32_t* sourceEnd = input + length;
    uint8_t* targetStart = output;
    uint8_t* targetEnd = output + maxOutLength;

    ConversionResult result = ConvertUTF32toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);

    if (result != conversionOK) {
        fmt_fprintf(stderr, "Error: Conversion from UTF-32 to UTF-8 failed in encoding_utf32_to_utf8.\n");
        free(output);
        return NULL;
    }

    // Resize the output to the actual UTF-8 string length
    size_t actualLength = targetStart - output;
    uint8_t* resizedOutput = (uint8_t*)realloc(output, actualLength + 1);
    // if (resizedOutput) {
    //     resizedOutput[actualLength] = '\0';
    //     return resizedOutput;
    // }

    // return output;

    if (!resizedOutput) {
        fmt_fprintf(stderr, "Error: Reallocation failed in encoding_utf32_to_utf8.\n");
        free(output);
        return NULL;
    }
    resizedOutput[actualLength] = '\0';

    return resizedOutput;
}

bool encoding_is_utf8_string(const uint8_t** input, size_t length) {
    if (input == NULL || *input == NULL || length == 0) {
        fmt_fprintf(stderr, "Error: Invalid input or length in encoding_is_utf8_string.\n");
        return false;
    }

    const uint8_t* source = *input;
    const uint8_t* sourceEnd = source + length;

    while (source < sourceEnd) {
        int trailLength = trailingBytesForUTF8[*source] + 1;
        if (trailLength > sourceEnd - source || !encoding_is_utf8(source, trailLength)) {
            fmt_fprintf(stderr, "Error: Invalid UTF-8 encoding detected in encoding_is_utf8_string.\n");
            return false;
        }
        source += trailLength;
    }

    *input = source; // Update the input pointer to the end of the string
    return true;
}

uint16_t* encoding_utf8_to_utf16(const uint8_t* input, size_t length) {
    if (input == NULL || length == 0) {
        fmt_fprintf(stderr, "Error: Invalid input or length in encoding_utf8_to_utf16.\n");
        return NULL;
    }
    // Estimate maximum output size (each UTF-8 character can be at most 4 bytes, 
    // but can translate to at most 2 UTF-16 characters)
    size_t maxOutLength = length * 2;
    uint16_t* output = (uint16_t*)malloc(maxOutLength * sizeof(uint16_t));
    if (!output) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for output in encoding_utf8_to_utf16.\n");
        return NULL;
    }

    const uint8_t* sourceStart = input;
    const uint8_t* sourceEnd = input + length;
    uint16_t* targetStart = output;
    uint16_t* targetEnd = output + maxOutLength;

    ConversionResult result = ConvertUTF8toUTF16(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);

    if (result != conversionOK) {
        fmt_fprintf(stderr, "Error: Conversion from UTF-8 to UTF-16 failed in encoding_utf8_to_utf16.\n");
        free(output);
        return NULL;
    }
    // Resize the output to the actual UTF-16 string length
    size_t actualLength = targetStart - output;
    uint16_t* resizedOutput = (uint16_t*)realloc(output, actualLength * sizeof(uint16_t) + 1);
    if (!resizedOutput) {
        fmt_fprintf(stderr, "Error: Reallocation failed in encoding_utf8_to_utf16.\n");
        free(output);
        return NULL;
    }
    resizedOutput[actualLength] = '\0'; // Null-terminate the UTF-16 string

    return resizedOutput;
}

uint32_t* encoding_utf8_to_utf32(const uint8_t* input, size_t length) {
    if (input == NULL || length == 0) {
        fmt_fprintf(stderr, "Error: Invalid input or length in encododing_utf8_to_utf32.\n");
        return NULL;
    }

    // Estimate maximum output size (each UTF-8 character can be at most 4 bytes, 
    // translating to a single UTF-32 character)
    size_t maxOutLength = length;
    uint32_t* output = (uint32_t*)malloc(maxOutLength * sizeof(uint32_t));
    if (!output) {
        fmt_fprintf(stderr, "Error: Can not Allocate memory in encoding_utf8_to_utf32.\n");
        return NULL; // Memory allocation failed
    }

    const uint8_t* sourceStart = input;
    const uint8_t* sourceEnd = input + length;
    uint32_t* targetStart = output;
    uint32_t* targetEnd = output + maxOutLength;

    ConversionResult result = ConvertUTF8toUTF32(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);

    if (result != conversionOK) {
        fmt_fprintf(stderr, "Error: Failed Convertion to UTF32 in encoding_utf8_to_utf32.\n");
        free(output);
        return NULL;
    }

    // Resize the output to the actual UTF-32 string length
    size_t actualLength = targetStart - output;
    uint32_t* resizedOutput = (uint32_t*)realloc(output, (actualLength + 1) * sizeof(uint32_t));
    if (resizedOutput) {
        resizedOutput[actualLength] = 0; // Null-terminate the UTF-32 string
        return resizedOutput;
    }
    return output;
}

void encoding_hex_dump(const void *data, size_t size) {
    const unsigned char *byte = (const unsigned char *)data;
    size_t i, j;

    for (i = 0; i < size; i += 16) {
        fmt_printf("%08zx  ", i); // Print the offset

        // Print hex values
        for (j = 0; j < 16; j++) {
            if (i + j < size) {
                fmt_printf("%02x ", byte[i + j]);
            } 
            else {
                fmt_printf("   "); // Fill space if less than 16 bytes in a line
            }
        }
        fmt_printf(" |");

        // Print ASCII representation
        for (j = 0; j < 16; j++) {
            if (i + j < size) {
                fmt_printf("%c", isprint(byte[i + j]) ? byte[i + j] : '.');
            }
        }
        fmt_printf("|\n");
    }
}

char* encododing_base85_encode(const uint8_t* input, size_t length) {
    if (input == NULL || length == 0) {
        fmt_fprintf(stderr, "Error: Invalid input or length in encododing_base85_encode.\n");
        return NULL;
    }

    // Calculate the maximum possible length of the encoded string
    size_t encoded_max_length = ((length + 3) / 4) * 5 + 2; // +2 for potential padding and null terminator
    char* encoded = malloc(encoded_max_length);
    if (!encoded) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for encoded string in encododing_base85_encode.\n");
        return NULL;
    }

    size_t input_index = 0;
    size_t encoded_index = 0;
    while (input_index < length) {
        uint32_t acc = 0;
        size_t chunk_len = (length - input_index < 4) ? (length - input_index) : 4;

        for (size_t i = 0; i < chunk_len; ++i) {
            acc = (acc << 8) | input[input_index++];
        }

        if (chunk_len < 4) {
            acc <<= (4 - chunk_len) * 8; // Padding
        }
        if (acc == 0 && chunk_len == 4) {
            encoded[encoded_index++] = 'z';
        } 
        else {
            for (int i = 4; i >= 0; --i) {
                encoded[encoded_index + i] = (acc % 85) + 33;
                acc /= 85;
            }
            encoded_index += 5;

            if (chunk_len < 4) {
                encoded_index -= (4 - chunk_len);  // Adjust for padding
                break;
            }
        }
    }

    encoded[encoded_index] = '\0';
    return encoded;
}

uint8_t* encododing_base85_decode(const char* input, size_t length) {
    if (input == NULL || length == 0) {
        fmt_fprintf(stderr, "Error: Invalid input or length in encododing_base85_decode.\n");
        return NULL;
    }

    // Calculate the maximum possible length of the decoded string
    size_t decoded_max_length = (length / 5) * 4;
    uint8_t* decoded = malloc(decoded_max_length);
    if (!decoded) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for decoded string in encododing_base85_decode.\n");
        return NULL;
    }

    size_t input_index = 0;
    size_t decoded_index = 0;
    while (input_index < length) {
        if (isspace(input[input_index])) {
            input_index++;   // Skip whitespace
            continue;
        }
        if (input[input_index] == 'z') {
            // Special case: 'z' represents four zero bytes
            memset(decoded + decoded_index, 0, 4);
            decoded_index += 4;
            input_index++;
            continue;
        }

        uint32_t acc = 0;
        int count = 0;
        for (int i = 0; i < 5 && input_index < length; ++i) {
            if (isspace(input[input_index])) {
                // Skip whitespace within the group
                input_index++;
                continue;
            }

            char ch = input[input_index++];
            if (ch < 33 || ch > 117) {
                fmt_fprintf(stderr, "Error: Invalid character encountered in encododing_base85_decode.\n");
                free(decoded);
                return NULL; // Invalid character
            }

            acc = acc * 85 + (ch - 33);
            count++;
        }

        int padding = 0;
        if (count < 5) {
            padding = 5 - count;
            for (int i = 0; i < padding; i++) {
                acc = acc * 85 + 84; // Assume 'u' for padding, which is the highest value (84)
            }
        }

        for (int i = 3; i >= 0; --i) {
            if (i < padding) {
                break; // Ignore padding bytes
            }
            decoded[decoded_index++] = (acc >> (i * 8)) & 0xFF;
        }

        if (count < 5) {
            break; // End of data
        }
    }

    // Resize the output buffer to the actual decoded data length
    uint8_t* resized_decoded = realloc(decoded, decoded_index + 1); // +1 for null terminator, if needed
    if (!resized_decoded) {
        fmt_fprintf(stderr, "Error: Reallocation failed in encododing_base85_decode.\n");
        free(decoded);
        return NULL;
    }

    resized_decoded[decoded_index] = '\0'; // Null-terminate if treating as a C-style string

    return resized_decoded;
}

char *encoding_base58_encode(const void *data, size_t binsz) {
    if (!data) {
        fmt_fprintf(stderr, "Error: Invalid input data in encoding_base58_encode.\n");
        return NULL;
    }

    const uint8_t *bin = data;
    int carry;
    size_t i, j, high, zcount = 0;
    size_t size;

    while (zcount < binsz && !bin[zcount]) {
        ++zcount;
    }

    size = (binsz - zcount) * 138 / 100 + 1;
    uint8_t *buf = malloc(size * sizeof(uint8_t));
    if (!buf) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for buffer in encoding_base58_encode.\n");
        return NULL;
    }
    memset(buf, 0, size);

    for (i = zcount, high = size - 1; i < binsz; ++i, high = j) {
        for (carry = bin[i], j = size - 1; (j > high) || carry; --j) {
            carry += 256 * buf[j];
            buf[j] = carry % 58;
            carry /= 58;

            if (!j) {
                break;
            }
        }
    }

    for (j = 0; j < size && !buf[j]; ++j) {
        // Skip leading zeros in binary
    }

    size_t b58sz = zcount + size - j + 1;
    char *b58 = malloc(b58sz);
    if (!b58) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for Base58 encoding in encoding_base58_encode.\n");
        free(buf);
        return NULL;
    }
    if (zcount) {
        memset(b58, '1', zcount);
    }
    for (i = zcount; j < size; ++i, ++j) {
        b58[i] = b58digits_ordered[buf[j]];
    }
    b58[i] = '\0';

    free(buf);
    return b58;
}

char *encoding_base58_decode(const char *b58, size_t *binszp) {
    if (b58 == NULL || binszp == NULL) {
        fmt_fprintf(stderr, "Error: Invalid input or binszp pointer in encoding_base58_decode.\n");
        return NULL;
    }

    size_t b58sz = strlen(b58);
    size_t binsz = b58sz * 733 / 1000 + 1; // Rough estimate of binary size
    uint8_t *bin = malloc(binsz);
    if (!bin) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_base58_decode.\n");
        return NULL;
    }
    memset(bin, 0, binsz);

    size_t i, j;
    int carry;
    size_t high = binsz - 1;

    // Process the Base58 string
    for (i = 0; i < b58sz; ++i) {
        if (b58[i] & 0x80 || b58digits_map[(unsigned char)b58[i]] == -1) {
            fmt_fprintf(stderr, "Error: Invalid Base58 character encountered in encoding_base58_decode.\n");
            free(bin);
            return NULL; // Invalid Base58 character
        }

        for (carry = b58digits_map[(unsigned char)b58[i]], j = binsz - 1; (j > high) || carry; --j) {
            carry += 58 * bin[j];
            bin[j] = carry % 256;
            carry /= 256;

            if (!j) {
                break; // Avoid wraparound
            }
        }
        high = j;
    }

    for (j = 0; j < binsz && !bin[j]; ++j) {
        // Skip leading zeros in binary
    }

    *binszp = binsz - j;
    char *result = malloc(*binszp);

    if (!result) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for result in encoding_base58_decode.\n");
        free(bin);
        return NULL;
    }
    memcpy(result, bin + j, *binszp);

    free(bin);
    return result;
}

uint8_t* encoding_base91_decode(const char* encoded, size_t* decoded_length) {
    if (!encoded || !decoded_length) {
        fmt_fprintf(stderr, "Error: Invalid input or decoded_length pointer in encoding_base91_decode.\n");
        return NULL;
    }

    size_t len = strlen(encoded);
    *decoded_length = 0;
    uint8_t* decoded = malloc(len); // Max possible size
    if (!decoded) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_base91_decode.\n");
        return NULL;
    }

    int v = -1;
    int b = 0;
    int n = 0;
    size_t index = 0;

    for (size_t i = 0; i < len; ++i) {
        int c = base91_decode_value(encoded[i]);
        if (c == -1) {
            fmt_fprintf(stderr, "Error: Invalid character encountered in encoding_base91_decode.\n");
            free(decoded);
            return NULL; // Invalid character
        }

        if (v < 0) {
            v = c;
        } 
        else {
            v += c * 91;
            b |= v << n;
            n += (v & 8191) > 88 ? 13 : 14;

            while (n > 7) {
                decoded[index++] = (uint8_t)(b & 255);
                b >>= 8;
                n -= 8;
            }
            v = -1;
        }
    }

    if (v != -1) {
        if (index >= len) {
            fmt_fprintf(stderr, "Error: Decoded index out of bounds in encoding_base91_decode.\n");
            free(decoded);
            return NULL;
        }
        decoded[index++] = (uint8_t)((b | v << n) & 255);
    }

    *decoded_length = index;
    return decoded;
}

char* encoding_base91_encode(const uint8_t* data, size_t length) {
    if (!data || length == 0) {
        fmt_fprintf(stderr, "Error: Invalid input data or length in encoding_base91_encode.\n");
        return NULL;
    }

    size_t estimated_length = length * 1.23 + 2; // +2 for padding and null terminator
    char* encoded = malloc(estimated_length);
    if (!encoded) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in encoding_base91_encode.\n");
        return NULL;
    }

    size_t index = 0;
    int b = 0;
    int n = 0;
    int v;

    for (size_t i = 0; i < length; ++i) {
        b |= (data[i] << n);
        n += 8;

        if (n > 13) {
            v = b & 8191;

            if (v > 88) {
                b >>= 13;
                n -= 13;
            } 
            else {
                v = b & 16383;
                b >>= 14;
                n -= 14;
            }

            if (index + 2 < estimated_length) {
                encoded[index++] = BASE91_ALPHABET[v % 91];
                encoded[index++] = BASE91_ALPHABET[v / 91];
            } 
            else {
                fmt_fprintf(stderr, "Error: Encoding index out of bounds in encoding_base91_encode.\n");
                free(encoded);
                return NULL;
            }
        }
    }

    if (n) {
        if (index + 1 < estimated_length) {
            encoded[index++] = BASE91_ALPHABET[b % 91];
        }
        if (n > 7 || b > 90) {
            if (index + 1 < estimated_length) {
                encoded[index++] = BASE91_ALPHABET[b / 91];
            }
        }
    }

    encoded[index] = '\0';
    return encoded;
}

#if defined(_WIN32) || defined(_WIN64)
// Function to convert UTF-8 string to wchar_t string (Windows only)
wchar_t* encoding_utf8_to_wchar(const char* utf8Str) {
    if (utf8Str == NULL) {
        fmt_fprintf(stderr, "Error: Input string is NULL\n");
        return NULL;
    }

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, NULL, 0);
    if (size_needed == 0) {
        fmt_fprintf(stderr, "Error: MultiByteToWideChar failed to calculate size. Error: %lu\n", GetLastError());
        return NULL;
    }

    wchar_t* wstr = (wchar_t*)malloc(size_needed * sizeof(wchar_t));
    if (!wstr) {
        fmt_fprintf(stderr, "Error: Cannot allocate memory for wchar\n");
        return NULL;
    }

    int result = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, wstr, size_needed);
    if (result == 0) {
        fmt_fprintf(stderr, "Error: Conversion from UTF-8 to wchar failed\n");
        free(wstr);
        return NULL;
    }
    return wstr;
}

char* encoding_wchar_to_utf8(const wchar_t* wstr) {
    if (wstr == NULL) {
        fmt_fprintf(stderr, "Error: Input wchar string is NULL\n");
        return NULL;
    }

    // Get the length of the required buffer
    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (utf8Length == 0) {
        fmt_fprintf(stderr, "Error: WideCharToMultiByte failed to calculate length\n");
        return NULL;
    }

    char* utf8Str = malloc(utf8Length * sizeof(char));
    if (!utf8Str) {
        fmt_fprintf(stderr, "Error: Cannot allocate memory for UTF-8 string\n");
        return NULL;
    }
    // Convert the wide-character string to UTF-8
    if (WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8Str, utf8Length, NULL, NULL) == 0) {
        fmt_fprintf(stderr, "Error: Conversion from wchar to UTF-8 failed\n");
        free(utf8Str);
        return NULL;
    }

    return utf8Str;
}
#endif

void encoding_initialize(void) {
    setlocale(LC_ALL, "");
}