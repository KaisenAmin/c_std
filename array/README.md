
# Array Library in C

**Author:** amin tahmasebi
**Release Date:** 2023
**License:** ISC License

The Array library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::array` in C++.

## Compilation

To compile the Array library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./array/array.c 
```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Array library in your project, include the `array.h` header file in your source code.

```c
#include "array/array.h"
```

## Function Descriptions

### Array Creation and Management
- `array_create(size_t element_size, size_t size)`: Creates a new Array with a specified element size and initial size.
- `array_deallocate(Array* arr)`: Frees the memory allocated for the Array.

### Relational Operators
- `array_is_equal(const Array* arr1, const Array* arr2)`: Checks if two Arrays are equal.
- `array_is_less(const Array* arr1, const Array* arr2)`: Determines if the first Array is less than the second.
- `array_is_greater(const Array* arr1, const Array* arr2)`: Determines if the first Array is greater than the second.
- `array_is_not_equal(const Array* arr1, const Array* arr2)`: Checks if two Arrays are not equal.
- `array_is_less_or_equal(const Array* arr1, const Array* arr2)`: Checks if the first Array is less than or equal to the second.
- `array_is_greater_or_equal(const Array* arr1, const Array* arr2)`: Checks if the first Array is greater than or equal to the second.

### Array Operations
- `array_set(Array* arr, size_t index, const void* value)`: Sets the value at a specific index in the Array.
- `array_insert(Array* mainArr, const Array* otherArr, size_t index)`: Inserts elements of another Array into the main Array at a specified index.
- `array_fill(Array* arr, const void* value)`: Fills the Array with a specified value.
- `array_swap(Array* arr1, Array* arr2)`: Swaps the contents of two Arrays.
- `array_copy(Array* dest, const Array* src)`: Copies all elements from the source array src to the destination array dest.
- `array_reverse(Array* arr)`: Reverses the order of the elements in the array.
- `array_sort(Array* arr, int (*compare)(const void*, const void*))`: Sorts the array using the provided comparison function.
- `array_clear(Array* arr)`: Removes all elements from the array without deallocating the memory, effectively setting the size to zero.


### Array Access
- `array_at(Array* arr, size_t index)`: Returns a pointer to the element at a specified index.
- `array_begin(Array* arr)`: Returns a pointer to the beginning of the Array.
- `array_end(Array* arr)`: Returns a pointer to the end of the Array.
- `array_rbegin(Array* arr)`: Returns a reverse iterator pointing to the end of the Array.
- `array_rend(Array* arr)`: Returns a reverse iterator pointing to the beginning of the Array.
- `array_front(Array* arr)`: Returns a pointer to the first element of the Array.
- `array_back(Array* arr)`: Returns a pointer to the last element of the Array.
- `array_data(Array* arr)`: Returns a pointer to the underlying data of the Array.

### Array Properties
- `array_size(Array* arr)`: Returns the number of elements in the Array.
- `array_max_size(Array* arr)`: Returns the maximum number of elements that the Array can hold.

### Const Array Iterators
- `array_cbegin(Array* arr)`: Returns a constant iterator to the beginning of the Array.
- `array_cend(Array* arr)`: Returns a constant iterator to the end of the Array.
- `array_crbegin(Array* arr)`: Returns a constant reverse iterator to the end of the Array.
- `array_crend(Array* arr)`: Returns a constant reverse iterator to the beginning of the Array.

### Empty Check
- `array_empty(Array* arr)`: Checks if the Array is empty.


## Example 1 : how to create Array object and `array_fill` array with value then get them with `array_at` method

```c
#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* intArray = array_create(sizeof(int), 5);
    int valueToFill = 42;

    array_fill(intArray, &valueToFill);

    for (size_t i = 0; i < array_size(intArray); i++) {
        fmt_printf("Number is %d\n", *(int*)array_at(intArray, i));
    }
    fmt_println("------------------", FMT_END_ARGS);

    for (size_t i = 0; i < array_size(intArray); ++i) {
        int* element = (int*)array_at(intArray, i);
        if (element != NULL) {
            fmt_printf("Element %zu: %d\n", i, *element);
        }
    }

    array_deallocate(intArray);
    return 0;
}
```

## Example 2 : check array is `array_empty` or not 

```c
#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* intArray = array_create(sizeof(int), 5);
    int valueToFill = 42;

    array_fill(intArray, &valueToFill);

    // Check if the array is empty
    if (array_empty(intArray)) {
        fmt_printf("Array is empty.\n");
    }
    else {
        fmt_printf("Array is not empty.\n");
    }

    array_deallocate(intArray);
    return 0;
}
```

## Example 3 : `array_front`, `array_back` return ref to first and end of the Array

```c
#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* intArray = array_create(sizeof(int), 5);
    int valueToFill = 42;

    array_fill(intArray, &valueToFill);

    *(int*)array_front(intArray) = 1563;
    // Access the front and back elements

    int* frontElement = (int*)array_front(intArray);
    int* backElement = (int*)array_back(intArray);

    if (frontElement != NULL && backElement != NULL) { 
        fmt_printf("Front element: %d, Back element: %d\n", *frontElement, *backElement);
    }

    array_deallocate(intArray);
    return 0;
}
```

## Example 4 : how to set data in position with `array_set` 

```c
#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* intArray = array_create(sizeof(int), 5);
    int valueToFill = 42;

    array_fill(intArray, &valueToFill);

    for (size_t i = 0; i < array_size(intArray); ++i) {
        int value = i * 2; 
        array_set(intArray, i, &value);
        int* element = (int*)array_at(intArray, i);

        if (element != NULL) {
            fmt_printf("Element %zu: %d\n", i, *element);
        }            
    }

    array_deallocate(intArray);
    return 0;
}
```

## Example 5: Iterating with `array_begin` and end with Doubles

```c
#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* doubleArray = array_create(sizeof(double), 5);
    double values[] = {1.5, 2.5, 3.5, 4.5, 5.5};

    for (size_t i = 0; i < array_size(doubleArray); ++i) { 
        array_set(doubleArray, i, &values[i]);
    }
    for (double* it = (double*)array_begin(doubleArray); it != (double*)array_end(doubleArray); ++it) {
        fmt_printf("Element: %f\n", *it);
    }

    array_deallocate(doubleArray);
    return 0;
}
```

## Example 6 : Using `array_cbegin` and 'cend' with char*

```c

#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* stringArray = array_create(sizeof(char*), 3);
    char* strings[] = {"Hello", "World", "Array"};

    for (size_t i = 0; i < array_size(stringArray); ++i) { 
        array_set(stringArray, i, &strings[i]);
    }
    for (const char** it = (const char**)array_cbegin(stringArray); it != (const char**)array_cend(stringArray); ++it) { 
        fmt_printf("String: %s\n", *it);
    }

    array_deallocate(stringArray);
    return 0;
}
```

## Example 7 : Using Relational Operators

```c

#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* arr1 = array_create(sizeof(int), 3);
    Array* arr2 = array_create(sizeof(int), 3);
    int arr1Values[] = {1, 2, 3};
    int arr2Values[] = {1, 2, 3};

    for (size_t i = 0; i < 3; ++i) {
        array_set(arr1, i, &arr1Values[i]);
        array_set(arr2, i, &arr2Values[i]);
    }

    if (array_is_equal(arr1, arr2)) {
        fmt_printf("Arrays are equal.\n");
    }
    else {
        fmt_printf("Arrays are not equal.\n");
    }

    array_deallocate(arr1);
    array_deallocate(arr2);

    return 0;
}

```

## Example 8 : Using `array_rbegin` and `array_rend` with float

```c
#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* floatArray = array_create(sizeof(float), 4);
    float floatValues[] = {1.1f, 2.2f, 3.3f, 4.4f};

    for (size_t i = 0; i < array_size(floatArray); ++i) { 
        array_set(floatArray, i, &floatValues[i]);
    }
    for (float* it = (float*)array_rbegin(floatArray); it != (float*)array_rend(floatArray); --it) { 
        fmt_printf("Element: %f\n", *it);
    }

    array_deallocate(floatArray);
    return 0;
}

```

## Example 9 : How to create 2D Array 

```c
#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    const size_t rows = 3;
    const size_t cols = 4;

    // Step 1 & 2: Create an Array of Array Pointers
    Array* array2D = array_create(sizeof(Array*), rows);

    // Step 3: Initialize Each Row Array
    for (size_t i = 0; i < rows; ++i) {
        Array* row = array_create(sizeof(int), cols);
        int valueToFill = i; // Example value to fill

        array_fill(row, &valueToFill);
        array_set(array2D, i, &row);
    }

    // Step 4: Use the 2D Array
    for (size_t i = 0; i < rows; ++i) {
        Array* row = *(Array**)array_at(array2D, i);
        for (size_t j = 0; j < cols; ++j) {
            int* element = (int*)array_at(row, j);
            fmt_printf("%d ", *element);
        }
        fmt_printf("\n");
    }

    // Step 5: Clean Up
    for (size_t i = 0; i < rows; ++i) {
        Array* row = *(Array**)array_at(array2D, i);
        array_deallocate(row);
    }

    array_deallocate(array2D);
    return 0;
}
```

## Example 10 : Use String object in Array 

```c

#include "fmt/fmt.h"
#include "array/array.h"
#include "string/string.h"

int main() {
    const size_t numStrings = 3; 
    Array* stringArray = array_create(sizeof(String*), numStrings);

    // Initialize each String in the Array
    for (size_t i = 0; i < numStrings; ++i) {
        String* str = string_create("Initial Text");
        array_set(stringArray, i, &str);
    }

    // Example manipulation: Append text to each string
    for (size_t i = 0; i < numStrings; ++i) {
        String** strPtr = (String**)array_at(stringArray, i);
        String* str = *strPtr;

        string_append(str, " - Appended Text");
        fmt_printf("String %zu: %s\n", i, string_c_str(str));
    }

    // Clean up: Deallocate each String and the Array
    for (size_t i = 0; i < numStrings; ++i) {
        String** strPtr = (String**)array_at(stringArray, i);
        String* str = *strPtr;

        string_deallocate(str);
    }
        
    array_deallocate(stringArray);
    return 0;
}
```

## Example 11: String Concatenation using Array and String Libraries

This example creates an array of strings and concatenates them into a single string.
```c
#include "array/array.h"
#include "string/string.h"
#include "fmt/fmt.h"

int main() {
    Array* stringArray = array_create(sizeof(String*), 3);
    char* words[] = {"Hello", "World", "Array"};

    // Initialize each string in the array
    for (size_t i = 0; i < 3; ++i) {
        String* str = string_create(words[i]);
        array_set(stringArray, i, &str);
    }

    // Concatenate all strings
    String* concatenated = string_create("");
    for (size_t i = 0; i < array_size(stringArray); ++i) {
        String** strPtr = (String**)array_at(stringArray, i);
        string_append(concatenated, string_c_str(*strPtr));

        if (i < array_size(stringArray) - 1) {
            string_append(concatenated, " "); // Add space between words
        }
    }

    fmt_printf("Concatenated String: %s\n", string_c_str(concatenated));

    for (size_t i = 0; i < array_size(stringArray); ++i) {
        String** strPtr = (String**)array_at(stringArray, i);
        string_deallocate(*strPtr);
    }

    array_deallocate(stringArray);
    string_deallocate(concatenated);

    return 0;
}
```

## Example 12 : A Simple Card Game

We will define a standard deck of cards using enum for suits and ranks. Then, we'll create a struct to represent a card. We'll use the Array library to create and manipulate a deck of cards.
```c
#include "fmt/fmt.h"
#include "array/array.h"

typedef enum {
    CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES
} Suit;

typedef enum {
    TWO = 2,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE
} Rank;

typedef struct {
    Suit suit;
    Rank rank;
} Card;

Array* initializeDeck() {
    Array* deck = array_create(sizeof(Card), 52);
    int index = 0;

    for (Suit suit = CLUBS; suit <= SPADES; suit++) {
        for (Rank rank = TWO; rank <= ACE; rank++) {
            Card card = { suit, rank };
            array_set(deck, index++, &card);
        }
    }
    return deck;
}

void printCard(const Card* card) {
    const char* suits[] = { "Clubs", "Diamonds", "Hearts", "Spades" };
    const char* ranks[] = { "", "", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace" };

    fmt_printf("%s of %s\n", ranks[card->rank], suits[card->suit]);
}

int main() {
    Array* deck = initializeDeck();

    // Example: Print the first and last card
    printCard((Card*)array_front(deck));
    printCard((Card*)array_back(deck));

    array_deallocate(deck);
    return 0;
}

```

## Example 13 : `array_copy`

```c
#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* src = array_create(sizeof(int), 5);
    Array* dest = array_create(sizeof(int), 5);

    for (size_t i = 0; i < array_size(src); ++i) {
        int value = (int)(i * 10); // Example values: 0, 10, 20, 30, 40
        array_set(src, i, &value);
    }
    array_copy(dest, src);

    fmt_printf("Source Array: ");
    for (size_t i = 0; i < array_size(src); ++i) {
        int* element = (int*)array_at(src, i);
        fmt_printf("%d ", *element);
    }
    fmt_printf("\n");

    fmt_printf("Destination Array: ");
    for (size_t i = 0; i < array_size(dest); ++i) {
        int* element = (int*)array_at(dest, i);
        fmt_printf("%d ", *element);
    }
    fmt_printf("\n");

    array_deallocate(src);
    array_deallocate(dest);

    return 0;
}
```

## Example 14 : `array_sort`

```c
#include "fmt/fmt.h"
#include "array/array.h"

int compare_ints(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    Array* arr = array_create(sizeof(int), 5);

    for (size_t i = 0; i < array_size(arr); ++i) {
        int value = (int)(50 - i * 10); // Example values: 50, 40, 30, 20, 10
        array_set(arr, i, &value);
    }
    array_sort(arr, compare_ints);

    fmt_printf("Sorted Array: ");
    for (size_t i = 0; i < array_size(arr); ++i) {
        int* element = (int*)array_at(arr, i);
        printf("%d ", *element);
    }
    fmt_printf("\n");

    array_deallocate(arr);
    return 0;
}
```

## Example 15 : `array_reverse`

```c
#include "fmt/fmt.h"
#include "array/array.h"

int compare_ints(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int main() {
    Array* arr = array_create(sizeof(int), 5);

    for (size_t i = 0; i < array_size(arr); ++i) {
        int value = (int)(i * 10); // Example values: 0, 10, 20, 30, 40
        array_set(arr, i, &value);
    }
    array_reverse(arr);

    fmt_printf("Reversed Array: ");
    for (size_t i = 0; i < array_size(arr); ++i) {
        int* element = (int*)array_at(arr, i);
        fmt_printf("%d ", *element);
    }
    fmt_printf("\n");

    array_deallocate(arr);
    return 0;
}
```

## Example 16 : `array_clear`

```c
#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* arr = array_create(sizeof(int), 5);

    for (size_t i = 0; i < array_size(arr); ++i) {
        int value = (int)(i * 10); // Example values: 0, 10, 20, 30, 40
        array_set(arr, i, &value);
    }

    array_clear(arr);
    fmt_printf("Array Size After Clear: %zu\n", array_size(arr));
    
    array_deallocate(arr);
    return 0;
}
```