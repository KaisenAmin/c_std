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
    BITSET_LOG("[bitset_create]: Function start.");

    Bitset* bs = (Bitset*)malloc(sizeof(Bitset));
    if (!bs) {
        BITSET_LOG("[bitset_create]: Error - Memory allocation failed for Bitset structure.");
        exit(-1);
    }

    size_t num_bytes = (num_bits + 7) / 8; // Each byte can store 8 bits
    BITSET_LOG("[bitset_create]: Allocating %zu bytes for bit array.", num_bytes);

    bs->bits = (unsigned char*)malloc(num_bytes);
    if (!bs->bits) {
        BITSET_LOG("[bitset_create]: Error - Memory allocation failed for bit array.");
        free(bs); 
        exit(-1);
    }

    memset(bs->bits, 0, num_bytes); // Initialize the bit array to 0
    bs->size = num_bits; // Set the size

    BITSET_LOG("[bitset_create]: Bitset created with size %zu bits.", num_bits);
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
    BITSET_LOG("[bitset_deallocate]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_deallocate]: Warning - Null pointer passed, no action taken.");
        return;
    }
    
    free(bs->bits);
    free(bs);
    BITSET_LOG("[bitset_deallocate]: Bitset deallocated successfully.");
}

/**
 * @brief Prints the Bitset to the standard output.
 *
 * Outputs the bits in the Bitset in a human-readable format, with the most significant bit on the left.
 *
 * @param bs Pointer to the Bitset to print.
 */
void bitset_print(const Bitset* bs) {
    BITSET_LOG("[bitset_print]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_print]: Warning - Null pointer passed, no action taken.");
        return;
    }

    for (int i = (int)bitset_size(bs) - 1; i >= 0; i--) {
        fmt_printf("%d", bitset_test(bs, i) ? 1 : 0);
    }

    fmt_printf("\n");
    BITSET_LOG("[bitset_print]: Bitset printed successfully.");
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
    BITSET_LOG("[bitset_set_from_string]: Function start.");
    if (!bs || !str) {
        BITSET_LOG("[bitset_set_from_string]: Error - Null pointer provided.");
        return;
    }

    size_t str_len = strlen(str);
    size_t bit_pos;

    BITSET_LOG("[bitset_set_from_string]: String length: %zu.", str_len);
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
            BITSET_LOG("[bitset_set_from_string]: Bit set at position %zu: %d.", pos, bit_value);
        } 
        else {
            BITSET_LOG("[bitset_set_from_string]: Warning - String length exceeds bitset size.");
            break;
        }
    }

    BITSET_LOG("[bitset_set_from_string]: Function end.");
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
    BITSET_LOG("[bitset_test]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_test]: Error - Null pointer provided.");
        return false;
    }
    if (pos >= bs->size) {
        BITSET_LOG("[bitset_test]: Error - Position out of range: %zu.", pos);
        return false;
    }

    size_t bit_index = pos % 8;
    size_t byte_index = pos / 8;

    BITSET_LOG("[bitset_test]: Testing bit at position %zu (byte index: %zu, bit index: %zu).", pos, byte_index, bit_index);
    return (bs->bits[byte_index] & (1 << bit_index)) != 0;
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
    BITSET_LOG("[bitset_set]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_set]: Error - Null pointer provided.");
        return NULL;
    }
    if (pos >= bs->size) {
        BITSET_LOG("[bitset_set]: Error - Position out of range: %zu.", pos);
        return bs;
    }

    size_t byte_index = pos / 8;
    size_t bit_index = pos % 8;

    if (value) {
        BITSET_LOG("[bitset_set]: Setting bit at position %zu (byte index: %zu, bit index: %zu).", pos, byte_index, bit_index);
        bs->bits[byte_index] |= (1 << bit_index);
    } 
    else {
        BITSET_LOG("[bitset_set]: Clearing bit at position %zu (byte index: %zu, bit index: %zu).", pos, byte_index, bit_index);
        bs->bits[byte_index] &= ~(1 << bit_index);
    }

    BITSET_LOG("[bitset_set]: Bit modified successfully.");
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
    BITSET_LOG("[bitset_reset]: Function start.");

    if (!bs) {
        BITSET_LOG("[bitset_reset]: Error - Null pointer provided.");
        return NULL;
    }
    if (pos >= bs->size) {
        BITSET_LOG("[bitset_reset]: Error - Position out of range: %zu.", pos);
        return bs;
    }

    size_t byte_index = pos / 8;
    size_t bit_index = pos % 8;

    BITSET_LOG("[bitset_reset]: Clearing bit at position %zu (byte index: %zu, bit index: %zu).", pos, byte_index, bit_index);
    bs->bits[byte_index] &= ~(1 << bit_index);

    BITSET_LOG("[bitset_reset]: Bit cleared successfully.");
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
    BITSET_LOG("[bitset_flip_all]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_flip_all]: Error - Null pointer provided.");
        return NULL;
    }

    size_t num_bytes = (bs->size + 7) / 8;
    BITSET_LOG("[bitset_flip_all]: Flipping all bits. Total bytes: %zu.", num_bytes);

    for (size_t i = 0; i < num_bytes; ++i) {
        BITSET_LOG("[bitset_flip_all]: Flipping byte %zu.", i);
        bs->bits[i] = ~(bs->bits[i]);
    }

    BITSET_LOG("[bitset_flip_all]: All bits flipped successfully.");
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
    BITSET_LOG("[bitset_flip]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_flip]: Error - Null pointer provided.");
        return NULL;
    }
    if (pos >= bs->size) {
        BITSET_LOG("[bitset_flip]: Error - Position out of range: %zu", pos);
        return bs;
    }

    size_t adjusted_pos = bs->size - 1 - pos;
    size_t byte_index = adjusted_pos / 8;
    size_t bit_index = adjusted_pos % 8;
    
    BITSET_LOG("[bitset_flip]: Flipping bit at position %zu (adjusted: %zu, byte index: %zu, bit index: %zu).", pos, adjusted_pos, byte_index, bit_index);
    bs->bits[byte_index] ^= (1 << bit_index);
    
    BITSET_LOG("[bitset_flip]: Bit flipped successfully.");
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
    BITSET_LOG("[bitset_all]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_all]: Error - Null pointer provided.");
        return false;
    }

    for (size_t i = 0; i < bs->size; ++i) {
        if (!(bs->bits[i / 8] & (1 << (i % 8)))) {
            BITSET_LOG("[bitset_all]: Bit %zu is not set.", i);
            return false;
        }
    }

    BITSET_LOG("[bitset_all]: All bits are set.");
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
    BITSET_LOG("[bitset_any]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_any]: Error - Null pointer provided.");
        return false;
    }

    for (size_t i = 0; i < bs->size; ++i) {
        if (bs->bits[i / 8] & (1 << (i % 8))) {
            BITSET_LOG("[bitset_any]: Bit %zu is set.", i);
            return true;
        }
    }

    BITSET_LOG("[bitset_any]: No bits are set.");
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
    BITSET_LOG("[bitset_none]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_none]: Error - Null pointer provided.");
        return true; 
    }

    for (size_t i = 0; i < bs->size; ++i) {
        if (bs->bits[i / 8] & (1 << (i % 8))) {
            BITSET_LOG("[bitset_none]: Bit %zu is set.", i);
            return false;
        }
    }

    BITSET_LOG("[bitset_none]: No bits are set.");
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
    BITSET_LOG("[bitset_count]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_count]: Error - Null pointer provided.");
        return 0; 
    }

    size_t count = 0;
    for (size_t i = 0; i < bs->size; ++i) {
        if (bs->bits[i / 8] & (1 << (i % 8))) {
            ++count;
            BITSET_LOG("[bitset_count]: Bit %zu is set. Current count: %zu", i, count);
        }
    }

    BITSET_LOG("[bitset_count]: Final count of set bits: %zu", count);
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
    BITSET_LOG("[bitset_size]: Function start.");

    if (!bs) {
        BITSET_LOG("[bitset_size]: Error - Null pointer provided.");
        return 0;
    }

    BITSET_LOG("[bitset_size]: Bitset size is %zu.", bs->size);
    return bs->size;
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
    BITSET_LOG("[bitset_to_ulong]: Function start.");

    if (!bs) {
        BITSET_LOG("[bitset_to_ulong]: Error - Null pointer provided.");
        return 0; // Returning 0 or an error code as appropriate for your use case
    }

    unsigned long value = 0;
    BITSET_LOG("[bitset_to_ulong]: Converting Bitset of size %zu to unsigned long.", bs->size);

    for (size_t i = 0; i < bs->size; ++i) {
        size_t byte_index = i / 8;
        size_t bit_index = i % 8;

        if (bs->bits[byte_index] & (1 << bit_index)) {
            value |= (1UL << i); // Set the corresponding bit in the unsigned long value
            BITSET_LOG("[bitset_to_ulong]: Bit %zu is set. Updated value: %lu", i, value);
        }
    }

    BITSET_LOG("[bitset_to_ulong]: Conversion completed. Final value: %lu", value);
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
    BITSET_LOG("[bitset_to_ullong]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_to_ullong]: Error - Null pointer provided.");
        return 0; // Returning 0 or an error code as appropriate for your use case
    }

    unsigned long long value = 0;
    BITSET_LOG("[bitset_to_ullong]: Converting Bitset of size %zu to unsigned long long.", bs->size);

    for (size_t i = 0; i < bs->size; ++i) {
        size_t byte_index = i / 8;
        size_t bit_index = i % 8;

        if (bs->bits[byte_index] & (1 << bit_index)) {
            value |= (1ULL << i);
            BITSET_LOG("[bitset_to_ullong]: Bit %zu is set. Updated value: %llu", i, value);
        }
    }

    BITSET_LOG("[bitset_to_ullong]: Conversion completed. Final value: %llu", value);
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
    BITSET_LOG("[bitset_to_string]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_to_string]: Error - Null pointer provided.");
        return NULL;
    }

    char* str = (char*)malloc(bs->size + 1); // +1 for the null terminator
    if (!str) {
        BITSET_LOG("[bitset_to_string]: Error - Memory allocation failed.");
        return NULL;
    }

    BITSET_LOG("[bitset_to_string]: Converting bitset of size %zu to string.", bs->size);
    for (size_t i = 0; i < bs->size; ++i) {
        str[bs->size - 1 - i] = bitset_test(bs, i) ? '1' : '0';
        BITSET_LOG("[bitset_to_string]: Bit at position %zu is %c.", i, str[bs->size - 1 - i]);
    }
    str[bs->size] = '\0'; 

    BITSET_LOG("[bitset_to_string]: Bitset successfully converted to string: %s", str);
    return str;
}

