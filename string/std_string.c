/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class String
*/
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
    STRING_LOG("[init_global_memory_pool]: Initializing global memory pool with size %zu.", size);

    if (global_pool == NULL) {
        global_pool = memory_pool_create(size);
        memoryPoolCreated = true;
        STRING_LOG("[init_global_memory_pool]: Global memory pool created successfully.");
    } 
    else {
        STRING_LOG("[init_global_memory_pool]: Global memory pool already exists.");
    }
}

static void destroy_global_memory_pool() {
    STRING_LOG("[destroy_global_memory_pool]: Destroying global memory pool.");

    if (global_pool != NULL && memoryPoolCreated) {
        memory_pool_destroy(global_pool);
        global_pool = NULL;
        STRING_LOG("[destroy_global_memory_pool]: Global memory pool destroyed successfully.");
    }
    else {
        STRING_LOG("[destroy_global_memory_pool]: No memory pool to destroy.");
    }
}

static MemoryPoolString *memory_pool_create(size_t size) {
    STRING_LOG("[memory_pool_create]: Creating memory pool with size %zu.", size);
    MemoryPoolString *pool = (MemoryPoolString*) malloc(sizeof(MemoryPoolString));

    if (pool) {
        pool->pool = malloc(size);
        if (!pool->pool) {
            free(pool);
            STRING_LOG("[memory_pool_create]: Failed to allocate memory for the pool.");
            return NULL;
        }

        pool->poolSize = size;
        pool->used = 0;
        STRING_LOG("[memory_pool_create]: Memory pool created successfully with size %zu.", size);
    } 
    else {
        STRING_LOG("[memory_pool_create]: Failed to allocate MemoryPoolString structure.");
    }

    return pool;
}

static void *memory_pool_allocate(MemoryPoolString *pool, size_t size) {
    STRING_LOG("[memory_pool_allocate]: Allocating %zu bytes from memory pool.", size);

    if (pool == NULL) {
        STRING_LOG("Error: Memory pool is NULL in memory_pool_allocate.");
        return NULL;
    }
    if (pool->used + size > pool->poolSize) {
        STRING_LOG("Error: Memory pool out of memory. Requested %zu bytes, but only %zu bytes are available.", size, pool->poolSize - pool->used);
        return NULL;
    }

    void *mem = (char *)pool->pool + pool->used;
    pool->used += size;
    STRING_LOG("[memory_pool_allocate]: Allocated %zu bytes. Total used memory: %zu/%zu.", size, pool->used, pool->poolSize);

    return mem;
}

static void memory_pool_destroy(MemoryPoolString *pool) {
    STRING_LOG("[memory_pool_destroy]: Destroying memory pool.");
    if (pool == NULL) {
        STRING_LOG("Warning: Attempt to destroy a NULL memory pool in memory_pool_destroy.");
        return;
    }
    free(pool->pool);
    free(pool);

    STRING_LOG("[memory_pool_destroy]: Memory pool destroyed successfully.");
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

    String* str = (String*)malloc(sizeof(String));
    if (!str) {
        STRING_LOG("Error: Memory allocation failed for String object in string_create.");
        exit(-1);
    }

    size_t initialSize = initialStr ? strlen(initialStr) : 0;
    str->size = initialSize;
    str->capacitySize = 32 + initialSize; // +1 for null terminator

    STRING_LOG("[string_create]: Initial size: %zu, Capacity size: %zu", initialSize, str->capacitySize);

    // Initialize memory pool for strings with a smaller size
    size_t initialPoolSize = 10000000; // 1KB
    str->pool = memory_pool_create(initialPoolSize);
    if (!str->pool) {
        STRING_LOG("[string_create]: Error: Memory pool creation failed in string_create.");
        free(str);
        exit(-1);
    }

    str->dataStr = (char*) memory_pool_allocate(str->pool, str->capacitySize);
    if (!str->dataStr) {
        STRING_LOG("[string_create]: Error: Memory pool allocation failed in string_create.");
        memory_pool_destroy(str->pool);
        free(str);
        exit(-1);
    }

    if (initialStr) {
        strcpy(str->dataStr, initialStr);
        STRING_LOG("[string_create]: String initialized with content: %s", str->dataStr);
    }

    STRING_LOG("[string_create]: String creation successful.");
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
        STRING_LOG("[string_create_with_pool]: Initializing global memory pool with size: %zu\n", size);
        init_global_memory_pool(size);
        counter++;
    }

    // Ensure global memory pool is initialized
    if (global_pool == NULL) {
        STRING_LOG("[string_create_with_pool]: Error: Failed to initialize global memory pool.\n");
        exit(-1);  // Consider handling the error without exiting
    }

    STRING_LOG("[string_create_with_pool]: Allocating memory for String object.\n");
    String* str = (String*)malloc(sizeof(String));
    if (!str) {
        STRING_LOG("[string_create_with_pool]: Error: Memory allocation failed for String object.\n");
        exit(-1);
    }

    STRING_LOG("[string_create_with_pool]: String object allocated. Setting initial size and capacity.\n");
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
    STRING_LOG("[string_substr]: Entering function with pos=%zu, len=%zu.\n", pos, len);

    if (str == NULL) {
        STRING_LOG("[string_substr]: Error: The String object is NULL.\n");
        return NULL;
    }
    if (pos >= str->size) {
        STRING_LOG("[string_substr]: Error: Position out of bounds (pos=%zu, size=%zu).\n", pos, str->size);
        return NULL;
    }

    // Adjust len if it goes beyond the end of the string
    if (pos + len > str->size) { 
        len = str->size - pos;
    }

    STRING_LOG("[string_substr]: Allocating memory for substring (len=%zu).\n", len);
    String* substr = string_create(NULL); // Allocate memory for the substring
    if (substr == NULL) { 
        STRING_LOG("[string_substr]: Error: Memory allocation failed for substring.\n");
        return NULL;
    }

    substr->size = len;
    substr->capacitySize = len + 1;
    substr->dataStr = (char*)malloc(substr->capacitySize * sizeof(char));

    if (substr->dataStr == NULL) {
        STRING_LOG("[string_substr]: Error: Memory allocation failed for dataStr in substring.\n");
        free(substr);
        return NULL;
    }

    STRING_LOG("[string_substr]: Copying substring from pos=%zu, len=%zu.\n", pos, len);
    strncpy(substr->dataStr, str->dataStr + pos, len); 
    substr->dataStr[len] = '\0';  

    STRING_LOG("[string_substr]: Successfully created substring.\n");
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
bool string_empty(const String* str) {
    STRING_LOG("[string_empty]: Checking if the string is empty.\n");

    if (str == NULL) {
        STRING_LOG("[string_empty]: The String object is NULL.\n");
        return true;
    }

    bool isEmpty = (str->size == 0);
    STRING_LOG("[string_empty]: String is %s.\n", isEmpty ? "empty" : "not empty");

    return isEmpty;
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

    if (str == NULL) {
        STRING_LOG("[string_contains]: Error: The String object is NULL.\n");
        return false;
    }
    if (substr == NULL) {
        STRING_LOG("[string_contains]: Error: The substring is NULL.\n");
        return false;
    }

    bool found = strstr(str->dataStr, substr) != NULL;
    STRING_LOG("[string_contains]: Substring %s in string: %s.\n", found ? "found" : "not found", substr);

    return found;
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
    STRING_LOG("[string_compare]: Comparing two strings.\n");

    if (str1 == NULL || str2 == NULL) {
        if (str1 == str2) {
            STRING_LOG("[string_compare]: Both strings are NULL. Returning 0 (equal).\n");
            return 0;  
        }

        STRING_LOG("[string_compare]: One or both String objects are NULL.\n");
        return (str1 == NULL) ? -1 : 1;  
    }
    int result = strcmp(str1->dataStr, str2->dataStr);
    STRING_LOG("[string_compare]: Comparison result is %d.\n", result);

    return result;
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
bool string_is_equal(const String* str1, const String* str2) {
    STRING_LOG("[string_is_equal]: Checking if two strings are equal.\n");
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
bool string_is_not_equal(const String* str1, const String* str2) {
    STRING_LOG("[string_is_not_equal]: Comparing if the two strings are not equal.\n");
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
        char* reverse = (char*)malloc(sizeof(char) * (str->size + 1));
        if (!reverse) {
            STRING_LOG("[string_reverse]: Error - Memory allocation failed.\n");
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
        if (newSize >= str->capacitySize) {
            size_t newCapacity = newSize + 1;
            char *newData = (char*)memory_pool_allocate(str->pool, newCapacity);

            if (!newData) {
                STRING_LOG("[string_resize]: Error - Memory allocation failed.\n");
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
    STRING_LOG("[string_shrink_to_fit]: Shrinking the String object to fit its size.\n");

    if (str == NULL) {
        STRING_LOG("[string_shrink_to_fit]: Error - The String object is NULL.\n");
        return;
    }
    if (str->size + 1 == str->capacitySize) {
        STRING_LOG("[string_shrink_to_fit]: No need to shrink, already optimal size.\n");
        return; // No need to shrink if already at optimal size
    }
    // Check if the string is using the memory pool
    if (str->dataStr != NULL) {
        // Allocate new space from the memory pool
        size_t newCapacity = str->size + 1; // +1 for null terminator
        char *newData = (char*)memory_pool_allocate(str->pool, newCapacity);

        if (newData == NULL) {
            STRING_LOG("[string_shrink_to_fit]: Error - Memory allocation failed.\n");
            return;
        }
        // Copy existing data to the new space
        memcpy(newData, str->dataStr, str->size);
        newData[str->size] = '\0'; // Null-terminate the string

        // Update the string's metadata
        str->dataStr = newData;
        str->capacitySize = newCapacity;
        STRING_LOG("[string_shrink_to_fit]: Shrink successful, new capacity: %zu.\n", newCapacity);
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
    STRING_LOG("[string_append]: Appending string '%s' to the String object.\n", strItem);

    if (str == NULL) {
        STRING_LOG("[string_append]: Error - The String object is NULL.\n");
        return;
    }
    if (strItem == NULL) {
        STRING_LOG("[string_append]: Error - The strItem is NULL.\n");
        return;
    }
    size_t strItemLength = strlen(strItem);
    if (strItemLength == 0) { 
        STRING_LOG("[string_append]: The strItem is empty, nothing to append.\n");
        return;
    }

    if (str->size + strItemLength >= str->capacitySize) {
        size_t newCapacity = str->size + strItemLength + 1;
        char *newData = (char*)memory_pool_allocate(str->pool, newCapacity);

        if (!newData) {
            STRING_LOG("[string_append]: Error - Memory allocation failed.\n");
            return;
        }

        memcpy(newData, str->dataStr, str->size);
        str->dataStr = newData;
        str->capacitySize = newCapacity;
        STRING_LOG("[string_append]: Resized the string to new capacity: %zu.\n", newCapacity);
    }

    strcpy(str->dataStr + str->size, strItem);
    str->size += strItemLength;
    STRING_LOG("[string_append]: Appended successfully, new size: %zu.\n", str->size);
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
    STRING_LOG("[string_push_back]: Pushing back character '%c' into the String object.\n", chItem);

    if (str == NULL) {
        STRING_LOG("[string_push_back]: Error - The String object is NULL.\n");
        return;
    }
    if (str->size + 1 >= str->capacitySize) {
        size_t newCapacity = str->capacitySize * 2;
        char* newData = (char*)memory_pool_allocate(str->pool, newCapacity);  // Allocate new space from the memory pool
        
        if (!newData) {
            STRING_LOG("[string_push_back]: Error - Memory allocation failed.\n");
            return;
        }

        // Copy existing string to the new space
        if (str->dataStr) { 
            memcpy(newData, str->dataStr, str->size);
        }
        str->dataStr = newData;
        str->capacitySize = newCapacity;
        STRING_LOG("[string_push_back]: Resized the string to new capacity: %zu.\n", newCapacity);
    }
    str->dataStr[str->size] = chItem;
    str->size++;
    str->dataStr[str->size] = '\0'; 

    STRING_LOG("[string_push_back]: Character added successfully, new size: %zu.\n", str->size);
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

    if (str == NULL) {
        STRING_LOG("[string_assign]: Error - The String object is NULL.\n");
        return;
    }
    if (newStr == NULL) {
        STRING_LOG("[string_assign]: Error - The newStr is NULL.\n");
        return;
    }

    size_t newStrLength = strlen(newStr);
    if (newStrLength + 1 > str->capacitySize) {
        char *newData = (char*)memory_pool_allocate(str->pool, newStrLength + 1);
        if (!newData) {
            STRING_LOG("[string_assign]: Error - Memory allocation failed.\n");
            return;
        }

        str->dataStr = newData;
        str->capacitySize = newStrLength + 1;
        STRING_LOG("[string_assign]: Resized the string to new capacity: %zu.\n", str->capacitySize);
    }

    strcpy(str->dataStr, newStr);
    str->size = newStrLength;

    STRING_LOG("[string_assign]: String assigned successfully, new size: %zu.\n", str->size);
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

    if (str == NULL) {
        STRING_LOG("[string_insert]: Error - The String object is NULL.\n");
        return;
    }
    if (strItem == NULL) {
        STRING_LOG("[string_insert]: Error - The strItem is NULL.\n");
        return;
    }
    if (pos > str->size) {
        STRING_LOG("[string_insert]: Error - Position out of bounds.\n");
        return;
    }

    size_t strItemLength = strlen(strItem);
    size_t newTotalLength = str->size + strItemLength;

    if (newTotalLength + 1 > str->capacitySize) {
        size_t newCapacity = newTotalLength + 1;
        char *newData =(char*)memory_pool_allocate(str->pool, newCapacity);
        if (!newData) {
            STRING_LOG("[string_insert]: Error - Memory allocation failed.\n");
            return;
        }

        memcpy(newData, str->dataStr, pos);
        memcpy(newData + pos + strItemLength, str->dataStr + pos, str->size - pos);
        str->dataStr = newData;
        str->capacitySize = newCapacity;
        STRING_LOG("[string_insert]: Resized the string to new capacity: %zu.\n", newCapacity);
    } 
    else { 
        memmove(str->dataStr + pos + strItemLength, str->dataStr + pos, str->size - pos);
    }

    memcpy(str->dataStr + pos, strItem, strItemLength);
    str->size = newTotalLength;
    STRING_LOG("[string_insert]: String inserted successfully, new size: %zu.\n", str->size);
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
    if (pos + len > str->size) { 
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

    if (str1 == NULL) {
        STRING_LOG("[string_replace]: Error - The String object (str1) is NULL.\n");
        return;
    }
    if (oldStr == NULL) {
        STRING_LOG("[string_replace]: Error - The oldStr is NULL.\n");
        return;
    }
    if (newStr == NULL) {
        STRING_LOG("[string_replace]: Error - The newStr is NULL.\n");
        return;
    }

    char *position = strstr(str1->dataStr, oldStr);
    if (position == NULL) { 
        STRING_LOG("[string_replace]: Warning - oldStr not found in str1.\n");
        return;  // oldStr not found in str1
    }

    size_t oldLen = strlen(oldStr);
    size_t newLen = strlen(newStr);
    size_t tailLen = strlen(position + oldLen);
    size_t newSize = (position - str1->dataStr) + newLen + tailLen;

    if (newSize + 1 > str1->capacitySize) {
        size_t newCapacity = newSize + 1;
        char *newData = (char*)memory_pool_allocate(str1->pool, newCapacity);
        if (!newData) {
            STRING_LOG("[string_replace]: Error - Memory allocation failed.\n");
            return;  // Handle allocation error
        }

        memcpy(newData, str1->dataStr, position - str1->dataStr);
        memcpy(newData + (position - str1->dataStr) + newLen, position + oldLen, tailLen);
        str1->dataStr = newData;
        str1->capacitySize = newCapacity;
        STRING_LOG("[string_replace]: Resized the string to new capacity: %zu.\n", newCapacity);
    } 
    else {
        memmove(position + newLen, position + oldLen, tailLen);
    }
    memcpy(position, newStr, newLen);
    str1->size = newSize;
    STRING_LOG("[string_replace]: Replacement successful, new size: %zu.\n", str1->size);
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

    if (str1 == NULL || str2 == NULL) {
        STRING_LOG("[string_swap]: Error - One or both String objects are NULL.\n");
        return;
    }

    String temp = *str1;
    *str1 = *str2;
    *str2 = temp;

    STRING_LOG("[string_swap]: Swap completed.\n");
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
    STRING_LOG("[string_pop_back]: Popping the last character.\n");

    if (str == NULL) {
        STRING_LOG("[string_pop_back]: Error - The String object is NULL.\n");
        return;
    }
    if (str->size == 0) {
        STRING_LOG("[string_pop_back]: Warning - Attempt to pop back from an empty string.\n");
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
    STRING_LOG("[string_deallocate]: Deallocating the String object.\n");

    if (str == NULL) {
        STRING_LOG("[string_deallocate]: Warning - Attempt to deallocate a NULL String object.\n");
        return;
    }
    if (str->pool != NULL) {
        memory_pool_destroy(str->pool);
        str->pool = NULL;
    }
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
char string_at(const String* str, size_t index) {
    STRING_LOG("[string_at]: Retrieving character at index %zu.\n", index);

    if (str == NULL) {
        STRING_LOG("[string_at]: Error - The String object is NULL.\n");
        return '\0';  // Return a default character
    }
    if (index >= str->size) {
        STRING_LOG("[string_at]: Error - Index out of range.\n");
        return '\0';  // Return a default character
    }
    return str->dataStr[index];
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
char* string_back(const String *str) {
    STRING_LOG("[string_back]: Accessing the last character.\n");

    if (str == NULL || str->size == 0) {
        STRING_LOG("[string_back]: Warning - The String object is NULL or empty.\n");
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
char* string_front(const String *str) {
    STRING_LOG("[string_front]: Retrieving the first character.\n");

    if (str == NULL || str->size == 0) {
        STRING_LOG("[string_front]: Warning - The String object is NULL or empty.\n");
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
size_t string_length(const String* str) {
    STRING_LOG("[string_length]: Retrieving the length of the String.\n");

    if (str == NULL) {
        STRING_LOG("[string_length]: Error - The String object is NULL.\n");
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
size_t string_capacity(const String* str) {
    STRING_LOG("[string_capacity]: Retrieving the capacity of the String.\n");

    if (str == NULL) {
        STRING_LOG("[string_capacity]: Error - The String object is NULL.\n");
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
size_t string_max_size(const String* str) {
    STRING_LOG("[string_max_size]: Retrieving the maximum possible size of the String.\n");

    if (str == NULL) {
        STRING_LOG("[string_max_size]: Error - The String object is NULL.\n");
        return 0;  // Or a special value indicating an error
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
    if (pos + len > str->size || len == 0) {
        copyLen = str->size - pos;  // Adjust copy length if it goes beyond the string end
    }

    strncpy(buffer, str->dataStr + pos, copyLen);
    buffer[copyLen] = '\0';  // Null-terminate the substring

    STRING_LOG("[string_copy]: Copied %zu characters.\n", copyLen);
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
int string_find(const String *str, const char *buffer, size_t pos) {
    STRING_LOG("[string_find]: Searching for substring starting from position %zu.\n", pos);

    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_find]: Error - The String object or its data is NULL.\n");
        return -1;
    }
    if (buffer == NULL) {
        STRING_LOG("[string_find]: Error - The buffer is NULL.\n");
        return -1;
    }
    if (pos >= str->size) {
        STRING_LOG("[string_find]: Error - Position %zu out of bounds. String size is %zu.\n", pos, str->size);
        return -1;
    }

    const char *found = strstr(str->dataStr + pos, buffer);
    if (found == NULL) { 
        STRING_LOG("[string_find]: Substring not found.\n");
        return -1;  // Substring not found
    }
    int foundPos = (int)(found - str->dataStr);
    STRING_LOG("[string_find]: Substring found at position %d.\n", foundPos);

    return foundPos;  // Return the position of the substring
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
    if (pos < bufferLen - 1) {
        STRING_LOG("[string_rfind]: Error - Position %zu is too small for the substring length %zu.\n", pos, bufferLen);
        return -1;
    }

    pos = (pos < str->size) ? pos : str->size - 1;  // Adjust pos to be within bounds
    STRING_LOG("[string_rfind]: Adjusted search position is %zu.\n", pos);

    for (int i = (int)pos; i >= 0; --i) {
        if (strncmp(str->dataStr + i, buffer, bufferLen) == 0) {
            STRING_LOG("[string_rfind]: Substring found at position %d.\n", i);
            return i;  // Found the substring
        }
    }
    STRING_LOG("[string_rfind]: Substring not found.\n");
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
int string_find_first_of(const String *str, const char *buffer, size_t pos) {
    STRING_LOG("[string_find_first_of]: Searching for first occurrence of any character from buffer starting from position %zu.\n", pos);

    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_find_first_of]: Error - The String object or its data is NULL.\n");
        return -1;
    }
    if (buffer == NULL) {
        STRING_LOG("[string_find_first_of]: Error - The buffer is NULL.\n");
        return -1;
    }
    if (pos >= str->size) {
        STRING_LOG("[string_find_first_of]: Error - Position %zu out of bounds. String size is %zu.\n", pos, str->size);
        return -1;
    }

    const char *found = strstr(str->dataStr + pos, buffer);
    if (found != NULL) {
        int foundPos = (int)(found - str->dataStr);
        STRING_LOG("[string_find_first_of]: Found at position %d.\n", foundPos);
        return foundPos;
    }
    STRING_LOG("[string_find_first_of]: No matching character found.\n");
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
int string_find_last_of(const String *str, const char *buffer, size_t pos) {
    STRING_LOG("[string_find_last_of]: Searching for last occurrence of substring up to position %zu.\n", pos);

    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_find_last_of]: Error - The String object or its data is NULL.\n");
        return -1;
    }
    if (buffer == NULL) {
        STRING_LOG("[string_find_last_of]: Error - The buffer is NULL.\n");
        return -1;
    }
    if (pos >= str->size) {
        STRING_LOG("[string_find_last_of]: Error - Position %zu out of bounds. String size is %zu.\n", pos, str->size);
        return -1;
    }

    int lastFound = -1;
    const char *currentFound = strstr(str->dataStr, buffer);

    while (currentFound != NULL && (size_t)(currentFound - str->dataStr) <= pos) {
        lastFound = (int)(currentFound - str->dataStr);
        currentFound = strstr(currentFound + 1, buffer);
    }
    
    STRING_LOG("[string_find_last_of]: Last occurrence found at position %d.\n", lastFound);
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
int string_find_first_not_of(const String *str, const char *buffer, size_t pos) {
    STRING_LOG("[string_find_first_not_of]: Searching for the first non-matching character from position %zu.\n", pos);

    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_find_first_not_of]: Error - The String object or its data is NULL.\n");
        return -1;
    }
    if (buffer == NULL) {
        STRING_LOG("[string_find_first_not_of]: Error - The buffer is NULL.\n");
        return -1;
    }
    if (pos >= str->size) {
        STRING_LOG("[string_find_first_not_of]: Error - Position %zu out of bounds. String size is %zu.\n", pos, str->size);
        return -1;
    }

    size_t bufferLen = strlen(buffer);
    if (bufferLen == 0) {
        STRING_LOG("[string_find_first_not_of]: The buffer is empty, returning position %zu.\n", pos);
        return (int)pos;  // If buffer is empty, return pos
    }

    for (size_t i = pos; i < str->size; ++i) {
        if (strchr(buffer, str->dataStr[i]) == NULL) { 
            STRING_LOG("[string_find_first_not_of]: First non-matching character found at position %zu.\n", i);
            return (int)i;
        }
    }

    STRING_LOG("[string_find_first_not_of]: No non-matching character found.\n");
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
    STRING_LOG("[string_find_last_not_of]: Searching for last non-matching character from position %zu.\n", pos);

    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_find_last_not_of]: Error - The String object or its data is NULL.\n");
        return -1;
    }
    if (buffer == NULL) {
        STRING_LOG("[string_find_last_not_of]: Error - The buffer is NULL.\n");
        return -1;
    }

    size_t bufferLen = strlen(buffer);
    if (bufferLen == 0) {
        STRING_LOG("[string_find_last_not_of]: Error - The buffer is empty.\n");
        return -1;
    }
    if (pos >= str->size) {
        pos = str->size - 1;
        STRING_LOG("[string_find_last_not_of]: Adjusted position to %zu.\n", pos);
    }

    for (int i = (int)pos; i >= 0; --i) {
        if (strchr(buffer, str->dataStr[i]) == NULL) {
            STRING_LOG("[string_find_last_not_of]: Last non-matching character found at position %d.\n", i);
            return i;
        }
    }

    STRING_LOG("[string_find_last_not_of]: No non-matching character found.\n");
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
const char *string_data(const String *str) {
    if (str == NULL || str->dataStr == NULL) {
        STRING_LOG("[string_data]: Error - Invalid input or uninitialized String.\n");
        return NULL;
    }
    STRING_LOG("[string_data]: Returning data string.\n");
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
        STRING_LOG("[string_c_str]: Error - Invalid input (NULL String).\n");
        return "";  
    }
    if (str->dataStr == NULL) {
        STRING_LOG("[string_c_str]: Error - Uninitialized String.\n");
        return "";  
    }
    STRING_LOG("[string_c_str]: Returning C-string.\n");
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
char *string_begin(const String *str) {
    if (str == NULL) { 
        STRING_LOG("[string_begin]: Error - Invalid input (NULL String).\n");
        return NULL; 
    }
    if (str->dataStr == NULL) {
        STRING_LOG("[string_begin]: Error - Uninitialized String.\n");
        return NULL; 
    }
    STRING_LOG("[string_begin]: Returning the beginning of the string.\n");
    return str->dataStr;  
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
char *string_end(const String *str) {
    if (str == NULL || str->dataStr == NULL) { 
        STRING_LOG("[string_end]: Error - Invalid input or uninitialized String.\n");
        return NULL;  // Return NULL for null or uninitialized String
    }
    STRING_LOG("[string_end]: Returning the end of the string.\n");
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
 * This is typically used in reverse iteration contexts.
 * If the input String is NULL or uninitialized, an error is logged, and the function returns NULL.
 *
 * @param str The String object for which to get the reverse end pointer. Must not be NULL.
 * 
 * @return A pointer to one before the first character in the string, or NULL on error.
 */
char *string_rend(const String *str) {
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
const char *string_cbegin(const String *str) {
    if (str == NULL || str->dataStr == NULL) { 
        STRING_LOG("[string_cbegin]: Error - Invalid input or uninitialized String.\n");
        return NULL; 
    }
    STRING_LOG("[string_cbegin]: Returning the beginning of the string.\n");
    return str->dataStr;  
}

/**
 * @brief This function returns a pointer to the position just after the last character of the string data.
 * If the input String is NULL or uninitialized, an error is logged, and the function returns NULL.
 *
 * @param str The String object for which to get the end pointer. Must not be NULL.
 * 
 * @return A constant pointer to the end of the string, or NULL on error.
 */
const char *string_cend(const String *str) {
    if (str == NULL || str->dataStr == NULL) { 
        STRING_LOG("[string_cend]: Error - Invalid input or uninitialized String.\n");
        return NULL;  
    }
    STRING_LOG("[string_cend]: Returning the end of the string.\n");
    return str->dataStr + str->size; 
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
    STRING_LOG("[string_crend]: Returning one before the first character.\n");
    return str->dataStr - 1;  
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
        if (str->dataStr != NULL) { 
            str->dataStr[0] = '\0';  // Set the first character to the null terminator
        }
        STRING_LOG("[string_clear]: String cleared successfully.\n");
    } 
    else {
        STRING_LOG("[string_clear]: Info - String object is NULL, nothing to clear.\n");
    }
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

    STRING_LOG("[string_set_pool_size]: Setting pool size to %zu.\n", newSize);

    // If a memory pool already exists, destroy it first
    if (str->pool) {
        STRING_LOG("[string_set_pool_size]: Destroying existing memory pool.\n");
        memory_pool_destroy(str->pool);
        str->pool = NULL;
    }

    // Create a new memory pool with the specified size
    str->pool = memory_pool_create(newSize);
    if (!str->pool) { 
        STRING_LOG("[string_set_pool_size]: Error - Failed to create new memory pool.\n");
        return false; 
    }

    STRING_LOG("[string_set_pool_size]: New memory pool created with size %zu.\n", newSize);

    // If the string already has data, reallocate it in the new pool
    if (str->size > 0 && str->dataStr) {
        STRING_LOG("[string_set_pool_size]: Reallocating string data in new memory pool.\n");
        char* newData = (char*)memory_pool_allocate(str->pool, str->size + 1); // +1 for null terminator
        if (!newData) {
            STRING_LOG("[string_set_pool_size]: Error - Failed to allocate memory for string data in new pool.\n");
            memory_pool_destroy(str->pool);
            str->pool = NULL;
            return false; 
        }
        memcpy(newData, str->dataStr, str->size);
        newData[str->size] = '\0';
        str->dataStr = newData;
    }
    STRING_LOG("[string_set_pool_size]: Successfully resized pool.\n");
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
        STRING_LOG("[string_concatenate]: Error - 'str1' is NULL.\n");
        return;
    }
    if (str2 == NULL) {
        STRING_LOG("[string_concatenate]: Error - 'str2' is NULL.\n");
        return;
    }

    STRING_LOG("[string_concatenate]: Concatenating strings.\n");
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
        STRING_LOG("[string_trim_left]: Error - String object is NULL.\n");
        return;
    }
    if (str->size == 0) {
        STRING_LOG("[string_trim_left]: String is empty, no trimming needed.\n");
        return;
    }

    size_t i = 0;
    while (i < str->size && isspace((unsigned char)str->dataStr[i])) {
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
    while (i > 0 && isspace((unsigned char)str->dataStr[i - 1])) {
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
    
    if (str == NULL) {
        STRING_LOG("[string_trim]: Error - Null String object.");
        return;
    }

    STRING_LOG("[string_trim]: Calling string_trim_left.");
    string_trim_left(str);
    
    STRING_LOG("[string_trim]: Calling string_trim_right.");
    string_trim_right(str);

    STRING_LOG("[string_trim]: Function end.");
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
    
    if (str == NULL) {
        STRING_LOG("[string_split]: Error - Null String object.");
        return NULL;
    }
    if (delimiter == NULL) {
        STRING_LOG("[string_split]: Error - Null delimiter.");
        return NULL;
    }

    size_t num_splits = 0;
    char *temp = string_strdup(str->dataStr);
    if (temp == NULL) {
        STRING_LOG("[string_split]: Error - Memory allocation failed.");
        return NULL;
    }

    char *token = strtok(temp, delimiter);

    while (token != NULL) {
        num_splits++;
        token = strtok(NULL, delimiter);
    }
    free(temp);

    if (num_splits == 0) {
        STRING_LOG("[string_split]: No splits found.");
        return NULL;
    }

    String **splits = (String**)malloc(sizeof(String*) * num_splits);
    if (splits == NULL) {
        STRING_LOG("[string_split]: Error - Memory allocation failed for splits array.");
        return NULL;
    }

    temp = string_strdup(str->dataStr);
    if (temp == NULL) {
        STRING_LOG("[string_split]: Error - Memory allocation failed.");
        free(splits);
        return NULL;
    }

    token = strtok(temp, delimiter);
    size_t index = 0;

    while (token != NULL && index < num_splits) {
        splits[index] = string_create(token);
        if (splits[index] == NULL) {
            STRING_LOG("[string_split]: Error - Failed to create string at index %zu.", index);
            // Free previously allocated strings and array
            for (size_t i = 0; i < index; i++) {
                string_deallocate(splits[i]); // Assuming string_deallocate is defined
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

    STRING_LOG("[string_split]: Successfully split into %d parts.", *count);
    STRING_LOG("[string_split]: Function end.");

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
    STRING_LOG("[string_join]: Function start.");

    if (strings == NULL) {
        STRING_LOG("[string_join]: Error - Null string array.");
        return NULL;
    }
    if (count <= 0) {
        STRING_LOG("[string_join]: Error - Invalid count.");
        return NULL;
    }
    if (delimiter == NULL) {
        STRING_LOG("[string_join]: Error - Null delimiter.");
        return NULL;
    }

    String *result = string_create("");
    if (result == NULL) {
        STRING_LOG("[string_join]: Error - Memory allocation failed.");
        return NULL;
    }

    for (int i = 0; i < count; i++) {
        STRING_LOG("[string_join]: Appending string at index %d.", i);
        string_append(result, strings[i]->dataStr);
        if (i < count - 1) {
            string_append(result, delimiter);
        }
    }

    STRING_LOG("[string_join]: Successfully joined %d strings.", count);
    STRING_LOG("[string_join]: Function end.");

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

    if (str == NULL) {
        STRING_LOG("[string_replace_all]: Error - Null String object.");
        return;
    }
    if (oldStr == NULL || newStr == NULL) {
        STRING_LOG("[string_replace_all]: Error - Null substring.");
        return;
    }

    String *temp = string_create("");
    if (temp == NULL) {
        STRING_LOG("[string_replace_all]: Error - Memory allocation failed.");
        return;
    }

    char *start = str->dataStr;
    char *end;

    while ((end = strstr(start, oldStr)) != NULL) {
        *end = '\0';
        STRING_LOG("[string_replace_all]: Replacing '%s' with '%s'.", oldStr, newStr);
        string_append(temp, start);
        string_append(temp, newStr);
        start = end + strlen(oldStr);
    }

    string_append(temp, start);
    string_assign(str, temp->dataStr);
    string_deallocate(temp);

    STRING_LOG("[string_replace_all]: Replacement completed.");
    STRING_LOG("[string_replace_all]: Function end.");
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

    float result = atof(str->dataStr);
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
    STRING_LOG("[string_pad_left]: Function start. Padding left with character '%c' to total length %zu.", padChar, totalLength);

    if (str == NULL) {
        STRING_LOG("[string_pad_left]: Error - Null String object.");
        return;
    }
    if (str->size >= totalLength) {
        STRING_LOG("[string_pad_left]: Error - String size (%zu) is greater than or equal to total length (%zu).", str->size, totalLength);
        return;
    }
    
    size_t padSize = totalLength - str->size;
    size_t newSize = str->size + padSize;
    char *newData = (char *)malloc(newSize + 1); // +1 for null terminator

    if (newData == NULL) {
        STRING_LOG("[string_pad_left]: Error - Failed to allocate memory.");
        return;
    }

    memset(newData, padChar, padSize);
    memcpy(newData + padSize, str->dataStr, str->size);
    newData[newSize] = '\0';

    free(str->dataStr);
    str->dataStr = newData;
    str->size = newSize;
    str->capacitySize = newSize + 1;

    STRING_LOG("[string_pad_left]: Padding completed. New size: %zu.", str->size);
    STRING_LOG("[string_pad_left]: Function end.");
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
    STRING_LOG("[string_pad_right]: Function start. Padding right with character '%c' to total length %zu.", padChar, totalLength);

    if (str == NULL) {
        STRING_LOG("[string_pad_right]: Error - Null String object.");
        return;
    }
    if (str->size >= totalLength) {
        STRING_LOG("[string_pad_right]: Error - String size (%zu) is greater than or equal to total length (%zu).", str->size, totalLength);
        return;
    }

    size_t padSize = totalLength - str->size;
    size_t newSize = str->size + padSize;
    char *newData = (char *)realloc(str->dataStr, newSize + 1); // +1 for null terminator

    if (newData == NULL) {
        STRING_LOG("[string_pad_right]: Error - Failed to allocate memory.");
        return;
    }

    memset(newData + str->size, padChar, padSize);
    newData[newSize] = '\0';

    str->dataStr = newData;
    str->size = newSize;
    str->capacitySize = newSize + 1;

    STRING_LOG("[string_pad_right]: Padding completed. New size: %zu.", str->size);
    STRING_LOG("[string_pad_right]: Function end.");
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
    STRING_LOG("[string_to_hex]: Function start. Converting String to hexadecimal.");

    if (str == NULL) {
        STRING_LOG("[string_to_hex]: Error - Null String object.");
        return NULL;
    }
    if (string_empty(str)) {
        STRING_LOG("[string_to_hex]: String is empty, returning empty String.");
        return string_create(""); // Return an empty string for an empty input.
    }

    String *hexStr = string_create("");
    if (hexStr == NULL) {
        STRING_LOG("[string_to_hex]: Error - Memory allocation failed.");
        return NULL;
    }

    for (size_t i = 0; i < str->size; ++i) {
        char buffer[3];  // Each char can be represented by max 2 hex digits + null terminator
        sprintf(buffer, "%02x", (unsigned char)str->dataStr[i]);
        string_append(hexStr, buffer);
    }

    STRING_LOG("[string_to_hex]: Conversion to hexadecimal completed. Hex String size: %zu.", hexStr->size);
    STRING_LOG("[string_to_hex]: Function end.");
    
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
String* string_from_hex(const String *hexStr) {
    STRING_LOG("[string_from_hex]: Function start.");
    
    if (hexStr == NULL) {
        STRING_LOG("[string_from_hex]: Error - Null String object.");
        return NULL;
    }
    if (string_empty(hexStr) || (hexStr->size % 2) != 0) {
        STRING_LOG("[string_from_hex]: Error - Invalid hex string size (%zu).", hexStr->size);
        return NULL; // Hex string should have an even number of characters
    }

    String *str = string_create("");
    if (str == NULL) {
        STRING_LOG("[string_from_hex]: Error - Memory allocation failed.");
        return NULL;
    }

    for (size_t i = 0; i < hexStr->size; i += 2) {
        char buffer[3] = {hexStr->dataStr[i], hexStr->dataStr[i + 1], '\0'};
        char ch = (char)strtol(buffer, NULL, 16);
        string_push_back(str, ch);
    }

    STRING_LOG("[string_from_hex]: Conversion completed. New String size: %zu.", str->size);
    STRING_LOG("[string_from_hex]: Function end.");

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

    if (str == NULL) {
        STRING_LOG("[string_count]: Error - Null String object.");
        return 0;
    }
    if (substr == NULL) {
        STRING_LOG("[string_count]: Error - Null substring.");
        return 0;
    }
    if (str->dataStr == NULL) {
        STRING_LOG("[string_count]: Error - Null data string in String object.");
        return 0;
    }

    size_t count = 0;
    const char* temp = str->dataStr;
    const char* found;

    while ((found = strstr(temp, substr)) != NULL) {
        count++;
        temp = found + strlen(substr);
    }

    STRING_LOG("[string_count]: Substring '%s' found %zu times.", substr, count);
    STRING_LOG("[string_count]: Function end.");

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

    if (str == NULL) {
        STRING_LOG("[string_remove]: Error - Null String object.");
        return;
    }
    if (substr == NULL) {
        STRING_LOG("[string_remove]: Error - Null substring.");
        return;
    }
    if (str->dataStr == NULL) {
        STRING_LOG("[string_remove]: Error - Null data string in String object.");
        return;
    }
    if (strlen(substr) == 0) {
        STRING_LOG("[string_remove]: Error - Empty substring.");
        return;
    }

    size_t len = strlen(substr);
    char* p = str->dataStr;

    while ((p = strstr(p, substr)) != NULL) {
        STRING_LOG("[string_remove]: Removing occurrence of substring '%s' at position %zu.", substr, p - str->dataStr);
        memmove(p, p + len, strlen(p + len) + 1);
    }

    STRING_LOG("[string_remove]: Function end. New String size: %zu.", str->size);
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
    STRING_LOG("[string_from_int]: Function start. Converting int %d to String.", value);

    char buffer[12]; 
    sprintf(buffer, "%d", value);

    String* result = string_create(buffer);
    if (result) {
        STRING_LOG("[string_from_int]: Successfully created String from int.");
    } 
    else {
        STRING_LOG("[string_from_int]: Error - Memory allocation failed.");
    }

    STRING_LOG("[string_from_int]: Function end.");
    return result;
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
    STRING_LOG("[string_from_int_cstr]: Function start. Converting int %d to C-string.", value);

    char buffer[12];
    sprintf(buffer, "%d", value);

    char* result = (char*)malloc(strlen(buffer) + 1); // +1 for null-terminator
    if (result) {
        strcpy(result, buffer);
        STRING_LOG("[string_from_int_cstr]: Successfully created C-string from int.");
    } 
    else {
        STRING_LOG("[string_from_int_cstr]: Error - Memory allocation failed.");
    }

    STRING_LOG("[string_from_int_cstr]: Function end.");
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
    STRING_LOG("[string_from_float]: Function start. Converting float %f to String.", value);

    char buffer[32]; 
    sprintf(buffer, "%f", value);

    String* result = string_create(buffer);
    if (result) {
        STRING_LOG("[string_from_float]: Successfully created String from float.");
    } 
    else {
        STRING_LOG("[string_from_float]: Error - Memory allocation failed.");
    }

    STRING_LOG("[string_from_float]: Function end.");
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
String* string_from_double(double value) {
    STRING_LOG("[string_from_double]: Function start. Converting double %f to String.", value);

    char buffer[32]; 
    snprintf(buffer, sizeof(buffer), "%f", value);

    String* result = string_create(buffer);
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

    if (str == NULL || delimiters == NULL) {
        STRING_LOG("[string_tokenize]: Error - Invalid input (str or delimiters is NULL).");
        return NULL;
    }

    size_t num_tokens = 0;
    char* temp_str = string_strdup(str->dataStr); // strdup
    if (temp_str == NULL) {
        STRING_LOG("[string_tokenize]: Error - Memory allocation failed for temp_str.");
        return NULL;
    }

    char* token = strtok(temp_str, delimiters);
    while (token != NULL) {
        num_tokens++;
        token = strtok(NULL, delimiters);
    }
    free(temp_str);

    STRING_LOG("[string_tokenize]: Number of tokens found: %zu.", num_tokens);

    // Allocate array of String pointers
    String** tokens = (String**)malloc(num_tokens * sizeof(String*));
    if (tokens == NULL) {
        STRING_LOG("[string_tokenize]: Error - Memory allocation failed for tokens array.");
        return NULL;
    }

    // Tokenize again to fill the array
    temp_str = string_strdup(str->dataStr);
    if (temp_str == NULL) {
        STRING_LOG("[string_tokenize]: Error - Memory allocation failed for temp_str (second pass).");
        free(tokens);
        return NULL;
    }

    token = strtok(temp_str, delimiters);
    size_t idx = 0;

    while (token != NULL && idx < num_tokens) {
        tokens[idx] = string_create(token);
        if (tokens[idx] == NULL) {
            STRING_LOG("[string_tokenize]: Error - Failed to create token string at index %zu.", idx);
            for (size_t i = 0; i < idx; ++i) {
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

    STRING_LOG("[string_tokenize]: Successfully tokenized string into %d parts.", *count);
    STRING_LOG("[string_tokenize]: Function end.");

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
        STRING_LOG("[string_compare_ignore_case]: One or both strings are NULL. str1: %p, str2: %p", (void*)str1, (void*)str2);
        if (str1 == str2) {
            return 0;
        }
        return (str1 == NULL) ? -1 : 1;
    }

    if (str1->dataStr == NULL || str2->dataStr == NULL) {
        STRING_LOG("[string_compare_ignore_case]: One or both string data pointers are NULL. str1 data: %p, str2 data: %p", (void*)str1->dataStr, (void*)str2->dataStr);
        if (str1->dataStr == str2->dataStr) {
            return 0; // Both dataStr are NULL, considered equal
        }
        return (str1->dataStr == NULL) ? -1 : 1; // One dataStr is NULL, the other is not
    }

    int result = strcasecmp(str1->dataStr, str2->dataStr);
    STRING_LOG("[string_compare_ignore_case]: Comparison result: %d", result);
    STRING_LOG("[string_compare_ignore_case]: Function end.");

    return result;
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
    
    if (input == NULL) {
        STRING_LOG("[string_base64_encode]: Error - The String object is NULL.");
        return NULL;
    }
    if (input->dataStr == NULL) {
        STRING_LOG("[string_base64_encode]: Error - The dataStr of String object is NULL.");
        return NULL;
    }

    String *encoded = string_create("");
    if (encoded == NULL) {
        STRING_LOG("[string_base64_encode]: Error - Failed to create encoded String object.");
        return NULL;
    }

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

    STRING_LOG("[string_base64_encode]: Successfully encoded input.");
    STRING_LOG("[string_base64_encode]: Function end.");
    
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

        val = (val << 6) | c;
        valb += 6;
        if (valb >= 0) {
            decodedStr[j++] = (char)((val >> valb) & 0xFF);
            valb -= 8;
        }
    }

    decodedStr[j] = '\0';

    String* decodedStringObject = string_create(decodedStr);
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

    if (str == NULL) {
        STRING_LOG("[string_format]: Error - The String object is NULL.");
        return;
    }
    if (format == NULL) {
        STRING_LOG("[string_format]: Error - The format string is NULL.");
        return;
    }

    va_list args;
    va_start(args, format);

    // Calculate the required length for the formatted string
    va_list args_copy;
    va_copy(args_copy, args);
    int length = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (length < 0) {
        STRING_LOG("[string_format]: Error - vsnprintf failed.");
        va_end(args);
        return;
    }

    // Allocate memory for the formatted string
    char* buffer = (char*)malloc(length + 1);
    if (buffer == NULL) {
        STRING_LOG("[string_format]: Error - Memory allocation failed.");
        va_end(args);
        return;
    }

    // Format the string
    vsnprintf(buffer, length + 1, format, args);

    // Assign the formatted string to the String object
    string_assign(str, buffer);

    // Clean up
    free(buffer);
    va_end(args);

    STRING_LOG("[string_format]: Successfully formatted the string.");
    STRING_LOG("[string_format]: Function end.");
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

    if (str == NULL) {
        STRING_LOG("[string_repeat]: Error - The String object is NULL.");
        return NULL;
    }
    if (str->dataStr == NULL) {
        STRING_LOG("[string_repeat]: Error - The dataStr of String object is NULL.");
        return NULL;
    }

    size_t newLength = str->size * count;
    char* repeatedStr = (char*)malloc(newLength + 1);
    if (repeatedStr == NULL) {
        STRING_LOG("[string_repeat]: Error - Memory allocation failed.");
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

    STRING_LOG("[string_repeat]: Successfully created the repeated string.");
    STRING_LOG("[string_repeat]: Function end.");

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
        String* str = va_arg(args, String*);
        totalLength += str->size;
    }
    va_end(args);

    char* joinedStr = (char*)malloc(totalLength + 1);
    if (joinedStr == NULL) {
        STRING_LOG("[string_join_variadic]: Error - Memory allocation failed.");
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

    STRING_LOG("[string_join_variadic]: Successfully joined the strings.");
    STRING_LOG("[string_join_variadic]: Function end.");

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

    srand(time(NULL));
    size_t length = strlen(str->dataStr);
    for (size_t i = length - 1; i > 0; i--) {
        size_t j = rand() % (i + 1);

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

    str->dataStr[0] = toupper(str->dataStr[0]);

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
        str->dataStr[i] = tolower(str->dataStr[i]);
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

    bool result = strncmp(str->dataStr, substr, substrLen) == 0;
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

    bool result = strncmp(str->dataStr + strLen - substrLen, substr, substrLen) == 0;
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
        if (islower(str->dataStr[i])) {
            str->dataStr[i] = toupper(str->dataStr[i]);
        }
        else if (isupper(str->dataStr[i])) {
            str->dataStr[i] = tolower(str->dataStr[i]);
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

    size_t len = mbstowcs(NULL, str, 0) + 1;
    if (len == (size_t)-1) {
        STRING_LOG("[string_to_unicode]: Error - Conversion failed.");
        return NULL;
    }

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
    String* stringObj = string_create(str);
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
    STRING_LOG("[string_strdup]: Function start.");

    if (str == NULL) {
        STRING_LOG("[string_strdup]: Error - Parameter 'str' is NULL.");
        return NULL;
    }

    char* new_str = (char*)malloc(strlen(str) + 1);
    if (new_str == NULL) {
        STRING_LOG("[string_strdup]: Error - Memory allocation failed for string: %s.", str);
        return NULL;
    }

    strcpy(new_str, str);
    STRING_LOG("[string_strdup]: Successfully duplicated string: %s.", str);

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
    
    size_t len = strlen(str);
    if (len > n) { 
        len = n;
    }

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
    if (!str) {
        return;
    } 

    if (newCapacity > str->capacitySize) {
        memory_pool_destroy(str->pool);
        str->capacitySize = newCapacity + 32;  
        str->pool = memory_pool_create(str->capacitySize);
        str->dataStr = (char*)memory_pool_allocate(str->pool, str->capacitySize);

        if (!str->dataStr) {
            STRING_LOG("[string_reserve]: Memory allocation failed.");
            exit(-1);  
        }

        STRING_LOG("[string_reserve]: Resized string capacity to %zu.", str->capacitySize);
    }
}
