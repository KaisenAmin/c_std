
# Bitset Library in C

**Author:** amin tahmasebi
**Release Date:** 2023
**License:** ISC License


The Bitset library is a part of a project to reimplement C++ standard library features in C. It provides a generic container that encapsulates dynamic size arrays, offering similar functionality to `std::bitset` in C++.

## Compilation

To compile the Array library along with your main program, use the following GCC command:
if you need other lib just you can add name of libs .c 

```bash
gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./bitset/bitset.c 
g++ -std=c++14 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.cpp

```

Ensure you have the GCC compiler installed on your system and that all source files are in the correct directory structure as shown in the project.

## Usage

To use the Array library in your project, include the `bitset.h` header file in your source code.

in these examples i rewrite cpp example in Bitset code 

```c
#include "array/array.h"
```
## Function Descriptions

- `bitset_create(size_t num_bits)`: Creates a new bitset with the specified number of bits.
- `bitset_deallocate(Bitset *bs)`: Frees the memory allocated for the bitset.
- `bitset_print(const Bitset* bs)`: Prints the entire bitset to the standard output.
- `bitset_set_from_string(Bitset* bs, const char* str)`: Sets the bitset's bits based on a string of '0's and '1's.
- `bitset_test(const Bitset *bs, size_t pos)`: Tests whether the bit at the specified position in the bitset is set (1) or not (0).
- `bitset_set(Bitset* bs, size_t pos, bool value)`: Sets or clears the bit at the specified position in the bitset.
- `bitset_reset(Bitset* bs, size_t pos)`: Clears (sets to 0) the bit at the specified position in the bitset.
- `bitset_flip(Bitset* bs, size_t pos)`: Toggles (flips) the bit at the specified position in the bitset.
- `bitset_flip_all(Bitset* bs)`: Toggles (flips) all bits in the bitset.
- `bitset_all(const Bitset* bs)`: Checks if all bits in the bitset are set (1).
- `bitset_any(const Bitset* bs)`: Checks if any bit in the bitset is set (1).
- `bitset_none(const Bitset* bs)`: Checks if no bits in the bitset are set (1).
- `bitset_count(const Bitset* bs)`: Returns the number of bits that are set (1) in the bitset.
- `bitset_size(const Bitset* bs)`: Returns the size (number of bits) of the bitset.
- `bitset_to_ulong(const Bitset* bs)`: Converts the bitset to an `unsigned long` value, assuming the bitset 
represents a binary number.

- `bitset_to_ullong(const Bitset* bs)`: Converts the bitset to an `unsigned long long` value, assuming the bitset represents a binary number.
- `bitset_to_string` : This function will convert the bitset to a string representation.

## Example 1 : how to use `bitset_none` and `bitset_count`

Please, enter a binary number: 11010111
0000000011010111 has 6 bits set.
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

```c
#include "bitset/bitset.h" // Include your bitset header file
#include "fmt/fmt.h"

#define MAX_INPUT_SIZE 16 // Define the maximum input size

int main() {
    // Create a Bitset with 16 bits
    Bitset* foo = bitset_create(16);
    if (!foo) {
        fprintf(stderr, "Failed to create bitset\n");
        return 1;
    }

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

## Example 2 : how to get size of bitset with `bitset_size` and flip it with `bitset_flip`
 
```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main(){
    Bitset* bi1 = bitset_create(4);
    Bitset* bi2 = bitset_create(8);

    if (!bi1 || !bi2) {
        return -1;
    }
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

## Example 3 : Using `bitset_all` and `bitset_none` and `bitset_any`

Please, enter an 8-bit binary number: 11111111
all: true
any: true
none: false

```c
#include "bitset/bitset.h"
#include "fmt/fmt.h"


int main() {
    Bitset* foo = bitset_create(8);

    if (!foo){
        fprintf(stderr, "Failed to create bitset\n");
        return 1;
    }

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

## Example 4 : Using `bitset_any` and `bitset_count`


Please, enter a binary number: 10110
0000000000010110 has 3 bits set.

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

## Example 5 : Manually set bits with `bitset_set`

```c
#include "fmt/fmt.h"
#include "bitset/bitset.h" 

int main() {
    // Create a Bitset with 8 bits
    Bitset* foo = bitset_create(8);

    if (!foo) {
        fprintf(stderr, "Failed to create bitset\n");
        return 1;
    }

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
    fmt_printf(" has %zu ones and %zu zeros.\n", bitset_count(foo), foo->size - bitset_count(foo));

    bitset_deallocate(foo);
    return 0;
}

```

## Example 6 : use `bitset_print` in Bitset also flip 
Original bitset: 11010010
After toggling: 00101101 
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

## Example 7 : set and print 

Enter a position to set (0-7): 2
Bitset after setting position 2: 00000100

Enter a position to set (0-7): 1
Bitset after setting position 1: 00000010

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

