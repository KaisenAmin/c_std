
# Bitset Library

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


## Example 1 : 'none', 'count'

Please, enter a binary number: 11010111
0000000011010111 has 6 bits set.
```cpp

#include <iostream>       // std::cin, std::cout
#include <bitset>         // std::bitset

int main ()
{
  std::bitset<16> foo;

  std::cout << "Please, enter a binary number: ";
  std::cin >> foo;

  if (foo.none())
    std::cout << foo << " has no bits set.\n";
  else
    std::cout << foo << " has " << foo.count() << " bits set.\n";

  return 0;
}
```

```c
#include "bitset/bitset.h" // Include your bitset header file
#include <stdio.h>
#include <string.h>

#define MAX_INPUT_SIZE 16 // Define the maximum input size

int main() 
{
    // Create a Bitset with 16 bits
    Bitset* foo = bitset_create(16);
    if (!foo) 
    {
        fprintf(stderr, "Failed to create bitset\n");
        return 1;
    }

    char input[MAX_INPUT_SIZE + 1]; // +1 for the null terminator
    
    printf("Please, enter a binary number: ");
    scanf("%16s", input); // Read up to 16 characters

    foo->set_from_string(foo, input);     // Set the Bitset from the string input

    if (foo->none(foo))  // Check if any bit is set and print the corresponding message
    {
        printf("Bitset: ");
        for (size_t i = 0; i < foo->size; ++i) 
            printf("%d", foo->test(foo, i) ? 1 : 0);
        
        printf(" has no bits set.\n");
    } 
    else 
    {
        printf("Bitset: ");
        for (size_t i = 0; i < foo->size; ++i) 
            printf("%d", foo->test(foo, i) ? 1 : 0);

        printf(" has %zu bits set.\n", foo->count(foo));
    }

    foo->deallocate(foo);

    return 0;
}

```


## Example 2 : 'size', 'flip'

```c

#include "bitset/bitset.h"
#include <stdio.h>
#include <string.h>

int main()
{
    Bitset* bi1 = bitset_create(4);
    Bitset* bi2 = bitset_create(8);

    if (!bi1 || !bi2)
        return -1;

    bi1->set_from_string(bi1, "0001");

    printf("%llu\n", bi2->to_ullong(bi2));
    printf("%zu\n%zu\n", bi1->get_size(bi1), bi2->get_size(bi2));

    bi1->flip(bi1, 2);

    for (size_t i = 0; i < bi1->get_size(bi1); i++)
        printf("%d", bi1->test(bi1, i));

    bi1->deallocate(bi1);
    bi2->deallocate(bi2);

    return 0;
}

```

## Example 3 : 'all', 'any', 'none'

Please, enter an 8-bit binary number: 11111111
all: true
any: true
none: false

```c
#include "bitset/bitset.h"
#include <stdio.h>
#include <string.h>

int main() 
{
    Bitset* foo = bitset_create(8);

    if (!foo)
    {
        fprintf(stderr, "Failed to create bitset\n");
        return 1;
    }

    char input[9]; // 8 bits + null terminator
    
    printf("Please, enter an 8-bit binary number: ");
    scanf("%8s", input);

    foo->set_from_string(foo, input);

    printf("all: %s\n", foo->all(foo) ? "true" : "false");
    printf("any: %s\n", foo->any(foo) ? "true" : "false");
    printf("none: %s\n", foo->none(foo) ? "true" : "false");

    // Deallocate the bitset
    foo->deallocate(foo);

    return 0;
}

```

```cpp
#include <iostream>       // std::cin, std::cout, std::boolalpha
#include <bitset>         // std::bitset

int main ()
{
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

## Example 4 : 'any', 'count'


Please, enter a binary number: 10110
0000000000010110 has 3 bits set.

```c
#include "bitset/bitset.h"
#include <stdio.h>
#include <string.h>

#define MAX_INPUT_SIZE 16

int main() 
{
    Bitset* foo = bitset_create(16);

    if (!foo) 
    {
        fprintf(stderr, "Failed to create bitset\n");
        return 1;
    }

    char input[MAX_INPUT_SIZE + 1]; // +1 for the null terminator

    printf("Please, enter a binary number: ");
    scanf("%16s", input);

    foo->set_from_string(foo, input);

    if (foo->any(foo)) 
    {
        printf("Bitset: ");
        for (size_t i = 0; i < foo->size; ++i) 
            printf("%d", foo->test(foo, i) ? 1 : 0);
    
        printf(" has %zu bits set.\n", foo->count(foo));
    } 
    else
    {
        printf("Bitset: ");
        for (size_t i = 0; i < foo->size; ++i) 
            printf("%d", foo->test(foo, i) ? 1 : 0);
    
        printf(" has no bits set.\n");
    }

    foo->deallocate(foo);

    return 0;
}

```

```cpp

#include <iostream>       // std::cin, std::cout
#include <bitset>         // std::bitset

int main ()
{
  std::bitset<16> foo;

  std::cout << "Please, enter a binary number: ";
  std::cin >> foo;

  if (foo.any())
    std::cout << foo << " has " << foo.count() << " bits set.\n";
  else
    std::cout << foo << " has no bits set.\n";

  return 0;
}

```

## Example 5 : Manually set bits 

```c
#include <stdio.h>
#include "bitset/bitset.h" 

int main() 
{
    // Create a Bitset with 8 bits
    Bitset* foo = bitset_create(8);

    if (!foo) 
    {
        fprintf(stderr, "Failed to create bitset\n");
        return 1;
    }

    // // Manually set the bits to match "10110011"
    foo->set(foo, 0, true);  // MSB
    foo->set(foo, 1, false);
    foo->set(foo, 2, true);
    foo->set(foo, 3, true);
    foo->set(foo, 4, false);
    foo->set(foo, 5, false);
    foo->set(foo, 6, true);
    foo->set(foo, 7, true);  // LSB

    // Print the bitset, count of ones, and count of zeros
    printf("Bitset: ");
    for (size_t i = 0; i < foo->size; ++i) 
        printf("%d", foo->test(foo, i) ? 1 : 0);
    
    printf(" has %zu ones and %zu zeros.\n", foo->count(foo), foo->size - foo->count(foo));

    foo->deallocate(foo);

    return 0;
}

```

## Example 6 : use 'print' in Bitset also flip 
Original bitset: 11010010
After toggling: 00101101 
```c
#include "bitset/bitset.h"
#include <stdio.h>
#include <string.h>

int main() 
{
    Bitset* myBitset = bitset_create(8);
    if (!myBitset) 
    {
        fprintf(stderr, "Failed to create bitset\n");
        return 1;
    }

    myBitset->set_from_string(myBitset, "11010010");

    printf("Original bitset: ");
    myBitset->print(myBitset);
    myBitset->flip_all(myBitset);

    printf("After toggling: ");
    myBitset->print(myBitset);

    myBitset->deallocate(myBitset);

    return 0;
}
```

```cpp
#include <iostream>
#include <bitset>

int main() 
{
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
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    Bitset* myBitset = bitset_create(8);
    if (!myBitset) 
    {
        fprintf(stderr, "Failed to create bitset\n");
        return 1;
    }

    int pos;

    printf("Enter a position to set (0-7): ");
    scanf("%d", &pos);

    if (pos >= 0 && pos < 8) 
    {
        myBitset->set(myBitset, pos, true);

        printf("Bitset after setting position %d: ", pos);
        myBitset->print(myBitset);
    }
    else 
        printf("Invalid position!\n");
    
    myBitset->deallocate(myBitset);

    return 0;
}

```

```cpp 

#include <iostream>
#include <bitset>

int main() 
{
    std::bitset<8> myBitset;
    int pos;

    std::cout << "Enter a position to set (0-7): ";
    std::cin >> pos;

    if (pos >= 0 && pos < 8) 
    {
        myBitset.set(pos);
        std::cout << "Bitset after setting position " << pos << ": " << myBitset << std::endl;
    } 
    else 
        std::cout << "Invalid position!" << std::endl;

    return 0;
}


```