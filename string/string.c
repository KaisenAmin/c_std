/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class String
*/


#include "string.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include <wchar.h>

const char* STRING_ASCII_LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* STRING_ASCII_LOWERCASE = "abcdefghijklmnopqrstuvwxyz";
const char* STRING_ASCII_UPPERCASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* STRING_DIGITS = "0123456789";
const char* STRING_HEXDIGITS = "0123456789abcdefABCDEF";
const char* STRING_WHITESPACE = " \t\n\r\f\v";
const char* STRING_PUNCTUATION = "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~";


MemoryPoolString* global_pool = NULL;
static MemoryPoolString *memory_pool_create(size_t size);
static void *memory_pool_allocate(MemoryPoolString *pool, size_t size);
static void memory_pool_destroy(MemoryPoolString *pool);
bool memoryPoolCreated = false;

static const char *base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static void init_global_memory_pool(size_t size) {
    if (global_pool == NULL) {
        global_pool = memory_pool_create(size);
        memoryPoolCreated = true;
    }   
}

static void destroy_global_memory_pool() {
    if (global_pool != NULL && memoryPoolCreated) {
        memory_pool_destroy(global_pool);
        global_pool = NULL;
    }
}

static MemoryPoolString *memory_pool_create(size_t size) {
    MemoryPoolString *pool = malloc(sizeof(MemoryPoolString));
    if (pool) {
        pool->pool = malloc(size);
        if (!pool->pool) {
            free(pool);
            return NULL;
        }

        pool->poolSize = size;
        pool->used = 0;
    }
    return pool;
}

static void *memory_pool_allocate(MemoryPoolString *pool, size_t size) {
    if (pool == NULL) {
        fmt_fprintf(stderr, "Error: Memory pool is NULL in memory_pool_allocate.\n");
        return NULL;
    }
    if (pool->used + size > pool->poolSize) {
        fmt_fprintf(stderr, "Error: Memory pool out of memory in memory_pool_allocate.\n");
        return NULL; // Pool is out of memory
    }

    void *mem = (char *)pool->pool + pool->used;
    pool->used += size;

    return mem;
}

static void memory_pool_destroy(MemoryPoolString *pool) {
    if (pool == NULL) {
        fmt_fprintf(stderr, "Warning: Attempt to destroy a NULL memory pool in memory_pool_destroy.\n");
        return;
    }
    free(pool->pool);
    free(pool);
}

String* string_create(const char* initialStr) {
    String* str = (String*)malloc(sizeof(String));
    if (!str) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for String object in string_create.\n");
        exit(-1);
    }

    size_t initialSize = initialStr ? strlen(initialStr) : 0;
    str->size = initialSize;
    str->capacitySize = 32 + initialSize; // +1 for null terminator

    // Initialize memory pool for strings with a smaller size
    size_t initialPoolSize = 1000000; // 1KB
    str->pool = memory_pool_create(initialPoolSize);
    if (!str->pool) {
        fmt_fprintf(stderr, "Error: Memory pool creation failed in string_create.\n");
        free(str);
        exit(-1);
    }

    str->dataStr = memory_pool_allocate(str->pool, str->capacitySize);
    if (!str->dataStr) {
        fmt_fprintf(stderr, "Error: Memory pool allocation failed in string_create.\n");
        memory_pool_destroy(str->pool);
        free(str);
        exit(-1);
    }

    if (initialStr) {
        strcpy(str->dataStr, initialStr);
    }
    return str;
}

String* string_create_with_pool(size_t size) {
    static int counter = 0;

    if (!counter) {
        init_global_memory_pool(size);
        counter++;
    }
    // Ensure global memory pool is initialized
    if (global_pool == NULL) {
        fmt_fprintf(stderr, "Error: Failed to initialize global memory pool in string_create_with_pool.\n");
        exit(-1);  // Consider handling the error without exiting
    }

    String* str = (String*)malloc(sizeof(String));
    if (!str) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for String object in string_create_with_pool.\n");
        exit(-1);
    }

    str->size = 0;
    str->capacitySize = 1;
    str->dataStr = NULL; // Data is not allocated yet (lazy allocation)
    str->pool = global_pool; // Use the global pool

    return str;
}

String* string_substr(String* str, size_t pos, size_t len) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_substr.\n");
        return NULL;
    }
    if (pos >= str->size) {
        fmt_fprintf(stderr, "Error: Position out of bounds in string_substr.\n");
        return NULL;
    }

    // Adjust len if it goes beyond the end of the string
    if (pos + len > str->size) { 
        len = str->size - pos;
    }

    String* substr = string_create(NULL); // Allocate memory for the substring
    if (substr == NULL) { 
        fmt_fprintf(stderr, "Error: Memory allocation failed for substring in string_substr.\n");
        return NULL;
    }

    substr->size = len;
    substr->capacitySize = len + 1;
    substr->dataStr = (char*)malloc(substr->capacitySize * sizeof(char));

    if (substr->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for dataStr in substring in string_substr.\n");
        free(substr);
        return NULL;
    }

    strncpy(substr->dataStr, str->dataStr + pos, len); // Copy the substring
    substr->dataStr[len] = '\0';  // Null terminate the substring

    return substr;
}

bool string_empty(String* str) {
    return (str == NULL) ? true : (str->size == 0);
}

bool string_contains(String* str, const char* substr) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_contains.\n");
        return false;
    }
    if (substr == NULL) {
        fmt_fprintf(stderr, "Error: The substring is NULL in string_contains.\n");
        return false;
    }
    return strstr(str->dataStr, substr) != NULL;
}

int string_compare(const String* str1, const String* str2) {
    if (str1 == NULL || str2 == NULL) {
        if (str1 == str2) {
            return 0;  // Both are NULL, considered equal
        }
        fmt_fprintf(stderr, "Error: One or both String objects are NULL in string_compare.\n");
        return (str1 == NULL) ? -1 : 1;  // NULL is considered less than non-NULL
    }
    return strcmp(str1->dataStr, str2->dataStr);
}

bool string_is_equal(String* str1, String* str2) {
    return string_compare(str1, str2) == 0;
}

bool string_is_less(String* str1, String* str2) {
    return string_compare(str1, str2) < 0;
}

bool string_is_greater(String* str1, String* str2) {
    return string_compare(str1, str2) > 0;
}

bool string_is_less_or_equal(String* str1, String* str2) {
    return string_compare(str1, str2) <= 0;
}

bool string_is_greater_or_equal(String* str1, String* str2) {
    return string_compare(str1, str2) >= 0;
}

bool string_is_not_equal(String* str1, String* str2) {
    return string_compare(str1, str2) != 0;
}

bool string_is_alpha(String* str) {
    if (str != NULL){
        for (size_t index = 0; index < str->size; index++){
            if (!(str->dataStr[index] >= 'a' && str->dataStr[index] <= 'z') && 
                !(str->dataStr[index] >= 'A' && str->dataStr[index] <= 'Z')) {
                return false;
            }
        }
        return true;
    }
    return false;
}

bool string_is_digit(String* str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_is_digit.\n");
        return false;
    }
    for (size_t index = 0; index < str->size; index++) {
        if (!(str->dataStr[index] >= '0' && str->dataStr[index] <= '9')) {
            return false;
        }
    }       
    return true;
}

bool string_is_upper(String* str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_is_upper.\n");
        return false;
    }
    for (size_t index = 0; index < str->size; index++) {
        if (str->dataStr[index] >= 'a' && str->dataStr[index] <= 'z') {
            return false;
        }
    }
    return true;
}

bool string_is_lower(String* str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_is_lower.\n");
        return false;
    }
    for (size_t index = 0; index < str->size; index++) {
        if (str->dataStr[index] >= 'A' && str->dataStr[index] <= 'Z') {
            return false;
        }
    }
    return true;
}

void string_reverse(String* str) {
    if (str != NULL && str->dataStr != NULL) {
        char* reverse = (char*) malloc(sizeof(char) * (str->size + 1));
        if (!reverse) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in string_reverse.\n");
            return;
        }

        for (int index = str->size - 1, j = 0; index >= 0; index--, j++) {
            reverse[j] = str->dataStr[index];
        }
        reverse[str->size] = '\0';
        string_replace(str, string_c_str(str), reverse);
        
        free(reverse);
    }
    else {
        fmt_fprintf(stderr, "Error: The String object or its data is NULL in string_reverse.\n");
        return;
    }
}

void string_resize(String *str, size_t newSize) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_resize.\n");
        return;
    }
    if (newSize < str->size) {
        str->size = newSize;
        str->dataStr[newSize] = '\0';
    } 
    else if (newSize > str->size) {
        if (newSize >= str->capacitySize) {
            size_t newCapacity = newSize + 1;
            char *newData = memory_pool_allocate(str->pool, newCapacity);

            if (!newData) {
                fmt_fprintf(stderr, "Error: Memory allocation failed in string_resize.\n");
                return;
            }
            memcpy(newData, str->dataStr, str->size);
            str->dataStr = newData;
            str->capacitySize = newCapacity;
        }

        memset(str->dataStr + str->size, '\0', newSize - str->size);
        str->size = newSize;
    }
}

void string_shrink_to_fit(String *str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_shrink_to_fit.\n");
        return;
    }
    if (str->size + 1 == str->capacitySize) {
        return; // No need to shrink if already at optimal size
    }
    // Check if the string is using the memory pool
    if (str->dataStr != NULL) {
        // Allocate new space from the memory pool
        size_t newCapacity = str->size + 1; // +1 for null terminator
        char *newData = memory_pool_allocate(str->pool, newCapacity);

        if (newData == NULL) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in string_shrink_to_fit.\n");
            return;
        }
        // Copy existing data to the new space
        memcpy(newData, str->dataStr, str->size);
        newData[str->size] = '\0'; // Null-terminate the string

        // Update the string's metadata
        str->dataStr = newData;
        str->capacitySize = newCapacity;
    }
}

void string_append(String *str, const char *strItem) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_append.\n");
        return;
    }
    if (strItem == NULL) {
        fmt_fprintf(stderr, "Error: The strItem is NULL in string_append.\n");
        return;
    }
    size_t strItemLength = strlen(strItem);
    if (strItemLength == 0) { 
        return;
    }

    if (str->size + strItemLength >= str->capacitySize) {
        size_t newCapacity = str->size + strItemLength + 1;
        char *newData = memory_pool_allocate(str->pool, newCapacity);

        if (!newData) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in string_append.\n");
            return;
        }

        memcpy(newData, str->dataStr, str->size);
        str->dataStr = newData;
        str->capacitySize = newCapacity;
    }

    strcpy(str->dataStr + str->size, strItem);
    str->size += strItemLength;
}

void string_push_back(String* str, char chItem) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_push_back.\n");
        return;
    }
    if (str->size + 1 >= str->capacitySize) {
        // static int counter = 0;
        size_t newCapacity = str->capacitySize * 2;
        char* newData = memory_pool_allocate(str->pool, newCapacity);  // Allocate new space from the memory pool
        
        if (!newData) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in string_push_back.\n");
            return;
        }

        // Copy existing string to the new space
        if (str->dataStr) { 
            memcpy(newData, str->dataStr, str->size);
        }
        str->dataStr = newData;
        str->capacitySize = newCapacity;
    }
    // Append the character
    str->dataStr[str->size] = chItem;
    str->size++;
    str->dataStr[str->size] = '\0'; // Null-terminate the string
}

void string_assign(String *str, const char *newStr) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_assign.\n");
        return;
    }
    if (newStr == NULL) {
        fmt_fprintf(stderr, "Error: The newStr is NULL in string_assign.\n");
        return;
    }

    size_t newStrLength = strlen(newStr);
    if (newStrLength + 1 > str->capacitySize) {
        char *newData = memory_pool_allocate(str->pool, newStrLength + 1);
        if (!newData) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in string_assign.\n");
            return;
        }


        str->dataStr = newData;
        str->capacitySize = newStrLength + 1;
    }

    strcpy(str->dataStr, newStr);
    str->size = newStrLength;
}

void string_insert(String *str, size_t pos, const char *strItem) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_insert.\n");
        return;
    }
    if (strItem == NULL) {
        fmt_fprintf(stderr, "Error: The strItem is NULL in string_insert.\n");
        return;
    }
    if (pos > str->size) {
        fmt_fprintf(stderr, "Error: Position out of bounds in string_insert.\n");
        return;
    }

    size_t strItemLength = strlen(strItem);
    size_t newTotalLength = str->size + strItemLength;

    if (newTotalLength + 1 > str->capacitySize) {
        size_t newCapacity = newTotalLength + 1;
        char *newData = memory_pool_allocate(str->pool, newCapacity);
        if (!newData) {
            fmt_fprintf(stderr, "Error: Memory allocation failed in string_insert.\n");
            return;
        }

        memcpy(newData, str->dataStr, pos);
        memcpy(newData + pos + strItemLength, str->dataStr + pos, str->size - pos);
        str->dataStr = newData;
        str->capacitySize = newCapacity;
    } 
    else { 
        memmove(str->dataStr + pos + strItemLength, str->dataStr + pos, str->size - pos);
    }

    memcpy(str->dataStr + pos, strItem, strItemLength);
    str->size = newTotalLength;
}

void string_erase(String *str, size_t pos, size_t len) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_erase.\n");
        return;
    }
    if (pos >= str->size) {
        fmt_fprintf(stderr, "Error: Position out of bounds in string_erase.\n");
        return;
    }
    if (pos + len > str->size) { 
        len = str->size - pos;  // Adjust len to not go beyond the string end
    }

    memmove(str->dataStr + pos, str->dataStr + pos + len, str->size - pos - len + 1);
    str->size -= len;
}

void string_replace(String *str1, const char *oldStr, const char *newStr) {
    if (str1 == NULL) {
        fmt_fprintf(stderr, "Error: The String object (str1) is NULL in string_replace.\n");
        return;
    }
    if (oldStr == NULL) {
        fmt_fprintf(stderr, "Error: The oldStr is NULL in string_replace.\n");
        return;
    }
    if (newStr == NULL) {
        fmt_fprintf(stderr, "Error: The newStr is NULL in string_replace.\n");
        return;
    }

    char *position = strstr(str1->dataStr, oldStr);
    if (position == NULL) { 
        fmt_fprintf(stderr, "Warning: oldStr not found in str1 in string_replace.\n");
        return;  // oldStr not found in str1
    }

    size_t oldLen = strlen(oldStr);
    size_t newLen = strlen(newStr);
    size_t tailLen = strlen(position + oldLen);
    size_t newSize = (position - str1->dataStr) + newLen + tailLen;

    if (newSize + 1 > str1->capacitySize) {
        size_t newCapacity = newSize + 1;
        char *newData = memory_pool_allocate(str1->pool, newCapacity);
        if (!newData) {
            return;  // Handle allocation error
        }

        memcpy(newData, str1->dataStr, position - str1->dataStr);
        memcpy(newData + (position - str1->dataStr) + newLen, position + oldLen, tailLen);
        str1->dataStr = newData;
        str1->capacitySize = newCapacity;
    } 
    else {
        memmove(position + newLen, position + oldLen, tailLen);
    }
    memcpy(position, newStr, newLen);
    str1->size = newSize;
}

void string_swap(String *str1, String *str2) {
    if (str1 == NULL || str2 == NULL) {
        fmt_fprintf(stderr, "Error: One or both String objects are NULL in string_swap.\n");
        return;
    }

    String temp = *str1;
    *str1 = *str2;
    *str2 = temp;
}

void string_pop_back(String *str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_pop_back.\n");
        return;
    }
    if (str->size == 0) {
        fmt_fprintf(stderr, "Warning: Attempt to pop back from an empty string in string_pop_back.\n");
        return;
    }

    str->dataStr[str->size - 1] = '\0';
    str->size--;
}

void string_deallocate(String *str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Warning: Attempt to deallocate a NULL String object in string_deallocate.\n");
        return;
    }
    // Destroy the memory pool associated with the string
    if (str->pool != NULL) {
        memory_pool_destroy(str->pool);
        str->pool = NULL;
    }
    // Since dataStr is managed by the memory pool, no separate free call is needed for it
    free(str);
    if (memoryPoolCreated) {
        destroy_global_memory_pool();
    }
}

char string_at(String* str, size_t index) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_at.\n");
        return '\0';  // Return a default character
    }
    if (index >= str->size) {
        fmt_fprintf(stderr, "Error: Index out of range in string_at.\n");
        return '\0';  // Return a default character
    }
    return str->dataStr[index]; // (const char)
}

char* string_back(String *str) {
    if (str == NULL || str->size == 0) { 
        return NULL; 
    }
    return &str->dataStr[str->size - 1];
}

char* string_front(String *str) {
    if (str == NULL || str->size == 0) { 
        return NULL;  
    }
    return &str->dataStr[0];
}

size_t string_length(String* str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_length.\n");
        return 0;
    }
    return str->size;
}

size_t string_capacity(String* str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_capacity.\n");
        return 0;
    }
    return str->capacitySize;
}

size_t string_max_size(String* str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_max_size.\n");
        return 0;  // Or a special value indicating error
    }
    return (size_t)-1;  // You may want to define a more realistic maximum size
}


size_t string_copy(String *str, char *buffer, size_t pos, size_t len) {
    if (str == NULL || str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The String object or its data is NULL in string_copy.\n");
        return 0;
    }
    if (buffer == NULL) {
        fmt_fprintf(stderr, "Error: The buffer is NULL in string_copy.\n");
        return 0;
    }
    if (pos >= str->size) {
        fmt_fprintf(stderr, "Error: Position out of bounds in string_copy.\n");
        return 0;
    }

    size_t copyLen = len;
    if (pos + len > str->size || len == 0) { 
        copyLen = str->size - pos;  // Adjust copy length if it goes beyond the string end
    }

    strncpy(buffer, str->dataStr + pos, copyLen);
    buffer[copyLen] = '\0';  

    return copyLen;  // Return the number of characters copied
}

int string_find(String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The String object or its data is NULL in string_find.\n");
        return -1;
    }
    if (buffer == NULL) {
        fmt_fprintf(stderr, "Error: The buffer is NULL in string_find.\n");
        return -1;
    }
    if (pos >= str->size) {
        fmt_fprintf(stderr, "Error: Position out of bounds in string_find.\n");
        return -1;
    }

    const char *found = strstr(str->dataStr + pos, buffer);
    if (found == NULL) { 
        return -1;  // Substring not found
    }
    return (int)(found - str->dataStr);  // Return the position of the substring
}

int string_rfind(String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The String object or its data is NULL in string_rfind.\n");
        return -1;
    }
    if (buffer == NULL) {
        fmt_fprintf(stderr, "Error: The buffer is NULL in string_rfind.\n");
        return -1;
    }

    size_t bufferLen = strlen(buffer);
    if (bufferLen == 0) {
        fmt_fprintf(stderr, "Error: The buffer is empty in string_rfind.\n");
        return -1;
    }
    if (pos < bufferLen - 1) {
        fmt_fprintf(stderr, "Error: Position is too small in string_rfind.\n");
        return -1;
    }

    pos = (pos < str->size) ? pos : str->size - 1;  // Adjust pos to be within bounds
    for (int i = (int)pos; i >= 0; --i) {
        if (strncmp(str->dataStr + i, buffer, bufferLen) == 0) { 
            return i;  // Found the substring
        }
    }
    return -1;  // Substring not found
}

int string_find_first_of(String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The String object or its data is NULL in string_find_first_of.\n");
        return -1;
    }
    if (buffer == NULL) {
        fmt_fprintf(stderr, "Error: The buffer is NULL in string_find_first_of.\n");
        return -1;
    }
    if (pos >= str->size) {
        fmt_fprintf(stderr, "Error: Position out of bounds in string_find_first_of.\n");
        return -1;
    }

    const char *found = strstr(str->dataStr + pos, buffer);
    if (found != NULL) {
        return (int)(found - str->dataStr);
    }
    return -1;  // Buffer string not found
}

int string_find_last_of(String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The String object or its data is NULL in string_find_last_of.\n");
        return -1;
    }
    if (buffer == NULL) {
        fmt_fprintf(stderr, "Error: The buffer is NULL in string_find_last_of.\n");
        return -1;
    }
    if (pos >= str->size) {
        fmt_fprintf(stderr, "Error: Position out of bounds in string_find_last_of.\n");
        return -1;
    }

    int lastFound = -1;
    const char *currentFound = strstr(str->dataStr, buffer);

    while (currentFound != NULL && (size_t)(currentFound - str->dataStr) <= pos) {
        lastFound = (int)(currentFound - str->dataStr);
        currentFound = strstr(currentFound + 1, buffer);
    }
    return lastFound;
}

int string_find_first_not_of(String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The String object or its data is NULL in string_find_first_not_of.\n");
        return -1;
    }
    if (buffer == NULL) {
        fmt_fprintf(stderr, "Error: The buffer is NULL in string_find_first_not_of.\n");
        return -1;
    }
    if (pos >= str->size) {
        fmt_fprintf(stderr, "Error: Position out of bounds in string_find_first_not_of.\n");
        return -1;
    }

    size_t bufferLen = strlen(buffer);
    if (bufferLen == 0) {
        return (int)pos;  // If buffer is empty, return pos
    }

    for (size_t i = pos; i <= str->size - bufferLen; ++i) { 
        if (strncmp(str->dataStr + i, buffer, bufferLen) != 0) { 
            return (int)i;
        }
    }
    return -1;  // No non-matching position found
}

int string_find_last_not_of(String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The String object or its data is NULL in string_find_last_not_of.\n");
        return -1;
    }
    if (buffer == NULL) {
        fmt_fprintf(stderr, "Error: The buffer is NULL in string_find_last_not_of.\n");
        return -1;
    }

    size_t bufferLen = strlen(buffer);
    if (bufferLen == 0) {
        fmt_fprintf(stderr, "Error: The buffer is empty in string_find_last_not_of.\n");
        return -1;
    }
    if (pos < bufferLen - 1) {
        fmt_fprintf(stderr, "Error: Position is too small in string_find_last_not_of.\n");
        return -1;
    }

    pos = (pos < str->size - bufferLen) ? pos : str->size - bufferLen;
    for (int i = (int)pos; i >= 0; --i) {
        if (strncmp(str->dataStr + i, buffer, bufferLen) != 0) { 
            return i;
        }
    }
    return -1;
}

const char *string_data(String *str) {
    if (str == NULL || str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: Invalid input or uninitialized String in string_data function.\n");
        return NULL;
    }
    return str->dataStr;
}

const char *string_c_str(const String *str) {
    if (str == NULL) { 
        fmt_fprintf(stderr, "Error: Invalid input in string_c_str function.\n");
        return "";  // Return empty string for null String
    }
    if (str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: Uninitialized String in string_c_str function.\n");
        return "";  // Return empty string for uninitialized String
    }
    return str->dataStr;
}

char *string_begin(String *str) {
    if (str == NULL) { 
        fmt_fprintf(stderr, "Error: Invalid input in string_begin function.\n");
        return "";  // Return empty string for null String
    }
    if (str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: Uninitialized String in string_begin function.\n");
        return "";  // Return empty string for uninitialized String
    }
    return str->dataStr;  // The beginning of the string
}

char *string_end(String *str) {
    if (str == NULL || str->dataStr == NULL) { 
        fmt_fprintf(stderr, "Error: Invalid input or uninitialized String in string_end function.\n");
        return NULL;  // Return NULL for null or uninitialized String
    }
    return str->dataStr + str->size;  // The end of the string
}

char *string_rbegin(String *str) {
    if (str == NULL || str->dataStr == NULL || str->size == 0) {
        fmt_fprintf(stderr, "Error: Invalid input, uninitialized, or empty String in string_rbegin function.\n");
        return NULL; 
    }
    return str->dataStr + str->size - 1;
}

char *string_rend(String *str) {
    if (str == NULL || str->dataStr == NULL) { 
        fmt_fprintf(stderr, "Error: Invalid input or uninitialized String in string_rend function.\n");
        return NULL;
    }
    return str->dataStr - 1; 
}

const char *string_cbegin(String *str) {
    if (str == NULL || str->dataStr == NULL) { 
        fmt_fprintf(stderr, "Error: Invalid input or uninitialized String in string_cbegin function.\n");
        return NULL;  // Return NULL for null or uninitialized String
    }
    return str->dataStr;  // The beginning of the string
}

const char *string_cend(String *str) {
    if (str == NULL || str->dataStr == NULL) { 
        fmt_fprintf(stderr, "Error: Invalid input or uninitialized String in string_cend function.\n");
        return NULL;  // Return NULL for null or uninitialized String
    }
    return str->dataStr + str->size;  // The end of the string
}

const char *string_crbegin(String *str) {
    if (str == NULL || str->dataStr == NULL || str->size == 0) { 
        fmt_fprintf(stderr, "Error: Invalid input, uninitialized, or empty String in string_crbegin function.\n");
        return NULL;  // Return NULL for null, uninitialized, or empty String
    }
    return str->dataStr + str->size - 1;  // Pointer to the last character
}

const char *string_crend(String *str) {
    if (str == NULL || str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: Invalid input or uninitialized String in string_crend function.\n");
        return NULL;  // Return NULL for null or uninitialized String
    }
    return str->dataStr - 1;  // Pointer to one before the first character
}

void string_clear(String* str) {
    if (str != NULL) {
        str->size = 0;  // Reset the size to 0, indicating the string is now empty

        // Set the first character to the null terminator.
        // This ensures that the string is considered empty when accessed.
        if (str->dataStr != NULL) { 
            str->dataStr[0] = '\0';
        }
    }
    fmt_fprintf(stderr, "Info : String object is null no need to clear in string_clear.\n");
}

char* string_to_upper(String* str) {
    if (str != NULL) {
        char* upper = (char*) malloc(sizeof(char) * (str->size + 1));
        if (!upper) {
            fmt_fprintf(stderr, "Error: Failed to allocate memory for string_to_upper function.\n");
            exit(-1);
        }

        for (size_t index = 0; index < str->size; index++) {
            if (isalpha(str->dataStr[index]) && (str->dataStr[index] >= 'a' && str->dataStr[index] <= 'z')) {
                upper[index] = toupper(str->dataStr[index]);
            }
            else { 
                upper[index] = str->dataStr[index];
            }
        }
        upper[str->size] = '\0';
        return upper;
    }

    fmt_fprintf(stderr, "Error: Input 'str' is NULL in string_to_upper function.\n");
    return NULL;
}

char* string_to_lower(String* str) {
    if (str != NULL) {
        char* lower = (char*) malloc(sizeof(char) * (str->size + 1));
        if (!lower) {
            fmt_fprintf(stderr, "Error: Failed to allocate memory for string_to_lower function.\n");
            exit(-1);
        }

        for (size_t index = 0; index < str->size; index++) {
            if (isalpha(str->dataStr[index]) && (str->dataStr[index] >= 'A' && str->dataStr[index] <= 'Z')) {
                lower[index] = tolower(str->dataStr[index]);
            }
            else { 
                lower[index] = str->dataStr[index];
            }
        }
        lower[str->size] = '\0';
        return lower;
    }

    fmt_fprintf(stderr, "Error: Input 'str' is NULL in string_to_lower function.\n");
    return NULL;
}

bool string_set_pool_size(String* str, size_t newSize) {
    if (!str) {
        fmt_fprintf(stderr, "Error: Invalid input - 'str' is NULL in string_set_pool_size.\n");
        return false;
    }
    
    if (newSize == 0) { 
        fmt_fprintf(stderr, "Error: Invalid input - 'newSize' is zero in string_set_pool_size.\n");
        return false;
    }
    // If a memory pool already exists, destroy it first
    if (str->pool) {
        memory_pool_destroy(str->pool);
        str->pool = NULL;
    }

    // Create a new memory pool with the specified size
    str->pool = memory_pool_create(newSize);
    if (!str->pool) { 
        fmt_fprintf(stderr, "Error: Failed to create a new memory pool in string_set_pool_size.\n");
        return false; // Return false if memory pool creation fails
    }
    // If the string already has data, reallocate it in the new pool
    if (str->size > 0 && str->dataStr) {
        char* newData = memory_pool_allocate(str->pool, str->size + 1); // +1 for null terminator
        if (!newData) {
            fmt_fprintf(stderr, "Error: Failed to allocate memory for string data in the new pool in string_set_pool_size.\n");
            memory_pool_destroy(str->pool);
            str->pool = NULL;
            return false; // Return false if allocation fails
        }
        memcpy(newData, str->dataStr, str->size);
        newData[str->size] = '\0';
        str->dataStr = newData;
    }
    return true; // Return true on successful pool resize
}

void string_concatenate(String *str1, const String *str2) {
    if (str1 == NULL) {
        fmt_fprintf(stderr, "Error: Null String object 'str1' in string_concatenate.\n");
        return;
    }
    if (str2 == NULL) {
        fmt_fprintf(stderr, "Error: Null String object 'str2' in string_concatenate.\n");
        return;
    }
    string_append(str1, str2->dataStr);
}

void string_trim_left(String *str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_trim_left.\n");
        return;
    }
    if (str->size == 0) {
        return;
    }

    size_t i = 0;
    while (i < str->size && isspace((unsigned char)str->dataStr[i])) {
        i++;
    }

    if (i > 0) {
        memmove(str->dataStr, str->dataStr + i, str->size - i);
        str->size -= i;
        str->dataStr[str->size] = '\0';
    }
}

void string_trim_right(String *str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_trim_right.\n");
        return;
    }
    if (str->size == 0) {
        return;
    }

    size_t i = str->size;
    while (i > 0 && isspace((unsigned char)str->dataStr[i - 1])) {
        i--;
    }

    if (i < str->size) {
        str->dataStr[i] = '\0';
        str->size = i;
    }
}

void string_trim(String *str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_trim.\n");
        return;
    }

    string_trim_left(str);
    string_trim_right(str);
}

String** string_split(String *str, const char *delimiter, int *count) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_split.\n");
        return NULL;
    }
    if (delimiter == NULL) {
        fmt_fprintf(stderr, "Error: Null delimiter in string_split.\n");
        return NULL;
    }

    size_t num_splits = 0;
    char *temp = string_strdup(str->dataStr);
    if (temp == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in string_split.\n");
        return NULL;
    }

    char *token = strtok(temp, delimiter);

    while (token != NULL) {
        num_splits++;
        token = strtok(NULL, delimiter);
    }
    free(temp);

    if (num_splits == 0) { 
        return NULL;
    }

    String **splits = malloc(sizeof(String*) * num_splits);
    if (splits == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for splits in string_split.\n");
        return NULL;
    }

    temp = string_strdup(str->dataStr);
    if (temp == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in string_split.\n");
        free(splits);
        return NULL;
    }

    token = strtok(temp, delimiter);
    size_t index = 0;

    while (token != NULL && index < num_splits) {
        splits[index] = string_create(token);
        if (splits[index] == NULL) {
            fmt_fprintf(stderr, "Error: Failed to create string in string_split.\n");
            // Free previously allocated strings and array
            for (size_t i = 0; i < index; i++) {
                string_deallocate(splits[i]); // Assuming string_free is defined
            }
            free(splits);
            free(temp);
            return NULL;
        }
        index++;
        token = strtok(NULL, delimiter);
    }
    free(temp);
    *count = num_splits;

    return splits;
}

String* string_join(String **strings, int count, const char *delimiter) {
    if (strings == NULL) {
        fmt_fprintf(stderr, "Error: Null string array in string_join.\n");
        return NULL;
    }
    if (count <= 0) {
        fmt_fprintf(stderr, "Error: Invalid count in string_join.\n");
        return NULL;
    }
    if (delimiter == NULL) {
        fmt_fprintf(stderr, "Error: Null delimiter in string_join.\n");
        return NULL;
    }

    String *result = string_create("");
    if (result == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in string_join.\n");
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        string_append(result, strings[i]->dataStr);
        if (i < count - 1) {
            string_append(result, delimiter);
        }
    }
    return result;
}

void string_replace_all(String *str, const char *oldStr, const char *newStr) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_replace_all.\n");
        return;
    }
    if (oldStr == NULL || newStr == NULL) {
        fmt_fprintf(stderr, "Error: Null substring in string_replace_all.\n");
        return;
    }

    String *temp = string_create("");
    if (temp == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in string_replace_all.\n");
        return;
    }

    char *start = str->dataStr;
    char *end;

    while ((end = strstr(start, oldStr)) != NULL) {
        *end = '\0';
        string_append(temp, start);
        string_append(temp, newStr);
        start = end + strlen(oldStr);
    }

    string_append(temp, start);
    string_assign(str, temp->dataStr);
    string_deallocate(temp);
}

int string_to_int(String *str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_to_int.\n");
        return 0;
    }
    if (string_empty(str)) {
        fmt_fprintf(stderr, "Error: Empty string in string_to_int.\n");
        return 0;
    }
    return atoi(str->dataStr);
}

float string_to_float(String *str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_to_float.\n");
        return 0.0f;
    }
    if (string_empty(str)) {
        fmt_fprintf(stderr, "Error: Empty string in string_to_float.\n");
        return 0.0f;
    }
    return atof(str->dataStr);
}

double string_to_double(String* str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_to_double.\n");
        return 0.0;
    }
    if (string_empty(str)) {
        fmt_fprintf(stderr, "Error: Empty string in string_to_double.\n");
        return 0.0;
    }
    return strtod(str->dataStr, NULL);
}

void string_pad_left(String *str, size_t totalLength, char padChar) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_pad_left.\n");
        return;
    }
    if (str->size >= totalLength) {
        fmt_fprintf(stderr, "Error: Size of String object is bigger or equal that total Length in string_pad_left.\n");
        return;
    }
    size_t padSize = totalLength - str->size;
    size_t newSize = str->size + padSize;
    char *newData = (char *)malloc(newSize + 1); // +1 for null terminator

    if (newData == NULL) {
        fmt_fprintf(stderr, "Error: Failed to allocate memory in string_pad_left.\n");
        return;
    }

    memset(newData, padChar, padSize);
    memcpy(newData + padSize, str->dataStr, str->size);
    newData[newSize] = '\0';

    free(str->dataStr);
    str->dataStr = newData;
    str->size = newSize;
    str->capacitySize = newSize + 1;
}

void string_pad_right(String *str, size_t totalLength, char padChar) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_pad_right.\n");
        return;
    }
    if (str->size >= totalLength) {
        fmt_fprintf(stderr, "Error: Size of String object is bigger or equal that total Length in string_pad_right.\n");
        return;
    }

    size_t padSize = totalLength - str->size;
    size_t newSize = str->size + padSize;
    char *newData = (char *)realloc(str->dataStr, newSize + 1); // +1 for null terminator

    if (newData == NULL) {
        fmt_fprintf(stderr, "Error: Failed to allocate memory in string_pad_right.\n");
        return;
    }

    memset(newData + str->size, padChar, padSize);
    newData[newSize] = '\0';

    str->dataStr = newData;
    str->size = newSize;
    str->capacitySize = newSize + 1;
}

String* string_to_hex(String *str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_to_hex.\n");
        return NULL;
    }
    if (string_empty(str)) {
        return string_create(""); // Return an empty string for an empty input.
    }

    String *hexStr = string_create("");
    if (hexStr == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in string_to_hex.\n");
        return NULL;
    }

    for (size_t i = 0; i < str->size; ++i) {
        char buffer[3];  // Each char can be represented by max 2 hex digits + null terminator
        
        sprintf(buffer, "%02x", (unsigned char)str->dataStr[i]);
        string_append(hexStr, buffer);
    }
    return hexStr;
}

String* string_from_hex(String *hexStr) {
    if (hexStr == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_from_hex.\n");
        return NULL;
    }
    if (string_empty(hexStr) || (hexStr->size % 2) != 0) {
        fmt_fprintf(stderr, "Error: Invalid hex string in string_from_hex.\n");
        return NULL; // Hex string should have an even number of characters
    }

    String *str = string_create("");
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in string_from_hex.\n");
        return NULL;
    }

    for (size_t i = 0; i < hexStr->size; i += 2) {
        char buffer[3] = {hexStr->dataStr[i], hexStr->dataStr[i + 1], '\0'};
        char ch = (char)strtol(buffer, NULL, 16);
        
        string_push_back(str, ch);
    }
    return str;
}

size_t string_count(String* str, const char* substr) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_count.\n");
        return 0;
    }
    if (substr == NULL) {
        fmt_fprintf(stderr, "Error: Null substring in string_count.\n");
        return 0;
    }
    if (str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: Null data string in String object in string_count.\n");
        return 0;
    }
    size_t count = 0;
    const char* temp = str->dataStr;
    const char* found;
    
    while ((found = strstr(temp, substr)) != NULL) {
        count++;
        temp = found + strlen(substr);
    }
    return count;
}

void string_remove(String* str, const char* substr) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_remove.\n");
        return;
    }
    if (substr == NULL) {
        fmt_fprintf(stderr, "Error: Null substring in string_remove.\n");
        return;
    }
    if (str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: Null data string in String object in string_remove.\n");
        return;
    }
    if (strlen(substr) == 0) {
        fmt_fprintf(stderr, "Error: Empty substring in string_remove.\n");
        return;
    }
    size_t len = strlen(substr);
    char* p = str->dataStr;

    while ((p = strstr(p, substr)) != NULL) { 
        memmove(p, p + len, strlen(p + len) + 1);
    }
}

void string_remove_range(String* str, size_t startPos, size_t endPos) {
    if (str == NULL || str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: Null String object in string_remove_range.\n");
        return;
    }
    if (startPos >= endPos || endPos > str->size) {
        fmt_fprintf(stderr, "Error: Invalid range in string_remove_range.\n");
        return;
    }
    size_t length = endPos - startPos;
    
    memmove(str->dataStr + startPos, str->dataStr + endPos, str->size - endPos + 1); // +1 for null terminator
    str->size -= length;
}

String* string_from_int(int value) {
    char buffer[12]; // Enough to hold any 32-bit integer
    sprintf(buffer, "%d", value);

    return string_create(buffer);
}

char* string_from_int_cstr(int value) {
    char buffer[12]; // Enough to hold any 32-bit integer
    sprintf(buffer, "%d", value);

    char* result = malloc(strlen(buffer) + 1); // +1 for null-terminator
    if (result) {
        strcpy(result, buffer);
    }

    return result;
}

String* string_from_float(float value) {
    char buffer[32]; // A general buffer size for a float
    sprintf(buffer, "%f", value);

    return string_create(buffer);
}

String* string_from_double(double value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%f", value);

    return string_create(buffer);
}

String** string_tokenize(String* str, const char* delimiters, int* count) {
    if (str == NULL || delimiters == NULL) {
        fmt_fprintf(stderr, "Error: Invalid input in string_tokenize.\n");
        return NULL;
    }
    size_t num_tokens = 0;
    char* temp_str = string_strdup(str->dataStr); // strdup
    if (temp_str == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in string_tokenize.\n");
        return NULL;
    }
    char* token = strtok(temp_str, delimiters);

    while (token != NULL) {
        num_tokens++;
        token = strtok(NULL, delimiters);
    }

    free(temp_str);

    // Allocate array of String pointers
    String** tokens = malloc(num_tokens * sizeof(String*));
    if (tokens == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for tokens in string_tokenize.\n");
        return NULL;
    }
    // Tokenize again to fill the array
    temp_str = string_strdup(str->dataStr);
    if (temp_str == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in string_tokenize.\n");
        free(tokens);
        return NULL;
    }
    token = strtok(temp_str, delimiters);
    size_t idx = 0;

    while (token != NULL && idx < num_tokens) {
        tokens[idx] = string_create(token);
        if (tokens[idx] == NULL) {
            fmt_fprintf(stderr, "Error: string_create failed in string_tokenize.\n");
            for (size_t i = 0; i < idx; ++i) {
                // Assuming a function to free String* is available
                string_deallocate(tokens[i]);
            }
            free(tokens);
            free(temp_str);
            return NULL;
        }
        idx++;
        token = strtok(NULL, delimiters);
    }
    free(temp_str);
    *count = num_tokens;

    return tokens;
}

int string_compare_ignore_case(String* str1, String* str2) {
    if (str1 == NULL || str2 == NULL) {
        if (str1 == str2) {
            return 0;
        }
        return (str1 == NULL) ? -1 : 1;
    }
    if (str1->dataStr == NULL || str2->dataStr == NULL) {
        if (str1->dataStr == str2->dataStr) {
            return 0; // Both dataStr are NULL, considered equal
        }
        return (str1->dataStr == NULL) ? -1 : 1; // One dataStr is NULL, the other is not
    }
    return strcasecmp(str1->dataStr, str2->dataStr);
}

String* string_base64_encode(const String *input) {
    if (input == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_baes64_encode.\n");
        return NULL;
    }
    if (input->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The dataStr of String object is NULL in string_base64_encode.\n");
        return NULL;
    }
    String *encoded = string_create("");
    int val = 0, valb = -6;
    size_t i;

    for (i = 0; i < input->size; i++) {
        unsigned char c = input->dataStr[i];
        val = (val << 8) + c;
        valb += 8;

        while (valb >= 0) {
            string_push_back(encoded, base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }

    if (valb > -6) {
        string_push_back(encoded, base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }

    while (encoded->size % 4) { 
        string_push_back(encoded, '=');
    }
    return encoded;
}

String* string_base64_decode(const String* encodedStr) {
    if (encodedStr == NULL) {
        fmt_fprintf(stderr, "Error: encodedStr param is null in string_base64_decode\n");
        return NULL;
    }
    if (encodedStr->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: dataStr item of encodedStr object is null in string_base64_decode\n");
        return NULL;
    }
    char* decodedStr = (char*)malloc(encodedStr->size * 3 / 4 + 1); 

    if (decodedStr == NULL) {
        fmt_fprintf(stderr, "Error: Failed to allocate memory for base64 decoding");
        return NULL;
    }

    int val = 0, valb = -8;
    size_t i = 0;
    size_t j = 0;

    for (i = 0; i < encodedStr->size; i++) {
        char c = encodedStr->dataStr[i];
        if (c == '=') 
            break;

        if (c >= 'A' && c <= 'Z') {
            c -= 'A';
        }
        else if (c >= 'a' && c <= 'z') {
            c = c - 'a' + 26;
        }
        else if (c >= '0' && c <= '9') {
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
        val = (val << 6) | c;
        valb += 6;

        if (valb >= 0) {
            decodedStr[j++] = (char)((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    decodedStr[j] = '\0';

    String* decodedStringObject = string_create(decodedStr);
    free(decodedStr); 

    return decodedStringObject;
}

void string_format(String* str, const char* format, ...) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_format.\n");
        return;
    }
    if (format == NULL) {
        fmt_fprintf(stderr, "Error: The format string is NULL in string_format.\n");
        return;
    }
    // Start variadic argument processing
    va_list args;
    va_start(args, format);

    // Calculate the required length of the result string
    int length = vsnprintf(NULL, 0, format, args);
    if (length < 0) {
        fmt_fprintf(stderr, "Error: vsnprintf failed in string_format.\n");
        va_end(args);
        return;
    }

    // Allocate memory for the formatted string
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fmt_fprintf(stderr, "Error: Failed to allocate memory in string_format.\n");
        va_end(args);
        return;
    }

    // Actually format the string
    vsnprintf(buffer, length + 1, format, args);

    // Assign the formatted string to the String object
    // Assuming you have a function like string_assign to replace the string's content
    string_assign(str, buffer);

    // Clean up
    free(buffer);
    va_end(args);
}

String* string_repeat(const String* str, size_t count) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_repeat.\n");
        return NULL;
    }
    if (str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The dataStr of String object is NULL in string_repeat.\n");
        return NULL;
    }

    size_t newLength = str->size * count;
    char* repeatedStr = (char*)malloc(newLength + 1);

    if (repeatedStr == NULL) {
        fmt_fprintf(stderr, "Failed to allocate memory in string_repeat\n");
        return NULL;
    }

    char* current = repeatedStr;
    for (size_t i = 0; i < count; ++i) {
        memcpy(current, str->dataStr, str->size);
        current += str->size;
    }
    *current = '\0';

    String* result = string_create(repeatedStr);
    free(repeatedStr);

    return result;
}

String* string_join_variadic(size_t count, ...) {
    va_list args;
    va_start(args, count);

    size_t totalLength = 0;
    for (size_t i = 0; i < count; ++i) {
        String* str = va_arg(args, String*);
        totalLength += str->size;
    }
    va_end(args);

    char* joinedStr = (char*)malloc(totalLength + 1);
    if (joinedStr == NULL) {
        perror("Failed to allocate memory in string_join_variadic");
        return NULL;
    }

    char* current = joinedStr;
    va_start(args, count);
    for (size_t i = 0; i < count; ++i) {
        String* str = va_arg(args, String*);

        memcpy(current, str->dataStr, str->size);
        current += str->size;
    }
    *current = '\0';
    va_end(args);

    String* result = string_create(joinedStr);
    free(joinedStr);

    return result;
}

void string_trim_characters(String* str, const char* chars) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_trim_characters.\n");
        return;
    }
    if (str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The dataStr of String object is NULL in string_trim_characters.\n");
        return;
    }
    if (chars == NULL) {
        fmt_fprintf(stderr, "Error: The chars parameter is NULL in string_trim_characters.\n");
        return;
    }
    char* start = str->dataStr;
    char* end = str->dataStr + str->size - 1;

    while (start <= end && strchr(chars, *start)) { 
        start++;
    }
    while (end > start && strchr(chars, *end)) { 
        end--;
    }
    size_t newLength = end - start + 1;

    memmove(str->dataStr, start, newLength);
    str->dataStr[newLength] = '\0';
    str->size = newLength;
}

void string_shuffle(String* str){
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_shuffle.\n");
        return;
    }
    if (str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The dataStr of String object is NULL in string_shuffle.\n");
        return;
    }

    srand(time(NULL)); 
    size_t length = strlen(str->dataStr);
    for (size_t i = length - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);

        // Swap characters at positions i and j
        char temp = str->dataStr[i];
        str->dataStr[i] = str->dataStr[j];
        str->dataStr[j] = temp;
    }
}

void string_to_title(String* str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_to_title.\n");
        return;
    }
    if (str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The dataStr of String object is NULL in string_to_title.\n");
        return;
    }

    bool capitalize = true;
    for (size_t i = 0; i < str->size; i++) {
        if (capitalize && isalpha(str->dataStr[i])) {
            str->dataStr[i] = toupper(str->dataStr[i]);
            capitalize = false;
        } 
        else if (!isalpha(str->dataStr[i])) {
            capitalize = true;
        } 
        else {
            str->dataStr[i] = tolower(str->dataStr[i]);
        }
    }
}

void string_to_capitalize(String* str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_to_capitalize.\n");
        return;
    }
    if (str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The dataStr of String object is NULL in string_to_capitalize.\n");
        return;
    }
    if (str->size == 0) {
        fmt_fprintf(stderr, "Error: The size of String object is zero in string_to_capitalize.\n");
        return;
    }
    str->dataStr[0] = toupper(str->dataStr[0]);
}

void string_to_casefold(String* str) {
     if (str == NULL || str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: Invalid string input in string_to_casefold.\n");
        return;
    }
    for (size_t i = 0; i < str->size; i++) {
        str->dataStr[i] = tolower(str->dataStr[i]);
    }
}

bool string_starts_with(const String* str, const char* substr) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_starts_with.\n");
        return false;
    }
    if (str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The dataStr of String object is NULL in string_starts_with.\n");
        return false;
    }
    if (substr == NULL) {
        fmt_fprintf(stderr, "Error: The substring is NULL in string_starts_with.\n");
        return false;
    }

    size_t substrLen = strlen(substr);
    if (substrLen > str->size) {
        return false;
    }
    return strncmp(str->dataStr, substr, substrLen) == 0;
}

bool string_ends_with(const String* str, const char* substr) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: The String object is NULL in string_ends_with.\n");
        return false;
    }
    if (str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: The dataStr of String object is NULL in string_ends_with.\n");
        return false;
    }
    if (substr == NULL) {
        fmt_fprintf(stderr, "Error: The substring is NULL in string_ends_with.\n");
        return false;
    }

    size_t substrLen = strlen(substr);
    size_t strLen = str->size;
    if (substrLen > strLen) {
        return false;
    }
    return strncmp(str->dataStr + strLen - substrLen, substr, substrLen) == 0;
}

void string_swap_case(String* str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: str is NULL in string_swap_case\n");
        return;
    }
    if (str->dataStr == NULL) {
        fmt_fprintf(stderr, "Error: str->dataStr is NULL in string_swap_case\n");
        return;
    }

    for (size_t i = 0; i < str->size; i++) {
        if (islower(str->dataStr[i])) {
            str->dataStr[i] = toupper(str->dataStr[i]);
        }
        else if (isupper(str->dataStr[i])) { 
            str->dataStr[i] = tolower(str->dataStr[i]);
        }
    }
}

wchar_t* string_to_unicode(const char* str) {
    if (str == NULL) {
        fmt_fprintf(stderr, "Error: Input string is NULL in string_to_unicode.\n");
        return NULL;
    }
    // Calculate the length of the wide string
    size_t len = mbstowcs(NULL, str, 0) + 1;
    if (len == (size_t)-1) {
        fmt_fprintf(stderr, "Error: Conversion failed in string_to_unicode.\n");
        return NULL;
    }
    wchar_t* wstr = malloc(len * sizeof(wchar_t));
    if (!wstr) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in string_to_unicode.\n");
        return NULL;
    }

    mbstowcs(wstr, str, len);
    return wstr;
}

String* string_from_unicode(const wchar_t* wstr) {
    if (wstr == NULL) {
        fmt_fprintf(stderr, "Error: Input wide string is NULL in string_from_unicode.\n");
        return NULL;
    }
    // Calculate the length of the string
    size_t len = wcstombs(NULL, wstr, 0);
    if (len == (size_t)-1) {
        fmt_fprintf(stderr, "Error: Conversion failed in string_from_unicode.\n");
        return NULL;
    }

    char* str = malloc(len + 1); // +1 for null terminator
    if (!str) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in string_from_unicode.\n");
        return NULL;
    }
    wcstombs(str, wstr, len + 1); // Convert and include the null terminator

    String* stringObj = string_create(str);
    free(str); // Free the temporary char* buffer

    return stringObj;
}

String** string_create_from_initializer(size_t count, ...) {
    if (count == 0) {
        fmt_fprintf(stderr, "Error: count is zero in string_create_from_initializer.\n");
        return NULL;
    }

    va_list args;
    va_start(args, count);

    // Allocate memory for the array of String pointers
    String** strings = (String**)malloc(sizeof(String*) * (count + 1)); // +1 for NULL termination
    if (!strings) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for strings array in string_create_from_initializer.\n");
        va_end(args);
        return NULL;
    }

    // Create each string and add it to the array
    for (size_t i = 0; i < count; i++) {
        char* str = va_arg(args, char*);
        strings[i] = string_create(str);
        if (!strings[i]) {
            fmt_fprintf(stderr, "Error: string_create failed for string: %s in string_create_from_initializer.\n", str);
            // Handle allocation failure: cleanup and exit
            for (size_t j = 0; j < i; j++) {
                string_deallocate(strings[j]);
            }
            free(strings);
            va_end(args);
            return NULL;
        }
    }

    // Null-terminate the array
    strings[count] = NULL;

    va_end(args);
    return strings;
}

char* string_strdup(const char* s) 
{
    if (s == NULL) {
        fmt_fprintf(stderr, "Error: Parameter 's' is NULL in string_strdup.\n");
        return NULL;
    }

    char* new_str = malloc(strlen(s) + 1);
    if (new_str == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in string_strdup for string: %s\n", s);
        return NULL;
    }
    strcpy(new_str, s);
    return new_str;
}

size_t string_length_cstr(const char* str) {
    if (!str) {
        fmt_fprintf(stderr, "Error: str is null in string_length_cstr.\n");
        return 0;
    }
    return (size_t)strlen(str);
}

size_t string_length_utf8(const char* str) {
    if (!str) {
        fmt_fprintf(stderr, "Error: str is null in string_length_cstr.\n");
        return 0;
    }
    size_t length = 0;

    while (*str) {
        if ((*str & 0xC0) != 0x80) {
            length++;
        }
        str++;
    }
    return length;
}

bool string_to_bool_from_cstr(const char* boolstr) {
    if (!boolstr) {
        fmt_fprintf(stderr, "Error: bool str is NULL and invalid in string_to_bool_cstr.\n");
        return false;
    }

    if (strcmp(boolstr, "true") == 0) {
        return true;
    }
    else if (strcmp(boolstr, "false") == 0) {
        return false;
    }

    return false;
}

size_t string_utf8_char_len(char c) {
    if ((c & 0x80) == 0) { 
        return 1;
    }
    if ((c & 0xE0) == 0xC0) {
        return 2;
    }
    if ((c & 0xF0) == 0xE0) {
        return 3;
    }
    if ((c & 0xF8) == 0xF0) {
        return 4;
    }
    return 0;
}
