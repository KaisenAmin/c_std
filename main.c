#include "numeric/numeric.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Example with uint64_t
    uint64_t e = 10000000000ULL, f = 20000000000ULL;
    uint64_t* midpoint_uint64 = (uint64_t*) numeric_midpoint(&e, &f, NUMERIC_UINT64, 0);
    printf("Midpoint uint64_t: %llu\n", *midpoint_uint64);
    
    // Example with int32_t
    int32_t a = 8, b = 4;
    int32_t* midpoint_int32 = (int32_t*) numeric_midpoint(&a, &b, NUMERIC_INT32, sizeof(int32_t));
    printf("Midpoint int32_t: %d\n", *midpoint_int32);

    // Example with int16_t
    int16_t c = 3000, d = 6000;
    int16_t* midpoint_int16 = (int16_t*) numeric_midpoint(&c, &d, NUMERIC_INT16, sizeof(int16_t));
    printf("Midpoint int16_t: %d\n", *midpoint_int16);

    // Example with uint16_t
    uint16_t g = 500, h = 1000;
    uint16_t* midpoint_uint16 = (uint16_t*) numeric_midpoint(&g, &h, NUMERIC_UINT16, sizeof(uint16_t));
    printf("Midpoint uint16_t: %u\n", *midpoint_uint16);

    const char text[] = "0123456789";
    const char* p = text + 2; // Pointing to '2'
    const char* q = text + 5; // Pointing to '5'
    const char* midpoint_char = (const char*) numeric_midpoint((void*)p, (void*)q, NUMERIC_POINTER, sizeof(char));
    printf("Midpoint with pointers ('%c' and '%c'): '%c'\n", *p, *q, *midpoint_char);
    
    // Free allocated memory
    free(midpoint_uint64);
    free(midpoint_int32);
    free(midpoint_int16);
    free(midpoint_uint16);

    return 0;
}