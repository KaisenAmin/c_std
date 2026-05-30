/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class Bitset
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in bitset.c (file-level convention).
 *
 * Dynamic-width bitset modeled on C++ std::bitset.
 */

#ifndef BITSET_H_
#define BITSET_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Sentinel returned by `bitset_test`-style functions when the
 * position is out of range. */
#define BITSET_OUT_OF_RANGE -1

/* #define BITSET_LOGGING_ENABLE */

#ifdef BITSET_LOGGING_ENABLE
    #define BITSET_LOG(fmt, ...) \
        do { fprintf(stderr, "[BITSET LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define BITSET_LOG(fmt, ...) do { } while (0)
#endif


typedef struct Bitset Bitset;

struct Bitset {
    unsigned char* bits;
    size_t         size;
};


/* ------------------------------------------------------------------ */
/* Construction / destruction                                         */
/* ------------------------------------------------------------------ */

Bitset*             bitset_create                 (size_t num_bits);
void                bitset_deallocate             (Bitset* bs);


/* ------------------------------------------------------------------ */
/* Bit mutation                                                       */
/* ------------------------------------------------------------------ */

Bitset*             bitset_set                    (Bitset* bs, size_t pos, bool value);
Bitset*             bitset_reset                  (Bitset* bs, size_t pos);
Bitset*             bitset_flip                   (Bitset* bs, size_t pos);
Bitset*             bitset_flip_all               (Bitset* bs);
void                bitset_set_from_string        (Bitset* bs, const char* bits);


/* ------------------------------------------------------------------ */
/* Element access                                                     */
/* ------------------------------------------------------------------ */

bool                bitset_test                   (const Bitset* bs, size_t pos);
bool                bitset_at                     (const Bitset* bs, size_t pos);
unsigned char*      bitset_at_ref                 (Bitset* bs, size_t pos);


/* ------------------------------------------------------------------ */
/* Predicates                                                         */
/* ------------------------------------------------------------------ */

bool                bitset_all                    (const Bitset* bs);
bool                bitset_any                    (const Bitset* bs);
bool                bitset_none                   (const Bitset* bs);


/* ------------------------------------------------------------------ */
/* Capacity / counting                                                */
/* ------------------------------------------------------------------ */

size_t              bitset_size                   (const Bitset* bs);
size_t              bitset_count                  (const Bitset* bs);


/* ------------------------------------------------------------------ */
/* Bitwise operations                                                 */
/* ------------------------------------------------------------------ */

Bitset*             bitset_and                    (const Bitset* bs1, const Bitset* bs2);
Bitset*             bitset_or                     (const Bitset* bs1, const Bitset* bs2);
Bitset*             bitset_xor                    (const Bitset* bs1, const Bitset* bs2);
Bitset*             bitset_not                    (const Bitset* bs);
Bitset*             bitset_shift_left             (const Bitset* bs, size_t shift);
Bitset*             bitset_shift_right            (const Bitset* bs, size_t shift);


/* ------------------------------------------------------------------ */
/* Comparison                                                         */
/* ------------------------------------------------------------------ */

bool                bitset_is_equal               (const Bitset* bs1, const Bitset* bs2);
bool                bitset_is_not_equal           (const Bitset* bs1, const Bitset* bs2);


/* ------------------------------------------------------------------ */
/* Conversions                                                        */
/* ------------------------------------------------------------------ */

char*               bitset_to_string              (const Bitset* bs);
unsigned long       bitset_to_ulong               (const Bitset* bs);
unsigned long long  bitset_to_ullong              (const Bitset* bs);


/* ------------------------------------------------------------------ */
/* Debug                                                              */
/* ------------------------------------------------------------------ */

void                bitset_print                  (const Bitset* bs);


#ifdef __cplusplus
}
#endif

#endif 
