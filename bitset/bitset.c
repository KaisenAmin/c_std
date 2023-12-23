#include "bitset.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


Bitset* bitset_create(size_t num_bits) 
{
    Bitset* bs = (Bitset*)malloc(sizeof(Bitset));
    if (!bs) 
        return NULL;
    
    // Calculate the number of bytes needed to store the bits
    size_t num_bytes = (num_bits + 7) / 8; // Each byte can store 8 bits
    bs->bits = (unsigned char*)malloc(num_bytes);  // Allocate memory for the bits

    if (!bs->bits)  
    {
        free(bs); // Handle memory allocation failure
        return NULL;
    }

    memset(bs->bits, 0, num_bytes); // Initialize the bit array to 0
    bs->size = num_bits; // Set the size

    // // Assign function pointers
    // bs->deallocate = bitset_deallocate_impl;
    // bs->test = bitset_test_impl;
    // bs->set = bitset_set_impl;
    // bs->reset = bitset_reset_impl;
    // bs->flip = bitset_flip_impl;
    // bs->all = bitset_all_impl;
    // bs->any = bitset_any_impl;
    // bs->none = bitset_none_impl;
    // bs->count = bitset_count_impl;
    // bs->get_size = bitset_size_impl;
    // bs->to_ulong = bitset_to_ulong_impl;
    // bs->to_ullong = bitset_to_ullong_impl;
    // bs->set_from_string = bitset_set_from_string_impl;
    // bs->flip_all = bitset_flip_all_impl;
    // bs->print = bitset_print_impl;

    return bs;
}

void bitset_deallocate(Bitset *bs) 
{
    if (bs) 
    {
        free(bs->bits); // Free the memory allocated for the bits
        free(bs); // Free the Bitset structure itself
    }
}

void bitset_print(const Bitset* bs)
{
    if (!bs)
        return;
    
    for (size_t i = 0; i < bitset_size(bs); i++)
        printf("%d", bitset_test(bs, i)? 1 : 0);
    printf("\n");
}


void bitset_set_from_string(Bitset* bs, const char* str) 
{
    if (bs == NULL || str == NULL) 
        return;

    size_t str_len = strlen(str);
    size_t bit_pos;

    for (bit_pos = 0; bit_pos < str_len; ++bit_pos) 
    {
        bool bit_value = (str[bit_pos] == '1');
        size_t pos = bs->size - str_len + bit_pos;

        if (pos < bs->size) 
        {
            size_t byte_index = pos / 8; // Calculate the byte position
            size_t bit_index = pos % 8;  // Calculate the bit position within that byte

            if (bit_value) 
                bs->bits[byte_index] |= (1 << bit_index);  // Set the bit
            else 
                bs->bits[byte_index] &= ~(1 << bit_index);  // Clear the bit
        }
    }
}

bool bitset_test(const Bitset *bs, size_t pos) 
{
    if (bs && pos < bs->size) 
    {
        size_t bit_index = pos % 8; // Calculate the byte position and the bit position within that byte
        size_t byte_index = pos / 8;

        return (bs->bits[byte_index] & (1 << bit_index)) != 0; // Use bitwise AND to test if the bit is set
    }
    else 
        return false;  // Return false if `bs` is NULL or `pos` is out of range
}

Bitset* bitset_set(Bitset* bs, size_t pos, bool value) 
{
    if (bs && pos < bs->size) 
    {
        size_t byte_index = pos / 8; // Calculate the byte position
        size_t bit_index = 7 - (pos % 8); // Calculate the bit position within that byte

        // printf("Setting bit at position %zu (byte %zu, bit %zu) to %d\n", pos, byte_index, bit_index, value);

        // // Print the bit array before setting
        // printf("Before setting: ");
        // for (size_t i = 0; i < bs->size; i++) {
        //     printf("%d", (bs->bits[i / 8] & (1 << (i % 8))) ? 1 : 0);
        // }
        // printf("\n");

        if (value) 
            bs->bits[byte_index] |= (1 << bit_index); // Set the bit
        else 
            bs->bits[byte_index] &= ~(1 << bit_index); // Clear the bit
        

        // Print the bit array after setting
        // printf("After setting: ");
        // for (size_t i = 0; i < bs->size; i++) {
        //     printf("%d", (bs->bits[i / 8] & (1 << (i % 8))) ? 1 : 0);
        // }
        // printf("\n");
    }
    return bs;
}

// Clears the bit at the specified position
Bitset* bitset_reset(Bitset* bs, size_t pos) 
{
    if (bs && pos < bs->size) 
    {
        size_t byte_index = pos / 8; // Calculate the byte position and the bit position within that byte
        size_t bit_index = pos % 8;

        bs->bits[byte_index] &= ~(1 << bit_index); // Clear the bit: Use bitwise AND with a mask that has the relevant bit cleared
    }
    return bs;
}

// Flips all bits in the bitset
Bitset* bitset_flip_all(Bitset* bs) 
{
    if (bs) 
    {
        size_t num_bytes = (bs->size + 7) / 8; // Calculate the number of bytes needed
        for (size_t i = 0; i < num_bytes; ++i) 
            bs->bits[i] = ~(bs->bits[i]); // Flip all bits in each byte
    }

    return bs;
}

Bitset* bitset_flip(Bitset* bs, size_t pos) 
{
    if (bs && pos < bs->size) 
    {
        size_t adjusted_pos = bs->size - 1 - pos;  // Adjust the position to count from the right
        size_t byte_index = adjusted_pos / 8; // Find the byte in which the bit is located
        size_t bit_index = adjusted_pos % 8;  // Find the bit's position within that byte

        bs->bits[byte_index] ^= (1 << bit_index); // Flip the bit using XOR
    }

    return bs;
}

// Checks if all bits in the bitset are set
bool bitset_all(const Bitset* bs) 
{
    if (bs) 
    {
        for (size_t i = 0; i < bs->size; ++i) 
        {
            if (!(bs->bits[i / 8] & (1 << (i % 8))))  // If any bit is not set, return false
                return false;
        }
        return true;
    }

    return false;
}

// Checks if any bit in the bitset is set
bool bitset_any(const Bitset* bs) 
{
    if (bs) 
    {
        for (size_t i = 0; i < bs->size; ++i) 
            if (bs->bits[i / 8] & (1 << (i % 8)))  // If any bit is set, return true
                return true;
    }

    return false;
}

// Checks if none of the bits in the bitset are set
bool bitset_none(const Bitset* bs) 
{
    if (bs) 
    {
        for (size_t i = 0; i < bs->size; ++i)
            if (bs->bits[i / 8] & (1 << (i % 8))) 
                return false;
        return true;
    }

    return false;
}

// Counts the number of set bits in the bitset
size_t bitset_count(const Bitset* bs) 
{
    size_t count = 0;
    if (bs) 
    {
        for (size_t i = 0; i < bs->size; ++i) 
        {
            if (bs->bits[i / 8] & (1 << (i % 8))) 
                ++count;
        }
    }

    return count;
}

// Returns the size of the bitset
size_t bitset_size(const Bitset* bs) 
{
    return bs ? bs->size : 0;
}

unsigned long bitset_to_ulong(const Bitset* bs) 
{
    unsigned long value = 0;
    if (bs) {
       
        for (size_t i = 0; i < bs->size; ++i)  // Iterate over each bit from LSB to MSB
        { 
            size_t actual_bit_index = bs->size - 1 - i;
            if (bs->bits[i / 8] & (1 << (i % 8))) 
                value |= (1UL << actual_bit_index); // Set the corresponding bit in the unsigned long value
        }
    }

    return value;
}

unsigned long long bitset_to_ullong(const Bitset* bs) 
{
    unsigned long long value = 0;
    if (bs) 
    {
        for (size_t i = 0; i < bs->size; ++i) // Iterate over each bit from LSB to MSB
        { 
            size_t actual_bit_index = bs->size - 1 - i;
            if (bs->bits[i / 8] & (1 << (i % 8))) 
                value |= (1ULL << actual_bit_index); // Set the corresponding bit in the unsigned long long value
        }
    }

    return value;
}
