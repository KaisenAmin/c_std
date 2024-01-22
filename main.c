#include "csv/csv.h"
#include "string/string.h"
#include "fmt/fmt.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Function to convert a string to uppercase using the String library
void to_uppercase(String *str) {
    char* data = (char*) malloc(string_length(str) * sizeof(char));
    if (!data) {
        perror("Can not allocate Memory");
        exit(-1);
    }

    for (size_t i = 0; i < string_length(str); ++i) {
        char ch = string_at(str, i); 
        data[i] = toupper(ch);
    }
    string_replace(str, string_c_str(str), data);
    free(data);
}

int main() {
    const char *input_filename = "sources/test_10.csv"; // Replace with your CSV file's path
    const char *output_filename = "sources/output.csv"; // Output CSV file's path
    char delimiter = ','; // Specify the delimiter used in your CSV file
    CsvFile* myCSV = csv_file_create(delimiter);

    csv_file_read(myCSV, input_filename);

    // Iterate through each row and cell
    for (size_t rowIndex = 0; rowIndex < myCSV->size; ++rowIndex)   {
        CsvRow* row = csv_file_get_row(myCSV, rowIndex);

        for (size_t cellIndex = 0; cellIndex < row->size; ++cellIndex) {
            char* cellValue = csv_row_get_cell(row, cellIndex);
            String* cellString = string_create(cellValue); // Create a String object from the cell value

            to_uppercase(cellString);   // Convert to uppercase and append text
            string_append(cellString, " MODIFIED");

            free(row->cells[cellIndex]); // Free the original cell value
            row->cells[cellIndex] = strdup(string_c_str(cellString));
            string_deallocate(cellString);  // Deallocate the String object
        }
    }

    csv_file_write(myCSV, output_filename); // Write the modified CSV to a new file
    csv_file_destroy(myCSV); 
    return 0;
}