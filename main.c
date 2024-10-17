#include "bitset/bitset.h"
#include "fmt/fmt.h"

int main() {
    Bitset* b1 = bitset_create(4);
    Bitset* b2 = bitset_create(4);

    bitset_set_from_string(b1, "0110");
    bitset_set_from_string(b2, "0011");

    Bitset* result = bitset_and(b1, b2);

    if (result) {
        fmt_printf("b1 & b2: ");
        bitset_print(result);
        bitset_deallocate(result); 
    } 
    else {
        fmt_fprintf(stderr, "Error: Bitsets are not compatible for AND operation.\n");
    }

    bitset_deallocate(b1);
    bitset_deallocate(b2);

    return 0;
}
