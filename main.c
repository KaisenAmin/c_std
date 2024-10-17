#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* bs = bitset_create(8);
    bitset_set_from_string(bs, "00101010");

    for (size_t i = 0; i < bitset_size(bs); ++i) {
        fmt_printf("bs[%zu]: %d\n", i, bitset_at(bs, i));
    }

    unsigned char* ref = bitset_at_ref(bs, 0);
    if (ref) {
        *ref |= 1 << 0; // Set the 0th bit to 1
    }

    fmt_printf("After setting bit 0, Bitset holds: ");
    bitset_print(bs);

    bitset_deallocate(bs);
    return 0;
}
