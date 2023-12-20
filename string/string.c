#include "string.h"
#include <stdlib.h>
#include <string.h>


static MemoryPoolString *memory_pool_create(size_t size);
static void *memory_pool_allocate(MemoryPoolString *pool, size_t size);
static void memory_pool_destroy(MemoryPoolString *pool);
bool memoryPoolCreated = false;
MemoryPoolString* global_pool = NULL;

static void init_global_memory_pool(size_t size) 
{
    if (global_pool == NULL) 
    {
        global_pool = memory_pool_create(size);
        memoryPoolCreated = true;
    }   
}

static void destroy_global_memory_pool() 
{
    if (global_pool != NULL && memoryPoolCreated) 
    {
        memory_pool_destroy(global_pool);
        global_pool = NULL;
    }
}

static MemoryPoolString *memory_pool_create(size_t size) 
{
    MemoryPoolString *pool = malloc(sizeof(MemoryPoolString));
    if (pool) 
    {
        pool->pool = malloc(size);
        if (!pool->pool) 
        {
            free(pool);
            return NULL;
        }
        pool->poolSize = size;
        pool->used = 0;
    }
    return pool;
}

static void *memory_pool_allocate(MemoryPoolString *pool, size_t size) 
{
    if (pool->used + size > pool->poolSize) 
        return NULL; // Pool is out of memory
    
    void *mem = (char *)pool->pool + pool->used;
    pool->used += size;

    return mem;
}

static void memory_pool_destroy(MemoryPoolString *pool) 
{
    if (pool) 
    {
        free(pool->pool);
        free(pool);
    }
}

String* string_create(const char* initialStr) 
{
    String* str = (String*)malloc(sizeof(String));
    if (!str) 
        return NULL;

    size_t initialSize = initialStr ? strlen(initialStr) : 0;
    str->size = initialSize;
    str->capacitySize = initialSize + 1; // +1 for null terminator

    // Initialize memory pool for strings with a smaller size
    size_t initialPoolSize = str->capacitySize * 2; // 1KB
    str->pool = memory_pool_create(initialPoolSize);
    if (!str->pool) {
        free(str);
        return NULL;
    }

    str->dataStr = memory_pool_allocate(str->pool, str->capacitySize);
    if (!str->dataStr) {
        memory_pool_destroy(str->pool);
        free(str);
        return NULL;
    }

    if (initialStr) 
        strcpy(str->dataStr, initialStr);

    return str;
}

String* string_create_with_pool(size_t size) 
{
    static int counter = 0;

    if (!counter)
    {
        init_global_memory_pool(size);
        counter++;
    }
    // Ensure global memory pool is initialized
    if (global_pool == NULL) 
    {
        perror("Global memory pool not initialized");
        exit(EXIT_FAILURE);
    }

    String* str = (String*)malloc(sizeof(String));
    if (!str) 
    return NULL;

    str->size = 0;
    str->capacitySize = 1;
    str->dataStr = NULL; // Data is not allocated yet (lazy allocation)
    str->pool = global_pool; // Use the global pool

    return str;
}

String* string_substr(String* str, size_t pos, size_t len) 
{
    if (str == NULL || pos >= str->size) 
        return NULL;
    
    // Adjust len if it goes beyond the end of the string
    if (pos + len > str->size) 
        len = str->size - pos;
    
    String* substr = string_create(NULL); // Allocate memory for the substring
    if (substr == NULL) 
        return NULL;
    
    substr->size = len;
    substr->capacitySize = len + 1;
    substr->dataStr = (char*)malloc(substr->capacitySize * sizeof(char));

    if (substr->dataStr == NULL) 
    {
        free(substr);
        return NULL;
    }

    strncpy(substr->dataStr, str->dataStr + pos, len); // Copy the substring
    substr->dataStr[len] = '\0';  // Null terminate the substring

    return substr;
}

bool string_empty(String* str) 
{
    return (str == NULL) ? true : (str->size == 0);
}

int string_compare(String* str1, String* str2) 
{
    if (str1 == NULL || str2 == NULL) 
    {
        if (str1 == str2) 
            return 0;  // Both are NULL, considered equal
        
        return (str1 == NULL) ? -1 : 1;  // NULL is considered less than non-NULL
    }

    return strcmp(str1->dataStr, str2->dataStr);
}

bool string_is_equal(String* str1, String* str2) 
{
    return string_compare(str1, str2) == 0;
}

bool string_is_less(String* str1, String* str2) 
{
    return string_compare(str1, str2) < 0;
}

bool string_is_greater(String* str1, String* str2) 
{
    return string_compare(str1, str2) > 0;
}

bool string_is_less_or_equal(String* str1, String* str2) 
{
    return string_compare(str1, str2) <= 0;
}

bool string_is_greater_or_equal(String* str1, String* str2) 
{
    return string_compare(str1, str2) >= 0;
}

bool string_is_not_equal(String* str1, String* str2) 
{
    return string_compare(str1, str2) != 0;
}

void string_resize(String *str, size_t newSize) 
{
    if (str == NULL) 
        return;
    
    if (newSize < str->size) 
    {
        str->size = newSize;
        str->dataStr[newSize] = '\0';
    } 
    else if (newSize > str->size) 
    {
        if (newSize >= str->capacitySize) 
        {
            size_t newCapacity = newSize + 1;
            char *newData = memory_pool_allocate(str->pool, newCapacity);

            if (!newData) 
                return;  // Handle allocation error
            
            memcpy(newData, str->dataStr, str->size);
            str->dataStr = newData;
            str->capacitySize = newCapacity;
        }

        memset(str->dataStr + str->size, '\0', newSize - str->size);
        str->size = newSize;
    }
}

void string_shrink_to_fit(String *str) 
{
    if (str == NULL || str->size + 1 == str->capacitySize) 
        return; // No need to shrink if already at optimal size

    if (str->dataStr != NULL)  // Check if the string is using the memory pool
    {
        // Allocate new space from the memory pool
        size_t newCapacity = str->size + 1; // +1 for null terminator
        char *newData = memory_pool_allocate(str->pool, newCapacity);

        if (newData == NULL) 
            return; // Handle allocation error or memory pool limit

        // Copy existing data to the new space
        memcpy(newData, str->dataStr, str->size);
        newData[str->size] = '\0'; // Null-terminate the string

        // Update the string's metadata
        str->dataStr = newData;
        str->capacitySize = newCapacity;
    }
}

void string_append(String *str, const char *strItem) 
{
    if (str == NULL || strItem == NULL) 
        return;

    size_t strItemLength = strlen(strItem);
    if (strItemLength == 0) 
        return;

    if (str->size + strItemLength >= str->capacitySize) 
    {
        size_t newCapacity = str->size + strItemLength + 1;
        char *newData = memory_pool_allocate(str->pool, newCapacity);

        if (!newData) 
            return;  // Handle allocation error

        memcpy(newData, str->dataStr, str->size);
        str->dataStr = newData;
        str->capacitySize = newCapacity;
    }

    strcpy(str->dataStr + str->size, strItem);
    str->size += strItemLength;
}

void string_push_back(String* str, char chItem) 
{
    if (!str) 
        return; 

    if (str->size + 1 >= str->capacitySize) 
    {
        // static int counter = 0;
        size_t newCapacity = str->capacitySize * 2;
        // printf("%zu\n%zu\n-----\n", str->capacitySize, newCapacity);
        char* newData = memory_pool_allocate(str->pool, newCapacity);  // Allocate new space from the memory pool
        // counter++;
        if (!newData) 
        {
            // printf("%zu\n%zu\n%d\n", str->capacitySize, newCapacity, counter);
            perror("Allocation failed in string_push_back_impl");
            exit(-1);
        }

        // Copy existing string to the new space
        if (str->dataStr) 
            memcpy(newData, str->dataStr, str->size);

        str->dataStr = newData;
        str->capacitySize = newCapacity;
    }

    // Append the character
    str->dataStr[str->size] = chItem;
    str->size++;
    str->dataStr[str->size] = '\0'; // Null-terminate the string
}

void string_assign(String *str, const char *newStr) 
{
    if (str == NULL || newStr == NULL) 
        return;
    
    size_t newStrLength = strlen(newStr);
    if (newStrLength + 1 > str->capacitySize) 
    {
        char *newData = memory_pool_allocate(str->pool, newStrLength + 1);
        if (!newData) 
            return;  // Handle allocation error
        
        str->dataStr = newData;
        str->capacitySize = newStrLength + 1;
    }

    strcpy(str->dataStr, newStr);
    str->size = newStrLength;
}


void string_insert(String *str, size_t pos, const char *strItem) 
{
    if (str == NULL || strItem == NULL || pos > str->size) 
        return;
    
    size_t strItemLength = strlen(strItem);
    size_t newTotalLength = str->size + strItemLength;

    if (newTotalLength + 1 > str->capacitySize) 
    {
        size_t newCapacity = newTotalLength + 1;
        char *newData = memory_pool_allocate(str->pool, newCapacity);
        if (!newData) 
            return;  // Handle allocation error
        
        memcpy(newData, str->dataStr, pos);
        memcpy(newData + pos + strItemLength, str->dataStr + pos, str->size - pos);
        str->dataStr = newData;
        str->capacitySize = newCapacity;
    } 
    else 
        memmove(str->dataStr + pos + strItemLength, str->dataStr + pos, str->size - pos);
    
    memcpy(str->dataStr + pos, strItem, strItemLength);
    str->size = newTotalLength;
}

void string_erase(String *str, size_t pos, size_t len) 
{
    if (str == NULL || pos >= str->size) 
        return;

    if (pos + len > str->size) 
        len = str->size - pos;  // Adjust len to not go beyond the string end
    
    memmove(str->dataStr + pos, str->dataStr + pos + len, str->size - pos - len + 1);
    str->size -= len;
}

void string_replace(String *str1, const char *oldStr, const char *newStr) 
{
    if (str1 == NULL || oldStr == NULL || newStr == NULL) 
        return;
    
    char *position = strstr(str1->dataStr, oldStr);
    if (position == NULL) 
        return;  // oldStr not found in str1

    size_t oldLen = strlen(oldStr);
    size_t newLen = strlen(newStr);
    size_t tailLen = strlen(position + oldLen);
    size_t newSize = (position - str1->dataStr) + newLen + tailLen;

    if (newSize + 1 > str1->capacitySize) 
    {
        size_t newCapacity = newSize + 1;
        char *newData = memory_pool_allocate(str1->pool, newCapacity);
        if (!newData) 
            return;  // Handle allocation error
        
        memcpy(newData, str1->dataStr, position - str1->dataStr);
        memcpy(newData + (position - str1->dataStr) + newLen, position + oldLen, tailLen);
        str1->dataStr = newData;
        str1->capacitySize = newCapacity;
    } 
    else 
        memmove(position + newLen, position + oldLen, tailLen);

    memcpy(position, newStr, newLen);
    str1->size = newSize;
}

void string_swap(String *str1, String *str2) 
{
    if (str1 == NULL || str2 == NULL) 
        return;
    
    String temp = *str1;
    *str1 = *str2;
    *str2 = temp;
}

void string_pop_back(String *str) 
{
    if (str == NULL || str->size == 0) 
        return;
    
    str->dataStr[str->size - 1] = '\0';
    str->size--;
}

void string_deallocate(String *str) 
{
    if (str == NULL) 
        return;

    // Destroy the memory pool associated with the string
    if (str->pool != NULL) 
    {
        memory_pool_destroy(str->pool);
        str->pool = NULL;
    }

    // Since dataStr is managed by the memory pool, no separate free call is needed for it
    free(str);
    if (memoryPoolCreated)
        destroy_global_memory_pool();
}

char string_at(String* str, size_t index)
{
    if (str == NULL || index >= str->size) 
    {
        perror("Index out of the range");
        exit(-1);
    }

    return (const char)str->dataStr[index];
}

char* string_back(String *str) 
{
    if (str == NULL || str->size == 0) 
        return NULL; 
    
    return &str->dataStr[str->size - 1];
}

char* string_front(String *str) 
{
    if (str == NULL || str->size == 0) 
        return NULL;  
    
    return &str->dataStr[0];
}

size_t string_length(String* str) 
{
    return (str != NULL) ? str->size : 0;
}

size_t string_capacity(String* str) 
{
    return (str != NULL) ? str->capacitySize : 0;
}

size_t string_max_size(String* str) 
{
    if (!str)
    {
        perror("Object is null");
        exit(-1);
    }
    return (size_t)-1;
}

size_t string_copy(String *str, char *buffer, size_t pos, size_t len) 
{
    if (str == NULL || str->dataStr == NULL || buffer == NULL || pos >= str->size) 
        return 0;  // Return 0 for invalid input or position out of bounds
    
    size_t copyLen = len;

    if (pos + len > str->size || len == 0) 
        copyLen = str->size - pos;  // Adjust copy length if it goes beyond the string end
    
    strncpy(buffer, str->dataStr + pos, copyLen);
    buffer[copyLen] = '\0';  

    return copyLen;  // Return the number of characters copied
}

int string_find(String *str, const char *buffer, size_t pos) 
{
    if (str == NULL || str->dataStr == NULL || buffer == NULL || pos >= str->size) 
        return -1;  // Return -1 for invalid input or position out of bounds
    
    const char *found = strstr(str->dataStr + pos, buffer);
    if (found == NULL) 
        return -1;  // Substring not found
    
    return (int)(found - str->dataStr);  // Return the position of the substring
}

int string_rfind(String *str, const char *buffer, size_t pos) 
{
    if (str == NULL || str->dataStr == NULL || buffer == NULL) 
        return -1;  // Return -1 for invalid input
    
    size_t bufferLen = strlen(buffer);
    if (bufferLen == 0 || pos < bufferLen - 1) 
        return -1;  // Return -1 if buffer is empty or pos is too small
    
    pos = (pos < str->size) ? pos : str->size - 1;  // Adjust pos to be within bounds

    for (int i = (int)pos; i >= 0; --i) 
        if (strncmp(str->dataStr + i, buffer, bufferLen) == 0) 
            return i;  // Found the substring
        
    return -1;  // Substring not found
}

int string_find_first_of(String *str, const char *buffer, size_t pos) 
{
    if (str == NULL || str->dataStr == NULL || buffer == NULL || pos >= str->size) 
        return -1;  // Return -1 for invalid input or position out of bounds
    
    const char *found = strstr(str->dataStr + pos, buffer);

    if (found != NULL) 
        return (int)(found - str->dataStr);
    
    return -1;  // Buffer string not found
}


int string_find_last_of(String *str, const char *buffer, size_t pos) 
{
    if (str == NULL || str->dataStr == NULL || buffer == NULL || pos >= str->size) 
        return -1;  // Return -1 for invalid input or position out of bounds
    
    int lastFound = -1;
    const char *currentFound = strstr(str->dataStr, buffer);

    while (currentFound != NULL && (size_t)(currentFound - str->dataStr) <= pos) 
    {
        lastFound = (int)(currentFound - str->dataStr);
        currentFound = strstr(currentFound + 1, buffer);
    }

    return lastFound;
}

int string_find_first_not_of(String *str, const char *buffer, size_t pos) 
{
    if (str == NULL || str->dataStr == NULL || buffer == NULL || pos >= str->size) 
        return -1;
    
    size_t bufferLen = strlen(buffer);

    if (bufferLen == 0) 
        return (int)pos;  // If buffer is empty, return pos
    
    for (size_t i = pos; i <= str->size - bufferLen; ++i) 
        if (strncmp(str->dataStr + i, buffer, bufferLen) != 0) 
            return (int)i;
        
    return -1;  // No non-matching position found
}

int string_find_last_not_of(String *str, const char *buffer, size_t pos) 
{
    if (str == NULL || str->dataStr == NULL || buffer == NULL) 
        return -1;
    
    size_t bufferLen = strlen(buffer);

    if (bufferLen == 0 || pos < bufferLen - 1) 
        return -1;
    
    pos = (pos < str->size - bufferLen) ? pos : str->size - bufferLen;

    for (int i = (int)pos; i >= 0; --i) 
        if (strncmp(str->dataStr + i, buffer, bufferLen) != 0) 
            return i;
        
    return -1;
}

const char *string_data(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;
    
    return str->dataStr;
}

const char *string_c_str(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return "";  // Return empty string for null or uninitialized String
    
    return str->dataStr;
}

char *string_begin(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;  // Return NULL for null or uninitialized String
    
    return str->dataStr;  // The beginning of the string
}

char *string_end(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;  // Return NULL for null or uninitialized String
    
    return str->dataStr + str->size;  // The end of the string
}

char *string_rbegin(String *str)
{
    if (str == NULL || str->dataStr == NULL || str->size == 0) 
        return NULL; 
    
    return str->dataStr + str->size - 1;
}

char *string_rend(String *str)
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;
    
    return str->dataStr - 1; 
}

const char *string_cbegin(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;  // Return NULL for null or uninitialized String
    
    return str->dataStr;  // The beginning of the string
}

const char *string_cend(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;  // Return NULL for null or uninitialized String
    
    return str->dataStr + str->size;  // The end of the string
}

const char *string_crbegin(String *str) 
{
    if (str == NULL || str->dataStr == NULL || str->size == 0) 
        return NULL;  // Return NULL for null, uninitialized, or empty String
    
    return str->dataStr + str->size - 1;  // Pointer to the last character
}

const char *string_crend(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;  // Return NULL for null or uninitialized String
    
    return str->dataStr - 1;  // Pointer to one before the first character
}

void string_clear(String* str) 
{
    if (str != NULL) 
    {
        str->size = 0;  // Reset the size to 0, indicating the string is now empty

        // Set the first character to the null terminator.
        // This ensures that the string is considered empty when accessed.
        if (str->dataStr != NULL) 
            str->dataStr[0] = '\0';
    }
}


bool string_set_pool_size(String* str, size_t newSize) 
{
    if (!str || newSize == 0) 
        return false; // Return false for invalid input

    // If a memory pool already exists, destroy it first
    if (str->pool) {
        memory_pool_destroy(str->pool);
        str->pool = NULL;
    }

    // Create a new memory pool with the specified size
    str->pool = memory_pool_create(newSize);
    if (!str->pool) 
        return false; // Return false if memory pool creation fails

    // If the string already has data, reallocate it in the new pool
    if (str->size > 0 && str->dataStr) {
        char* newData = memory_pool_allocate(str->pool, str->size + 1); // +1 for null terminator
        if (!newData) {
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
