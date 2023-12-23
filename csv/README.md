
# CSV Library

## Overview

This library provides a simple and efficient way to read, manipulate, and write CSV (Comma-Separated Values) files in C. It offers functions for creating and destroying CSV rows and files, reading and writing CSV files, and performing various operations like appending rows, inserting columns, and exporting data in JSON format.

## Features

- **Dynamic Memory Management**: Automatically manages memory for CSV rows and files, resizing as needed.
- **File Operations**: Read from and write to CSV files with customizable delimiters.
- **Data Manipulation**: Append rows, insert columns, and access individual cells.
- **Utility Functions**: Includes functions for printing CSV files to the console, finding rows based on search terms, validating cell formats, and more.
- **JSON Export**: Ability to export CSV data to JSON format.

## Installation

To use this library, include the `csv.h` header in your C project and compile the `csv.c` source file along with your project files.

## Usage Example

Below is a simple example of how to use this library to read a CSV file and print its contents:

```c
#include "csv/csv.h"
#include <stdio.h>

int main() 
{
    const char *filename = "test.csv"; // Replace with your CSV file's path
    char delimiter = ','; // Specify the delimiter used in your CSV file

    // Create a new CSVFile instance
    CSVFile* myCSV = csv_file_create(delimiter);
    if (!myCSV) {
        fprintf(stderr, "Failed to create CSV file structure.\n");
        return 1;
    }

    // Read the CSV file
    csv_file_read(myCSV, filename);
    
    // Print the contents of the CSV file
    printf("Contents of the CSV file:\n");
    csv_print(myCSV);

    // Clean up and free memory
    csv_file_destroy(myCSV);

    return 0;
}
```

## Functions

- `CSVRow* csv_row_create()`: Create a new CSV row.
- `void csv_row_destroy(CSVRow *row)`: Destroy a CSV row and free memory.
- `void csv_row_append_cell(CSVRow *row, const char *value)`: Append a cell to a CSV row.
- `char* csv_row_get_cell(const CSVRow *row, size_t index)`: Get a cell value from a CSV row.
- `CSVFile* csv_file_create(char delimiter)`: Create a new CSV file structure.
- `void csv_file_destroy(CSVFile *file)`: Destroy a CSV file structure and free memory.
- `void csv_file_read(CSVFile *file, const char *filename)`: Read a CSV file.
- `void csv_file_write(const CSVFile *file, const char *filename)`: Write to a CSV file.
- `void csv_file_append_row(CSVFile *file, CSVRow *row)`: Append a row to a CSV file.
- `CSVRow* csv_file_get_row(const CSVFile *file, size_t index)`: Get a row from a CSV file.
- `void csv_file_remove_row(CSVFile *file, size_t index)`: Remove a row from a CSV file.
- Additional utility functions are provided for column insertion, JSON export, etc.



Sure, I can help you add these examples to your README for the CSV Library. Here's how it would look with the examples added under separate sections titled "Example 1", "Example 2", "Example 3", and so on.

---

# CSV Library

## Overview

This library provides a simple and efficient way to read, manipulate, and write CSV (Comma-Separated Values) files in C. It offers functions for creating and destroying CSV rows and files, reading and writing CSV files, and performing various operations like appending rows, inserting columns, and exporting data in JSON format.

## Features

- **Dynamic Memory Management**: Automatically manages memory for CSV rows and files, resizing as needed.
- **File Operations**: Read from and write to CSV files with customizable delimiters.
- **Data Manipulation**: Append rows, insert columns, and access individual cells.
- **Utility Functions**: Includes functions for printing CSV files to the console, finding rows based on search terms, validating cell formats, and more.
- **JSON Export**: Ability to export CSV data to JSON format.

## Installation

To use this library, include the `csv.h` header in your C project and compile the `csv.c` source file along with your project files.

## Example 1

Below is a simple example of how to use this library to read a CSV file and print its contents:

```c
#include "csv/csv.h"
#include <stdio.h>

int main() 
{
    const char *filename = "test.csv"; // Replace with your CSV file's path
    char delimiter = ','; // Specify the delimiter used in your CSV file

    // Create a new CSVFile instance
    CSVFile* myCSV = csv_file_create(delimiter);
    if (!myCSV) 
    {
        fprintf(stderr, "Failed to create CSV file structure.\n");
        return 1;
    }

    // Read the CSV file
    csv_file_read(myCSV, filename);
    
    // Print the contents of the CSV file
    printf("Contents of the CSV file:\n");
    csv_print(myCSV);

    // Clean up and free memory
    csv_file_destroy(myCSV);

    return 0;
}
```

### Example 2: Appending Rows to CSV

```c
#include "csv/csv.h"
#include <stdio.h>

int main() 
{
    CSVFile* csv = csv_file_create(',');

    // Create a new row and add some cells to it
    CSVRow* newRow = csv_row_create();
    csv_row_append_cell(newRow, "New Cell 1");
    csv_row_append_cell(newRow, "New Cell 2");
    csv_row_append_cell(newRow, "New Cell 3");

    // Append the new row to the CSV file
    csv_file_append_row(csv, newRow);

    // Write the updated CSV file to a new file
    csv_file_write(csv, "updated_file.csv");

    // Clean up
    csv_file_destroy(csv);

    return 0;
}
```

### Example 2: Removing Rows from CSV

```c
#include "csv/csv.h"
#include <stdio.h>

int main() 
{
    CSVFile* csv = csv_file_create(',');

    // Read an existing CSV file
    csv_file_read(csv, "test.csv");

    // Remove the second row (index 1)
    csv_file_remove_row(csv, 1);

    // Write the updated CSV file to a new file
    csv_file_write(csv, "reduced_file.csv");

    // Clean up
    csv_file_destroy(csv);

    return 0;
}
```

### Example 3: Exporting CSV to JSON

```c
#include "csv/csv.h"
#include <stdio.h>
#include <stdlib.h>

int main() 
{
    CSVFile* csv = csv_file_create(',');

    if (csv)  // Read an existing CSV file
    {
        csv_file_read(csv, "test.csv");
        if (csv->size == 0) 
            fprintf(stderr, "Error: No data read from CSV file.\n");
        else 
        {
            char* json = csv_export_to_json(csv); // Export to JSON
            if (json) 
            {
                printf("JSON Output:\n%s\n", json);
                free(json); // Remember to free the allocated memory for the JSON string
            }
            else 
                fprintf(stderr, "Error: Failed to export CSV to JSON.\n");
        }
        csv_file_destroy(csv);
    }
    else 
        fprintf(stderr, "Error: Failed to create CSV structure.\n");
    
    return 0;
}
```

### Example 4: Inserting Columns into CSV

```c
#include "csv/csv.h"
#include <stdio.h>

int main() 
{
    CSVFile* csv = csv_file_create(',');

    // Read an existing CSV file
    csv_file_read(csv, "test.csv");

    // Create a column to be inserted
    CSVRow* newColumn = csv_row_create();
    csv_row_append_cell(newColumn, "Column Header");
    csv_row_append_cell(newColumn, "Data 1");
    csv_row_append_cell(newColumn, "Data 2");
    // ... add more cells as needed

    // Insert the column at index 2
    csv_file_insert_column(csv, 2, newColumn);

    // Write the updated CSV file to a new file
    csv_file_write(csv, "file_with_new_column.csv");

    // Clean up
    csv_row_destroy(newColumn);
    csv_file_destroy(csv);

    return 0;
}
```
