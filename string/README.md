# String Library in C

**Author:** amin tahmasebi
**Release Date:** 2023
**License:** ISC License


The String library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::string` in C++.

## Compilation

To compile the String library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./string/string.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the String library in your project, include the `string.h` header file in your source code.

Absolutely, adding a brief description for each function at the top of your README can greatly enhance its clarity and usability. Here's how you might structure this section:

---

## Function Descriptions

## Constants 
- `STRING_ASCII_LETTERS`
- `STRING_ASCII_LOWERCASE`
- `STRING_ASCII_UPPERCASE`
- `STRING_DIGITS`
- `STRING_HEXDIGITS`
- `STRING_WHITESPACE`
- `STRING_PUNCTUATION`

### Explanation of Functions 

#### `String* string_create(const char* initialStr)`
- **Purpose**:  
  Creates a new `String` object initialized with the provided `initialStr`. If no string is provided (`initialStr` is `NULL`), the `String` object is initialized with an empty string.
- **Parameters**:
  - `initialStr`: The initial string to be stored in the `String` object. Can be `NULL` for an empty string.
- **Return Value**:  
  - Returns a pointer to the newly created `String` object. Terminates the program if memory allocation fails.

---

#### `String* string_create_with_pool(size_t size)`
- **Purpose**:  
  Creates a new `String` object that uses a globally initialized memory pool for memory management.
- **Parameters**:
  - `size`: The size of the global memory pool to be initialized.
- **Return Value**:  
  - Returns a pointer to the newly created `String` object. Terminates the program if memory allocation or pool initialization fails.

---

#### `String* string_substr(String* str, size_t pos, size_t len)`
- **Purpose**:  
  Extracts a substring from the `String` object, starting at position `pos` and having the length `len`.
- **Parameters**:
  - `str`: The original `String` object from which the substring is extracted.
  - `pos`: The starting position of the substring within the original string.
  - `len`: The length of the substring to extract.
- **Return Value**:  
  - Returns a pointer to the newly created `String` object containing the substring. Returns `NULL` if an error occurs.

---

#### `bool string_empty(const String* str)`
- **Purpose**:  
  Checks if the `String` object is empty, meaning it contains no characters or is `NULL`.
- **Parameters**:
  - `str`: The `String` object to check.
- **Return Value**:  
  - Returns `true` if the `String` object is empty or `NULL`, otherwise `false`.

---

#### `bool string_contains(const String* str, const char* substr)`
- **Purpose**:  
  Checks if the specified substring (`substr`) exists within the `String` object.
- **Parameters**:
  - `str`: The `String` object to search within.
  - `substr`: The substring to search for.
- **Return Value**:  
  - Returns `true` if the substring is found, otherwise `false`.

---

#### `int string_compare(const String* str1, const String* str2)`
- **Purpose**:  
  Compares two `String` objects lexicographically.
- **Parameters**:
  - `str1`: The first `String` object.
  - `str2`: The second `String` object.
- **Return Value**:  
  - Returns `0` if both strings are equal, less than `0` if `str1` is less than `str2`, and greater than `0` if `str1` is greater than `str2`.

---

#### `bool string_is_equal(const String* str1, const String* str2)`
- **Purpose**:  
  Checks if two `String` objects are lexicographically equal.
- **Parameters**:
  - `str1`: The first `String` object.
  - `str2`: The second `String` object.
- **Return Value**:  
  - Returns `true` if the two strings are equal, otherwise `false`.

---

#### `bool string_is_less(const String* str1, const String* str2)`
- **Purpose**:  
  Checks if the first `String` is lexicographically less than the second.
- **Parameters**:
  - `str1`: The first `String` object.
  - `str2`: The second `String` object.
- **Return Value**:  
  - Returns `true` if `str1` is less than `str2`, otherwise `false`.

---

#### `bool string_is_greater(const String* str1, const String* str2)`
- **Purpose**:  
  Checks if the first `String` is lexicographically greater than the second.
- **Parameters**:
  - `str1`: The first `String` object.
  - `str2`: The second `String` object.
- **Return Value**:  
  - Returns `true` if `str1` is greater than `str2`, otherwise `false`.

---

#### `bool string_is_less_or_equal(const String* str1, const String* str2)`
- **Purpose**:  
  Checks if the first `String` is lexicographically less than or equal to the second.
- **Parameters**:
  - `str1`: The first `String` object.
  - `str2`: The second `String` object.
- **Return Value**:  
  - Returns `true` if `str1` is less than or equal to `str2`, otherwise `false`.

---

#### `bool string_is_greater_or_equal(const String* str1, const String* str2)`
- **Purpose**:  
  Checks if the first `String` is lexicographically greater than or equal to the second.
- **Parameters**:
  - `str1`: The first `String` object.
  - `str2`: The second `String` object.
- **Return Value**:  
  - Returns `true` if `str1` is greater than or equal to `str2`, otherwise `false`.

---

#### `bool string_is_not_equal(const String* str1, const String* str2)`
- **Purpose**:  
  Compares two `String` objects for inequality.
- **Parameters**:
  - `str1`: The first `String` object.
  - `str2`: The second `String` object.
- **Return Value**:  
  - Returns `true` if the strings are not equal, otherwise `false`.

---

#### `bool string_is_alpha(const String* str)`
- **Purpose**:  
  Checks if all characters in the `String` object are alphabetic (A-Z, a-z).
- **Parameters**:
  - `str`: The `String` object to check.
- **Return Value**:  
  - Returns `true` if all characters are alphabetic, otherwise `false`.

---

#### `bool string_is_digit(const String* str)`
- **Purpose**:  
  Checks if all characters in the `String` object are digits (0-9).
- **Parameters**:
  - `str`: The `String` object to check.
- **Return Value**:  
  - Returns `true` if all characters are digits, otherwise `false`.

---

#### `bool string_is_upper(const String* str)`
- **Purpose**:  
  Checks if all characters in the `String` object are uppercase.
- **Parameters**:
  - `str`: The `String` object to check.
- **Return Value**:  
  - Returns `true` if all characters are uppercase, otherwise `false`.

---

#### `bool string_is_lower(const String* str)`
- **Purpose**:  
  Checks if all characters in the `String` object are lowercase.
- **Parameters**:
  - `str`: The `String` object to check.
- **Return Value**:  
  - Returns `true` if all characters are lowercase, otherwise `false`.

---

#### `void string_reverse(String* str)`
- **Purpose**:  
  Reverses the order of characters in the `String` object.
- **Parameters**:
  - `str`: The `String` object to reverse.
- **Return Value**:  
  - No return value, modifies the `String` object in place.

---

#### `void string_resize(String *str, size_t newSize)`
- **Purpose**:  
  Resizes the `String` object to the specified `newSize`.
- **Parameters**:
  - `str`: The `String` object to resize.
  - `newSize`: The new size to resize the `String` to.
- **Return Value**:  
  - No return value, modifies the `String` object in place.

---

#### `void string_shrink_to_fit(String *str)`
- **Purpose**:  
  Reduces the capacity of the `String` object to fit its current size, shrinking the capacity to match the exact size of the string plus the null terminator. If the current capacity is already optimal, no action is taken.
- **Parameters**:
  - `str`: The `String` object to shrink. Must not be `NULL`.
- **Return Value**:  
  - No return value, modifies the `String` object in place.

---

#### `void string_append(String *str, const char *strItem)`
- **Purpose**:  
  Appends a C-string (`strItem`) to the end of the `String` object. If the new size exceeds the current capacity, the function reallocates memory to accommodate the additional characters.
- **Parameters**:
  - `str`: The `String` object to which the string will be appended. Must not be `NULL`.
  - `strItem`: The C-string to append. Must not be `NULL`.
- **Return Value**:  
  - No return value, modifies the `String` object in place.

---

#### `void string_push_back(String *str, char chItem)`
- **Purpose**:  
  Appends a single character (`chItem`) to the end of the `String` object. If necessary, the function reallocates memory to accommodate the additional character.
- **Parameters**:
  - `str`: The `String` object to which the character will be appended. Must not be `NULL`.
  - `chItem`: The character to append.
- **Return Value**:  
  - No return value, modifies the `String` object in place.

---

#### `void string_assign(String *str, const char *newStr)`
- **Purpose**:  
  Replaces the current contents of the `String` object with the contents of `newStr`. If the new string is larger than the current capacity, the function reallocates memory to accommodate it.
- **Parameters**:
  - `str`: The `String` object to which the new string will be assigned. Must not be `NULL`.
  - `newStr`: The new string to assign. Must not be `NULL`.
- **Return Value**:  
  - No return value, modifies the `String` object in place.

---

#### `void string_insert(String *str, size_t pos, const char *strItem)`
- **Purpose**:  
  Inserts the contents of `strItem` into the `String` object at the specified position (`pos`). If the new size exceeds the current capacity, the function reallocates memory to accommodate the insertion.
- **Parameters**:
  - `str`: The `String` object into which the substring will be inserted. Must not be `NULL`.
  - `pos`: The position at which to insert the substring. Must be within the bounds of the string's size.
  - `strItem`: The substring to insert. Must not be `NULL`.
- **Return Value**:  
  - No return value, modifies the `String` object in place.

---

#### `void string_erase(String *str, size_t pos, size_t len)`
- **Purpose**:  
  This function erases a portion of the `String` object starting at the specified position (`pos`) and spanning a specified length (`len`). If `len` exceeds the remaining length from `pos`, it is adjusted accordingly.
- **Parameters**:
  - `str`: The `String` object from which characters will be erased. Must not be `NULL`.
  - `pos`: The starting position for the erase operation. Must be within the bounds of the string's size.
  - `len`: The number of characters to erase.
- **Return Value**:  
  - No return value, modifies the `String` object in place.

---

#### `void string_replace(String *str1, const char *oldStr, const char *newStr)`
- **Purpose**:  
  Replaces the first occurrence of the substring `oldStr` in the `String` object `str1` with `newStr`. If `oldStr` is not found, the function does nothing.
- **Parameters**:
  - `str1`: The `String` object in which the replacement will occur. Must not be `NULL`.
  - `oldStr`: The substring to be replaced. Must not be `NULL`.
  - `newStr`: The new substring to replace `oldStr` with. Must not be `NULL`.
- **Return Value**:  
  - No return value, modifies the `String` object in place.

---

#### `void string_swap(String *str1, String *str2)`
- **Purpose**:  
  Swaps the contents of two `String` objects.
- **Parameters**:
  - `str1`: The first `String` object. Must not be `NULL`.
  - `str2`: The second `String` object. Must not be `NULL`.
- **Return Value**:  
  - No return value, swaps the contents of the two `String` objects.

---

#### `void string_pop_back(String *str)`
- **Purpose**:  
  Removes the last character from the `String` object, reducing its size by one. If the string is empty, the function logs a warning and does nothing.
- **Parameters**:
  - `str`: The `String` object from which the last character is to be removed. Must not be `NULL`.
- **Return Value**:  
  - No return value, modifies the `String` object in place.

---

#### `void string_deallocate(String *str)`
- **Purpose**:  
  Deallocates the memory used by the `String` object and its associated resources, including the memory pool. If the `String` object is `NULL`, the function logs a warning and does nothing.
- **Parameters**:
  - `str`: The `String` object to be deallocated. Must not be `NULL`.
- **Return Value**:  
  - No return value, frees the `String` and its associated memory.

---

#### `char string_at(const String *str, size_t index)`
- **Purpose**:  
  Retrieves the character at the specified index in the `String` object. If the index is out of range, the function logs an error and returns a default character (`'\0'`).
- **Parameters**:
  - `str`: The `String` object from which the character is to be retrieved. Must not be `NULL`.
  - `index`: The index of the character to retrieve.
- **Return Value**:  
  - The character at the specified index, or `'\0'` if the index is out of range or the `String` is `NULL`.

---

#### `char* string_back(const String *str)`
- **Purpose**:  
  Returns a pointer to the last character in the `String` object. If the `String` is `NULL` or empty, the function returns `NULL`.
- **Parameters**:
  - `str`: The `String` object to access. Must not be `NULL` or empty.
- **Return Value**:  
  - A pointer to the last character in the string, or `NULL` if the string is `NULL` or empty.

---

#### `char* string_front(const String *str)`
- **Purpose**:  
  Returns a pointer to the first character in the `String` object. If the `String` is `NULL` or empty, the function returns `NULL`.
- **Parameters**:
  - `str`: The `String` object to access. Must not be `NULL` or empty.
- **Return Value**:  
  - A pointer to the first character in the string, or `NULL` if the string is `NULL` or empty.

---

#### `size_t string_length(const String *str)`
- **Purpose**:  
  Retrieves the current length (size) of the `String` object. If the `String` is `NULL`, the function logs an error and returns `0`.
- **Parameters**:
  - `str`: The `String` object whose length is to be determined. Must not be `NULL`.
- **Return Value**:  
  - The length of the string, or `0` if the `String` is `NULL`.

---

#### `size_t string_capacity(const String *str)`
- **Purpose**:  
  Retrieves the current capacity of the `String` object, indicating the maximum size the string can grow to without requiring reallocation. If the `String` is `NULL`, the function logs an error and returns `0`.
- **Parameters**:
  - `str`: The `String` object whose capacity is to be determined. Must not be `NULL`.
- **Return Value**:  
  - The capacity of the string, or `0` if the `String` is `NULL`.

---

#### `size_t string_max_size(const String* str)`
- **Purpose**:  
  Returns the maximum possible size of the `String` object. Typically, this is a large value representing the maximum size the string can have on the system.
- **Parameters**:
  - `str`: The `String` object. Must not be `NULL`.
- **Return Value**:  
  - The maximum possible size of the string, or `0` if the `String` object is `NULL`.

---

#### `size_t string_copy(const String* str, char* buffer, size_t pos, size_t len)`
- **Purpose**:  
  Copies up to `len` characters from the `String` object, starting at the specified position `pos`, into the provided buffer. It ensures the copied string is null-terminated.
- **Parameters**:
  - `str`: The `String` object to copy from. Must not be `NULL`.
  - `buffer`: The buffer to copy the substring into. Must not be `NULL`.
  - `pos`: The starting position in the string from which to begin copying.
  - `len`: The number of characters to copy.
- **Return Value**:  
  - The number of characters actually copied.

---

#### `int string_find(const String* str, const char* buffer, size_t pos)`
- **Purpose**:  
  Finds the first occurrence of the substring `buffer` in the `String` object, starting from the position `pos`.
- **Parameters**:
  - `str`: The `String` object to search in. Must not be `NULL`.
  - `buffer`: The substring to search for. Must not be `NULL`.
  - `pos`: The starting position for the search.
- **Return Value**:  
  - The index of the first occurrence of the substring, or `-1` if not found or an error occurs.

---

#### `int string_rfind(const String* str, const char* buffer, size_t pos)`
- **Purpose**:  
  Finds the last occurrence of the substring `buffer` in the `String` object, up to the specified position `pos`.
- **Parameters**:
  - `str`: The `String` object to search in. Must not be `NULL`.
  - `buffer`: The substring to search for. Must not be `NULL`.
  - `pos`: The ending position for the search.
- **Return Value**:  
  - The index of the last occurrence of the substring, or `-1` if not found or an error occurs.

---

#### `int string_find_first_of(const String* str, const char* buffer, size_t pos)`
- **Purpose**:  
  Finds the first occurrence of any character from the `buffer` in the `String` object, starting from the position `pos`.
- **Parameters**:
  - `str`: The `String` object to search in. Must not be `NULL`.
  - `buffer`: The buffer containing characters to search for. Must not be `NULL`.
  - `pos`: The starting position for the search.
- **Return Value**:  
  - The index of the first matching character, or `-1` if no match is found or an error occurs.

---

#### `int string_find_last_of(const String* str, const char* buffer, size_t pos)`
- **Purpose**:  
  Finds the last occurrence of any character from the `buffer` in the `String` object, up to the specified position `pos`.
- **Parameters**:
  - `str`: The `String` object to search in. Must not be `NULL`.
  - `buffer`: The buffer containing characters to search for. Must not be `NULL`.
  - `pos`: The ending position for the search.
- **Return Value**:  
  - The index of the last matching character, or `-1` if no match is found or an error occurs.

---

#### `int string_find_first_not_of(const String* str, const char* buffer, size_t pos)`
- **Purpose**:  
  Finds the first character in the `String` object starting from the specified position that does not match any character in the `buffer`.
- **Parameters**:
  - `str`: The `String` object to search in. Must not be `NULL`.
  - `buffer`: The buffer containing characters to exclude. Must not be `NULL`.
  - `pos`: The starting position for the search. Must be within the bounds of the string.
- **Return Value**:  
  The index of the first non-matching character, or `-1` if all characters match or an error occurs.

---

#### `int string_find_last_not_of(const String* str, const char* buffer, size_t pos)`
- **Purpose**:  
  Finds the last character in the `String` object, starting from the specified position and searching backwards, that does not match any character in the `buffer`.
- **Parameters**:
  - `str`: The `String` object to search in. Must not be `NULL`.
  - `buffer`: The buffer containing characters to exclude. Must not be `NULL`.
  - `pos`: The starting position for the search. Must be within the bounds of the string.
- **Return Value**:  
  The index of the last non-matching character, or `-1` if all characters match or an error occurs.

---

#### `const char* string_data(const String* str)`
- **Purpose**:  
  Returns the raw string data from the `String` object.
- **Parameters**:
  - `str`: The `String` object. Must not be `NULL`.
- **Return Value**:  
  A pointer to the character array managed by the `String` object, or `NULL` on error.

---

#### `const char* string_c_str(const String* str)`
- **Purpose**:  
  Returns a constant pointer to the character array managed by the `String` object. Ensures that the returned string is null-terminated.
- **Parameters**:
  - `str`: The `String` object. Must not be `NULL`.
- **Return Value**:  
  A constant pointer to the C-string, or an empty string (`""`) on error.

---

#### `char* string_begin(const String* str)`
- **Purpose**:  
  Returns a pointer to the first character in the `String` object.
- **Parameters**:
  - `str`: The `String` object. Must not be `NULL`.
- **Return Value**:  
  A pointer to the first character of the string, or an empty string on error.

---

#### `char* string_end(const String* str)`
- **Purpose**:  
  Returns a pointer to the position just after the last character in the `String` object.
- **Parameters**:
  - `str`: The `String` object. Must not be `NULL`.
- **Return Value**:  
  A pointer to the end of the string, or `NULL` on error.

---

#### `char* string_rbegin(const String* str)`
- **Purpose**:  
  Returns a pointer to the last character in the `String` object for reverse iteration.
- **Parameters**:
  - `str`: The `String` object. Must not be `NULL` or empty.
- **Return Value**:  
  A pointer to the last character of the string, or `NULL` on error.

---

#### `char* string_rend(const String* str)`
- **Purpose**:  
  Returns a pointer to one position before the start of the string data, typically used for reverse iteration.
- **Parameters**:
  - `str`: The `String` object. Must not be `NULL`.
- **Return Value**:  
  A pointer to one position before the first character, or `NULL` on error.

---

#### `const char* string_cbegin(const String* str)`
- **Purpose**:  
  Returns a constant pointer to the first character of the `String` object's data.
- **Parameters**:
  - `str`: The `String` object to retrieve the beginning pointer from. Must not be `NULL`.
- **Return Value**:  
  A constant pointer to the first character of the string, or `NULL` on error.

---

#### `const char* string_cend(const String* str)`
- **Purpose**:  
  Returns a constant pointer to the position just after the last character of the string data.
- **Parameters**:
  - `str`: The `String` object to retrieve the end pointer from. Must not be `NULL`.
- **Return Value**:  
  A constant pointer to the end of the string, or `NULL` on error.

---

#### `const char* string_crbegin(const String* str)`
- **Purpose**:  
  Returns a constant pointer to the last character in the `String` object for reverse iteration.
- **Parameters**:
  - `str`: The `String` object to retrieve the reverse beginning pointer from. Must not be `NULL` or empty.
- **Return Value**:  
  A constant pointer to the last character of the string, or `NULL` on error.

---

#### `const char* string_crend(const String* str)`
- **Purpose**:  
  Returns a constant pointer to one position before the start of the string data, used for reverse iteration.
- **Parameters**:
  - `str`: The `String` object to retrieve the reverse end pointer from. Must not be `NULL`.
- **Return Value**:  
  A constant pointer to one before the first character, or `NULL` on error.

---

#### `void string_clear(String* str)`
- **Purpose**:  
  Resets the `String` object, setting its size to zero and making it an empty string.
- **Parameters**:
  - `str`: The `String` object to clear. Must not be `NULL`.
- **Return Value**:  
  This function does not return a value.

---

#### `char* string_to_upper(const String* str)`
- **Purpose**:  
  Converts all characters in the `String` object to uppercase and returns a new C-string with the uppercase characters.
- **Parameters**:
  - `str`: The `String` object to convert. Must not be `NULL`.
- **Return Value**:  
  A new C-string with all characters in uppercase. The caller is responsible for freeing the allocated memory.

---

#### `char* string_to_lower(const String* str)`
- **Purpose**:  
  Converts all characters in the `String` object to lowercase and returns a new C-string with the lowercase characters.
- **Parameters**:
  - `str`: The `String` object to convert. Must not be `NULL`.
- **Return Value**:  
  A new C-string with all characters in lowercase. The caller is responsible for freeing the allocated memory.

---

#### `bool string_set_pool_size(String* str, size_t newSize)`
- **Purpose**:  
  Resizes the memory pool for the `String` object. If the pool already exists, it is destroyed and recreated with the new size.
- **Parameters**:
  - `str`: The `String` object to resize the memory pool for. Must not be `NULL`.
  - `newSize`: The new size for the memory pool. Must be greater than zero.
- **Return Value**:  
  Returns `true` if the pool was successfully resized, otherwise `false`.

---

#### `void string_concatenate(String* str1, const String* str2)`
- **Purpose**:  
  Concatenates the content of the second `String` object (`str2`) to the first `String` object (`str1`).
- **Parameters**:
  - `str1`: The `String` object to which the content will be appended. Must not be `NULL`.
  - `str2`: The `String` object whose content will be appended to `str1`. Must not be `NULL`.
- **Return Value**:  
  This function does not return a value.

---

#### `void string_trim_left(String *str)`
- **Purpose**:  
  This function removes all leading whitespace characters from the `String` object.
- **Parameters**:
  - `str`: The `String` object to be trimmed. Must not be `NULL`.
- **Functionality**:
  - Iterates over the string data and trims all leading whitespace characters.
  - Adjusts the string size and content accordingly.
- **Return Value**:  
  This function does not return a value.

---

#### `void string_trim_right(String *str)`
- **Purpose**:  
  This function removes all trailing whitespace characters from the `String` object.
- **Parameters**:
  - `str`: The `String` object to be trimmed. Must not be `NULL`.
- **Functionality**:
  - Iterates backward over the string data and trims all trailing whitespace characters.
  - Adjusts the string size and content accordingly.
- **Return Value**:  
  This function does not return a value.

---

#### `void string_trim(String *str)`
- **Purpose**:  
  This function removes both leading and trailing whitespace characters from the `String` object by combining `string_trim_left` and `string_trim_right`.
- **Parameters**:
  - `str`: The `String` object to be trimmed. Must not be `NULL`.
- **Functionality**:
  - Calls the `string_trim_left` and `string_trim_right` functions in sequence to remove both leading and trailing whitespaces.
- **Return Value**:  
  This function does not return a value.

---

#### `String** string_split(const String *str, const char *delimiter, int *count)`
- **Purpose**:  
  Splits a `String` object into multiple `String` objects based on a specified delimiter.
- **Parameters**:
  - `str`: The `String` object to split. Must not be `NULL`.
  - `delimiter`: The delimiter used to split the string. Must not be `NULL`.
  - `count`: A pointer to an integer where the number of splits will be stored.
- **Functionality**:
  - Splits the `String` object into multiple strings based on the provided delimiter.
  - Stores the split results in an array and returns the number of splits through `count`.
- **Return Value**:  
  An array of `String*` containing the split strings, or `NULL` if an error occurs.

---

#### `String* string_join(String **strings, int count, const char *delimiter)`
- **Purpose**:  
  Joins an array of `String` objects into a single `String` object, separated by a specified delimiter.
- **Parameters**:
  - `strings`: The array of `String` pointers to be joined. Must not be `NULL`.
  - `count`: The number of `String` objects in the array. Must be greater than 0.
  - `delimiter`: The delimiter to be used between each string. Must not be `NULL`.
- **Functionality**:
  - Concatenates all the strings in the array into a single `String`, separated by the delimiter.
- **Return Value**:  
  A new `String` object containing the concatenated result, or `NULL` if an error occurs.

---

#### `void string_replace_all(String *str, const char *oldStr, const char *newStr)`
- **Purpose**:  
  Replaces all occurrences of a substring (`oldStr`) within a `String` object with another substring (`newStr`).
- **Parameters**:
  - `str`: The `String` object in which the replacement occurs. Must not be `NULL`.
  - `oldStr`: The substring to be replaced. Must not be `NULL`.
  - `newStr`: The substring to replace `oldStr` with. Must not be `NULL`.
- **Functionality**:
  - Iterates through the `String` and replaces every occurrence of `oldStr` with `newStr`.
- **Return Value**:  
  This function does not return a value.

---

#### `int string_to_int(const String *str)`
- **Purpose**:  
  This function converts the content of a `String` object to an integer. If the `String` is `NULL` or empty, it returns 0.
- **Parameters**:
  - `str`: The `String` object to convert. Must not be `NULL`.
- **Functionality**:
  - Validates if the string is not `NULL` or empty.
  - Converts the string content to an integer using `atoi`.
- **Return Value**:  
  The integer value of the `String`'s content or 0 if the string is `NULL` or empty.

---

#### `float string_to_float(const String *str)`
- **Purpose**:  
  Converts the content of a `String` object to a floating-point number (float).
- **Parameters**:
  - `str`: The `String` object to convert. Must not be `NULL`.
- **Functionality**:
  - Validates if the string is not `NULL` or empty.
  - Converts the string content to a float using `atof`.
- **Return Value**:  
  The floating-point value of the `String`'s content or 0.0f if the string is `NULL` or empty.

---

#### `double string_to_double(const String* str)`
- **Purpose**:  
  Converts the content of a `String` object to a double-precision floating-point number (double).
- **Parameters**:
  - `str`: The `String` object to convert. Must not be `NULL`.
- **Functionality**:
  - Validates if the string is not `NULL` or empty.
  - Converts the string content to a double using `strtod`.
- **Return Value**:  
  The double value of the `String`'s content or 0.0 if the string is `NULL` or empty.

---

#### `void string_pad_left(String *str, size_t totalLength, char padChar)`
- **Purpose**:  
  Pads the left side of a `String` object with a specified character until it reaches a desired length.
- **Parameters**:
  - `str`: The `String` object to be padded. Must not be `NULL`.
  - `totalLength`: The total length the string should be after padding.
  - `padChar`: The character to use for padding.
- **Functionality**:
  - If the string is shorter than `totalLength`, it pads the left side with `padChar`.
  - If memory allocation fails, it logs an error.
- **Return Value**:  
  This function does not return a value.

---

#### `void string_pad_right(String *str, size_t totalLength, char padChar)`
- **Purpose**:  
  Pads the right side of a `String` object with a specified character until it reaches a desired length.
- **Parameters**:
  - `str`: The `String` object to be padded. Must not be `NULL`.
  - `totalLength`: The total length the string should be after padding.
  - `padChar`: The character to use for padding.
- **Functionality**:
  - If the string is shorter than `totalLength`, it pads the right side with `padChar`.
  - If memory allocation fails, it logs an error.
- **Return Value**:  
  This function does not return a value.

---

#### `String* string_to_hex(const String *str)`
- **Purpose**:  
  Converts the content of a `String` object to its hexadecimal representation.
- **Parameters**:
  - `str`: The `String` object to convert to hexadecimal. Must not be `NULL`.
- **Functionality**:
  - Converts each character in the string to its corresponding hexadecimal value.
  - Returns a new `String` object containing the hexadecimal representation.
- **Return Value**:  
  A new `String` object containing the hexadecimal representation, or `NULL` if an error occurs.

---

#### `String* string_from_hex(const String *hexStr)`
- **Purpose**:  
  Converts a hex-encoded string (e.g., "48656c6c6f") into a new `String` object containing the corresponding ASCII characters (e.g., "Hello").
- **Parameters**:
  - `hexStr`: The hex-encoded `String` object. Must not be `NULL` and must have an even number of characters.
- **Functionality**:
  - Validates the input and checks if the hex string length is even.
  - Converts every two hex digits into a character and appends it to a new `String` object.
- **Return Value**:  
  A new `String` object containing the decoded ASCII characters, or `NULL` if an error occurs.

---

#### `size_t string_count(const String* str, const char* substr)`
- **Purpose**:  
  Counts how many times a given substring appears within a `String` object.
- **Parameters**:
  - `str`: The `String` object in which to search for the substring. Must not be `NULL`.
  - `substr`: The substring to count within the `String` object. Must not be `NULL`.
- **Functionality**:
  - Searches for occurrences of `substr` in the string and counts how many times it appears.
- **Return Value**:  
  The number of occurrences of `substr`, or `0` if an error occurs.

---

#### `void string_remove(String* str, const char* substr)`
- **Purpose**:  
  Removes all instances of a specified substring from a `String` object.
- **Parameters**:
  - `str`: The `String` object from which to remove the substring. Must not be `NULL`.
  - `substr`: The substring to remove. Must not be `NULL` or empty.
- **Functionality**:
  - Finds and removes all occurrences of `substr` from the string.
- **Return Value**:  
  This function does not return a value.

---

#### `void string_remove_range(String* str, size_t startPos, size_t endPos)`
- **Purpose**:  
  Removes characters from the `String` object within a specified range.
- **Parameters**:
  - `str`: The `String` object from which to remove characters. Must not be `NULL`.
  - `startPos`: The starting position of the range to remove.
  - `endPos`: The ending position of the range to remove.
- **Functionality**:
  - Removes characters from `startPos` to `endPos` and adjusts the size of the `String`.
- **Return Value**:  
  This function does not return a value.

---

#### `String* string_from_int(int value)`
- **Purpose**:  
  Creates a new `String` object from an integer value.
- **Parameters**:
  - `value`: The integer to convert.
- **Functionality**:
  - Converts the integer to a string and returns a new `String` object containing the result.
- **Return Value**:  
  A new `String` object containing the string representation of the integer.

---

#### `char* string_from_int_cstr(int value)`
- **Purpose**:  
  Converts an integer to its C-string representation.
- **Parameters**:
  - `value`: The integer to convert.
- **Functionality**:
  - Converts the integer to a C-string and allocates memory for it. The caller is responsible for freeing the memory.
- **Return Value**:  
  A newly allocated C-string containing the integer's string representation.

---

#### `String* string_from_float(float value)`
- **Purpose**:  
  Converts a floating-point number to its string representation and returns a new `String` object.
- **Parameters**:
  - `value`: The float value to convert.
- **Functionality**:
  - Converts the float value into a string and creates a new `String` object.
- **Return Value**:  
  A new `String` object containing the string representation of the float value.

---

#### `String* string_from_double(double value)`
- **Purpose**:  
  Converts a double-precision floating-point number to its string representation and returns a new `String` object.
- **Parameters**:
  - `value`: The double value to convert.
- **Functionality**:
  - Converts the double value into a string and creates a new `String` object.
- **Return Value**:  
  A new `String` object containing the string representation of the double value.

---

#### `String** string_tokenize(const String* str, const char* delimiters, int* count)`
- **Purpose**:  
  Tokenizes a `String` object into an array of tokens based on the specified delimiters.
- **Parameters**:
  - `str`: The `String` object to tokenize. Must not be `NULL`.
  - `delimiters`: A C-string containing delimiter characters.
  - `count`: A pointer to an integer to store the number of tokens.
- **Functionality**:
  - Splits the string into tokens based on the provided delimiters and returns an array of `String` objects.
- **Return Value**:  
  An array of `String` objects representing the tokens, or `NULL` if an error occurs.

---

#### `int string_compare_ignore_case(const String* str1, const String* str2)`
- **Purpose**:  
  Compares two `String` objects in a case-insensitive manner.
- **Parameters**:
  - `str1`: The first `String` object to compare.
  - `str2`: The second `String` object to compare.
- **Functionality**:
  - Performs a case-insensitive comparison between two strings.
- **Return Value**:  
  An integer indicating the result of the comparison: 0 if equal, negative if `str1` is less than `str2`, and positive if `str1` is greater than `str2`.

---

#### `String* string_base64_encode(const String *input)`
- **Purpose**:  
  Encodes the content of a `String` object into Base64 format.
- **Parameters**:
  - `input`: The `String` object to encode.
- **Functionality**:
  - Converts the string's content into Base64 encoding.
- **Return Value**:  
  A new `String` object containing the Base64-encoded data, or `NULL` if an error occurs.

---

#### `String* string_base64_decode(const String* encodedStr)`
- **Purpose**:  
  Decodes a Base64-encoded `String` object into its original binary form.
- **Parameters**:
  - `encodedStr`: The Base64-encoded `String` object. Must not be `NULL`.
- **Functionality**:
  - Decodes the Base64 string into its original binary form.
- **Return Value**:  
  A new `String` object containing the decoded binary data, or `NULL` if an error occurs.

---

#### `void string_format(String* str, const char* format, ...)`
- **Purpose**:  
  Formats the content of a `String` object based on a format string and additional arguments, similar to how `printf` works.
- **Parameters**:
  - `str`: The `String` object to format.
  - `format`: A format string specifying how to format the content.
  - `...`: Additional arguments for formatting.
- **Functionality**:
  - Formats the content of the string based on the format and arguments and assigns it to the `String` object.

---

#### `String* string_repeat(const String* str, size_t count)`
- **Purpose**:  
  Creates a new `String` object containing the content of the input `String` repeated a specified number of times.
- **Parameters**:
  - `str`: The `String` object to repeat.
  - `count`: The number of times to repeat the string.
- **Functionality**:
  - Repeats the content of the string `count` times and returns a new `String` object.

---

#### `String* string_join_variadic(size_t count, ...)`
- **Purpose**:  
  Joins multiple `String` objects passed as variadic arguments into a single `String` object.
- **Parameters**:
  - `count`: The number of `String` objects to join.
  - `...`: The `String` objects to join.
- **Functionality**:
  - Joins the strings and returns a new `String` object containing the concatenated content.

---

#### `void string_trim_characters(String* str, const char* chars)`
- **Purpose**:  
  Removes specified characters from both the beginning and end of a `String` object.
- **Parameters**:
  - `str`: The `String` object to trim.
  - `chars`: A C-string containing characters to be trimmed from both ends of the string.
- **Functionality**:
  - Trims any occurrence of the specified characters from both the beginning and the end of the `String` object.

---

#### `void string_shuffle(String* str)`
- **Purpose**:  
  Shuffles the characters in the `String` object randomly.
- **Parameters**:
  - `str`: The `String` object to shuffle. Must not be `NULL`.
- **Functionality**:
  - Uses the Fisher-Yates algorithm to shuffle the characters in the string randomly.

---

#### `void string_to_title(String* str)`
- **Purpose**:  
  Converts the `String` object to title case, where the first letter of each word is capitalized, and the other letters are lowercase.
- **Parameters**:
  - `str`: The `String` object to convert. Must not be `NULL`.
- **Functionality**:
  - Capitalizes the first letter of each word while converting the rest of the characters to lowercase.

---

#### `void string_to_capitalize(String* str)`
- **Purpose**:  
  Capitalizes the first character of the `String` object.
- **Parameters**:
  - `str`: The `String` object to capitalize. Must not be `NULL`.
- **Functionality**:
  - Converts the first character of the string to uppercase if it exists.

---

#### `void string_to_casefold(String* str)`
- **Purpose**:  
  Converts all characters in the `String` object to lowercase.
- **Parameters**:
  - `str`: The `String` object to convert. Must not be `NULL`.
- **Functionality**:
  - Iterates through the string and converts each character to lowercase.

---

#### `bool string_starts_with(const String* str, const char* substr)`
- **Purpose**:  
  Checks if the `String` object starts with the specified substring.
- **Parameters**:
  - `str`: The `String` object to check. Must not be `NULL`.
  - `substr`: The substring to compare at the start of the string. Must not be `NULL`.
- **Functionality**:
  - Returns `true` if the string starts with `substr`, otherwise `false`.

---

#### `bool string_ends_with(const String* str, const char* substr)`
- **Purpose**:  
  Checks if the `String` object ends with the specified substring.
- **Parameters**:
  - `str`: The `String` object to check. Must not be `NULL`.
  - `substr`: The substring to compare at the end of the string. Must not be `NULL`.
- **Functionality**:
  - Returns `true` if the string ends with `substr`, otherwise `false`.

---

#### `void string_swap_case(String* str)`
- **Purpose**:  
  Swaps the case of each character in the `String` object, converting lowercase characters to uppercase and vice versa.
- **Parameters**:
  - `str`: The `String` object to swap case. Must not be `NULL`.
- **Functionality**:
  - Converts lowercase characters to uppercase and vice versa.

---

#### `wchar_t* string_to_unicode(const char* str)`
- **Purpose**:  
  Converts a multi-byte C-string to a wide-character (Unicode) string.
- **Parameters**:
  - `str`: The C-string to convert. Must not be `NULL`.
- **Functionality**:
  - Converts the string to a wide-character string using `mbstowcs`.

---

#### `String* string_from_unicode(const wchar_t* wstr)`
- **Purpose**:  
  Converts a wide-character string to a multi-byte C-string and creates a new `String` object from it.
- **Parameters**:
  - `wstr`: The wide-character string to convert. Must not be `NULL`.
- **Functionality**:
  - Converts the wide-character string to a multi-byte string using `wcstombs` and creates a new `String` object.

---

#### `String** string_create_from_initializer(size_t count, ...)`
- **Purpose**:  
  Creates an array of `String` objects from a variable number of C-strings passed as variadic arguments.
- **Parameters**:
  - `count`: The number of C-strings to convert to `String` objects.
  - `...`: A variable number of C-strings to convert.
- **Functionality**:
  - Converts each C-string to a `String` object and returns an array of pointers to these objects, with the array being null-terminated.

---

#### `char* string_strdup(const char* str)`
- **Purpose**:  
  Duplicates the provided C-string by allocating new memory and copying its content.
- **Parameters**:
  - `str`: The C-string to duplicate. Must not be `NULL`.
- **Functionality**:
  - Allocates memory for a copy of the input string and returns a pointer to the new string.

---

#### `size_t string_length_cstr(const char* str)`
- **Purpose**:  
  Calculates the length of a C-string (without counting the null terminator).
- **Parameters**:
  - `str`: A pointer to a C-string. Must not be `NULL`.
- **Return Value**:
  - The length of the C-string, or `0` if the string is `NULL`.

---

#### `size_t string_length_utf8(const char* str)`
- **Purpose**:  
  Calculates the number of characters in a UTF-8 encoded C-string, correctly accounting for multi-byte characters.
- **Parameters**:
  - `str`: A pointer to a UTF-8 encoded C-string. Must not be `NULL`.
- **Return Value**:
  - The number of UTF-8 characters in the string, or `0` if the string is `NULL`.

---

#### `bool string_to_bool_from_cstr(const char* boolstr)`
- **Purpose**:  
  Converts a C-string to a boolean value based on its content. The comparison is case-sensitive.
- **Parameters**:
  - `boolstr`: A C-string, expected to be "true" or "false".
- **Return Value**:
  - `true` if the string is "true", `false` if it is "false" or any other value (including `NULL`).

---

#### `size_t string_utf8_char_len(char c)`
- **Purpose**:  
  Determines how many bytes a UTF-8 character occupies based on its first byte.
- **Parameters**:
  - `c`: The first byte of the UTF-8 character.
- **Return Value**:
  - The number of bytes the UTF-8 character occupies (1-4), or `0` if it is not a valid UTF-8 start byte.

---

#### `int string_strcmp(const char* str1, const char* str2)`
- **Purpose**:  
  Compares two C-strings using `strcmp` and returns the result.
- **Parameters**:
  - `str1`, `str2`: Pointers to the C-strings to compare.
- **Return Value**:
  - The result of the `strcmp` comparison:  
    - `<0` if `str1` is less than `str2`,  
    - `0` if they are equal,  
    - `>0` if `str1` is greater than `str2`.

---

#### `char* string_strndup(const char* str, size_t n)`
- **Purpose**:  
  Duplicates the first `n` characters of a C-string and returns a pointer to the newly allocated memory.
- **Parameters**:
  - `str`: The input C-string to duplicate.
  - `n`: The maximum number of characters to duplicate.
- **Return Value**:
  - A pointer to the newly allocated string, or `NULL` if memory allocation fails.

---

#### `bool string_is_palindrome(const String* str)`
- **Purpose**:  
  Checks if the content of a `String` object is a palindrome (reads the same forwards and backwards).
- **Parameters**:
  - `str`: The `String` object to check.
- **Return Value**:
  - `true` if the string is a palindrome, `false` otherwise.

---

#### `bool string_is_alnum(const String* str)`
- **Purpose**:  
  Checks if a `String` object contains only alphanumeric characters (letters and digits).
- **Parameters**:
  - `str`: The `String` object to check.
- **Return Value**:
  - `true` if the string is alphanumeric, `false` otherwise.

---

#### `bool string_is_title(const String* str)`
- **Purpose**:  
  Checks if a `String` object is in title case (each word starts with an uppercase letter and all other letters are lowercase).
- **Parameters**:
  - `str`: The `String` object to check.
- **Return Value**:
  - `true` if the string is in title case, `false` otherwise.

---

#### `bool string_is_space(const String* str)`
- **Purpose**:  
  Checks if a `String` object contains only space characters.
- **Parameters**:
  - `str`: The `String` object to check.
- **Return Value**:
  - `true` if the string contains only spaces, `false` otherwise.

---

#### `bool string_is_printable(const String* str)`
- **Purpose**:  
  Checks if all characters in a `String` object are printable (ASCII values greater than 31).
- **Parameters**:
  - `str`: The `String` object to check.
- **Return Value**:
  - `true` if the string contains only printable characters or is empty, `false` otherwise.


---

#### `void string_reserve(String *str, size_t newCapacity)`
  - **Purpose:** The purpose of `string_reserve` is to ensure that a `String` object has enough capacity to store at least `newCapacity` characters. If the current capacity of the string is less than `newCapacity`, it reallocates memory to increase the capacity. 
  - **Parameters:**
   - `str`** (`String*`): A pointer to the `String` object whose capacity you want to reserve. 
   - `newCapacity` (`size_t`): The minimum number of characters that the `String` object should be able to store.
  - **Return Value:** The function does not return a value (`void`). If the string's capacity is insufficient, it reallocates memory and increases the capacity. If memory allocation fails, the 

---

### Example 1: how to create String obj and use `string_push_back`, `string_append`, `string_at`, `string_clear`, `string_length`, `string_capacity` also `string_deallocate`

```c

#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* myString = string_create("");
   
    // Push back a new character
    string_push_back(myString, '!');
    fmt_printf("String after push_back: %s\n", myString->dataStr);

    // Access a character
    char ch = string_at(myString, 0);
    fmt_printf("Character at index 1: %c\n", ch);

    // Print length and capacity
    fmt_printf("Length: %zu, Capacity: %zu\n", string_length(myString), string_capacity(myString));

    // Clear the string
    string_clear(myString);
    fmt_printf("String after clear: %s\n", myString->dataStr);

    // Clean up
    string_clear(myString);
    string_append(myString, "Hello C Programmers");
        
    fmt_printf("%s\n", string_c_str(myString));

    string_deallocate(myString);
    return 0;
}
```
**Result:**
```
String after push_back: !
Character at index 1: !
Length: 1, Capacity: 32
Info : String object is null no need to clear in string_clear.
String after clear:
Info : String object is null no need to clear in string_clear.
Hello C Programmers
```

---

## Example 2: `string_shrink_to_fit`, `string_resize`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello");

    // Append a string
    string_append(myString, ", World!");
    fmt_printf("String after append: %s\n", myString->dataStr);

    // Resize the string
    string_resize(myString, 5);  // Resize to "Hello"
    fmt_printf("String after resize: %s\n", myString->dataStr);

    // Shrink to fit
    string_shrink_to_fit(myString);
    fmt_printf("String capacity after shrink to fit: %zu\n", string_capacity(myString));

    // clean up and dellocate
    string_clear(myString);
    string_deallocate(myString);

    return 0;
}
```
**Result:**
```
String after append: Hello, World!
String after resize: Hello
String capacity after shrink to fit: 6
Info : String object is null no need to clear in string_clear.
```

---

## Example 3: how to `string_assign` new string and `string_erase` some part of String obj also `string_insert` new ...  

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello World");

    // Assign a new string
    string_assign(myString, "New String");
    fmt_printf("String after assign: %s\n", myString->dataStr);

    // Insert a string
    string_insert(myString, 4, "Test ");
    fmt_printf("String after insert: %s\n", myString->dataStr);

    // Erase a portion of the string
    string_erase(myString, 0, 5);  // Erase "New T"
    fmt_printf("String after erase: %s\n", myString->dataStr);

    // Deallocate and clean up
    string_clear(myString);
    string_deallocate(myString);

    return 0;
}
```
**Result:**
```
String after assign: New String
String after insert: New Test String
String after erase: est String
Info : String object is null no need to clear in string_clear.
```

---

## Example 4: `string_replace` Strings 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *str1 = string_create("Hello World");
    string_replace(str1, "World", "There");
        
    fmt_printf("After replace: %s\n", str1->dataStr);

    string_deallocate(str1);
    return 0;
}
```
**Result:**
```
After replace: Hello There
```

---

## Example 5: `string_swap` two String obj and also `string_pop_back`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *str1 = string_create("Hello World");
    string_replace(str1, "World", "There");
        
    fmt_printf("After replace: %s\n", str1->dataStr);

    // Swap example
    String *str2 = string_create("Sample Text");
    string_swap(str1, str2);

    fmt_printf("After swap, str1: %s, str2: %s\n", str1->dataStr, str2->dataStr);

    // Pop back example
    string_pop_back(str2);
    fmt_printf("After pop back: %s\n", str2->dataStr);

    string_deallocate(str1);
    string_deallocate(str2);

    return 0;
}
```
**Result:**
```
After replace: Hello There
After swap, str1: Sample Text, str2: Hello There
After pop back: Hello Ther
```

---

## Example 6: `string_back` and `string_front` return ref to front and last char of String 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello World");
    char lastChar = *(char*)string_back(myString); // Get the last character

    fmt_printf("Last character : %c\n", lastChar);
    // its just syntax !!!
    *(char*)string_back(myString) = 'a';
    fmt_printf("Wowww: %c\n", *(char*)string_back(myString));

    // Get the first character
    char firstChar = *(char*)string_front(myString);
    fmt_printf("First character: %c\n", firstChar);
    fmt_printf("Max size is %zu\n", string_max_size(myString));
        
    // Deallocate and clean up
    string_clear(myString);
    string_deallocate(myString);

    return 0;
}
```
**Result:**
```
Last character : d
Wowww: a
First character: H
Max size is 18446744073709551615
Info : String object is null no need to clear in string_clear.
```

---

## Example 7 : `string_copy` how to copy some part of String 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello, World!");

    // Copy example
    char buffer[50];
    size_t copied = string_copy(myString, buffer, 7, 5);
    fmt_printf("Copied '%s' (%zu characters)\n", buffer, copied);

    string_deallocate(myString);
    return 0;
}
```
**Result:**
```
Copied 'World' (5 characters)
```

---

## Example 8 : `string_find` find string in String and return position

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello, World!");

    // Find example
    int findPos = string_find(myString, "World", 0);
    fmt_printf("Found 'World' at position: %d\n", findPos);

    string_deallocate(myString);
    return 0;
}
```
**Result:**
```
Found 'World' at position: 7
```

---

## Example 9 : how to use `string_rfind`, `string_find_first_of`, `string_find_last_of`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello, World!");

    // rfind example
    int rfindPos = string_rfind(myString, "o,", string_length(myString) - 1);
    fmt_printf("Last 'o' found at position: %d\n", rfindPos);

    int findFirstOfPos = string_find_first_of(myString, "World", 0);
    fmt_printf("First occurrence of 'World' found at position: %d\n", findFirstOfPos);

    // find_last_of example
    int findLastOfPos = string_find_last_of(myString, "World", string_length(myString) - 1);
    fmt_printf("Last occurrence of 'World' found at position: %d\n", findLastOfPos);

    // Deallocate and clean up
    string_clear(myString);
    string_deallocate(myString);

    return 0;
}
```
**Result:**
```
Last 'o' found at position: 4
First occurrence of 'World' found at position: 7
Last occurrence of 'World' found at position: 7
Info : String object is null no need to clear in string_clear.
```

---

## Example 10 : `string_find_first_not_of`, `string_find_last_not_of`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello, World!");

    // find_first_not_of example
    int firstNotOfPos = string_find_first_not_of(myString, "Hello", 0);
    fmt_printf("First position not matching 'Hello': %d\n", firstNotOfPos);

    // find_last_not_of example
    int lastNotOfPos = string_find_last_not_of(myString, "World", string_length(myString) - 1);
    fmt_printf("Last position not matching 'World': %d\n", lastNotOfPos);

    string_deallocate(myString);
    return 0;
}
```
**Result:**
```
First position not matching 'Hello': 1
Last position not matching 'World': 8
```

---

## Example 11 : `string_data` return pointer to String sequence 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello, World!");
        
    // Data example
    const char *data = string_data(myString);
    fmt_printf("Data: %s\n", data);

    // Deallocate and clean up
    string_clear(myString);
    string_deallocate(myString);

    return 0;
}
```
**Result:**
```
Data: Hello, World!
Info : String object is null no need to clear in string_clear.
```

---

## Example 12 : `string_begin`, `string_end` return iterator pointer from begin and end of String sequence

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello amin");

    for (char* it = string_begin(myString); it != string_end(myString); it++) {
        fmt_printf("%c", *it);
    }
    
    fmt_printf("\n");

    string_clear(myString);
    string_deallocate(myString);

    return 0;
}
```
**Result:**
```
Hello amin
Info : String object is null no need to clear in string_clear.
```

---

## Example 13 : `string_rbeing`, `string_rend` return reverse iterator 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello amin");

    // Reverse iteration using rbegin and rend
    fmt_printf("String in reverse: ");
    for (char* it = string_rbegin(myString); it != string_rend(myString); --it) { 
        fmt_printf("%c", *it);
    }   
    fmt_printf("\n");

    // Clean up
    string_clear(myString);
    string_deallocate(myString);

    return 0;
}
```
**Result:**
```
String in reverse: nima olleH
Info : String object is null no need to clear in string_clear.
```

---

## Example 14 : `string_cbegin`, `string_cend`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello, World!");

    // Iterate using cbegin and cend
    fmt_printf("String using cbegin and cend: ");
    for (const char* it = string_cbegin(myString); it != string_cend(myString); ++it) { 
        fmt_printf("%c", *it);
    }
    fmt_printf("\n");

    string_deallocate(myString);
    return 0;
}
```
**Result:**
```
String using cbegin and cend: Hello, World!
```

---

## Example 15 : `string_crbegin`, `string_crend` 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello, World!");

    // Reverse iterate using crbegin and crend
    fmt_printf("String in reverse using crbegin and crend: ");
    for (const char* it = string_crbegin(myString); it != string_crend(myString); --it) { 
        fmt_printf("%c", *it);
    }
    fmt_printf("\n");

    string_deallocate(myString);
    return 0;
}
```
**Result:**
```
String in reverse using crbegin and crend: !dlroW ,olleH
```

---

## Example 16 : how to use relationals operators in String 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str1 = string_create("Hello");
    String* str2 = string_create("World");

    if (string_is_equal(str1, str2)) {
        fmt_printf("Strings are equal\n");
    }   
    if (string_is_less(str1, str2)) { 
        fmt_printf("String 1 is less than String 2\n");
    }
    if (string_is_greater(str1, str2)) {
        fmt_printf("String1 is greater that String2\n");
    }
    if (string_is_less_or_equal(str1, str2)) {
        fmt_printf("String1 is less than or equal String2\n");
    }
    if (string_is_greater_or_equal(str1, str2)) {
        fmt_printf("String1 is greater than or equal String2\n");
    }

    string_deallocate(str1);
    string_deallocate(str2);

    return 0;
}
```
**Result:**
```
String 1 is less than String 2
String1 is less than or equal String2
```

---

## Example 17: Manipulate Multiple String 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    // Creating string objects
    String* greeting = string_create("Hello");
    String* name = string_create("Alice");
    String* sentence = string_create("");

    // Concatenate 'greeting' and ', '
    string_append(greeting, ", ");
    fmt_printf("Greeting: %s\n", greeting->dataStr);

    // Append 'name' to 'greeting'
    string_append(greeting, name->dataStr);
    fmt_printf("Greeting with name: %s\n", greeting->dataStr);

    // Create a substring of 'greeting' and assign it to 'sentence'
    String* tempSubstr = string_substr(greeting, 0, 5); // Extract "Hello"
    string_assign(sentence, tempSubstr->dataStr);
    string_deallocate(tempSubstr);

    // Append '!' to 'sentence'
    string_push_back(sentence, '!');
    fmt_printf("Sentence: %s\n", sentence->dataStr);

    // Compare 'greeting' and 'sentence'
    if (string_is_less(sentence, greeting)) {
        fmt_printf("Sentence is less than greeting.\n"); 
    }
    else {
        fmt_printf("Sentence is not less than greeting.\n");
    }

    string_deallocate(greeting);
    string_deallocate(name);
    string_deallocate(sentence);

    return 0;
}
```
**Result:**
```
Greeting: Hello, 
Greeting with name: Hello, Alice
Sentence: Hello!
Sentence is less than greeting.
```

---

## Example 18: 2D String Array using Custom String Struct

```c
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

String*** create_2d_string_array(const size_t rows, const size_t cols) {
    String*** array = malloc(rows * sizeof(String**));

    for (size_t i = 0; i < rows; ++i) {
        array[i] = malloc(cols * sizeof(String*));

        for (size_t j = 0; j < cols; ++j) { 
            array[i][j] = string_create("");  // Initialize with empty strings
        }
    }
    return array;
}

void deallocate_2d_string_array(String*** array, const size_t rows, const size_t cols) {
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            string_deallocate(array[i][j]);
        }
        free(array[i]);
    }
    free(array);
}

int main() {
    const size_t rows = 2;
    const size_t cols = 3;
    String*** my2DString = create_2d_string_array(rows, cols);

    // Example usage
    string_assign(my2DString[0][0], "Hello");
    string_assign(my2DString[0][1], "World");
    string_assign(my2DString[0][2], "!");

    string_assign(my2DString[1][0], "Goodbye");
    string_assign(my2DString[1][1], "Cruel");
    string_assign(my2DString[1][2], "World");

    // Print the 2D array
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            fmt_printf("%s ", string_c_str(my2DString[i][j]));
        }
        fmt_printf("\n");
    }
    // Clean up
    deallocate_2d_string_array(my2DString, rows, cols);
    return 0;
}
```
**Result:**
```
Hello World ! 
Goodbye Cruel World
```

---

## Example 19 : bench mark operation in `String` and `std::string` 

gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main .\main.c .\string\string.c
Time taken (Custom String): 0.286000 seconds

```c
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 100000
#define OPERATIONS 100

int main() {
    struct timespec start, end;
    String** stringArray = malloc(ARRAY_SIZE * sizeof(String*));

    if (!stringArray) {
        fmt_fprintf(stderr, "Failed to allocate memory for stringArray");
        return 1; // Or handle error appropriately
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        // stringArray[i] = string_create_with_pool(100000000); // if you use this method for largeArray speed efficient but need to large memory
        stringArray[i] = string_create(""); // but this one is more slower than create_pool
    }
    
    for (int i = 0; i < ARRAY_SIZE; i++) {
        for (int j = 0; j < OPERATIONS; j++) { 
            string_push_back(stringArray[i], 'a');
        } 
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_in_sec = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    fmt_printf("Average Custom Vector Time: %f seconds\n", time_in_sec);

    for (int i = 0; i < ARRAY_SIZE; i++) { 
        string_deallocate(stringArray[i]);
    }

    return 0;
}
```
**Result in C:**
```
Average Custom Vector Time: 0.007211 seconds
```

```c++
#include <iostream>
#include <string>
#include <chrono>

#define ARRAY_SIZE 1000000
#define OPERATIONS 100

int main() {
    std::string *stringArray = new std::string[ARRAY_SIZE];
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ARRAY_SIZE; i++) {
        for (int j = 0; j < OPERATIONS; j++) {
            stringArray[i] += 'a';
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time taken (std::string): " << elapsed.count() << " seconds\n";

    return 0;
}
```
**Result in C++:**
```
Time taken (std::string): 0.81031 seconds
```

---

## Example 20 : benchmark of `String` and `std::string`


```c
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <time.h>

#define ARRAY_SIZE 10000
#define OPERATIONS 100

int main() {
    struct timespec start, end;
    String* stringArray = string_create("");
    
    if (!stringArray) {
        fmt_fprintf(stderr, "Failed to allocate memory for stringArray.\n");
        return 1; // Or handle error appropriately
    }
    
    if (string_set_pool_size(stringArray, 10000000)) {
        fmt_printf("create pool\n");
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ARRAY_SIZE; i++) {
        string_push_back(stringArray, 'a'); 
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    double time_in_sec = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    fmt_printf("Average Custom Vector Time: %f seconds\n", time_in_sec);

    for (int i = 0; i < ARRAY_SIZE; i++) {
        string_deallocate(stringArray);
    }

    return 0;
}
```
**Result in C:**
```
create pool
Average Custom Vector Time: 0.000034 seconds
```

`Cpp Code`

```cpp 
#include <iostream>
#include <string>
#include <chrono>

#define ARRAY_SIZE 1000000
#define OPERATIONS 100

int main() {
    std::string *stringArray = new std::string[ARRAY_SIZE];
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ARRAY_SIZE; i++) {
        stringArray[i] += 'a';
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time taken (std::string): " << elapsed.count() << " seconds\n";

    return 0;
}
```
**Result in C++:**
```
Time taken (std::string): 0.0065333 seconds
```

---

### Example 21 : string to Lower or Upper 
```c
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    String* str = string_create("Hello World");
    char* toUpper = string_to_upper(str); // convert str to upper case and return as char*
    char* toLower = string_to_lower(str); // convert str to lower case and return as char* 

    if (toUpper) {   
        fmt_printf("To Upper is -> %s\n", toUpper);
        free(toUpper);
    }
    if (toLower) {
        fmt_printf("To Lower is -> %s\n", toLower);
        free(toLower);
    }

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
To Upper is -> HELLO WORLD
To Lower is -> hello world
```

---

### Example 22 : Reverse String with `string_reverse`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    String* str = string_create("Hello World");

    if (string_contains(str, "Hello")) { 
        fmt_printf("Yes 'Hello' exists in str\n");
    }
    
    string_reverse(str); // reverse String 
    fmt_printf("Reverse String is %s\n", string_c_str(str));

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Yes 'Hello' exists in str
Reverse String is dlroW olleH
```

---

### Example 23 : `string_is_digit`

```c
#include "string/std_string.h" 
#include "fmt/fmt.h"

int main() {
    String* myString = string_create("123456");

    if (string_is_digit(myString)) {
        fmt_printf("The string contains only digits.\n");
    }
    else {
        fmt_printf("The string contains non-digit characters or is not a string.\n");
    }

    string_deallocate(myString);
    return 0;
}
```
**Result:**
```
The string contains only digits.
```

---

### Example 24 : `string_is_lower`

```c
#include "string/std_string.h"
#include "fmt/fmt.h" 
#include <stdlib.h>

int main() {
    String* myString = string_create("Hello World");
    
    char* upper = string_to_upper(myString);
    fmt_printf("Uppercase: %s\n", upper);
    free(upper); // Remember to free the memory allocated by string_to_upper

    char* lower = string_to_lower(myString);
    fmt_printf("Lowercase: %s\n", lower);
    free(lower); // Remember to free the memory allocated by string_to_lower

    string_deallocate(myString);
    return 0;
}
```
**Result:**
```
Uppercase: HELLO WORLD
Lowercase: hello world
```

---

### Example 25 : concatenate two String with `string_concatenate`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    String* str1 = string_create("Hello");
    String* str2 = string_create(" World");

    string_concatenate(str1, str2);
    fmt_printf("Concatenated string: %s\n", string_c_str(str1));

    string_deallocate(str1);
    string_deallocate(str2);

    return 0;
}
```
**Result:**
```
Concatenated string: Hello World
```

---

### Example 26 : trim String from start, end or both 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    String* str = string_create("   Hello World   ");

    string_trim_left(str);
    fmt_printf("Trimmed Start: '%s'\n", string_c_str(str));

    string_trim_right(str);
    fmt_printf("Trimmed End: '%s'\n", string_c_str(str));

    string_clear(str);
    string_assign(str, "   Hello World   ");
    string_trim(str);
    
    fmt_printf("Trimmed Both: '%s'\n", string_c_str(str));

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Trimmed Start: 'Hello World   '
Trimmed End: 'Hello World'
Info : String object is null no need to clear in string_clear.
Trimmed Both: 'Hello World'
```

---

### Example 27 : split String with `string_split`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    String* str = string_create("Hello,World,This,Is,A,Test");
    int count = 0;
    String** splits = string_split(str, ",", &count);

    for (int i = 0; i < count; i++) {
        fmt_printf("Split %d: %s\n", i, string_c_str(splits[i]));
        string_deallocate(splits[i]);
    }
    free(splits);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Split 0: Hello
Split 1: World
Split 2: This
Split 3: Is
Split 4: A
Split 5: Test
```

---

### Example 28 : joins several String objects with `string_join` 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    String* parts[3];

    parts[0] = string_create("Hello");
    parts[1] = string_create("World");
    parts[2] = string_create("Again");

    String* joined = string_join(parts, 3, " ");
    fmt_printf("Joined string: %s\n", string_c_str(joined));

    for (int i = 0; i < 3; i++) {
        string_deallocate(parts[i]);
    }

    string_deallocate(joined);
    return 0;
}
```
**Result:**
```
Joined string: Hello World Again
```

---

### Example 29 : `replace_all` 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    String* str = string_create("This is a test. This is only a test.");

    string_replace_all(str, "test", "example");
    fmt_printf("Replaced string: %s\n", string_c_str(str));

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Replaced string: This is a example. This is only a example.
```

---

### Example 30: `string_to_int`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *str = string_create("12345");
    int intValue = string_to_int(str);

    fmt_printf("Integer value: %d\n", intValue);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Integer value: 12345
```

---

### Example 31: `string_to_float`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *str = string_create("123.45");
    float floatValue = string_to_float(str);

    fmt_printf("Float value: %.2f\n", floatValue);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Float value: 123.45
```

---

### Example 32: `string_pad_left`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *str = string_create("hello");
    string_pad_left(str, 10, '*'); // Pads with '*' to a total length of 10

    fmt_printf("Padded String (Start): '%s'\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Padded String (Start): '*****hello'
```

---

### Example 33: `string_pad_right`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *str = string_create("world");
    string_pad_right(str, 10, '-'); // Pads with '-' to a total length of 10

    fmt_printf("Padded String (End): '%s'\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Padded String (End): 'world-----'
```

---

### Example 34 : `string_to_hex`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *originalStr = string_create("Hello");
    String *hexStr = string_to_hex(originalStr);

    fmt_printf("Original String: %s\n", originalStr->dataStr);
    fmt_printf("Hexadecimal Representation: %s\n", hexStr->dataStr);

    string_deallocate(originalStr);
    string_deallocate(hexStr);

    return 0;
}
```
**Result:**
```
Original String: Hello
Hexadecimal Representation: 48656c6c6f
```

---

### Example 35 : `string_from_hex`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String *hexStr = string_create("48656c6c6f"); // Hexadecimal representation of "Hello"
    String *convertedStr = string_from_hex(hexStr);

    fmt_printf("Hexadecimal String: %s\n", hexStr->dataStr);
    fmt_printf("Converted String: %s\n", convertedStr->dataStr);

    string_deallocate(hexStr);
    string_deallocate(convertedStr);

    return 0;
}
```
**Result:**
```
Hexadecimal String: 48656c6c6f
Converted String: Hello
```

---

## Example 36 : `string_count`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("Hello World, World!");
    const char* substr = "World";

    size_t count = string_count(str, substr);
    fmt_printf("The substring '%s' appears %zu times in '%s'.\n", substr, count, str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
The substring 'World' appears 2 times in 'Hello World, World!'.
```

---

### Example 37 : `string_remove`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("Hello World, World!");
    const char* substr = "World";

    fmt_printf("Original string: %s\n", str->dataStr);
    string_remove(str, substr);
    fmt_printf("String after removal: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Original string: Hello World, World!
String after removal: Hello , !
```

---

### Example 38 : `string_from_int`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    int value = 123;
    String* str = string_from_int(value);

    fmt_printf("Integer %d converted to string: %s\n", value, str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Integer 123 converted to string: 123
```

---

### Example 39 : `string_from_float`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    float value = 123.456;
    String* str = string_from_float(value);

    fmt_printf("Float %.3f converted to string: %s\n", value, str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Float 123.456 converted to string: 123.456001
```

---

### Example 40 : `string_tokenize` 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    String* str = string_create("Hello,World-This.Is;A-Test");
    int count = 0;
    String** tokens = string_tokenize(str, ",.-;", &count);

    for (int i = 0; i < count; i++) {
        fmt_printf("Token %d: %s\n", i, tokens[i]->dataStr);
        string_deallocate(tokens[i]);
    }
    free(tokens);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Token 0: Hello
Token 1: World
Token 2: This
Token 3: Is
Token 4: A
Token 5: Test
```

---

### Example 40 : `string_to_double` 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("123.456");
    double value = string_to_double(str);

    fmt_printf("Double value: %f\n", value);
    
    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Double value: 123.456000
```

---

### Example 41 : `string_from_double` 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    double value = 123.456;
    String* str = string_from_double(value);

    fmt_printf("String from double: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
String from double: 123.456000
```

---

### Example 42 : `string_compare_ignore_case`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str1 = string_create("Hello");
    String* str2 = string_create("hello");

    if (string_compare_ignore_case(str1, str2) == 0) { 
        fmt_printf("Strings are equal (ignoring case)\n");
    }
    else { 
        fmt_printf("Strings are not equal\n");
    }

    string_deallocate(str1);
    string_deallocate(str2);

    return 0;
}
```
**Result:**
```
Strings are equal (ignoring case)
```

---

### Example 43 : `string_base64_encode` and `string_base64_decode`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    const char* sampleText = "Hello, World!";
    
    String* originalStr = string_create(sampleText);
    fmt_printf("Original String: %s\n", originalStr->dataStr);

    // Encode the string
    String* encodedStr = string_base64_encode(originalStr);
    fmt_printf("Base64 Encoded: %s\n", encodedStr->dataStr);

    // Decode the string back
    String* decodedStr = string_base64_decode(encodedStr);
    fmt_printf("Base64 Decoded: %s\n", decodedStr->dataStr);

    string_deallocate(originalStr);
    string_deallocate(encodedStr);
    string_deallocate(decodedStr);

    return 0;
}
```
**Result:**
```
Original String: Hello, World!
Base64 Encoded: SGVsbG8sIFdvcmxkIQ==
Base64 Decoded: Hello, World!
```

---

### Example 44 : `string_format` 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("");
    
    // Format the string with some arguments
    string_format(str, "Hello, %s! You have %d new messages.", "Alice", 5);
    fmt_printf("%s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Hello, Alice! You have 5 new messages.
```

---

### Example 45 : `string_repeat`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* original = string_create("Hello ");
    String* repeated = string_repeat(original, 3);

    fmt_printf("Repeated String: %s\n", repeated->dataStr);

    string_deallocate(original);
    string_deallocate(repeated);

    return 0;
}
```
**Result:**
```
Repeated String: Hello Hello Hello
```

---

### Example 46 : `string_join_variadic`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* part1 = string_create("Hello");
    String* part2 = string_create(", ");
    String* part3 = string_create("World!");

    String* joined = string_join_variadic(3, part1, part2, part3);
    fmt_printf("Joined String: %s\n", joined->dataStr);

    string_deallocate(part1);
    string_deallocate(part2);
    string_deallocate(part3);
    string_deallocate(joined);

    return 0;
}
```
**Result:**
```
Joined String: Hello, World!
```

---

## Example 47 : `string_trim_characters`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("##Hello, World!##");
    
    string_trim_characters(str, "#");
    fmt_printf("Trimmed String: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Trimmed String: Hello, World!
```

---

## Example 48 : `string_shuffle`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* myStr = string_create("Hello, World!");
    fmt_printf("Original String: %s\n", myStr->dataStr);

    string_shuffle(myStr);
    fmt_printf("Shuffled String: %s\n", myStr->dataStr);

    string_deallocate(myStr);
    return 0;
}
```
**Result:**
```
Original String: Hello, World!
Shuffled String: l,lerdlW!oH o
```

---

### Example 49 : `string_to_title` 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("hello world");

    string_to_title(str);
    fmt_printf("Title Case: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Title Case: Hello World
```

---

### Example 50 : `string_to_casefold`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("HeLLo WoRLd");

    string_to_casefold(str);
    fmt_printf("Casefolded: %s\n", str->dataStr);
    
    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Casefolded: hello world
```

---

### Example 51 : `string_remove_range` 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("Hello, World!");

    string_remove_range(str, 5, 7); 
    fmt_printf("After removal: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
After removal: HelloWorld!
```

---

### Example 52 : `string_starts_with`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("Hello, World!");
    bool startsWithHello = string_starts_with(str, "Hello");

    fmt_printf("Starts with 'Hello': %s\n", startsWithHello ? "true" : "false");

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Starts with 'Hello': true
```

---

### Example 53 : `string_ends_with`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("Hello, World!");
    bool endsWithWorld = string_ends_with(str, "World!");

    fmt_printf("Ends with 'World!': %s\n", endsWithWorld ? "true" : "false");
    
    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Ends with 'World!': true
```

---

### Example 54 : `string_to_capitalize` 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("hello world");

    string_to_capitalize(str);
    fmt_printf("Capitalized: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Capitalized: Hello world
```

---

### Example 55 : `string_swap_case` 

```c
#include "string/std_string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("Hello World");

    string_swap_case(str);
    fmt_printf("Swapped Case: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```
**Result:**
```
Swapped Case: hELLO wORLD
```

---

### Example 56 : `string_to_unicode` and `string_from_unicode`

```c
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <wchar.h>
#include <stdlib.h>

int main() {
    char* originalStr = u8"Привет, мир! مرحبا بالعالم"; // "Hello, world!" in Russian and Arabic

    // Convert to wide string (Unicode)
    wchar_t* unicodeStr = string_to_unicode(originalStr);
    if (unicodeStr == NULL) {
        printf("Conversion to wide string failed.\n");
        return 1;
    }

    fmt_printf("Unicode String: %ls\n", unicodeStr);

    // Convert back to String object
    String* convertedStr = string_from_unicode(unicodeStr);
    if (convertedStr == NULL) {
        fmt_fprintf(stderr, "Conversion from wide string failed.\n");
        free(unicodeStr);
        return 1;
    }
    fmt_printf("Converted String: %s\n", convertedStr->dataStr);

    free(unicodeStr);
    string_deallocate(convertedStr);
    
    return 0;
}
```
**Result:**
```
Unicode String: Привет, мир! مرحبا بالعالم
Converted String: Привет, мир! مرحبا بالعالم
```

---

## Example 57 : define different kind of constants strings 

```c
#include "fmt/fmt.h"
#include "string/std_string.h"

int main() {
    fmt_printf("ASCII Letters: %s\n", STRING_ASCII_LETTERS);
    fmt_printf("ASCII Lowercase: %s\n", STRING_ASCII_LOWERCASE);

    return 0;
}
```
**Result:**
```
ASCII Letters: abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ
ASCII Lowercase: abcdefghijklmnopqrstuvwxyz
```

---

## Example 58 : How to use `string_length_cstr` and `string_length_utf8`

```c
#include "fmt/fmt.h"
#include "string/std_string.h"

int main() {
    const char* str = "سلام دنیا";
    const char* cstr = "Hello World";
    
    fmt_printf("Size of ASCII string is %zu\n", string_length_cstr(cstr));
    fmt_printf("Size of unicode string is %zu\n", string_length_utf8(str));

    return 0;
}
```
**Result:**
```
Size of ASCII string is 11
Size of unicode string is 9
```

---

## Example 59 : Check String object is palindrome or not with `string_is_palindrome`

```c
#include "fmt/fmt.h"
#include "string/std_string.h"


int main() {
    String* str = string_create("121");

    fmt_printf("%s is palindrome %d", string_c_str(str), string_is_palindrome(str));

    string_deallocate(str);
    return 0;
}
```
**Result**
```
"121 is palindrome 1"
```

---

## Example 60 : Check given string is alpha numeric or not with `string_is_alnum`

```c
#include "fmt/fmt.h"
#include "string/std_string.h"


int main() {
    String* str = string_create("amin 15adf");
    String* new_one = string_create("Hello1994");

    fmt_printf("'%s' is alnum %d\n", string_c_str(str), string_is_alnum(str));
    fmt_printf("'%s' is alnum %d\n", string_c_str(new_one), string_is_alnum(new_one));
    
    string_deallocate(new_one);
    string_deallocate(str);

    return 0;
}
```
**Result**
```
'amin 15adf' is alnum 0
'Hello1994' is alnum 1
```



## Example 61 : Check given string is title or not with `string_is_title`

```c
#include "fmt/fmt.h"
#include "string/std_string.h"


int main() {
    String* a = string_create("HELLO, AND WELCOME TO MY WORLD");
    fmt_printf("%s is title: %d\n", string_c_str(a), string_is_title(a)); 

    String* b = string_create("Hello World");
    fmt_printf("%s is title: %d\n", string_c_str(b), string_is_title(b)); 

    String* c = string_create("22 Names");
    fmt_printf("%s is title: %d\n", string_c_str(c), string_is_title(c)); 

    String* d = string_create("This Is %'!?");
    fmt_printf("%s is title: %d\n", string_c_str(d), string_is_title(d)); 

    String* e = string_create("Hello world");
    fmt_printf("%s is title: %d\n", string_c_str(e), string_is_title(e)); 


    string_deallocate(a);
    string_deallocate(b);
    string_deallocate(c);
    string_deallocate(d);
    string_deallocate(e);
    
    return 0;
}
```
**Result**
```
HELLO, AND WELCOME TO MY WORLD is title: 0
Hello World is title: 1
22 Names is title: 1
This Is %'!? is title: 1
Hello world is title: 0
```

---

## Example 62 : Check given string is space or not with `string_is_space`

```c
#include "fmt/fmt.h"
#include "string/std_string.h"


int main() {
    String* str1 = string_create("     ");
    String* str2 = string_create("");

    fmt_printf("%s is space %d\n", string_c_str(str1), string_is_space(str1));
    fmt_printf("%s is space %d\n", string_c_str(str2), string_is_space(str2));

    string_deallocate(str2);
    string_deallocate(str1);

    return 0;
}
```
**Result**
```
      is space 1
 is space 0
```

---

## Example 63 : Check given string is printable or not with `string_is_printable`

```c
#include "fmt/fmt.h"
#include "string/std_string.h"


int main() {
    String* str1 = string_create("Hello!Areyou#1?");
    String* str2 = string_create("Hello!\nAre you #1?");

    fmt_printf("'%s' is printable %d\n", string_c_str(str1), string_is_printable(str1));
    fmt_printf("'%s' is printable %d\n", string_c_str(str2), string_is_printable(str2));

    string_deallocate(str1);
    string_deallocate(str2);

    return 0;
}
```

**Result**
```
'Hello!Areyou#1?' is printable 1
'Hello!
Are you #1?' is printable 0
```