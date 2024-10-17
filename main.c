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
