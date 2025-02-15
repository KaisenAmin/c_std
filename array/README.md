
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

---

### `array_create(type, N, name)`
- **Purpose**:  
  Declares a new fixed‑size array type named `name` that contains exactly `N` elements of the specified type. The array storage is allocated on the stack.
  
- **Parameters**:  
  - `type`: The element type.
  - `N`: The number of elements (must be a compile‑time constant).
  - `name`: The new type name for this fixed‑size array.
  
- **Usage Example**:  
  ```c
  #include "array/array.h"
  
  // Declare a new type 'ArrayInt5' that holds 5 ints.
  array_create(int, 5, ArrayInt5);
  
  int main(void) {
      ArrayInt5 arr; // stack-allocated array of 5 ints
      return 0;
  }
  ```

---

### `array_size(arr)`
- **Purpose**:  
  Returns the number of elements in the array. Since the array is fixed‑size, this value is computed at compile time.
  
- **Parameters**:  
  - `arr`: An instance of a fixed‑size array (declared using `array_create`).
  
- **Return Value**:  
  The number of elements in the array.
  
- **Usage Example**:  
  ```c
  ArrayInt5 arr;
  fmt_printf("Size: %zu\n", array_size(arr));  // Prints "Size: 5"
  ```

---

### `array_at(arr, index)`
- **Purpose**:  
  Retrieves the element at the specified index (with no bounds checking).
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  - `index`: The index of the desired element (0‑based).
  
- **Return Value**:  
  The element at the given index.
  
- **Usage Example**:  
  ```c
  array_at(arr, 2) = 42;
  fmt_printf("Element 2: %d\n", array_at(arr, 2));
  ```

---

### `array_begin(arr)`  
- **Purpose**:  
  Returns a pointer to the first element of the array.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  
- **Return Value**:  
  A pointer to the first element.
  
- **Usage Example**:  
  ```c
  int* p = array_begin(arr);
  ```

---

### `array_end(arr)`  
- **Purpose**:  
  Returns a pointer to one past the last element of the array.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  
- **Return Value**:  
  A pointer to one element past the last element.
  
- **Usage Example**:  
  ```c
  int* end = array_end(arr);
  ```

---

### `array_front(arr)`  
- **Purpose**:  
  Returns the first element of the array (by value).
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  
- **Return Value**:  
  The first element.
  
- **Usage Example**:  
  ```c
  fmt_printf("First: %d\n", array_front(arr));
  ```

---

### `array_back(arr)`  
- **Purpose**:  
  Returns the last element of the array (by value).
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  
- **Return Value**:  
  The last element.
  
- **Usage Example**:  
  ```c
  fmt_printf("Last: %d\n", array_back(arr));
  ```

---

### `array_data(arr)`  
- **Purpose**:  
  Returns a pointer to the underlying data of the array (equivalent to `array_begin`).
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  
- **Return Value**:  
  A pointer to the first element.
  
- **Usage Example**:  
  ```c
  int* data = array_data(arr);
  ```

---

### `array_fill(arr, value)`  
- **Purpose**:  
  Fills every element of the array with the given value.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  - `value`: The value to assign to each element.
  
- **Return Value**:  
  No return value.
  
- **Usage Example**:  
  ```c
  array_fill(arr, 42);  // sets every element to 42
  ```

---

### `array_reverse(arr)`  
- **Purpose**:  
  Reverses the order of elements in the array in place.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  
- **Return Value**:  
  No return value.
  
- **Usage Example**:  
  ```c
  array_reverse(arr);
  ```

---

### `array_sort(arr, compare)`  
- **Purpose**:  
  Sorts the elements of the array in place using the specified comparison function.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  - `compare`: A pointer to a comparison function (compatible with qsort), which should return an integer less than, equal to, or greater than zero.
  
- **Return Value**:  
  No return value.
  
- **Usage Example**:  
  ```c
  int compare_ints(const void *a, const void *b) {
      int ia = *(const int*)a;
      int ib = *(const int*)b;
      return ia - ib;
  }
  array_sort(arr, compare_ints);
  ```

---

### `array_copy(dest, src)`  
- **Purpose**:  
  Copies the contents of the source array into the destination array.  
  **Note:** Both arrays must be of the same type and fixed size.
  
- **Parameters**:  
  - `dest`: The destination fixed‑size array.
  - `src`: The source fixed‑size array.
  
- **Return Value**:  
  No return value.
  
- **Usage Example**:  
  ```c
  array_copy(dest, src);
  ```

---

### `array_swap(arr1, arr2)`  
- **Purpose**:  
  Exchanges the contents of the two arrays. Both arrays must be of the same type and fixed size.
  
- **Parameters**:  
  - `arr1`: The first fixed‑size array.
  - `arr2`: The second fixed‑size array.
  
- **Return Value**:  
  No return value.
  
- **Usage Example**:  
  ```c
  array_swap(arr1, arr2);
  ```

---

### `array_clear(arr)`  
- **Purpose**:  
  Clears the array by setting all bytes of its storage to zero.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  
- **Return Value**:  
  No return value.
  
- **Usage Example**:  
  ```c
  array_clear(arr);
  ```

---

### `array_find(arr, key, cmp)`  
- **Purpose**:  
  Searches the array for an element that is equal to the provided key using the specified comparison function.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  - `key`: A pointer to the key to search for.
  - `cmp`: A comparison function that takes two `const void*` arguments and returns 0 when equal.
  
- **Return Value**:  
  A pointer to the matching element if found, or `NULL` if no element matches.
  
- **Usage Example**:  
  ```c
  int *found = array_find(arr, &key, compare_ints);
  ```

---

### `array_find_if(arr, predicate)`  
- **Purpose**:  
  Searches the array for an element that satisfies the given predicate.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  - `predicate`: A function pointer with the signature `bool predicate(const void*)` that returns `true` for a matching element.
  
- **Return Value**:  
  A pointer to the first element for which the predicate returns `true`, or `NULL` if none do.
  
- **Usage Example**:  
  ```c
  int *found = array_find_if(arr, is_42);
  ```

---

### `array_for_each(arr, func)`  
- **Purpose**:  
  Applies a user‑provided function to every element in the array.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  - `func`: A function pointer that accepts a pointer to an element.
  
- **Return Value**:  
  No return value.
  
- **Usage Example**:  
  ```c
  void print_int(void *elem) { fmt_printf("%d ", *(int*)elem); }
  array_for_each(arr, print_int);
  ```

---

### `array_transform(src, dest, func)`  
- **Purpose**:  
  Applies a transformation function to each element of the source array and stores the result in the destination array.  
  Both arrays must be of the same type and fixed size.
  
- **Parameters**:  
  - `src`: The source fixed‑size array.
  - `dest`: The destination fixed‑size array.
  - `func`: A transformation function with signature `void func(const void *src_elem, void *dest_elem)`.
  
- **Return Value**:  
  No return value.
  
- **Usage Example**:  
  ```c
  void increment(const void *src, void *dest) {
      *(int*)dest = *(const int*)src + 1;
  }
  array_transform(src_arr, dest_arr, increment);
  ```

---

### `array_reduce(arr, result, reduce_func)`  
- **Purpose**:  
  Reduces the array to a single value by repeatedly applying a binary reduction function.  
  The reduction function should combine two elements into a single result.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  - `result`: A pointer to memory (at least the size of one element) where the result will be stored.
  - `reduce_func`: A function with the signature  
    `void reduce_func(const void *accumulator, const void *element, void *result)`  
    that combines the accumulator and the next element.
  
- **Return Value**:  
  No return value.
  
- **Usage Example**:  
  ```c
  void add_reduce(const void *a, const void *b, void *result) {
      *(int*)result = *(const int*)a + *(const int*)b;
  }
  int sum;
  array_reduce(arr, &sum, add_reduce);
  ```

---

### `array_count_if(arr, predicate)`  
- **Purpose**:  
  Counts the number of elements in the array that satisfy the given predicate.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  - `predicate`: A function pointer with the signature `bool predicate(const void*)` that returns `true` for matching elements.
  
- **Return Value**:  
  The number of elements for which the predicate returns `true`.
  
- **Usage Example**:  
  ```c
  size_t count = array_count_if(arr, is_42);
  ```

---

### `array_any_of(arr, predicate)`  
- **Purpose**:  
  Checks if any element in the array satisfies the given predicate.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  - `predicate`: A predicate function with signature `bool predicate(const void*)`.
  
- **Return Value**:  
  `true` if at least one element satisfies the predicate, `false` otherwise.
  
- **Usage Example**:  
  ```c
  bool found = array_any_of(arr, is_42);
  ```

---

### `array_all_of(arr, predicate)`  
- **Purpose**:  
  Checks if every element in the array satisfies the given predicate.
  
- **Parameters**:  
  - `arr`: A fixed‑size array instance.
  - `predicate`: A predicate function with signature `bool predicate(const void*)`.
  
- **Return Value**:  
  `true` if all elements satisfy the predicate, `false` otherwise.
  
- **Usage Example**:  
  ```c
  bool all = array_all_of(arr, geq_10);
  ```

---

### `array_is_equal(arr1, arr2)`  
- **Purpose**:  
  Compares two fixed‑size arrays lexicographically to determine if they are equal.
  
- **Parameters**:  
  - `arr1`: The first array.
  - `arr2`: The second array.
  
- **Return Value**:  
  `true` if the arrays have identical contents, `false` otherwise.
  
- **Usage Example**:  
  ```c
  if (array_is_equal(arr1, arr2))
      fmt_println("Arrays are equal.");
  ```

---

### `array_is_less(arr1, arr2)`  
- **Purpose**:  
  Checks if the first array is lexicographically less than the second.
  
- **Parameters**:  
  - `arr1`: The first array.
  - `arr2`: The second array.
  
- **Return Value**:  
  `true` if `arr1` is lexicographically less than `arr2`, `false` otherwise.
  
- **Usage Example**:  
  ```c
  if (array_is_less(arr1, arr2))
      fmt_println("First array is less.");
  ```

---

### `array_is_greater(arr1, arr2)`  
- **Purpose**:  
  Checks if the first array is lexicographically greater than the second.
  
- **Parameters**:  
  - `arr1`: The first array.
  - `arr2`: The second array.
  
- **Return Value**:  
  `true` if `arr1` is lexicographically greater than `arr2`, `false` otherwise.
  
- **Usage Example**:  
  ```c
  if (array_is_greater(arr1, arr2))
      fmt_println("First array is greater.");
  ```

---

### `array_is_not_equal(arr1, arr2)`  
- **Purpose**:  
  Checks if two arrays differ in content.
  
- **Parameters**:  
  - `arr1`: The first array.
  - `arr2`: The second array.
  
- **Return Value**:  
  `true` if the arrays are not equal, `false` otherwise.
  
- **Usage Example**:  
  ```c
  if (array_is_not_equal(arr1, arr2))
      fmt_println("Arrays differ.");
  ```

---

### `array_is_less_or_equal(arr1, arr2)`  
- **Purpose**:  
  Checks if the first array is lexicographically less than or equal to the second.
  
- **Parameters**:  
  - `arr1`: The first array.
  - `arr2`: The second array.
  
- **Return Value**:  
  `true` if `arr1` is less than or equal to `arr2`, `false` otherwise.
  
- **Usage Example**:  
  ```c
  if (array_is_less_or_equal(arr1, arr2))
      fmt_println("First array is less or equal.");
  ```

---

### `array_is_greater_or_equal(arr1, arr2)`  
- **Purpose**:  
  Checks if the first array is lexicographically greater than or equal to the second.
  
- **Parameters**:  
  - `arr1`: The first array.
  - `arr2`: The second array.
  
- **Return Value**:  
  `true` if `arr1` is greater than or equal to `arr2`, `false` otherwise.
  
- **Usage Example**:  
  ```c
  if (array_is_greater_or_equal(arr1, arr2))
      fmt_println("First array is greater or equal.");
  ```

---

### Example 1: Create an Array of `int` and Fill It Using `array_fill`

```c
#include "fmt/fmt.h"
#include "array/array.h"


array_create(int, 5, ArrayInt5);

int main(void) {
    ArrayInt5 arr;

    array_fill(arr, 42);
    for (size_t i = 0; i < array_size(arr); i++) {
         fmt_printf("Number is %d\n", array_at(arr, i));
    }

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
```

---

### Example 2: Check If an Array Is “Empty” (All Zeros) Using `array_clear` and `array_all_of`

```c
#include "fmt/fmt.h"
#include "array/array.h"


array_create(int, 5, ArrayInt5);

bool is_zero(const void *p) {
    return (*(const int*)p == 0);
}

int main(void) {
    ArrayInt5 arr;
    array_clear(arr);

    if (array_all_of(arr, is_zero)) {
         fmt_println("Array is empty (all zeros).");
    }
    else {
         fmt_println("Array is not empty.");
    }

    return 0;
}
```

**Result**
```
Array is empty (all zeros).
```

---

### Example 3: Using `array_front` and `array_back`

```c
#include "fmt/fmt.h"
#include "array/array.h"

array_create(int, 5, ArrayInt5);

int main(void) {
    ArrayInt5 arr;

    array_fill(arr, 42);
    array_at(arr, 0) = 100;

    fmt_printf("Front element: %d, Back element: %d\n", array_front(arr), array_back(arr));

    return 0;
}
```

**Result**
```
Front element: 100, Back element: 42
```

---

### Example 4: Setting Data at Specific Positions

```c
#include "fmt/fmt.h"
#include "array/array.h"


array_create(int, 5, ArrayInt5);

int main(void) {
    ArrayInt5 arr;
    array_fill(arr, 42);

    for (size_t i = 0; i < array_size(arr); ++i) {
         int value = i * 2; 
         array_at(arr, i) = value;

         fmt_printf("Element %zu: %d\n", i, array_at(arr, i));
    }

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

### Example 5: Iterating Over an Array of `double` Using `array_begin` and `array_end`

```c
#include "fmt/fmt.h"
#include "array/array.h"

array_create(double, 5, ArrayDouble5);

int main(void) {
    ArrayDouble5 arr;
    double values[] = {1.5, 2.5, 3.5, 4.5, 5.5};

    for (size_t i = 0; i < array_size(arr); ++i) {
         array_at(arr, i) = values[i];
    }
    for (double *it = array_begin(arr); it != array_end(arr); ++it) {
         fmt_printf("Element: %f\n", *it);
    }

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

### Example 6: Using an Array of `const char*`

```c
#include "fmt/fmt.h"
#include "array/array.h"

array_create(const char*, 3, ArrayStr3);

int main(void) {
    ArrayStr3 arr;
    const char* strings[] = {"Hello", "World", "Array"};

    for (size_t i = 0; i < array_size(arr); ++i) {
        array_at(arr, i) = strings[i];
    }
    for (size_t i = 0; i < array_size(arr); ++i) {
        fmt_printf("String: %s\n", array_at(arr, i));
    }

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

### Example 7: Comparing Two Arrays Using Relational Operators

```c
#include "fmt/fmt.h"
#include "array/array.h"


array_create(int, 3, ArrayInt3);

int main(void) {
    ArrayInt3 arr1, arr2;
    int vals1[] = {1, 2, 3};
    int vals2[] = {1, 2, 3};
    for (size_t i = 0; i < array_size(arr1); ++i) {
         array_at(arr1, i) = vals1[i];
         array_at(arr2, i) = vals2[i];
    }

    if (array_is_equal(arr1, arr2)) {
        fmt_println("Arrays are equal.");
    }
    else {
        fmt_println("Arrays are not equal.");
    }

    return 0;
}
```

**Result**
```
Arrays are equal.
```

---

### Example 8: Reversing an Array of `float`

```c
#include "fmt/fmt.h"
#include "array/array.h"


array_create(float, 4, ArrayFloat4);

int main(void) {
    ArrayFloat4 arr;
    float values[] = {1.1f, 2.2f, 3.3f, 4.4f};

    for (size_t i = 0; i < array_size(arr); ++i) {
        array_at(arr, i) = values[i];
    }

    array_reverse(arr);
    for (size_t i = 0; i < array_size(arr); ++i) {
        fmt_printf("Element: %f\n", array_at(arr, i));
    }

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

### Example 9: Creating and Using a 2D Array

```c
#include "fmt/fmt.h"
#include "array/array.h"


array_create(int, 4, ArrayInt4);
array_create(ArrayInt4, 3, Matrix3x4);

int main(void) {
    Matrix3x4 matrix;
    

    for (size_t i = 0; i < array_size(matrix); i++) {
         for (size_t j = 0; j < array_size(matrix.data[0]); j++) {
              matrix.data[i].data[j] = i;
         }
    }
   
    for (size_t i = 0; i < array_size(matrix); i++) {
         for (size_t j = 0; j < array_size(matrix.data[0]); j++) {
              fmt_printf("%d ", matrix.data[i].data[j]);
         }
         fmt_println("");
    }

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

### Example 10: Using an Array of String Objects

```c
#include "fmt/fmt.h"
#include "array/array.h"
#include "string/std_string.h"

array_create(String*, 3, ArrayStrPtr3);

int main(void) {
    ArrayStrPtr3 arr;
    for (size_t i = 0; i < array_size(arr); i++) {
         String* str = string_create("Initial Text");
         array_at(arr, i) = str;
    }
    for (size_t i = 0; i < array_size(arr); i++) {
         String* str = array_at(arr, i);
         string_append(str, " - Appended");
         fmt_printf("String %zu: %s\n", i, string_c_str(str));
         string_deallocate(str);
    }
    return 0;
}
```

**Result**
```
String 0: Initial Text - Appended
String 1: Initial Text - Appended
String 2: Initial Text - Appended
```

---

### Example 11: String Concatenation Using an Array and the String Library

```c
#include "fmt/fmt.h"
#include "array/array.h"
#include "string/std_string.h"


array_create(String*, 3, ArrayStrPtr3);

int main(void) {
    ArrayStrPtr3 arr;
    const char* words[] = {"Hello", "World", "Array"};

    for (size_t i = 0; i < array_size(arr); i++) {
         String* str = string_create(words[i]);
         array_at(arr, i) = str;
    }

    String* concatenated = string_create("");
    for (size_t i = 0; i < array_size(arr); i++) {
         String* str = array_at(arr, i);
         string_append(concatenated, string_c_str(str));
         if (i < array_size(arr) - 1)
             string_append(concatenated, " ");
    }

    fmt_printf("Concatenated String: %s\n", string_c_str(concatenated));
    string_deallocate(concatenated);

    for (size_t i = 0; i < array_size(arr); i++) {
         String* str = array_at(arr, i);
         string_deallocate(str);
    }

    return 0;
}
```

**Result**
```
Concatenated String: Hello World Array
```

---

### Example 12: A Simple Card Game Using an Array of Cards

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
    TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
    JACK, QUEEN, KING, ACE
} Rank;

typedef struct {
    Suit suit;
    Rank rank;
} Card;


array_create(Card, 52, Deck);

void printCard(const Card* card) {
    const char* suits[] = {"Clubs", "Diamonds", "Hearts", "Spades"};
    const char* ranks[] = {"", "", "2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace"};
    fmt_printf("%s of %s\n", ranks[card->rank], suits[card->suit]);
}

int main(void) {
    Deck deck;
    int index = 0;

    for (Suit suit = CLUBS; suit <= SPADES; suit++) {
         for (Rank rank = TWO; rank <= ACE; rank++) {
              deck.data[index].suit = suit;
              deck.data[index].rank = rank;
              index++;
         }
    }
 
    printCard(&deck.data[0]);                   
    printCard(&deck.data[array_size(deck) - 1]);

    return 0;
}
```

**Result**
```
2 of Clubs
Ace of Spades
```

---

### Example 13: Demonstrating `array_copy`

```c
#include "fmt/fmt.h"
#include "array/array.h"


array_create(int, 5, ArrayInt5);

int main(void) {
    ArrayInt5 src, dest;
    for (size_t i = 0; i < array_size(src); i++) {
         array_at(src, i) = i * 10;  
    }

    array_copy(dest, src);
    fmt_printf("Source Array: ");
    for (size_t i = 0; i < array_size(src); i++) {
         fmt_printf("%d ", array_at(src, i));
    }

    fmt_println("");
    fmt_printf("Destination Array: ");
    for (size_t i = 0; i < array_size(dest); i++) {
         fmt_printf("%d ", array_at(dest, i));
    }
    fmt_println("");

    return 0;
}
```

**Result**
```
Source Array: 0 10 20 30 40 
Destination Array: 0 10 20 30 40 
```

---

### Example 14: Demonstrating `array_sort`

```c
#include "fmt/fmt.h"
#include "array/array.h"

array_create(int, 5, ArrayInt5);

int compare_ints(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return ia - ib;
}

int main(void) {
    ArrayInt5 arr;

    for (size_t i = 0; i < array_size(arr); i++) {
         array_at(arr, i) = 50 - i * 10; 
    }

    array_sort(arr, compare_ints);
    fmt_printf("Sorted Array: ");

    for (size_t i = 0; i < array_size(arr); i++) {
         fmt_printf("%d ", array_at(arr, i));
    }
    fmt_println("");

    return 0;
}
```

**Result**
```
Sorted Array: 10 20 30 40 50 
```

---

### Example 15: Demonstrating Aggregate/Transformation Functions

```c
#include "fmt/fmt.h"
#include "array/array.h"

array_create(int, 5, ArrayInt5);

/* Transformation function: add 1 to the source element */
void add_one(const void *src, void *dest) {
    *(int*)dest = *(const int*)src + 1;
}

/* Reduction function: sum two ints */
void sum_reduce(const void *a, const void *b, void *result) {
    *(int*)result = *(const int*)a + *(const int*)b;
}

/* Predicate: check if the value equals 42 */
bool is_42(const void *elem) {
    return (*(const int*)elem == 42);
}

/* Predicate: check if the value is at least 10 */
bool geq_10(const void *elem) {
    return (*(const int*)elem >= 10);
}

int main(void) {
    ArrayInt5 arr, transformed;

    for (size_t i = 0; i < array_size(arr); i++) {
         array_at(arr, i) = i * 10; 
    }
 
    array_transform(arr, transformed, add_one);
    fmt_printf("Transformed Array: ");
    for (size_t i = 0; i < array_size(transformed); i++) {
         fmt_printf("%d ", array_at(transformed, i));
    }
    fmt_println("");
    
    int sum = 0;
    array_reduce(arr, &sum, sum_reduce);
    fmt_printf("Sum: %d\n", sum);

    size_t count = array_count_if(arr, is_42);
    fmt_printf("Count of 42: %zu\n", count);
    
    // Check if any element is at least 10.
    bool any_geq_10 = array_any_of(arr, geq_10);
    fmt_printf("Any element >= 10? %s\n", any_geq_10 ? "yes" : "no");
    
    // Check if all elements are at least 10.
    bool all_geq_10 = array_all_of(arr, geq_10);
    fmt_printf("All elements >= 10? %s\n", all_geq_10 ? "yes" : "no");
    
    return 0;
}
```

**Result**
```
Transformed Array: 1 11 21 31 41 
Sum: 100
Count of 42: 0
Any element >= 10? yes
All elements >= 10? no
```
---

### Example 16: Array of `char` – Reversing a Sequence of Letters

This example creates an array of 10 characters, fills it with the letters A–J, prints the array, reverses it, and prints it again.

```c
#include "fmt/fmt.h"
#include "array/array.h"

array_create(char, 10, ArrayChar10);

int main(void) {
    ArrayChar10 arr;
    
    // Fill the array with letters 'A' to 'J'
    for (size_t i = 0; i < array_size(arr); i++) {
        array_at(arr, i) = 'A' + i;
    }
    
    fmt_println("Original array:");
    for (size_t i = 0; i < array_size(arr); i++) {
        fmt_printf("%c ", array_at(arr, i));
    }
    fmt_println("");
    
    array_reverse(arr);
    
    fmt_println("Reversed array:");
    for (size_t i = 0; i < array_size(arr); i++) {
        fmt_printf("%c ", array_at(arr, i));
    }
    fmt_println("");
    
    return 0;
}
```

**Result**

```
Original array:
A B C D E F G H I J 
Reversed array:
J I H G F E D C B A 
```

---

### Example 17: Array of Custom Struct (`Person`) – Sorting by Age

In this example, a custom `Person` struct is defined. An array of three persons is created, filled with data, then sorted by age using a comparison function.

```c
#include "fmt/fmt.h"
#include "array/array.h"

typedef struct {
    char name[20];
    int age;
} Person;

// Comparison function to sort Person by age.
int compare_persons(const void *a, const void *b) {
    const Person *p1 = (const Person *)a;
    const Person *p2 = (const Person *)b;
    return p1->age - p2->age;
}

array_create(Person, 3, PersonArray);

int main(void) {
    PersonArray persons;
    
    persons.data[0] = (Person) { "Alice", 30 };
    persons.data[1] = (Person) { "Bob", 25 };
    persons.data[2] = (Person) { "Charlie", 35 };
    
    fmt_println("Before sorting:");
    for (size_t i = 0; i < array_size(persons); i++) {
        fmt_printf("%s, %d\n", persons.data[i].name, persons.data[i].age);
    }
    
    array_sort(persons, compare_persons);
    
    fmt_println("After sorting (by age):");
    for (size_t i = 0; i < array_size(persons); i++) {
        fmt_printf("%s, %d\n", persons.data[i].name, persons.data[i].age);
    }
    
    return 0;
}
```

**Result**

```
Before sorting:
Alice, 30
Bob, 25
Charlie, 35
After sorting (by age):
Bob, 25
Alice, 30
Charlie, 35
```

---

### Example 18: Array of `float` – Transforming Values

This example creates an array of floats, fills it with values 1.0–5.0, then uses a transformation function to double each value into a new array.

```c
#include "fmt/fmt.h"
#include "array/array.h"

array_create(float, 5, ArrayFloat5);

// Transformation function: doubles the source value.
void double_value(const void *src, void *dest) {
    *(float*)dest = *(const float*)src * 2.0f;
}

int main(void) {
    ArrayFloat5 original, transformed;
    
    for (size_t i = 0; i < array_size(original); i++) {
        array_at(original, i) = (float)i + 1.0f;
    }
    array_transform(original, transformed, double_value);
    
    fmt_println("Original float array:");
    for (size_t i = 0; i < array_size(original); i++) {
        fmt_printf("%f ", array_at(original, i));
    }
    fmt_println("");
    
    fmt_println("Transformed (doubled) array:");
    for (size_t i = 0; i < array_size(transformed); i++) {
        fmt_printf("%f ", array_at(transformed, i));
    }
    fmt_println("");
    
    return 0;
}
```

**Result**

```
Original float array:
1.000000 2.000000 3.000000 4.000000 5.000000 
Transformed (doubled) array:
2.000000 4.000000 6.000000 8.000000 10.000000 
```

---

### Example 19: Array of `bool` – Counting and Testing Conditions

In this example, an array of booleans is created and manually filled. The program then uses aggregate functions to count true values, and check if any or all elements satisfy a condition.

```c
#include "fmt/fmt.h"
#include "array/array.h"

array_create(bool, 8, ArrayBool8);

// Predicate function that returns true if the element is true.
bool is_true(const void *elem) {
    return *(const bool*)elem;
}

int main(void) {
    ArrayBool8 arr;

    arr.data[0] = true;
    arr.data[1] = false;
    arr.data[2] = true;
    arr.data[3] = true;
    arr.data[4] = false;
    arr.data[5] = false;
    arr.data[6] = true;
    arr.data[7] = false;
    
    size_t true_count = array_count_if(arr, is_true);
    fmt_printf("Number of true values: %zu\n", true_count);
    
    bool any_true = array_any_of(arr, is_true);
    fmt_printf("Any true? %s\n", any_true ? "yes" : "no");
    
    bool all_true = array_all_of(arr, is_true);
    fmt_printf("All true? %s\n", all_true ? "yes" : "no");
    
    return 0;
}
```

**Result**

```
Number of true values: 4
Any true? yes
All true? no
```

---

### Example 20: Array of Pointers to `int` – Swapping and Copying

This example demonstrates the use of an array of pointers. Two fixed‑size arrays of int pointers are created; their contents are swapped and then one array is copied over the other.

```c
#include "fmt/fmt.h"
#include "array/array.h"

array_create(int*, 3, ArrayIntPtr3);

int main(void) {
    ArrayIntPtr3 arr1, arr2;
    int a = 10, b = 20, c = 30;
    int d = 40, e = 50, f = 60;
    
    arr1.data[0] = &a;
    arr1.data[1] = &b;
    arr1.data[2] = &c;
    
    arr2.data[0] = &d;
    arr2.data[1] = &e;
    arr2.data[2] = &f;
    
    fmt_println("Before swap, arr1:");
    for (size_t i = 0; i < array_size(arr1); i++) {
        fmt_printf("%d ", *array_at(arr1, i));
    }
    fmt_println("");
    
    fmt_println("Before swap, arr2:");
    for (size_t i = 0; i < array_size(arr2); i++) {
        fmt_printf("%d ", *array_at(arr2, i));
    }
    fmt_println("");
    
    // Swap the contents of arr1 and arr2.
    array_swap(arr1, arr2);
    
    fmt_println("After swap, arr1:");
    for (size_t i = 0; i < array_size(arr1); i++) {
        fmt_printf("%d ", *array_at(arr1, i));
    }
    fmt_println("");
    
    fmt_println("After swap, arr2:");
    for (size_t i = 0; i < array_size(arr2); i++) {
        fmt_printf("%d ", *array_at(arr2, i));
    }
    fmt_println("");
    
    // Copy arr1 into arr2.
    array_copy(arr2, arr1);
    
    fmt_println("After copying arr1 into arr2, arr2:");
    for (size_t i = 0; i < array_size(arr2); i++) {
        fmt_printf("%d ", *array_at(arr2, i));
    }
    fmt_println("");
    
    return 0;
}
```

**Result**

```
Before swap, arr1:
10 20 30 
Before swap, arr2:
40 50 60 
After swap, arr1:
40 50 60 
After swap, arr2:
10 20 30 
After copying arr1 into arr2, arr2:
40 50 60 
```

---


## License

This project is open-source and available under [ISC License].