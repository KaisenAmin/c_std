/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class String
*/

#ifndef STD_STRING_H_
#define STD_STRING_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif 

// #define STRING_LOGGING_ENABLE

#ifdef STRING_LOGGING_ENABLE
    #define STRING_LOG(fmt, ...) \
        do { fprintf(stderr, "[STRING LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define STRING_LOG(fmt, ...) do { } while (0)
#endif

extern const char* STRING_ASCII_LETTERS;
extern const char* STRING_ASCII_LOWERCASE;
extern const char* STRING_ASCII_UPPERCASE;
extern const char* STRING_DIGITS;
extern const char* STRING_HEXDIGITS;
extern const char* STRING_WHITESPACE;
extern const char* STRING_PUNCTUATION;

typedef struct String String;
typedef struct MemoryPoolString {
    void *pool;        // Pointer to the memory pool
    size_t poolSize;   // Total size of the pool
    size_t used;       // Memory used so far
} MemoryPoolString;

struct String {
    char* dataStr;
    size_t size;
    size_t capacitySize;
    MemoryPoolString* pool;
};

char string_at(const String* str, size_t index);
float string_to_float(const String *str);
double string_to_double(const String* str);
wchar_t* string_to_unicode(const char* str);

String** string_create_from_initializer(size_t count, ...);
String** string_split(const String *str, const char *delimiter, int *count);
String** string_tokenize(const String* str, const char* delimiters, int* count);

String* string_create(const char* initialStr);
String* string_create_with_pool(size_t size);
String* string_substr(String* str, size_t pos, size_t len);
String* string_join(String **strings, int count, const char *delimiter);
String* string_from_int(int value);
String* string_from_float(float value);
String* string_from_double(double value);
String* string_from_unicode(const wchar_t* wstr);
String* string_to_hex(const String *str);
String* string_from_hex(const String *hexStr);
String* string_base64_encode(const String *input);
String* string_base64_decode(const String* input);
String* string_repeat(const String* str, size_t count);
String* string_join_variadic(size_t count, ...); // Where ... are String* arguments

bool string_is_equal(const String* str1, const String* str2);
bool string_is_less(const String* str1, const String* str2);
bool string_is_greater(const String* str1, const String* str2);
bool string_is_less_or_equal(const String* str1, const String* str2);
bool string_is_greater_or_equal(const String* str1, const String* str2);
bool string_is_not_equal(const String* str1, const String* str2);
bool string_is_alpha(const String* str);
bool string_is_digit(const String* str);
bool string_is_lower(const String* str);
bool string_is_upper(const String* str);
bool string_is_palindrome(const String* str);
bool string_is_alnum(const String* str);
bool string_is_title(const String* str);
bool string_is_space(const String* str);
bool string_is_printable(const String* str);
bool string_empty(const String* str);
bool string_contains(const String* str, const char* substr);
bool string_set_pool_size(String* str, size_t newSize);
bool string_starts_with(const String* str, const char* substr);
bool string_ends_with(const String* str, const char* substr);
bool string_to_bool_from_cstr(const char* boolstr);

int string_compare(const String* str1, const String* str2);
int string_find(const String* str, const char* buffer, size_t pos);
int string_rfind(const String* str, const char* buffer, size_t pos);
int string_find_first_of(const String* str, const char* buffer, size_t pos);
int string_find_last_of(const String* str, const char* buffer, size_t pos);
int string_find_first_not_of(const String* str, const char* buffer, size_t pos);
int string_find_last_not_of(const String* str, const char* buffer, size_t pos);
int string_compare_ignore_case(const String* str1, const String* str2);
int string_to_int(const String *str);
int string_strcmp(const char* str1, const char* str2);

char* string_to_upper(const String* str);
char* string_to_lower(const String* str);
char* string_begin(const String* str);
char* string_end(const String* str);
char* string_rbegin(const String* str);
char* string_rend(const String* str);
char* string_back(const String* str);
char* string_front(const String* str);
char* string_strdup(const char* s);
char* string_from_int_cstr(int value);
char* string_strndup(const char* str, size_t n);

size_t string_length_cstr(const char* str);
size_t string_length_utf8(const char* str);
size_t string_length(const String* str);
size_t string_capacity(const String* str);
size_t string_max_size(const String* str);
size_t string_copy(const String* str, char* buffer, size_t pos, size_t len);
size_t string_count(const String* str, const char* substr);
size_t string_utf8_char_len(char c);

const char* string_data(const String* str);
const char* string_c_str(const String* str);
const char* string_cbegin(const String* str);
const char* string_cend(const String* str);
const char* string_crbegin(const String* str);
const char* string_crend(const String* str);

void string_deallocate(String *str);
void string_concatenate(String *str1, const String *str2);
void string_trim_left(String *str);
void string_trim_right(String *str);
void string_trim(String *str);
void string_replace_all(String *str, const char *oldStr, const char *newStr);
void string_pad_left(String *str, size_t totalLength, char padChar);
void string_pad_right(String *str, size_t totalLength, char padChar);
void string_trim_characters(String* str, const char* chars);
void string_shuffle(String* str);
void string_to_title(String* str);
void string_to_capitalize(String* str);
void string_to_casefold(String* str);
void string_format(String* str, const char* format, ...);
void string_remove(String* str, const char* substr);
void string_remove_range(String* str, size_t startPos, size_t endPos);
void string_swap_case(String* str);
void string_reverse(String* str);
void string_resize(String* str, size_t newSize);
void string_shrink_to_fit(String* str);
void string_append(String* str, const char* strItem);
void string_push_back(String* str, const char chItem); 
void string_assign(String* str, const char* newStr);
void string_insert(String* str, size_t pos, const char* strItem);
void string_erase(String* str, size_t pos, size_t len);
void string_replace(String* str1, const char* oldStr, const char* newStr);
void string_swap(String* str1, String* str2);
void string_pop_back(String* str);
void string_deallocate(String* str);
void string_clear(String* str);
void string_reserve(String *str, size_t newCapacity);

#ifdef __cplusplus
}
#endif

#endif 