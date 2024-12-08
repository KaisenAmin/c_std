/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Encoding
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>
#include "encoding.h"



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

/**
 * This function finds the index of a character within the BASE91_ALPHABET array, which is used for Base91 decoding. 
 * If the character is not found, it returns -1 to indicate an invalid character.
 */
static int base91_decode_value(char c) {
    ENCODING_LOG("[base91_decode_value] Decoding character: %c", c);

    for (int i = 0; i < 91; ++i) {
        if (BASE91_ALPHABET[i] == c) {
            ENCODING_LOG("[base91_decode_value] Character '%c' decoded to: %d", c, i);
            return i;
        }
    }

    ENCODING_LOG("[base91_decode_value] Error: Character '%c' not found in BASE91_ALPHABET", c);
    return -1;
}

/**
 * This function verifies whether a given sequence of bytes is valid UTF-8 by checking the leading 
 * byte and its corresponding continuation bytes. It handles various cases, including overlong sequences 
 * and invalid characters, ensuring the input adheres to UTF-8 encoding rules.
 */
bool encoding_is_utf8(const uint8_t* input, size_t length) {
    ENCODING_LOG("[encoding_is_utf8] Validating UTF-8 encoding for input sequence of length: %zu", length);

    uint8_t a;
    const uint8_t *srcptr = input + length;

    switch (length) {
    default:
        ENCODING_LOG("[encoding_is_utf8] Error: Invalid sequence length");
        return false;
        /* Everything else falls through when "true"... */
    case 4:
        if ((a = (*--srcptr)) < 0x80 || a > 0xBF) {
            ENCODING_LOG("[encoding_is_utf8] Error: Invalid continuation byte in 4-byte sequence");
            return false;
        }
    /* fall through */
    case 3:
        if ((a = (*--srcptr)) < 0x80 || a > 0xBF) {
            ENCODING_LOG("[encoding_is_utf8] Error: Invalid continuation byte in 3-byte sequence");
            return false;
        }
    /* fall through */
    case 2:
        if ((a = (*--srcptr)) < 0x80 || a > 0xBF) {
            ENCODING_LOG("[encoding_is_utf8] Error: Invalid continuation byte in 2-byte sequence");
            return false;
        }
    /* fall through */

        switch (*input) {
            case 0xE0:
                if (a < 0xA0) {
                    ENCODING_LOG("[encoding_is_utf8] Error: Overlong encoding for 0xE0");
                    return false;
                }
                break;
            case 0xED:
                if (a > 0x9F) {
                    ENCODING_LOG("[encoding_is_utf8] Error: Invalid character for 0xED");
                    return false;
                }
                break;
            case 0xF0:
                if (a < 0x90) {
                    ENCODING_LOG("[encoding_is_utf8] Error: Overlong encoding for 0xF0");
                    return false;
                }
                break;
            case 0xF4:
                if (a > 0x8F) {
                    ENCODING_LOG("[encoding_is_utf8] Error: Invalid character for 0xF4");
                    return false;
                }
                break;
            default:
                if (a < 0x80) {
                    ENCODING_LOG("[encoding_is_utf8] Error: Invalid leading byte in UTF-8 sequence");
                    return false;
                }
            /* fall through */
        }
    /* fall through */
    case 1:
        if (*input >= 0x80 && *input < 0xC2) {
            ENCODING_LOG("[encoding_is_utf8] Error: Overlong sequence detected");
            return false;
        }
    }

    if (*input > 0xF4) {
        ENCODING_LOG("[encoding_is_utf8] Error: Invalid leading byte greater than 0xF4");
        return false;
    }

    ENCODING_LOG("[encoding_is_utf8] Input sequence is valid UTF-8");
    return true;
}

/**
 * This function converts a UTF-16 encoded string to UTF-8. It handles surrogate pairs by converting 
 * them to their corresponding UTF-32 representation before encoding them as UTF-8, ensuring proper handling 
 * of all UTF-16 characters, including edge cases.
 */
static ConversionResult ConvertUTF16toUTF8 (
        const uint16_t** sourceStart, const uint16_t* sourceEnd, 
        uint8_t** targetStart, uint8_t* targetEnd, ConversionFlags flags) {
    
    ENCODING_LOG("[ConvertUTF16toUTF8] Starting UTF-16 to UTF-8 conversion");

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
                    ENCODING_LOG("[ConvertUTF16toUTF8] Successfully decoded surrogate pair: 0x%X", ch);
                } 
                else if (flags == strictConversion) { /* it's an unpaired high surrogate */
                    ENCODING_LOG("[ConvertUTF16toUTF8] Error: Unpaired high surrogate in strict mode");
                    --source; /* return to the illegal value itself */
                    result = sourceIllegal;
                    break;
                }
            } 
            else { /* We don't have the 16 bits following the high surrogate. */
                ENCODING_LOG("[ConvertUTF16toUTF8] Error: Source exhausted, missing low surrogate");
                --source; /* return to the high surrogate */
                result = sourceExhausted;
                break;
            }
        } 
        else if (flags == strictConversion) {
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_LOW_START && ch <= UNI_SUR_LOW_END) {
                ENCODING_LOG("[ConvertUTF16toUTF8] Error: Unpaired low surrogate in strict mode");
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
            ENCODING_LOG("[ConvertUTF16toUTF8] Error: Illegal UTF-16 code point, replacing with replacement character");                  
            bytesToWrite = 3;
            ch = UNI_REPLACEMENT_CHAR;
        }

        target += bytesToWrite;
        if (target > targetEnd) {
            ENCODING_LOG("[ConvertUTF16toUTF8] Error: Target buffer exhausted");
            source = oldSource; /* Back up source pointer! */
            target -= bytesToWrite; result = targetExhausted; 
            break;
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
        ENCODING_LOG("[ConvertUTF16toUTF8] Successfully wrote %d bytes for character: 0x%X", bytesToWrite, ch);
    }
    *sourceStart = source;
    *targetStart = target;
    ENCODING_LOG("[ConvertUTF16toUTF8] Conversion completed");

    return result;
}

/**
 * This function converts a UTF-32 encoded string to UTF-8. It processes each UTF-32 character,
 * determining how many bytes are needed in UTF-8, and encodes the character accordingly, 
 * while replacing any illegal UTF-32 values with a replacement character.
 */
static ConversionResult ConvertUTF32toUTF8 (
        const uint32_t** sourceStart, const uint32_t* sourceEnd, 
        uint8_t** targetStart, uint8_t* targetEnd, ConversionFlags flags) {
    
    ENCODING_LOG("[ConvertUTF32toUTF8] Starting UTF-32 to UTF-8 conversion");
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
                ENCODING_LOG("[ConvertUTF32toUTF8] Error: Illegal surrogate in strict mode at 0x%X", ch);
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
            ENCODING_LOG("[ConvertUTF32toUTF8] Error: Illegal UTF-32 code point, replaced with replacement char");
        }
        
        target += bytesToWrite;
        if (target > targetEnd) {
            --source; /* Back up source pointer! */
            target -= bytesToWrite; result = targetExhausted; 
            break;
            ENCODING_LOG("[ConvertUTF32toUTF8] Error: Target buffer exhausted");
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
        ENCODING_LOG("[ConvertUTF32toUTF8] Successfully wrote %d bytes for character: 0x%X", bytesToWrite, ch);
    }
    *sourceStart = source;
    *targetStart = target;
    ENCODING_LOG("[ConvertUTF32toUTF8] Conversion completed");

    return result;
}

/**
 * This function converts a UTF-8 encoded string to UTF-16. It iterates through the source UTF-8 string, 
 * decodes each character, and checks for any invalid sequences. It handles cases where characters need to be split 
 * into surrogate pairs in UTF-16 or replaced with a replacement character for illegal sequences.
 */
static ConversionResult ConvertUTF8toUTF16 (
        const uint8_t** sourceStart, const uint8_t* sourceEnd, 
        uint16_t** targetStart, uint16_t* targetEnd, ConversionFlags flags) {

    ENCODING_LOG("[ConvertUTF8toUTF16] Starting UTF-8 to UTF-16 conversion");
    ConversionResult result = conversionOK;
    const uint8_t* source = *sourceStart;
    uint16_t* target = *targetStart;

    while (source < sourceEnd) {
        uint32_t ch = 0;
        unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
        
        if (extraBytesToRead >= sourceEnd - source) {
            ENCODING_LOG("[ConvertUTF8toUTF16] Error: Source exhausted");
            result = sourceExhausted; 
            break;
        }
        /* Do this check whether lenient or strict */
        if (!encoding_is_utf8(source, extraBytesToRead + 1)) {
            ENCODING_LOG("[ConvertUTF8toUTF16] Error: Illegal UTF-8 sequence detected");
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
            ENCODING_LOG("[ConvertUTF8toUTF16] Error: Target buffer exhausted");
            source -= (extraBytesToRead+1); /* Back up source pointer! */
            result = targetExhausted; break;
        }
        if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (flags == strictConversion) {
                    ENCODING_LOG("[ConvertUTF8toUTF16] Error: Illegal surrogate found in strict mode");
                    source -= (extraBytesToRead+1); /* return to the illegal value itself */
                    result = sourceIllegal;
                    break;
                } 
                else {
                    ENCODING_LOG("[ConvertUTF8toUTF16] Replacing illegal surrogate with replacement character");
                    *target++ = UNI_REPLACEMENT_CHAR;
                }
            } 
            else {
                ENCODING_LOG("[ConvertUTF8toUTF16] Successfully decoded character: 0x%X", ch);
                *target++ = (uint16_t)ch; /* normal case */
            }
        } 
        else if (ch > UNI_MAX_UTF16) {
            if (flags == strictConversion) {
                ENCODING_LOG("[ConvertUTF8toUTF16] Error: Illegal UTF-16 code point in strict mode");
                result = sourceIllegal;
                source -= (extraBytesToRead+1); /* return to the start */
                break; /* Bail out; shouldn't continue */
            } 
            else {
                ENCODING_LOG("[ConvertUTF8toUTF16] Replacing out-of-range character with replacement character");
                *target++ = UNI_REPLACEMENT_CHAR;
            }
        } 
        else {
            /* target is a character in range 0xFFFF - 0x10FFFF. */
            if (target + 1 >= targetEnd) {
                source -= (extraBytesToRead+1); /* Back up source pointer! */
                ENCODING_LOG("[ConvertUTF8toUTF16] Error: Target buffer exhausted for surrogate pair");
                result = targetExhausted; break;
            }
            ch -= halfBase;
            *target++ = (uint16_t)((ch >> halfShift) + UNI_SUR_HIGH_START);
            *target++ = (uint16_t)((ch & halfMask) + UNI_SUR_LOW_START);
            ENCODING_LOG("[ConvertUTF8toUTF16] Successfully decoded surrogate pair: 0x%X", ch);
        }
    }
    *sourceStart = source;
    *targetStart = target;
    ENCODING_LOG("[ConvertUTF8toUTF16] Conversion completed");

    return result;
}

/**
 * This function converts a UTF-8 encoded string to UTF-32. It processes the UTF-8 input by decoding each character, 
 * verifying its legality, and then directly storing it in the UTF-32 output. It also replaces any illegal 
 * sequences with a replacement character and ensures no overflow occurs in the output buffer.
 */
ConversionResult ConvertUTF8toUTF32 (
        const uint8_t** sourceStart, const uint8_t* sourceEnd, 
        uint32_t** targetStart, uint32_t* targetEnd, ConversionFlags flags) {

    ENCODING_LOG("[ConvertUTF8toUTF32] Starting UTF-8 to UTF-32 conversion");
    ConversionResult result = conversionOK;
    const uint8_t* source = *sourceStart;
    uint32_t* target = *targetStart;
    
    while (source < sourceEnd) {
        uint32_t ch = 0;
        unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
        if (extraBytesToRead >= sourceEnd - source) {
            ENCODING_LOG("[ConvertUTF8toUTF32] Error: Source exhausted");
            result = sourceExhausted; 
            break;
        }
        /* Do this check whether lenient or strict */
        if (!encoding_is_utf8(source, extraBytesToRead+1)) {
            ENCODING_LOG("[ConvertUTF8toUTF32] Error: Illegal UTF-8 sequence detected");
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
            ENCODING_LOG("[ConvertUTF8toUTF32] Error: Target buffer exhausted");
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
                    ENCODING_LOG("[ConvertUTF8toUTF32] Error: Illegal surrogate found in strict mode");
                    source -= (extraBytesToRead+1); /* return to the illegal value itself */
                    result = sourceIllegal;
                    break;
                } 
                else {
                    ENCODING_LOG("[ConvertUTF8toUTF32] Replacing illegal surrogate with replacement character");
                    *target++ = UNI_REPLACEMENT_CHAR;
                }
            } 
            else {
                *target++ = ch;
                ENCODING_LOG("[ConvertUTF8toUTF32] Successfully decoded character: 0x%X", ch);
            }
        } 
        else { /* i.e., ch > UNI_MAX_LEGAL_UTF32 */
            ENCODING_LOG("[ConvertUTF8toUTF32] Error: Invalid UTF-32 code point");
            result = sourceIllegal;
            *target++ = UNI_REPLACEMENT_CHAR;
        }
    }
    *sourceStart = source;
    *targetStart = target;
    ENCODING_LOG("[ConvertUTF8toUTF32] Conversion completed");
    return result;
}

/**
 * This function decodes a single Base32 character. If the character is between 'A' and 'Z', 
 * it returns a value between 0 and 25. If the character is between '2' and '7', 
 * it returns a value between 26 and 31. If the character is invalid, it returns -1.
*/
static int decode_char(unsigned char c) {
    ENCODING_LOG("[decode_char] Decoding character: %c", c);
    char retval = -1;

    if (c >= 'A' && c <= 'Z') {
        retval = c - 'A';
        ENCODING_LOG("[decode_char] Character '%c' decoded to: %d", c, retval);
    } 
    else if (c >= '2' && c <= '7') {
        retval = c - '2' + 26;
        ENCODING_LOG("[decode_char] Character '%c' decoded to: %d", c, retval);
    }

    assert(retval == -1 || ((retval & 0x1F) == retval));
    if (retval == -1) {
        ENCODING_LOG("[decode_char] Error: Invalid character '%c'", c);
    }
    
    return retval;
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
    ENCODING_LOG("[get_octet] Calculating octet for block: %d", block);
    assert(block >= 0 && block < 8);
    
    int octet = (block * 5) / 8;
    ENCODING_LOG("[get_octet] Block %d starts in octet: %d", block, octet);
    
    return octet;
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
    ENCODING_LOG("[get_offset] Calculating offset for block: %d", block);
    assert(block >= 0 && block < 8);
    int offset = (8 - 5 - (5 * block) % 8);
    ENCODING_LOG("[get_offset] Calculated offset for block %d is: %d", block, offset);

    return offset;
}


/**
 * Like "b >> offset" but it will do the right thing with negative offset.
 * We need this as bitwise shifting by a negative offset is undefined
 * behavior.
 */
static unsigned char shift_right(unsigned char byte, signed char offset) {
    ENCODING_LOG("[shift_right] Shifting byte %02x to the right by %d bits", byte, offset);

    unsigned char result;
    if (offset > 0) {
        result = byte >> offset;
    } 
    else {
        result = byte << -offset;
    }

    ENCODING_LOG("[shift_right] Result after shifting: %02x", result);
    return result;
}

/**
 * Shift the byte to the left by the specified offset
 * This function calls shift_right with the negative of the offset,
 * effectively performing a left shift.
 */
static unsigned char shift_left(unsigned char byte, signed char offset) {
    ENCODING_LOG("[shift_left] Shifting byte %02x to the left by %d bits", byte, offset);
    unsigned char result = shift_right(byte, -offset);
    ENCODING_LOG("[shift_left] Result after shifting: %02x", result);

    return result;
}

static int decode_sequence(const unsigned char *coded, unsigned char *plain) {
    ENCODING_LOG("[decode_sequence] Starting sequence decoding");

    assert(CHAR_BIT == 8);
    assert(coded && plain);

    plain[0] = 0;
    for (int block = 0; block < 8; block++) {
        int offset = get_offset(block);
        int octet = get_octet(block);

        ENCODING_LOG("[decode_sequence] Decoding block %d, offset: %d, octet: %d", block, offset, octet);

        int c = decode_char(coded[block]);
        if (c < 0) {
            ENCODING_LOG("[decode_sequence] Invalid character at block %d", block);
            return octet;
        }

        plain[octet] |= shift_left(c, offset);
        if (offset < 0) {
            assert(octet < 4);
            plain[octet + 1] = shift_left(c, 8 + offset);
            ENCODING_LOG("[decode_sequence] Block overflows to next octet.");
        }
    }

    ENCODING_LOG("[decode_sequence] Decoding completed successfully");
    return 5;
}

/**
 * @brief Encodes binary data into Base64 format.
 *
 * This function encodes a given input string into Base64 format. Base64 encoding 
 * represents binary data in an ASCII string format by translating it into a radix-64 
 * representation. The output string is null-terminated.
 *
 * @param input Pointer to the input binary data.
 * @param length Length of the input data.
 * 
 * @return A dynamically allocated buffer containing the Base64 encoded string. 
 * Returns NULL if an error occurs (e.g., memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned encoded string.
 */
char* encoding_base64_encode(const char* input, size_t length) {
    ENCODING_LOG("[encoding_base64_encode] Starting Base64 encoding");

    size_t output_length = 4 * ((length + 2) / 3);
    char* encoded = (char*) malloc(output_length + 1); // +1 for null terminator
    if (!encoded) {
        ENCODING_LOG("[encoding_base64_encode] Error: Memory allocation failed.");
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

    // Add padding
    for (size_t k = 0; k < (3 - length % 3) % 3; k++) {
        encoded[output_length - 1 - k] = '=';
    }

    encoded[output_length] = '\0';

    ENCODING_LOG("[encoding_base64_encode] Encoding completed successfully.");
    return encoded;
}

/**
 * @brief Decodes a Base64 encoded string back to binary data.
 *
 * This function decodes a Base64 encoded string into its original binary form. 
 * The input string length must be a multiple of 4.
 *
 * @param input Pointer to the Base64 encoded input string.
 * @param length Length of the Base64 encoded string. Must be a multiple of 4.
 * 
 * @return A dynamically allocated buffer containing the decoded binary data. 
 * Returns NULL if an error occurs (e.g., memory allocation failure, invalid input length).
 *
 * @note The caller is responsible for freeing the returned decoded data.
 */
char* encoding_base64_decode(const char* input, size_t length) {
    if (length % 4 != 0) {
        ENCODING_LOG("[encoding_base64_decode] Error: Invalid input length. Length must be a multiple of 4.");
        return NULL;
    }

    size_t output_length = length / 4 * 3;
    if (input[length - 1] == '=') {
        output_length--;
    }
    if (input[length - 2] == '=') {
        output_length--;
    }

    char* decoded = (char*) malloc(output_length + 1);
    if (!decoded) {
        ENCODING_LOG("[encoding_base64_decode] Error: Memory allocation failed.");
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

    ENCODING_LOG("[encoding_base64_decode] Decoding completed successfully.");
    return decoded;
}

/**
 * @brief Encodes a URL string by replacing special characters with their percent-encoded representation.
 *
 * This function encodes a URL string by replacing special characters with their 
 * percent-encoded representation. Characters that are alphanumeric or one of 
 * `-`, `.`, `_`, `~` are left unchanged.
 *
 * @param input Pointer to the input string.
 * @param length Length of the input string.
 * 
 * @return A dynamically allocated buffer containing the URL encoded string. 
 * Returns NULL if an error occurs (e.g., memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned encoded string.
 */
char* encoding_url_encode(const char* input, size_t length) {
    ENCODING_LOG("[encoding_url_encode] Starting URL encoding");

    char* result = (char*) malloc(3 * length + 1); // Worst case scenario, every character needs encoding
    if (!result) {
        ENCODING_LOG("[encoding_url_encode] Error: Memory allocation failed.");
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

    ENCODING_LOG("[encoding_url_encode] Encoding completed successfully.");
    return result;
}


/**
 * @brief Decodes a URL-encoded string.
 *
 * This function decodes a URL-encoded string where each `%xx` sequence is replaced 
 * by its corresponding character, and `+` is replaced by a space character. The 
 * decoded string is stored in a dynamically allocated buffer.
 *
 * @param input Pointer to the URL-encoded input string.
 * @param length Length of the input string.
 * @return A dynamically allocated buffer containing the decoded string. Returns 
 *         NULL if an error occurs (e.g., memory allocation failure, invalid percent-encoding).
 *
 * @note The caller is responsible for freeing the returned decoded string.
 */
char* encoding_url_decode(const char* input, size_t length) {
    ENCODING_LOG("[encoding_url_decode] Starting URL decoding");

    char* result = (char*) malloc(length + 1); // Decoded string will be equal or smaller in size
    if (!result) {
        ENCODING_LOG("[encoding_url_decode] Error: Memory allocation failed");
        return NULL;
    }

    size_t result_index = 0;
    for (size_t i = 0; i < length; ++i) {
        char ch = input[i];
        if (ch == '%') {
            if (i + 2 >= length) {
                ENCODING_LOG("[encoding_url_decode] Error: Incomplete percent-encoding");
                free(result);
                return NULL;
            }

            static const char hex[] = "0123456789ABCDEF";
            char hi = input[++i];
            char lo = input[++i];
            int hi_index = strchr(hex, toupper(hi)) - hex;
            int lo_index = strchr(hex, toupper(lo)) - hex;

            if (hi_index < 0 || hi_index >= 16 || lo_index < 0 || lo_index >= 16) {
                ENCODING_LOG("[encoding_url_decode] Error: Invalid hex characters in percent-encoding");
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

    ENCODING_LOG("[encoding_url_decode] Decoding completed successfully");
    return result;
}

/**
 * @brief Encodes binary data into Base32 format.
 *
 * This function encodes a given input string into Base32 format. Base32 encoding 
 * uses 32 characters to represent binary data. The output is padded with `=` characters 
 * if necessary to ensure the encoded string length is a multiple of 8.
 *
 * @param input Pointer to the input binary data.
 * @param length Length of the input data.
 * 
 * @return A dynamically allocated buffer containing the Base32 encoded string. 
 *         Returns NULL if an error occurs (e.g., memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned encoded string.
 */
char* encoding_base32_encode(const char* input, size_t length) {
    ENCODING_LOG("[encoding_base32_encode] Starting Base32 encoding");

    size_t output_length = ((length + 4) / 5) * 8; // Output length including padding
    char* encoded = (char*) malloc(output_length + 1);

    if (!encoded) {
        ENCODING_LOG("[encoding_base32_encode] Error: Memory allocation failed.");
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
    ENCODING_LOG("[encoding_base32_encode] Encoding completed successfully.");

    return encoded;
}


/**
 * @brief Decodes a Base32 encoded string back to binary data.
 *
 * This function decodes a Base32 encoded string into its original binary form. The 
 * input string must be properly padded with `=` characters to ensure it is a multiple 
 * of 8 characters long.
 *
 * @param input Pointer to the Base32 encoded input string.
 * @param length Length of the Base32 encoded string. Must be a multiple of 8.
 * 
 * @return A dynamically allocated buffer containing the decoded binary data. 
 *         Returns NULL if an error occurs (e.g., memory allocation failure, invalid input length).
 *
 * @note The caller is responsible for freeing the returned decoded data.
 */
char* encoding_base32_decode(const char* input, size_t length) {
    if (length % 8 != 0) {
        ENCODING_LOG("[encoding_base32_decode] Error: Invalid input length. Length must be a multiple of 8.");
        return NULL;
    }

    size_t olength = (length / 8) * 5;
    unsigned char* result = (unsigned char*) malloc(olength + 1);
    if (!result) {
        ENCODING_LOG("[encoding_base32_decode] Error: Memory allocation failed.");
        return NULL;
    }

    size_t i = 0, j = 0;
    while (i < length) {
        if (input[i] == '=') {
            break; 
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
    ENCODING_LOG("[encoding_base32_decode] Decoding completed successfully.");
    return (char*)result;
}


/**
 * @brief Encodes binary data into Base16 (hexadecimal) format.
 *
 * This function encodes a given input string into Base16 format, also known as 
 * hexadecimal encoding. Each byte of the input is represented by two hexadecimal 
 * characters in the output string.
 *
 * @param input Pointer to the input binary data.
 * @param length Length of the input data.
 * 
 * @return A dynamically allocated buffer containing the Base16 encoded string. 
 * Returns NULL if an error occurs (e.g., memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned encoded string.
 */
char* encoding_base16_encode(const char* input, size_t length) {
    size_t output_length = length * 2;
    char* encoded = (char*) malloc(output_length + 1);

    if (!encoded) {
        ENCODING_LOG("[encoding_base16_encode] Error: Memory allocation failed.");
        return NULL;
    }

    for (size_t i = 0, j = 0; i < length; ++i) {
        uint8_t ch = (uint8_t)input[i];
        encoded[j++] = base16_chars[(ch & 0xF0) >> 4];
        encoded[j++] = base16_chars[ch & 0x0F];
    }

    encoded[output_length] = '\0';
    ENCODING_LOG("[encoding_base16_encode] Encoding completed successfully.");
    return encoded;
}


/**
 * @brief Decodes a Base16 (hexadecimal) encoded string into its binary form.
 *
 * This function takes a string encoded in Base16 (hexadecimal) and decodes it 
 * into its original binary form. Each pair of hexadecimal characters is converted 
 * into a single byte.
 *
 * @param input Pointer to the Base16 encoded input string.
 * @param length Length of the Base16 encoded string. Must be an even number.
 * 
 * @return A dynamically allocated array containing the decoded binary data.
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure, invalid character).
 *
 * @note The caller is responsible for freeing the returned binary data.
 */
char* encoding_base16_decode(const char* input, size_t length) {
    if (input == NULL) {
        ENCODING_LOG("[encoding_base16_decode] Error: Invalid input parameter");
        return NULL;
    }

    static const unsigned char base16_decode[128] = {
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
        ENCODING_LOG("[encoding_base16_decode] Error: Invalid input length");
        return NULL; 
    }

    size_t olength = length / 2;
    char* decoded = (char*) malloc(olength + 1);
    if (!decoded) {
        ENCODING_LOG("[encoding_base16_decode] Error: Memory allocation failed for decoded string");
        return NULL;
    }

    for (size_t i = 0, j = 0; i < length;) {
        uint8_t a = base16_decode[(unsigned char)input[i++]];
        uint8_t b = base16_decode[(unsigned char)input[i++]];

        if (a == 0xFF || b == 0xFF) {
            ENCODING_LOG("[encoding_base16_decode] Error: Invalid character encountered");
            free(decoded);
            return NULL; // Invalid character
        }

        decoded[j++] = (a << 4) | b;
    }

    decoded[olength] = '\0';
    ENCODING_LOG("[encoding_base16_decode] Decoding completed successfully");
    return decoded;
}


/**
 * @brief Converts a UTF-32 encoded string to a UTF-16 encoded string.
 *
 * This function converts a string from UTF-32 encoding to UTF-16 encoding. 
 * UTF-32 uses fixed-width 32-bit code units, while UTF-16 uses variable-width encoding. 
 * The conversion may require allocating additional memory for the UTF-16 output, especially 
 * when handling characters outside the Basic Multilingual Plane (BMP) which are encoded as 
 * surrogate pairs in UTF-16.
 *
 * @param input Pointer to the UTF-32 encoded input string.
 * @param length Length of the UTF-32 encoded string in characters.
 * 
 * @return A dynamically allocated array containing the UTF-16 encoded string.
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure, invalid character).
 *
 * @note The caller is responsible for freeing the returned UTF-16 string.
 */
uint16_t* encoding_utf32_to_utf16(const uint32_t* input, size_t length) {
    if (input == NULL || length == 0) {
        ENCODING_LOG("[encoding_utf32_to_utf16] Error: Invalid input or length");
        return NULL;
    }

    ENCODING_LOG("[encoding_utf32_to_utf16] Starting UTF-32 to UTF-16 conversion");

    // Allocate maximum possible size (each UTF-32 character might become two UTF-16 characters)
    uint16_t* output = (uint16_t*) malloc(sizeof(uint16_t) * (length * 2 + 1));
    if (!output) {
        ENCODING_LOG("[encoding_utf32_to_utf16] Error: Memory allocation failed");
        return NULL;
    }

    size_t j = 0;
    for (size_t i = 0; i < length; ++i) {
        uint32_t ch = input[i];

        if (ch > UNI_MAX_LEGAL_UTF32) {
            ENCODING_LOG("[encoding_utf32_to_utf16] Error: Invalid character encountered");
            free(output);
            return NULL;
        }

        if (ch <= UNI_MAX_BMP) {
            ENCODING_LOG("[encoding_utf32_to_utf16] Info : Character can be directly converted to a single UTF-16 unit");
            output[j++] = (uint16_t)ch;
        } 
        else if (ch > UNI_MAX_BMP && ch <= UNI_MAX_UTF16) {
            ENCODING_LOG("[encoding_utf32_to_utf16] Info: Convert character to a surrogate pair");
            ch -= halfBase;
            output[j++] = (uint16_t)((ch >> halfShift) + UNI_SUR_HIGH_START);
            output[j++] = (uint16_t)((ch & halfMask) + UNI_SUR_LOW_START);
        }
    }

    output[j] = 0;
    ENCODING_LOG("[encoding_utf32_to_utf16] Conversion completed successfully, length: %zu", j);

    return output;
}

/**
 * @brief Converts a UTF-16 encoded string to a UTF-32 encoded string.
 *
 * This function converts a string from UTF-16 encoding to UTF-32 encoding. 
 * UTF-16 uses variable-width encoding, while UTF-32 uses fixed-width 32-bit code units. 
 * The conversion may require handling surrogate pairs in UTF-16 to reconstruct 
 * the original UTF-32 code points.
 *
 * @param input Pointer to the UTF-16 encoded input string.
 * @param length Length of the UTF-16 encoded string in characters.
 * 
 * @return A dynamically allocated array containing the UTF-32 encoded string.
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure, invalid surrogate pair).
 *
 * @note The caller is responsible for freeing the returned UTF-32 string.
 */
uint32_t* encoding_utf16_to_utf32(const uint16_t* input, size_t length) {
    if (input == NULL || length == 0) {
        ENCODING_LOG("[encoding_utf16_to_utf32] Error: Invalid input or length");
        return NULL;
    }
    ENCODING_LOG("[encoding_utf16_to_utf32] Starting UTF-16 to UTF-32 conversion");

    // Allocate memory for the worst-case scenario (all characters are non-surrogates)
    uint32_t* output = (uint32_t*) malloc(sizeof(uint32_t) * (length + 1));
    if (!output) {
        ENCODING_LOG("[encoding_utf16_to_utf32] Error: Memory allocation failed");
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
                    ENCODING_LOG("[encoding_utf16_to_utf32] Error: Invalid surrogate pair");
                    free(output);
                    return NULL;
                }
            } 
            else {
                ENCODING_LOG("[encoding_utf16_to_utf32] Error: Lone high surrogate without a low surrogate");
                free(output);
                return NULL;
            }
        }
        output[j++] = ch;
    }

    // Null-terminate the output
    output[j] = 0;
    ENCODING_LOG("[encoding_utf16_to_utf32] Conversion completed successfully, length: %zu", j);

    return output;
}

/**
 * @brief Converts a UTF-16 encoded string to a UTF-8 encoded string.
 *
 * This function converts a string from UTF-16 encoding to UTF-8 encoding. 
 * UTF-16 uses variable-width encoding, while UTF-8 uses a more compact variable-width encoding.
 * The conversion may require allocating additional memory for the UTF-8 output.
 *
 * @param input Pointer to the UTF-16 encoded input string.
 * @param length Length of the UTF-16 encoded string in characters.
 * 
 * @return A dynamically allocated array containing the UTF-8 encoded string.
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned UTF-8 string.
 */
uint8_t* encoding_utf16_to_utf8(const uint16_t* input, size_t length) {
    if (input == NULL || length == 0) {
        ENCODING_LOG("[encoding_utf16_to_utf8] Error: Invalid input or length");
        return NULL;
    }

    ENCODING_LOG("[encoding_utf16_to_utf8] Starting UTF-16 to UTF-8 conversion");

    // Estimate maximum output size (4 bytes per UTF-16 character)
    size_t maxOutLength = length * 4;
    uint8_t* output = (uint8_t*)malloc(maxOutLength);
    if (!output) {
        ENCODING_LOG("[encoding_utf16_to_utf8] Error: Memory allocation failed");
        return NULL;
    }

    const uint16_t* sourceStart = input;
    const uint16_t* sourceEnd = input + length;
    uint8_t* targetStart = output;
    uint8_t* targetEnd = output + maxOutLength;

    // Perform the UTF-16 to UTF-8 conversion using lenient conversion method
    ConversionResult result = ConvertUTF16toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);

    if (result != conversionOK) {
        ENCODING_LOG("[encoding_utf16_to_utf8] Error: Conversion from UTF-16 to UTF-8 failed");
        free(output);
        return NULL;
    }

    // Resize the output to the actual UTF-8 string length
    size_t actualLength = targetStart - output;
    uint8_t* resizedOutput = (uint8_t*)realloc(output, actualLength + 1);
    if (!resizedOutput) {
        ENCODING_LOG("[encoding_utf16_to_utf8] Error: Reallocation failed");
        free(output);
        return NULL;
    }

    resizedOutput[actualLength] = '\0'; // Null-terminate the UTF-8 string
    ENCODING_LOG("[encoding_utf16_to_utf8] Conversion completed successfully, length: %zu", actualLength);

    return resizedOutput;
}

/**
 * @brief Converts a UTF-32 encoded string to a UTF-8 encoded string.
 *
 * This function converts a string from UTF-32 encoding to UTF-8 encoding. 
 * UTF-32 uses fixed-width 32-bit code units, while UTF-8 uses variable-width encoding. 
 * The conversion may require allocating additional memory for the UTF-8 output.
 *
 * @param input Pointer to the UTF-32 encoded input string.
 * @param length Length of the UTF-32 encoded string in characters.
 * 
 * @return A dynamically allocated array containing the UTF-8 encoded string.
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned UTF-8 string.
 */
uint8_t* encoding_utf32_to_utf8(const uint32_t* input, size_t length) {
    if (input == NULL || length == 0) {
        ENCODING_LOG("[encoding_utf32_to_utf8] Error: Invalid input or length");
        return NULL;
    }
    ENCODING_LOG("[encoding_utf32_to_utf8] Starting UTF-32 to UTF-8 conversion");

    // Estimate maximum output size (4 bytes per UTF-32 character)
    size_t maxOutLength = length * 4;
    uint8_t* output = (uint8_t*)malloc(maxOutLength);
    if (!output) {
        ENCODING_LOG("[encoding_utf32_to_utf8] Error: Memory allocation failed");
        return NULL;
    }

    const uint32_t* sourceStart = input;
    const uint32_t* sourceEnd = input + length;
    uint8_t* targetStart = output;
    uint8_t* targetEnd = output + maxOutLength;

    // Perform the UTF-32 to UTF-8 conversion using lenient conversion method
    ConversionResult result = ConvertUTF32toUTF8(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);
    if (result != conversionOK) {
        ENCODING_LOG("[encoding_utf32_to_utf8] Error: Conversion from UTF-32 to UTF-8 failed");
        free(output);
        return NULL;
    }

    // Resize the output to the actual UTF-8 string length
    size_t actualLength = targetStart - output;
    uint8_t* resizedOutput = (uint8_t*)realloc(output, actualLength + 1);
    if (!resizedOutput) {
        ENCODING_LOG("[encoding_utf32_to_utf8] Error: Reallocation failed");
        free(output);
        return NULL;
    }

    resizedOutput[actualLength] = '\0'; // Null-terminate the UTF-8 string
    ENCODING_LOG("[encoding_utf32_to_utf8] Conversion completed successfully, length: %zu", actualLength);

    return resizedOutput;
}

/**
 * @brief Validates whether the provided string is a valid UTF-8 string.
 *
 * This function checks if a given byte sequence conforms to UTF-8 encoding rules.
 * It verifies that each character in the string is correctly encoded according to UTF-8 standards.
 *
 * @param input Pointer to a pointer to the UTF-8 encoded input string.
 * The pointer is updated to point to the end of the string after validation.
 * @param length Length of the UTF-8 encoded string in bytes.
 * 
 * @return True if the string is a valid UTF-8 encoded string, false otherwise.
 */
bool encoding_is_utf8_string(const uint8_t** input, size_t length) {
    if (input == NULL || *input == NULL || length == 0) {
        ENCODING_LOG("[encoding_is_utf8_string] Error: Invalid input or length");
        return false;
    }

    ENCODING_LOG("[encoding_is_utf8_string] Validating if the input is a valid UTF-8 string");

    const uint8_t* source = *input;
    const uint8_t* sourceEnd = source + length;

    while (source < sourceEnd) {
        int trailLength = trailingBytesForUTF8[*source] + 1;
        if (trailLength > sourceEnd - source || !encoding_is_utf8(source, trailLength)) {
            ENCODING_LOG("[encoding_is_utf8_string] Error: Invalid UTF-8 encoding detected");
            return false;
        }
        source += trailLength;
    }

    *input = source; // Update the input pointer to the end of the string
    ENCODING_LOG("[encoding_is_utf8_string] UTF-8 validation completed successfully");
    
    return true;
}

/**
 * @brief Converts a UTF-8 encoded string to a UTF-16 encoded string.
 *
 * This function converts a string from UTF-8 encoding to UTF-16 encoding. UTF-8 and UTF-16 
 * are both variable-length encoding schemes, and this conversion may require allocating 
 * additional memory for the UTF-16 output.
 *
 * @param input Pointer to the UTF-8 encoded input string.
 * @param length Length of the UTF-8 encoded string in bytes.
 * 
 * @return A dynamically allocated array containing the UTF-16 encoded string.
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned UTF-16 string.
 */
uint16_t* encoding_utf8_to_utf16(const uint8_t* input, size_t length) {
    if (input == NULL || length == 0) {
        ENCODING_LOG("[encoding_utf8_to_utf16] Error: Invalid input or length");
        return NULL;
    }

    ENCODING_LOG("[encoding_utf8_to_utf16] Starting UTF-8 to UTF-16 conversion");

    // Estimate maximum output size (each UTF-8 character can be at most 4 bytes,
    // but can translate to at most 2 UTF-16 characters)
    size_t maxOutLength = length * 2;
    uint16_t* output = (uint16_t*)malloc(maxOutLength * sizeof(uint16_t));
    if (!output) {
        ENCODING_LOG("[encoding_utf8_to_utf16] Error: Memory allocation failed for output");
        return NULL;
    }

    const uint8_t* sourceStart = input;
    const uint8_t* sourceEnd = input + length;
    uint16_t* targetStart = output;
    uint16_t* targetEnd = output + maxOutLength;

    // Perform the UTF-8 to UTF-16 conversion using lenient conversion method
    ConversionResult result = ConvertUTF8toUTF16(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);

    if (result != conversionOK) {
        ENCODING_LOG("[encoding_utf8_to_utf16] Error: Conversion from UTF-8 to UTF-16 failed");
        free(output);
        return NULL;
    }

    // Resize the output to the actual UTF-16 string length
    size_t actualLength = targetStart - output;
    uint16_t* resizedOutput = (uint16_t*)realloc(output, actualLength * sizeof(uint16_t) + 1);
    if (!resizedOutput) {
        ENCODING_LOG("[encoding_utf8_to_utf16] Error: Reallocation failed");
        free(output);
        return NULL;
    }

    resizedOutput[actualLength] = '\0'; // Null-terminate the UTF-16 string
    ENCODING_LOG("[encoding_utf8_to_utf16] Conversion completed successfully, length: %zu", actualLength);

    return resizedOutput;
}

/**
 * @brief Converts a UTF-8 encoded string to a UTF-32 encoded string.
 *
 * This function converts a string from UTF-8 encoding to UTF-32 encoding. UTF-8 is a variable-length encoding,
 * while UTF-32 uses fixed-width 32-bit code units. This conversion ensures that all characters are represented
 * as 32-bit integers.
 *
 * @param input Pointer to the UTF-8 encoded input string.
 * @param length Length of the UTF-8 encoded string in bytes.
 * 
 * @return A dynamically allocated array containing the UTF-32 encoded string.
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned UTF-32 string.
 */
uint32_t* encoding_utf8_to_utf32(const uint8_t* input, size_t length) {
    if (input == NULL || length == 0) {
        ENCODING_LOG("[encoding_utf8_to_utf32] Error: Invalid input or length");
        return NULL;
    }
    ENCODING_LOG("[encoding_utf8_to_utf32] Starting UTF-8 to UTF-32 conversion");

    // Estimate maximum output size (each UTF-8 character can be at most 4 bytes,
    // translating to a single UTF-32 character)
    size_t maxOutLength = length;
    uint32_t* output = (uint32_t*)malloc(maxOutLength * sizeof(uint32_t));
    if (!output) {
        ENCODING_LOG("[encoding_utf8_to_utf32] Error: Memory allocation failed");
        return NULL; // Memory allocation failed
    }

    const uint8_t* sourceStart = input;
    const uint8_t* sourceEnd = input + length;
    uint32_t* targetStart = output;
    uint32_t* targetEnd = output + maxOutLength;

    // Perform the UTF-8 to UTF-32 conversion using a lenient conversion method
    ConversionResult result = ConvertUTF8toUTF32(&sourceStart, sourceEnd, &targetStart, targetEnd, lenientConversion);
    if (result != conversionOK) {
        ENCODING_LOG("[encoding_utf8_to_utf32] Error: Conversion to UTF-32 failed");
        free(output);
        return NULL;
    }

    // Resize the output to the actual UTF-32 string length
    size_t actualLength = targetStart - output;
    uint32_t* resizedOutput = (uint32_t*)realloc(output, (actualLength + 1) * sizeof(uint32_t));
    if (resizedOutput) {
        resizedOutput[actualLength] = 0; // Null-terminate the UTF-32 string
        ENCODING_LOG("[encoding_utf8_to_utf32] Conversion completed successfully, length: %zu", actualLength);
        return resizedOutput;
    }

    ENCODING_LOG("[encoding_utf8_to_utf32] Error: Memory reallocation failed");
    return output;
}

/**
 * @brief Converts a UTF-8 encoded string to a UTF-32 encoded string.
 *
 * This function converts a string from UTF-8 encoding to UTF-32 encoding. UTF-8 is a variable-length encoding,
 * while UTF-32 uses fixed-width 32-bit code units. This conversion ensures that all characters are represented
 * as 32-bit integers.
 *
 * @param input Pointer to the UTF-8 encoded input string.
 * @param length Length of the UTF-8 encoded string in bytes.
 * 
 * @return A dynamically allocated array containing the UTF-32 encoded string.
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned UTF-32 string.
 */
void encoding_hex_dump(const void *data, size_t size) {
    if (!data || size == 0) {
        ENCODING_LOG("[encoding_hex_dump] Error: Invalid input data or size");
        return;
    }
    ENCODING_LOG("[encoding_hex_dump] Starting hex dump of size %zu bytes", size);

    const unsigned char *byte = (const unsigned char *)data;
    size_t i, j;

    for (i = 0; i < size; i += 16) {
        printf("%08zx  ", i); // Print the offset

        // Print hex values
        for (j = 0; j < 16; j++) {
            if (i + j < size) {
                printf("%02x ", byte[i + j]);
            } 
            else {
                printf("   "); // Fill space if less than 16 bytes in a line
            }
        }
        printf(" |");

        // Print ASCII representation
        for (j = 0; j < 16; j++) {
            if (i + j < size) {
                printf("%c", isprint(byte[i + j]) ? byte[i + j] : '.');
            }
        }
        printf("|\n");
    }

    ENCODING_LOG("[encoding_hex_dump] Hex dump completed successfully");
}

/**
 * @brief Encodes binary data into a Base85 encoded string.
 *
 * Base85 is an encoding method used to encode binary data into ASCII text, often used 
 * in contexts where binary data needs to be handled as text, such as in Adobe's PostScript.
 *
 * @param input Pointer to the binary data to be encoded.
 * @param length The size of the binary data in bytes.
 * 
 * @return A dynamically allocated string containing the Base85 encoded representation.
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned encoded string.
 */
char* encododing_base85_encode(const uint8_t* input, size_t length) {
    if (input == NULL || length == 0) {
        ENCODING_LOG("[encododing_base85_encode] Error: Invalid input or length");
        return NULL;
    }
    ENCODING_LOG("[encododing_base85_encode] Starting Base85 encoding");

    // Calculate the maximum possible length of the encoded string
    size_t encoded_max_length = ((length + 3) / 4) * 5 + 2; // +2 for potential padding and null terminator
    char* encoded = (char*) malloc(encoded_max_length);
    if (!encoded) {
        ENCODING_LOG("[encododing_base85_encode] Error: Memory allocation failed for encoded string");
        return NULL;
    }

    size_t input_index = 0;
    size_t encoded_index = 0;
    while (input_index < length) {
        uint32_t acc = 0;
        size_t chunk_len = (length - input_index < 4) ? (length - input_index) : 4;

        // Pack 4 bytes into acc
        for (size_t i = 0; i < chunk_len; ++i) {
            acc = (acc << 8) | input[input_index++];
        }
        if (chunk_len < 4) { // Add padding if the chunk is less than 4 bytes
            acc <<= (4 - chunk_len) * 8; // Padding
        }
        if (acc == 0 && chunk_len == 4) { // Special case for 'z' when 4 bytes are zero
            encoded[encoded_index++] = 'z';
        } 
        else {
            for (int i = 4; i >= 0; --i) {
                encoded[encoded_index + i] = (acc % 85) + 33;
                acc /= 85;
            }
            encoded_index += 5;

            if (chunk_len < 4) {
                encoded_index -= (4 - chunk_len);  
                break;
            }
        }
    }

    encoded[encoded_index] = '\0';
    ENCODING_LOG("[encododing_base85_encode] Encoding completed successfully");

    return encoded;
}

/**
 * @brief Decodes a Base85 encoded string back into its original binary form.
 *
 * Base85 decoding interprets the encoded text and converts it back into the binary data
 * it originally represented.
 *
 * @param input Pointer to the Base85 encoded string.
 * @param length The length of the encoded string.
 * 
 * @return A dynamically allocated array containing the decoded binary data.
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned decoded data.
 */
uint8_t* encododing_base85_decode(const char* input, size_t length) {
    if (input == NULL || length == 0) {
        ENCODING_LOG("[encododing_base85_decode] Error: Invalid input or length");
        return NULL;
    }

    ENCODING_LOG("[encododing_base85_decode] Starting Base85 decoding");

    // Calculate the maximum possible length of the decoded string
    size_t decoded_max_length = (length / 5) * 4;
    uint8_t* decoded = (uint8_t*) malloc(decoded_max_length);
    if (!decoded) {
        ENCODING_LOG("[encododing_base85_decode] Error: Memory allocation failed for decoded string");
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
                ENCODING_LOG("[encododing_base85_decode] Error: Invalid character encountered");
                free(decoded);
                return NULL; 
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
    uint8_t* resized_decoded = (uint8_t*) realloc(decoded, decoded_index + 1); // +1 for null terminator, if needed
    if (!resized_decoded) {
        ENCODING_LOG("[encododing_base85_decode] Error: Reallocation failed");
        free(decoded);
        return NULL;
    }

    resized_decoded[decoded_index] = '\0'; 
    ENCODING_LOG("[encododing_base85_decode] Decoding completed successfully");

    return resized_decoded;
}

/**
 * @brief Encodes binary data into a Base58 encoded string.
 *
 * Base58 is commonly used in cryptocurrencies such as Bitcoin. It is designed to avoid 
 * visually similar characters and is not case-sensitive.
 *
 * @param data Pointer to the binary data to be encoded.
 * @param binsz The size of the binary data in bytes.
 * 
 * @return A dynamically allocated string containing the Base58 encoded representation. 
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned encoded string.
 */
char *encoding_base58_encode(const void *data, size_t binsz) {
    if (!data) {
        ENCODING_LOG("[encoding_base58_encode] Error: Invalid input data");
        return NULL;
    }

    ENCODING_LOG("[encoding_base58_encode] Starting Base58 encoding");

    const uint8_t *bin = (const uint8_t*)data;
    int carry;
    size_t i, j, high, zcount = 0;
    size_t size;

    // Count leading zero bytes
    while (zcount < binsz && !bin[zcount]) {
        ++zcount;
    }

    // Estimate size of Base58 encoded string
    size = (binsz - zcount) * 138 / 100 + 1;
    uint8_t *buf = (uint8_t*) malloc(size * sizeof(uint8_t));
    if (!buf) {
        ENCODING_LOG("[encoding_base58_encode] Error: Memory allocation failed for buffer");
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

    // Skip leading zeros in the buffer
    for (j = 0; j < size && !buf[j]; ++j) {
        // Skip leading zeros in binary
    }

    size_t b58sz = zcount + size - j + 1;
    char *b58 = (char*) malloc(b58sz);
    if (!b58) {
        ENCODING_LOG("[encoding_base58_encode] Error: Memory allocation failed for Base58 encoded result");
        free(buf);
        return NULL;
    }

    // Handle leading zeros in the input
    if (zcount) {
        memset(b58, '1', zcount);
    }

    for (i = zcount; j < size; ++i, ++j) {
        b58[i] = b58digits_ordered[buf[j]];
    }
    b58[i] = '\0';

    ENCODING_LOG("[encoding_base58_encode] Encoding completed successfully");
    
    free(buf);
    return b58;
}


/**
 * @brief Decodes a Base58 encoded string back into its original binary form.
 *
 * @param b58 Pointer to the Base58 encoded string.
 * @param binszp Pointer to a size_t variable where the length of the decoded data will be stored.
 * 
 * @return A dynamically allocated array containing the decoded binary data.
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 */
char *encoding_base58_decode(const char *b58, size_t *binszp) {
    if (b58 == NULL || binszp == NULL) {
        ENCODING_LOG("[encoding_base58_decode] Error: Invalid input or binszp pointer");
        return NULL;
    }

    ENCODING_LOG("[encoding_base58_decode] Starting Base58 decoding");

    size_t b58sz = strlen(b58);
    size_t binsz = b58sz * 733 / 1000 + 1; // Rough estimate of binary size
    uint8_t *bin = (uint8_t*) malloc(binsz);
    if (!bin) {
        ENCODING_LOG("[encoding_base58_decode] Error: Memory allocation failed");
        return NULL;
    }
    memset(bin, 0, binsz);

    size_t i, j;
    int carry;
    size_t high = binsz - 1;

    // Process the Base58 string
    for (i = 0; i < b58sz; ++i) {
        if (b58[i] & 0x80 || b58digits_map[(unsigned char)b58[i]] == -1) {
            ENCODING_LOG("[encoding_base58_decode] Error: Invalid Base58 character encountered");
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
    }

    *binszp = binsz - j;
    char *result = (char*) malloc(*binszp);
    if (!result) {
        ENCODING_LOG("[encoding_base58_decode] Error: Memory allocation failed for result");
        free(bin);
        return NULL;
    }

    memcpy(result, bin + j, *binszp);
    ENCODING_LOG("[encoding_base58_decode] Decoding completed successfully, decoded length: %zu", *binszp);

    free(bin);
    return result;
}

/**
 * @brief Decodes a Base91 encoded string into its original binary form.
 *
 * @param encoded Pointer to the Base91 encoded string.
 * @param decoded_length Pointer to a size_t variable where the length of the decoded data will be stored.
 * 
 * @return A dynamically allocated array of uint8_t containing the decoded data. 
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned decoded data.
 */
uint8_t* encoding_base91_decode(const char* encoded, size_t* decoded_length) {
    if (!encoded || !decoded_length) {
        ENCODING_LOG("[encoding_base91_decode] Error: Invalid input or decoded_length pointer");
        return NULL;
    }

    ENCODING_LOG("[encoding_base91_decode] Starting Base91 decoding");

    size_t len = strlen(encoded);
    *decoded_length = 0;
    uint8_t* decoded = (uint8_t*) malloc(len); // Max possible size
    if (!decoded) {
        ENCODING_LOG("[encoding_base91_decode] Error: Memory allocation failed");
        return NULL;
    }

    int v = -1;
    int b = 0;
    int n = 0;
    size_t index = 0;

    for (size_t i = 0; i < len; ++i) {
        int c = base91_decode_value(encoded[i]);
        if (c == -1) {
            ENCODING_LOG("[encoding_base91_decode] Error: Invalid character encountered");
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
                if (index >= len) {
                    ENCODING_LOG("[encoding_base91_decode] Error: Decoded index out of bounds");
                    free(decoded);
                    return NULL;
                }
                decoded[index++] = (uint8_t)(b & 255);
                b >>= 8;
                n -= 8;
            }
            v = -1;
        }
    }

    if (v != -1) {
        if (index >= len) {
            ENCODING_LOG("[encoding_base91_decode] Error: Decoded index out of bounds");
            free(decoded);
            return NULL;
        }
        decoded[index++] = (uint8_t)((b | v << n) & 255);
    }

    *decoded_length = index;
    ENCODING_LOG("[encoding_base91_decode] Decoding completed successfully, decoded length: %zu", *decoded_length);
    return decoded;
}

/**
 * @brief Encodes binary data into a Base91 encoded string.
 *
 * @param data Pointer to the binary data to be encoded.
 * @param length The length of the binary data.
 * 
 * @return A dynamically allocated string containing the Base91 encoded representation. 
 * Returns NULL if an error occurs (e.g., invalid input, memory allocation failure).
 *
 * @note The caller is responsible for freeing the returned encoded string.
 */
char* encoding_base91_encode(const uint8_t* data, size_t length) {
    if (!data || length == 0) {
        ENCODING_LOG("[encoding_base91_encode] Error: Invalid input data or length");
        return NULL;
    }
    ENCODING_LOG("[encoding_base91_encode] Starting Base91 encoding");

    size_t estimated_length = length * 1.23 + 2; // +2 for padding and null terminator
    char* encoded = (char*) malloc(estimated_length);
    if (!encoded) {
        ENCODING_LOG("[encoding_base91_encode] Error: Memory allocation failed");
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
                ENCODING_LOG("[encoding_base91_encode] Error: Encoding index out of bounds");
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
    ENCODING_LOG("[encoding_base91_encode] Encoding completed successfully");
    return encoded;
}


#if defined(_WIN32) || defined(_WIN64)
/**
 * @brief Converts a UTF-8 encoded string to a wide character string (wchar_t) on Windows.
 *
 * @param utf8Str Pointer to the UTF-8 encoded string.
 * @return A dynamically allocated wide character string (wchar_t*). 
 * Returns NULL if the conversion fails or if the input string is NULL.
 */
wchar_t* encoding_utf8_to_wchar(const char* utf8Str) {
    if (utf8Str == NULL) {
        ENCODING_LOG("[encoding_utf8_to_wchar] Error: Input string is NULL");
        return NULL;
    }

    ENCODING_LOG("[encoding_utf8_to_wchar] Calculating required size for conversion");
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, NULL, 0);
    if (size_needed == 0) {
        ENCODING_LOG("[encoding_utf8_to_wchar] Error: MultiByteToWideChar failed to calculate size. Error: %lu", GetLastError());
        return NULL;
    }

    ENCODING_LOG("[encoding_utf8_to_wchar] Allocating memory for wchar string");
    wchar_t* wstr = (wchar_t*)malloc(size_needed * sizeof(wchar_t));
    if (!wstr) {
        ENCODING_LOG("[encoding_utf8_to_wchar] Error: Cannot allocate memory for wchar string");
        return NULL;
    }

    ENCODING_LOG("[encoding_utf8_to_wchar] Performing conversion from UTF-8 to wchar");
    int result = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, wstr, size_needed);
    if (result == 0) {
        ENCODING_LOG("[encoding_utf8_to_wchar] Error: Conversion from UTF-8 to wchar failed");
        free(wstr);
        return NULL;
    }

    ENCODING_LOG("[encoding_utf8_to_wchar] Conversion successful");
    return wstr;
}


#endif

/**
 * @brief Converts a wide character string (wchar_t) to a UTF-8 encoded string.
 *
 * @param wstr Pointer to the wide character string (wchar_t).
 * 
 * @return A dynamically allocated UTF-8 encoded string (char*). Returns NULL if the conversion fails or if the input string is NULL.
 */
char* encoding_wchar_to_utf8(const wchar_t* wstr) {
    if (wstr == NULL) {
        ENCODING_LOG("[encoding_wchar_to_utf8] Error: Input wchar string is NULL");
        return NULL;
    }

#if defined(_WIN32) || defined(_WIN64)
    ENCODING_LOG("[encoding_wchar_to_utf8] Converting wchar to UTF-8 on Windows");
    
    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    if (utf8Length == 0) {
        ENCODING_LOG("[encoding_wchar_to_utf8] Error: WideCharToMultiByte failed to calculate length");
        return NULL;
    }

    char* utf8Str = (char*)malloc(utf8Length * sizeof(char));
    if (!utf8Str) {
        ENCODING_LOG("[encoding_wchar_to_utf8] Error: Cannot allocate memory for UTF-8 string");
        return NULL;
    }
    if (WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8Str, utf8Length, NULL, NULL) == 0) {
        ENCODING_LOG("[encoding_wchar_to_utf8] Error: Conversion from wchar to UTF-8 failed");
        free(utf8Str);
        return NULL;
    }

#else
    ENCODING_LOG("[encoding_wchar_to_utf8] Converting wchar to UTF-8 on non-Windows platform");

    setlocale(LC_ALL, "en_US.UTF-8");

    size_t utf8Length = wcstombs(NULL, wstr, 0);
    if (utf8Length == (size_t)-1) {
        ENCODING_LOG("[encoding_wchar_to_utf8] Error: wcstombs failed");
        return NULL;
    }

    char* utf8Str = (char*)malloc((utf8Length + 1) * sizeof(char));
    if (!utf8Str) {
        ENCODING_LOG("[encoding_wchar_to_utf8] Error: Cannot allocate memory for UTF-8 string");
        return NULL;
    }

    if (wcstombs(utf8Str, wstr, utf8Length + 1) == (size_t)-1) {
        ENCODING_LOG("[encoding_wchar_to_utf8] Error: Conversion from wchar to UTF-8 failed");
        free(utf8Str);
        return NULL;
    }
#endif

    ENCODING_LOG("[encoding_wchar_to_utf8] Conversion successful");
    return utf8Str;
}

/**
 * @brief Initializes the encoding library by setting the locale for character encoding.
 * 
 * This function should be called at the beginning of the program to ensure proper handling of character encodings.
 */
void encoding_initialize(void) {
    ENCODING_LOG("[encoding_initialize] Initializing encoding library");
    setlocale(LC_ALL, "");
    ENCODING_LOG("[encoding_initialize] Locale set for character encoding");
}