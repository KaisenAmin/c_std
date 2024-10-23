
# Bitset Library in C

**Author:** amin tahmasebi
**Release Date:** 2023
**License:** ISC License


The Bitset library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::bitset` in C++.

## Compilation

To compile the Bitset library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./bitset/bitset.c 
g++ -std=c++14 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.cpp

```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Bitset library in your project, include the `bitset.h` header file in your source code.

in these examples i rewrite cpp example in Bitset code 

```c
#include "array/array.h"
```
## Function Descriptions

### `Bitset* bitset_create(size_t num_bits);`
- **Purpose**:  
  Creates a new `Bitset` with the specified number of bits. It allocates memory for the bit array, initializing all bits to 0.
  
- **Parameters**:  
  - `num_bits`: The number of bits to be stored in the `Bitset`.
  
- **Return Value**:  
  Returns a pointer to the newly created `Bitset`. If memory allocation fails, the program exits.

---

### `void bitset_deallocate(Bitset *bs);`
- **Purpose**:  
  Frees the memory allocated for the `Bitset` and its internal bit array.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset` to be deallocated.
  
- **Return Value**:  
  No return value. It gracefully handles NULL pointers.

---

### `void bitset_print(const Bitset* bs);`
- **Purpose**:  
  Prints the bits in the `Bitset` to the console in a human-readable format (with the most significant bit on the left).
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset` to be printed.
  
- **Return Value**:  
  No return value. Logs errors for NULL pointers.

---

### `void bitset_set_from_string(Bitset* bs, const char* str);`
- **Purpose**:  
  Sets the bits of a `Bitset` based on a string of '0's and '1's.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  - `str`: A string containing '0's and '1's to set the bits.
  
- **Return Value**:  
  No return value. Logs a warning if the string is longer than the `Bitset`.

---

### `bool bitset_test(const Bitset *bs, size_t pos);`
- **Purpose**:  
  Tests whether the bit at the specified position is set (1) or not.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  - `pos`: The position of the bit to test.
  
- **Return Value**:  
  Returns `true` if the bit is set, `false` if it is cleared or if the position is invalid.

---

### `Bitset* bitset_set(Bitset* bs, size_t pos, bool value);`
- **Purpose**:  
  Sets or clears the bit at the specified position based on the provided value.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  - `pos`: The position of the bit to set or clear.
  - `value`: A boolean value (`true` to set the bit, `false` to clear it).
  
- **Return Value**:  
  Returns a pointer to the modified `Bitset`.

---

### `Bitset* bitset_reset(Bitset* bs, size_t pos);`
- **Purpose**:  
  Clears the bit at the specified position (sets it to 0).
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  - `pos`: The position of the bit to clear.
  
- **Return Value**:  
  Returns a pointer to the modified `Bitset`.

---

### `Bitset* bitset_flip_all(Bitset* bs);`
- **Purpose**:  
  Flips all the bits in the `Bitset` (1 becomes 0, and 0 becomes 1).
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  
- **Return Value**:  
  Returns a pointer to the modified `Bitset`.

---

### `Bitset* bitset_flip(Bitset* bs, size_t pos);`
- **Purpose**:  
  Flips the bit at the specified position (toggles it).
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  - `pos`: The position of the bit to flip.
  
- **Return Value**:  
  Returns a pointer to the modified `Bitset`.

---

### `bool bitset_all(const Bitset* bs);`
- **Purpose**:  
  Checks if all bits in the `Bitset` are set to 1.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  
- **Return Value**:  
  Returns `true` if all bits are set, `false` otherwise.

---

### `bool bitset_any(const Bitset* bs);`
- **Purpose**:  
  Checks if any bit in the `Bitset` is set to 1.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  
- **Return Value**:  
  Returns `true` if any bit is set, `false` if all bits are 0.

---

### `bool bitset_none(const Bitset* bs);`
- **Purpose**:  
  Checks if none of the bits in the `Bitset` are set (all bits are 0).
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  
- **Return Value**:  
  Returns `true` if all bits are 0, `false` otherwise.

---

### `size_t bitset_count(const Bitset* bs);`
- **Purpose**:  
  Counts the number of bits set to 1 in the `Bitset`.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  
- **Return Value**:  
  Returns the count of bits set to 1.

---

### `size_t bitset_size(const Bitset* bs);`
- **Purpose**:  
  Returns the total number of bits in the `Bitset`.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  
- **Return Value**:  
  Returns the size of the `Bitset` (number of bits).

---

### `unsigned long bitset_to_ulong(const Bitset* bs);`
- **Purpose**:  
  Converts the `Bitset` to an unsigned long integer.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  
- **Return Value**:  
  Returns the bitset as an unsigned long.

---

### `unsigned long long bitset_to_ullong(const Bitset* bs);`
- **Purpose**:  
  Converts the `Bitset` to an unsigned long long integer.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  
- **Return Value**:  
  Returns the bitset as an unsigned long long.

---

### `char* bitset_to_string(const Bitset* bs);`
- **Purpose**:  
  Converts the `Bitset` into a string of '1's and '0's representing the bits.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  
- **Return Value**:  
  Returns a dynamically allocated string of '1's and '0's representing the bitset.

---

### `Bitset* bitset_and(const Bitset* bs1, const Bitset* bs2);`
- **Purpose**:  
  Performs a bitwise AND operation between two `Bitsets`.
  
- **Parameters**:  
  - `bs1`: A pointer to the first `Bitset`.
  - `bs2`: A pointer to the second `Bitset`.
  
- **Return Value**:  
  Returns a new `Bitset` containing the result, or `NULL` if the `Bitsets` are of different sizes.

---

### `Bitset* bitset_or(const Bitset* bs1, const Bitset* bs2);`
- **Purpose**:  
  Performs a bitwise OR operation between two `Bitsets`.
  
- **Parameters**:  
  - `bs1`: A pointer to the first `Bitset`.
  - `bs2`: A pointer to the second `Bitset`.
  
- **Return Value**:  
  Returns a new `Bitset` containing the result, or `NULL` if the `Bitsets` are of different sizes.

---

### `Bitset* bitset_xor(const Bitset* bs1, const Bitset* bs2);`
- **Purpose**:  
  Performs a bitwise XOR operation between two `Bitsets`.
  
- **Parameters**:  
  - `bs1`: A pointer to the first `Bitset`.
  - `bs2`: A pointer to the second `Bitset`.
  
- **Return Value**:  
  Returns a new `Bitset` containing the result, or `NULL` if the `Bitsets` are of different sizes.

---

### `Bitset* bitset_not(const Bitset* bs);`
- **Purpose**:  
  Performs a bitwise NOT operation on a `Bitset` (flips all bits).
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  
- **Return Value**:  
  Returns a

 new `Bitset` containing the flipped bits.

---

### `Bitset* bitset_shift_left(const Bitset* bs, size_t shift);`
- **Purpose**:  
  Shifts all bits in the `Bitset` to the left by a specified number of positions.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  - `shift`: The number of positions to shift.
  
- **Return Value**:  
  Returns a new `Bitset` with the bits shifted left.

---

### `Bitset* bitset_shift_right(const Bitset* bs, size_t shift);`
- **Purpose**:  
  Shifts all bits in the `Bitset` to the right by a specified number of positions.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  - `shift`: The number of positions to shift.
  
- **Return Value**:  
  Returns a new `Bitset` with the bits shifted right.

---

### `bool bitset_is_equal(const Bitset* bs1, const Bitset* bs2);`
- **Purpose**:  
  Compares two `Bitsets` for equality (both size and bit values).
  
- **Parameters**:  
  - `bs1`: A pointer to the first `Bitset`.
  - `bs2`: A pointer to the second `Bitset`.
  
- **Return Value**:  
  Returns `true` if the `Bitsets` are equal, `false` otherwise.

---

### `bool bitset_is_not_equal(const Bitset* bs1, const Bitset* bs2);`
- **Purpose**:  
  Checks if two `Bitsets` are not equal (either size or bit values differ).
  
- **Parameters**:  
  - `bs1`: A pointer to the first `Bitset`.
  - `bs2`: A pointer to the second `Bitset`.
  
- **Return Value**:  
  Returns `true` if the `Bitsets` are not equal, `false` if they are the same.

---

### `bool bitset_at(const Bitset* bs, size_t pos);`
- **Purpose**:  
  Returns the value of the bit at the specified position in the `Bitset`.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  - `pos`: The position of the bit to retrieve.
  
- **Return Value**:  
  Returns `true` if the bit is set, `false` if it is cleared or if the position is invalid.

---

### `unsigned char* bitset_at_ref(Bitset* bs, size_t pos);`
- **Purpose**:  
  Returns a reference (pointer) to the byte that contains the bit at the specified position, allowing direct modification of the bit.
  
- **Parameters**:  
  - `bs`: A pointer to the `Bitset`.
  - `pos`: The position of the bit.
  
- **Return Value**:  
  Returns a pointer to the byte that contains the bit at the specified position.


--- 


## Example 1 : how to use `bitset_none` and `bitset_count`

```cpp

#include <iostream>       // std::cin, std::cout
#include <bitset>         // std::bitset

int main (){
  std::bitset<16> foo;

  std::cout << "Please, enter a binary number: ";
  std::cin >> foo;

  if (foo.none()) {
    std::cout << foo << " has no bits set.\n";
  }
  else {
    std::cout << foo << " has " << foo.count() << " bits set.\n";
  }

  return 0;
}
```
**Result in C++:**
```
Please, enter a binary number: 11010111
0000000011010111 has 6 bits set.
```

```c
#include "bitset/bitset.h" // Include your bitset header file
#include "fmt/fmt.h"

#define MAX_INPUT_SIZE 16 // Define the maximum input size

int main() {
    Bitset* foo = bitset_create(16);
    char input[MAX_INPUT_SIZE + 1]; // +1 for the null terminator
    
    fmt_printf("Please, enter a binary number: ");
    scanf("%16s", input); // Read up to 16 characters

    bitset_set_from_string(foo, input);     // Set the Bitset from the string input

    // Check if any bit is set and print the corresponding message
    if (bitset_none(foo)) {
        fmt_printf("Bitset: ");
        bitset_print(foo);
        fmt_printf(" has no bits set.\n");
    } 
    else {
        fmt_printf("Bitset: ");
        bitset_print(foo);
        fmt_printf(" has %zu bits set.\n", bitset_count(foo));
    }

    bitset_deallocate(foo);
    return 0;
}
```
**Result in C:**
```
Please, enter a binary number: 11010111
Bitset: 0000000011010111
 has 6 bits set.
```

---

## Example 2 : how to get size of bitset with `bitset_size` and flip it with `bitset_flip`
 
```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main(){
    Bitset* bi1 = bitset_create(4);
    Bitset* bi2 = bitset_create(8);

    bitset_set_from_string(bi1, "0001");

    fmt_printf("%llu\n", bitset_to_ullong(bi2));
    fmt_printf("%zu\n%zu\n", bitset_size(bi1), bitset_size(bi2));

    bitset_flip(bi1, 2);
    bitset_print(bi1);

    bitset_deallocate(bi1);
    bitset_deallocate(bi2);

    return 0;
}
```
**Result:**
```
0
4
8
0011
```

---

## Example 3 : Using `bitset_all` and `bitset_none` and `bitset_any`

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"


int main() {
    Bitset* foo = bitset_create(8);
    char input[9]; // 8 bits + null terminator
    
    fmt_printf("Please, enter an 8-bit binary number: ");
    scanf("%8s", input);

    bitset_set_from_string(foo, input);

    fmt_printf("all: %s\n", bitset_all(foo) ? "true" : "false");
    fmt_printf("any: %s\n", bitset_any(foo) ? "true" : "false");
    fmt_printf("none: %s\n", bitset_none(foo) ? "true" : "false");

    // Deallocate the bitset
    bitset_deallocate(foo);
    return 0;
}
```
**Result in C:**
```
Please, enter an 8-bit binary number: 11111111
all: true
any: true
none: false
```

```cpp
#include <iostream>       // std::cin, std::cout, std::boolalpha
#include <bitset>         // std::bitset

int main (){
  std::bitset<8> foo;

  std::cout << "Please, enter an 8-bit binary number: ";
  std::cin >> foo;

  std::cout << std::boolalpha;
  std::cout << "all: " << foo.all() << '\n';
  std::cout << "any: " << foo.any() << '\n';
  std::cout << "none: " << foo.none() << '\n';

  return 0;
}
```
**Result in C++:**
```
Please, enter an 8-bit binary number: 11111
all: false
any: true
none: false
```

---

## Example 4 : Using `bitset_any` and `bitset_count`

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

#define MAX_INPUT_SIZE 16

int main() {
    Bitset* foo = bitset_create(16);
    char input[MAX_INPUT_SIZE + 1]; // +1 for the null terminator

    fmt_printf("Please, enter a binary number: ");
    scanf("%16s", input);

    bitset_set_from_string(foo, input);

    if (bitset_any(foo)) {
        fmt_printf("Bitset: ");
        bitset_print(foo);
        fmt_printf("\bhas %zu bits set.\n", bitset_count(foo));
    } 
    else {
        fmt_printf("Bitset: ");
        bitset_print(foo);
        fmt_printf(" has no bits set.\n");
    }

    bitset_deallocate(foo);
    return 0;
}
```
**Result in C:**
```
Please, enter a binary number: 10110
Bitset: 0000000000010110
has 3 bits set.
```

```cpp

#include <iostream>       // std::cin, std::cout
#include <bitset>         // std::bitset

int main (){
  std::bitset<16> foo;

  std::cout << "Please, enter a binary number: ";
  std::cin >> foo;

  if (foo.any()) {
    std::cout << foo << " has " << foo.count() << " bits set.\n";
  }
  else {
    std::cout << foo << " has no bits set.\n";
  }

  return 0;
}
```
**Result in C++:**
```
Please, enter a binary number: 10110
0000000000010110 has 3 bits set.
```

---

## Example 5 : Manually set bits with `bitset_set`

```c
#include "fmt/fmt.h"
#include "bitset/bitset.h" 

int main() {
    Bitset* foo = bitset_create(8);

    // // Manually set the bits to match "10110011"
    bitset_set(foo, 0, true);  // MSB
    bitset_set(foo, 1, false);
    bitset_set(foo, 2, true);
    bitset_set(foo, 3, true);
    bitset_set(foo, 4, false);
    bitset_set(foo, 5, false);
    bitset_set(foo, 6, true);
    bitset_set(foo, 7, true);  // LSB

    // Print the bitset, count of ones, and count of zeros
    fmt_printf("Bitset: ");
    bitset_print(foo);
    fmt_printf("has %zu ones and %zu zeros.\n", bitset_count(foo), foo->size - bitset_count(foo));

    bitset_deallocate(foo);
    return 0;
}
```
**Result:**
```
Bitset: 11001101
has 5 ones and 3 zeros.
```

---

## Example 6 : use `bitset_print` in Bitset also flip 

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* myBitset = bitset_create(8);

    bitset_set_from_string(myBitset, "11010010");

    fmt_printf("Original bitset: ");
    bitset_print(myBitset);
    bitset_flip_all(myBitset);

    fmt_printf("After toggling: ");
    bitset_print(myBitset);

    bitset_deallocate(myBitset);
    return 0;
}
```
**Result in C:**
```
Original bitset: 11010010
After toggling: 00101101 
```

```cpp
#include <iostream>
#include <bitset>

int main() {
    std::bitset<8> myBitset("11010010");

    std::cout << "Original bitset: " << myBitset << std::endl;

    // Toggle all bits
    myBitset.flip();
    std::cout << "After toggling: " << myBitset << std::endl;

    return 0;
}
```
**Result in C++:**
```
Original bitset: 11010010
After toggling: 00101101 
```

---

## Example 7 : set and print 

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* myBitset = bitset_create(8);
    int pos;

    fmt_printf("Enter a position to set (0-7): ");
    scanf("%d", &pos);

    if (pos >= 0 && pos < 8) {
        bitset_set(myBitset, pos, true);
        fmt_printf("Bitset after setting position %d: ", pos);
        bitset_print(myBitset);
    }
    else {
        fmt_printf("Invalid position!\n");
    }

    bitset_deallocate(myBitset);
    return 0;
}
```
**Result in C:**
```
Enter a position to set (0-7): 2
Bitset after setting position 2: 00000100

Enter a position to set (0-7): 1
Bitset after setting position 1: 00000010
```

`C++`

```cpp 

#include <iostream>
#include <bitset>

int main() {
    std::bitset<8> myBitset;
    int pos;

    std::cout << "Enter a position to set (0-7): ";
    std::cin >> pos;

    if (pos >= 0 && pos < 8) {
        myBitset.set(pos);
        std::cout << "Bitset after setting position " << pos << ": " << myBitset << std::endl;
    } 
    else {
        std::cout << "Invalid position!" << std::endl;
    }

    return 0;
}
```
**Result:**
```
Enter a position to set (0-7): 3
Bitset after setting position 3: 00001000
```

---

---

## Example 8: Set specific bits and convert to unsigned long long

**C Implementation:**

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* myBitset = bitset_create(64);

    // Set specific bits
    bitset_set(myBitset, 1, true);
    bitset_set(myBitset, 3, true);
    bitset_set(myBitset, 5, true);
    bitset_set(myBitset, 7, true);

    fmt_printf("Bitset: ");
    bitset_print(myBitset);

    // Convert to unsigned long long and print
    unsigned long long value = bitset_to_ullong(myBitset);
    fmt_printf("Converted to unsigned long long: %llu\n", value);

    bitset_deallocate(myBitset);
    return 0;
}
```
**Result:**
```
Bitset: 0000000000000000000000000000000000000000000000000000000010101010
Converted to unsigned long long: 170
```

**C++ Implementation:**

```cpp
#include <iostream>
#include <bitset>

int main() {
    std::bitset<64> myBitset;

    // Set specific bits
    myBitset.set(1);
    myBitset.set(3);
    myBitset.set(5);
    myBitset.set(7);

    std::cout << "Bitset: " << myBitset << std::endl;

    unsigned long long value = myBitset.to_ullong();
    std::cout << "Converted to unsigned long long: " << value << std::endl;

    return 0;
}
```
**Result in C++:**
```
Bitset: 0000000000000000000000000000000000000000000000000000000010101010
Converted to unsigned long long: 170
```

---

## Example 9: Check bit status and count set bits

**C Implementation:**

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* myBitset = bitset_create(8);

    bitset_set(myBitset, 2, true);
    bitset_set(myBitset, 4, true);

    for (size_t i = 0; i < bitset_size(myBitset); i++) {
        fmt_printf("Bit %zu is %s\n", i, bitset_test(myBitset, i) ? "set" : "not set");
    }

    fmt_printf("Number of set bits: %zu\n", bitset_count(myBitset));
    
    bitset_deallocate(myBitset);
    return 0;
}
```
**Result in C:**
```
Bit 0 is not set
Bit 1 is not set
Bit 2 is set
Bit 3 is not set
Bit 4 is set
Bit 5 is not set
Bit 6 is not set
Bit 7 is not set
Number of set bits: 2
```

**C++ Implementation:**

```cpp
#include <iostream>
#include <bitset>

int main() {
    std::bitset<8> myBitset;

    // Set a few bits
    myBitset.set(2);
    myBitset.set(4);

    for (size_t i = 0; i < myBitset.size(); i++) {
        std::cout << "Bit " << i << " is " << (myBitset.test(i) ? "set" : "not set") << std::endl;
    }
    std::cout << "Number of set bits: " << myBitset.count() << std::endl;
    
    return 0;
}
```
**Result in C++:**
```
Bit 0 is not set
Bit 1 is not set
Bit 2 is set
Bit 3 is not set
Bit 4 is set
Bit 5 is not set
Bit 6 is not set
Bit 7 is not set
Number of set bits: 2
```

---

## Example 10: Converts Bitset Object to String (char*)

**C Implementation:**

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset *bit = bitset_create(8);

    bitset_set(bit, 1, true);
    bitset_set(bit, 3, true);

    char *bitsetString = bitset_to_string(bit);
    fmt_printf("Bitset in string is %s\n", bitsetString);

    bitset_deallocate(bit);
    return 0;
}
```
**Result:**
```
Bitset in string is 00001010
```

**C++ Implementation:**

```cpp
#include <iostream>
#include <bitset>
#include <string>

int main() {
    std::bitset<8> myBitset;

    myBitset.set(1);
    myBitset.set(3);

    std::string bitsetStr = myBitset.to_string();
    std::cout << "Bitset String: " << bitsetStr << std::endl;

    return 0;
}
```
**Result in C++:**
```
Bitset String: 00001010
```

---

## Example 11: Bitwise Operations on Two Bitsets

**C Implementation:**

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

// Function to perform bitwise AND on two bitsets and print the result
void bitwise_and_print(Bitset* bs1, Bitset* bs2) {
    if (!bs1 || !bs2 || bs1->size != bs2->size) {
        fprintf(stderr, "Bitsets are not compatible for operation.\n");
        return;
    }

    Bitset* result = bitset_create(bs1->size);
    for (size_t i = 0; i < bs1->size; ++i) {
        bool bit1 = bitset_test(bs1, i);
        bool bit2 = bitset_test(bs2, i);

        bitset_set(result, i, bit1 && bit2);
    }

    fmt_printf("Bitwise AND: ");
    bitset_print(result);
    bitset_deallocate(result);
}

int main() {
    Bitset* bs1 = bitset_create(8);
    bitset_set_from_string(bs1, "10101010");

    Bitset* bs2 = bitset_create(8);
    bitset_set_from_string(bs2, "11001100");

    fmt_printf("Bitset 1: ");
    bitset_print(bs1);

    fmt_printf("Bitset 2: ");
    bitset_print(bs2);

    bitwise_and_print(bs1, bs2);

    bitset_deallocate(bs1);
    bitset_deallocate(bs2);

    return 0;
}
```
**Result:**
```
Bitset 1: 10101010
Bitset 2: 11001100
Bitwise AND: 10001000
```

**C++ Implementation:**

```cpp
#include <iostream>
#include <bitset>

int main() {
    std::bitset<8> bs1("10101010");
    std::bitset<8> bs2("11001100");

    std::cout << "Bitset 1: " << bs1 << std::endl;
    std::cout << "Bitset 2: " << bs2 << std::endl;

    std::bitset<8> result = bs1 & bs2;
    std::cout << "Bitwise AND: " << result << std::endl;

    return 0;
}
```
**Result in C++:**
```
Bitset 1: 10101010
Bitset 2: 11001100
Bitwise AND: 10001000
```

---

## Example 13: Shifting Bits in a Bitset

**C Implementation:**

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

void shift_left_and_print(Bitset* bs, size_t shift) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Bitset object is null and invalid in shift_left_and_print.\n");
        return;
    }

    Bitset* shifted = bitset_create(bs->size);
    for (size_t i = 0; i < bs->size - shift; ++i) {
        if (bitset_test(bs, i)) {
            bitset_set(shifted, i + shift, true);
        }
    }

    fmt_printf("Shifted Left by %zu: ", shift);
    bitset_print(shifted);

    bitset_deallocate(shifted);
}

int main() {
    Bitset* bs = bitset_create(8);
    bitset_set_from_string(bs, "10011001");

    fmt_printf("Original Bitset: ");
    bitset_print(bs);

    shift_left_and_print(bs, 2);

    bitset_deallocate(bs);
    return 0;
}
```
**Result in C:**
```
Original Bitset: 10011001
Shifted Left by 2: 01100100
```

**C++ Implementation:**

```cpp
#include <iostream>
#include <bitset>

int main() {
    std::bitset<8> bs("10011001");
    std::cout << "Original Bitset: " << bs << std::endl;

    bs <<= 2; // Shift left by 2 positions
    std::cout << "Shifted Left by 2: " << bs << std::endl;

    return 0;
}
```
**Result in C++:**
```
Original Bitset: 10011001
Shifted Left by 2: 01100100
```

---

## Example 14 : how to use `and operator` in Bitset with `bitset_and`

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* b1 = bitset_create(4);
    Bitset* b2 = bitset_create(4);

    bitset_set_from_string(b1, "0110");
    bitset_set_from_string(b2, "0011");

    Bitset* result = bitset_and(b1, b2);
    fmt_printf("b1 & b2: ");
    bitset_print(result);

    bitset_deallocate(result); 
    bitset_deallocate(b1);
    bitset_deallocate(b2);

    return 0;
}
```

**Result**
```
b1 & b2: 0010
```

**C++ Implementation:**

```c++
#include <iostream>
#include <bitset>

int main() {
    std::bitset<4> b1("0110");
    std::bitset<4> b2("0011");

    std::cout << "b1 & b2: " << (b1 & b2) << '\n';

    return 0;
}

```

**Result in C++:**
```
b1 & b2: 0010
```

---

## Example 15 : how to use `or operator` in Bitset with `bitset_or`

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* b1 = bitset_create(4);
    Bitset* b2 = bitset_create(4);

    bitset_set_from_string(b1, "0110");
    bitset_set_from_string(b2, "0011");

    Bitset* result = bitset_or(b1, b2);

    fmt_printf("b1 | b2: ");
    bitset_print(result);

    bitset_deallocate(result); 
    bitset_deallocate(b1);
    bitset_deallocate(b2);

    return 0;
}
```
**Result C**
```
b1 | b2: 0111
```

**C++ Implementation:**

```c++ 
#include <bitset>
#include <iostream>
 
int main() {
    std::bitset<4> b1("0110");
    std::bitset<4> b2("0011");
 
    std::cout << "b1 | b2: " << (b1 | b2) << '\n';

    return 0;
}
```

**Result C++**
```
b1 | b2: 0111
```

---

## Example 16 : how to use `xor operator` in Bitset with `bitset_xor`

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* b1 = bitset_create(4);
    Bitset* b2 = bitset_create(4);

    bitset_set_from_string(b1, "0110");
    bitset_set_from_string(b2, "0011");

    Bitset* result = bitset_xor(b1, b2);
    fmt_printf("b1 ^ b2: ");
    bitset_print(result);

    bitset_deallocate(result); 
    bitset_deallocate(b1);
    bitset_deallocate(b2);

    return 0;
}
```

**Result C**
```
b1 ^ b2: 0101
```

**C++ Implementation:**

```c++ 
#include <bitset>
#include <iostream>
 
int main() {
    std::bitset<4> b1("0110");
    std::bitset<4> b2("0011");
 
    std::cout << "b1 ^ b2: " << (b1 | b2) << '\n';

    return 0;
}
```

**Result C++**
```
b1 ^ b2: 0101
```

---

## Example 17 : not operator in bitset `bitset_not`

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* b1 = bitset_create(16);

    bitset_set_from_string(b1, "10011001");
    Bitset* result = bitset_not(b1);

    fmt_printf("Original bitset: ");
    bitset_print(b1);

    fmt_printf("~Original bitset: ");
    bitset_print(result);

    bitset_deallocate(result); 
    bitset_deallocate(b1);
    return 0;
}
```

**Result c_std**
```
Original bitset: 0000000010011001
~Original bitset: 1111111101100110
```

**C++ Implementation**

```c++
#include <iostream>
#include <bitset>

int main() {
    std::bitset<16> pattern{"10011001"};
    
    std::cout << "Original bitset : " << pattern << std::endl;
    std::cout << "~Original bitset : " << ~pattern << std::endl;

    return 0;
}
```

**Result C++**
```
Original bitset: 0000000010011001
~Original bitset: 1111111101100110
```

---

## Example 18 : `shift_to_left` and `shift_to_right`

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* bs = bitset_create(8);
    bitset_set_from_string(bs, "11010010");

    fmt_printf("Original Bitset: ");
    bitset_print(bs);

    Bitset* left_shifted = bitset_shift_left(bs, 2);
    fmt_printf("After left_shift by 2: ");
    bitset_print(left_shifted);

    Bitset* right_shift = bitset_shift_right(bs, 3);
    fmt_printf("After right_shift by 3: ");
    bitset_print(right_shift);

    bitset_deallocate(bs);
    bitset_deallocate(right_shift);
    bitset_deallocate(left_shifted);
    
    return 0;
}
```

**Result c_std:**
```
Original Bitset: 11010010
After left_shift by 2: 01001000
After right_shift by 3: 00011010
```

**C++ Implementation:**

```c++
int main() {
    std::bitset<8> bs("11010010");
    std::cout << "Original Bitset: " << bs << std::endl;

    std::bitset<8> left_shifted = bs << 2;
    std::cout << "After left shift by 2: " << left_shifted << std::endl;

    std::bitset<8> right_shifted = bs >> 3;
    std::cout << "After right shift by 3: " << right_shifted << std::endl;

    return 0;
}
```

**C++ Result**
```
Original Bitset: 11010010
After left_shift by 2: 01001000
After right_shift by 3: 00011010
```

---

## Example 19 : check Bitset object are equal or not `bitset_is_equal` and `bitset_is_not_equal`

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* bs1 = bitset_create(8);
    Bitset* bs2 = bitset_create(8);

    bitset_set_from_string(bs1, "11010010");
    bitset_set_from_string(bs2, "11010010");

    if (bitset_is_equal(bs1, bs2)) {
        fmt_printf("bs1 and bs2 are equal.\n");
    } 
    else {
        fmt_printf("bs1 and bs2 are not equal.\n");
    }

    bitset_set_from_string(bs2, "01010101");

    if (bitset_is_not_equal(bs1, bs2)) {
        fmt_printf("After modification, bs1 and bs2 are not equal.\n");
    }

    bitset_deallocate(bs1);
    bitset_deallocate(bs2);

    return 0;
}
```

**Result c_std:**

```
bs1 and bs2 are equal.
After modification, bs1 and bs2 are not equal.
```

**C++ Implementation**

```c++
#include <bitset>
#include <iostream>

int main() {
    std::bitset<8> bs1("11010010");
    std::bitset<8> bs2("11010010");

    if (bs1 == bs2) {
        std::cout << "bs1 and bs2 are equal." << std::endl;
    } 
    else {
        std::cout << "bs1 and bs2 are not equal." << std::endl;
    }

    bs2 = std::bitset<8>("01010101");

    if (bs1 != bs2) {
        std::cout << "After modification, bs1 and bs2 are not equal." << std::endl;
    }

    return 0;
}
```

**Result C++:**

```
bs1 and bs2 are equal.
After modification, bs1 and bs2 are not equal.
```

---

## Example 20 : access bit positions with `bitset_at` or modify value of positions `bitset_at_ref`

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* bs = bitset_create(8);
    bitset_set_from_string(bs, "00101010");

    for (size_t i = 0; i < bitset_size(bs); ++i) {
        fmt_printf("bs[%zu]: %d\n", i, bitset_at(bs, i));
    }

    unsigned char* ref = bitset_at_ref(bs, 0);
    if (ref) {
        *ref |= 1 << 0; // Set the 0th bit to 1
    }

    fmt_printf("After setting bit 0, Bitset holds: ");
    bitset_print(bs);

    bitset_deallocate(bs);
    return 0;
}
```
**Result c_std:**

```
bs[0]: 0
bs[1]: 1
bs[2]: 0
bs[3]: 1
bs[4]: 0
bs[5]: 1
bs[6]: 0
bs[7]: 0
After setting bit 0, Bitset holds: 00101011
```

**C++ Implementation**

```c++
#include <bitset>
#include <cstddef>
#include <iostream>
 
int main() {
    std::bitset<8> b1{0b00101010};
 
    for (std::size_t i = 0; i < b1.size(); ++i)
        std::cout << "b1[" << i << "]: " << b1[i] << '\n';
    b1[0] = true; // modifies the first bit through bitset::reference
 
    std::cout << "After setting bit 0, b1 holds " << b1 << '\n';
}
```

**C++ Result**

```
b1[0]: 0
b1[1]: 1
b1[2]: 0
b1[3]: 1
b1[4]: 0
b1[5]: 1
b1[6]: 0
b1[7]: 0
After setting bit 0, b1 holds 00101011
```

---

## License

This project is open-source and available under [ISC License].