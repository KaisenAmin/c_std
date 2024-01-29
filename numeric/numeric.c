#include <string.h>
#include "numeric.h"
#include <stdlib.h>


void numeric_iota(void* first, const void* last, size_t size, void* element, IncrementFunc func) {
    char* current = (char*)first;
    char* end = (char*)((const char*)last);

    for (;current != end; current += size) {
        memcpy(current, element, size);
        func(element);
    }
}

int numeric_gcd(int a, int b) {
    while (b != 0) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int numeric_lcm(int a, int b) {
    return (a / numeric_gcd(a, b)) * b;  
}

void* numeric_midpoint(void* a, void* b, NumericType type, size_t element_size) {
    switch (type) {
        case NUMERIC_INT64: {
            int64_t* result = malloc(sizeof(int64_t));
            if (!result) {
                return NULL;    
            }

            int64_t aa = *((int64_t*)a);
            int64_t bb = *((int64_t*)b);
            int64_t sum = aa + bb;
            *result = sum / 2 + (sum % 2) * (aa > bb ? 1 : 0);
            return result;
        }
        case NUMERIC_UINT64: {
            uint64_t* result = malloc(sizeof(uint64_t));
            if (!result) {
                return NULL;    
            }

            uint64_t aa = *((uint64_t*)a);
            uint64_t bb = *((uint64_t*)b);
            *result = aa / 2 + bb / 2 + ((aa & 1) && (bb & 1));
            return result;
        }
        case NUMERIC_INT16 : {
            int16_t* result = malloc(sizeof(int16_t));
            if (!result) {
                return NULL;    
            }

            int16_t aa = *((int16_t*)a);
            int16_t bb = *((int16_t*)b);
            int16_t sum = (int16_t)(aa + bb);
            *result = sum / 2 + (sum % 2) * (aa > bb ? 1 : 0);
            return result;
        }
        case NUMERIC_UINT16 : {
            uint16_t* result = malloc(sizeof(uint16_t));
            if (!result) {
                return NULL;    
            }

            uint16_t aa = *((uint16_t*)a);
            uint16_t bb = *((uint16_t*)b);
            *result = aa / 2 + bb / 2 + ((aa & 1) && (bb & 1));
            return result;            
        }
        case NUMERIC_INT32 : {
            int32_t* result = malloc(sizeof(int32_t));
            if (!result) {
                return NULL;    
            }

            int32_t aa = *((int32_t*)a);
            int32_t bb = *((int32_t*)b);

            // Adjusted rounding logic
            int32_t sum = aa + bb;
            *result = sum / 2 + (sum % 2) * (aa > bb ? 1 : 0);
            return result;
        }
        case NUMERIC_UINT32 : {
            uint32_t* result = malloc(sizeof(uint32_t));
            if (!result) {
                return NULL;    
            }

            uint32_t aa = *((uint32_t*)a);
            uint32_t bb = *((uint32_t*)b);
            *result = aa / 2 + bb / 2 + ((aa & 1) && (bb & 1));
            return result;
        }
        case NUMERIC_UINT8 : {
            uint8_t* result = malloc(sizeof(uint8_t));
            if (!result) {
                return NULL;    
            }

            uint8_t aa = *((uint8_t*)a);
            uint8_t bb = *((uint8_t*)b);
            *result = aa / 2 + bb / 2 + ((aa & 1) && (bb & 1));
            return result;
        }
        case NUMERIC_INT8 : {
            int8_t* result = malloc(sizeof(int8_t));
            if (!result) {
                return NULL;    
            }

            int8_t aa = *((int8_t*)a);
            int8_t bb = *((int8_t*)b);
            int16_t sum = (int16_t)aa + (int16_t)bb; // Cast to prevent overflow
            *result = sum / 2 + (sum % 2) * (aa > bb ? 1 : 0);
            return result;
        }
        case NUMERIC_FLOAT: {
            float* result = malloc(sizeof(float));
            if (!result) {
                return NULL;    
            }
            float aa = *((float*)a);
            float bb = *((float*)b);
            *result = (aa + bb) / 2.0;
            return result;
        }
        case NUMERIC_DOUBLE: {
            double* result = malloc(sizeof(double));
            if (!result) {
                return NULL;    
            }

            double aa = *((double*)a);
            double bb = *((double*)b);
            *result = (aa + bb) / 2.0;
            return result;
        }
        case NUMERIC_POINTER: {
            return (void*)((char*)a + ((char*)b - (char*)a) / (2 * (ptrdiff_t)element_size) * (ptrdiff_t)element_size);
        }
        default:
            return NULL;
    }
}