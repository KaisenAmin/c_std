/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Numeric
*/

#ifndef NUMERIC_H_
#define NUMERIC_H_

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

typedef enum {
    NUMERIC_INT32,
    NUMERIC_UINT32,
    NUMERIC_UINT64,
    NUMERIC_INT64,
    NUMERIC_INT16,
    NUMERIC_UINT16,
    NUMERIC_UINT8,
    NUMERIC_INT8,
    NUMERIC_FLOAT,
    NUMERIC_DOUBLE,
    NUMERIC_POINTER
} NumericType;

typedef void (*IncrementFunc)(void*);

void* numeric_midpoint(void* a, void* b, NumericType type, size_t element_size);
void numeric_iota(void *first, const void* last, size_t size, void* element, IncrementFunc func);

int numeric_gcd(int a, int b);
int numeric_lcm(int a, int b);

#endif 