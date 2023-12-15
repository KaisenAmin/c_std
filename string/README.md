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