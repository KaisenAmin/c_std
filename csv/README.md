
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


### `CsvRow* csv_row_create()`

- **Purpose**: Creates a new, empty `CsvRow` structure.
- **Parameters**: None.
- **Return**: A pointer to the newly created `CsvRow` structure, or terminates the program if memory allocation fails.
- **Details**: 
  - Allocates memory for a `CsvRow` structure.
  - Initializes the row by setting the `cells` pointer to `NULL`, `size` to `0`, and `capacity` to `0` to indicate an empty row.
  - Logs the success or failure of the operation.

---

### `void csv_row_destroy(CsvRow *row)`

- **Purpose**: Frees the memory associated with a `CsvRow` structure and its contents.
- **Parameters**:
  - `row`: Pointer to the `CsvRow` structure to be destroyed.
- **Return**: None.
- **Details**:
  - Frees each individual cell in the `CsvRow`.
  - Frees the array of cells (`row->cells`) and then the `CsvRow` structure itself.
  - Logs each step, including any errors if the `row` is `NULL`.

---

### `void csv_row_append_cell(CsvRow *row, const char *value)`

- **Purpose**: Adds a new cell with a specified value to a `CsvRow`.
- **Parameters**:
  - `row`: Pointer to the `CsvRow` structure where the cell will be added.
  - `value`: A string representing the value to append.
- **Return**: None.
- **Details**:
  - Dynamically resizes the internal array of cells if necessary.
  - Uses `string_strdup` to copy the `value` string into the newly created cell.
  - Logs each step of the process, including the new value being appended and any memory allocation failures.

---

### `char* csv_row_get_cell(const CsvRow *row, size_t index)`

- **Purpose**: Retrieves the value of a cell at a specified index in a `CsvRow`.
- **Parameters**:
  - `row`: Pointer to the `CsvRow` structure.
  - `index`: The index of the cell to retrieve.
- **Return**: A pointer to the string value of the cell, or `NULL` if the index is out of bounds or the row is `NULL`.
- **Details**:
  - Checks if the index is valid and the `row` is not `NULL`.
  - Logs the outcome, including any errors, and returns the value at the specified index if valid.

---

### `CsvFile* csv_file_create(char delimiter)`

- **Purpose**: Creates a new `CsvFile` structure with a specified delimiter.
- **Parameters**:
  - `delimiter`: The character used to separate fields in the CSV file (e.g., `,` or `;`).
- **Return**: A pointer to the newly created `CsvFile` structure.
- **Details**:
  - Allocates memory for the `CsvFile` structure.
  - Initializes the `rows` pointer to `NULL`, `size` to `0`, and `capacity` to `0` (indicating an empty file).
  - Sets the provided delimiter for separating fields.
  - Logs the success or failure of the memory allocation.

---

### `void csv_file_destroy(CsvFile *file)`

- **Purpose**: Frees the memory associated with a `CsvFile` structure and its rows.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` structure to be destroyed.
- **Return**: None.
- **Details**:
  - Iterates through each `CsvRow` in the `CsvFile` and destroys it using `csv_row_destroy`.
  - Frees the array of `rows` and the `CsvFile` structure itself.
  - Logs each step, including any errors (e.g., if `file` is `NULL`).

---

### `void csv_file_read(CsvFile *file, const char *filename)`

- **Purpose**: Reads the contents of a CSV file into a `CsvFile` structure.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` structure where the CSV contents will be stored.
  - `filename`: The name of the CSV file to read.
- **Return**: None.
- **Details**:
  - Opens the CSV file for reading using a file reader utility (`file_reader_open`).
  - Reads the file line by line, stripping any newline characters.
  - Each line is parsed into a `CsvRow` using `parse_csv_line` and then appended to the `CsvFile`.
  - Logs each step of the process, including opening, reading, parsing, and closing the file.

---

### `void csv_file_write(const CsvFile *file, const char *filename)`

- **Purpose**: Writes the contents of a `CsvFile` structure to a specified file.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` structure containing the CSV data.
  - `filename`: The name of the file where the CSV data will be written.
- **Return**: None.
- **Details**:
  - Opens the specified file for writing.
  - Iterates through each row and cell of the `CsvFile`, writing them to the file, separating cells with the specified delimiter.
  - Writes each row as a new line in the file.
  - Ensures proper memory handling and logs progress, including any errors encountered during file operations.

---

### `void csv_file_append_row(CsvFile *file, CsvRow *row)`

- **Purpose**: Appends a new row to a `CsvFile` structure.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` structure where the row will be appended.
  - `row`: Pointer to the `CsvRow` structure to append.
- **Return**: None.
- **Details**:
  - Checks if the `CsvFile` has enough capacity to hold the new row; if not, it reallocates memory for the `rows` array, doubling its capacity.
  - Appends the new row at the end of the `rows` array.
  - Logs the resizing and appending process, including memory allocation checks and errors.

---

### `CsvRow* csv_file_get_row(const CsvFile *file, size_t index)`

- **Purpose**: Retrieves a specific row from a `CsvFile` by its index.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` structure from which to retrieve the row.
  - `index`: The index of the row to retrieve.
- **Return**: Pointer to the `CsvRow` structure at the specified index, or `NULL` if the index is out of bounds or the file is `NULL`.
- **Details**:
  - Performs bounds checking to ensure the index is valid.
  - Logs the process and any errors, such as invalid indices or `NULL` pointers.

---

### `void csv_file_remove_row(CsvFile *file, size_t index)`

- **Purpose**: Removes a row from a `CsvFile` at the specified index.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` structure from which to remove the row.
  - `index`: The index of the row to remove.
- **Return**: None.
- **Details**:
  - Destroys the row at the specified index using `csv_row_destroy`.
  - Shifts all subsequent rows one position forward to fill the gap left by the removed row.
  - Decreases the size of the `CsvFile` and logs the process, including any errors due to invalid indices or memory issues.

---

### `void csv_print(const CsvFile *file)`

- **Purpose**: Prints the contents of a `CsvFile` structure to the standard output.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` structure to print.
- **Return**: None.
- **Details**:
  - Iterates through each row and cell of the `CsvFile`, printing the content to the standard output.
  - Cells within each row are separated by the delimiter character.
  - Handles error cases where the `file` is `NULL`.
  - Logs the process and any issues that occur during execution.

---

### `CsvRow* csv_row_read_next(FileReader *reader, char delimiter)`

- **Purpose**: Reads the next row from a file using a `FileReader` object.
- **Parameters**:
  - `reader`: Pointer to the `FileReader` object used to read the file.
  - `delimiter`: The character used to separate fields in the row.
- **Return**: Pointer to a `CsvRow` containing the parsed cells, or `NULL` if an error occurs or the end of the file is reached.
- **Details**:
  - Reads a line from the file and splits it into cells based on the specified delimiter.
  - Handles quoted fields and trims newlines from the end of the line.
  - Logs progress and errors during the reading and parsing of each line.

---

### `void csv_file_insert_column(CsvFile *file, size_t colIndex, const CsvRow *colData)`

- **Purpose**: Inserts a new column into a `CsvFile` structure at a specified index.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` where the column will be inserted.
  - `colIndex`: The index at which to insert the new column.
  - `colData`: Pointer to the `CsvRow` containing the data for the new column.
- **Return**: None.
- **Details**:
  - Inserts a column into each row of the `CsvFile` at the specified index.
  - If a row does not have a corresponding value in `colData`, a `NULL` value is inserted.
  - Handles memory reallocation to ensure enough space for the new column.
  - Logs the process, including memory allocation and shifting of cells to make room for the new column.

---

### `CsvRow* csv_file_get_header(const CsvFile *file)`

- **Purpose**: Retrieves the header row from a `CsvFile`.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` from which to retrieve the header.
- **Return**: Pointer to the header `CsvRow`, or `NULL` if the file is empty or invalid.
- **Details**:
  - Returns the first row of the `CsvFile`, which is typically considered the header.
  - Logs the process and any issues, such as empty or `NULL` files.

---

### `void csv_file_set_header(CsvFile *file, CsvRow *header)`

- **Purpose**: Replaces the existing header row in a `CsvFile` structure with a new header. If the file has no rows, the header is inserted as the first row.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` structure where the new header will be set.
  - `header`: Pointer to the `CsvRow` structure that will be set as the new header.
- **Return**: None.
- **Details**:
  - If the file already contains rows, the current header (first row) is destroyed and replaced by the new header.
  - If the file is empty, it checks the capacity and resizes the row array if necessary.
  - The function logs all operations, including memory allocation and row destruction.

---

### `int csv_row_get_cell_as_int(const CsvRow *row, size_t index)`

- **Purpose**: Retrieves the value of a specific cell in a `CsvRow` and converts it to an integer.
- **Parameters**:
  - `row`: Pointer to the `CsvRow` from which the cell value will be retrieved.
  - `index`: The index of the cell to retrieve within the row.
- **Return**: The integer value of the cell. Returns 0 if an error occurs (invalid index or null row).
- **Details**:
  - Logs the start of the function, checks for invalid input, and converts the string value of the cell at the given index into an integer.
  - Uses the `atoi` function for conversion.
  - Logs any errors, including invalid indices or null row references.

---

### `CsvRow** csv_file_find_rows(const CsvFile *file, const char* searchTerm)`

- **Purpose**: Finds and returns all rows in a `CsvFile` that contain a specific search term.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` to search.
  - `searchTerm`: The term to search for in the cells of the CSV file.
- **Return**: An array of pointers to the rows that contain the search term. The array is null-terminated. If no matches are found, the function returns `NULL`.
- **Details**:
  - Searches through each row and each cell in the CSV file for the given search term.
  - Allocates memory to store pointers to matching rows. The array is resized to match the exact number of found rows.
  - Logs memory allocation and search progress for each row and cell, as well as when matches are found.
  - The function breaks the inner loop after finding the first occurrence in a row, ensuring only the first match per row is returned.

---

### `bool csv_validate_cell_format(const CsvRow *row, size_t index, const char *format)`

- **Purpose**: Validates the format of a specific cell in a `CsvRow` by comparing the content of the cell to a formatted string.
- **Parameters**:
  - `row`: Pointer to the `CsvRow` containing the cell to be validated.
  - `index`: The index of the cell in the row.
  - `format`: A format string that specifies the expected format of the cell content.
- **Return**: Returns `true` if the cell content matches the given format, `false` otherwise.
- **Details**:
  - The function uses the `snprintf` function to format the cell content into a string and then compares it with the actual cell content.
  - Logs errors for invalid parameters or if the cell doesn't match the format.

---

### `void csv_file_concatenate(CsvFile *file1, const CsvFile *file2)`

- **Purpose**: Appends all rows from one `CsvFile` (source) to another `CsvFile` (destination). The rows are copied to ensure the source file remains unchanged.
- **Parameters**:
  - `file1`: The target `CsvFile` where rows will be appended.
  - `file2`: The source `CsvFile` from which rows will be copied.
- **Return**: None.
- **Details**:
  - For each row in the source file (`file2`), the function creates a new row in the target file (`file1`) and appends all cells from the source row.
  - The source file is not modified, and memory is dynamically allocated for the new rows in the target file.

---

### `int csv_column_sum(const CsvFile *file, size_t columnIndex)`

- **Purpose**: Sums the integer values of a specific column in a `CsvFile`.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` containing the data.
  - `columnIndex`: The index of the column to sum.
- **Return**: The sum of the integer values in the specified column. If there are errors, it returns `0`.
- **Details**:
  - Iterates through each row in the CSV file and adds the integer values in the specified column.
  - Logs warnings if a row doesn't contain enough columns for the specified index, indicating that the column is out of range.

---

### `char* csv_export_to_json(const CsvFile *file)`

- **Purpose**: Exports the contents of a `CsvFile` to a JSON-formatted string.
- **Parameters**:
  - `file`: Pointer to the `CsvFile` to be exported.
- **Return**: A JSON string representing the CSV file. The caller is responsible for freeing the allocated memory.
- **Details**:
  - The function loops through each row and cell of the CSV file, converting the data into JSON format.
  - Each cell is represented as a JSON field (`"fieldX": "value"`) where `X` is the cell's index.
  - The function dynamically allocates memory for the JSON string and ensures proper formatting (e.g., commas, curly braces).
  - Logs all operations and errors, including memory allocation failures.

---

## Example 1
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
**Result**
```
Contents of the CSV file:
1,"Eldon Base for stackable storage shelf, platinum",Muhammed MacIntyre,3,-213.25,38.94,35,Nunavut,Storage & Organization,0.8
2,"1.7 Cubic Foot Compact ""Cube"" Office Refrigerators",Barry French,293,457.81,208.16,68.02,Nunavut,Appliances,0.58
3,"Cardinal Slant-D� Ring Binder, Heavy Gauge Vinyl",Barry French,293,46.71,8.69,2.99,Nunavut,Binders and Binder Accessories,0.39
4,R380,Clay Rozendal,483,1198.97,195.99,3.99,Nunavut,Telephones and Communication,0.58
5,Holmes HEPA Air Purifier,Carlos Soltero,515,30.94,21.78,5.94,Nunavut,Appliances,0.5
6,G.E. Longer-Life Indoor Recessed Floodlight Bulbs,Carlos Soltero,515,4.43,6.64,4.95,Nunavut,Office Furnishings,0.37
7,"Angle-D Binders with Locking Rings, Label Holders",Carl Jackson,613,-54.04,7.3,7.72,Nunavut,Binders and Binder Accessories,0.38
8,"SAFCO Mobile Desk Side File, Wire Frame",Carl Jackson,613,127.70,42.76,6.22,Nunavut,Storage & Organization,
9,"SAFCO Commercial Wire Shelving, Black",Monica Federle,643,-695.26,138.14,35,Nunavut,Storage & Organization,
10,Xerox 198,Dorothy Badders,678,-226.36,4.98,8.33,Nunavut,Paper,0.38
```

---

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
**Result in file**
```
New Cell 1,New Cell 2,سلول جدید 3
```

---

## Example 2: Removing Rows from CSV

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    CsvFile* csv = csv_file_create(',');

    csv_file_read(csv, "sources/test.csv"); // Read an existing CSV file
    csv_file_remove_row(csv, 1); // Remove the second row (index 1)
    csv_file_write(csv, "sources/reduced_file.csv"); 

    // Clean up
    csv_file_destroy(csv);
    return 0;
}
```
**Result**
```
Contents of the CSV file:
1,"Eldon Base for stackable storage shelf, platinum",Muhammed MacIntyre,3,-213.25,38.94,35,Nunavut,Storage & Organization,0.8
3,"Cardinal Slant-D� Ring Binder, Heavy Gauge Vinyl",Barry French,293,46.71,8.69,2.99,Nunavut,Binders and Binder Accessories,0.39
4,R380,Clay Rozendal,483,1198.97,195.99,3.99,Nunavut,Telephones and Communication,0.58
5,Holmes HEPA Air Purifier,Carlos Soltero,515,30.94,21.78,5.94,Nunavut,Appliances,0.5
6,G.E. Longer-Life Indoor Recessed Floodlight Bulbs,Carlos Soltero,515,4.43,6.64,4.95,Nunavut,Office Furnishings,0.37
7,"Angle-D Binders with Locking Rings, Label Holders",Carl Jackson,613,-54.04,7.3,7.72,Nunavut,Binders and Binder Accessories,0.38
8,"SAFCO Mobile Desk Side File, Wire Frame",Carl Jackson,613,127.70,42.76,6.22,Nunavut,Storage & Organization,
9,"SAFCO Commercial Wire Shelving, Black",Monica Federle,643,-695.26,138.14,35,Nunavut,Storage & Organization,
10,Xerox 198,Dorothy Badders,678,-226.36,4.98,8.33,Nunavut,Paper,0.38
```

---

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
**Result**
```
Csv file is Data is 
Order ID,Product Name,Customer,Quantity,Discount,Tax,Profit,Region,Category,Rating
1,"Eldon Base for stackable storage shelf, platinum",Muhammed MacIntyre,3,-213.25,38.94,35,Nunavut,Storage & Organization,0.8
2,"1.7 Cubic Foot Compact ""Cube"" Office Refrigerators",Barry French,293,457.81,208.16,68.02,Nunavut,Appliances,0.58
3,"Cardinal Slant-D Ring Binder, Heavy Gauge Vinyl",Barry French,293,46.71,8.69,2.99,Nunavut,Binders and Binder Accessories,0.39
4,R380,Clay Rozendal,483,1198.97,195.99,3.99,Nunavut,Telephones and Communication,0.58
5,Holmes HEPA Air Purifier,Carlos Soltero,515,30.94,21.78,5.94,Nunavut,Appliances,0.5

---

JSON Output:
[
  {
    "Order ID": "1",
    "Product Name": ""Eldon Base for stackable storage shelf, platinum"",
    "Customer": "Muhammed MacIntyre",
    "Quantity": "3",
    "Discount": "-213.25",
    "Tax": "38.94",
    "Profit": "35",
    "Region": "Nunavut",
    "Category": "Storage & Organization",
    "Rating": "0.8"
  },
  {
    "Order ID": "2",
    "Product Name": ""1.7 Cubic Foot Compact ""Cube"" Office Refrigerators"",
    "Customer": "Barry French",
    "Quantity": "293",
    "Discount": "457.81",
    "Tax": "208.16",
    "Profit": "68.02",
    "Region": "Nunavut",
    "Category": "Appliances",
    "Rating": "0.58"
  },
  {
    "Order ID": "3",
    "Product Name": ""Cardinal Slant-D Ring Binder, Heavy Gauge Vinyl"",
    "Customer": "Barry French",
    "Quantity": "293",
    "Discount": "46.71",
    "Tax": "8.69",
    "Profit": "2.99",
    "Region": "Nunavut",
    "Category": "Binders and Binder Accessories",
    "Rating": "0.39"
  },
  {
    "Order ID": "4",
    "Product Name": "R380",
    "Customer": "Clay Rozendal",
    "Quantity": "483",
    "Discount": "1198.97",
    "Tax": "195.99",
    "Profit": "3.99",
    "Region": "Nunavut",
    "Category": "Telephones and Communication",
    "Rating": "0.58"
  },
  {
    "Order ID": "5",
    "Product Name": "Holmes HEPA Air Purifier",
    "Customer": "Carlos Soltero",
    "Quantity": "515",
    "Discount": "30.94",
    "Tax": "21.78",
    "Profit": "5.94",
    "Region": "Nunavut",
    "Category": "Appliances",
    "Rating": "0.5"
  }
]
```

---

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
**Result in file_with_new_column.csv**
```
Order ID,Product Name,Column Header,Customer,Quantity,Discount,Tax,Profit,Region,Category,Rating
1,"Eldon Base for stackable storage shelf, platinum",Data 1,Muhammed MacIntyre,3,-213.25,38.94,35,Nunavut,Storage & Organization,0.8
2,"1.7 Cubic Foot Compact ""Cube"" Office Refrigerators",Data 2,Barry French,293,457.81,208.16,68.02,Nunavut,Appliances,0.58
3,"Cardinal Slant-D Ring Binder, Heavy Gauge Vinyl",(null),Barry French,293,46.71,8.69,2.99,Nunavut,Binders and Binder Accessories,0.39
4,R380,(null),Clay Rozendal,483,1198.97,195.99,3.99,Nunavut,Telephones and Communication,0.58
5,Holmes HEPA Air Purifier,(null),Carlos Soltero,515,30.94,21.78,5.94,Nunavut,Appliances,0.5
```

---

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
**Result**
```
Order ID,Product Name,Customer,Quantity,Discount,Tax,Profit,Region,Category,Rating
1,"Eldon Base for stackable storage shelf, platinum",Muhammed MacIntyre,3,-213.25,38.94,35,Nunavut,Storage & Organization,0.8
2,"1.7 Cubic Foot Compact ""Cube"" Office Refrigerators",Barry French,293,457.81,208.16,68.02,Nunavut,Appliances,0.58
3,"Cardinal Slant-D Ring Binder, Heavy Gauge Vinyl",Barry French,293,46.71,8.69,2.99,Nunavut,Binders and Binder Accessories,0.39
4,R380,Clay Rozendal,483,1198.97,195.99,3.99,Nunavut,Telephones and Communication,0.58
5,Holmes HEPA Air Purifier,Carlos Soltero,515,30.94,21.78,5.94,Nunavut,Appliances,0.5
1,"Eldon Base for stackable storage shelf, platinum",Muhammed MacIntyre,3,-213.25,38.94,35,Nunavut,Storage & Organization,0.8
2,"1.7 Cubic Foot Compact ""Cube"" Office Refrigerators",Barry French,293,457.81,208.16,68.02,Nunavut,Appliances,0.58
3,"Cardinal Slant-D� Ring Binder, Heavy Gauge Vinyl",Barry French,293,46.71,8.69,2.99,Nunavut,Binders and Binder Accessories,0.39
4,R380,Clay Rozendal,483,1198.97,195.99,3.99,Nunavut,Telephones and Communication,0.58
5,Holmes HEPA Air Purifier,Carlos Soltero,515,30.94,21.78,5.94,Nunavut,Appliances,0.5
```

---

## Example 6 : find value in rows 
```c
#include "csv/csv.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    CsvFile* csv = csv_file_create(',');
    csv_file_read(csv, "sources/test_10.csv"); 

    const char* searchTerm = "Barry"; 
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
**Result**
```
Found Row 0:
2 "1.7 Cubic Foot Compact ""Cube"" Office Refrigerators" Barry French 293 457.81 208.16 68.02 Nunavut Appliances 0.58
Found Row 1:
3 "Cardinal Slant-D Ring Binder, Heavy Gauge Vinyl" Barry French 293 46.71 8.69 2.99 Nunavut Binders and Binder Accessories 0.39
```

---

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
            char* cell = csv_row_get_cell(row, 2); // Get the third cell 
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
**Result**
```
 g++ -std=c++17 -Wno-deprecated-declarations -o main main.cpp .\csv.c .\fmt.c .\std_string.c .\encoding.c .\file_writer.c .\file_reader.c | ./main
Value is Customer
Row 1 - Quantity Ordered 'Customer' is invalid
Value is Muhammed MacIntyre
Row 2 - Quantity Ordered 'Muhammed MacIntyre' is invalid
Value is 10
Row 3 - Quantity Ordered '10' is valid
Value is Barry French
Row 4 - Quantity Ordered 'Barry French' is invalid
Value is Clay Rozendal
Row 5 - Quantity Ordered 'Clay Rozendal' is invalid
Value is Carlos Soltero
Row 6 - Quantity Ordered 'Carlos Soltero' is invalid
PS C:\Users\asus\OneDrive\Desktop\stack>
```

---

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
    csv_file_read(csv, "./test_10.csv"); 
    
    fmt_println("**Header Row Before set new header**");
    CsvRow* retrievedHeader = csv_file_get_header(csv);

    for (size_t i = 0; i < retrievedHeader->size; ++i) {
        fmt_printf("%s ", retrievedHeader->cells[i]);
    }

    csv_file_set_header(csv, header);
    retrievedHeader = csv_file_get_header(csv); // Get and print the header row

    fmt_printf("\n**Header Row After set new headers**\n");

    for (size_t i = 0; i < retrievedHeader->size; ++i) {
        fmt_printf("%s ", retrievedHeader->cells[i]);
    }
    fmt_printf("\n");

    csv_file_destroy(csv);
    return 0;
}
```
**Result**
```
**Header Row Before set new header** 
Order ID Product Name Customer Quantity Discount Tax Profit Region Category Rating
**Header Row After set new headers**
Column1 Column2 Column3
```

---

## Example 9 : Finding cell value and convert it to Integer 
```c

#include "csv/csv.h"
#include "fmt/fmt.h"

int main() { 
    CsvFile* csv = csv_file_create(',');
    csv_file_read(csv, "sources/test_10.csv"); // Read a CSV file

    CsvRow* row = csv_file_get_row(csv, 2); // Get the third row (index 2)
    if (row != NULL)
    {
        int value = csv_row_get_cell_as_int(row, 0);  // Convert the value of the first cell to an integer
        fmt_printf("Value of the first cell in third row as integer: %d\n", value);
    }

    csv_file_destroy(csv);

    return 0;
}
```
**Result**
```
Value of the first cell in third row as integer: 2
```

---

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
**Result**
```
Order ID,Product Name,Customer,Quantity,Discount,Tax,Profit,Region,Category,Rating
1,"Eldon Base for stackable storage shelf, platinum",Muhammed MacIntyre,3,-213.25,38.94,35,Nunavut,Storage & Organization,0.8
2,"1.7 Cubic Foot Compact ""Cube"" Office Refrigerators",10, French,293,457.81,208.16,68.02,Nunavut,Appliances,0.58
3,"Cardinal Slant-D Ring Binder, Heavy Gauge Vinyl",Barry French,293,46.71,8.69,2.99,Nunavut,Binders and Binder Accessories,0.39
4,R380,Clay Rozendal,483,1198.97,195.99,3.99,Nunavut,Telephones and Communication,0.58
5,Holmes HEPA Air Purifier,Carlos Soltero,515,30.94,21.78,5.94,Nunavut,Appliances,0.5
```

---

## Example 11 : Using String lib with Csv 

```c
#include "csv/csv.h"
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Function to convert a string to uppercase using the String library
void to_uppercase(String *str) {
    char* data = (char*) malloc(string_length(str) * sizeof(char));
    if (!data) {
        fmt_fprintf(stderr, "Error: Can not allocate Memory");
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
    const char *input_filename = "sources/test_10.csv"; 
    const char *output_filename = "sources/output.csv"; 
    char delimiter = ','; 
    CsvFile* myCSV = csv_file_create(delimiter);

    csv_file_read(myCSV, input_filename);

    // Iterate through each row and cell
    for (size_t rowIndex = 0; rowIndex < myCSV->size; ++rowIndex)   {
        CsvRow* row = csv_file_get_row(myCSV, rowIndex);

        for (size_t cellIndex = 0; cellIndex < row->size; ++cellIndex) {
            char* cellValue = csv_row_get_cell(row, cellIndex);
            String* cellString = string_create(cellValue); 

            to_uppercase(cellString);  
            string_append(cellString, " MODIFIED");

            free(row->cells[cellIndex]); 
            row->cells[cellIndex] = strdup(string_c_str(cellString));
            string_deallocate(cellString);  
        }
    }

    csv_file_write(myCSV, output_filename);
    csv_file_destroy(myCSV); 
    return 0;
}
```
**Result**
```
ORDER ID MODIFIED,PRODUCT NAME MODIFIED,CUSTOMER MODIFIED,QUANTITY MODIFIED,DISCOUNT MODIFIED,TAX MODIFIED,PROFIT MODIFIED,REGION MODIFIED,CATEGORY MODIFIED,RATING MODIFIED
1 MODIFIED,"ELDON BASE FOR STACKABLE STORAGE SHELF, PLATINUM" MODIFIED,MUHAMMED MACINTYRE MODIFIED,3 MODIFIED,-213.25 MODIFIED,38.94 MODIFIED,35 MODIFIED,NUNAVUT MODIFIED,STORAGE & ORGANIZATION MODIFIED,0.8 MODIFIED
2 MODIFIED,"1.7 CUBIC FOOT COMPACT ""CUBE"" OFFICE REFRIGERATORS" MODIFIED,10 MODIFIED, FRENCH MODIFIED,293 MODIFIED,457.81 MODIFIED,208.16 MODIFIED,68.02 MODIFIED,NUNAVUT MODIFIED,APPLIANCES MODIFIED,0.58 MODIFIED
3 MODIFIED,"CARDINAL SLANT-D RING BINDER, HEAVY GAUGE VINYL" MODIFIED,BARRY FRENCH MODIFIED,293 MODIFIED,46.71 MODIFIED,8.69 MODIFIED,2.99 MODIFIED,NUNAVUT MODIFIED,BINDERS AND BINDER ACCESSORIES MODIFIED,0.39 MODIFIED
4 MODIFIED,R380 MODIFIED,CLAY ROZENDAL MODIFIED,483 MODIFIED,1198.97 MODIFIED,195.99 MODIFIED,3.99 MODIFIED,NUNAVUT MODIFIED,TELEPHONES AND COMMUNICATION MODIFIED,0.58 MODIFIED
5 MODIFIED,HOLMES HEPA AIR PURIFIER MODIFIED,CARLOS SOLTERO MODIFIED,515 MODIFIED,30.94 MODIFIED,21.78 MODIFIED,5.94 MODIFIED,NUNAVUT MODIFIED,APPLIANCES MODIFIED,0.5 MODIFIED
```

## License

This project is open-source and available under [ISC License].