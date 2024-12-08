
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

### `Array* array_create(size_t element_size, size_t size);`
- **Purpose**:  
  Creates a new `Array` with the specified element size and an initial number of elements. It allocates memory for the internal structure and the vector that stores the elements.

- **Parameters**:  
  - `element_size`: The size of each element in bytes.
  - `size`: The initial number of elements in the array.
  
- **Return Value**:  
  Returns a pointer to the newly created `Array`. Exits the program if memory allocation fails.

---

### `bool array_is_equal(const Array* arr1, const Array* arr2);`
- **Purpose**:  
  Checks whether two arrays are equal by comparing the size and elements of the arrays.

- **Parameters**:  
  - `arr1`: A pointer to the first `Array`.
  - `arr2`: A pointer to the second `Array`.
  
- **Return Value**:  
  Returns `true` if the arrays are equal in size and content, `false` otherwise.

---

### `bool array_is_less(const Array* arr1, const Array* arr2);`
- **Purpose**:  
  Compares two arrays lexicographically to check if the first array is smaller than the second.

- **Parameters**:  
  - `arr1`: A pointer to the first `Array`.
  - `arr2`: A pointer to the second `Array`.
  
- **Return Value**:  
  Returns `true` if `arr1` is lexicographically less than `arr2`, `false` otherwise.

---

### `bool array_is_greater(const Array* arr1, const Array* arr2);`
- **Purpose**:  
  Compares two arrays lexicographically to check if the first array is greater than the second.

- **Parameters**:  
  - `arr1`: A pointer to the first `Array`.
  - `arr2`: A pointer to the second `Array`.
  
- **Return Value**:  
  Returns `true` if `arr1` is lexicographically greater than `arr2`, `false` otherwise.

---

### `bool array_is_not_equal(const Array* arr1, const Array* arr2);`
- **Purpose**:  
  Checks if two arrays are not equal, either in size or content.

- **Parameters**:  
  - `arr1`: A pointer to the first `Array`.
  - `arr2`: A pointer to the second `Array`.
  
- **Return Value**:  
  Returns `true` if the arrays differ in size or content, `false` otherwise.

---

### `bool array_is_less_or_equal(const Array* arr1, const Array* arr2);`
- **Purpose**:  
  Checks if `arr1` is lexicographically less than or equal to `arr2`.

- **Parameters**:  
  - `arr1`: A pointer to the first `Array`.
  - `arr2`: A pointer to the second `Array`.
  
- **Return Value**:  
  Returns `true` if `arr1` is less than or equal to `arr2`, `false` otherwise.

---

### `bool array_is_greater_or_equal(const Array* arr1, const Array* arr2);`
- **Purpose**:  
  Checks if `arr1` is lexicographically greater than or equal to `arr2`.

- **Parameters**:  
  - `arr1`: A pointer to the first `Array`.
  - `arr2`: A pointer to the second `Array`.
  
- **Return Value**:  
  Returns `true` if `arr1` is greater than or equal to `arr2`, `false` otherwise.

---

### `bool array_empty(Array* arr);`
- **Purpose**:  
  Checks if the array is empty.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns `true` if the array is empty or `NULL`, `false` otherwise.

---

### `void array_deallocate(Array* arr);`
- **Purpose**:  
  Deallocates the memory used by the array and its internal vector.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  No return value. Safely deallocates resources.

---

### `void array_set(Array* arr, size_t index, const void* value);`
- **Purpose**:  
  Sets the value of an element at the specified index.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  - `index`: The index where the value will be set.
  - `value`: A pointer to the value to be set.
  
- **Return Value**:  
  No return value. Logs errors for invalid parameters.

---

### `void array_insert(Array* mainArr, const Array* otherArr, size_t index);`
- **Purpose**:  
  Inserts elements from `otherArr` into `mainArr` starting at the specified index.

- **Parameters**:  
  - `mainArr`: A pointer to the main `Array` where elements will be inserted.
  - `otherArr`: A pointer to the `Array` whose elements will be inserted.
  - `index`: The position in `mainArr` where the elements will be inserted.
  
- **Return Value**:  
  No return value. Logs errors for invalid parameters.

---

### `void array_fill(Array* arr, const void* value);`
- **Purpose**:  
  Fills the array with the specified value.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  - `value`: A pointer to the value to be set for each element.
  
- **Return Value**:  
  No return value. Logs errors for invalid parameters.

---

### `void array_swap(Array* arr1, Array* arr2);`
- **Purpose**:  
  Swaps the contents of two arrays by exchanging their internal vectors.

- **Parameters**:  
  - `arr1`: A pointer to the first `Array`.
  - `arr2`: A pointer to the second `Array`.
  
- **Return Value**:  
  No return value. Logs errors for invalid parameters.

---

### `void* array_at(Array* arr, size_t index);`
- **Purpose**:  
  Retrieves a pointer to the element at the specified index.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  - `index`: The index of the element to retrieve.
  
- **Return Value**:  
  Returns a pointer to the element or `NULL` if the index is out of bounds.

---

### `void* array_begin(Array* arr);`
- **Purpose**:  
  Returns a pointer to the first element of the array.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns a pointer to the first element or `NULL` if the array is empty.

---

### `void* array_end(Array* arr);`
- **Purpose**:  
  Returns a pointer to one element past the last element of the array.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns a pointer to one element past the last element or `NULL` if the array is empty.

---

### `void* array_rbegin(Array* arr);`
- **Purpose**:  
  Returns a pointer to the last element of the array (reverse iteration).

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns a pointer to the last element or `NULL` if the array is empty.

---

### `void* array_rend(Array* arr);`
- **Purpose**:  
  Returns a pointer to one element before the first element (for reverse iteration).

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns a pointer to one element before the first or `NULL` if the array is empty.

---

### `void* array_front(Array* arr);`
- **Purpose**:  
  Returns a pointer to the first element of the array.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns a pointer to the first element or `NULL` if the array is empty.

---

### `void* array_back(Array* arr);`
- **Purpose**:  
  Returns a pointer to the last element of the array.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns a pointer to the last element or `NULL` if the array is empty.

---

### `void* array_data(Array* arr);`
- **Purpose**:  
  Returns a pointer to the underlying data of the array.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns a pointer to the data or `NULL` if the array is empty.

---

### `size_t array_size(Array* arr);`
- **Purpose**:  
  Returns the number of elements currently stored in the array.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns the size of the array or `0` if the array is `NULL`.

---

### `size_t array_max_size(Array* arr);`
- **Purpose**:  
  Returns the maximum number of elements that the array can hold.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns the maximum size of the array.

---

### `const void* array_cbegin(Array* arr);`
- **Purpose**:  
  Returns a constant pointer to the first element of the array.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns a constant pointer to the first element or `NULL` if the array is empty.

---

### `const void* array_cend(Array* arr);`
- **Purpose**:  
  Returns a constant pointer to one element past the last element.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns a constant pointer to one element past the last element or `NULL`.

---

### `const void* array_crbegin(Array* arr);`
- **Purpose**:  
  Returns a constant reverse pointer to the last element of the array.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns a constant reverse pointer to the last element or `NULL`.

---

### `const void* array_crend(Array* arr);`
- **Purpose**:  
  Returns a constant reverse pointer to one element before the first element.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  Returns a constant reverse pointer to one element before the first element or `NULL`.

---

### `void array_clear(Array* arr);`
- **Purpose**:  
  Clears all elements from the array, resetting its size to zero.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  No return value.

---

### `void array_reverse(Array* arr);`
- **Purpose**:  
  Reverses the order of elements in the array.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  
- **Return Value**:  
  No return value.

---

### `void array_sort(Array* arr, int (*compare)(const void*, const void*));`
- **Purpose**:  
  Sorts the elements in the array using the specified comparison function.

- **Parameters**:  
  - `arr`: A pointer to the `Array`.
  - `compare`: A function pointer to a comparison function.
  
- **Return Value**:  
  No return value.

---

### `void array_copy(Array* dest, const Array* src);`
- **Purpose**:  
  Copies the contents of the source array into the destination array.

- **Parameters**:  
  - `dest`: A pointer to the destination `Array`.
  - `src`: A pointer to the source `Array`.
  
- **Return Value**:  
  No return value.

---

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
    fmt_println("------------------");

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
**Result**
```
Number is 42
Number is 42
Number is 42
Number is 42
Number is 42
------------------
Element 0: 42
Element 1: 42
Element 2: 42
Element 3: 42
Element 4: 42
```

---

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
**Result**
```
Array is not empty.
```

---

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
**Result**
```
Front element: 1563, Back element: 42
```

---

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
**Result**
```
Element 0: 0
Element 1: 2
Element 2: 4
Element 3: 6
Element 4: 8
```

---

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
**Result**
```
Element: 1.500000
Element: 2.500000
Element: 3.500000
Element: 4.500000
Element: 5.500000
```

---

## Example 6 : Using `array_cbegin` and 'cend' with char*

```c
#include "fmt/fmt.h"
#include "array/array.h"

int main() {
    Array* stringArray = array_create(sizeof(char*), 3);
    const char* strings[] = {"Hello", "World", "Array"};

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
**Result**
```
String: Hello
String: World
String: Array
```

---

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
**Result**
```
Arrays are equal.
```

---


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
**Result**
```
Element: 4.400000
Element: 3.300000
Element: 2.200000
Element: 1.100000
```

---

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
**Result**
```
0 0 0 0 
1 1 1 1
2 2 2 2
```

---

## Example 10 : Use String object in Array 

```c
#include "fmt/fmt.h"
#include "array/array.h"
#include "string/std_string.h"

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
**Result**
```
String 0: Initial Text - Appended Text
String 1: Initial Text - Appended Text
String 2: Initial Text - Appended Text
```

---

## Example 11: String Concatenation using Array and String Libraries

This example creates an array of strings and concatenates them into a single string.
```c
#include "array/array.h"
#include "string/std_string.h"
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
**Resutl**
```
Concatenated String: Hello World Array
```

---

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

    for (Suit suit = CLUBS; suit <= SPADES; 
    #ifdef __cplusplus
        suit = static_cast<Suit>(suit + 1))
    #else 
        suit++)
    #endif  
    {
        for (Rank rank = TWO; rank <= ACE; 
    #ifdef __cplusplus
        rank = static_cast<Rank>(rank + 1))
    #else 
        rank++)
    #endif  
       {
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
**Result**
```
2 of Clubs
Ace of Spades
```

---

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
**Result**
```
Source Array: 0 10 20 30 40 
Destination Array: 0 10 20 30 40
```

---

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
**Result**
```
Sorted Array: 10 20 30 40 50
```

---

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
**Result**
```
Reversed Array: 40 30 20 10 0
```

---

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
**Result**
```
Array Size After Clear: 0
```

---

## License

This project is open-source and available under [ISC License].