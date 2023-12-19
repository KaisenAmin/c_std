#include "string/string.h"
#include <stdio.h>
#include <stdlib.h>

String*** create_2d_string_array(const size_t rows, const size_t cols) 
{
    String*** array = malloc(rows * sizeof(String**));

    for (size_t i = 0; i < rows; ++i) 
    {
        array[i] = malloc(cols * sizeof(String*));
        for (size_t j = 0; j < cols; ++j) 
            array[i][j] = string_create("");  // Initialize with empty strings
    }

    return array;
}

void deallocate_2d_string_array(String*** array, const size_t rows, const size_t cols) 
{
    for (size_t i = 0; i < rows; ++i) 
    {
        for (size_t j = 0; j < cols; ++j) 
            string_deallocate(array[i][j]);
        
        free(array[i]);
    }
    free(array);
}

int main() 
{
    const size_t rows = 2;
    const size_t cols = 3;
    String*** my2DString = create_2d_string_array(rows, cols);

    // Example usage
    string_assign(my2DString[0][0], "Hello");
    string_assign(my2DString[0][1], "World");
    string_assign(my2DString[0][2], "!");

    string_assign(my2DString[1][0], "Goodbye");
    string_assign(my2DString[1][1], "Cruel");
    string_assign(my2DString[1][2], "World");

    // Print the 2D array
    for (size_t i = 0; i < rows; ++i) 
    {
        for (size_t j = 0; j < cols; ++j) 
            printf("%s ", string_c_str(my2DString[i][j]));
        
        printf("\n");
    }

    // Clean up
    deallocate_2d_string_array(my2DString, rows, cols);

    return 0;
}