/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class Bitset
*/

#include <stdlib.h>
#include <string.h>
#include "bitset.h"


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
        printf("%d", bitset_test(bs, i) ? 1 : 0);
    }

    printf("\n");
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

/**
 * @brief Performs bitwise AND operation on two Bitsets.
 *
 * Takes two Bitsets and returns a new Bitset that is the result of the AND operation.
 * The sizes of the Bitsets must be the same for the operation to be valid.
 *
 * @param bs1 The first Bitset.
 * @param bs2 The second Bitset.
 * @return Pointer to the resulting Bitset after the AND operation, or NULL if the sizes are not compatible.
 */
Bitset* bitset_and(const Bitset* bs1, const Bitset* bs2) {
    BITSET_LOG("[bitset_and]: Function start.");
    if (bs1->size != bs2->size) {
        BITSET_LOG("[bitset_and]: Error - Bitsets have different sizes.");
        return NULL;
    }

    Bitset* result = bitset_create(bs1->size);
    if (!result) {
        BITSET_LOG("[bitset_and]: Error - Memory allocation failed for result Bitset.");
        return NULL;
    }

    // Perform bitwise AND for each byte in the bit array
    size_t num_bytes = (bs1->size + 7) / 8; // Calculate the number of bytes
    for (size_t i = 0; i < num_bytes; ++i) {
        result->bits[i] = bs1->bits[i] & bs2->bits[i];
        BITSET_LOG("[bitset_and]: Byte %zu - Result: 0x%x", i, result->bits[i]);
    }

    BITSET_LOG("[bitset_and]: Bitwise AND operation completed successfully.");
    return result;
}

/**
 * @brief Performs bitwise OR operation on two Bitsets.
 *
 * Takes two Bitsets and returns a new Bitset that is the result of the OR operation.
 * The sizes of the Bitsets must be the same for the operation to be valid.
 *
 * @param bs1 The first Bitset.
 * @param bs2 The second Bitset.
 * @return Pointer to the resulting Bitset after the OR operation, or NULL if the sizes are not compatible.
 */
Bitset* bitset_or(const Bitset* bs1, const Bitset* bs2) {
    BITSET_LOG("[bitset_or]: Function start.");
    if (bs1->size != bs2->size) {
        BITSET_LOG("[bitset_or]: Error - Bitsets have different sizes.");
        return NULL;
    }

    Bitset* result = bitset_create(bs1->size);
    if (!result) {
        BITSET_LOG("[bitset_or]: Error - Memory allocation failed for result Bitset.");
        return NULL;
    }

    // Perform bitwise OR for each byte in the bit array
    size_t num_bytes = (bs1->size + 7) / 8; // Calculate the number of bytes
    for (size_t i = 0; i < num_bytes; ++i) {
        result->bits[i] = bs1->bits[i] | bs2->bits[i];
        BITSET_LOG("[bitset_or]: Byte %zu - Result: 0x%x", i, result->bits[i]);
    }

    BITSET_LOG("[bitset_or]: Bitwise OR operation completed successfully.");
    return result;
}

/**
 * @brief Performs bitwise XOR operation on two Bitsets.
 *
 * Takes two Bitsets and returns a new Bitset that is the result of the XOR operation.
 * The sizes of the Bitsets must be the same for the operation to be valid.
 *
 * @param bs1 The first Bitset.
 * @param bs2 The second Bitset.
 * @return Pointer to the resulting Bitset after the XOR operation, or NULL if the sizes are not compatible.
 */
Bitset* bitset_xor(const Bitset* bs1, const Bitset* bs2) {
    BITSET_LOG("[bitset_xor]: Function start.");
    if (bs1->size != bs2->size) {
        BITSET_LOG("[bitset_xor]: Error - Bitsets have different sizes.");
        return NULL;
    }

    Bitset* result = bitset_create(bs1->size);
    if (!result) {
        BITSET_LOG("[bitset_xor]: Error - Memory allocation failed for result Bitset.");
        return NULL;
    }

    // Perform bitwise XOR for each byte in the bit array
    size_t num_bytes = (bs1->size + 7) / 8; 
    for (size_t i = 0; i < num_bytes; ++i) {
        result->bits[i] = bs1->bits[i] ^ bs2->bits[i];
        BITSET_LOG("[bitset_xor]: Byte %zu - Result: 0x%x", i, result->bits[i]);
    }

    BITSET_LOG("[bitset_xor]: Bitwise XOR operation completed successfully.");
    return result;
}

/**
 * @brief Performs bitwise NOT operation on a Bitset.
 *
 * Creates a new Bitset that is the result of flipping all bits in the given Bitset.
 *
 * @param bs The Bitset to flip.
 * @return Pointer to the resulting Bitset after the NOT operation, or NULL if the input is NULL.
 */
Bitset* bitset_not(const Bitset* bs) {
    BITSET_LOG("[bitset_not]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_not]: Error - Null pointer provided.");
        return NULL;
    }

    Bitset* result = bitset_create(bs->size);
    if (!result) {
        BITSET_LOG("[bitset_not]: Error - Memory allocation failed for result Bitset.");
        return NULL;
    }

    size_t num_bytes = (bs->size + 7) / 8; 
    for (size_t i = 0; i < num_bytes; ++i) {
        result->bits[i] = ~bs->bits[i];
        BITSET_LOG("[bitset_not]: Byte %zu - Result: 0x%x", i, result->bits[i]);
    }

    // Clear any bits beyond the actual size of the bitset (if the size is not a multiple of 8)
    size_t extra_bits = (num_bytes * 8) - bs->size;
    if (extra_bits > 0) {
        result->bits[num_bytes - 1] &= (1 << (8 - extra_bits)) - 1;
        BITSET_LOG("[bitset_not]: Cleared extra bits beyond the size of the bitset.");
    }

    BITSET_LOG("[bitset_not]: Bitwise NOT operation completed successfully.");
    return result;
}

/**
 * @brief Shifts the bits in the Bitset to the left by the specified number of positions.
 *
 * This function shifts all bits to the left by the given number of positions, filling the rightmost bits with zeros.
 *
 * @param bs The Bitset to be shifted.
 * @param shift The number of positions to shift the bits to the left.
 * @return Pointer to the modified Bitset (bs).
 */
Bitset* bitset_shift_left(const Bitset* bs, size_t shift) {
    BITSET_LOG("[bitset_shift_left]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_shift_left]: Error - Null pointer provided.");
        return NULL;
    }
    if (shift >= bs->size) {
        // If the shift is larger than or equal to the size of the bitset, create a new bitset filled with zeros
        Bitset* result = bitset_create(bs->size);
        memset(result->bits, 0, (bs->size + 7) / 8);
        BITSET_LOG("[bitset_shift_left]: Shift greater than or equal to bitset size. Bitset filled with zeros.");
        return result;
    }

    Bitset* result = bitset_create(bs->size); 
    size_t num_bytes = (bs->size + 7) / 8; // Number of bytes in the bit array

    // Shift each byte in the bit array
    for (size_t i = 0; i < num_bytes; ++i) {
        result->bits[i] = (bs->bits[i] << shift) | (i + 1 < num_bytes ? (bs->bits[i + 1] >> (8 - shift)) : 0);
        BITSET_LOG("[bitset_shift_left]: Byte %zu shifted. New value: 0x%x", i, result->bits[i]);
    }

    // Clear any bits that may have been shifted out of range
    size_t extra_bits = bs->size % 8;
    if (extra_bits != 0) {
        result->bits[num_bytes - 1] &= (1 << extra_bits) - 1;
    }

    BITSET_LOG("[bitset_shift_left]: Bitset shifted left successfully.");
    return result;
}

/**
 * @brief Shifts the bits in the Bitset to the right by the specified number of positions.
 *
 * This function shifts all bits to the right by the given number of positions, filling the leftmost bits with zeros.
 *
 * @param bs The Bitset to be shifted.
 * @param shift The number of positions to shift the bits to the right.
 * @return Pointer to the modified Bitset (bs).
 */
Bitset* bitset_shift_right(const Bitset* bs, size_t shift) {
    BITSET_LOG("[bitset_shift_right]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_shift_right]: Error - Null pointer provided.");
        return NULL;
    }
    if (shift >= bs->size) {
        // If the shift is larger than or equal to the size of the bitset, create a new bitset filled with zeros
        Bitset* result = bitset_create(bs->size);
        memset(result->bits, 0, (bs->size + 7) / 8);
        BITSET_LOG("[bitset_shift_right]: Shift greater than or equal to bitset size. Bitset filled with zeros.");
        return result;
    }

    Bitset* result = bitset_create(bs->size); 
    size_t num_bytes = (bs->size + 7) / 8; // Number of bytes in the bit array

    for (size_t i = num_bytes; i-- > 0;) {
        result->bits[i] = (bs->bits[i] >> shift) | (i > 0 ? (bs->bits[i - 1] << (8 - shift)) : 0);
        BITSET_LOG("[bitset_shift_right]: Byte %zu shifted. New value: 0x%x", i, result->bits[i]);
    }

    BITSET_LOG("[bitset_shift_right]: Bitset shifted right successfully.");
    return result;
}

/**
 * @brief Compares two Bitsets for equality.
 *
 * This function checks if two Bitsets are identical in size and bit values. 
 * It returns true if the Bitsets are equal and false otherwise.
 *
 * @param bs1 The first Bitset to compare.
 * @param bs2 The second Bitset to compare.
 * @return true if the Bitsets are equal, false otherwise.
 */
bool bitset_is_equal(const Bitset* bs1, const Bitset* bs2) {
    BITSET_LOG("[bitset_is_equal]: Function start.");
    if (!bs1 || !bs2) {
        BITSET_LOG("[bitset_is_equal]: Error - Null pointer provided.");
        return false;
    }
    if (bs1->size != bs2->size) {
        BITSET_LOG("[bitset_is_equal]: Bitsets have different sizes.");
        return false;
    }

    size_t num_bytes = (bs1->size + 7) / 8;
    for (size_t i = 0; i < num_bytes; ++i) {
        if (bs1->bits[i] != bs2->bits[i]) {
            BITSET_LOG("[bitset_is_equal]: Bitsets differ at byte %zu.", i);
            return false;
        }
    }

    BITSET_LOG("[bitset_is_equal]: Bitsets are equal.");
    return true;
}

/**
 * @brief Compares two Bitsets for inequality.
 *
 * This function checks if two Bitsets are different in size or bit values. 
 * It returns true if the Bitsets are not equal and false otherwise.
 *
 * @param bs1 The first Bitset to compare.
 * @param bs2 The second Bitset to compare.
 * @return true if the Bitsets are not equal, false otherwise.
 */
bool bitset_is_not_equal(const Bitset* bs1, const Bitset* bs2) {
    BITSET_LOG("[bitset_is_not_equal]: Function start.");
    return !bitset_is_equal(bs1, bs2);
}

/**
 * @brief Returns the value of the bit at the given position.
 *
 * This function returns the value of the bit at the specified position.
 * It mimics the behavior of `std::bitset::operator[]` in C++.
 *
 * @param bs The Bitset to access.
 * @param pos The position of the bit to access.
 * @return The value of the bit at the specified position (true if set, false if not).
 */
bool bitset_at(const Bitset* bs, size_t pos) {
    BITSET_LOG("[bitset_at]: Function start.");

    if (!bs) {
        BITSET_LOG("[bitset_at]: Error - Null pointer provided.");
        return false;
    }
    if (pos >= bs->size) {
        BITSET_LOG("[bitset_at]: Error - Position out of bounds.");
        return false; 
    }

    size_t byte_index = pos / 8;
    size_t bit_index = pos % 8;

    bool result = (bs->bits[byte_index] & (1 << bit_index)) != 0;
    BITSET_LOG("[bitset_at]: Bit at position %zu is %d.", pos, result);

    return result;
}

/**
 * @brief Returns a reference (pointer) to the bit at the given position for modification.
 *
 * This function allows modification of the bit at the specified position.
 * The user can set the bit using the returned pointer (mimics the non-const version of `std::bitset::operator[]` in C++).
 *
 * @param bs The Bitset to modify.
 * @param pos The position of the bit to modify.
 * @return Pointer to the byte that contains the bit, shifted to the correct bit position.
 */
unsigned char* bitset_at_ref(Bitset* bs, size_t pos) {
    BITSET_LOG("[bitset_at_ref]: Function start.");

    if (!bs) {
        BITSET_LOG("[bitset_at_ref]: Error - Null pointer provided.");
        return NULL;
    }
    if (pos >= bs->size) {
        BITSET_LOG("[bitset_at_ref]: Error - Position out of bounds.");
        return NULL; 
    }

    size_t byte_index = pos / 8;

    BITSET_LOG("[bitset_at_ref]: Returning reference to bit at position %zu.", pos);
    return &bs->bits[byte_index];
}
