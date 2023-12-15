#include "string.h"
#include <stdlib.h>
#include <string.h>

static String* string_substr_impl(String* str, size_t pos, size_t len);
static bool string_is_equal_impl(String* str1, String* str2);
static bool string_is_less_impl(String* str1, String* str2);
static bool string_is_greater_impl(String* str1, String* str2);
static bool string_is_less_or_equal_impl(String* str1, String* str2);
static bool string_is_greater_or_equal_impl(String* str1, String* str2);
static bool string_is_not_equal_impl(String* str1, String* str2);
static bool string_empty_impl(String* str);
static int string_compare_impl(String* str1, String* str2);
static void string_resize_impl(String* str, size_t newSize);
static void string_shrink_to_fit_impl(String* str);
static void string_append_impl(String* str, const char* strItem);
static void string_push_back_impl(String* str, const char chItem); 
static void string_assign_impl(String* str, const char* newStr);
static void string_insert_impl(String* str, size_t pos, const char* strItem);
static void string_erase_impl(String* str, size_t pos, size_t len);
static void string_replace_impl(String* str1, const char* oldStr, const char* newStr);
static void string_swap_impl(String* str1, String* str2);
static void string_pop_back_impl(String* str);
static void string_deallocate_impl(String* str);
static void string_clear_impl(String* str);
static size_t string_length_impl(String* str);
static size_t string_capacity_impl(String* str);
static size_t string_max_size_impl(String* str);
static size_t string_copy_impl(String* str, char* buffer, size_t pos, size_t len);
static int string_find_impl(String* str, const char* buffer, size_t pos);
static int string_rfind_impl(String* str, const char* buffer, size_t pos);
static int string_find_first_of_impl(String* str, const char* buffer, size_t pos);
static int string_find_last_of_impl(String* str, const char* buffer, size_t pos);
static int string_find_first_not_of_impl(String* str, const char* buffer, size_t pos);
static int string_find_last_not_of_impl(String* str, const char* buffer, size_t pos);
static const char* string_data_impl(String* str);
static const char* string_c_str_impl(String* str);
static char* string_begin_impl(String* str);
static char* string_end_impl(String* str);
static char* string_rbegin_impl(String* str);
static char* string_rend_impl(String* str);
static const char* string_cbegin_impl(String* str);
static const char* string_cend_impl(String* str);
static const char* string_crbegin_impl(String* str);
static const char* string_crend_impl(String* str);
static const char string_at_impl(String* str, size_t index);
static const char string_back_impl(String* str);
static const char string_front_impl(String* str);

String* string_create(const char* initialStr) 
{
    String* str = (String*)malloc(sizeof(String));

    if (!str) 
        return NULL; // Allocation failed
    
    str->substr = string_substr_impl;
    str->is_equal = string_is_equal_impl;
    str->is_less = string_is_less_impl;
    str->is_greater = string_is_greater_impl;
    str->is_less_or_equal = string_is_less_or_equal_impl;
    str->is_greater_or_equal = string_is_greater_or_equal_impl;
    str->is_not_equal = string_is_not_equal_impl;
    str->empty = string_empty_impl;
    str->compare = string_compare_impl;
    str->resize = string_resize_impl;
    str->shrink_to_fit = string_shrink_to_fit_impl;
    str->append = string_append_impl;
    str->push_back = string_push_back_impl;
    str->assign = string_assign_impl;
    str->insert = string_insert_impl;
    str->erase = string_erase_impl;
    str->replace = string_replace_impl;
    str->swap = string_swap_impl;
    str->pop_back = string_pop_back_impl;
    str->deallocate = string_deallocate_impl;
    str->clear = string_clear_impl;
    str->length = string_length_impl;
    str->capacity = string_capacity_impl;
    str->max_size = string_max_size_impl;
    str->copy = string_copy_impl;
    str->find = string_find_impl;
    str->rfind = string_rfind_impl;
    str->find_first_of = string_find_first_of_impl;
    str->find_last_of = string_find_last_of_impl;
    str->find_first_not_of = string_find_first_not_of_impl;
    str->find_last_not_of = string_find_last_not_of_impl;
    str->data = string_data_impl;
    str->c_str = string_c_str_impl;
    str->begin = string_begin_impl;
    str->end = string_end_impl;
    str->rbegin = string_rbegin_impl;
    str->rend = string_rend_impl;
    str->cbegin = string_cbegin_impl;
    str->cend = string_cend_impl;
    str->crbegin = string_crbegin_impl;
    str->crend = string_crend_impl;
    str->at = string_at_impl;
    str->back = string_back_impl;
    str->front = string_front_impl;

    size_t initialCapacity = 16;  // Default initial capacity
    size_t initialSize = 0;

    if (initialStr) 
    {
        initialSize = strlen(initialStr);
        initialCapacity = initialSize + 1;  // Include space for null terminator
    }

    str->dataStr = (char*)malloc(initialCapacity * sizeof(char));

    if (!str->dataStr) 
    {
        free(str); // Free the String struct if data allocation fails
        return NULL;
    }

    if (initialStr) 
        strcpy(str->dataStr, initialStr);  // Copy initial string into data
    else 
        str->dataStr[0] = '\0';  // If no initial string, start with empty string
    
    str->size = initialSize;
    str->capacitySize = initialCapacity;

    return str;
}

static String* string_substr_impl(String* str, size_t pos, size_t len) 
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

static bool string_empty_impl(String* str) 
{
    if (str == NULL) 
        return true;  // A NULL string is considered empty
    
    return str->size == 0;
}

static int string_compare_impl(String* str1, String* str2) 
{
    if (str1 == NULL || str2 == NULL) 
    {
        if (str1 == str2) 
            return 0;  // Both are NULL, considered equal
        
        return (str1 == NULL) ? -1 : 1;  // NULL is considered less than non-NULL
    }

    return strcmp(str1->dataStr, str2->dataStr);
}

static bool string_is_equal_impl(String* str1, String* str2) 
{
    return string_compare_impl(str1, str2) == 0;
}

static bool string_is_less_impl(String* str1, String* str2) 
{
    return string_compare_impl(str1, str2) < 0;
}

static bool string_is_greater_impl(String* str1, String* str2) 
{
    return string_compare_impl(str1, str2) > 0;
}

static bool string_is_less_or_equal_impl(String* str1, String* str2) 
{
    return string_compare_impl(str1, str2) <= 0;
}

static bool string_is_greater_or_equal_impl(String* str1, String* str2) 
{
    return string_compare_impl(str1, str2) >= 0;
}

static bool string_is_not_equal_impl(String* str1, String* str2) 
{
    return string_compare_impl(str1, str2) != 0;
}

static void string_resize_impl(String *str, size_t newSize) 
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
            char *newData = realloc(str->dataStr, newCapacity);

            if (!newData) 
                return;  // Handle allocation error
            
            str->dataStr = newData;
            str->capacitySize = newCapacity;
        }

        memset(str->dataStr + str->size, '\0', newSize - str->size);
        str->size = newSize;
    }
}

static void string_shrink_to_fit_impl(String *str) 
{
    if (str == NULL || str->size + 1 == str->capacitySize) 
        return;
    
    char *newData = realloc(str->dataStr, str->size + 1);

    if (!newData && str->size > 0) 
        return;  // Handle allocation error
    
    str->dataStr = newData;
    str->capacitySize = str->size + 1;
}


static void string_append_impl(String *str, const char *strItem) 
{
    if (str == NULL || strItem == NULL) 
        return;

    size_t strItemLength = strlen(strItem);

    if (strItemLength == 0) 
        return;

    if (str->size + strItemLength >= str->capacitySize) 
    {
        size_t newCapacity = str->size + strItemLength + 1;
        char *newData = realloc(str->dataStr, newCapacity);

        if (!newData) 
            return;  // Handle allocation error
        
        str->dataStr = newData;
        str->capacitySize = newCapacity;
    }

    strcpy(str->dataStr + str->size, strItem);
    str->size += strItemLength;
}

static void string_push_back_impl(String *str, char chItem)
{
    if (str == NULL) 
        return;

    if (str->size + 1 >= str->capacitySize) 
    {
        size_t newCapacity = (str->capacitySize == 0) ? 1 : str->capacitySize * 2;
        char* newData = realloc(str->dataStr, newCapacity);

        if (!newData) 
            return;  // Handle allocation error
        
        str->dataStr = newData;
        str->capacitySize = newCapacity;
    }

    str->dataStr[str->size] = chItem;
    str->dataStr[str->size + 1] = '\0';  // Null-terminate the string
    str->size++;
}

static void string_assign_impl(String *str, const char *newStr) 
{
    if (str == NULL || newStr == NULL) 
        return;
    
    size_t newStrLength = strlen(newStr);

    if (newStrLength + 1 > str->capacitySize) 
    {
        char *newData = realloc(str->dataStr, newStrLength + 1);
        if (!newData) 
            return;  // Handle allocation error
        
        str->dataStr = newData;
        str->capacitySize = newStrLength + 1;
    }

    strcpy(str->dataStr, newStr);
    str->size = newStrLength;
}

static void string_insert_impl(String *str, size_t pos, const char *strItem) 
{
    if (str == NULL || strItem == NULL || pos > str->size) 
        return;
    
    size_t strItemLength = strlen(strItem);
    size_t newTotalLength = str->size + strItemLength;

    if (newTotalLength + 1 > str->capacitySize) 
    {
        size_t newCapacity = newTotalLength + 1;
        char *newData = realloc(str->dataStr, newCapacity);

        if (!newData) 
            return;  // Handle allocation error
        
        str->dataStr = newData;
        str->capacitySize = newCapacity;
    }

    memmove(str->dataStr + pos + strItemLength, str->dataStr + pos, str->size - pos + 1);
    memcpy(str->dataStr + pos, strItem, strItemLength);
    str->size = newTotalLength;
}

static void string_erase_impl(String *str, size_t pos, size_t len) 
{
    if (str == NULL || pos >= str->size) 
        return;

    if (pos + len > str->size) 
        len = str->size - pos;  // Adjust len to not go beyond the string end
    
    memmove(str->dataStr + pos, str->dataStr + pos + len, str->size - pos - len + 1);
    str->size -= len;
}

static void string_replace_impl(String *str1, const char *oldStr, const char *newStr) 
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
        char *newData = realloc(str1->dataStr, newCapacity);

        if (!newData) 
            return;  // Handle allocation error
        
        str1->dataStr = newData;
        str1->capacitySize = newCapacity;
    }

    memmove(position + newLen, position + oldLen, tailLen + 1); // +1 for null terminator
    memcpy(position, newStr, newLen);
    str1->size = newSize;
}

static void string_swap_impl(String *str1, String *str2) 
{
    if (str1 == NULL || str2 == NULL) 
        return;
    
    String temp = *str1;
    *str1 = *str2;
    *str2 = temp;
}

static void string_pop_back_impl(String *str) 
{
    if (str == NULL || str->size == 0) 
        return;
    
    str->dataStr[str->size - 1] = '\0';
    str->size--;
}


void string_deallocate_impl(String *str)
{
    if (str == NULL) 
        return; 

    if (str->dataStr != NULL)  // Free the dataStr if it's not NULL
    {
        free(str->dataStr);
        str->dataStr = NULL;

        free(str);
    }
}

static char string_at_impl(String* str, size_t index)
{
    if (str == NULL || index >= str->size) 
    {
        perror("Index out of the range");
        exit(-1);
    }

    return str->dataStr[index];
}

const char string_back_impl(String *str) 
{
    if (str == NULL || str->size == 0) 
        return '\0'; 
    
    return str->dataStr[str->size - 1];
}

const char string_front_impl(String *str) 
{
    if (str == NULL || str->size == 0) 
        return '\0';  // Return null character for empty or null string
    
    return str->dataStr[0];
}

static size_t string_length_impl(String* str) 
{
    return (str != NULL) ? str->size : 0;
}

static size_t string_capacity_impl(String* str) 
{
    return (str != NULL) ? str->capacitySize : 0;
}

static size_t string_max_size_impl(String* str) 
{
    return (size_t)-1;
}

static size_t string_copy_impl(String *str, char *buffer, size_t pos, size_t len) 
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


static int string_find_impl(String *str, const char *buffer, size_t pos) 
{
    if (str == NULL || str->dataStr == NULL || buffer == NULL || pos >= str->size) 
        return -1;  // Return -1 for invalid input or position out of bounds
    
    const char *found = strstr(str->dataStr + pos, buffer);
    if (found == NULL) 
        return -1;  // Substring not found
    
    return (int)(found - str->dataStr);  // Return the position of the substring
}

static int string_rfind_impl(String *str, const char *buffer, size_t pos) 
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

static int string_find_first_of_impl(String *str, const char *buffer, size_t pos) 
{
    if (str == NULL || str->dataStr == NULL || buffer == NULL || pos >= str->size) 
        return -1;  // Return -1 for invalid input or position out of bounds
    
    const char *found = strstr(str->dataStr + pos, buffer);

    if (found != NULL) 
        return (int)(found - str->dataStr);
    
    return -1;  // Buffer string not found
}


static int string_find_last_of_impl(String *str, const char *buffer, size_t pos) 
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

static int string_find_first_not_of_impl(String *str, const char *buffer, size_t pos) 
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

static int string_find_last_not_of_impl(String *str, const char *buffer, size_t pos) 
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

static const char *string_data_impl(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;
    
    return str->dataStr;
}

static const char *string_c_str_impl(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return "";  // Return empty string for null or uninitialized String
    
    return str->dataStr;
}

static char *string_begin_impl(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;  // Return NULL for null or uninitialized String
    
    return str->dataStr;  // The beginning of the string
}

static char *string_end_impl(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;  // Return NULL for null or uninitialized String
    
    return str->dataStr + str->size;  // The end of the string
}

static char *string_rbegin_impl(String *str)
{
    if (str == NULL || str->dataStr == NULL || str->size == 0) 
        return NULL; 
    
    return str->dataStr + str->size - 1;
}

static char *string_rend_impl(String *str)
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;
    
    return str->dataStr - 1; 
}

static const char *string_cbegin_impl(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;  // Return NULL for null or uninitialized String
    
    return str->dataStr;  // The beginning of the string
}

static const char *string_cend_impl(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;  // Return NULL for null or uninitialized String
    
    return str->dataStr + str->size;  // The end of the string
}

static const char *string_crbegin_impl(String *str) 
{
    if (str == NULL || str->dataStr == NULL || str->size == 0) 
        return NULL;  // Return NULL for null, uninitialized, or empty String
    
    return str->dataStr + str->size - 1;  // Pointer to the last character
}

static const char *string_crend_impl(String *str) 
{
    if (str == NULL || str->dataStr == NULL) 
        return NULL;  // Return NULL for null or uninitialized String
    
    return str->dataStr - 1;  // Pointer to one before the first character
}

static void string_clear_impl(String* str) 
{
    if (str != NULL) 
    {
        str->size = 0;
        str->dataStr[0] = '\0'; // Set first character to null terminator
    }
}
