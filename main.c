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

    bitset_set_from_string(foo, input);     // Set the Bitset from the string input

    if (bitset_none(foo))  // Check if any bit is set and print the corresponding message
    {
        printf("Bitset: ");
        for (size_t i = 0; i < foo->size; ++i) 
            printf("%d", bitset_test(foo, i) ? 1 : 0);
        
        printf(" has no bits set.\n");
    } 
    else 
    {
        printf("Bitset: ");
        for (size_t i = 0; i < foo->size; ++i) 
            printf("%d", bitset_test(foo, i) ? 1 : 0);

        printf(" has %zu bits set.\n", bitset_count(foo));
    }

    bitset_deallocate(foo);

    return 0;
}