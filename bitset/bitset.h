#ifndef BITSET_H_
#define BITSET_H_

#include <stdbool.h>
#include <stddef.h>

#define BITSET_OUT_OF_RANGE -1

typedef struct Bitset Bitset;

struct Bitset 
{
    unsigned char* bits;
    size_t size;

    void (*deallocate)(Bitset* bs);
    void (*print)(const Bitset* bs);
    void (*set_from_string)(Bitset* bs, const char* bitsStr);

    // Bit manipulation functions
    bool (*test)(const Bitset* bs, size_t pos);
    Bitset* (*set)(Bitset* bs, size_t pos, bool value);
    Bitset* (*reset)(Bitset* bs, size_t pos);
    Bitset* (*flip)(Bitset* bs, size_t pos);
    Bitset* (*flip_all)(Bitset* bs);

    // Bitset state functions
    bool (*all)(const Bitset* bs);
    bool (*any)(const Bitset* bs);
    bool (*none)(const Bitset* bs);
    size_t (*count)(const Bitset* bs);
    size_t (*get_size)(const Bitset* bs);

    // Conversion functions
    unsigned long (*to_ulong)(const Bitset* bs);
    unsigned long long (*to_ullong)(const Bitset* bs);
};

// Constructor
Bitset* bitset_create(size_t num_bits);

#endif // BITSET_H_
