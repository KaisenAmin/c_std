# String Library

The String library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::string` in C++.

## Compilation

To compile the String library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -o main ./main.c ./string/string.c
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the String library in your project, include the `string.h` header file in your source code.

```c
#include "string/string.h"
```


### Example 1: how to create String obj and use 'push_back', 'append', 'at', 'clear', 'length', 'capacity' also 'deallocate'

```c

String* myString = string_create("");
   
// Push back a new character
myString->push_back(myString, '!');
printf("String after push_back: %s\n", myString->dataStr);

// Access a character
char ch = myString->at(myString, 0);
printf("Character at index 1: %c\n", ch);

// Print length and capacity
printf("Length: %zu, Capacity: %zu\n", myString->length(myString), myString->capacity(myString));

// Clear the string
myString->clear(myString);
printf("String after clear: %s\n", myString->dataStr);

// Clean up
myString->clear(myString);
myString->append(myString, "Hello C Programmers");
    
printf("%s\n", myString->c_str(myString));

myString->deallocate(myString);

```

## Example 2: 'shrink_to_fit', 'resize'

```c

String *myString = string_create("Hello");

// Append a string
myString->append(myString, ", World!");
printf("String after append: %s\n", myString->dataStr);

// Resize the string
myString->resize(myString, 5);  // Resize to "Hello"
printf("String after resize: %s\n", myString->dataStr);

// Shrink to fit
myString->shrink_to_fit(myString);
printf("String capacity after shrink to fit: %zu\n", myString->capacity(myString));

// clean up and dellocate
myString->clear(myString);
myString->deallocate(myString);

```

## Example 3: how to 'assign' new string and 'erase' some part of String obj also 'insert' new ...  

```c

String *myString = string_create("Hello World");

// Assign a new string
myString->assign(myString, "New String");
printf("String after assign: %s\n", myString->dataStr);

// Insert a string
myString->insert(myString, 4, "Test ");
printf("String after insert: %s\n", myString->dataStr);

// Erase a portion of the string
myString->erase(myString, 0, 5);  // Erase "New T"
printf("String after erase: %s\n", myString->dataStr);

// Deallocate and clean up
myString->clear(myString);
myString->deallocate(myString);

```

## Example 4: 'replace' Strings 

```c

String *str1 = string_create("Hello World");
str1->replace(str1, "World", "There");
    
printf("After replace: %s\n", str1->dataStr);

str1->deallocate(str1);

```


## Example 5: 'swap' two String obj and also 'pop_back'

```c

String *str1 = string_create("Hello World");
str1->replace(str1, "World", "There");
    
printf("After replace: %s\n", str1->dataStr);

// Swap example
String *str2 = string_create("Sample Text");
str2->swap(str1, str2);

printf("After swap, str1: %s, str2: %s\n", str1->dataStr, str2->dataStr);

// Pop back example
str2->pop_back(str2);
printf("After pop back: %s\n", str2->dataStr);

str1->deallocate(str1);
str2->deallocate(str2);

```


## Example 6: 'back' and 'front' return ref to front and last char of String 

```c

String *myString = string_create("Hello World");

// Get the last character
char lastChar = *(char*)myString->back(myString);

printf("Last character : %c\n", lastChar);
// its just syntax !!!
*(char*)myString->back(myString) = 'a';

printf("Wowww: %c\n", *(char*)myString->back(myString));

// Get the first character
char firstChar = *(char*)myString->front(myString);
printf("First character: %c\n", firstChar);
printf("Max size is %zu\n", myString->max_size(myString));
    
// Deallocate and clean up
myString->clear(myString);
myString->deallocate(myString);

```

## Example 7 : 'copy' how to copy some part of String 

```c

String *myString = string_create("Hello, World!");

// Copy example
char buffer[50];
size_t copied = myString->copy(myString, buffer, 7, 5);
printf("Copied '%s' (%zu characters)\n", buffer, copied);

myString->deallocate(myString);

```

## Example 8 : 'find' find string in String and return position

```c

String *myString = string_create("Hello, World!");

// Find example
int findPos = myString->find(myString, "World", 0);
printf("Found 'World' at position: %d\n", findPos);

myString->deallocate(myString);

```

## Example 9 : how to use 'rfind', 'find_first_of', 'find_last_of'

```c

String *myString = string_create("Hello, World!");

// rfind example
int rfindPos = myString->rfind(myString, "o,", myString->length(myString) - 1);
printf("Last 'o' found at position: %d\n", rfindPos);

int findFirstOfPos = myString->find_first_of(myString, "World", 0);
printf("First occurrence of 'World' found at position: %d\n", findFirstOfPos);

// find_last_of example
int findLastOfPos = myString->find_last_of(myString, "World", myString->length(myString) - 1);
printf("Last occurrence of 'World' found at position: %d\n", findLastOfPos);

// Deallocate and clean up
myString->clear(myString);
myString->deallocate(myString);

```

## Example 10 : 'find_first_not_of', 'find_last_not_of'

```c

String *myString = string_create("Hello, World!");

// find_first_not_of example
int firstNotOfPos = myString->find_first_not_of(myString, "Hello", 0);
printf("First position not matching 'Hello': %d\n", firstNotOfPos);

// find_last_not_of example
int lastNotOfPos = myString->find_last_not_of(myString, "World", myString->length(myString) - 1);
printf("Last position not matching 'World': %d\n", lastNotOfPos);

myString->deallocate(myString);

```

## Example 11 : 'data' return pointer to String sequence 

```c

String *myString = string_create("Hello, World!");
    
// Data example
const char *data = myString->data(myString);
printf("Data: %s\n", data);

// Deallocate and clean up
myString->clear(myString);
myString->deallocate(myString);

```


## Example 12 : 'begin', 'end' return iterator pointer from begin and end of String sequence

```c

String *myString = string_create("Hello amin");

for (char* it = myString->begin(myString); it != myString->end(myString); it++)
    printf("%c", *it);
    
myString->clear(myString);
myString->deallocate(myString);

```

## Example 13 : 'rbeing', 'rend' return reverse iterator 

```c

String *myString = string_create("Hello amin");

// Reverse iteration using rbegin and rend
printf("String in reverse: ");
for (char* it = myString->rbegin(myString); it != myString->rend(myString); --it) 
    printf("%c", *it);
    
printf("\n");

// Clean up
myString->clear(myString);
myString->deallocate(myString);

```

## Example 14 : 'cbegin', 'cend'

```c

String *myString = string_create("Hello, World!");

// Iterate using cbegin and cend
printf("String using cbegin and cend: ");
for (const char* it = myString->cbegin(myString); it != myString->cend(myString); ++it) 
    printf("%c", *it);

printf("\n");

myString->deallocate(myString);

```

## Example 15 : 'crbegin', 'crend' 

```c

String *myString = string_create("Hello, World!");

// Reverse iterate using crbegin and crend
printf("String in reverse using crbegin and crend: ");
for (const char* it = myString->crbegin(myString); it != myString->crend(myString); --it) 
    printf("%c", *it);
    
printf("\n");

// Clean up
myString->deallocate(myString);

```

## Example 16 : how to use relationals operators in String 

```c

String* str1 = string_create("Hello");
String* str2 = string_create("World");

if (str1->is_equal(str1, str2)) 
    printf("Strings are equal\n");
    
if (str1->is_less(str1, str2)) 
    printf("String 1 is less than String 2\n");

if (str1->is_greater(str1, str2))
    printf("String1 is greater that String2\n");

if (str1->is_less_or_equal(str1, str2))
    printf("String1 is less than or equal String2\n");

if (str1->is_greater_or_equal(str1, str2))
    printf("String1 is greater than or equal String2\n");
                
str1->deallocate(str1);
str2->deallocate(str2);

```