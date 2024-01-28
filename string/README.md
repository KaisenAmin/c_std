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

## String Creation and Management
- `string_create(const char*)`: Creates a new String object with an initial value.
- `string_create_with_pool(size_t)`: Creates a new String object with a specified memory pool size.
- `string_deallocate(String*)`: Deallocates the memory used by a String object.
- `string_clear(String*)`: Clears the contents of a String object.

## String Manipulation
- `string_push_back(String*, char)`: Appends a character to the end of a String.
- `string_append(String*, const char*)`: Appends a string to the end of a String.
- `string_assign(String*, const char*)`: Assigns a new value to a String.
- `string_insert(String*, size_t, const char*)`: Inserts a string at a specified position.
- `string_erase(String*, size_t, size_t)`: Erases a portion of a String.
- `string_replace(String*, const char*, const char*)`: Replaces occurrences of a substring.
- `string_concatenate(String*, const String*)`: Concatenates two String objects.
- `string_swap(String*, String*)`: Swaps the contents of two String objects.
- `string_reverse(String*)`: Reverses the content of a String.
- `string_substr(String*, size_t, size_t)`: Creates a substring from a String object.
- `string_pop_back(String*)`: Removes the last character of a String.
- `string_resize(String*, size_t)`: Resizes a String to a specified size.
- `string_shrink_to_fit(String*)`: Reduces the capacity of a String to fit its size.
- `string_replace_all(String *, const char *, const char *)`: Replace occurrences of all substr in String object.
- `string_trim_left(String *str)`: This function trims leading whitespace characters from the beginning of the String object str.
- `string_trim_right(String *str)`: This function trims trailing whitespace characters from the end of the String object str
## String Comparison
- `string_is_equal(String*, String*)`: Checks if two Strings are equal.
- `string_is_less(String*, String*)`: Checks if the first String is less than the second.
- `string_is_greater(String*, String*)`: Checks if the first String is greater than the second.
- `string_is_less_or_equal(String*, String*)`: Checks if the first String is less than or equal to the second.
- `string_is_greater_or_equal(String*, String*)`: Checks if the first String is greater than or equal to the second.
- `string_is_not_equal(String*, String*)`: Checks if two Strings are not equal.
- `string_compare(String*, String*)`: Compares two Strings.
- `string_compare_ignore_case(String*, String*)`: Compares two Strings, ignoring case differences.

## String Information and Properties
- `string_length(String*)`: Returns the length of a String.
- `string_capacity(String*)`: Returns the capacity of a String.
- `string_max_size(String*)`: Returns the maximum size of a String.
- `string_empty(String*)`: Checks if a String is empty.
- `string_contains(String*, const char*)`: Checks if a String contains a specific substring.
- `string_count(Strin*, char*)` : count number of substr appears in String object 'str'.
- `string_length_cstr(const char*)` : Returns the length of a char*.
- `string_length_utf(const char*)` : Return the length of utf-8 char*.
- `string_utf8_char_len(char )` : Return the len of each unicode character.

## String Characteristics
- `string_is_alpha(String*)`: Checks if a String contains only alphabetic characters.
- `string_is_digit(String*)`: Checks if a String contains only digits.
- `string_is_lower(String*)`: Checks if all characters in a String are lowercase.
- `string_is_upper(String*)`: Checks if all characters in a String are uppercase.

## String Access and Iteration
- `string_begin(String*)`: Returns an iterator to the beginning.
- `string_end(String*)`: Returns an iterator to the end.
- `string_rbegin(String*)`: Returns a reverse iterator to the beginning.
- `string_rend(String*)`: Returns a reverse iterator to the end.
- `string_cbegin(String*)`: Returns a constant iterator to the beginning.
- `string_cend(String*)`: Returns a constant iterator to the end.
- `string_crbegin(String*)`: Returns a constant reverse iterator to the beginning.
- `string_crend(String*)`: Returns a constant reverse iterator to the end.

## String Data Access
- `string_data(String*)`: Returns a pointer to the data stored in a String.
- `string_c_str(String*)`: Returns a pointer to the null-terminated sequence of characters.
- `string_at(String*, size_t)`: Returns a reference to the character at a specified index.
- `string_back(String*)`: Returns a reference to the last character.
- `string_front(String*)`: Returns a reference to the first character.

## String Conversion Functions
- `string_to_int(String*)`: Converts a String to an integer.
- `string_to_float(String*)`: Converts a String to a float.
- `string_to_double(String*)`: Converts a String to a double.
- `string_from_int(int)`: Creates a String from an integer.
- `string_from_float(float)`: Creates a String from a float.
- `string_from_double(double)`: Creates a String from a double.

## Unicode Handling
- `string_to_unicode(const char*)`: Converts a regular string to a wide string.
- `string_from_unicode(const wchar_t*)`: Converts a wide string back to a regular string.

## String Case and Encoding Operations
- `string_to_upper(String*)`: Converts a String to uppercase.
- `string_to_lower(String*)`: Converts a String to lowercase.
- `string_to_casefold(String*)`: Converts a String to casefolded form.
- `string_swap_case(String*)`: Swaps the case of each character in a String.

## String Search and Replace
- `string_find_first_of(String*, const char*, size_t)`: Finds the first occurrence of any of the characters in the given string.
- `string_find_last_of(String*, const char*, size_t)`: Finds the last occurrence of any of the characters in the given string.
- `string_find_first_not_of(String*, const char*, size_t)`: Finds the first character that does not match any of the characters in the given string.
- `string_find_last_not_of(String*, const char*, size_t)`: Finds the last character that does not match any of the characters in the given string.
- `string_replace_all(String*, const char*, const char*)`: Replaces all occurrences of a substring.
- `string_remove(String*, const char*)`: Removes all occurrences of a substring.

## Advanced String Operations
- `string_split(String*, const char*, int*)`: Splits a String into an array of String objects.
- `string_join(String**, int, const char*)`: Joins several String objects into one.
- `string_trim(String*)`: Trims whitespace from both ends of a String.
- `string_pad_left(String*, size_t, char)`: Pads a String from the left.
- `string_pad_right(String*, size_t, char)`: Pads a String from the right.
- `string_repeat(const String*, size_t)`: Creates a new String by repeating the original String a specified number of times.
- `string_join_variadic(size_t, ...)`: Concatenates multiple Strings (variadic function).
- `string_trim_characters(String*, const char*)`: Trims specified characters from both ends of a String.
- `string_shuffle(String*)`: Randomly shuffle character of String object
- `string_format(String*, const char*, ...)`: Formats a String using given format specifiers.
- `string_to_title(String*)`: Converts each word in the String to title case.
- `string_to_capitalize(String*)`: Capitalizes the first character of a String.
- `string_to_casefold(String*)`: Converts a String to a case-insensitive form for comparisons.
- `string_remove_range(String* str, size_t startPos, size_t endPos)`: Removes a range of characters from a String object, starting from startPos and ending at endPos.
- `string_starts_with(String*, const char*)`: Checks if a String starts with a specified substring.
- `string_ends_with(String*, const char*)`: Checks if a String ends with a specified substring.
- `string_base64_encode(const String*)`: Encodes a String to base64 format.
- `string_base64_decode(const String*)`: Decodes a base64 encoded String.
- `string_remove(String*, const char*)`: Removes all occurrences of a substring.
- `string_set_pool_size(String*, size_t)`: Sets the size of the memory pool for a String.
- `string_tokenize(String*, const char* , int*)`: This function splits a string into tokens based on multiple delimiters.
- `string_create_from_initializer` : The string_create_from_initializer function dynamically creates an array of String pointers, each initialized with a string passed as a variadic argument.
## String Encoding Functions
- `string_to_hex(String*)`: Converts a String to its hexadecimal representation.
- `string_from_hex(String*)`: Converts a hexadecimal String back to the original string.
```c
#include "string/string.h"
```


### Example 1: how to create String obj and use `string_push_back`, `string_append`, `string_at`, `string_clear`, `string_length`, `string_capacity` also `string_deallocate`

```c

#include "string/string.h"
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

## Example 2: `string_shrink_to_fit`, `string_resize`

```c
#include "string/string.h"
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

## Example 3: how to `string_assign` new string and `string_erase` some part of String obj also `string_insert` new ...  

```c
#include "string/string.h"
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

## Example 4: `string_replace` Strings 

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String *str1 = string_create("Hello World");
    string_replace(str1, "World", "There");
        
    fmt_printf("After replace: %s\n", str1->dataStr);

    string_deallocate(str1);
    return 0;
}
```


## Example 5: `string_swap` two String obj and also `string_pop_back`

```c
#include "string/string.h"
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

## Example 6: `string_back` and `string_front` return ref to front and last char of String 

```c
#include "string/string.h"
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

## Example 7 : `string_copy` how to copy some part of String 

```c
#include "string/string.h"
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

## Example 8 : `string_find` find string in String and return position

```c
#include "string/string.h"
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

## Example 9 : how to use `string_rfind`, `string_find_first_of`, `string_find_last_of`

```c

#include "string/string.h"
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

## Example 10 : `string_find_first_not_of`, `string_find_last_not_of`

```c

#include "string/string.h"
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

## Example 11 : `string_data` return pointer to String sequence 

```c

#include "string/string.h"
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

## Example 12 : `string_begin`, `string_end` return iterator pointer from begin and end of String sequence

```c

#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello amin");

    for (char* it = string_begin(myString); it != string_end(myString); it++) {
        fmt_printf("%c", *it);
    }
        
    string_clear(myString);
    string_deallocate(myString);
    return 0;
}

```

## Example 13 : `string_rbeing`, `string_rend` return reverse iterator 

```c

#include "string/string.h"
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

## Example 14 : `string_cbegin`, `string_cend`

```c

#include "string/string.h"
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

## Example 15 : `string_crbegin`, `string_crend` 

```c

#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String *myString = string_create("Hello, World!");

    // Reverse iterate using crbegin and crend
    fmt_printf("String in reverse using crbegin and crend: ");
    for (const char* it = string_crbegin(myString); it != string_crend(myString); --it) { 
        fmt_printf("%c", *it);
    }
    fmt_printf("\n");

    // Clean up
    string_deallocate(myString);
    return 0;
}

```

## Example 16 : how to use relationals operators in String 

```c

#include "string/string.h"
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

## Example 17: Manipulate Multiple String 

```c

#include "string/string.h"
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

## Example 18: 2D String Array using Custom String Struct

```c

#include "string/string.h"
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

## Example 19 : bench mark operation in `String` and `std::string` 

gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main .\main.c .\string\string.c
Time taken (Custom String): 0.286000 seconds

```c

#include "string/string.h"
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

and in Cpp 

```c
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

## Example 20 : benchmark of `String` and `std::string`

create pool
Average Custom Vector Time: 0.001507 seconds

```c
#include "string/string.h"
#include "fmt/fmt.h"
#include <time.h>

#define ARRAY_SIZE 1000000
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

Time taken (std::string): 0.0051607 seconds

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

### Example 21 : string to Lower or Upper 
```c
#include "string/string.h"
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

### Example 22 : Reverse String with `string_reverse`

```c
#include "string/string.h"
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

### Example 23 : `string_is_digit`

```c
#include "string/string.h" 
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

### Example 24 : `string_is_lower`

```c
#include "string/string.h"
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

### Example 25 : concatenate two String with `string_concatenate`

```c
#include "string/string.h"
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

### Example 26 : trim String from start, end or both 

```c
#include "string/string.h"
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

### Example 27 : split String with `string_split`

```c
#include "string/string.h"
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

### Example 28 : joins several String objects with `string_join` 

```c
#include "string/string.h"
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

### Example 29 : `replace_all` 

```c
#include "string/string.h"
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

### Example 30: `string_to_int`

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String *str = string_create("12345");
    int intValue = string_to_int(str);

    fmt_printf("Integer value: %d\n", intValue);

    string_deallocate(str);
    return 0;
}
```

### Example 31: `string_to_float`

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String *str = string_create("123.45");
    float floatValue = string_to_float(str);

    fmt_printf("Float value: %.2f\n", floatValue);

    string_deallocate(str);
    return 0;
}
```

### Example 32: `string_pad_left`

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String *str = string_create("hello");
    string_pad_left(str, 10, '*'); // Pads with '*' to a total length of 10

    fmt_printf("Padded String (Start): '%s'\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```

### Example 33: `string_pad_right`

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String *str = string_create("world");
    string_pad_right(str, 10, '-'); // Pads with '-' to a total length of 10

    fmt_printf("Padded String (End): '%s'\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```

### Example 34 : `string_to_hex`

```c
#include "string/string.h"
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

### Example 35 : `string_from_hex`

```c
#include "string/string.h"
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

## Example 36 : `string_count`

```c
#include "string/string.h"
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

### Example 37 : `string_remove`

```c
#include "string/string.h"
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

### Example 38 : `string_from_int`

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    int value = 123;
    String* str = string_from_int(value);

    fmt_printf("Integer %d converted to string: %s\n", value, str->dataStr);

    string_deallocate(str);
    return 0;
}
```

### Example 39 : `string_from_float`

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    float value = 123.456;
    String* str = string_from_float(value);

    fmt_printf("Float %.3f converted to string: %s\n", value, str->dataStr);

    string_deallocate(str);
    return 0;
}
```

### Example 40 : `string_tokenize` 

```c
#include "string/string.h"
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

### Example 40 : `string_to_double` 

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("123.456");
    double value = string_to_double(str);

    fmt_printf("Double value: %f\n", value);
    
    string_deallocate(str);
    return 0;
}

```

### Example 41 : `string_from_double` 

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    double value = 123.456;
    String* str = string_from_double(value);

    fmt_printf("String from double: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}

```

### Example 42 : `string_compare_ignore_case`

```c
#include "string/string.h"
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

### Example 43 : `string_base64_encode` and `string_base64_decode`

```c
#include "string/string.h"
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

### Example 44 : `string_format` 

```c
#include "string/string.h"
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

### Example 45 : `string_repeat`

```c
#include "string/string.h"
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

### Example 46 : `string_join_variadic`

```c
#include "string/string.h"
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

## Example 47 : `string_trim_characters`

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("##Hello, World!##");
    
    string_trim_characters(str, "#");
    fmt_printf("Trimmed String: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```

## Example 48 : `string_shuffle`

```c
#include "string/string.h"
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

### Example 49 : `string_to_title` 

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("hello world");

    string_to_title(str);
    fmt_printf("Title Case: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```

### Example 50 : `string_to_casefold`

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("HeLLo WoRLd");

    string_to_casefold(str);
    fmt_printf("Casefolded: %s\n", str->dataStr);
    
    string_deallocate(str);
    return 0;
}
```

### Example 51 : `string_remove_range` 

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("Hello, World!");

    string_remove_range(str, 5, 7); 
    fmt_printf("After removal: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```

### Example 52 : `string_starts_with`

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("Hello, World!");
    bool startsWithHello = string_starts_with(str, "Hello");

    fmt_printf("Starts with 'Hello': %s\n", startsWithHello ? "true" : "false");

    string_deallocate(str);
    return 0;
}
```

### Example 53 : `string_ends_with`

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("Hello, World!");
    bool endsWithWorld = string_ends_with(str, "World!");

    fmt_printf("Ends with 'World!': %s\n", endsWithWorld ? "true" : "false");
    
    string_deallocate(str);
    return 0;
}
```

### Example 54 : `string_to_capitalize` 

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("hello world");

    string_to_capitalize(str);
    fmt_printf("Capitalized: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```

### Example 55 : `string_swap_case` 

```c
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    String* str = string_create("Hello World");

    string_swap_case(str);
    fmt_printf("Swapped Case: %s\n", str->dataStr);

    string_deallocate(str);
    return 0;
}
```

### Example 56 : `string_to_unicode` and `string_from_unicode`

```c
#include "string/string.h"
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

    // Clean up
    free(unicodeStr);
    string_deallocate(convertedStr);
    
    return 0;
}
```

## Example 57 : define different kind of constants strings 

```c
#include "fmt/fmt.h"
#include "string/string.h"

int main() {
    fmt_printf("ASCII Letters: %s\n", STRING_ASCII_LETTERS);
    fmt_printf("ASCII Lowercase: %s\n", STRING_ASCII_LOWERCASE);

    return 0;
}
```

## Example 58 : How to use `string_length_cstr` and `string_length_utf8`

```c
#include "fmt/fmt.h"
#include "string/string.h"

int main() {
    const char* str = "سلام دنیا";
    const char* cstr = "Hello World";
    
    fmt_printf("Size of ASCII string is %zu\n", string_length_cstr(cstr));
    fmt_printf("Size of unicode string is %zu\n", string_length_utf8(str));

    return 0;
}
```