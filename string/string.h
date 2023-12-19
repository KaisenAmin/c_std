#ifndef STRING_H_
#define STRING_H_

#include <stdio.h>
#include <stdbool.h>

#define SMALL_STRING_SIZE 16

typedef struct String String;

typedef struct MemoryPoolString
{
    void *pool;        // Pointer to the memory pool
    size_t poolSize;   // Total size of the pool
    size_t used;       // Memory used so far
} MemoryPoolString;

struct String 
{
    char* dataStr;
    size_t size;
    size_t capacitySize;
    MemoryPoolString* pool;
};

String* string_create(const char* initialStr);
String* string_create_with_pool(size_t size);
String* string_substr(String* str, size_t pos, size_t len);
bool string_is_equal(String* str1, String* str2);
bool string_is_less(String* str1, String* str2);
bool string_is_greater(String* str1, String* str2);
bool string_is_less_or_equal(String* str1, String* str2);
bool string_is_greater_or_equal(String* str1, String* str2);
bool string_is_not_equal(String* str1, String* str2);
bool string_empty(String* str);
int string_compare(String* str1, String* str2);
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
size_t string_length(String* str);
size_t string_capacity(String* str);
size_t string_max_size(String* str);
size_t string_copy(String* str, char* buffer, size_t pos, size_t len);
int string_find(String* str, const char* buffer, size_t pos);
int string_rfind(String* str, const char* buffer, size_t pos);
int string_find_first_of(String* str, const char* buffer, size_t pos);
int string_find_last_of(String* str, const char* buffer, size_t pos);
int string_find_first_not_of(String* str, const char* buffer, size_t pos);
int string_find_last_not_of(String* str, const char* buffer, size_t pos);
const char* string_data(String* str);
const char* string_c_str(String* str);
char* string_begin(String* str);
char* string_end(String* str);
char* string_rbegin(String* str);
char* string_rend(String* str);
const char* string_cbegin(String* str);
const char* string_cend(String* str);
const char* string_crbegin(String* str);
const char* string_crend(String* str);
char string_at(String* str, size_t index);
char* string_back(String* str);
char* string_front(String* str);
void string_deallocate(String *str);
bool string_set_pool_size(String* str, size_t newSize);

#endif 