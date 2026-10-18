/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class String
*/
/* mbstowcs / wcstombs are standard C; MSVC's "use the _s version" warning
   (C4996) is off for this file so it builds cleanly without project flags. */
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include <wchar.h>
#include "std_string.h"

/* SSE2 is part of every x86-64 CPU (and of x86 builds that ask for it); the
   substring search uses it there and plain memchr elsewhere (or when built
   with -DSTRING_NO_SIMD). */
#if !defined(STRING_NO_SIMD) && \
    (defined(__SSE2__) || defined(_M_X64) || defined(_M_AMD64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2))
#  define SSTR_HAVE_SSE2 1
#  include <emmintrin.h>
#  if defined(_MSC_VER) && !defined(__clang__)
#    include <intrin.h>
#  endif
#endif

const char* STRING_ASCII_LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* STRING_ASCII_LOWERCASE = "abcdefghijklmnopqrstuvwxyz";
const char* STRING_ASCII_UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* STRING_DIGITS = "0123456789";
const char* STRING_HEXDIGITS = "0123456789abcdefABCDEF";
const char* STRING_WHITESPACE = " \t\n\r\f\v";
const char* STRING_PUNCTUATION = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";



static bool sstr_is_ascii_upper(char c) {
    return c >= 'A' && c <= 'Z';
}

static bool sstr_is_ascii_lower(char c) {
    return c >= 'a' && c <= 'z';
}

static char sstr_ascii_toupper(char c) {
    return sstr_is_ascii_lower(c) ? (char)(c - 'a' + 'A') : c;
}

static char sstr_ascii_tolower(char c) {
    return sstr_is_ascii_upper(c) ? (char)(c - 'A' + 'a') : c;
}

static int sstr_hex_digit(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

/* A "word" character for title casing: an ASCII letter, or any byte of a
   UTF-8 multi-byte sequence (so "été" is one word, not three). */
static bool sstr_is_word_char(char c) {
    return sstr_is_ascii_upper(c) || sstr_is_ascii_lower(c) || (unsigned char)c >= 0x80;
}

static const char *base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";


/* ------------------------------------------------------------------ */
/* Storage                                                            */
/* ------------------------------------------------------------------ */

static bool sstr_is_inline(const String* str) {
    return str->dataStr == str->sso;
}

/* Capacity at least `cap` bytes (terminator included), keeping the content. */
static bool sstr_reserve_cap(String* str, size_t cap) {
    if (cap <= str->capacitySize) {
        return true;
    }
    char* buf;
    if (sstr_is_inline(str)) {
        buf = (char*)malloc(cap);
        if (buf == NULL) {
            return false;
        }
        memcpy(buf, str->sso, str->size + 1);
    }
    else {
        buf = (char*)realloc(str->dataStr, cap);
        if (buf == NULL) {
            return false;
        }
    }
    str->dataStr = buf;
    str->capacitySize = cap;
    return true;
}

/* Room for `extra` more bytes: the capacity doubles (at least the exact need)
 * so appends are amortised O(1). */
bool stringbuf_grow_for(String* str, size_t extra) {
    if (str == NULL) {
        return false;
    }
    if (extra > SIZE_MAX - 1 - str->size) {
        STRING_LOG("[stringbuf_grow_for]: Error - size overflow.");
        return false;
    }
    size_t need = str->size + extra + 1;
    if (need <= str->capacitySize) {
        return true;
    }
    size_t cap = (str->capacitySize <= SIZE_MAX / 2) ? str->capacitySize * 2 : need;
    if (cap < need) {
        cap = need;
    }
    return sstr_reserve_cap(str, cap) || sstr_reserve_cap(str, need);
}

/* Offset of `p` inside str's buffer, or SIZE_MAX when it points elsewhere.
   Arguments that alias the String being modified (string_append(s, s->dataStr))
   are re-derived from this offset after the buffer moves. */
static size_t sstr_alias_offset(const String* str, const char* p) {
    uintptr_t base = (uintptr_t)str->dataStr;
    uintptr_t at = (uintptr_t)p;
    if (str->dataStr != NULL && at >= base && at < base + str->capacitySize) {
        return (size_t)(at - base);
    }
    return SIZE_MAX;
}

static bool sstr_aliases(const String* str, const char* p) {
    return sstr_alias_offset(str, p) != SIZE_MAX;
}

/* An empty String in `str` (whose memory the caller provides). */
static void sstr_init_empty(String* str) {
    str->dataStr = str->sso;
    str->size = 0;
    str->capacitySize = STRING_SSO_CAPACITY;
    str->pool = NULL;
    str->sso[0] = '\0';
}

/* Sets str's content to `n` bytes from `data` (binary-safe; data may point
   into str). false on allocation failure, with str unchanged. */
static bool sstr_assign_bytes(String* str, const char* data, size_t n) {
    if (n + 1 > str->capacitySize) {
        size_t off = sstr_alias_offset(str, data);
        if (!sstr_reserve_cap(str, n + 1)) {
            return false;
        }
        if (off != SIZE_MAX) {
            data = str->dataStr + off;
        }
    }
    if (n > 0) {
        memmove(str->dataStr, data, n);
    }
    str->dataStr[n] = '\0';
    str->size = n;
    return true;
}

/* Appends `n` bytes (binary-safe; data may point into str). */
static bool sstr_append_bytes(String* str, const char* data, size_t n) {
    if (n == 0) {
        return true;
    }
    if (n >= str->capacitySize - str->size) {
        size_t off = sstr_alias_offset(str, data);
        if (!stringbuf_grow_for(str, n)) {
            return false;
        }
        if (off != SIZE_MAX) {
            data = str->dataStr + off;
        }
    }
    memmove(str->dataStr + str->size, data, n);
    str->size += n;
    str->dataStr[str->size] = '\0';
    return true;
}

/* New heap String holding `n` bytes from `data` (NULL data: n zero bytes...
   only used with n == 0). One allocation for up to 31 bytes. */
static String* sstr_new(const char* data, size_t n) {
    String* str = (String*)malloc(sizeof(String));
    if (str == NULL) {
        return NULL;
    }
    sstr_init_empty(str);
    if (n >= STRING_SSO_CAPACITY) {
        char* buf = (n < SIZE_MAX) ? (char*)malloc(n + 1) : NULL;
        if (buf == NULL) {
            free(str);
            return NULL;
        }
        str->dataStr = buf;
        str->capacitySize = n + 1;
    }
    if (n > 0 && data != NULL) {
        memcpy(str->dataStr, data, n);
    }
    str->dataStr[n] = '\0';
    str->size = n;
    return str;
}

/* First occurrence of needle[0, nlen) in hay[0, hlen), or NULL. Binary-safe:
   memchr finds the candidates for the first byte, memcmp checks the rest
   (the same scheme as std::string::find, without strstr's stop at NUL). */
static const char* sstr_search(const char* hay, size_t hlen, const char* needle, size_t nlen) {
    if (nlen == 0) {
        return hay;
    }
    if (nlen > hlen) {
        return NULL;
    }
    if (nlen == 1) {
        return (const char*)memchr(hay, (unsigned char)needle[0], hlen);
    }
#if defined(SSTR_HAVE_SSE2)
    {
        /* 16 candidate positions at a time: a position is checked with memcmp
           only when both the needle's first byte and its last byte match
           there (W. Mula's "generic SIMD" substring search). Every load stays
           inside hay[0, hlen). */
        const __m128i first16 = _mm_set1_epi8(needle[0]);
        const __m128i last16  = _mm_set1_epi8(needle[nlen - 1]);
        size_t starts = hlen - nlen + 1;                /* candidate start positions */
        size_t i = 0;
        for (; starts - i >= 16; i += 16) {
            __m128i a = _mm_loadu_si128((const __m128i*)(const void*)(hay + i));
            __m128i b = _mm_loadu_si128((const __m128i*)(const void*)(hay + i + nlen - 1));
            unsigned mask = (unsigned)_mm_movemask_epi8(_mm_and_si128(_mm_cmpeq_epi8(a, first16),
                                                                      _mm_cmpeq_epi8(b, last16)));
            while (mask != 0) {
#if defined(_MSC_VER) && !defined(__clang__)
                unsigned long bit;
                _BitScanForward(&bit, mask);
#else
                unsigned bit = (unsigned)__builtin_ctz(mask);
#endif
                if (memcmp(hay + i + bit + 1, needle + 1, nlen - 2) == 0) {
                    return hay + i + bit;
                }
                mask &= mask - 1;
            }
        }
        for (; i < starts; ++i) {
            if (hay[i] == needle[0] && hay[i + nlen - 1] == needle[nlen - 1] &&
                memcmp(hay + i + 1, needle + 1, nlen - 2) == 0) {
                return hay + i;
            }
        }
        return NULL;
    }
#else
    const char* p = hay;
    const char* last = hay + (hlen - nlen);           /* last possible start */
    unsigned char first = (unsigned char)needle[0];
    while (p <= last) {
        p = (const char*)memchr(p, first, (size_t)(last - p) + 1);
        if (p == NULL) {
            return NULL;
        }
        if (memcmp(p + 1, needle + 1, nlen - 1) == 0) {
            return p;
        }
        ++p;
    }
    return NULL;
#endif
}

/* A 256-entry membership table for the bytes of the C string `set` (the
   terminator is not a member). */
static void sstr_make_set(const char* set, bool table[256]) {
    memset(table, 0, 256 * sizeof(bool));
    for (const unsigned char* s = (const unsigned char*)set; *s; ++s) {
        table[*s] = true;
    }
}

static bool sstr_is_ascii_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

/* Writes the decimal digits of `value` ending at `end` (exclusive); returns
   the first digit. */
static char* sstr_format_uint(char* end, unsigned long long value) {
    static const char pairs[] =
        "00010203040506070809101112131415161718192021222324252627282930313233343536373839"
        "40414243444546474849505152535455565758596061626364656667686970717273747576777879"
        "8081828384858687888990919293949596979899";
    char* p = end;
    while (value >= 100) {
        unsigned idx = (unsigned)(value % 100) * 2;
        value /= 100;
        *--p = pairs[idx + 1];
        *--p = pairs[idx];
    }
    if (value >= 10) {
        unsigned idx = (unsigned)value * 2;
        *--p = pairs[idx + 1];
        *--p = pairs[idx];
    }
    else {
        *--p = (char)('0' + (int)value);
    }
    return p;
}

/* Decimal text of an int into buf (at least 12 bytes); returns its start. */
static char* sstr_format_int(char buf[12], int value) {
    char* end = buf + 11;
    *end = '\0';
    unsigned long long mag = (value < 0) ? (unsigned long long)(-(long long)value) : (unsigned long long)value;
    char* p = sstr_format_uint(end, mag);
    if (value < 0) {
        *--p = '-';
    }
    return p;
}


/**
 * @brief Creates a new String object initialized with the given initial string.
 *
 * This function allocates memory for a new String object and initializes it with the provided initial string. 
 * If the initial string is not provided (i.e., NULL), the String object will be created with an empty string.
 * The function also initializes a memory pool for efficient memory management of the string data.
 *
 * @param initialStr The initial string to initialize the String object with. Can be NULL for an empty string.
 * @return A pointer to the newly created String object, or exits the program with an error if memory allocation fails.
 */
String* string_create(const char* initialStr) {
    STRING_LOG("[string_create]: Creating string with initial content: %s", initialStr ? initialStr : "(null)");
    return sstr_new(initialStr, initialStr ? strlen(initialStr) : 0);
}


/**
 * @brief Creates a String holding `length` bytes from `data` - binary-safe, so
 *        the bytes may include NULs (std::string(data, length)).
 *
 * @param data   The bytes (may be NULL only when length is 0).
 * @param length Number of bytes.
 * @return The new String, or NULL for invalid arguments / out of memory.
 */
String* string_create_n(const char* data, size_t length) {
    if (data == NULL && length != 0) {
        return NULL;
    }
    return sstr_new(data, length);
}


/**
 * @brief Creates a copy of `str` (binary-safe; std::string's copy constructor).
 *
 * @return The copy, or NULL for a NULL String / out of memory.
 */
String* string_clone(const String* str) {
    if (str == NULL) {
        return NULL;
    }
    return sstr_new(str->dataStr, str->size);
}


/**
 * @brief Initialises a String whose memory the caller provides - on the stack
 *        or inside another struct - so the String itself needs no allocation,
 *        and a short content (up to STRING_SSO_CAPACITY - 1 bytes) none at all.
 *
 * Use every String function on it as usual, and release it with
 * string_destroy (NOT string_deallocate, which would free the struct).
 *
 * @param str        The String to initialise.
 * @param initialStr Initial content (NULL for empty).
 * @return true on success; false for NULL str or out of memory (str is then
 *         a valid empty String).
 */
bool string_init(String* str, const char* initialStr) {
    if (str == NULL) {
        return false;
    }
    sstr_init_empty(str);
    if (initialStr != NULL) {
        return sstr_assign_bytes(str, initialStr, strlen(initialStr));
    }
    return true;
}


/**
 * @brief Releases the buffer of a String set up with string_init; the String
 *        is left empty and may be used again.
 */
void string_destroy(String* str) {
    if (str == NULL) {
        return;
    }
    if (str->dataStr != NULL && !sstr_is_inline(str)) {
        free(str->dataStr);
    }
    sstr_init_empty(str);
}


/**
 * @brief Creates a new String object with a memory pool of the specified size.
 *
 * The String gets its own pool with room for @p size bytes, so it can grow to
 * that length without reallocating. It used to share one global bump pool
 * instead: memory taken from it was never returned (string_deallocate freed
 * only the struct, so programs creating and freeing such strings grew without
 * bound), every call after the first ignored its size, and the new String had
 * no buffer (dataStr == NULL), which crashed string_contains, string_compare,
 * string_replace, ...
 *
 * @param size Number of bytes to reserve.
 * @return A pointer to the newly created (empty) String object, or NULL if memory allocation fails.
 */
String* string_create_with_pool(size_t size) {
    STRING_LOG("[string_create_with_pool]: Creating an empty String with %zu bytes reserved.\n", size);
    String* str = string_create(NULL);
    if (str && size > 0) {
        string_reserve(str, size);
    }
    return str;
}


/**
 * @brief Extracts a substring from the given String object.
 *
 * This function creates a new String object containing a substring of the original string starting at the specified 
 * position and having the specified length. If the length exceeds the bounds of the original string, 
 * the substring will be shortened accordingly.
 *
 * @param str The original String object from which the substring is extracted. Must not be NULL.
 * @param pos The starting position of the substring within the original string. Must be within the bounds of the string.
 * @param len The length of the substring. If it exceeds the end of the original string, it will be adjusted.
 * 
 * @return A new String object containing the substring, or NULL if an error occurs.
 */
String* string_substr(String* str, size_t pos, size_t len) {
    STRING_LOG("[string_substr]: Entering function with pos=%zu, len=%zu.\n", pos, len);

    if (str == NULL) {
        STRING_LOG("[string_substr]: Error: The String object is NULL.\n");
        return NULL;
    }
    if (pos >= str->size) {
        STRING_LOG("[string_substr]: Error: Position out of bounds (pos=%zu, size=%zu).\n", pos, str->size);
        return NULL;
    }
    /* len > size - pos (pos < size here): a huge len such as SIZE_MAX (the npos
       idiom) cannot wrap pos + len around and skip the clamp */
    if (len > str->size - pos) {
        len = str->size - pos;
    }
    return sstr_new(str->dataStr + pos, len);
}


/**
 * @brief Checks if the given substring is contained within the String object.
 *
 * This function checks whether the specified substring is present in the String object.
 *
 * @param str The String object to search within. Must not be NULL.
 * @param substr The substring to search for. Must not be NULL.
 * 
 * @return true if the substring is found within the String object, false otherwise.
 */
bool string_contains(const String* str, const char* substr) {
    STRING_LOG("[string_contains]: Checking if string contains substring.\n");

    if (str == NULL || substr == NULL) {
        STRING_LOG("[string_contains]: Error: NULL argument.\n");
        return false;
    }
    return sstr_search(str->dataStr, str->size, substr, strlen(substr)) != NULL;
}


/**
 * @brief Compares if the first String object is less than the second.
 *
 * This function compares two String objects to determine if the first one is lexicographically less than the second.
 *
 * @param str1 The first String object. Must not be NULL.
 * @param str2 The second String object. Must not be NULL.
 * 
 * @return true if str1 is less than str2, false otherwise.
 */
bool string_is_less(const String* str1, const String* str2) {
    STRING_LOG("[string_is_less]: Checking if the first string is less than the second.\n");
    return string_compare(str1, str2) < 0;
}


/**
 * @brief Compares if the first String object is greater than the second.
 *
 * This function compares two String objects to determine if the first one is lexicographically greater than the second.
 *
 * @param str1 The first String object. Must not be NULL.
 * @param str2 The second String object. Must not be NULL.
 * 
 * @return true if str1 is greater than str2, false otherwise.
 */
bool string_is_greater(const String* str1, const String* str2) {
    STRING_LOG("[string_is_greater]: Checking if the first string is greater than the second.\n");
    return string_compare(str1, str2) > 0;
}


/**
 * @brief Compares if the first String object is less than or equal to the second.
 *
 * This function compares two String objects to determine if the first one is lexicographically less than or equal to the second.
 *
 * @param str1 The first String object. Must not be NULL.
 * @param str2 The second String object. Must not be NULL.
 * 
 * @return true if str1 is less than or equal to str2, false otherwise.
 */
bool string_is_less_or_equal(const String* str1, const String* str2) {
    STRING_LOG("[string_is_less_or_equal]: Checking if the first string is less than or equal to the second.\n");
    return string_compare(str1, str2) <= 0;
}


/**
 * @brief Compares if the first String object is greater than or equal to the second.
 *
 * This function compares two String objects to determine if the first one is lexicographically greater than or equal to the second.
 *
 * @param str1 The first String object. Must not be NULL.
 * @param str2 The second String object. Must not be NULL.
 * 
 * @return true if str1 is greater than or equal to str2, false otherwise.
 */
bool string_is_greater_or_equal(const String* str1, const String* str2) {
    STRING_LOG("[string_is_greater_or_equal]: Checking if the first string is greater than or equal to the second.\n");
    return string_compare(str1, str2) >= 0;
}


/**
 * @brief Checks if the String object contains only alphabetic characters.
 *
 * This function checks if all characters in the String object are alphabetic (A-Z, a-z). 
 * It returns true if all characters are alphabetic, otherwise false.
 *
 * @param str The String object to check. Must not be NULL.
 * 
 * @return true if all characters are alphabetic, false otherwise.
 */
bool string_is_alpha(const String* str) {
    STRING_LOG("[string_is_alpha]: Checking if the string contains only alphabetic characters.\n");
    if (str != NULL) {
        for (size_t index = 0; index < str->size; index++) {
            if (!(str->dataStr[index] >= 'a' && str->dataStr[index] <= 'z') &&
                !(str->dataStr[index] >= 'A' && str->dataStr[index] <= 'Z')) {
                return false;
            }
        }
        return true;
    }
    return false;
}


/**
 * @brief Checks if the String object contains only digit characters.
 *
 * This function checks if all characters in the String object are numeric digits (0-9). 
 * It returns true if all characters are digits, otherwise false.
 *
 * @param str The String object to check. Must not be NULL.
 * 
 * @return true if all characters are digits, false otherwise.
 */
bool string_is_digit(const String* str) {
    STRING_LOG("[string_is_digit]: Checking if the string contains only digit characters.\n");
    if (str == NULL) {
        STRING_LOG("[string_is_digit]: Error - The String object is NULL.\n");
        return false;
    }
    for (size_t index = 0; index < str->size; index++) {
        if (!(str->dataStr[index] >= '0' && str->dataStr[index] <= '9')) {
            return false;
        }
    }
    return true;
}


/**
 * @brief Checks if all characters in the String object are uppercase.
 *
 * This function iterates through the characters of the String object and checks if all are uppercase.
 * If any lowercase letter is found, it returns false. It returns true if all characters are uppercase.
 *
 * @param str The String object to check. Must not be NULL.
 * @return true if all characters are uppercase, false otherwise.
 */
bool string_is_upper(const String* str) {
    STRING_LOG("[string_is_upper]: Checking if all characters are uppercase.\n");
    if (str == NULL) {
        STRING_LOG("[string_is_upper]: Error - The String object is NULL.\n");
        return false;
    }
    for (size_t index = 0; index < str->size; index++) {
        if (str->dataStr[index] >= 'a' && str->dataStr[index] <= 'z') {
            return false;
        }
    }
    return true;
}


/**
 * @brief Checks if all characters in the String object are lowercase.
 *
 * This function iterates through the characters of the String object and checks if all are lowercase.
 * If any uppercase letter is found, it returns false. It returns true if all characters are lowercase.
 *
 * @param str The String object to check. Must not be NULL.
 * @return true if all characters are lowercase, false otherwise.
 */
bool string_is_lower(const String* str) {
    STRING_LOG("[string_is_lower]: Checking if all characters are lowercase.\n");
    if (str == NULL) {
        STRING_LOG("[string_is_lower]: Error - The String object is NULL.\n");
        return false;
    }
    for (size_t index = 0; index < str->size; index++) {
        if (str->dataStr[index] >= 'A' && str->dataStr[index] <= 'Z') {
            return false;
        }
    }

    return true;
}

/**
 * @brief Reverses the characters in the String object.
 *
 * This function reverses the order of characters in the String object. 
 * It allocates memory for the reversed string, replaces the original string with the reversed one, and frees the temporary memory.
 *
 * @param str The String object to reverse. Must not be NULL.
 */
void string_reverse(String* str) {
    STRING_LOG("[string_reverse]: Reversing the String object.\n");
    if (str != NULL && str->dataStr != NULL) {
        /* In place and by size: the old copy went through string_replace,
           which measured with strlen and emptied strings holding 0x00 bytes. */
        for (size_t i = 0, j = str->size; i + 1 < j; ++i) {
            --j;
            char tmp = str->dataStr[i];
            str->dataStr[i] = str->dataStr[j];
            str->dataStr[j] = tmp;
        }
    } 
    else {
        STRING_LOG("[string_reverse]: Error - The String object or its data is NULL.\n");
        return;
    }
}


/**
 * @brief Resizes the String object to the specified size.
 *
 * This function resizes the String object to the new size specified by `newSize`. 
 * If the new size is smaller, the string is truncated. If the new size is larger, the string is expanded with null characters.
 *
 * @param str The String object to resize. Must not be NULL.
 * @param newSize The new size of the String object.
 */
void string_resize(String *str, size_t newSize) {
    STRING_LOG("[string_resize]: Resizing the String object to new size: %zu.\n", newSize);

    if (str == NULL) {
        STRING_LOG("[string_resize]: Error - The String object is NULL.\n");
        return;
    }
    if (newSize < str->size) {
        str->size = newSize;
        str->dataStr[newSize] = '\0';
    }
    else if (newSize > str->size) {
        if (newSize == SIZE_MAX || !sstr_reserve_cap(str, newSize + 1)) {
            STRING_LOG("[string_resize]: Error - Memory allocation failed.\n");
            return;
        }
        memset(str->dataStr + str->size, '\0', newSize - str->size);
        str->size = newSize;
        str->dataStr[newSize] = '\0';
    }
}


/**
 * @brief Reduces the capacity of the String object to fit its current size.
 *
 * This function shrinks the capacity of the String object to match its current size plus the null terminator.
 * If the current capacity is already optimal, the function does nothing.
 *
 * @param str The String object to shrink. Must not be NULL.
 */
void string_shrink_to_fit(String *str) {
    STRING_LOG("[string_shrink_to_fit]: Shrinking the String object to fit its size.\n");

    if (str == NULL || sstr_is_inline(str)) {
        return;                              /* the inline buffer has a fixed size */
    }
    if (str->size < STRING_SSO_CAPACITY) {
        /* fits inline again: move it back and free the heap buffer */
        char* heap = str->dataStr;
        memcpy(str->sso, heap, str->size + 1);
        str->dataStr = str->sso;
        str->capacitySize = STRING_SSO_CAPACITY;
        free(heap);
        return;
    }
    if (str->size + 1 < str->capacitySize) {
        char* buf = (char*)realloc(str->dataStr, str->size + 1);
        if (buf != NULL) {                   /* a failed shrink keeps the old buffer */
            str->dataStr = buf;
            str->capacitySize = str->size + 1;
        }
    }
}


/**
 * @brief Appends `length` bytes from `data` - binary-safe, the bytes may include
 *        NULs (std::string::append(data, length)). `data` may point into str.
 */
void string_append_n(String *str, const char *data, size_t length) {
    if (str == NULL || (data == NULL && length != 0)) {
        STRING_LOG("[string_append_n]: Error - NULL argument.\n");
        return;
    }
    if (!sstr_append_bytes(str, data, length)) {
        STRING_LOG("[string_append_n]: Error - Memory allocation failed.\n");
    }
}


/**
 * @brief Assigns a new string to the String object, replacing its current contents.
 *
 * This function replaces the current contents of the String object with the contents of `newStr`.
 * If the new string is larger than the current capacity, the function reallocates memory to accommodate it.
 *
 * @param str The String object to which the new string will be assigned. Must not be NULL.
 * @param newStr The new string to assign. Must not be NULL.
 */
void string_assign(String *str, const char *newStr) {
    STRING_LOG("[string_assign]: Assigning new string '%s' to the String object.\n", newStr);

    if (str == NULL || newStr == NULL) {
        STRING_LOG("[string_assign]: Error - NULL argument.\n");
        return;
    }
    if (!sstr_assign_bytes(str, newStr, strlen(newStr))) {
        STRING_LOG("[string_assign]: Error - Memory allocation failed.\n");
    }
}


/**
 * @brief Inserts a substring into the String object at the specified position.
 *
 * This function inserts the contents of `strItem` into the String object at the specified position `pos`.
 * If the new size exceeds the current capacity, the function reallocates memory to accommodate the insertion.
 *
 * @param str The String object into which the substring will be inserted. Must not be NULL.
 * @param pos The position at which to insert the substring. Must be within the bounds of the string's size.
 * @param strItem The substring to insert. Must not be NULL.
 */
void string_insert(String *str, size_t pos, const char *strItem) {
    STRING_LOG("[string_insert]: Inserting string '%s' at position %zu.\n", strItem, pos);

    if (str == NULL || strItem == NULL || pos > str->size) {
        STRING_LOG("[string_insert]: Error - invalid arguments.\n");
        return;
    }
    size_t n = strlen(strItem);
    if (n == 0) {
        return;
    }
    /* strItem may point into this String: work from its offset, which stays
       valid when the buffer moves */
    size_t off = sstr_alias_offset(str, strItem);
    if (n >= str->capacitySize - str->size && !stringbuf_grow_for(str, n)) {
        STRING_LOG("[string_insert]: Error - Memory allocation failed.\n");
        return;
    }
    char* base = str->dataStr;
    memmove(base + pos + n, base + pos, str->size - pos + 1);      /* tail + terminator */
    if (off == SIZE_MAX) {
        memcpy(base + pos, strItem, n);
    }
    else if (off + n <= pos) {
        memcpy(base + pos, base + off, n);                         /* source before the gap: unmoved */
    }
    else if (off >= pos) {
        memcpy(base + pos, base + off + n, n);                     /* source moved right by n */
    }
    else {
        /* source straddles pos: its head stayed, its tail moved by n */
        size_t head = pos - off;
        memmove(base + pos, base + off, head);
        memmove(base + pos + head, base + pos + n, n - head);
    }
    str->size += n;
}


/**
 * @brief Erases a portion of the String object starting from the specified position.
 *
 * This function removes `len` characters from the String object starting at position `pos`.
 * If `len` exceeds the number of characters from `pos` to the end of the string, the function adjusts `len` accordingly.
 *
 * @param str The String object from which characters will be erased. Must not be NULL.
 * @param pos The starting position for the erase operation. Must be within the bounds of the string's size.
 * @param len The number of characters to erase. If `pos + len` exceeds the string size, `len` is adjusted.
 */
void string_erase(String *str, size_t pos, size_t len) {
    STRING_LOG("[string_erase]: Erasing %zu characters starting from position %zu.\n", len, pos);

    if (str == NULL) {
        STRING_LOG("[string_erase]: Error - The String object is NULL.\n");
        return;
    }
    if (pos >= str->size) {
        STRING_LOG("[string_erase]: Error - Position out of bounds.\n");
        return;
    }
    if (len > str->size - pos) {  /* overflow-safe form of pos + len > size */
        len = str->size - pos;  // Adjust len to not go beyond the string end
        STRING_LOG("[string_erase]: Adjusted length to %zu.\n", len);
    }

    memmove(str->dataStr + pos, str->dataStr + pos + len, str->size - pos - len + 1);
    str->size -= len;
    STRING_LOG("[string_erase]: String erased successfully, new size: %zu.\n", str->size);
}


/**
 * @brief Replaces the first occurrence of a substring within the String object with another substring.
 *
 * This function searches for the first occurrence of `oldStr` in `str1` and replaces it with `newStr`.
 * If `oldStr` is not found, the function does nothing and logs a warning.
 *
 * @param str1 The String object in which the replacement is to occur. Must not be NULL.
 * @param oldStr The substring to be replaced. Must not be NULL.
 * @param newStr The substring to replace `oldStr` with. Must not be NULL.
 */
void string_replace(String *str1, const char *oldStr, const char *newStr) {
    STRING_LOG("[string_replace]: Replacing '%s' with '%s'.\n", oldStr, newStr);

    if (str1 == NULL || oldStr == NULL || newStr == NULL) {
        STRING_LOG("[string_replace]: Error - NULL argument.\n");
        return;
    }
    size_t oldLen = strlen(oldStr);
    size_t newLen = strlen(newStr);
    const char* hit = sstr_search(str1->dataStr, str1->size, oldStr, oldLen);
    if (hit == NULL) {
        STRING_LOG("[string_replace]: Warning - oldStr not found in str1.\n");
        return;
    }
    size_t at = (size_t)(hit - str1->dataStr);

    /* newStr may point into this String; copy it aside (the tail move below
       could overwrite it) */
    char small[64];
    char* copy = NULL;
    if (sstr_aliases(str1, newStr)) {
        copy = (newLen < sizeof small) ? small : (char*)malloc(newLen + 1);
        if (copy == NULL) {
            STRING_LOG("[string_replace]: Error - Memory allocation failed.\n");
            return;
        }
        memcpy(copy, newStr, newLen);
        newStr = copy;
    }
    if (newLen > oldLen && !stringbuf_grow_for(str1, newLen - oldLen)) {
        STRING_LOG("[string_replace]: Error - Memory allocation failed.\n");
        if (copy != small) {
            free(copy);
        }
        return;
    }
    char* base = str1->dataStr;
    memmove(base + at + newLen, base + at + oldLen, str1->size - at - oldLen + 1);   /* tail + terminator */
    memcpy(base + at, newStr, newLen);
    str1->size = str1->size - oldLen + newLen;
    if (copy != NULL && copy != small) {
        free(copy);
    }
}


/**
 * @brief Swaps the contents of two String objects.
 *
 * This function exchanges the contents of `str1` and `str2`.
 *
 * @param str1 The first String object. Must not be NULL.
 * @param str2 The second String object. Must not be NULL.
 */
void string_swap(String *str1, String *str2) {
    STRING_LOG("[string_swap]: Swapping two strings.\n");

    if (str1 == NULL || str2 == NULL || str1 == str2) {
        return;
    }
    String temp = *str1;
    *str1 = *str2;
    *str2 = temp;
    /* inline contents moved with the struct: point them at their new home */
    if (str1->dataStr == str2->sso) {
        str1->dataStr = str1->sso;
    }
    if (str2->dataStr == str1->sso) {
        str2->dataStr = str2->sso;
    }
}


/**
 * @brief Deallocates a String object and its associated resources.
 *
 * This function deallocates the memory used by the String object and its associated memory pool.
 * If the String object is NULL, the function logs a warning and does nothing.
 *
 * @param str The String object to be deallocated. Must not be NULL.
 */
void string_deallocate(String *str) {
    STRING_LOG("[string_deallocate]: Deallocating the String object.\n");

    if (str == NULL) {
        return;
    }
    /* Call this exactly once per String created by string_create & co (NULL
       is a no-op); Strings set up with string_init are released with
       string_destroy instead. */
    if (str->dataStr != NULL && !sstr_is_inline(str)) {
        free(str->dataStr);
    }
    free(str);
}


/**
 * @brief Returns the maximum possible size of the String object.
 *
 * This function returns the maximum possible size of the String object. The value returned is typically a large value
 * representing the maximum allowable size for a string on the system. If the String object is NULL, the function returns 0.
 * 
 * @param str The String object. Must not be NULL.
 * @return The maximum possible size of the string, or 0 if the String object is NULL.
 */
size_t string_max_size(const String* str) {
    STRING_LOG("[string_max_size]: Retrieving the maximum possible size of the String.\n");

    if (str == NULL) {
        STRING_LOG("[string_max_size]: Error - The String object is NULL.\n");
        return 0;  
    }
    return (size_t)-1; 
}


/**
 * @brief Copies a substring from the String object into the provided buffer.
 *
 * This function copies up to `len` characters from the String object, starting at position `pos`, into the provided buffer.
 * The function ensures that the copied substring is null-terminated. If `len` is 0 or the specified range exceeds the
 * string's size, it adjusts the length to copy only the valid portion of the string.
 * 
 * @param str The String object to copy from. Must not be NULL.
 * @param buffer The buffer to copy the substring into. Must not be NULL.
 * @param pos The starting position in the string from which to begin copying. Must be within the bounds of the string.
 * @param len The number of characters to copy. If 0 or exceeding the string's length, the function adjusts this value.
 * 
 * @return The number of characters actually copied.
 */
size_t string_copy(const String *str, char *buffer, size_t pos, size_t len) {
    STRING_LOG("[string_copy]: Copying a substring starting at position %zu, length %zu.\n", pos, len);

    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_copy]: Error - The String object or its data is NULL.\n");
        return 0;
    }
    if (buffer == NULL) {
        STRING_LOG("[string_copy]: Error - The buffer is NULL.\n");
        return 0;
    }
    if (pos >= str->size) {
        STRING_LOG("[string_copy]: Error - Position out of bounds.\n");
        return 0;
    }

    size_t copyLen = len;
    if (len > str->size - pos || len == 0) {  /* overflow-safe form of pos + len > size */
        copyLen = str->size - pos;  // Adjust copy length if it goes beyond the string end
    }

    memcpy(buffer, str->dataStr + pos, copyLen);
    buffer[copyLen] = '\0';  

    STRING_LOG("[string_copy]: Copied %zu characters.\n", copyLen);
    return copyLen;  
}


/**
 * @brief Finds the first occurrence of a substring in the String object starting from the given position.
 *
 * This function searches for the first occurrence of the specified buffer within the String object, starting from
 * the position `pos`. If the substring is found, the function returns the index of its first character.
 * If the substring is not found or an error occurs, the function returns -1.
 * 
 * @param str The String object to search in. Must not be NULL.
 * @param buffer The substring to search for. Must not be NULL.
 * @param pos The starting position for the search. Must be within the bounds of the string.
 * @return The index of the first occurrence of the substring, or -1 if not found or an error occurs.
 */
int string_find(const String *str, const char *buffer, size_t pos) {
    STRING_LOG("[string_find]: Searching for substring starting from position %zu.\n", pos);

    if (str == NULL || str->dataStr == NULL || buffer == NULL) {
        STRING_LOG("[string_find]: Error - NULL argument.\n");
        return -1;
    }
    if (pos >= str->size) {
        STRING_LOG("[string_find]: Error - Position %zu out of bounds. String size is %zu.\n", pos, str->size);
        return -1;
    }
    const char* found = sstr_search(str->dataStr + pos, str->size - pos, buffer, strlen(buffer));
    if (found == NULL || (size_t)(found - str->dataStr) > (size_t)INT_MAX) {
        STRING_LOG("[string_find]: Substring not found.\n");
        return -1;
    }
    return (int)(found - str->dataStr);
}


/**
 * @brief Finds the last occurrence of a substring in the String object up to the given position.
 *
 * This function searches for the last occurrence of the specified buffer within the String object up to
 * the position `pos`. If the substring is found, the function returns the index of its first character.
 * If the substring is not found or an error occurs, the function returns -1.
 * 
 * @param str The String object to search in. Must not be NULL.
 * @param buffer The substring to search for. Must not be NULL.
 * @param pos The ending position for the search. Must be within the bounds of the string.
 * 
 * @return The index of the last occurrence of the substring, or -1 if not found or an error occurs.
 */
int string_rfind(const String *str, const char *buffer, size_t pos) {
    STRING_LOG("[string_rfind]: Searching for last occurrence of substring up to position %zu.\n", pos);

    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_rfind]: Error - The String object or its data is NULL.\n");
        return -1;
    }
    if (buffer == NULL) {
        STRING_LOG("[string_rfind]: Error - The buffer is NULL.\n");
        return -1;
    }

    size_t bufferLen = strlen(buffer);
    if (bufferLen == 0) {
        STRING_LOG("[string_rfind]: Error - The buffer is empty.\n");
        return -1;
    }
    if (bufferLen > str->size) {
        STRING_LOG("[string_rfind]: Substring longer than the string.\n");
        return -1;
    }

    /* As std::string::rfind: pos is the LAST allowed start index of a match,
       so rfind("abc", "ab", 0) is 0. (It used to reject pos < len - 1.) */
    size_t start = str->size - bufferLen;
    if (pos < start) {
        start = pos;
    }
    STRING_LOG("[string_rfind]: Adjusted search position is %zu.\n", start);

    for (size_t i = start + 1; i-- > 0; ) {
        if (memcmp(str->dataStr + i, buffer, bufferLen) == 0) {
            STRING_LOG("[string_rfind]: Substring found at position %zu.\n", i);
            return (int)i;
        }
    }
    STRING_LOG("[string_rfind]: Substring not found.\n");
    return -1;  
}


/**
 * @brief Finds the first occurrence of any character from the buffer in the string starting from the given position.
 *
 * This function searches the string starting from the specified position to find the first character
 * that matches any character in the buffer. If no such character is found, the function returns -1.
 * 
 * @param str The String object to search in. Must not be NULL.
 * @param buffer The buffer containing characters to search for. Must not be NULL.
 * @param pos The starting position for the search. Must be within the bounds of the string.
 * 
 * @return The index of the first matching character, or -1 if no match is found or an error occurs.
 */
int string_find_first_of(const String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL || buffer == NULL || pos >= str->size) {
        STRING_LOG("[string_find_first_of]: Error - invalid arguments.\n");
        return -1;
    }
    bool set[256];
    sstr_make_set(buffer, set);
    const unsigned char* s = (const unsigned char*)str->dataStr;
    for (size_t i = pos; i < str->size && i <= (size_t)INT_MAX; ++i) {
        if (set[s[i]]) {
            return (int)i;
        }
    }
    return -1;
}


/**
 * @brief Finds the last occurrence of any character from the buffer in the string up to the given position.
 *
 * This function searches the string up to the specified position to find the last character
 * that matches any character in the buffer. If no such character is found, the function returns -1.
 * 
 * @param str The String object to search in. Must not be NULL.
 * @param buffer The buffer containing characters to search for. Must not be NULL.
 * @param pos The ending position for the search. Must be within the bounds of the string.
 * 
 * @return The index of the last matching character, or -1 if no match is found or an error occurs.
 */
int string_find_last_of(const String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL || buffer == NULL || str->size == 0) {
        STRING_LOG("[string_find_last_of]: Error - invalid arguments or empty string.\n");
        return -1;
    }
    if (pos >= str->size) {
        pos = str->size - 1;
    }
    bool set[256];
    sstr_make_set(buffer, set);
    const unsigned char* s = (const unsigned char*)str->dataStr;
    for (size_t i = pos + 1; i-- > 0; ) {
        if (set[s[i]] && i <= (size_t)INT_MAX) {
            return (int)i;
        }
    }
    return -1;
}


/**
 * @brief Finds the first character in the string that does not match any character in the buffer starting from the given position.
 *
 * This function searches the string starting from the specified position to find the first character
 * that does not match any character in the buffer. If all characters match, the function returns -1.
 * 
 * @param str The String object to search in. Must not be NULL.
 * @param buffer The buffer containing characters to exclude. Must not be NULL.
 * @param pos The starting position for the search. Must be within the bounds of the string.
 * 
 * @return The index of the first non-matching character, or -1 if all characters match or an error occurs.
 */
int string_find_first_not_of(const String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL || buffer == NULL || pos >= str->size) {
        STRING_LOG("[string_find_first_not_of]: Error - invalid arguments.\n");
        return -1;
    }
    bool set[256];
    sstr_make_set(buffer, set);
    const unsigned char* s = (const unsigned char*)str->dataStr;
    for (size_t i = pos; i < str->size && i <= (size_t)INT_MAX; ++i) {
        if (!set[s[i]]) {
            return (int)i;
        }
    }
    return -1;
}


/**
 * @brief Finds the last position in the string that does not match any character in the buffer.
 *
 * This function searches from the given position backwards to find the first character in the string
 * that does not match any of the characters in the buffer. If no such character is found, the function returns -1.
 * 
 * @param str The String object to search in. Must not be NULL.
 * @param buffer The buffer containing characters to exclude. Must not be NULL or empty.
 * @param pos The starting position for the search. If the position is larger than the string, it starts from the end.
 * 
 * @return The index of the last character not in the buffer, or -1 if all characters match or an error occurs.
 */
int string_find_last_not_of(const String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL || buffer == NULL || str->size == 0) {
        STRING_LOG("[string_find_last_not_of]: Error - invalid arguments or empty string.\n");
        return -1;
    }
    if (pos >= str->size) {
        pos = str->size - 1;
    }
    /* An empty set excludes nothing: pos itself is the answer (like
       std::string::find_last_not_of and string_find_first_not_of). */
    bool set[256];
    sstr_make_set(buffer, set);
    const unsigned char* s = (const unsigned char*)str->dataStr;
    for (size_t i = pos + 1; i-- > 0; ) {
        if (!set[s[i]] && i <= (size_t)INT_MAX) {
            return (int)i;
        }
    }
    return -1;
}


/**
 * @brief Returns a pointer to the last character of the string.
 *
 * This function returns a pointer to the last character in the string data, allowing reverse iteration.
 * If the input String is NULL, uninitialized, or empty, an error is logged, and the function returns NULL.
 *
 * @param str The String object to retrieve the reverse beginning pointer from. Must not be NULL or empty.
 * @return A pointer to the last character in the string, or NULL on error.
 */
char *string_rbegin(const String *str) {
    if (str == NULL || str->dataStr == NULL || str->size == 0) {
        STRING_LOG("[string_rbegin]: Error - Invalid input, uninitialized, or empty String.\n");
        return NULL; 
    }
    STRING_LOG("[string_rbegin]: Returning the reverse beginning of the string.\n");
    return str->dataStr + str->size - 1;
}


/**
 * @brief This function returns a pointer to one position before the start of the string data.
 * This is typically used in reverse iteration contexts. Never dereference it. For an empty string it
 * returns NULL, the same value string_rbegin returns, so a reverse loop runs zero times.
 * If the input String is NULL or uninitialized, an error is logged, and the function returns NULL.
 *
 * @param str The String object for which to get the reverse end pointer. Must not be NULL.
 * 
 * @return A pointer to one before the first character in the string, or NULL on error.
 */
char *string_rend(const String *str) {
    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_rend]: Error - Invalid input or uninitialized String.\n");
        return NULL;
    }
    if (str->size == 0) {
        return NULL;   /* == string_rbegin of an empty string: the loop runs zero times */
    }
    /* one before the first character, computed on the integer value: pointer
       arithmetic before the start of the array is undefined behaviour */
    return (char *)((uintptr_t)str->dataStr - 1);
}


/**
 * @brief This function returns a pointer to the last character of the string data, allowing reverse iteration.
 * If the input String is NULL, uninitialized, or empty, an error is logged, and the function returns NULL.
 *
 * @param str The String object for which to get the reverse beginning pointer. Must not be NULL or empty.
 * @return A constant pointer to the last character in the string, or NULL on error.
 */
const char *string_crbegin(const String *str) {
    if (str == NULL || str->dataStr == NULL || str->size == 0) { 
        STRING_LOG("[string_crbegin]: Error - Invalid input, uninitialized, or empty String.\n");
        return NULL;  
    }
    STRING_LOG("[string_crbegin]: Returning the reverse beginning of the string.\n");
    return str->dataStr + str->size - 1; 
}


/**
 * @brief This function returns a pointer to one position before the start of the string data, allowing reverse iteration to stop.
 * If the input String is NULL or uninitialized, an error is logged, and the function returns NULL.
 *
 * @param str The String object for which to get the reverse end pointer. Must not be NULL.
 * @return A constant pointer to one before the first character in the string, or NULL on error.
 */
const char *string_crend(const String *str) {
    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_crend]: Error - Invalid input or uninitialized String.\n");
        return NULL; 
    }
    if (str->size == 0) {
        return NULL;   /* == string_crbegin of an empty string */
    }
    STRING_LOG("[string_crend]: Returning one before the first character.\n");
    return (const char *)((uintptr_t)str->dataStr - 1);
}


/**
 * @brief This function creates a new C-string with all characters in the given String object converted to uppercase.
 * If the input String is NULL, an error is logged, and the function returns NULL.
 *
 * @param str The String object to be converted. Must not be NULL.
 * @return A new C-string with all characters in uppercase. The caller is responsible for freeing the allocated memory.
 */
char* string_to_upper(const String* str) {
    if (str != NULL) {
        STRING_LOG("[string_to_upper]: Converting string to uppercase.\n");
        
        char* upper = (char*) malloc(sizeof(char) * (str->size + 1));
        if (!upper) {
            STRING_LOG("[string_to_upper]: Error - Failed to allocate memory.\n");
            return NULL;
        }

        for (size_t index = 0; index < str->size; index++) {
            upper[index] = sstr_ascii_toupper(str->dataStr[index]);
        }
        upper[str->size] = '\0';
        STRING_LOG("[string_to_upper]: Successfully converted string to uppercase.\n");
        return upper;
    }

    STRING_LOG("[string_to_upper]: Error - Input string is NULL.\n");
    return NULL;
}


/**
 * @brief This function creates a new C-string with all characters in the given String object converted to lowercase.
 * If the input String is NULL, an error is logged, and the function returns NULL.
 *
 * @param str The String object to be converted. Must not be NULL.
 * @return A new C-string with all characters in lowercase. The caller is responsible for freeing the allocated memory.
 */
char* string_to_lower(const String* str) {
    if (str != NULL) {
        STRING_LOG("[string_to_lower]: Converting string to lowercase.\n");
        
        char* lower = (char*) malloc(sizeof(char) * (str->size + 1));
        if (!lower) {
            STRING_LOG("[string_to_lower]: Error - Failed to allocate memory.\n");
            return NULL;
        }

        for (size_t index = 0; index < str->size; index++) {
            lower[index] = sstr_ascii_tolower(str->dataStr[index]);
        }
        lower[str->size] = '\0';
        STRING_LOG("[string_to_lower]: Successfully converted string to lowercase.\n");
        return lower;
    }

    STRING_LOG("[string_to_lower]: Error - Input string is NULL.\n");
    return NULL;
}


/**
 * @brief This function sets a new memory pool size for the given String object. If a memory pool already exists,
 * it is destroyed and a new one is created with the specified size. If the String object has existing data,
 * it is reallocated in the new pool.
 * If the input String is NULL, or the new size is zero, an error is logged, and the function returns false.
 *
 * @param str The String object for which the memory pool size is to be set. Must not be NULL.
 * @param newSize The new size of the memory pool. Must be greater than zero.
 * 
 * @return true if the memory pool was successfully resized, false otherwise.
 */
bool string_set_pool_size(String* str, size_t newSize) {
    if (!str) {
        STRING_LOG("[string_set_pool_size]: Error - 'str' is NULL.\n");
        return false;
    }
    if (newSize == 0) {
        STRING_LOG("[string_set_pool_size]: Error - 'newSize' is zero.\n");
        return false;
    }
    /* Strings have no pools any more: this reserves room for newSize bytes,
       keeping the content. */
    return newSize < SIZE_MAX && sstr_reserve_cap(str, newSize + 1);
}


/**
 * @brief This function removes all leading whitespace characters from the String object.
 * If the String object is NULL or empty, the function returns without performing any operation.
 *
 * @param str The String object to be trimmed. Must not be NULL.
 */
void string_trim_left(String *str) {
    if (str == NULL) {
        STRING_LOG("[string_trim_left]: Error - String object is NULL.\n");
        return;
    }
    if (str->size == 0) {
        STRING_LOG("[string_trim_left]: String is empty, no trimming needed.\n");
        return;
    }

    size_t i = 0;
    while (i < str->size && sstr_is_ascii_space(str->dataStr[i])) {
        i++;
    }

    if (i > 0) {
        STRING_LOG("[string_trim_left]: Trimming %zu leading whitespace characters.\n", i);

        memmove(str->dataStr, str->dataStr + i, str->size - i);
        str->size -= i;
        str->dataStr[str->size] = '\0';
    } 
    else {
        STRING_LOG("[string_trim_left]: No leading whitespace found.\n");
    }
}


/**
 * @brief This function removes all trailing whitespace characters from the String object.
 * If the String object is NULL or empty, the function returns without performing any operation.
 *
 * @param str The String object to be trimmed. Must not be NULL.
 */
void string_trim_right(String *str) {
    STRING_LOG("[string_trim_right]: Function start.");
    
    if (str == NULL) {
        STRING_LOG("[string_trim_right]: Error - Null String object.");
        return;
    }
    if (str->size == 0) {
        STRING_LOG("[string_trim_right]: String is empty, no trimming needed.");
        return;
    }

    size_t i = str->size;
    while (i > 0 && sstr_is_ascii_space(str->dataStr[i - 1])) {
        i--;
    }

    if (i < str->size) {
        STRING_LOG("[string_trim_right]: Trimming %zu trailing whitespace characters.", str->size - i);
        str->dataStr[i] = '\0';
        str->size = i;
    } 
    else {
        STRING_LOG("[string_trim_right]: No trailing whitespace found.");
    }

    STRING_LOG("[string_trim_right]: Function end.");
}


/**
 * @brief This function removes all leading and trailing whitespace characters from the String object.
 * It combines the operations of string_trim_left and string_trim_right.
 * If the String object is NULL, the function returns without performing any operation.
 *
 * @param str The String object to be trimmed. Must not be NULL.
 */
void string_trim(String *str) {
    STRING_LOG("[string_trim]: Function start.");

    if (str == NULL || str->size == 0) {
        return;
    }
    /* one move instead of trim_left + trim_right */
    size_t begin = 0, stop = str->size;
    while (begin < stop && sstr_is_ascii_space(str->dataStr[begin])) {
        begin++;
    }
    while (stop > begin && sstr_is_ascii_space(str->dataStr[stop - 1])) {
        stop--;
    }
    if (begin > 0) {
        memmove(str->dataStr, str->dataStr + begin, stop - begin);
    }
    str->size = stop - begin;
    str->dataStr[str->size] = '\0';
}


/**
 * @brief Splits a String object into an array of String objects based on a specified delimiter.
 *
 * This function takes a String object and splits it into multiple String objects using the specified delimiter.
 * The result is an array of String pointers, and the number of resulting splits is stored in the count parameter.
 * If the input String object, delimiter, or memory allocation fails, the function logs an error and returns NULL.
 *
 * @param str The String object to split. Must not be NULL.
 * @param delimiter The delimiter used to split the string. Must not be NULL.
 * @param count A pointer to an integer where the number of splits will be stored. Must not be NULL.
 * @return An array of String pointers containing the split strings, or NULL if an error occurs.
 */
String** string_split(const String *str, const char *delimiter, int *count) {
    STRING_LOG("[string_split]: Function start.");

    if (count) {
        *count = 0;
    }
    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_split]: Error - Null String object.");
        return NULL;
    }
    if (delimiter == NULL || delimiter[0] == '\0') {
        STRING_LOG("[string_split]: Error - Null or empty delimiter.");
        return NULL;
    }


    const char* start = str->dataStr;
    const char* end = start + str->size;
    size_t delimLen = strlen(delimiter);

    size_t cap = 8, fields = 0;
    String** splits = (String**)malloc(cap * sizeof(String*));
    if (splits == NULL) {
        STRING_LOG("[string_split]: Error - Memory allocation failed for splits array.");
        return NULL;
    }
    for (;;) {
        const char* hit = sstr_search(start, (size_t)(end - start), delimiter, delimLen);
        const char* stop = hit ? hit : end;
        if (fields == cap) {
            String** grown = (cap <= SIZE_MAX / 2 / sizeof(String*) && cap * 2 <= (size_t)INT_MAX)
                             ? (String**)realloc(splits, cap * 2 * sizeof(String*)) : NULL;
            if (grown == NULL) {
                break;                               /* cleaned up below */
            }
            splits = grown;
            cap *= 2;
        }
        String* part = sstr_new(start, (size_t)(stop - start));
        if (part == NULL) {
            break;
        }
        splits[fields++] = part;
        if (hit == NULL) {
            if (count) {
                *count = (int)fields;
            }
            return splits;
        }
        start = hit + delimLen;
    }
    STRING_LOG("[string_split]: Error - Memory allocation failed.");
    for (size_t i = 0; i < fields; i++) {
        string_deallocate(splits[i]);
    }
    free(splits);
    return NULL;
}


/**
 * @brief Joins an array of String objects into a single String object using a specified delimiter.
 *
 * This function takes an array of String objects and concatenates them into a single String object,
 * with each element separated by the specified delimiter. If the input string array, count, or delimiter
 * is invalid, or if memory allocation fails, the function logs an error and returns NULL.
 *
 * @param strings The array of String pointers to be joined. Must not be NULL.
 * @param count The number of String objects in the array. Must be greater than 0.
 * @param delimiter The delimiter to be used between each string. Must not be NULL.
 * 
 * @return A new String object containing the concatenated result, or NULL if an error occurs.
 */
String* string_join(String **strings, int count, const char *delimiter) {
    STRING_LOG("[string_join]: Function start.");

    if (strings == NULL || count <= 0 || delimiter == NULL) {
        STRING_LOG("[string_join]: Error - invalid arguments.");
        return NULL;
    }
    /* total size first: one allocation, binary-safe pieces */
    size_t delimLen = strlen(delimiter);
    size_t total = 0;
    for (int i = 0; i < count; i++) {
        size_t part = strings[i] ? strings[i]->size : 0;
        size_t add = part + (i + 1 < count ? delimLen : 0);
        if (add > SIZE_MAX - 1 - total) {
            return NULL;
        }
        total += add;
    }
    String *result = sstr_new(NULL, 0);
    if (result == NULL || !sstr_reserve_cap(result, total + 1)) {
        string_deallocate(result);
        STRING_LOG("[string_join]: Error - Memory allocation failed.");
        return NULL;
    }
    char* out = result->dataStr;
    for (int i = 0; i < count; i++) {
        if (strings[i] && strings[i]->size) {
            memcpy(out, strings[i]->dataStr, strings[i]->size);
            out += strings[i]->size;
        }
        if (i + 1 < count && delimLen) {
            memcpy(out, delimiter, delimLen);
            out += delimLen;
        }
    }
    *out = '\0';
    result->size = total;
    return result;
}


/**
 * @brief Replaces all occurrences of a substring within a String object with another substring.
 *
 * This function searches for all occurrences of the specified oldStr within the given String object
 * and replaces them with the newStr. If either the String object or the substrings are NULL, 
 * the function logs an error and does nothing. If memory allocation fails during the process, 
 * the function logs an error and leaves the original string unchanged.
 *
 * @param str The String object in which the replacement will occur. Must not be NULL.
 * @param oldStr The substring to be replaced. Must not be NULL.
 * @param newStr The substring to replace oldStr with. Must not be NULL.
 */
void string_replace_all(String *str, const char *oldStr, const char *newStr) {
    STRING_LOG("[string_replace_all]: Function start.");

    if (str == NULL || oldStr == NULL || newStr == NULL) {
        STRING_LOG("[string_replace_all]: Error - NULL argument.");
        return;
    }
    /* An empty pattern matches everywhere and never advances: nothing to replace. */
    size_t oldLen = strlen(oldStr);
    if (oldLen == 0) {
        return;
    }
    size_t newLen = strlen(newStr);
    const char* text = str->dataStr;
    const char* end = text + str->size;
    const char* hit = sstr_search(text, str->size, oldStr, oldLen);
    if (hit == NULL) {
        return;
    }

    size_t cap = str->size + 1;
    if (newLen > oldLen) {
        size_t extra = (newLen - oldLen) * 8;
        cap = (extra <= SIZE_MAX - cap) ? cap + extra : SIZE_MAX;
    }
    char* out = (char*)malloc(cap);
    if (out == NULL) {
        STRING_LOG("[string_replace_all]: Error - Memory allocation failed.");
        return;
    }
    size_t w = 0;
    const char* p = text;
    while (hit != NULL) {
        size_t keep = (size_t)(hit - p);
        size_t rest = (size_t)(end - hit) - oldLen;           /* still to come after this match */
        if (keep > SIZE_MAX - w || newLen > SIZE_MAX - w - keep || rest > SIZE_MAX - 1 - w - keep - newLen) {
            free(out);
            STRING_LOG("[string_replace_all]: Error - Result too large.");
            return;
        }
        size_t need = w + keep + newLen + rest + 1;           /* enough for the rest unchanged */
        if (need > cap) {
            size_t ncap = (cap <= SIZE_MAX / 2) ? cap * 2 : need;
            if (ncap < need) {
                ncap = need;
            }
            char* grown = (char*)realloc(out, ncap);
            if (grown == NULL) {
                free(out);
                STRING_LOG("[string_replace_all]: Error - Memory allocation failed.");
                return;
            }
            out = grown;
            cap = ncap;
        }
        memcpy(out + w, p, keep);
        w += keep;
        memcpy(out + w, newStr, newLen);
        w += newLen;
        p = hit + oldLen;
        hit = sstr_search(p, (size_t)(end - p), oldStr, oldLen);
    }
    size_t tail = (size_t)(end - p);
    memcpy(out + w, p, tail);           /* room guaranteed by the last `need` */
    w += tail;
    out[w] = '\0';

    if (w < STRING_SSO_CAPACITY) {
        if (!sstr_is_inline(str)) {
            free(str->dataStr);
        }
        memcpy(str->sso, out, w + 1);
        str->dataStr = str->sso;
        str->capacitySize = STRING_SSO_CAPACITY;
        free(out);
    }
    else {
        if (!sstr_is_inline(str)) {
            free(str->dataStr);
        }
        str->dataStr = out;
        str->capacitySize = cap;
    }
    str->size = w;
}


/**
 * @brief Converts the content of a String object to an integer.
 *
 * This function converts the content of the given String object to an integer. 
 * If the String object is NULL or empty, the function logs an error and returns 0.
 *
 * @param str The String object to convert. Must not be NULL.
 * @return The integer value of the String object's content, or 0 if the String object is NULL or empty.
 */
int string_to_int(const String *str) {
    STRING_LOG("[string_to_int]: Function start.");
    
    if (str == NULL) {
        STRING_LOG("[string_to_int]: Error - Null String object.");
        return 0;
    }
    if (string_empty(str)) {
        STRING_LOG("[string_to_int]: Error - Empty string.");
        return 0;
    }

    int result = atoi(str->dataStr);
    STRING_LOG("[string_to_int]: Converted '%s' to integer %d.", str->dataStr, result);
    STRING_LOG("[string_to_int]: Function end.");
    
    return result;
}


/**
 * @brief Converts the content of a String object to a floating-point number.
 *
 * This function converts the content of the given String object to a floating-point number (float).
 * If the String object is NULL or empty, the function logs an error and returns 0.0f.
 *
 * @param str The String object to convert. Must not be NULL.
 * @return The floating-point value of the String object's content, or 0.0f if the String object is NULL or empty.
 */
float string_to_float(const String *str) {
    STRING_LOG("[string_to_float]: Function start.");

    if (str == NULL) {
        STRING_LOG("[string_to_float]: Error - Null String object.");
        return 0.0f;
    }
    if (string_empty(str)) {
        STRING_LOG("[string_to_float]: Error - Empty string.");
        return 0.0f;
    }

    float result = strtof(str->dataStr, NULL);   /* rounded to float once (atof gave a double) */
    STRING_LOG("[string_to_float]: Converted '%s' to float %f.", str->dataStr, result);
    STRING_LOG("[string_to_float]: Function end.");

    return result;
}


/**
 * @brief Converts the content of a String object to a double-precision floating-point number.
 *
 * This function converts the content of the given String object to a double-precision floating-point number (double).
 * If the String object is NULL or empty, the function logs an error and returns 0.0.
 *
 * @param str The String object to convert. Must not be NULL.
 * @return The double value of the String object's content, or 0.0 if the String object is NULL or empty.
 */
double string_to_double(const String* str) {
    STRING_LOG("[string_to_double]: Function start.");

    if (str == NULL) {
        STRING_LOG("[string_to_double]: Error - Null String object.");
        return 0.0;
    }
    if (string_empty(str)) {
        STRING_LOG("[string_to_double]: Error - Empty string.");
        return 0.0;
    }

    double result = strtod(str->dataStr, NULL);
    STRING_LOG("[string_to_double]: Converted '%s' to double %lf.", str->dataStr, result);
    STRING_LOG("[string_to_double]: Function end.");

    return result;
}


/**
 * @brief Pads the left side of a String object with a specified character until it reaches the desired total length.
 *
 * This function pads the left side of a String object with a specified character (padChar) until the String reaches
 * the totalLength specified. If the current size of the String object is already equal to or greater than totalLength,
 * the function logs an error and does nothing. If memory allocation fails, it logs an error and the original string remains unchanged.
 *
 * @param str The String object to be padded. Must not be NULL.
 * @param totalLength The desired total length of the String object after padding. Must be greater than the current size.
 * @param padChar The character to use for padding.
 */
void string_pad_left(String *str, size_t totalLength, char padChar) {
    STRING_LOG("[string_pad_left]: Padding left with character '%c' to total length %zu.", padChar, totalLength);

    if (str == NULL || str->size >= totalLength || totalLength == SIZE_MAX) {
        return;
    }
    size_t padSize = totalLength - str->size;
    if (!sstr_reserve_cap(str, totalLength + 1)) {
        STRING_LOG("[string_pad_left]: Error - Failed to allocate memory.");
        return;
    }
    memmove(str->dataStr + padSize, str->dataStr, str->size + 1);
    memset(str->dataStr, padChar, padSize);
    str->size = totalLength;
}


/**
 * @brief Pads the right side of a String object with a specified character until it reaches the desired total length.
 *
 * This function pads the right side of a String object with a specified character (padChar) until the String reaches
 * the totalLength specified. If the current size of the String object is already equal to or greater than totalLength,
 * the function logs an error and does nothing. If memory allocation fails, it logs an error and the original string remains unchanged.
 *
 * @param str The String object to be padded. Must not be NULL.
 * @param totalLength The desired total length of the String object after padding. Must be greater than the current size.
 * @param padChar The character to use for padding.
 */
void string_pad_right(String *str, size_t totalLength, char padChar) {
    STRING_LOG("[string_pad_right]: Padding right with character '%c' to total length %zu.", padChar, totalLength);

    if (str == NULL || str->size >= totalLength || totalLength == SIZE_MAX) {
        return;
    }
    if (!sstr_reserve_cap(str, totalLength + 1)) {
        STRING_LOG("[string_pad_right]: Error - Failed to allocate memory.");
        return;
    }
    memset(str->dataStr + str->size, padChar, totalLength - str->size);
    str->dataStr[totalLength] = '\0';
    str->size = totalLength;
}


/**
 * @brief Converts the content of a String object to its hexadecimal representation.
 *
 * This function converts each character in the given String object to its corresponding hexadecimal value and
 * returns a new String object containing the result. If the input String object is NULL or empty, the function
 * logs an error and returns an empty String object or NULL respectively. If memory allocation fails during the process,
 * it returns NULL.
 *
 * @param str The String object to convert to hexadecimal. Must not be NULL.
 * @return A new String object containing the hexadecimal representation of the input string, or NULL if an error occurs.
 */
String* string_to_hex(const String *str) {
    STRING_LOG("[string_to_hex]: Converting String to hexadecimal.");

    if (str == NULL) {
        return NULL;
    }
    if (str->size > (SIZE_MAX - 1) / 2) {
        return NULL;
    }
    static const char digits[] = "0123456789abcdef";
    String *hexStr = sstr_new(NULL, 0);
    if (hexStr == NULL || !sstr_reserve_cap(hexStr, str->size * 2 + 1)) {
        string_deallocate(hexStr);
        return NULL;
    }
    char* out = hexStr->dataStr;
    const unsigned char* in = (const unsigned char*)str->dataStr;
    for (size_t i = 0; i < str->size; ++i) {
        out[2 * i]     = digits[in[i] >> 4];
        out[2 * i + 1] = digits[in[i] & 0x0F];
    }
    hexStr->size = str->size * 2;
    out[hexStr->size] = '\0';
    return hexStr;
}


/**
 * @brief Converts a hex-encoded string to a new String object.
 *
 * This function takes a hex-encoded string (e.g., "48656c6c6f") and converts it into a new String object containing the
 * corresponding ASCII characters (e.g., "Hello"). The hex string must have an even number of characters; otherwise,
 * the function logs an error and returns NULL. If the input String object is NULL or memory allocation fails,
 * the function also returns NULL.
 *
 * @param hexStr The hex-encoded String object to convert. Must not be NULL and must have an even number of characters.
 * @return A new String object containing the decoded ASCII characters, or NULL if an error occurs
 *         (including a character that is not a hex digit).
 */
String* string_from_hex(const String *hexStr) {
    STRING_LOG("[string_from_hex]: Function start.");

    if (hexStr == NULL) {
        return NULL;
    }
    if (string_empty(hexStr) || (hexStr->size % 2) != 0) {
        STRING_LOG("[string_from_hex]: Error - Invalid hex string size (%zu).", hexStr->size);
        return NULL;
    }
    String *str = sstr_new(NULL, 0);
    if (str == NULL || !sstr_reserve_cap(str, hexStr->size / 2 + 1)) {
        string_deallocate(str);
        return NULL;
    }
    for (size_t i = 0; i < hexStr->size; i += 2) {
        int hi = sstr_hex_digit(hexStr->dataStr[i]);
        int lo = sstr_hex_digit(hexStr->dataStr[i + 1]);
        if (hi < 0 || lo < 0) {
            /* strtol turned "zz" into 0 and "4G" into 4 without complaint */
            STRING_LOG("[string_from_hex]: Error - Invalid hex digit at position %zu.", i);
            string_deallocate(str);
            return NULL;
        }
        str->dataStr[i / 2] = (char)(hi * 16 + lo);
    }
    str->size = hexStr->size / 2;
    str->dataStr[str->size] = '\0';
    return str;
}


/**
 * @brief Counts the occurrences of a substring within a String object.
 *
 * This function counts how many times a given substring appears within a String object. If the String object,
 * its data, or the substring is NULL, the function logs an error and returns 0.
 *
 * @param str The String object in which to search for the substring. Must not be NULL.
 * @param substr The substring to count within the String object. Must not be NULL.
 * 
 * @return The number of times the substring appears within the String object, or 0 if an error occurs.
 */
size_t string_count(const String* str, const char* substr) {
    STRING_LOG("[string_count]: Function start.");

    if (str == NULL || substr == NULL || str->dataStr == NULL) {
        return 0;
    }
    /* An empty substring matches between every pair of characters and at
       both ends (Python: "abc".count("") == 4). */
    size_t len = strlen(substr);
    if (len == 0) {
        return str->size + 1;
    }
    size_t count = 0;
    const char* end = str->dataStr + str->size;
    for (const char* p = str->dataStr; (p = sstr_search(p, (size_t)(end - p), substr, len)) != NULL; p += len) {
        count++;                     /* non-overlapping, like Python's str.count */
    }
    return count;
}


/**
 * @brief Removes all occurrences of a substring from a String object.
 *
 * This function removes all instances of a specified substring from a String object. If the String object,
 * its data, or the substring is NULL, the function logs an error and does nothing. If the substring is empty,
 * the function also logs an error and does nothing.
 *
 * @param str The String object from which to remove the substring. Must not be NULL.
 * @param substr The substring to remove from the String object. Must not be NULL or empty.
 */
void string_remove(String* str, const char* substr) {
    STRING_LOG("[string_remove]: Function start.");

    if (str == NULL || substr == NULL || str->dataStr == NULL) {
        return;
    }
    size_t len = strlen(substr);
    if (len == 0) {
        STRING_LOG("[string_remove]: Error - Empty substring.");
        return;
    }
    /* substr may point into this String: search for a copy */
    char small[64];
    char* copy = NULL;
    if (sstr_aliases(str, substr)) {
        copy = (len < sizeof small) ? small : (char*)malloc(len + 1);
        if (copy == NULL) {
            return;
        }
        memcpy(copy, substr, len + 1);
        substr = copy;
    }
    /* one compacting pass (the old loop moved the whole tail for every
       occurrence: O(n^2)) */
    char* base = str->dataStr;
    const char* end = base + str->size;
    const char* r = base;
    char* w = base;
    const char* hit;
    while ((hit = sstr_search(r, (size_t)(end - r), substr, len)) != NULL) {
        size_t keep = (size_t)(hit - r);
        if (w != r) {
            memmove(w, r, keep);
        }
        w += keep;
        r = hit + len;
    }
    size_t rest = (size_t)(end - r);
    if (w != r) {
        memmove(w, r, rest);
    }
    w += rest;
    *w = '\0';
    str->size = (size_t)(w - base);
    if (copy != NULL && copy != small) {
        free(copy);
    }
}


/**
 * @brief Removes a range of characters from the String object.
 *
 * This function removes characters from the String object starting from @p startPos up to but not including @p endPos.
 * It adjusts the size of the String accordingly. If the String object or its data is NULL, or if the specified range
 * is invalid, the function logs an error and does nothing.
 *
 * @param str The String object from which to remove characters. Must not be NULL.
 * @param startPos The starting position of the range to remove (inclusive). Must be less than @p endPos.
 * 
 * @param endPos The ending position of the range to remove (exclusive). Must be less than or equal to the String's size.
 */
void string_remove_range(String* str, size_t startPos, size_t endPos) {
    STRING_LOG("[string_remove_range]: Function start.");

    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_remove_range]: Error - Null String object or data.");
        return;
    }
    if (startPos >= endPos || endPos > str->size) {
        STRING_LOG("[string_remove_range]: Error - Invalid range (start: %zu, end: %zu, size: %zu).", startPos, endPos, str->size);
        return;
    }

    size_t length = endPos - startPos;
    STRING_LOG("[string_remove_range]: Removing range from %zu to %zu (length: %zu).", startPos, endPos, length);

    memmove(str->dataStr + startPos, str->dataStr + endPos, str->size - endPos + 1); // +1 for null terminator
    str->size -= length;

    STRING_LOG("[string_remove_range]: New size after removal: %zu.", str->size);
    STRING_LOG("[string_remove_range]: Function end.");
}


/**
 * @brief Creates a new String object from an integer value.
 *
 * This function converts an integer value to its string representation and returns a new String object containing this representation.
 *
 * @param value The integer value to convert.
 * @return A new String object containing the string representation of the integer value.
 */
String* string_from_int(int value) {
    STRING_LOG("[string_from_int]: Converting int %d to String.", value);
    char buffer[12];
    char* text = sstr_format_int(buffer, value);           /* no snprintf */
    return sstr_new(text, (size_t)(buffer + 11 - text));
}


/**
 * @brief Converts an integer value to a C-string representation.
 *
 * This function converts an integer value to its C-string representation and returns a pointer to a newly allocated
 * string containing this representation. The caller is responsible for freeing the returned string.
 *
 * @param value The integer value to convert.
 * @return A pointer to a newly allocated C-string containing the string representation of the integer value.
 */
char* string_from_int_cstr(int value) {
    STRING_LOG("[string_from_int_cstr]: Converting int %d to C-string.", value);
    char buffer[12];
    char* text = sstr_format_int(buffer, value);
    size_t n = (size_t)(buffer + 11 - text);
    char* result = (char*)malloc(n + 1);
    if (result) {
        memcpy(result, text, n + 1);
    }
    return result;
}


/**
 * @brief Creates a new String object from a float value.
 *
 * This function converts a floating-point number to its string representation
 * and returns a new String object containing this representation.
 *
 * @param value The float value to convert.
 * 
 * @return A new String object containing the string representation of the float value.
 */
String* string_from_float(float value) {
    STRING_LOG("[string_from_float]: Converting float %f to String.", (double)value);

    /* %f prints every integer digit: -FLT_MAX needs 47 characters; 64 bytes
       hold any float. */
    char buffer[64];
    int n = snprintf(buffer, sizeof(buffer), "%f", (double)value);
    if (n < 0) {
        return NULL;
    }
    return sstr_new(buffer, (size_t)n < sizeof buffer ? (size_t)n : sizeof buffer - 1);
}

/**
 * @brief Creates a new String object from a float value.
 *
 * This function converts a floating-point number to its string representation
 * and returns a new String object containing this representation.
 *
 * @param value The float value to convert.
 * 
 * @return A new String object containing the string representation of the float value.
 */
String* string_from_double(double value) {
    STRING_LOG("[string_from_double]: Function start. Converting double %f to String.", value);

    /* "%f" of a large value needs up to ~320 characters (DBL_MAX); a fixed
       32-byte buffer silently cut it to a wrong number. */
    char small[64];
    char* buffer = small;
    int need = snprintf(NULL, 0, "%f", value);
    if (need < 0) {
        return NULL;
    }
    if ((size_t)need >= sizeof(small)) {
        buffer = (char*)malloc((size_t)need + 1);
        if (buffer == NULL) {
            STRING_LOG("[string_from_double]: Error - Memory allocation failed.");
            return NULL;
        }
    }
    snprintf(buffer, (size_t)need + 1, "%f", value);

    String* result = string_create(buffer);
    if (buffer != small) {
        free(buffer);
    }
    if (result) {
        STRING_LOG("[string_from_double]: Successfully created String from double.");
    } 
    else {
        STRING_LOG("[string_from_double]: Error - Memory allocation failed.");
    }

    STRING_LOG("[string_from_double]: Function end.");
    return result;
}


/**
 * @brief This function tokenizes the content of the given String object using the specified delimiters.
 * It returns an array of String objects, each representing a token. The number of tokens is
 * stored in the variable pointed to by @p count. If the input String object or delimiters are NULL,
 * the function returns NULL and logs an error.
 *
 * @param str The String object to tokenize. Must not be NULL.
 * @param delimiters A C-string containing the delimiter characters. Must not be NULL.
 * @param count A pointer to an integer where the number of tokens will be stored.
 * @return An array of String objects, each representing a token, or NULL if an error occurs.
 */
String** string_tokenize(const String* str, const char* delimiters, int* count) {
    STRING_LOG("[string_tokenize]: Function start.");

    if (count) {
        *count = 0;
    }
    if (str == NULL || str->dataStr == NULL || delimiters == NULL) {
        STRING_LOG("[string_tokenize]: Error - Invalid input (str or delimiters is NULL).");
        return NULL;
    }
    /* a delimiter table instead of strspn / strcspn: one pass, and the scan
       covers the whole String (strspn stopped at an embedded NUL) */
    bool delim[256];
    sstr_make_set(delimiters, delim);
    const unsigned char* s = (const unsigned char*)str->dataStr;
    size_t n = str->size;

    size_t num_tokens = 0;
    for (size_t i = 0; i < n; ) {
        while (i < n && delim[s[i]]) {
            i++;
        }
        if (i == n) {
            break;
        }
        num_tokens++;
        while (i < n && !delim[s[i]]) {
            i++;
        }
    }
    if (num_tokens == 0 || num_tokens > (size_t)INT_MAX || num_tokens > SIZE_MAX / sizeof(String*)) {
        return NULL;
    }

    String** tokens = (String**)malloc(num_tokens * sizeof(String*));
    if (tokens == NULL) {
        STRING_LOG("[string_tokenize]: Error - Memory allocation failed for tokens array.");
        return NULL;
    }
    size_t idx = 0;
    for (size_t i = 0; i < n && idx < num_tokens; ) {
        while (i < n && delim[s[i]]) {
            i++;
        }
        size_t start = i;
        while (i < n && !delim[s[i]]) {
            i++;
        }
        String* tok = sstr_new(str->dataStr + start, i - start);
        if (tok == NULL) {
            for (size_t k = 0; k < idx; ++k) {
                string_deallocate(tokens[k]);
            }
            free(tokens);
            return NULL;
        }
        tokens[idx++] = tok;
    }
    if (count) {
        *count = (int)num_tokens;
    }
    return tokens;
}


/**
 * @brief Compares two String objects in a case-insensitive manner.
 *
 * This function compares two String objects without considering case sensitivity.
 * It returns an integer less than, equal to, or greater than zero if the first 
 * string is found to be less than, equal to, or greater than the second string, 
 * respectively. If either String object or their dataStr is NULL, the function 
 * considers NULL values and handles them accordingly.
 *
 * @param str1 The first String object to compare. Can be NULL.
 * @param str2 The second String object to compare. Can be NULL.
 * 
 * @return An integer indicating the result of the comparison:
 *         - 0 if the strings are equal.
 *         - A negative value if str1 is less than str2.
 *         - A positive value if str1 is greater than str2.
 */
int string_compare_ignore_case(const String* str1, const String* str2) {
    STRING_LOG("[string_compare_ignore_case]: Function start.");

    if (str1 == NULL || str2 == NULL) {
        if (str1 == str2) {
            return 0;
        }
        return (str1 == NULL) ? -1 : 1;
    }
    if (str1->dataStr == NULL || str2->dataStr == NULL) {
        if (str1->dataStr == str2->dataStr) {
            return 0;
        }
        return (str1->dataStr == NULL) ? -1 : 1;
    }
    /* ASCII case folding, byte-wise over the whole Strings (strcasecmp
       depended on the locale and stopped at a NUL) */
    size_t n = str1->size < str2->size ? str1->size : str2->size;
    for (size_t i = 0; i < n; ++i) {
        unsigned char a = (unsigned char)sstr_ascii_tolower(str1->dataStr[i]);
        unsigned char b = (unsigned char)sstr_ascii_tolower(str2->dataStr[i]);
        if (a != b) {
            return a < b ? -1 : 1;
        }
    }
    return (str1->size < str2->size) ? -1 : (str1->size > str2->size) ? 1 : 0;
}


/**
 * @brief Encodes a String object into Base64 format.
 *
 * This function takes a String object and encodes its content into Base64 format.
 * The encoded result is returned as a new String object. If the input String object 
 * or its dataStr is NULL, the function returns NULL and logs an error.
 *
 * @param input The String object to encode. Must not be NULL.
 * 
 * @return A new String object containing the Base64-encoded data, or NULL if an error occurs.
 */
String* string_base64_encode(const String *input) {
    STRING_LOG("[string_base64_encode]: Function start.");

    if (input == NULL || input->dataStr == NULL) {
        return NULL;
    }
    if (input->size / 3 >= (SIZE_MAX - 5) / 4) {
        return NULL;
    }
    size_t outLen = (input->size + 2) / 3 * 4;
    String *encoded = sstr_new(NULL, 0);
    if (encoded == NULL || !sstr_reserve_cap(encoded, outLen + 1)) {
        string_deallocate(encoded);
        return NULL;
    }
    /* whole 3-byte groups straight into the buffer, then the padded tail */
    const unsigned char* in = (const unsigned char*)input->dataStr;
    char* out = encoded->dataStr;
    size_t i = 0, o = 0;
    for (; i + 3 <= input->size; i += 3) {
        unsigned v = ((unsigned)in[i] << 16) | ((unsigned)in[i + 1] << 8) | in[i + 2];
        out[o++] = base64_chars[(v >> 18) & 0x3F];
        out[o++] = base64_chars[(v >> 12) & 0x3F];
        out[o++] = base64_chars[(v >> 6) & 0x3F];
        out[o++] = base64_chars[v & 0x3F];
    }
    size_t rest = input->size - i;
    if (rest) {
        unsigned v = (unsigned)in[i] << 16;
        if (rest == 2) {
            v |= (unsigned)in[i + 1] << 8;
        }
        out[o++] = base64_chars[(v >> 18) & 0x3F];
        out[o++] = base64_chars[(v >> 12) & 0x3F];
        out[o++] = (rest == 2) ? base64_chars[(v >> 6) & 0x3F] : '=';
        out[o++] = '=';
    }
    out[o] = '\0';
    encoded->size = o;
    return encoded;
}


/**
 * @brief This function takes a Base64-encoded String object and decodes it back into its original 
 * binary form. The decoded result is returned as a new String object. If the input 
 * encodedStr or its data is NULL, the function returns NULL and logs an error.
 *
 * @param encodedStr The Base64-encoded String object to decode. Must not be NULL.
 * 
 * @return A new String object containing the decoded binary data, or NULL if an error occurs.
 */
String* string_base64_decode(const String* encodedStr) {
    STRING_LOG("[string_base64_decode]: Function start.");
    
    if (encodedStr == NULL) {
        STRING_LOG("[string_base64_decode]: Error - The encodedStr parameter is NULL.");
        return NULL;
    }
    if (encodedStr->dataStr == NULL) {
        STRING_LOG("[string_base64_decode]: Error - The dataStr of encodedStr object is NULL.");
        return NULL;
    }

    char* decodedStr = (char*)malloc(encodedStr->size * 3 / 4 + 1); 
    if (decodedStr == NULL) {
        STRING_LOG("[string_base64_decode]: Error - Failed to allocate memory for decoded string.");
        return NULL;
    }

    int val = 0, valb = -8;
    size_t i = 0;
    size_t j = 0;

    for (i = 0; i < encodedStr->size; i++) {
        char c = encodedStr->dataStr[i];
        if (c == '=') {
            break;
        }
        if (c >= 'A' && c <= 'Z') {
            c -= 'A';
        } 
        else if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 26;
        } else if (c >= '0' && c <= '9') {
            c = c - '0' + 52;
        } 
        else if (c == '+') {
            c = 62;
        } 
        else if (c == '/') {
            c = 63;
        } 
        else {
            continue;
        }

        /* Only the low 24 bits are ever needed; without the mask the int
           shifted left on every character and overflowed (UB) on input
           longer than a few characters. */
        val = ((val << 6) | c) & 0xFFFFFF;
        valb += 6;
        if (valb >= 0) {
            decodedStr[j++] = (char)((val >> valb) & 0xFF);
            valb -= 8;
        }
    }

    decodedStr[j] = '\0';

    /* The decoded bytes are binary: build the String with their real length.
       string_create(decodedStr) stopped at the first 0x00 byte. */
    String* decodedStringObject = sstr_new(decodedStr, j);
    if (decodedStringObject == NULL) {
        STRING_LOG("[string_base64_decode]: Error - Failed to create decoded String object.");
        free(decodedStr);
        return NULL;
    }

    STRING_LOG("[string_base64_decode]: Successfully decoded the string.");
    free(decodedStr);
    
    STRING_LOG("[string_base64_decode]: Function end.");
    return decodedStringObject;
}


/**
 * @brief This function formats the content of a String object based on the provided format 
 * string and additional arguments, similar to how printf works. The formatted result 
 * is assigned to the input String object. If the input String object or format string 
 * is NULL, the function logs an error and returns without making changes.
 *
 * @param str The String object to which the formatted content will be assigned. Must not be NULL.
 * @param format The format string specifying how to format the content. Must not be NULL.
 *
 * @param ... Additional arguments to be formatted into the String object.
 */
void string_format(String* str, const char* format, ...) {
    STRING_LOG("[string_format]: Function start.");

    if (str == NULL || format == NULL) {
        STRING_LOG("[string_format]: Error - NULL argument.");
        return;
    }
    /* Format straight into a stack buffer (one vsnprintf for the usual short
       result), with one exact-size retry for a longer one. */
    char small[256];
    va_list args;
    va_start(args, format);
    va_list again;
    va_copy(again, args);
    int length = vsnprintf(small, sizeof small, format, args);
    va_end(args);
    if (length < 0) {
        STRING_LOG("[string_format]: Error - vsnprintf failed.");
        va_end(again);
        return;
    }
    if ((size_t)length < sizeof small) {
        sstr_assign_bytes(str, small, (size_t)length);
    }
    else {
        char* buffer = (char*)malloc((size_t)length + 1);
        if (buffer != NULL) {
            vsnprintf(buffer, (size_t)length + 1, format, again);
            sstr_assign_bytes(str, buffer, (size_t)length);
            free(buffer);
        }
        else {
            STRING_LOG("[string_format]: Error - Memory allocation failed.");
        }
    }
    va_end(again);
}


/**
 * @brief This function creates a new String object that contains the content of the input 
 * String object `str` repeated `count` times. If `str` or its data is NULL, the function 
 * returns NULL and logs an error.
 *
 * @param str The String object to repeat. Must not be NULL.
 * @param count The number of times to repeat the string.
 * 
 * @return A new String object containing the repeated content, or NULL if an error occurs.
 */
String* string_repeat(const String* str, size_t count) {
    STRING_LOG("[string_repeat]: Function start.");

    if (str == NULL || str->dataStr == NULL) {
        return NULL;
    }
    /* size * count wrapped for huge counts: check first */
    if (count != 0 && str->size > (SIZE_MAX - 1) / count) {
        STRING_LOG("[string_repeat]: Error - Result size overflows size_t.");
        return NULL;
    }
    size_t newLength = str->size * count;
    String* result = sstr_new(NULL, 0);
    if (result == NULL || !sstr_reserve_cap(result, newLength + 1)) {
        string_deallocate(result);
        return NULL;
    }
    /* doubling copies instead of `count` small ones; binary-safe */
    char* out = result->dataStr;
    if (newLength != 0) {
        memcpy(out, str->dataStr, str->size);
        size_t done = str->size;
        while (done < newLength) {
            size_t chunk = (done < newLength - done) ? done : newLength - done;
            memcpy(out + done, out, chunk);
            done += chunk;
        }
    }
    out[newLength] = '\0';
    result->size = newLength;
    return result;
}


/**
 * @brief This function joins `count` String objects together into a single String object. 
 * The String objects are passed as variadic arguments. If memory allocation fails, 
 * the function returns NULL and logs an error.
 *
 * @param count The number of String objects to join.
 * @param ... The String objects to be joined.
 * 
 * @return A new String object containing the concatenated content, or NULL if an error occurs.
 */
String* string_join_variadic(size_t count, ...) {
    STRING_LOG("[string_join_variadic]: Function start.");

    va_list args;
    va_start(args, count);
    size_t totalLength = 0;
    for (size_t i = 0; i < count; ++i) {
        const String* s = va_arg(args, String*);
        size_t add = s ? s->size : 0;
        if (add > SIZE_MAX - 1 - totalLength) {
            va_end(args);
            return NULL;
        }
        totalLength += add;
    }
    va_end(args);

    String* result = sstr_new(NULL, 0);
    if (result == NULL || !sstr_reserve_cap(result, totalLength + 1)) {
        string_deallocate(result);
        return NULL;
    }
    char* current = result->dataStr;
    va_start(args, count);
    for (size_t i = 0; i < count; ++i) {
        const String* s = va_arg(args, String*);
        if (s && s->size) {
            memcpy(current, s->dataStr, s->size);
            current += s->size;
        }
    }
    va_end(args);
    *current = '\0';
    result->size = totalLength;
    return result;
}


/**
 * @brief This function removes any characters specified in the `chars` string from the beginning 
 * and the end of the String object `str`. It ensures that the String object, its data, 
 * and the `chars` parameter are not NULL before proceeding with the operation.
 *
 * @param str The String object to trim. Must not be NULL.
 * 
 * @param chars A C-string containing characters to be trimmed from both ends of the String object. Must not be NULL.
 */
void string_trim_characters(String* str, const char* chars) {
    STRING_LOG("[string_trim_characters]: Function start.");

    if (str == NULL) {
        STRING_LOG("[string_trim_characters]: Error - The String object is NULL.");
        return;
    }
    if (str->dataStr == NULL) {
        STRING_LOG("[string_trim_characters]: Error - The dataStr of String object is NULL.");
        return;
    }
    if (chars == NULL) {
        STRING_LOG("[string_trim_characters]: Error - The chars parameter is NULL.");
        return;
    }

    size_t begin = 0;
    size_t stop = str->size;
    bool set[256];
    sstr_make_set(chars, set);
    const unsigned char* s = (const unsigned char*)str->dataStr;

    while (begin < stop && set[s[begin]]) {
        begin++;
    }
    while (stop > begin && set[s[stop - 1]]) {
        stop--;
    }
    size_t newLength = stop - begin;

    memmove(str->dataStr, str->dataStr + begin, newLength);
    str->dataStr[newLength] = '\0';
    str->size = newLength;

    STRING_LOG("[string_trim_characters]: Successfully trimmed the string.");
    STRING_LOG("[string_trim_characters]: Function end.");
}


/**
 * @brief This function randomizes the order of characters in the String object `str`. 
 * The function checks that the String object and its data are not NULL before shuffling.
 *
 * @param str The String object to shuffle. Must not be NULL.
 */
void string_shuffle(String* str) {
    STRING_LOG("[string_shuffle]: Function start.");

    if (str == NULL) {
        STRING_LOG("[string_shuffle]: Error - The String object is NULL.");
        return;
    }
    if (str->dataStr == NULL) {
        STRING_LOG("[string_shuffle]: Error - The dataStr of String object is NULL.");
        return;
    }

    size_t length = str->size;           /* by size: a NUL byte is shuffled too */
    if (length < 2) {
        return;  /* nothing to shuffle (and length - 1 would wrap for "") */
    }

    uint64_t rng = (uint64_t)time(NULL) ^ ((uint64_t)clock() << 32) ^ (uint64_t)(uintptr_t)str ^ ((uint64_t)(uintptr_t)&length << 7);
    for (size_t i = length - 1; i > 0; i--) {
        rng += 0x9E3779B97F4A7C15ULL;
        uint64_t z = rng;
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        z ^= z >> 31;
        size_t j = (size_t)(z % ((uint64_t)i + 1));

        // Swap characters at positions i and j
        char temp = str->dataStr[i];
        str->dataStr[i] = str->dataStr[j];
        str->dataStr[j] = temp;
    }

    STRING_LOG("[string_shuffle]: Successfully shuffled the string.");
    STRING_LOG("[string_shuffle]: Function end.");
}


/**
 * @brief This function converts the String object `str` to title case, where the first letter 
 * of each word is capitalized and all other letters are in lowercase. 
 * Non-alphabetic characters are treated as word boundaries.
 *
 * @param str The String object to convert to title case. Must not be NULL.
 */
void string_to_title(String* str) {
    STRING_LOG("[string_to_title]: Function start.");

    if (str == NULL) {
        STRING_LOG("[string_to_title]: Error - The String object is NULL.");
        return;
    }
    if (str->dataStr == NULL) {
        STRING_LOG("[string_to_title]: Error - The dataStr of String object is NULL.");
        return;
    }

    bool capitalize = true;
    for (size_t i = 0; i < str->size; i++) {
        char c = str->dataStr[i];
        if (!sstr_is_word_char(c)) {
            capitalize = true;
        }
        else if (capitalize) {
            str->dataStr[i] = sstr_ascii_toupper(c);
            capitalize = false;
        }
        else {
            str->dataStr[i] = sstr_ascii_tolower(c);
        }
    }

    STRING_LOG("[string_to_title]: Successfully converted the string to title case.");
    STRING_LOG("[string_to_title]: Function end.");
}


/**
 * @brief This function converts the first character of the String object `str` to uppercase.
 * The function ensures that the String object is not NULL, its data is not NULL, 
 * and that it contains at least one character.
 *
 * @param str The String object to capitalize. Must not be NULL.
 */
void string_to_capitalize(String* str) {
    STRING_LOG("[string_to_capitalize]: Function start.");

    if (str == NULL) {
        STRING_LOG("[string_to_capitalize]: Error - The String object is NULL.");
        return;
    }
    if (str->dataStr == NULL) {
        STRING_LOG("[string_to_capitalize]: Error - The dataStr of String object is NULL.");
        return;
    }
    if (str->size == 0) {
        STRING_LOG("[string_to_capitalize]: Error - The size of String object is zero.");
        return;
    }

    str->dataStr[0] = sstr_ascii_toupper(str->dataStr[0]);

    STRING_LOG("[string_to_capitalize]: Successfully capitalized the first character.");
    STRING_LOG("[string_to_capitalize]: Function end.");
}


/**
 * @brief This function iterates over the characters in the String object `str` and converts 
 * each character to its lowercase equivalent.
 *
 * @param str The String object to casefold. Must not be NULL.
 */
void string_to_casefold(String* str) {
    STRING_LOG("[string_to_casefold]: Function start.");
    
    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_to_casefold]: Error - Invalid string input.");
        return;
    }
    for (size_t i = 0; i < str->size; i++) {
        str->dataStr[i] = sstr_ascii_tolower(str->dataStr[i]);
    }

    STRING_LOG("[string_to_casefold]: Successfully converted the string to casefold.");
}


/**
 * @brief This function determines whether the String object `str` starts with the specified 
 * C-string `substr`.
 * 
 * @param str The String object to check. Must not be NULL.
 * @param substr The substring to look for at the start of the String object. Must not be NULL.
 * 
 * @return true if the String object starts with the specified substring, false otherwise.
 *         Returns false if the String object, its data, or the substring is NULL.
 */
bool string_starts_with(const String* str, const char* substr) {
    STRING_LOG("[string_starts_with]: Function start.");
    
    if (str == NULL) {
        STRING_LOG("[string_starts_with]: Error - The String object is NULL.");
        return false;
    }
    if (str->dataStr == NULL) {
        STRING_LOG("[string_starts_with]: Error - The dataStr of String object is NULL.");
        return false;
    }
    if (substr == NULL) {
        STRING_LOG("[string_starts_with]: Error - The substring is NULL.");
        return false;
    }

    size_t substrLen = strlen(substr);
    if (substrLen > str->size) {
        STRING_LOG("[string_starts_with]: Substring is longer than the main string.");
        return false;
    }

    bool result = memcmp(str->dataStr, substr, substrLen) == 0;
    STRING_LOG("[string_starts_with]: %s", result ? "String starts with the substring." : "String does not start with the substring.");
    return result;
}


/**
 * @brief This function determines whether the String object `str` ends with the specified C-string `substr`.
 * 
 * @param str The String object to check. Must not be NULL.
 * @param substr The substring to look for at the end of the String object. Must not be NULL.
 * 
 * @return true if the String object ends with the specified substring, false otherwise.
 *         Returns false if the String object, its data, or the substring is NULL.
 */
bool string_ends_with(const String* str, const char* substr) {
    STRING_LOG("[string_ends_with]: Function start.");

    if (str == NULL) {
        STRING_LOG("[string_ends_with]: Error - The String object is NULL.");
        return false;
    }
    if (str->dataStr == NULL) {
        STRING_LOG("[string_ends_with]: Error - The dataStr of String object is NULL.");
        return false;
    }
    if (substr == NULL) {
        STRING_LOG("[string_ends_with]: Error - The substring is NULL.");
        return false;
    }

    size_t substrLen = strlen(substr);
    size_t strLen = str->size;
    if (substrLen > strLen) {
        STRING_LOG("[string_ends_with]: Substring is longer than the main string.");
        return false;
    }

    bool result = memcmp(str->dataStr + strLen - substrLen, substr, substrLen) == 0;
    STRING_LOG("[string_ends_with]: %s", result ? "String ends with the substring." : "String does not end with the substring.");
    
    return result;
}


/**
 * @brief This function converts all lowercase characters in the String object `str` to uppercase,
 * and all uppercase characters to lowercase.
 *
 * @param str The String object whose case is to be swapped. Must not be NULL.
 */
void string_swap_case(String* str) {
    STRING_LOG("[string_swap_case]: Function start.");

    if (str == NULL) {
        STRING_LOG("[string_swap_case]: Error - str is NULL.");
        return;
    }
    if (str->dataStr == NULL) {
        STRING_LOG("[string_swap_case]: Error - str->dataStr is NULL.");
        return;
    }

    for (size_t i = 0; i < str->size; i++) {
        char c = str->dataStr[i];
        if (sstr_is_ascii_lower(c)) {
            str->dataStr[i] = sstr_ascii_toupper(c);
        }
        else if (sstr_is_ascii_upper(c)) {
            str->dataStr[i] = sstr_ascii_tolower(c);
        }
    }
    
    STRING_LOG("[string_swap_case]: Successfully swapped the case of all characters.");
}


/**
 * @brief This function converts a multi-byte C-string `str` to a wide-character string (typically Unicode).
 *
 * @param str The C-string to be converted. Must not be NULL.
 * 
 * @return A pointer to the newly allocated wide-character string, or NULL if the input string
 *         is NULL, the conversion fails, or memory allocation fails.
 */
wchar_t* string_to_unicode(const char* str) {
    STRING_LOG("[string_to_unicode]: Function start.");

    if (str == NULL) {
        STRING_LOG("[string_to_unicode]: Error - Input string is NULL.");
        return NULL;
    }

    /* Check for failure BEFORE adding room for the terminator: (size_t)-1 + 1
       wraps to 0 and used to slip past the check. */
    size_t converted = mbstowcs(NULL, str, 0);
    if (converted == (size_t)-1) {
        STRING_LOG("[string_to_unicode]: Error - Conversion failed.");
        return NULL;
    }
    size_t len = converted + 1;

    wchar_t* wstr = (wchar_t*)malloc(len * sizeof(wchar_t));
    if (!wstr) {
        STRING_LOG("[string_to_unicode]: Error - Memory allocation failed.");
        return NULL;
    }

    mbstowcs(wstr, str, len);
    STRING_LOG("[string_to_unicode]: Successfully converted string to unicode.");
    return wstr;
}


/**
 * @brief This function converts a wide-character string `wstr` (typically in Unicode) to a
 * multi-byte C-string and then creates a new String object from the converted string.
 *
 * @param wstr The wide-character string to be converted and used for creating the String object.
 *             Must not be NULL.
 * 
 * @return A pointer to the newly created String object, or NULL if the input string
 *         is NULL, conversion fails, or memory allocation fails.
 */
String* string_from_unicode(const wchar_t* wstr) {
    STRING_LOG("[string_from_unicode]: Function start.");
    
    if (wstr == NULL) {
        STRING_LOG("[string_from_unicode]: Error - Input wide string is NULL.");
        return NULL;
    }

    size_t len = wcstombs(NULL, wstr, 0);
    if (len == (size_t)-1) {
        STRING_LOG("[string_from_unicode]: Error - Conversion failed.");
        return NULL;
    }

    char* str = (char*)malloc(len + 1);  // +1 for null terminator
    if (!str) {
        STRING_LOG("[string_from_unicode]: Error - Memory allocation failed.");
        return NULL;
    }

    wcstombs(str, wstr, len + 1);  // Convert and include the null terminator
    String* stringObj = sstr_new(str, len);
    free(str);  // Free the temporary char* buffer
    
    STRING_LOG("[string_from_unicode]: Successfully created String object from wide string.");
    return stringObj;
}


/**
 * @brief This function takes a variable number of C-strings as input, creates a new String object
 * for each, and returns an array of pointers to these String objects. The array is null-terminated.
 *
 * @param count The number of C-strings passed as arguments.
 * 
 * @return A pointer to an array of String object pointers, or NULL if memory allocation fails
 *         or the count is zero. The array is null-terminated.
 */
String** string_create_from_initializer(size_t count, ...) {
    STRING_LOG("[string_create_from_initializer]: Function start with count = %zu.", count);
    if (count == 0) {
        STRING_LOG("[string_create_from_initializer]: Error - count is zero.");
        return NULL;
    }

    va_list args;
    va_start(args, count);

    String** strings = (String**)malloc(sizeof(String*) * (count + 1));  // +1 for NULL termination
    if (!strings) {
        STRING_LOG("[string_create_from_initializer]: Error - Memory allocation failed for strings array.");
        va_end(args);

        return NULL;
    }

    for (size_t i = 0; i < count; i++) {
        char* str = va_arg(args, char*);
        strings[i] = string_create(str);
        if (!strings[i]) {
            STRING_LOG("[string_create_from_initializer]: Error - string_create failed for string: %s.", str);
            for (size_t j = 0; j < i; j++) {
                string_deallocate(strings[j]);
            }
            free(strings);
            va_end(args);
            return NULL;
        }
    }

    strings[count] = NULL;  // Null-terminate the array
    va_end(args);
    STRING_LOG("[string_create_from_initializer]: Successfully created String array from initializer.");

    return strings;
}


/**
 * @brief This function allocates sufficient memory to hold a copy of the provided
 * C-string `str`, copies the content of `str` into the newly allocated memory, and
 * returns a pointer to this memory. It is the caller's responsibility to free
 * the allocated memory when it is no longer needed.
 *
 * @param str The C-string to be duplicated. Must not be NULL.
 *
 * @return A pointer to the newly allocated string that contains a copy of `str`,
 * or NULL if the input string is NULL or memory allocation fails.
 */
char* string_strdup(const char* str) {
    if (str == NULL) {
        STRING_LOG("[string_strdup]: Error - Parameter 'str' is NULL.");
        return NULL;
    }
    size_t n = strlen(str) + 1;
    char* new_str = (char*)malloc(n);
    if (new_str != NULL) {
        memcpy(new_str, str, n);
    }
    return new_str;
}


/**
 * @brief This function returns the length of the given C-string `str`, excluding the
 * null terminator.
 *
 * @param str The C-string whose length is to be calculated. Must not be NULL.
 * @return The length of the string, or 0 if the input string is NULL.
 */
size_t string_length_cstr(const char* str) {
    STRING_LOG("[string_length_cstr]: Function start.");
    if (!str) {
        STRING_LOG("[string_length_cstr]: Error - str is NULL.");
        return 0;
    }

    size_t length = strlen(str);
    STRING_LOG("[string_length_cstr]: Successfully calculated string length: %zu.", length);

    return length;
}


/**
 * @brief This function returns the number of UTF-8 characters in the given C-string
 * `str`. It correctly handles multi-byte UTF-8 characters.
 *
 * @param str The UTF-8 encoded C-string whose length is to be calculated. Must
 * not be NULL.
 * 
 * @return The number of UTF-8 characters in the string, or 0 if the input string
 * is NULL.
 */
size_t string_length_utf8(const char* str) {
    STRING_LOG("[string_length_utf8]: Function start.");
    if (!str) {
        STRING_LOG("[string_length_utf8]: Error - str is NULL.");
        return 0;
    }

    size_t length = 0;
    STRING_LOG("[string_length_utf8]: Starting to calculate UTF-8 string length.");

    while (*str) {
        if ((*str & 0xC0) != 0x80) {
            length++;
        }
        str++;
    }

    STRING_LOG("[string_length_utf8]: Successfully calculated UTF-8 string length: %zu.", length);
    return length;
}


/**
 * @brief This function checks if the provided C-string is "true" or "false" and
 * returns the corresponding boolean value. The comparison is case-sensitive.
 *
 * @param boolstr The C-string to convert. Should be "true" or "false".
 * 
 * @return `true` if the string is "true", `false` if the string is "false"
 * or if the string is NULL or any other value.
 */
bool string_to_bool_from_cstr(const char* boolstr) {
    STRING_LOG("[string_to_bool_from_cstr]: Function start.");
    if (!boolstr) {
        STRING_LOG("[string_to_bool_from_cstr]: Error - boolstr is NULL.");
        return false;
    }
    if (strcmp(boolstr, "true") == 0) {
        STRING_LOG("[string_to_bool_from_cstr]: Returning true.");
        return true;
    }
    else if (strcmp(boolstr, "false") == 0) {
        STRING_LOG("[string_to_bool_from_cstr]: Returning false.");
        return false;
    }

    STRING_LOG("[string_to_bool_from_cstr]: Invalid input - Returning false.");
    return false;
}


/**
 * @brief This function checks the first byte of a UTF-8 encoded character and returns 
 * the number of bytes that the character occupies. UTF-8 characters can range 
 * from 1 to 4 bytes in length.
 *
 * @param c The first byte of the UTF-8 character.
 * 
 * @return The number of bytes the UTF-8 character occupies: 1, 2, 3, or 4. 
 * Returns 0 if the byte is not a valid UTF-8 start byte.
 */
size_t string_utf8_char_len(char c) {
    STRING_LOG("[string_utf8_char_len]: Function start.");
    
    size_t len = 0;
    if ((c & 0x80) == 0) { 
        len = 1;
    } 
    else if ((c & 0xE0) == 0xC0) {
        len = 2;
    } 
    else if ((c & 0xF0) == 0xE0) {
        len = 3;
    }
    else if ((c & 0xF8) == 0xF0) {
        len = 4;
    } 
    else {
        STRING_LOG("[string_utf8_char_len]: Invalid UTF-8 start byte.");
    }

    STRING_LOG("[string_utf8_char_len]: Returning length %zu.", len);
    return len;
}


/**
 * @brief This function compares two C-strings using the standard `strcmp` function.
 * It returns an integer indicating the relationship between the two strings.
 *
 * @param str1 The first C-string to compare.
 * @param str2 The second C-string to compare.
 * 
 * @return An integer less than, equal to, or greater than zero if `str1` is 
 * found, respectively, to be less than, to match, or be greater than `str2`.
 */
int string_strcmp(const char* str1, const char* str2) {
    STRING_LOG("[string_strcmp]: Function start.");
    /* NULL-safety: strcmp(NULL, ...) is UB and crashes on glibc / MSVC. Treat
     * NULL as ordering-less-than any real string, and (NULL,NULL) == 0. */
    if (!str1 && !str2) {
        STRING_LOG("[string_strcmp]: Both NULL -> 0.");
        return 0;
    }
    if (!str1) {
        STRING_LOG("[string_strcmp]: str1 NULL -> -1.");
        return -1;
    }
    if (!str2) {
        STRING_LOG("[string_strcmp]: str2 NULL -> 1.");
        return 1;
    }
    int result = strcmp(str1, str2);
    STRING_LOG("[string_strcmp]: Comparison result: %d.", result);
    return result;
}


/**
 * @brief Duplicates up to `n` characters from a given string.
 *
 * This function creates a new string by duplicating the first `n` characters
 * from the input string `str`. If the length of `str` is less than `n`, the 
 * entire string is duplicated. The resulting string is null-terminated.
 *
 * @param str The input string to duplicate.
 * @param n The maximum number of characters to duplicate from `str`.
 * 
 * @return A pointer to the newly allocated string that contains the first `n`
 *         characters from `str`. The string is null-terminated. If memory 
 *         allocation fails, the function returns `NULL`.
 *
 * @note The caller is responsible for freeing the memory allocated by this
 *       function using `free()`.
 */
char* string_strndup(const char* str, size_t n) {
    STRING_LOG("[string_strndup]: Function start.");
    /* NULL-safety: strlen(NULL) is UB. */
    if (!str) {
        STRING_LOG("[string_strndup]: Error - str is NULL.");
        return NULL;
    }

    /* Like POSIX strndup, never look past str[n - 1]: the source may be a
       slice of a buffer that is not NUL-terminated (strlen would overread). */
    const char* nul = (const char*)memchr(str, '\0', n);
    size_t len = nul ? (size_t)(nul - str) : n;

    char* result = (char*)malloc(len + 1);
    if (!result) {
        STRING_LOG("[string_strndup]: Error - Memory allocation failed.");
        return NULL;
    }

    result[len] = '\0';
    memcpy(result, str, len);
    STRING_LOG("[string_strndup]: Successfully duplicated %zu characters.", len);

    return result;
}


/**
 * @brief Checks if the given String object is a palindrome.
 *
 * This function verifies whether the input String object is a palindrome. 
 * A palindrome is a string that reads the same forwards and backwards.
 * The function compares characters from the beginning and the end of the string,
 * working towards the center.
 *
 * If the input string is empty, it is considered a palindrome.
 *
 * @param str A pointer to the String object to be checked.
 *
 * @return Returns `true` if the String object is a palindrome, `false` otherwise.
 * 
 * @note If the String object is `NULL`, the function will print an error message 
 * to `stderr` and return `false`.
 */
bool string_is_palindrome(const String* str) {
    STRING_LOG("[string_is_palindrome]: Function start.");
    if (!str) {
        STRING_LOG("[string_is_palindrome]: Error - str is NULL.");
        return false;
    }

    size_t str_size = string_length(str);
    if (str_size == 0) {
        STRING_LOG("[string_is_palindrome]: Empty string - returning true.");
        return true;
    }

    for (size_t i = 0, j = str_size - 1; i < str_size; i++, j--) {
        if (string_at(str, i) != string_at(str, j)) {
            STRING_LOG("[string_is_palindrome]: Mismatch found at index %zu - returning false.", i);
            return false;
        }
    }

    STRING_LOG("[string_is_palindrome]: String is a palindrome - returning true.");
    return true;
}


/**
 * @brief Checks if the given String object contains only alphanumeric characters.
 *
 * This function verifies whether the input String object consists solely of 
 * alphanumeric characters (letters and digits). It returns `true` if all characters 
 * are either alphabetic (`a-z`, `A-Z`) or numeric (`0-9`). If any non-alphanumeric character
 * is found, the function returns `false`.
 *
 * @param str A pointer to the String object to be checked.
 *
 * @return Returns `true` if the String object contains only alphanumeric characters, 
 * `false` otherwise.
 *
 * @note If the String object is `NULL`, the function will print an error message to `stderr` 
 * and return `false`. If the String object is empty, the function also returns `false`.
 */
bool string_is_alnum(const String* str) {
    STRING_LOG("[string_is_alnum]: Function start.");
    if (!str) {
        STRING_LOG("[string_is_alnum]: Error - str is NULL.");
        return false;
    }

    size_t str_size = string_length(str);
    if (str_size == 0) {
        STRING_LOG("[string_is_alnum]: Empty string - returning false.");
        return false;
    }

    for (size_t i = 0; i < str_size; i++) {
        char ch = string_at(str, i);
        if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))) {
            STRING_LOG("[string_is_alnum]: Non-alphanumeric character found at index %zu - returning false.", i);
            return false;
        }
    }

    STRING_LOG("[string_is_alnum]: String is alphanumeric - returning true.");
    return true;
}


/**
 * @brief Checks if the given String object is in title case.
 *
 * A string is considered to be in title case if every word starts with an uppercase letter 
 * and all subsequent characters in the word are lowercase. Non-alphabetic characters such as 
 * punctuation and numbers are allowed and do not affect the title case condition. 
 * 
 * `Words are separated by spaces.`
 * 
 * @param str A pointer to the String object to check.
 * 
 * @return Returns `true` if the string is in title case, `false` otherwise.
 */
bool string_is_title(const String* str) {
    STRING_LOG("[string_is_title]: Function start.");

    if (!str) {
        STRING_LOG("[string_is_title]: Error - String object `str` is NULL.");
        return false;
    }

    size_t str_size = string_length(str);
    if (str_size == 0) {
        STRING_LOG("[string_is_title]: String is empty - returning false.");
        return false;
    }

    bool start_of_word = true;

    for (size_t i = 0; i < str_size; i++) {
        char ch = string_at(str, i);

        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
            if (start_of_word) {
                if (ch >= 'A' && ch <= 'Z') {
                    start_of_word = false;
                } 
                else {
                    STRING_LOG("[string_is_title]: Lowercase letter found at start of word - returning false.");
                    return false;
                }
            } 
            else {
                if (ch >= 'A' && ch <= 'Z') {
                    STRING_LOG("[string_is_title]: Uppercase letter found in middle of word - returning false.");
                    return false;
                }
            }
        } 
        else if (ch == ' ') {
            start_of_word = true; 
        } 
        else {
            start_of_word = false; 
        }
    }

    STRING_LOG("[string_is_title]: String is in title case - returning true.");
    return true;
}


/**
 * @brief Checks if the given String object consists entirely of space characters.
 *
 * This function iterates over the characters in the String object and checks
 * if each character is a space (' '). If any character is not a space, it returns false.
 * If all characters are spaces or the string is empty, it returns true.
 * 
 * @param str Pointer to the String object to check.
 * @return Returns true if the String contains only space characters, otherwise false.
 */
bool string_is_space(const String* str) {
    STRING_LOG("[string_is_space]: Function start.");
    if (!str) {
        STRING_LOG("[string_is_space]: Error - String object `str` is NULL.");
        return false;
    }

    size_t str_size = string_length(str);
    if (str_size == 0) {
        STRING_LOG("[string_is_space]: Empty string - returning false.");
        return false;
    }

    for (size_t i = 0; i < str_size; i++) {
        if (string_at(str, i) != ' ') {
            STRING_LOG("[string_is_space]: Non-space character found at index %zu - returning false.", i);
            return false;
        }
    }

    STRING_LOG("[string_is_space]: String contains only spaces - returning true.");
    return true;
}


/**
 * @brief Checks if all characters in the given String object are printable.
 *
 * A character is considered printable if its ASCII value is greater than 31.
 * Printable characters include letters, digits, punctuation, and space.
 *
 * This function iterates through the String object and verifies that each character
 * is a printable character. If a non-printable character (with an ASCII value 
 * between 0 and 31) is encountered, the function returns false.
 * 
 * If the string is empty, the function returns true.
 *
 * @param str Pointer to the String object to be checked.
 * @return true if all characters are printable or if the string is empty.
 * @return false if the String object contains non-printable characters or is NULL.
 *
 * @note If the String object `str` is NULL, an error message is printed and the function returns false.
 */
bool string_is_printable(const String* str) {
    STRING_LOG("[string_is_printable]: Function start.");
    if (!str) {
        STRING_LOG("[string_is_printable]: Error - String object `str` is NULL.");
        return false;
    }

    size_t str_size = string_length(str);
    if (str_size == 0) {
        STRING_LOG("[string_is_printable]: Empty string - returning true.");
        return true;
    }

    for (size_t i = 0; i < str_size; i++) {
        int ord = string_at(str, i);

        if (ord >= 0 && ord <= 31) {
            STRING_LOG("[string_is_printable]: Non-printable character found at index %zu - returning false.", i);
            return false;
        }
    }

    STRING_LOG("[string_is_printable]: String contains only printable characters - returning true.");
    return true;
}


/**
 * @brief Reserves memory for the string to ensure it has enough capacity.
 *
 * This function checks if the string has enough capacity for the specified size.
 * If the current capacity is insufficient, it reallocates memory to fit the new size.
 *
 * @param str The String object to be resized.
 * @param newCapacity The new capacity size required.
 */
void string_reserve(String *str, size_t newCapacity) {
    if (!str || newCapacity == SIZE_MAX) {
        return;
    }
    /* room for newCapacity characters plus the terminator (std::string::reserve) */
    if (!sstr_reserve_cap(str, newCapacity + 1)) {
        STRING_LOG("[string_reserve]: Memory allocation failed.");
    }
}
