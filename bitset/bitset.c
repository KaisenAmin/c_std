/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Bitset
*/

#include "bitset.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Creates a new Bitset with the specified number of bits.
 *
 * Allocates memory for a Bitset structure and its internal bit array, initializing all bits to 0.
 *
 * @param num_bits The number of bits the bitset should contain.
 * @return Pointer to the newly created Bitset, or NULL if memory allocation fails.
 */
Bitset* bitset_create(size_t num_bits) {
    Bitset* bs = (Bitset*)malloc(sizeof(Bitset));
    if (!bs) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for Bitset structure in bitset_create.\n");
        exit(-1);
    }
    // Calculate the number of bytes needed to store the bits
    size_t num_bytes = (num_bits + 7) / 8; // Each byte can store 8 bits
    bs->bits = (unsigned char*)malloc(num_bytes);  // Allocate memory for the bits

    if (!bs->bits) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for bit array in bitset_create.\n");
        free(bs); // Handle memory allocation failure
        exit(-1);
    }

    memset(bs->bits, 0, num_bytes); // Initialize the bit array to 0
    bs->size = num_bits; // Set the size

    return bs;
}

/**
 * @brief Deallocates memory associated with a Bitset.
 *
 * Frees the memory allocated for the Bitset structure and its internal bit array.
 *
 * @param bs Pointer to the Bitset to be deallocated.
 */
void bitset_deallocate(Bitset *bs) {
    if (!bs) {
        fmt_fprintf(stderr, "Warning: Null pointer passed to bitset_deallocate. No action taken.\n");
        return;
    }
    free(bs->bits);
    free(bs);
}

/**
 * @brief Prints the Bitset to the standard output.
 *
 * Outputs the bits in the Bitset in a human-readable format, with the most significant bit on the left.
 *
 * @param bs Pointer to the Bitset to print.
 */
void bitset_print(const Bitset* bs) {
    if (!bs) {
        fmt_fprintf(stderr, "Warning: Null pointer passed to bitset_print. No action taken.\n");
        return;
    }

    for (int i = (int)bitset_size(bs) - 1; i >= 0; i--) {
        fmt_printf("%d", bitset_test(bs, i) ? 1 : 0);
    }
    fmt_printf("\n");
}

/**
 * @brief Sets the Bitset's bits from a string representation.
 *
 * Converts a string of '0's and '1's into corresponding bits in the Bitset, starting from the least significant bit.
 *
 * @param bs Pointer to the Bitset to modify.
 * @param str String of '0's and '1's representing the desired bit pattern.
 */
void bitset_set_from_string(Bitset* bs, const char* str) {
    if (!bs || !str) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_set_from_string.\n");
        return;
    }
    size_t str_len = strlen(str);
    size_t bit_pos;

    for (bit_pos = 0; bit_pos < str_len; ++bit_pos) {
        bool bit_value = (str[str_len - 1 - bit_pos] == '1'); // Start from the rightmost character
        size_t pos = bit_pos;

        if (pos < bs->size) {
            size_t byte_index = pos / 8; 
            size_t bit_index = pos % 8;

            if (bit_value) {
                bs->bits[byte_index] |= (1 << bit_index);
            }
            else {
                bs->bits[byte_index] &= ~(1 << bit_index);
            } 
        }
        else {
            fmt_fprintf(stderr, "Warning: String length exceeds bitset size in bitset_set_from_string.\n");
            break;
        }
    }
}

/**
 * @brief Tests whether a specific bit is set in the Bitset.
 *
 * Checks if the bit at the specified position in the Bitset is set to 1.
 *
 * @param bs Pointer to the Bitset.
 * @param pos The position of the bit to test.
 * @return true if the bit is set, false otherwise.
 */
bool bitset_test(const Bitset *bs, size_t pos) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_test function.\n");
        return false;
    }
    if (pos >= bs->size) {
        fmt_fprintf(stderr, "Error: Position out of range in bitset_test function.\n");
        return false;
    }

    size_t bit_index = pos % 8; // Calculate the byte position and the bit position within that byte
    size_t byte_index = pos / 8;

    return (bs->bits[byte_index] & (1 << bit_index)) != 0; // Use bitwise AND to test if the bit is set
}

/**
 * @brief Sets or clears a specific bit in the Bitset.
 *
 * Modifies the bit at the specified position in the Bitset, setting it to the specified value.
 *
 * @param bs Pointer to the Bitset.
 * @param pos The position of the bit to modify.
 * @param value The value to set the bit to (true for 1, false for 0).
 * 
 * @return Pointer to the modified Bitset.
 */
Bitset* bitset_set(Bitset* bs, size_t pos, bool value) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_set function.\n");
        return NULL;
    }
    if (pos >= bs->size) {
        fmt_fprintf(stderr, "Error: Position out of range in bitset_set function.\n");
        return bs;
    }
    size_t byte_index = pos / 8; // Calculate the byte position
    size_t bit_index = (pos % 8); // Calculate the bit position within that byte

    if (value) {
        bs->bits[byte_index] |= (1 << bit_index); // Set the bit
    }
    else { 
        bs->bits[byte_index] &= ~(1 << bit_index); // Clear the bit
    }
    return bs;
}

/**
 * @brief Clears the bit at the specified position in the Bitset.
 *
 * Sets the bit at the specified position in the Bitset to 0.
 *
 * @param bs Pointer to the Bitset.
 * @param pos The position of the bit to clear.
 * @return Pointer to the modified Bitset.
 */
Bitset* bitset_reset(Bitset* bs, size_t pos) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_reset function.\n");
        return NULL;
    }
    if (pos >= bs->size) {
        fmt_fprintf(stderr, "Error: Position out of range in bitset_reset function.\n");
        return bs;
    }
    size_t byte_index = pos / 8; // Calculate the byte position and the bit position within that byte
    size_t bit_index = pos % 8;
    bs->bits[byte_index] &= ~(1 << bit_index); // Clear the bit: Use bitwise AND with a mask that has the relevant bit cleared
    
    return bs;
}

/**
 * @brief Flips all bits in the Bitset.
 *
 * Inverts every bit in the Bitset, turning 1s to 0s and 0s to 1s.
 *
 * @param bs Pointer to the Bitset.
 * @return Pointer to the modified Bitset.
 */
Bitset* bitset_flip_all(Bitset* bs) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_flip_all function.\n");
        return NULL;
    }

    size_t num_bytes = (bs->size + 7) / 8;
    for (size_t i = 0; i < num_bytes; ++i) {
        bs->bits[i] = ~(bs->bits[i]);
    }
    return bs;
}

/**
 * @brief Flips a specific bit in the Bitset.
 *
 * Inverts the bit at the specified position in the Bitset, turning a 1 to 0 or a 0 to 1.
 *
 * @param bs Pointer to the Bitset.
 * @param pos The position of the bit to flip.
 * @return Pointer to the modified Bitset.
 */
Bitset* bitset_flip(Bitset* bs, size_t pos) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_flip function.\n");
        return NULL;
    }
    if (pos >= bs->size) {
        fmt_fprintf(stderr, "Error: Position out of range in bitset_flip function.\n");
        return bs;
    }

    size_t adjusted_pos = bs->size - 1 - pos;  // Adjust the position to count from the right
    size_t byte_index = adjusted_pos / 8; // Find the byte in which the bit is located
    size_t bit_index = adjusted_pos % 8;  // Find the bit's position within that byte
    bs->bits[byte_index] ^= (1 << bit_index); // Flip the bit using XOR
    
    return bs;
}

/**
 * @brief Checks if all bits in the Bitset are set.
 *
 * Returns true if all bits in the Bitset are set to 1.
 *
 * @param bs Pointer to the Bitset.
 * @return true if all bits are set, false otherwise.
 */
bool bitset_all(const Bitset* bs) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_all function.\n");
        return false;
    }

    for (size_t i = 0; i < bs->size; ++i) {
        if (!(bs->bits[i / 8] & (1 << (i % 8)))) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Checks if any bit in the Bitset is set.
 *
 * Returns true if at least one bit in the Bitset is set to 1.
 *
 * @param bs Pointer to the Bitset.
 * @return true if any bit is set, false otherwise.
 */
bool bitset_any(const Bitset* bs) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_any function.\n");
        return false; // false or an appropriate error indicator
    }

    for (size_t i = 0; i < bs->size; ++i) {
        if (bs->bits[i / 8] & (1 << (i % 8))) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Checks if none of the bits in the Bitset are set.
 *
 * Returns true if none of the bits in the Bitset are set to 1.
 *
 * @param bs Pointer to the Bitset.
 * @return true if no bits are set, false otherwise.
 */
bool bitset_none(const Bitset* bs) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_none function.\n");
        return true; // true or an appropriate error indicator
    }

    for (size_t i = 0; i < bs->size; ++i) {
        if (bs->bits[i / 8] & (1 << (i % 8))) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Counts the number of set bits in the Bitset.
 *
 * Returns the total number of bits in the Bitset that are set to 1.
 *
 * @param bs Pointer to the Bitset.
 * @return The number of set bits.
 */
size_t bitset_count(const Bitset* bs) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_count function.\n");
        return 0; // 0 or an appropriate error indicator
    }

    size_t count = 0;
    for (size_t i = 0; i < bs->size; ++i) {
        if (bs->bits[i / 8] & (1 << (i % 8))) {
            ++count;
        }
    }
    return count;
}


/**
 * @brief Returns the size of the Bitset.
 *
 * Gets the number of bits contained in the Bitset.
 *
 * @param bs Pointer to the Bitset.
 * @return The size of the Bitset (number of bits).
 */
size_t bitset_size(const Bitset* bs) {
    return bs ? bs->size : 0;
}

/**
 * @brief Converts the Bitset to an unsigned long.
 *
 * Converts the Bitset's bits to an unsigned long, with the least significant bit at position 0.
 *
 * @param bs Pointer to the Bitset.
 * @return The corresponding unsigned long value.
 */
unsigned long bitset_to_ulong(const Bitset* bs) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_to_ulong function.\n");
        return 0; // Returning 0 or an error code as appropriate for your use case
    }

    unsigned long value = 0;
    for (size_t i = 0; i < bs->size; ++i) {
        size_t byte_index = i / 8;
        size_t bit_index = i % 8;

        if (bs->bits[byte_index] & (1 << bit_index)) {
            value |= (1UL << i); // Set the corresponding bit in the unsigned long value
        }
    }
    return value;
}

/**
 * @brief Converts the Bitset to an unsigned long long.
 *
 * Converts the Bitset's bits to an unsigned long long, with the least significant bit at position 0.
 *
 * @param bs Pointer to the Bitset.
 * @return The corresponding unsigned long long value.
 */
unsigned long long bitset_to_ullong(const Bitset* bs) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_to_ullong function.\n");
        return 0; // Returning 0 or an error code as appropriate for your use case
    }

    unsigned long long value = 0;
    for (size_t i = 0; i < bs->size; ++i) {
        size_t bit_index = i % 8;
        size_t byte_index = i / 8;

        if (bs->bits[byte_index] & (1 << bit_index)) {
            value |= (1ULL << i); 
        }
    }
    return value;
}

/**
 * @brief Converts the Bitset to a string representation.
 *
 * Creates a string representation of the Bitset, with '1' representing set bits and '0' representing clear bits.
 *
 * @param bs Pointer to the Bitset.
 * @return A dynamically allocated string containing the Bitset's bits, or NULL if memory allocation fails.
 */
char* bitset_to_string(const Bitset* bs) {
    if (!bs) {
        fmt_fprintf(stderr, "Error: Null pointer provided to bitset_to_string function.\n");
        return NULL;
    }

    char* str = (char*)malloc(bs->size + 1); // +1 for the null terminator
    if (!str) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in bitset_to_string function.\n");
        return NULL;
    }

    for (size_t i = 0; i < bs->size; ++i) {
        str[bs->size - 1 - i] = bitset_test(bs, i) ? '1' : '0';
    }
    str[bs->size] = '\0'; 

    return str;
}
