
# CSV Library in C

**Author:** amin tahmasebi
**Release Date:** 2023
**License:** ISC License


## Overview

This library provides a simple and efficient way to read, manipulate, and write CSV (Comma-Separated Values) files . It offers functions for creating and destroying CSV rows and files, reading and writing CSV files, and performing various operations like appending rows, inserting columns, and exporting data in JSON format.

## Features

- **Dynamic Memory Management**: Automatically manages memory for CSV rows and files, resizing as needed.
- **File Operations**: Read from and write to CSV files with customizable delimiters.
- **Data Manipulation**: Append rows, insert columns, and access individual cells.
- **Utility Functions**: Includes functions for printing CSV files to the console, finding rows based on search terms, validating cell formats, and more.
- **JSON Export**: Ability to export CSV data to JSON format.

## Installation

To use this library, include the `csv.h` header in your C project and compile the `csv.c` source file along with your project files.

## Functions Descriptions 

Here is a brief two-line description of each function in the CSV library:

- `CsvRow* csv_row_create()`: Creates a new CSV row structure. Initializes the internal array for storing cell values.
- `CsvRow* csv_file_get_row(const CsvFile *file, size_t index)`: Retrieves a row from a CSV file at the specified index. Returns NULL if the index is invalid.
- `CsvRow* csv_row_read_next(FileReader *reader, char delimiter)`: Reads the next row from a file using the provided FileReader object and delimiter.
- `CsvRow* csv_file_get_header(const CsvFile *file)`: Retrieves the header row from a CSV file, typically the first row.
- `CsvRow** csv_file_find_rows(const CsvFile *file, const char* searchTerm)`: Searches for rows in the CSV file that contain the given search term.

- `CsvFile* csv_file_create(char delimiter)`: Creates a new CSV file structure and initializes its internal state with the given delimiter.

- `char* csv_row_get_cell(const CsvRow *row, size_t index)`: Retrieves the value of a cell from a CSV row at the specified index. Returns NULL if the index is out of bounds.
- `char* csv_export_to_json(const CsvFile *file)`: Exports the contents of a CSV file to a JSON format string.

- `void csv_row_destroy(CsvRow *row)`: Frees all memory associated with a CSV row, including the cells and the row structure itself.
- `void csv_row_append_cell(CsvRow *row, const char *value)`: Appends a new cell with the given string value to a CSV row, resizing the row if needed.
- `void csv_file_destroy(CsvFile *file)`: Destroys a CSV file structure and frees memory associated with all its rows and internal data.
- `void csv_file_read(CsvFile *file, const char *filename)`: Reads the contents of a CSV file from the specified filename into the CSV structure.
- `void csv_file_write(const CsvFile *file, const char *filename)`: Writes the contents of the CSV structure to a specified file, using the configured delimiter.
- `void csv_file_append_row(CsvFile *file, CsvRow *row)`: Appends a new row to the CSV file structure, expanding the internal array if necessary.
- `void csv_file_remove_row(CsvFile *file, size_t index)`: Removes a row from the CSV file at the specified index and shifts subsequent rows.
- `void csv_file_insert_column(CsvFile *file, size_t colIndex, const CsvRow *colData)`: Inserts a new column at the specified index with the provided data.
- `void csv_file_set_header(CsvFile *file, CsvRow *header)`: Sets the header row for a CSV file, replacing the existing header if one exists.
- `void csv_file_concatenate(CsvFile *file1, const CsvFile *file2)`: Concatenates two CSV files, appending the rows of one file to another.

- `int csv_row_get_cell_as_int(const CsvRow *row, size_t index)`: Converts the value of a specified cell in a row to an integer.
- `int csv_column_sum(const CsvFile *file, size_t columnIndex)`: Sums the values of a specific column in the CSV file, assuming they contain integers.

- `bool csv_validate_cell_format(const CsvRow *row, size_t index, const char *format)`: Validates whether a specific cell in a row matches a given format string.

---

## Example 1

Below is a simple example of how to use this library to read a CSV file and print its contents:

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    const char *filename = "./test.csv"; 
    char delimiter = ','; 
    CsvFile* myCSV = csv_file_create(delimiter);  // Create a new CsvFile instance

    csv_file_read(myCSV, filename); 

    fmt_printf("Contents of the CSV file:\n"); 
    csv_print(myCSV);

    csv_file_destroy(myCSV);
    return 0;
}
```

## Example 2: Appending Rows to CSV

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    CsvFile* csv = csv_file_create(',');
    CsvRow* newRow = csv_row_create();

    csv_row_append_cell(newRow, "New Cell 1");
    csv_row_append_cell(newRow, "New Cell 2");
    csv_row_append_cell(newRow, "سلول جدید 3");

    csv_file_append_row(csv, newRow); // Append the new row to the CSV file
    csv_file_write(csv, "sources/updated_file.csv");  

    csv_file_destroy(csv);
    return 0;
}
```

## Example 2: Removing Rows from CSV

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() 
{
    CsvFile* csv = csv_file_create(',');

    csv_file_read(csv, "sources/test.csv"); // Read an existing CSV file
    csv_file_remove_row(csv, 1); // Remove the second row (index 1)
    csv_file_write(csv, "sources/reduced_file.csv"); 

    // Clean up
    csv_file_destroy(csv);
    return 0;
}
```

## Example 3: Exporting CSV to JSON

```c
#include "csv/csv.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    CsvFile* csv = csv_file_create(',');

    if (csv) {
        csv_file_read(csv, "./test.csv");
        if (csv->size == 0) {
            fmt_fprintf(stderr, "Error: No data read from CSV file.\n");
            return -1;
        }
        else {
            char* json = csv_export_to_json(csv); // Export to JSON
            if (json) {
                fmt_printf("JSON Output:\n%s\n", json);
                free(json); 
            }
            else {
                fprintf(stderr, "Error: Failed to export CSV to JSON.\n");
            }
        }
        csv_file_destroy(csv);
    }
    else {
        fprintf(stderr, "Error: Failed to create CSV structure.\n");
    } 
    return 0;
}
```

## Example 4: Inserting Columns into CSV

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    CsvFile* csv = csv_file_create(',');
    csv_file_read(csv, "./test.csv"); // Read an existing CSV file

    // Create a column to be inserted
    CsvRow* newColumn = csv_row_create();
    csv_row_append_cell(newColumn, "Column Header");
    csv_row_append_cell(newColumn, "Data 1");
    csv_row_append_cell(newColumn, "Data 2");
    // ... add more cells as needed

    csv_file_insert_column(csv, 2, newColumn);  // Insert the column at index 2
    csv_file_write(csv, "file_with_new_column.csv"); // Write the updated CSV file to a new file

    // Clean up
    csv_row_destroy(newColumn);
    csv_file_destroy(csv);
    return 0;
}
```

## Example 5 : Concatenate two file 
```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    CsvFile* file1 = csv_file_create(',');
    CsvFile* file2 = csv_file_create(',');

    csv_file_read(file1, "sources/test_10.csv"); // Read the first CSV file
    csv_file_read(file2, "sources/test_100.csv"); // Read the second CSV file

    csv_file_concatenate(file1, file2); // Join file2 rows to file1
    csv_file_write(file1, "sources/merged_file.csv"); // Write the merged data to a new CSV file

    csv_file_destroy(file1);
    csv_file_destroy(file2);
    return 0;
}
```

## Example 6 : find value in rows 
```c
#include "csv/csv.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    CsvFile* csv = csv_file_create(',');
    csv_file_read(csv, "sources/test_10.csv"); // Read a CSV file

    const char* searchTerm = "Air"; // Define the search term
    CsvRow** foundRows = csv_file_find_rows(csv, searchTerm); // Search for rows

    // Print found rows
    for (size_t i = 0; foundRows[i] != NULL; i++){
        fmt_printf("Found Row %zu:\n", i);
        for (size_t j = 0; j < foundRows[i]->size; j++) {
            fmt_printf("%s ", foundRows[i]->cells[j]);
        }
        fmt_printf("\n");
    }

    csv_file_destroy(csv);
    return 0;
}
```

## Example 7 :  If the string from the cell can be converted to an integer and back to a string without changing its content, it's considered valid. 

```c
#include "csv/csv.h"
#include "fmt/fmt.h"
#include <stdlib.h> 
#include <string.h>

int main(){
    CsvFile* csv = csv_file_create(',');
    csv_file_read(csv, "sources/test_10.csv"); 

    // Iterate through each row in the CSV file
    for (size_t i = 0; i < csv->size; ++i) {
        CsvRow* row = csv_file_get_row(csv, i);
        if (row != NULL) {
            char* cell = csv_row_get_cell(row, 2); // Get the third cell (Quantity Ordered)
            int value = atoi(cell); // Attempt to convert the cell to an integer
            char *buffer = fmt_sprintf("%d", value);

            bool isValid = strcmp(buffer, cell) == 0;
            fmt_printf("Row %zu - Quantity Ordered '%s' is %s\n", i + 1, cell, isValid ? "valid" : "invalid");
            free(buffer);
        }
    }

    csv_file_destroy(csv);
    return 0;
}
```

## Example 8 : Just how header work 
```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    CsvFile* csv = csv_file_create(',');
    CsvRow* header = csv_row_create();
    
    csv_row_append_cell(header, "Column1");
    csv_row_append_cell(header, "Column2");
    csv_row_append_cell(header, "Column3");
    csv_file_set_header(csv, header);

    csv_file_read(csv, "sources/test_10.csv"); // Read an existing CSV file (excluding the header)
    
    CsvRow* retrievedHeader = csv_file_get_header(csv); // Get and print the header row
    fmt_printf("Header Row:\n");

    for (size_t i = 0; i < retrievedHeader->size; ++i) {
        fmt_printf("%s ", retrievedHeader->cells[i]);
    }
    fmt_printf("\n");

    csv_file_destroy(csv);
    return 0;
}
```

## Example 9 : Finding cell value and convert it to Integer 
```c

#include "csv/csv.h"
#include "fmt/fmt.h"

int main()
{
    CsvFile* csv = csv_file_create(',');
    csv_file_read(csv, "sources/test_10.csv"); // Read a CSV file

    CSVRow* row = csv_file_get_row(csv, 2); // Get the third row (index 2)
    if (row != NULL)
    {
        int value = csv_row_get_cell_as_int(row, 0);  // Convert the value of the first cell to an integer
        fmt_printf("Value of the first cell in third row as integer: %d\n", value);
    }

    csv_file_destroy(csv);

    return 0;
}

```

## Example 10 : Printgin CSV file to Console 
```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    CsvFile* csv = csv_file_create(',');

    csv_file_read(csv, "sources/test.csv");  // Read an existing CSV file
    csv_print(csv); // Print the contents of the CSV file to the console

    csv_file_destroy(csv); 
    return 0;
}
```

## Example 11 : Using String lib with Csv 

gcc -std=c11 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c .\string\string.c .\csv\csv.c

```c
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
```