/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class String
*/
#include <stdio.h>
#include <string.h>
#include <strings.h>  // For functions like strcasecmp (POSIX-specific)
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include <wchar.h>
#include "std_string.h"


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
        fprintf(stderr, "Error: Memory pool is NULL in memory_pool_allocate.\n");
        return NULL;
    }
    if (pool->used + size > pool->poolSize) {
        fprintf(stderr, "Error: Memory pool out of memory in memory_pool_allocate.\n");
        return NULL; // Pool is out of memory
    }

    void *mem = (char *)pool->pool + pool->used;
    pool->used += size;

    return mem;
}

static void memory_pool_destroy(MemoryPoolString *pool) {
    if (pool == NULL) {
        fprintf(stderr, "Warning: Attempt to destroy a NULL memory pool in memory_pool_destroy.\n");
        return;
    }
    free(pool->pool);
    free(pool);
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
    String* str = (String*)malloc(sizeof(String));
    if (!str) {
        fprintf(stderr, "Error: Memory allocation failed for String object in string_create.\n");
        exit(-1);
    }

    size_t initialSize = initialStr ? strlen(initialStr) : 0;
    str->size = initialSize;
    str->capacitySize = 32 + initialSize; // +1 for null terminator

    // Initialize memory pool for strings with a smaller size
    size_t initialPoolSize = 10000000; // 1KB
    str->pool = memory_pool_create(initialPoolSize);
    if (!str->pool) {
        fprintf(stderr, "Error: Memory pool creation failed in string_create.\n");
        free(str);
        exit(-1);
    }

    str->dataStr = memory_pool_allocate(str->pool, str->capacitySize);
    if (!str->dataStr) {
        fprintf(stderr, "Error: Memory pool allocation failed in string_create.\n");
        memory_pool_destroy(str->pool);
        free(str);
        exit(-1);
    }

    if (initialStr) {
        strcpy(str->dataStr, initialStr);
    }
    return str;
}

/**
 * @brief Creates a new String object with a memory pool of the specified size.
 *
 * This function creates a new String object that uses a globally initialized memory pool for efficient memory allocation.
 * The function ensures that the global memory pool is initialized before creating the String object.
 *
 * @param size The size of the memory pool to be initialized and used for the String object.
 * @return A pointer to the newly created String object, or exits the program with an error if memory allocation fails.
 */
String* string_create_with_pool(size_t size) {
    static int counter = 0;

    if (!counter) {
        init_global_memory_pool(size);
        counter++;
    }
    // Ensure global memory pool is initialized
    if (global_pool == NULL) {
        fprintf(stderr, "Error: Failed to initialize global memory pool in string_create_with_pool.\n");
        exit(-1);  // Consider handling the error without exiting
    }

    String* str = (String*)malloc(sizeof(String));
    if (!str) {
        fprintf(stderr, "Error: Memory allocation failed for String object in string_create_with_pool.\n");
        exit(-1);
    }

    str->size = 0;
    str->capacitySize = 1;
    str->dataStr = NULL; // Data is not allocated yet (lazy allocation)
    str->pool = global_pool; // Use the global pool

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
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_substr.\n");
        return NULL;
    }
    if (pos >= str->size) {
        fprintf(stderr, "Error: Position out of bounds in string_substr.\n");
        return NULL;
    }

    // Adjust len if it goes beyond the end of the string
    if (pos + len > str->size) { 
        len = str->size - pos;
    }

    String* substr = string_create(NULL); // Allocate memory for the substring
    if (substr == NULL) { 
        fprintf(stderr, "Error: Memory allocation failed for substring in string_substr.\n");
        return NULL;
    }

    substr->size = len;
    substr->capacitySize = len + 1;
    substr->dataStr = (char*)malloc(substr->capacitySize * sizeof(char));

    if (substr->dataStr == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for dataStr in substring in string_substr.\n");
        free(substr);
        return NULL;
    }

    strncpy(substr->dataStr, str->dataStr + pos, len); // Copy the substring
    substr->dataStr[len] = '\0';  // Null terminate the substring

    return substr;
}

/**
 * @brief Checks if the given String object is empty.
 *
 * This function checks whether the String object is empty, meaning it contains no characters or is NULL.
 *
 * @param str The String object to check. Can be NULL.
 * 
 * @return true if the String object is empty or NULL, false otherwise.
 */
bool string_empty(String* str) {
    return (str == NULL) ? true : (str->size == 0);
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
bool string_contains(String* str, const char* substr) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_contains.\n");
        return false;
    }
    if (substr == NULL) {
        fprintf(stderr, "Error: The substring is NULL in string_contains.\n");
        return false;
    }
    return strstr(str->dataStr, substr) != NULL;
}

/**
 * @brief Compares two String objects lexicographically.
 *
 * This function compares two String objects lexicographically. 
 * If either string is NULL, the NULL string is considered less than the non-NULL string. 
 * If both are NULL, they are considered equal.
 *
 * @param str1 The first String object to compare. Can be NULL.
 * @param str2 The second String object to compare. Can be NULL.
 * 
 * @return An integer value indicating the relationship between the strings:
 *         - A value less than 0 if str1 is less than str2
 *         - 0 if str1 is equal to str2
 *         - A value greater than 0 if str1 is greater than str2
 */
int string_compare(const String* str1, const String* str2) {
    if (str1 == NULL || str2 == NULL) {
        if (str1 == str2) {
            return 0;  // Both are NULL, considered equal
        }
        fprintf(stderr, "Error: One or both String objects are NULL in string_compare.\n");
        return (str1 == NULL) ? -1 : 1;  // NULL is considered less than non-NULL
    }
    return strcmp(str1->dataStr, str2->dataStr);
}

/**
 * @brief Compares two String objects for equality.
 *
 * This function compares two String objects to check if they are equal. 
 * It returns true if the strings are equal, otherwise false.
 *
 * @param str1 The first String object. Must not be NULL.
 * @param str2 The second String object. Must not be NULL.
 * 
 * @return true if the strings are equal, false otherwise.
 */
bool string_is_equal(String* str1, String* str2) {
    return string_compare(str1, str2) == 0;
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
bool string_is_less(String* str1, String* str2) {
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
bool string_is_greater(String* str1, String* str2) {
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
bool string_is_less_or_equal(String* str1, String* str2) {
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
bool string_is_greater_or_equal(String* str1, String* str2) {
    return string_compare(str1, str2) >= 0;
}

/**
 * @brief Compares two String objects for inequality.
 *
 * This function compares two String objects to check if they are not equal. 
 * It returns true if the strings are not equal, otherwise false.
 *
 * @param str1 The first String object. Must not be NULL.
 * @param str2 The second String object. Must not be NULL.
 * 
 * @return true if the strings are not equal, false otherwise.
 */
bool string_is_not_equal(String* str1, String* str2) {
    return string_compare(str1, str2) != 0;
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
bool string_is_digit(String* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_is_digit.\n");
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
bool string_is_upper(String* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_is_upper.\n");
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
bool string_is_lower(String* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_is_lower.\n");
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
    if (str != NULL && str->dataStr != NULL) {
        char* reverse = (char*) malloc(sizeof(char) * (str->size + 1));
        if (!reverse) {
            fprintf(stderr, "Error: Memory allocation failed in string_reverse.\n");
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
        fprintf(stderr, "Error: The String object or its data is NULL in string_reverse.\n");
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
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_resize.\n");
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
                fprintf(stderr, "Error: Memory allocation failed in string_resize.\n");
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

/**
 * @brief Reduces the capacity of the String object to fit its current size.
 *
 * This function shrinks the capacity of the String object to match its current size plus the null terminator.
 * If the current capacity is already optimal, the function does nothing.
 *
 * @param str The String object to shrink. Must not be NULL.
 */
void string_shrink_to_fit(String *str) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_shrink_to_fit.\n");
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
            fprintf(stderr, "Error: Memory allocation failed in string_shrink_to_fit.\n");
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

/**
 * @brief Appends a C-string to the end of the String object.
 *
 * This function appends the contents of `strItem` to the end of the String object.
 * If necessary, it reallocates memory to accommodate the additional characters.
 *
 * @param str The String object to which the string will be appended. Must not be NULL.
 * @param strItem The C-string to append. Must not be NULL.
 */
void string_append(String *str, const char *strItem) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_append.\n");
        return;
    }
    if (strItem == NULL) {
        fprintf(stderr, "Error: The strItem is NULL in string_append.\n");
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
            fprintf(stderr, "Error: Memory allocation failed in string_append.\n");
            return;
        }

        memcpy(newData, str->dataStr, str->size);
        str->dataStr = newData;
        str->capacitySize = newCapacity;
    }

    strcpy(str->dataStr + str->size, strItem);
    str->size += strItemLength;
}

/**
 * @brief Appends a single character to the end of the String object.
 *
 * This function appends the character `chItem` to the end of the String object.
 * If necessary, it reallocates memory to accommodate the additional character.
 *
 * @param str The String object to which the character will be appended. Must not be NULL.
 * @param chItem The character to append.
 */
void string_push_back(String* str, char chItem) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_push_back.\n");
        return;
    }
    if (str->size + 1 >= str->capacitySize) {
        // static int counter = 0;
        size_t newCapacity = str->capacitySize * 2;
        char* newData = memory_pool_allocate(str->pool, newCapacity);  // Allocate new space from the memory pool
        
        if (!newData) {
            fprintf(stderr, "Error: Memory allocation failed in string_push_back.\n");
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
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_assign.\n");
        return;
    }
    if (newStr == NULL) {
        fprintf(stderr, "Error: The newStr is NULL in string_assign.\n");
        return;
    }

    size_t newStrLength = strlen(newStr);
    if (newStrLength + 1 > str->capacitySize) {
        char *newData = memory_pool_allocate(str->pool, newStrLength + 1);
        if (!newData) {
            fprintf(stderr, "Error: Memory allocation failed in string_assign.\n");
            return;
        }


        str->dataStr = newData;
        str->capacitySize = newStrLength + 1;
    }

    strcpy(str->dataStr, newStr);
    str->size = newStrLength;
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
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_insert.\n");
        return;
    }
    if (strItem == NULL) {
        fprintf(stderr, "Error: The strItem is NULL in string_insert.\n");
        return;
    }
    if (pos > str->size) {
        fprintf(stderr, "Error: Position out of bounds in string_insert.\n");
        return;
    }

    size_t strItemLength = strlen(strItem);
    size_t newTotalLength = str->size + strItemLength;

    if (newTotalLength + 1 > str->capacitySize) {
        size_t newCapacity = newTotalLength + 1;
        char *newData = memory_pool_allocate(str->pool, newCapacity);
        if (!newData) {
            fprintf(stderr, "Error: Memory allocation failed in string_insert.\n");
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
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_erase.\n");
        return;
    }
    if (pos >= str->size) {
        fprintf(stderr, "Error: Position out of bounds in string_erase.\n");
        return;
    }
    if (pos + len > str->size) { 
        len = str->size - pos;  // Adjust len to not go beyond the string end
    }

    memmove(str->dataStr + pos, str->dataStr + pos + len, str->size - pos - len + 1);
    str->size -= len;
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
    if (str1 == NULL) {
        fprintf(stderr, "Error: The String object (str1) is NULL in string_replace.\n");
        return;
    }
    if (oldStr == NULL) {
        fprintf(stderr, "Error: The oldStr is NULL in string_replace.\n");
        return;
    }
    if (newStr == NULL) {
        fprintf(stderr, "Error: The newStr is NULL in string_replace.\n");
        return;
    }

    char *position = strstr(str1->dataStr, oldStr);
    if (position == NULL) { 
        fprintf(stderr, "Warning: oldStr not found in str1 in string_replace.\n");
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

/**
 * @brief Swaps the contents of two String objects.
 *
 * This function exchanges the contents of `str1` and `str2`.
 *
 * @param str1 The first String object. Must not be NULL.
 * @param str2 The second String object. Must not be NULL.
 */
void string_swap(String *str1, String *str2) {
    if (str1 == NULL || str2 == NULL) {
        fprintf(stderr, "Error: One or both String objects are NULL in string_swap.\n");
        return;
    }

    String temp = *str1;
    *str1 = *str2;
    *str2 = temp;
}

/**
 * @brief Removes the last character from the String object.
 *
 * This function removes the last character from the String object, reducing its size by one.
 * If the string is empty, the function logs a warning and does nothing.
 *
 * @param str The String object from which the last character is to be removed. Must not be NULL.
 */
void string_pop_back(String *str) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_pop_back.\n");
        return;
    }
    if (str->size == 0) {
        fprintf(stderr, "Warning: Attempt to pop back from an empty string in string_pop_back.\n");
        return;
    }

    str->dataStr[str->size - 1] = '\0';
    str->size--;
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
    if (str == NULL) {
        fprintf(stderr, "Warning: Attempt to deallocate a NULL String object in string_deallocate.\n");
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

/**
 * @brief Retrieves the character at the specified index in the String object.
 *
 * This function returns the character at the given index in the String object. If the index is out of range,
 * or the String object is NULL, the function logs an error and returns a default character ('\0').
 *
 * @param str The String object from which the character is to be retrieved. Must not be NULL.
 * @param index The index of the character to retrieve. Must be within the range of the string's size.
 * @return The character at the specified index, or '\0' if the index is out of range or the String object is NULL.
 */
char string_at(String* str, size_t index) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_at.\n");
        return '\0';  // Return a default character
    }
    if (index >= str->size) {
        fprintf(stderr, "Error: Index out of range in string_at.\n");
        return '\0';  // Return a default character
    }
    return str->dataStr[index]; // (const char)
}

/**
 * @brief Returns a pointer to the last character in the String object.
 *
 * This function returns a pointer to the last character in the String object. If the String object is NULL or empty,
 * the function returns NULL.
 * 
 * @param str The String object to access. Must not be NULL or empty.
 * @return A pointer to the last character in the string, or NULL if the string is NULL or empty.
 */
char* string_back(String *str) {
    if (str == NULL || str->size == 0) { 
        return NULL; 
    }
    return &str->dataStr[str->size - 1];
}

/**
 * @brief Returns a pointer to the first character in the String object.
 *
 * This function returns a pointer to the first character in the String object. If the String object is NULL or empty,
 * the function returns NULL.
 * 
 * @param str The String object to access. Must not be NULL or empty.
 * @return A pointer to the first character in the string, or NULL if the string is NULL or empty.
 */
char* string_front(String *str) {
    if (str == NULL || str->size == 0) { 
        return NULL;  
    }
    return &str->dataStr[0];
}

/**
 * @brief Retrieves the length of the String object.
 *
 * This function returns the current length (size) of the String object. If the String object is NULL, the function
 * returns 0 and logs an error.
 * 
 * @param str The String object whose length is to be determined. Must not be NULL.
 * @return The length of the string, or 0 if the String object is NULL.
 */
size_t string_length(String* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_length.\n");
        return 0;
    }
    return str->size;
}

/**
 * @brief Retrieves the capacity of the String object.
 *
 * This function returns the current capacity of the String object, which indicates the maximum size the string can grow
 * to without requiring a reallocation. If the String object is NULL, the function returns 0 and logs an error.
 * 
 * @param str The String object whose capacity is to be determined. Must not be NULL.
 * @return The capacity of the string, or 0 if the String object is NULL.
 */
size_t string_capacity(String* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_capacity.\n");
        return 0;
    }
    return str->capacitySize;
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
size_t string_max_size(String* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_max_size.\n");
        return 0;  // Or a special value indicating error
    }
    return (size_t)-1;  // You may want to define a more realistic maximum size
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
size_t string_copy(String *str, char *buffer, size_t pos, size_t len) {
    if (str == NULL || str->dataStr == NULL) {
        fprintf(stderr, "Error: The String object or its data is NULL in string_copy.\n");
        return 0;
    }
    if (buffer == NULL) {
        fprintf(stderr, "Error: The buffer is NULL in string_copy.\n");
        return 0;
    }
    if (pos >= str->size) {
        fprintf(stderr, "Error: Position out of bounds in string_copy.\n");
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
int string_find(String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL) {
        fprintf(stderr, "Error: The String object or its data is NULL in string_find.\n");
        return -1;
    }
    if (buffer == NULL) {
        fprintf(stderr, "Error: The buffer is NULL in string_find.\n");
        return -1;
    }
    if (pos >= str->size) {
        fprintf(stderr, "Error: Position out of bounds in string_find.\n");
        return -1;
    }

    const char *found = strstr(str->dataStr + pos, buffer);
    if (found == NULL) { 
        return -1;  // Substring not found
    }
    return (int)(found - str->dataStr);  // Return the position of the substring
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
int string_rfind(String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL) {
        fprintf(stderr, "Error: The String object or its data is NULL in string_rfind.\n");
        return -1;
    }
    if (buffer == NULL) {
        fprintf(stderr, "Error: The buffer is NULL in string_rfind.\n");
        return -1;
    }

    size_t bufferLen = strlen(buffer);
    if (bufferLen == 0) {
        fprintf(stderr, "Error: The buffer is empty in string_rfind.\n");
        return -1;
    }
    if (pos < bufferLen - 1) {
        fprintf(stderr, "Error: Position is too small in string_rfind.\n");
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
int string_find_first_of(String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL) {
        fprintf(stderr, "Error: The String object or its data is NULL in string_find_first_of.\n");
        return -1;
    }
    if (buffer == NULL) {
        fprintf(stderr, "Error: The buffer is NULL in string_find_first_of.\n");
        return -1;
    }
    if (pos >= str->size) {
        fprintf(stderr, "Error: Position out of bounds in string_find_first_of.\n");
        return -1;
    }

    const char *found = strstr(str->dataStr + pos, buffer);
    if (found != NULL) {
        return (int)(found - str->dataStr);
    }
    return -1;  // Buffer string not found
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
int string_find_last_of(String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL) {
        fprintf(stderr, "Error: The String object or its data is NULL in string_find_last_of.\n");
        return -1;
    }
    if (buffer == NULL) {
        fprintf(stderr, "Error: The buffer is NULL in string_find_last_of.\n");
        return -1;
    }
    if (pos >= str->size) {
        fprintf(stderr, "Error: Position out of bounds in string_find_last_of.\n");
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
int string_find_first_not_of(String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL) {
        fprintf(stderr, "Error: The String object or its data is NULL in string_find_first_not_of.\n");
        return -1;
    }
    if (buffer == NULL) {
        fprintf(stderr, "Error: The buffer is NULL in string_find_first_not_of.\n");
        return -1;
    }
    if (pos >= str->size) {
        fprintf(stderr, "Error: Position out of bounds in string_find_first_not_of.\n");
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
int string_find_last_not_of(String *str, const char *buffer, size_t pos) {
    if (str == NULL || str->dataStr == NULL) {
        fprintf(stderr, "Error: The String object or its data is NULL in string_find_last_not_of.\n");
        return -1;
    }
    if (buffer == NULL) {
        fprintf(stderr, "Error: The buffer is NULL in string_find_last_not_of.\n");
        return -1;
    }

    size_t bufferLen = strlen(buffer);
    if (bufferLen == 0) {
        fprintf(stderr, "Error: The buffer is empty in string_find_last_not_of.\n");
        return -1;
    }
    if (pos < bufferLen - 1) {
        fprintf(stderr, "Error: Position is too small in string_find_last_not_of.\n");
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

/**
 * @brief Returns the raw string data.
 *
 * This function returns a pointer to the character array managed by the String object.
 * If the input String is NULL or uninitialized, an error is logged, and the function returns NULL.
 *
 * @param str The String object to retrieve data from. Must not be NULL.
 * @return A pointer to the character data, or NULL on error.
 */
const char *string_data(String *str) {
    if (str == NULL || str->dataStr == NULL) {
        fprintf(stderr, "Error: Invalid input or uninitialized String in string_data function.\n");
        return NULL;
    }
    return str->dataStr;
}

/**
 * @brief Returns a constant C-string representation of the string.
 *
 * This function returns a constant pointer to the character array managed by the String object,
 * ensuring that the returned string is properly null-terminated.
 * If the input String is NULL or uninitialized, an error is logged, and the function returns an empty string.
 *
 * @param str The String object to retrieve the C-string from. Must not be NULL.
 * @return A constant pointer to the C-string, or an empty string on error.
 */
const char *string_c_str(const String *str) {
    if (str == NULL) { 
        fprintf(stderr, "Error: Invalid input in string_c_str function.\n");
        return "";  // Return empty string for null String
    }
    if (str->dataStr == NULL) {
        fprintf(stderr, "Error: Uninitialized String in string_c_str function.\n");
        return "";  // Return empty string for uninitialized String
    }
    return str->dataStr;
}

/**
 * @brief Returns a pointer to the beginning of the string.
 *
 * This function returns a pointer to the first character of the string data.
 * If the input String is NULL or uninitialized, an error is logged, and the function returns an empty string.
 *
 * @param str The String object to retrieve the beginning pointer from. Must not be NULL.
 * 
 * @return A pointer to the first character in the string, or an empty string on error.
 */
char *string_begin(String *str) {
    if (str == NULL) { 
        fprintf(stderr, "Error: Invalid input in string_begin function.\n");
        return "";  // Return empty string for null String
    }
    if (str->dataStr == NULL) {
        fprintf(stderr, "Error: Uninitialized String in string_begin function.\n");
        return "";  // Return empty string for uninitialized String
    }
    return str->dataStr;  // The beginning of the string
}

/**
 * @brief Returns a pointer to the end of the string.
 *
 * This function returns a pointer to the position just after the last character of the string data.
 * If the input String is NULL or uninitialized, an error is logged, and the function returns NULL.
 *
 * @param str The String object to retrieve the end pointer from. Must not be NULL.
 * @return A pointer to the end of the string, or NULL on error.
 */
char *string_end(String *str) {
    if (str == NULL || str->dataStr == NULL) { 
        fprintf(stderr, "Error: Invalid input or uninitialized String in string_end function.\n");
        return NULL;  // Return NULL for null or uninitialized String
    }
    return str->dataStr + str->size;  // The end of the string
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
char *string_rbegin(String *str) {
    if (str == NULL || str->dataStr == NULL || str->size == 0) {
        fprintf(stderr, "Error: Invalid input, uninitialized, or empty String in string_rbegin function.\n");
        return NULL; 
    }
    return str->dataStr + str->size - 1;
}

/**
 * @brief This function returns a pointer to one position before the start of the string data.
 * This is typically used in reverse iteration contexts.
 * If the input String is NULL or uninitialized, an error is logged, and the function returns NULL.
 *
 * @param str The String object for which to get the reverse end pointer. Must not be NULL.
 * 
 * @return A pointer to one before the first character in the string, or NULL on error.
 */
char *string_rend(String *str) {
    if (str == NULL || str->dataStr == NULL) { 
        fprintf(stderr, "Error: Invalid input or uninitialized String in string_rend function.\n");
        return NULL;
    }
    return str->dataStr - 1; 
}

/**
 * @brief This function returns a pointer to the first character of the string data.
 * If the input String is NULL or uninitialized, an error is logged, and the function returns NULL.
 *
 * @param str The String object for which to get the beginning pointer. Must not be NULL.
 * 
 * @return A constant pointer to the first character in the string, or NULL on error.
 */
const char *string_cbegin(String *str) {
    if (str == NULL || str->dataStr == NULL) { 
        fprintf(stderr, "Error: Invalid input or uninitialized String in string_cbegin function.\n");
        return NULL;  // Return NULL for null or uninitialized String
    }
    return str->dataStr;  // The beginning of the string
}

/**
 * @brief This function returns a pointer to the position just after the last character of the string data.
 * If the input String is NULL or uninitialized, an error is logged, and the function returns NULL.
 *
 * @param str The String object for which to get the end pointer. Must not be NULL.
 * 
 * @return A constant pointer to the end of the string, or NULL on error.
 */
const char *string_cend(String *str) {
    if (str == NULL || str->dataStr == NULL) { 
        fprintf(stderr, "Error: Invalid input or uninitialized String in string_cend function.\n");
        return NULL;  // Return NULL for null or uninitialized String
    }
    return str->dataStr + str->size;  // The end of the string
}

/**
 * @brief This function returns a pointer to the last character of the string data, allowing reverse iteration.
 * If the input String is NULL, uninitialized, or empty, an error is logged, and the function returns NULL.
 *
 * @param str The String object for which to get the reverse beginning pointer. Must not be NULL or empty.
 * @return A constant pointer to the last character in the string, or NULL on error.
 */
const char *string_crbegin(String *str) {
    if (str == NULL || str->dataStr == NULL || str->size == 0) { 
        fprintf(stderr, "Error: Invalid input, uninitialized, or empty String in string_crbegin function.\n");
        return NULL;  // Return NULL for null, uninitialized, or empty String
    }
    return str->dataStr + str->size - 1;  // Pointer to the last character
}

/**
 * @brief This function returns a pointer to one position before the start of the string data, allowing reverse iteration to stop.
 * If the input String is NULL or uninitialized, an error is logged, and the function returns NULL.
 *
 * @param str The String object for which to get the reverse end pointer. Must not be NULL.
 * @return A constant pointer to one before the first character in the string, or NULL on error.
 */
const char *string_crend(String *str) {
    if (str == NULL || str->dataStr == NULL) {
        fprintf(stderr, "Error: Invalid input or uninitialized String in string_crend function.\n");
        return NULL;  // Return NULL for null or uninitialized String
    }
    return str->dataStr - 1;  // Pointer to one before the first character
}

/**
 * @brief This function resets the String object, setting its size to zero and its first character to the null terminator,
 * effectively making it an empty string. If the input String is NULL, an informational message is logged.
 *
 * @param str The String object to clear. Must not be NULL.
 */
void string_clear(String* str) {
    if (str != NULL) {
        str->size = 0;  // Reset the size to 0, indicating the string is now empty

        // Set the first character to the null terminator.
        // This ensures that the string is considered empty when accessed.
        if (str->dataStr != NULL) { 
            str->dataStr[0] = '\0';
        }
    }
    fprintf(stderr, "Info : String object is null no need to clear in string_clear.\n");
}

/**
 * @brief This function creates a new C-string with all characters in the given String object converted to uppercase.
 * If the input String is NULL, an error is logged, and the function returns NULL.
 *
 * @param str The String object to be converted. Must not be NULL.
 * @return A new C-string with all characters in uppercase. The caller is responsible for freeing the allocated memory.
 */
char* string_to_upper(String* str) {
    if (str != NULL) {
        char* upper = (char*) malloc(sizeof(char) * (str->size + 1));
        if (!upper) {
            fprintf(stderr, "Error: Failed to allocate memory for string_to_upper function.\n");
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

    fprintf(stderr, "Error: Input 'str' is NULL in string_to_upper function.\n");
    return NULL;
}

/**
 * @brief This function creates a new C-string with all characters in the given String object converted to lowercase.
 * If the input String is NULL, an error is logged, and the function returns NULL.
 *
 * @param str The String object to be converted. Must not be NULL.
 * @return A new C-string with all characters in lowercase. The caller is responsible for freeing the allocated memory.
 */
char* string_to_lower(String* str) {
    if (str != NULL) {
        char* lower = (char*) malloc(sizeof(char) * (str->size + 1));
        if (!lower) {
            fprintf(stderr, "Error: Failed to allocate memory for string_to_lower function.\n");
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

    fprintf(stderr, "Error: Input 'str' is NULL in string_to_lower function.\n");
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
        fprintf(stderr, "Error: Invalid input - 'str' is NULL in string_set_pool_size.\n");
        return false;
    }
    
    if (newSize == 0) { 
        fprintf(stderr, "Error: Invalid input - 'newSize' is zero in string_set_pool_size.\n");
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
        fprintf(stderr, "Error: Failed to create a new memory pool in string_set_pool_size.\n");
        return false; // Return false if memory pool creation fails
    }
    // If the string already has data, reallocate it in the new pool
    if (str->size > 0 && str->dataStr) {
        char* newData = memory_pool_allocate(str->pool, str->size + 1); // +1 for null terminator
        if (!newData) {
            fprintf(stderr, "Error: Failed to allocate memory for string data in the new pool in string_set_pool_size.\n");
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

/**
 * @brief This function appends the content of the second String object (str2) to the first String object (str1).
 * If either of the input String objects is NULL, the function logs an error and returns without performing any operation.
 *
 * @param str1 The String object to which the content will be appended. Must not be NULL.
 * @param str2 The String object whose content will be appended to str1. Must not be NULL.
 */
void string_concatenate(String *str1, const String *str2) {
    if (str1 == NULL) {
        fprintf(stderr, "Error: Null String object 'str1' in string_concatenate.\n");
        return;
    }
    if (str2 == NULL) {
        fprintf(stderr, "Error: Null String object 'str2' in string_concatenate.\n");
        return;
    }
    string_append(str1, str2->dataStr);
}

/**
 * @brief This function removes all leading whitespace characters from the String object.
 * If the String object is NULL or empty, the function returns without performing any operation.
 *
 * @param str The String object to be trimmed. Must not be NULL.
 */
void string_trim_left(String *str) {
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_trim_left.\n");
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

/**
 * @brief This function removes all trailing whitespace characters from the String object.
 * If the String object is NULL or empty, the function returns without performing any operation.
 *
 * @param str The String object to be trimmed. Must not be NULL.
 */
void string_trim_right(String *str) {
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_trim_right.\n");
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

/**
 * @brief This function removes all leading and trailing whitespace characters from the String object.
 * It combines the operations of string_trim_left and string_trim_right.
 * If the String object is NULL, the function returns without performing any operation.
 *
 * @param str The String object to be trimmed. Must not be NULL.
 */
void string_trim(String *str) {
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_trim.\n");
        return;
    }

    string_trim_left(str);
    string_trim_right(str);
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
String** string_split(String *str, const char *delimiter, int *count) {
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_split.\n");
        return NULL;
    }
    if (delimiter == NULL) {
        fprintf(stderr, "Error: Null delimiter in string_split.\n");
        return NULL;
    }

    size_t num_splits = 0;
    char *temp = string_strdup(str->dataStr);
    if (temp == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in string_split.\n");
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
        fprintf(stderr, "Error: Memory allocation failed for splits in string_split.\n");
        return NULL;
    }

    temp = string_strdup(str->dataStr);
    if (temp == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in string_split.\n");
        free(splits);
        return NULL;
    }

    token = strtok(temp, delimiter);
    size_t index = 0;

    while (token != NULL && index < num_splits) {
        splits[index] = string_create(token);
        if (splits[index] == NULL) {
            fprintf(stderr, "Error: Failed to create string in string_split.\n");
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
    if (strings == NULL) {
        fprintf(stderr, "Error: Null string array in string_join.\n");
        return NULL;
    }
    if (count <= 0) {
        fprintf(stderr, "Error: Invalid count in string_join.\n");
        return NULL;
    }
    if (delimiter == NULL) {
        fprintf(stderr, "Error: Null delimiter in string_join.\n");
        return NULL;
    }

    String *result = string_create("");
    if (result == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in string_join.\n");
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
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_replace_all.\n");
        return;
    }
    if (oldStr == NULL || newStr == NULL) {
        fprintf(stderr, "Error: Null substring in string_replace_all.\n");
        return;
    }

    String *temp = string_create("");
    if (temp == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in string_replace_all.\n");
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

/**
 * @brief Converts the content of a String object to an integer.
 *
 * This function converts the content of the given String object to an integer. 
 * If the String object is NULL or empty, the function logs an error and returns 0.
 *
 * @param str The String object to convert. Must not be NULL.
 * @return The integer value of the String object's content, or 0 if the String object is NULL or empty.
 */
int string_to_int(String *str) {
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_to_int.\n");
        return 0;
    }
    if (string_empty(str)) {
        fprintf(stderr, "Error: Empty string in string_to_int.\n");
        return 0;
    }
    return atoi(str->dataStr);
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
float string_to_float(String *str) {
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_to_float.\n");
        return 0.0f;
    }
    if (string_empty(str)) {
        fprintf(stderr, "Error: Empty string in string_to_float.\n");
        return 0.0f;
    }
    return atof(str->dataStr);
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
double string_to_double(String* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_to_double.\n");
        return 0.0;
    }
    if (string_empty(str)) {
        fprintf(stderr, "Error: Empty string in string_to_double.\n");
        return 0.0;
    }
    return strtod(str->dataStr, NULL);
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
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_pad_left.\n");
        return;
    }
    if (str->size >= totalLength) {
        fprintf(stderr, "Error: Size of String object is bigger or equal that total Length in string_pad_left.\n");
        return;
    }
    size_t padSize = totalLength - str->size;
    size_t newSize = str->size + padSize;
    char *newData = (char *)malloc(newSize + 1); // +1 for null terminator

    if (newData == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory in string_pad_left.\n");
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
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_pad_right.\n");
        return;
    }
    if (str->size >= totalLength) {
        fprintf(stderr, "Error: Size of String object is bigger or equal that total Length in string_pad_right.\n");
        return;
    }

    size_t padSize = totalLength - str->size;
    size_t newSize = str->size + padSize;
    char *newData = (char *)realloc(str->dataStr, newSize + 1); // +1 for null terminator

    if (newData == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory in string_pad_right.\n");
        return;
    }

    memset(newData + str->size, padChar, padSize);
    newData[newSize] = '\0';

    str->dataStr = newData;
    str->size = newSize;
    str->capacitySize = newSize + 1;
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
String* string_to_hex(String *str) {
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_to_hex.\n");
        return NULL;
    }
    if (string_empty(str)) {
        return string_create(""); // Return an empty string for an empty input.
    }

    String *hexStr = string_create("");
    if (hexStr == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in string_to_hex.\n");
        return NULL;
    }

    for (size_t i = 0; i < str->size; ++i) {
        char buffer[3];  // Each char can be represented by max 2 hex digits + null terminator
        
        sprintf(buffer, "%02x", (unsigned char)str->dataStr[i]);
        string_append(hexStr, buffer);
    }
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
 * @return A new String object containing the decoded ASCII characters, or NULL if an error occurs.
 */
String* string_from_hex(String *hexStr) {
    if (hexStr == NULL) {
        fprintf(stderr, "Error: Null String object in string_from_hex.\n");
        return NULL;
    }
    if (string_empty(hexStr) || (hexStr->size % 2) != 0) {
        fprintf(stderr, "Error: Invalid hex string in string_from_hex.\n");
        return NULL; // Hex string should have an even number of characters
    }

    String *str = string_create("");
    if (str == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in string_from_hex.\n");
        return NULL;
    }

    for (size_t i = 0; i < hexStr->size; i += 2) {
        char buffer[3] = {hexStr->dataStr[i], hexStr->dataStr[i + 1], '\0'};
        char ch = (char)strtol(buffer, NULL, 16);
        
        string_push_back(str, ch);
    }
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
size_t string_count(String* str, const char* substr) {
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_count.\n");
        return 0;
    }
    if (substr == NULL) {
        fprintf(stderr, "Error: Null substring in string_count.\n");
        return 0;
    }
    if (str->dataStr == NULL) {
        fprintf(stderr, "Error: Null data string in String object in string_count.\n");
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
    if (str == NULL) {
        fprintf(stderr, "Error: Null String object in string_remove.\n");
        return;
    }
    if (substr == NULL) {
        fprintf(stderr, "Error: Null substring in string_remove.\n");
        return;
    }
    if (str->dataStr == NULL) {
        fprintf(stderr, "Error: Null data string in String object in string_remove.\n");
        return;
    }
    if (strlen(substr) == 0) {
        fprintf(stderr, "Error: Empty substring in string_remove.\n");
        return;
    }
    size_t len = strlen(substr);
    char* p = str->dataStr;

    while ((p = strstr(p, substr)) != NULL) { 
        memmove(p, p + len, strlen(p + len) + 1);
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
    if (str == NULL || str->dataStr == NULL) {
        fprintf(stderr, "Error: Null String object in string_remove_range.\n");
        return;
    }
    if (startPos >= endPos || endPos > str->size) {
        fprintf(stderr, "Error: Invalid range in string_remove_range.\n");
        return;
    }
    size_t length = endPos - startPos;
    
    memmove(str->dataStr + startPos, str->dataStr + endPos, str->size - endPos + 1); // +1 for null terminator
    str->size -= length;
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
    char buffer[12]; // Enough to hold any 32-bit integer
    sprintf(buffer, "%d", value);

    return string_create(buffer);
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
    char buffer[12]; // Enough to hold any 32-bit integer
    sprintf(buffer, "%d", value);

    char* result = malloc(strlen(buffer) + 1); // +1 for null-terminator
    if (result) {
        strcpy(result, buffer);
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
    char buffer[32]; // A general buffer size for a float
    sprintf(buffer, "%f", value);

    return string_create(buffer);
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
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%f", value);

    return string_create(buffer);
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
String** string_tokenize(String* str, const char* delimiters, int* count) {
    if (str == NULL || delimiters == NULL) {
        fprintf(stderr, "Error: Invalid input in string_tokenize.\n");
        return NULL;
    }
    size_t num_tokens = 0;
    char* temp_str = string_strdup(str->dataStr); // strdup
    if (temp_str == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in string_tokenize.\n");
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
        fprintf(stderr, "Error: Memory allocation failed for tokens in string_tokenize.\n");
        return NULL;
    }
    // Tokenize again to fill the array
    temp_str = string_strdup(str->dataStr);
    if (temp_str == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in string_tokenize.\n");
        free(tokens);
        return NULL;
    }
    token = strtok(temp_str, delimiters);
    size_t idx = 0;

    while (token != NULL && idx < num_tokens) {
        tokens[idx] = string_create(token);
        if (tokens[idx] == NULL) {
            fprintf(stderr, "Error: string_create failed in string_tokenize.\n");
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
    if (input == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_baes64_encode.\n");
        return NULL;
    }
    if (input->dataStr == NULL) {
        fprintf(stderr, "Error: The dataStr of String object is NULL in string_base64_encode.\n");
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
    if (encodedStr == NULL) {
        fprintf(stderr, "Error: encodedStr param is null in string_base64_decode\n");
        return NULL;
    }
    if (encodedStr->dataStr == NULL) {
        fprintf(stderr, "Error: dataStr item of encodedStr object is null in string_base64_decode\n");
        return NULL;
    }
    char* decodedStr = (char*)malloc(encodedStr->size * 3 / 4 + 1); 

    if (decodedStr == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for base64 decoding");
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
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_format.\n");
        return;
    }
    if (format == NULL) {
        fprintf(stderr, "Error: The format string is NULL in string_format.\n");
        return;
    }
    // Start variadic argument processing
    va_list args;
    va_start(args, format);

    // Calculate the required length of the result string
    int length = vsnprintf(NULL, 0, format, args);
    if (length < 0) {
        fprintf(stderr, "Error: vsnprintf failed in string_format.\n");
        va_end(args);
        return;
    }

    // Allocate memory for the formatted string
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Error: Failed to allocate memory in string_format.\n");
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
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_repeat.\n");
        return NULL;
    }
    if (str->dataStr == NULL) {
        fprintf(stderr, "Error: The dataStr of String object is NULL in string_repeat.\n");
        return NULL;
    }

    size_t newLength = str->size * count;
    char* repeatedStr = (char*)malloc(newLength + 1);

    if (repeatedStr == NULL) {
        fprintf(stderr, "Failed to allocate memory in string_repeat\n");
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
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_trim_characters.\n");
        return;
    }
    if (str->dataStr == NULL) {
        fprintf(stderr, "Error: The dataStr of String object is NULL in string_trim_characters.\n");
        return;
    }
    if (chars == NULL) {
        fprintf(stderr, "Error: The chars parameter is NULL in string_trim_characters.\n");
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

/**
 * @brief This function randomizes the order of characters in the String object `str`. 
 * The function checks that the String object and its data are not NULL before shuffling.
 *
 * @param str The String object to shuffle. Must not be NULL.
 */
void string_shuffle(String* str){
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_shuffle.\n");
        return;
    }
    if (str->dataStr == NULL) {
        fprintf(stderr, "Error: The dataStr of String object is NULL in string_shuffle.\n");
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

/**
 * @brief This function converts the String object `str` to title case, where the first letter 
 * of each word is capitalized and all other letters are in lowercase. 
 * Non-alphabetic characters are treated as word boundaries.
 *
 * @param str The String object to convert to title case. Must not be NULL.
 */
void string_to_title(String* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_to_title.\n");
        return;
    }
    if (str->dataStr == NULL) {
        fprintf(stderr, "Error: The dataStr of String object is NULL in string_to_title.\n");
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

/**
 * @brief This function converts the first character of the String object `str` to uppercase.
 * The function ensures that the String object is not NULL, its data is not NULL, 
 * and that it contains at least one character.
 *
 * @param str The String object to capitalize. Must not be NULL.
 */
void string_to_capitalize(String* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_to_capitalize.\n");
        return;
    }
    if (str->dataStr == NULL) {
        fprintf(stderr, "Error: The dataStr of String object is NULL in string_to_capitalize.\n");
        return;
    }
    if (str->size == 0) {
        fprintf(stderr, "Error: The size of String object is zero in string_to_capitalize.\n");
        return;
    }
    str->dataStr[0] = toupper(str->dataStr[0]);
}

/**
 * @brief This function iterates over the characters in the String object `str` and converts 
 * each character to its lowercase equivalent.
 *
 * @param str The String object to casefold. Must not be NULL.
 */
void string_to_casefold(String* str) {
     if (str == NULL || str->dataStr == NULL) {
        fprintf(stderr, "Error: Invalid string input in string_to_casefold.\n");
        return;
    }
    for (size_t i = 0; i < str->size; i++) {
        str->dataStr[i] = tolower(str->dataStr[i]);
    }
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
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_starts_with.\n");
        return false;
    }
    if (str->dataStr == NULL) {
        fprintf(stderr, "Error: The dataStr of String object is NULL in string_starts_with.\n");
        return false;
    }
    if (substr == NULL) {
        fprintf(stderr, "Error: The substring is NULL in string_starts_with.\n");
        return false;
    }

    size_t substrLen = strlen(substr);
    if (substrLen > str->size) {
        return false;
    }
    return strncmp(str->dataStr, substr, substrLen) == 0;
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
    if (str == NULL) {
        fprintf(stderr, "Error: The String object is NULL in string_ends_with.\n");
        return false;
    }
    if (str->dataStr == NULL) {
        fprintf(stderr, "Error: The dataStr of String object is NULL in string_ends_with.\n");
        return false;
    }
    if (substr == NULL) {
        fprintf(stderr, "Error: The substring is NULL in string_ends_with.\n");
        return false;
    }

    size_t substrLen = strlen(substr);
    size_t strLen = str->size;
    if (substrLen > strLen) {
        return false;
    }
    return strncmp(str->dataStr + strLen - substrLen, substr, substrLen) == 0;
}

/**
 * @brief This function converts all lowercase characters in the String object `str` to uppercase,
 * and all uppercase characters to lowercase.
 *
 * @param str The String object whose case is to be swapped. Must not be NULL.
 */
void string_swap_case(String* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: str is NULL in string_swap_case\n");
        return;
    }
    if (str->dataStr == NULL) {
        fprintf(stderr, "Error: str->dataStr is NULL in string_swap_case\n");
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

/**
 * @brief This function converts a multi-byte C-string `str` to a wide-character string (typically Unicode).
 *
 * @param str The C-string to be converted. Must not be NULL.
 * 
 * @return A pointer to the newly allocated wide-character string, or NULL if the input string
 *         is NULL, the conversion fails, or memory allocation fails.
 */
wchar_t* string_to_unicode(const char* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: Input string is NULL in string_to_unicode.\n");
        return NULL;
    }
    // Calculate the length of the wide string
    size_t len = mbstowcs(NULL, str, 0) + 1;
    if (len == (size_t)-1) {
        fprintf(stderr, "Error: Conversion failed in string_to_unicode.\n");
        return NULL;
    }
    wchar_t* wstr = malloc(len * sizeof(wchar_t));
    if (!wstr) {
        fprintf(stderr, "Error: Memory allocation failed in string_to_unicode.\n");
        return NULL;
    }

    mbstowcs(wstr, str, len);
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
    if (wstr == NULL) {
        fprintf(stderr, "Error: Input wide string is NULL in string_from_unicode.\n");
        return NULL;
    }
    // Calculate the length of the string
    size_t len = wcstombs(NULL, wstr, 0);
    if (len == (size_t)-1) {
        fprintf(stderr, "Error: Conversion failed in string_from_unicode.\n");
        return NULL;
    }

    char* str = malloc(len + 1); // +1 for null terminator
    if (!str) {
        fprintf(stderr, "Error: Memory allocation failed in string_from_unicode.\n");
        return NULL;
    }
    wcstombs(str, wstr, len + 1); // Convert and include the null terminator

    String* stringObj = string_create(str);
    free(str); // Free the temporary char* buffer

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
    if (count == 0) {
        fprintf(stderr, "Error: count is zero in string_create_from_initializer.\n");
        return NULL;
    }

    va_list args;
    va_start(args, count);

    // Allocate memory for the array of String pointers
    String** strings = (String**)malloc(sizeof(String*) * (count + 1)); // +1 for NULL termination
    if (!strings) {
        fprintf(stderr, "Error: Memory allocation failed for strings array in string_create_from_initializer.\n");
        va_end(args);
        return NULL;
    }

    // Create each string and add it to the array
    for (size_t i = 0; i < count; i++) {
        char* str = va_arg(args, char*);
        strings[i] = string_create(str);
        if (!strings[i]) {
            fprintf(stderr, "Error: string_create failed for string: %s in string_create_from_initializer.\n", str);
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

/**
 * @brief This function allocates sufficient memory to hold a copy of the provided
 * C-string `s`, copies the content of `s` into the newly allocated memory, and
 * returns a pointer to this memory. It is the caller's responsibility to free
 * the allocated memory when it is no longer needed.
 *
 * @param s The C-string to be duplicated. Must not be NULL.
 * 
 * @return A pointer to the newly allocated string that contains a copy of `s`,
 * or NULL if the input string is NULL or memory allocation fails.
 */
char* string_strdup(const char* str) {
    if (str == NULL) {
        fprintf(stderr, "Error: Parameter 'str' is NULL in string_strdup.\n");
        return NULL;
    }

    char* new_str = malloc(strlen(str) + 1);
    if (new_str == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in string_strdup for string: %s\n", str);
        return NULL;
    }
    strcpy(new_str, str);
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
    if (!str) {
        fprintf(stderr, "Error: str is null in string_length_cstr.\n");
        return 0;
    }
    return (size_t)strlen(str);
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
    if (!str) {
        fprintf(stderr, "Error: str is null in string_length_cstr.\n");
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
    if (!boolstr) {
        fprintf(stderr, "Error: bool str is NULL and invalid in string_to_bool_cstr.\n");
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
    return strcmp(str1, str2);
}