#ifndef STRING_H_
#define STRING_H_

typedef struct String String;

#include <stdio.h>
#include <stdbool.h>

struct String 
{
    char* dataStr;
    size_t size;
    size_t capacitySize;

    String* (*substr)(String* str, size_t pos, size_t len);
    bool (*is_equal)(String* str1, String* str2);
    bool (*is_less)(String* str1, String* str2);
    bool (*is_greater)(String* str1, String* str2);
    bool (*is_less_or_equal)(String* str1, String* str2);
    bool (*is_greater_or_equal)(String* str1, String* str2);
    bool (*is_not_equal)(String* str1, String* str2);
    bool (*empty)(String* str);
    int (*compare)(String* str1, String* str2);
    void (*resize)(String* str, size_t newSize);
    void (*shrink_to_fit)(String* str);
    void (*append)(String* str, const char* strItem);
    void (*push_back)(String* str, const char chItem); 
    void (*assign)(String* str, const char* newStr);
    void (*insert)(String* str, size_t pos, const char* strItem);
    void (*erase)(String* str, size_t pos, size_t len);
    void (*replace)(String* str1, const char* oldStr, const char* newStr);
    void (*swap)(String* str1, String* str2);
    void (*pop_back)(String* str);
    void (*deallocate)(String* str);
    void (*clear)(String* str);
    size_t (*length)(String* str);
    size_t (*capacity)(String* str);
    size_t (*max_size)(String* str);
    size_t (*copy)(String* str, char* buffer, size_t pos, size_t len);
    int (*find)(String* str, const char* buffer, size_t pos);
    int (*rfind)(String* str, const char* buffer, size_t pos);
    int (*find_first_of)(String* str, const char* buffer, size_t pos);
    int (*find_last_of)(String* str, const char* buffer, size_t pos);
    int (*find_first_not_of)(String* str, const char* buffer, size_t pos);
    int (*find_last_not_of)(String* str, const char* buffer, size_t pos);
    const char* (*data)(String* str);
    const char* (*c_str)(String* str);
    char* (*begin)(String* str);
    char* (*end)(String* str);
    char* (*rbegin)(String* str);
    char* (*rend)(String* str);
    const char* (*cbegin)(String* str);
    const char* (*cend)(String* str);
    const char* (*crbegin)(String* str);
    const char* (*crend)(String* str);
    const char (*at)(String* str, size_t index);
    char* (*back)(String* str);
    char* (*front)(String* str);
};

String* string_create(const char* initialStr);

#endif 