#include "bitset/bitset.h"
#include <stdio.h>

void shift_left_and_print(Bitset* bs, size_t shift) {
    if (!bs) {
        return;
    }

    Bitset* shifted = bitset_create(bs->size);
    for (size_t i = 0; i < bs->size - shift; ++i) {
        if (bitset_test(bs, i)) {
            bitset_set(shifted, i + shift, true);
        }
    }

    printf("Shifted Left by %zu: ", shift);
    bitset_print(shifted);

    bitset_deallocate(shifted);
}

int main() {
    Bitset* bs = bitset_create(8);
    bitset_set_from_string(bs, "10011001");

    printf("Original Bitset: ");
    bitset_print(bs);

    shift_left_and_print(bs, 2);

    bitset_deallocate(bs);
    return 0;
}