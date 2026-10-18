/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class String
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in std_string.c (file-level convention).
 */

#ifndef STD_STRING_H_
#define STD_STRING_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>   /* uintptr_t, used by the inline string_append */
#include <string.h>   /* memcmp / memcpy / strlen, used by the inline fast paths */

#ifdef __cplusplus
extern "C" {
#endif


/* #define STRING_LOGGING_ENABLE */

#ifdef STRING_LOGGING_ENABLE
    #define STRING_LOG(...) \
        do { fprintf(stderr, "[STRING LOG] "); fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)
#else
    #define STRING_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Character-class constants (Python-style)                           */
/* ------------------------------------------------------------------ */

extern const char* STRING_ASCII_LETTERS;
extern const char* STRING_ASCII_LOWERCASE;
extern const char* STRING_ASCII_UPPERCASE;
extern const char* STRING_DIGITS;
extern const char* STRING_HEXDIGITS;
extern const char* STRING_WHITESPACE;
extern const char* STRING_PUNCTUATION;



typedef struct String String;

/* Kept so that code naming the type still compiles; Strings no longer use
   memory pools (see `pool` below). */
typedef struct MemoryPoolString {
    void*  pool;
    size_t poolSize;
    size_t used;
    void*  spill;
    bool   shared;
} MemoryPoolString;

/* Bytes stored inside the String itself, terminator included: strings up to
   STRING_SSO_CAPACITY - 1 characters need no separate buffer (small-string
   optimisation, like std::string's; libstdc++ keeps 15 characters inline). */
#define STRING_SSO_CAPACITY 32

struct String {
    char*             dataStr;       /* the characters, always NUL-terminated (dataStr[size] == '\0');
                                        either `sso` below or a heap buffer               */
    size_t            size;          /* number of bytes, not counting the terminator     */
    size_t            capacitySize;  /* bytes dataStr can hold, terminator included      */
    MemoryPoolString* pool;          /* unused, always NULL (kept for source compatibility) */
    char              sso[STRING_SSO_CAPACITY];   /* inline buffer for short strings */
};
/* A String may point into itself (dataStr == sso), so never copy a String by
 * value (String b = *a; memcpy(&b, a, ...)): use string_clone, or
 * string_swap to exchange two. */


/* ------------------------------------------------------------------ */
/* Hot paths, inline like std::string's                               */
/* ------------------------------------------------------------------ */
bool          stringbuf_grow_for              (String* str, size_t extra);

/* Number of bytes (0 for NULL). */
static inline size_t string_length(const String* str) {
    return str ? str->size : 0;
}

/* Bytes the buffer holds before it must grow, terminator included (0 for NULL). */
static inline size_t string_capacity(const String* str) {
    return str ? str->capacitySize : 0;
}

/* true for an empty or NULL String. */
static inline bool string_empty(const String* str) {
    return str == NULL || str->size == 0;
}

/* The byte at `index`, or '\0' when out of range / NULL. */
static inline char string_at(const String* str, size_t index) {
    return (str != NULL && index < str->size) ? str->dataStr[index] : '\0';
}

/* Pointer to the first / last byte, or NULL when empty / NULL. */
static inline char* string_front(const String* str) {
    return (str != NULL && str->size != 0) ? str->dataStr : NULL;
}

static inline char* string_back(const String* str) {
    return (str != NULL && str->size != 0) ? str->dataStr + str->size - 1 : NULL;
}

/* The characters (NUL-terminated); string_data gives NULL and string_c_str
   "" for a NULL String. */
static inline const char* string_data(const String* str) {
    return str ? str->dataStr : NULL;
}

static inline const char* string_c_str(const String* str) {
    return (str != NULL && str->dataStr != NULL) ? str->dataStr : "";
}

/* [begin, end) over the bytes (NULL for a NULL String). */
static inline char* string_begin(const String* str) {
    return str ? str->dataStr : NULL;
}

static inline char* string_end(const String* str) {
    return (str != NULL && str->dataStr != NULL) ? str->dataStr + str->size : NULL;
}

static inline const char* string_cbegin(const String* str) {
    return str ? str->dataStr : NULL;
}

static inline const char* string_cend(const String* str) {
    return (str != NULL && str->dataStr != NULL) ? str->dataStr + str->size : NULL;
}

/* Empties the String (the buffer is kept). */
static inline void string_clear(String* str) {
    if (str != NULL) {
        str->size = 0;
        if (str->dataStr != NULL) {
            str->dataStr[0] = '\0';
        }
    }
}

/* Removes the last byte (no-op when empty). */
static inline void string_pop_back(String* str) {
    if (str != NULL && str->size != 0) {
        str->dataStr[--str->size] = '\0';
    }
}

/* Appends one byte; amortised O(1) (the buffer doubles when full). */
static inline void string_push_back(String* str, char chItem) {
    if (str != NULL && (str->size + 1 < str->capacitySize || stringbuf_grow_for(str, 1))) {
        size_t n = str->size;
        str->dataStr[n] = chItem;
        str->dataStr[n + 1] = '\0';
        str->size = n + 1;
    }
}

/* Byte-for-byte equality (embedded NULs included); two NULLs are equal. */
static inline bool string_is_equal(const String* str1, const String* str2) {
    if (str1 == NULL || str2 == NULL) {
        return str1 == str2;
    }
    return str1->size == str2->size && memcmp(str1->dataStr, str2->dataStr, str1->size) == 0;
}

static inline bool string_is_not_equal(const String* str1, const String* str2) {
    return !string_is_equal(str1, str2);
}

/* Lexicographic byte comparison (embedded NULs included; a prefix sorts
   first): -1, 0 or 1. NULL sorts before every String; two NULLs are equal. */
static inline int string_compare(const String* str1, const String* str2) {
    if (str1 == NULL || str2 == NULL) {
        return (str1 == str2) ? 0 : (str1 == NULL) ? -1 : 1;
    }
    size_t n = str1->size < str2->size ? str1->size : str2->size;
    int c = memcmp(str1->dataStr, str2->dataStr, n);
    if (c != 0) {
        return c < 0 ? -1 : 1;
    }
    return (str1->size < str2->size) ? -1 : (str1->size > str2->size) ? 1 : 0;
}

/* Binary-safe append of `length` bytes (out of line; data may point into str). */
void          string_append_n                 (String* str, const char* data, size_t length);

/* Appends the C string `strItem` (it may point into str). When it fits and
   does not alias str, the copy is inline: for a literal the compiler folds the
   strlen and copies with a few moves, like std::string::operator+=. */
static inline void string_append(String* str, const char* strItem) {
    if (str != NULL && strItem != NULL) {
        size_t n = strlen(strItem);
        if (n < str->capacitySize - str->size &&
            (uintptr_t)strItem - (uintptr_t)str->dataStr >= (uintptr_t)str->capacitySize) {
            memcpy(str->dataStr + str->size, strItem, n);
            str->size += n;
            str->dataStr[str->size] = '\0';
        }
        else {
            string_append_n(str, strItem, n);
        }
    }
}

/* Appends the bytes of str2 to str1 (binary-safe; str2 may be str1). */
static inline void string_concatenate(String* str1, const String* str2) {
    if (str1 != NULL && str2 != NULL) {
        size_t n = str2->size;
        if (str1 != str2 && n < str1->capacitySize - str1->size) {
            memcpy(str1->dataStr + str1->size, str2->dataStr, n);
            str1->size += n;
            str1->dataStr[str1->size] = '\0';
        }
        else {
            string_append_n(str1, str2->dataStr, n);
        }
    }
}


/* ------------------------------------------------------------------ */
/* Construction                                                       */
/* ------------------------------------------------------------------ */

String*       string_create                   (const char* initialStr);
String*       string_create_n                 (const char* data, size_t length);
String*       string_clone                    (const String* str);
String*       string_create_with_pool         (size_t size);
String*       string_substr                   (String* str, size_t pos, size_t len);
String*       string_repeat                   (const String* str, size_t count);
String*       string_join                     (String** strings, int count, const char* delimiter);
String*       string_join_variadic            (size_t count, ...);
String**      string_create_from_initializer  (size_t count, ...);


/* ------------------------------------------------------------------ */
/* Destruction                                                        */
/* ------------------------------------------------------------------ */

void          string_deallocate               (String* str);

/* A String that lives on the stack or inside another struct (no allocation
   for the String itself; up to STRING_SSO_CAPACITY - 1 characters none at
   all). Pair string_init with string_destroy, never with string_deallocate. */
bool          string_init                     (String* str, const char* initialStr);
void          string_destroy                  (String* str);


/* ------------------------------------------------------------------ */
/* Conversions to / from primitive types                              */
/* ------------------------------------------------------------------ */

String*       string_from_int                 (int value);
String*       string_from_float               (float value);
String*       string_from_double              (double value);
String*       string_from_unicode             (const wchar_t* wstr);

int           string_to_int                   (const String* str);
float         string_to_float                 (const String* str);
double        string_to_double                (const String* str);
wchar_t*      string_to_unicode               (const char* str);

bool          string_to_bool_from_cstr        (const char* boolstr);
char*         string_from_int_cstr            (int value);


/* ------------------------------------------------------------------ */
/* Hex / Base64                                                       */
/* ------------------------------------------------------------------ */

String*       string_to_hex                   (const String* str);
String*       string_from_hex                 (const String* hexStr);
String*       string_base64_encode            (const String* input);
String*       string_base64_decode            (const String* input);


/* ------------------------------------------------------------------ */
/* Splitting / tokenizing                                             */
/* ------------------------------------------------------------------ */

String**      string_split                    (const String* str, const char* delimiter, int* count);
String**      string_tokenize                 (const String* str, const char* delimiters, int* count);


/* ------------------------------------------------------------------ */
/* Capacity / size                                                    */
/* ------------------------------------------------------------------ */

size_t        string_length_cstr              (const char* str);
size_t        string_length_utf8              (const char* str);
size_t        string_max_size                 (const String* str);
size_t        string_utf8_char_len            (char c);

void          string_resize                   (String* str, size_t newSize);
void          string_reserve                  (String* str, size_t newCapacity);
void          string_shrink_to_fit            (String* str);
bool          string_set_pool_size            (String* str, size_t newSize);


/* ------------------------------------------------------------------ */
/* Element access (string_at, front, back, data, c_str: inline above)  */
/* ------------------------------------------------------------------ */

size_t        string_copy                     (const String* str, char* buffer, size_t pos, size_t len);


/* ------------------------------------------------------------------ */
/* Iterators — mutable (string_begin / string_end: inline above)      */
/* ------------------------------------------------------------------ */

char*         string_rbegin                   (const String* str);
char*         string_rend                     (const String* str);


/* ------------------------------------------------------------------ */
/* Iterators — const (string_cbegin / string_cend: inline above)      */
/* ------------------------------------------------------------------ */

const char*   string_crbegin                  (const String* str);
const char*   string_crend                    (const String* str);


/* ------------------------------------------------------------------ */
/* Modifiers — append / assign / insert / erase / replace             */
/* ------------------------------------------------------------------ */

void          string_assign                   (String* str, const char* newStr);
void          string_append_n                 (String* str, const char* data, size_t length);
void          string_insert                   (String* str, size_t pos, const char* strItem);
void          string_erase                    (String* str, size_t pos, size_t len);
void          string_replace                  (String* str1, const char* oldStr, const char* newStr);
void          string_replace_all              (String* str, const char* oldStr, const char* newStr);
void          string_remove                   (String* str, const char* substr);
void          string_remove_range             (String* str, size_t startPos, size_t endPos);
void          string_swap                     (String* str1, String* str2);
#if defined(__MINGW_PRINTF_FORMAT)
#  define STRING_PRINTF_FORMAT(f, a) __attribute__((format(__MINGW_PRINTF_FORMAT, f, a)))
#elif defined(__GNUC__) || defined(__clang__)
#  define STRING_PRINTF_FORMAT(f, a) __attribute__((format(printf, f, a)))
#else
#  define STRING_PRINTF_FORMAT(f, a)
#endif
void          string_format                   (String* str, const char* format, ...) STRING_PRINTF_FORMAT(2, 3);


/* ------------------------------------------------------------------ */
/* Modifiers — case / trim / pad / shape                              */
/* ------------------------------------------------------------------ */

char*         string_to_upper                 (const String* str);
char*         string_to_lower                 (const String* str);
void          string_to_title                 (String* str);
void          string_to_capitalize            (String* str);
void          string_to_casefold              (String* str);
void          string_swap_case                (String* str);
void          string_reverse                  (String* str);
void          string_shuffle                  (String* str);
void          string_trim                     (String* str);
void          string_trim_left                (String* str);
void          string_trim_right               (String* str);
void          string_trim_characters          (String* str, const char* chars);
void          string_pad_left                 (String* str, size_t totalLength, char padChar);
void          string_pad_right                (String* str, size_t totalLength, char padChar);


/* ------------------------------------------------------------------ */
/* Searching                                                          */
/* ------------------------------------------------------------------ */

int           string_find                     (const String* str, const char* buffer, size_t pos);
int           string_rfind                    (const String* str, const char* buffer, size_t pos);
int           string_find_first_of            (const String* str, const char* buffer, size_t pos);
int           string_find_last_of             (const String* str, const char* buffer, size_t pos);
int           string_find_first_not_of        (const String* str, const char* buffer, size_t pos);
int           string_find_last_not_of         (const String* str, const char* buffer, size_t pos);
size_t        string_count                    (const String* str, const char* substr);
bool          string_contains                 (const String* str, const char* substr);
bool          string_starts_with              (const String* str, const char* substr);
bool          string_ends_with                (const String* str, const char* substr);


/* ------------------------------------------------------------------ */
/* Predicates                                                         */
/* ------------------------------------------------------------------ */

bool          string_is_alpha                 (const String* str);
bool          string_is_digit                 (const String* str);
bool          string_is_alnum                 (const String* str);
bool          string_is_lower                 (const String* str);
bool          string_is_upper                 (const String* str);
bool          string_is_title                 (const String* str);
bool          string_is_space                 (const String* str);
bool          string_is_printable             (const String* str);
bool          string_is_palindrome            (const String* str);


/* ------------------------------------------------------------------ */
/* Comparison                                                         */
/* ------------------------------------------------------------------ */

int           string_compare_ignore_case      (const String* str1, const String* str2);
int           string_strcmp                   (const char* str1, const char* str2);
bool          string_is_less                  (const String* str1, const String* str2);
bool          string_is_less_or_equal         (const String* str1, const String* str2);
bool          string_is_greater               (const String* str1, const String* str2);
bool          string_is_greater_or_equal      (const String* str1, const String* str2);


/* ------------------------------------------------------------------ */
/* C-string helpers (not String*)                                     */
/* ------------------------------------------------------------------ */

char*         string_strdup                   (const char* s);
char*         string_strndup                  (const char* str, size_t n);


#ifdef __cplusplus
}
#endif

#endif 
