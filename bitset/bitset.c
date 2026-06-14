/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Bitset
*/

#include <stdlib.h>
#include <string.h>
#include "bitset.h"



static inline size_t bitset_num_bytes(size_t num_bits) {
    return (num_bits + 7) / 8;
}


static inline void bitset_mask_tail(Bitset* bs) {
    if (!bs || bs->size == 0) {
        return;
    }

    size_t tail_bits = bs->size % 8;
    if (tail_bits != 0) {
        size_t last = bitset_num_bytes(bs->size) - 1;
        bs->bits[last] &= (unsigned char)((1u << tail_bits) - 1u);
    }
}


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
        return NULL;
    }

    size_t num_bytes = bitset_num_bytes(num_bits);
    if (num_bytes == 0) {
        num_bytes = 1; // avoid malloc(0)
    }
    BITSET_LOG("[bitset_create]: Allocating %zu bytes for bit array.", num_bytes);

    bs->bits = (unsigned char*)malloc(num_bytes);
    if (!bs->bits) {
        BITSET_LOG("[bitset_create]: Error - Memory allocation failed for bit array.");
        free(bs);
        return NULL;
    }

    memset(bs->bits, 0, num_bytes);
    bs->size = num_bits;

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

    for (size_t i = bs->size; i-- > 0; ) {
        printf("%d", bitset_test(bs, i) ? 1 : 0);
    }

    printf("\n");
    BITSET_LOG("[bitset_print]: Bitset printed successfully.");
}


/**
 * @brief Sets the Bitset's bits from a string representation.
 *
 * Converts a string of '0's and '1's into corresponding bits in the Bitset. The leftmost
 * character of the string corresponds to the highest bit index (MSB), matching the
 * behavior of std::bitset's string constructor.
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
    BITSET_LOG("[bitset_set_from_string]: String length: %zu.", str_len);

    for (size_t bit_pos = 0; bit_pos < str_len; ++bit_pos) {
        if (bit_pos >= bs->size) {
            BITSET_LOG("[bitset_set_from_string]: Warning - String length exceeds bitset size.");
            break;
        }
        bool bit_value = (str[str_len - 1 - bit_pos] == '1');
        size_t byte_index = bit_pos / 8;
        size_t bit_index = bit_pos % 8;

        if (bit_value) {
            bs->bits[byte_index] |= (unsigned char)(1u << bit_index);
        }
        else {
            bs->bits[byte_index] &= (unsigned char)~(1u << bit_index);
        }
        BITSET_LOG("[bitset_set_from_string]: Bit set at position %zu: %d.", bit_pos, bit_value);
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

    size_t byte_index = pos / 8;
    size_t bit_index = pos % 8;

    BITSET_LOG("[bitset_test]: Testing bit at position %zu (byte index: %zu, bit index: %zu).", pos, byte_index, bit_index);
    return (bs->bits[byte_index] & (unsigned char)(1u << bit_index)) != 0;
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
        bs->bits[byte_index] |= (unsigned char)(1u << bit_index);
    }
    else {
        BITSET_LOG("[bitset_set]: Clearing bit at position %zu (byte index: %zu, bit index: %zu).", pos, byte_index, bit_index);
        bs->bits[byte_index] &= (unsigned char)~(1u << bit_index);
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
    bs->bits[byte_index] &= (unsigned char)~(1u << bit_index);

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

    size_t num_bytes = bitset_num_bytes(bs->size);
    BITSET_LOG("[bitset_flip_all]: Flipping all bits. Total bytes: %zu.", num_bytes);

    for (size_t i = 0; i < num_bytes; ++i) {
        bs->bits[i] = (unsigned char)~bs->bits[i];
    }

    bitset_mask_tail(bs);

    BITSET_LOG("[bitset_flip_all]: All bits flipped successfully.");
    return bs;
}


/**
 * @brief Flips a specific bit in the Bitset.
 *
 * Inverts the bit at the specified position in the Bitset, turning a 1 to 0 or a 0 to 1.
 * The position is the bit index (0 = LSB), consistent with bitset_set / bitset_test.
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

    size_t byte_index = pos / 8;
    size_t bit_index = pos % 8;

    BITSET_LOG("[bitset_flip]: Flipping bit at position %zu (byte index: %zu, bit index: %zu).", pos, byte_index, bit_index);
    bs->bits[byte_index] ^= (unsigned char)(1u << bit_index);

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
    if (bs->size == 0) {
        return true;
    }

    for (size_t i = 0; i < bs->size; ++i) {
        if (!(bs->bits[i / 8] & (unsigned char)(1u << (i % 8)))) {
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
        if (bs->bits[i / 8] & (unsigned char)(1u << (i % 8))) {
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

    return !bitset_any(bs);
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
        if (bs->bits[i / 8] & (unsigned char)(1u << (i % 8))) {
            ++count;
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
 * Bits beyond the width of unsigned long are ignored.
 *
 * @param bs Pointer to the Bitset.
 * @return The corresponding unsigned long value.
 */
unsigned long bitset_to_ulong(const Bitset* bs) {
    BITSET_LOG("[bitset_to_ulong]: Function start.");

    if (!bs) {
        BITSET_LOG("[bitset_to_ulong]: Error - Null pointer provided.");
        return 0;
    }

    unsigned long value = 0;
    size_t limit = bs->size < sizeof(unsigned long) * 8 ? bs->size : sizeof(unsigned long) * 8;

    for (size_t i = 0; i < limit; ++i) {
        if (bs->bits[i / 8] & (unsigned char)(1u << (i % 8))) {
            value |= (1UL << i);
        }
    }

    BITSET_LOG("[bitset_to_ulong]: Conversion completed. Final value: %lu", value);
    return value;
}


/**
 * @brief Converts the Bitset to an unsigned long long.
 *
 * Converts the Bitset's bits to an unsigned long long, with the least significant bit at position 0.
 * Bits beyond the width of unsigned long long are ignored.
 *
 * @param bs Pointer to the Bitset.
 * @return The corresponding unsigned long long value.
 */
unsigned long long bitset_to_ullong(const Bitset* bs) {
    BITSET_LOG("[bitset_to_ullong]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_to_ullong]: Error - Null pointer provided.");
        return 0;
    }

    unsigned long long value = 0;
    size_t limit = bs->size < sizeof(unsigned long long) * 8 ? bs->size : sizeof(unsigned long long) * 8;

    for (size_t i = 0; i < limit; ++i) {
        if (bs->bits[i / 8] & (unsigned char)(1u << (i % 8))) {
            value |= (1ULL << i);
        }
    }

    BITSET_LOG("[bitset_to_ullong]: Conversion completed. Final value: %llu", value);
    return value;
}


/**
 * @brief Converts the Bitset to a string representation.
 *
 * Creates a string representation of the Bitset, with '1' representing set bits and '0' representing clear bits.
 * The leftmost character of the result corresponds to the highest bit index (MSB).
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

    char* str = (char*)malloc(bs->size + 1);
    if (!str) {
        BITSET_LOG("[bitset_to_string]: Error - Memory allocation failed.");
        return NULL;
    }

    for (size_t i = 0; i < bs->size; ++i) {
        str[bs->size - 1 - i] = bitset_test(bs, i) ? '1' : '0';
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
 * @return Pointer to the resulting Bitset after the AND operation, or NULL if the inputs are NULL or sizes differ.
 */
Bitset* bitset_and(const Bitset* bs1, const Bitset* bs2) {
    BITSET_LOG("[bitset_and]: Function start.");
    if (!bs1 || !bs2) {
        BITSET_LOG("[bitset_and]: Error - Null pointer provided.");
        return NULL;
    }
    if (bs1->size != bs2->size) {
        BITSET_LOG("[bitset_and]: Error - Bitsets have different sizes.");
        return NULL;
    }

    Bitset* result = bitset_create(bs1->size);
    if (!result) {
        return NULL;
    }

    size_t num_bytes = bitset_num_bytes(bs1->size);
    for (size_t i = 0; i < num_bytes; ++i) {
        result->bits[i] = (unsigned char)(bs1->bits[i] & bs2->bits[i]);
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
 * @return Pointer to the resulting Bitset after the OR operation, or NULL if the inputs are NULL or sizes differ.
 */
Bitset* bitset_or(const Bitset* bs1, const Bitset* bs2) {
    BITSET_LOG("[bitset_or]: Function start.");
    if (!bs1 || !bs2) {
        BITSET_LOG("[bitset_or]: Error - Null pointer provided.");
        return NULL;
    }
    if (bs1->size != bs2->size) {
        BITSET_LOG("[bitset_or]: Error - Bitsets have different sizes.");
        return NULL;
    }

    Bitset* result = bitset_create(bs1->size);
    if (!result) {
        return NULL;
    }

    size_t num_bytes = bitset_num_bytes(bs1->size);
    for (size_t i = 0; i < num_bytes; ++i) {
        result->bits[i] = (unsigned char)(bs1->bits[i] | bs2->bits[i]);
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
 * @return Pointer to the resulting Bitset after the XOR operation, or NULL if the inputs are NULL or sizes differ.
 */
Bitset* bitset_xor(const Bitset* bs1, const Bitset* bs2) {
    BITSET_LOG("[bitset_xor]: Function start.");
    if (!bs1 || !bs2) {
        BITSET_LOG("[bitset_xor]: Error - Null pointer provided.");
        return NULL;
    }
    if (bs1->size != bs2->size) {
        BITSET_LOG("[bitset_xor]: Error - Bitsets have different sizes.");
        return NULL;
    }

    Bitset* result = bitset_create(bs1->size);
    if (!result) {
        return NULL;
    }

    size_t num_bytes = bitset_num_bytes(bs1->size);
    for (size_t i = 0; i < num_bytes; ++i) {
        result->bits[i] = (unsigned char)(bs1->bits[i] ^ bs2->bits[i]);
    }

    BITSET_LOG("[bitset_xor]: Bitwise XOR operation completed successfully.");
    return result;
}


/**
 * @brief Performs bitwise NOT operation on a Bitset.
 *
 * Creates a new Bitset that is the result of flipping all bits in the given Bitset.
 * Bits beyond the logical size are kept clear.
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
        return NULL;
    }

    size_t num_bytes = bitset_num_bytes(bs->size);
    for (size_t i = 0; i < num_bytes; ++i) {
        result->bits[i] = (unsigned char)~bs->bits[i];
    }

    bitset_mask_tail(result);

    BITSET_LOG("[bitset_not]: Bitwise NOT operation completed successfully.");
    return result;
}


/**
 * @brief Shifts the bits in the Bitset to the left by the specified number of positions.
 *
 * In MSB-on-left convention this moves bits toward higher indices. Bits shifted past
 * the high end are discarded; the low end is filled with zeros.
 *
 * @param bs The Bitset to be shifted.
 * @param shift The number of positions to shift the bits to the left.
 * @return Newly allocated Bitset containing the result, or NULL on error.
 */
Bitset* bitset_shift_left(const Bitset* bs, size_t shift) {
    BITSET_LOG("[bitset_shift_left]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_shift_left]: Error - Null pointer provided.");
        return NULL;
    }

    Bitset* result = bitset_create(bs->size);
    if (!result) {
        return NULL;
    }
    if (shift >= bs->size) {
        BITSET_LOG("[bitset_shift_left]: Shift >= size, result is all zeros.");
        return result;
    }

    for (size_t i = bs->size; i-- > 0; ) {
        if (i < shift) {
            break;
        }
        size_t src = i - shift;
        if (bs->bits[src / 8] & (unsigned char)(1u << (src % 8))) {
            result->bits[i / 8] |= (unsigned char)(1u << (i % 8));
        }
    }

    bitset_mask_tail(result);

    BITSET_LOG("[bitset_shift_left]: Bitset shifted left successfully.");
    return result;
}


/**
 * @brief Shifts the bits in the Bitset to the right by the specified number of positions.
 *
 * In MSB-on-left convention this moves bits toward lower indices. Bits shifted past
 * the low end are discarded; the high end is filled with zeros.
 *
 * @param bs The Bitset to be shifted.
 * @param shift The number of positions to shift the bits to the right.
 * @return Newly allocated Bitset containing the result, or NULL on error.
 */
Bitset* bitset_shift_right(const Bitset* bs, size_t shift) {
    BITSET_LOG("[bitset_shift_right]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_shift_right]: Error - Null pointer provided.");
        return NULL;
    }

    Bitset* result = bitset_create(bs->size);
    if (!result) {
        return NULL;
    }
    if (shift >= bs->size) {
        BITSET_LOG("[bitset_shift_right]: Shift >= size, result is all zeros.");
        return result;
    }

    for (size_t i = 0; i + shift < bs->size; ++i) {
        size_t src = i + shift;
        if (bs->bits[src / 8] & (unsigned char)(1u << (src % 8))) {
            result->bits[i / 8] |= (unsigned char)(1u << (i % 8));
        }
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

    if (bs1 == bs2) {
        return true;
    }
    if (!bs1 || !bs2) {
        BITSET_LOG("[bitset_is_equal]: One side NULL.");
        return false;
    }
    if (bs1->size != bs2->size) {
        BITSET_LOG("[bitset_is_equal]: Bitsets have different sizes.");
        return false;
    }

    size_t num_bytes = bitset_num_bytes(bs1->size);
    if (num_bytes == 0) {
        return true;
    }

    // Compare full bytes; for the final byte, mask off bits beyond size.
    size_t tail_bits = bs1->size % 8;
    size_t full_bytes = (tail_bits == 0) ? num_bytes : num_bytes - 1;

    for (size_t i = 0; i < full_bytes; ++i) {
        if (bs1->bits[i] != bs2->bits[i]) {
            return false;
        }
    }
    if (tail_bits != 0) {
        unsigned char mask = (unsigned char)((1u << tail_bits) - 1u);
        if ((unsigned char)(bs1->bits[num_bytes - 1] & mask) !=
            (unsigned char)(bs2->bits[num_bytes - 1] & mask)) {
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
    return bitset_test(bs, pos);
}


/**
 * @brief Returns a pointer to the underlying byte that contains the bit at the given position.
 *
 * The returned pointer points to a byte; the caller must use the mask `(1u << (pos % 8))`
 * to read or modify the target bit. Returns NULL on error or out-of-range.
 *
 * @param bs The Bitset to modify.
 * @param pos The position of the bit to modify.
 * @return Pointer to the byte that contains the bit at the given position, or NULL.
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
    BITSET_LOG("[bitset_at_ref]: Returning reference to byte at position %zu.", pos);
    
    return &bs->bits[byte_index];
}


/**
 * @brief Creates a deep copy of a Bitset.
 *
 * Allocates a new Bitset of the same size and copies every bit. The result is
 * fully independent — mutating it does not affect the source and vice versa.
 * Fills the gap that the library could produce new bitsets via the bitwise
 * operators but had no way to simply duplicate one.
 *
 * @param bs The Bitset to copy. May be NULL.
 * @return A newly-allocated copy (release with `bitset_deallocate`), or NULL if
 *         @p bs is NULL or allocation fails.
 */
Bitset* bitset_copy(const Bitset* bs) {
    BITSET_LOG("[bitset_copy]: Function start.");
    if (!bs) {
        BITSET_LOG("[bitset_copy]: Error - Null pointer provided.");
        return NULL;
    }

    Bitset* out = bitset_create(bs->size);
    if (!out) {
        BITSET_LOG("[bitset_copy]: Error - Allocation failed.");
        return NULL;
    }

    size_t num_bytes = bitset_num_bytes(bs->size);
    if (num_bytes > 0 && bs->bits && out->bits) {
        memcpy(out->bits, bs->bits, num_bytes);
    }

    BITSET_LOG("[bitset_copy]: Copied %zu-bit bitset.", bs->size);
    return out;
}


/**
 * @brief Returns the index of the first (lowest-indexed) set bit.
 *
 * Scans byte by byte (skipping all-zero bytes) so it is efficient even on
 * large, sparse bitsets. The companion `bitset_find_next` enables the standard
 * set-bit iteration idiom:
 * @code
 *   for (size_t i = bitset_find_first(bs); i < bitset_size(bs); i = bitset_find_next(bs, i)) {
 *       // bit i is set
 *   }
 * @endcode
 *
 * @param bs The Bitset to scan. May be NULL.
 * @return The index of the lowest set bit, or `bitset_size(bs)` if no bit is
 *         set (so the loop condition `i < bitset_size(bs)` terminates cleanly).
 *         Returns 0 for a NULL bitset.
 */
size_t bitset_find_first(const Bitset* bs) {
    BITSET_LOG("[bitset_find_first]: Function start.");
    if (!bs || !bs->bits) {
        return bs ? bs->size : 0;
    }

    size_t num_bytes = bitset_num_bytes(bs->size);
    for (size_t b = 0; b < num_bytes; ++b) {
        unsigned char byte = bs->bits[b];
        if (byte != 0) {
            for (size_t bit = 0; bit < 8; ++bit) {
                if (byte & (unsigned char)(1u << bit)) {
                    size_t pos = b * 8 + bit;
                    return (pos < bs->size) ? pos : bs->size;
                }
            }
        }
    }
    return bs->size;
}


/**
 * @brief Returns the index of the first set bit strictly after @p pos.
 *
 * Like `bitset_find_first`, but starts the search at @p pos + 1. Skips all-zero
 * bytes for efficiency. Designed to be called in a loop seeded by
 * `bitset_find_first` (see that function for the iteration idiom).
 *
 * @param bs  The Bitset to scan. May be NULL.
 * @param pos The position to search after (the bit at @p pos itself is skipped).
 * @return The index of the next set bit after @p pos, or `bitset_size(bs)` if
 *         there is none. Returns 0 for a NULL bitset.
 */
size_t bitset_find_next(const Bitset* bs, size_t pos) {
    BITSET_LOG("[bitset_find_next]: Function start (pos=%zu).", pos);
    if (!bs || !bs->bits) {
        return bs ? bs->size : 0;
    }
    if (pos >= bs->size) {
        return bs->size;          /* nothing past the end (also guards pos+1 overflow) */
    }

    size_t start = pos + 1;
    if (start >= bs->size) {
        return bs->size;
    }

    size_t num_bytes = bitset_num_bytes(bs->size);
    size_t first_byte = start / 8;
    size_t first_bit = start % 8;

    /* First (partial) byte: mask off the bits below `first_bit`. */
    unsigned char byte = (unsigned char)(bs->bits[first_byte] & (0xFFu << first_bit));
    for (size_t b = first_byte; b < num_bytes; ++b) {
        if (b != first_byte) {
            byte = bs->bits[b];
        }
        if (byte != 0) {
            for (size_t bit = 0; bit < 8; ++bit) {
                if (byte & (unsigned char)(1u << bit)) {
                    size_t p = b * 8 + bit;
                    return (p < bs->size) ? p : bs->size;
                }
            }
        }
    }
    return bs->size;
}
