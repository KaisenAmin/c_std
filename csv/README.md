
# CSV Library in C

**Author:** amin tahmasebi
**Release Date:** 2023
**License:** ISC License


## Overview

This library provides a simple and efficient way to read, manipulate, and write CSV (Comma-Separated Values) files. It offers functions for creating and destroying CSV rows and files, reading and writing CSV files, and performing various operations like appending rows, inserting columns, and exporting data in JSON format.

## Features

- **Dynamic Memory Management**: Automatically manages memory for CSV rows and files, resizing as needed.
- **File Operations**: Read from and write to CSV files with customizable delimiters.
- **Data Manipulation**: Append rows, insert columns, and access individual cells.
- **Utility Functions**: Includes functions for printing CSV files to the console, finding rows based on search terms, validating cell formats, and more.
- **JSON Export**: Ability to export CSV data to JSON format.

## Installation

To use this library, include the `csv.h` header in your C project and compile the `csv.c` source file along with your project files.

```bash
gcc -std=c17 -O2 -o my_program my_program.c csv/csv.c fmt/fmt.c file_io/file_reader.c file_io/file_writer.c
```

> **Note:** The examples below reference two sample files: `sources/test.csv` (a CSV with a header row followed by data rows) and `sources/test_10.csv` (ten data rows without a header row).

## Functions Descriptions


### `CsvRow* csv_row_create(void)`
**Purpose**: Creates a new, empty `CsvRow` structure.
**Parameters**: None.
**Return Value**: A pointer to the newly created `CsvRow` structure, or terminates the program if memory allocation fails.
**Usage Case**: Allocates memory for a `CsvRow` structure and initializes the row by setting the `cells` pointer to `NULL`, `size` to `0`, and `capacity` to `0` to indicate an empty row. Logs the success or failure of the operation.

---

### `void csv_row_destroy(CsvRow *row)`
**Purpose**: Frees the memory associated with a `CsvRow` structure and its contents.
**Parameters**:
  - `row`: Pointer to the `CsvRow` structure to be destroyed.
**Return Value**: None.
**Usage Case**: Frees each individual cell in the `CsvRow`, then frees the array of cells (`row->cells`) and the `CsvRow` structure itself. Logs each step, including any errors if the `row` is `NULL`.

---

### `void csv_row_append_cell(CsvRow *row, const char *value)`
**Purpose**: Adds a new cell with a specified value to a `CsvRow`.
**Parameters**:
  - `row`: Pointer to the `CsvRow` structure where the cell will be added.
  - `value`: A string representing the value to append.
**Return Value**: None.
**Usage Case**: Dynamically resizes the internal array of cells if necessary. Uses `string_strdup` to copy the `value` string into the newly created cell. Logs each step of the process, including the new value being appended and any memory allocation failures.

---

### `char* csv_row_get_cell(const CsvRow *row, size_t index)`
**Purpose**: Retrieves the value of a cell at a specified index in a `CsvRow`.
**Parameters**:
  - `row`: Pointer to the `CsvRow` structure.
  - `index`: The index of the cell to retrieve.
**Return Value**: A pointer to the string value of the cell, or `NULL` if the index is out of bounds or the row is `NULL`.
**Usage Case**: Checks if the index is valid and the `row` is not `NULL`. Logs the outcome, including any errors, and returns the value at the specified index if valid.

---

### `CsvFile* csv_file_create(char delimiter)`
**Purpose**: Creates a new `CsvFile` structure with a specified delimiter.
**Parameters**:
  - `delimiter`: The character used to separate fields in the CSV file (e.g., `,` or `;`).
**Return Value**: A pointer to the newly created `CsvFile` structure.
**Usage Case**: Allocates memory for the `CsvFile` structure, initializes the `rows` pointer to `NULL`, `size` to `0`, and `capacity` to `0` (indicating an empty file), and sets the provided delimiter for separating fields. Logs the success or failure of the memory allocation.

---

### `void csv_file_destroy(CsvFile *file)`
**Purpose**: Frees the memory associated with a `CsvFile` structure and its rows.
**Parameters**:
  - `file`: Pointer to the `CsvFile` structure to be destroyed.
**Return Value**: None.
**Usage Case**: Iterates through each `CsvRow` in the `CsvFile` and destroys it using `csv_row_destroy`, then frees the array of `rows` and the `CsvFile` structure itself. Logs each step, including any errors (e.g., if `file` is `NULL`).

---

### `void csv_file_read(CsvFile *file, const char *filename)`
**Purpose**: Reads the contents of a CSV file into a `CsvFile` structure.
**Parameters**:
  - `file`: Pointer to the `CsvFile` structure where the CSV contents will be stored.
  - `filename`: The name of the CSV file to read.
**Return Value**: None.
**Usage Case**: Opens the CSV file for reading using a file reader utility (`file_reader_open`), reads the file line by line stripping any newline characters, parses each line into a `CsvRow` using `parse_csv_line`, and appends it to the `CsvFile`. Logs each step of the process, including opening, reading, parsing, and closing the file.

---

### `void csv_file_write(const CsvFile *file, const char *filename)`
**Purpose**: Writes the contents of a `CsvFile` structure to a specified file.
**Parameters**:
  - `file`: Pointer to the `CsvFile` structure containing the CSV data.
  - `filename`: The name of the file where the CSV data will be written.
**Return Value**: None.
**Usage Case**: Opens the specified file for writing, iterates through each row and cell of the `CsvFile` writing them to the file separated by the specified delimiter, and writes each row as a new line. Ensures proper memory handling and logs progress, including any errors encountered during file operations.

---

### `void csv_file_append_row(CsvFile *file, CsvRow *row)`
**Purpose**: Appends a new row to a `CsvFile` structure.
**Parameters**:
  - `file`: Pointer to the `CsvFile` structure where the row will be appended.
  - `row`: Pointer to the `CsvRow` structure to append.
**Return Value**: None.
**Usage Case**: Checks if the `CsvFile` has enough capacity to hold the new row; if not, reallocates memory for the `rows` array doubling its capacity. Appends the new row at the end of the `rows` array. Logs the resizing and appending process, including memory allocation checks and errors.

---

### `CsvRow* csv_file_get_row(const CsvFile *file, size_t index)`
**Purpose**: Retrieves a specific row from a `CsvFile` by its index.
**Parameters**:
  - `file`: Pointer to the `CsvFile` structure from which to retrieve the row.
  - `index`: The index of the row to retrieve.
**Return Value**: Pointer to the `CsvRow` structure at the specified index, or `NULL` if the index is out of bounds or the file is `NULL`.
**Usage Case**: Performs bounds checking to ensure the index is valid. Logs the process and any errors, such as invalid indices or `NULL` pointers.

---

### `void csv_file_remove_row(CsvFile *file, size_t index)`
**Purpose**: Removes a row from a `CsvFile` at the specified index.
**Parameters**:
  - `file`: Pointer to the `CsvFile` structure from which to remove the row.
  - `index`: The index of the row to remove.
**Return Value**: None.
**Usage Case**: Destroys the row at the specified index using `csv_row_destroy`, shifts all subsequent rows one position forward to fill the gap, and decreases the size of the `CsvFile`. Logs the process, including any errors due to invalid indices or memory issues.

---

### `void csv_print(const CsvFile *file)`
**Purpose**: Prints the contents of a `CsvFile` structure to the standard output.
**Parameters**:
  - `file`: Pointer to the `CsvFile` structure to print.
**Return Value**: None.
**Usage Case**: Iterates through each row and cell of the `CsvFile`, printing the content to the standard output with cells within each row separated by the delimiter character. Handles error cases where the `file` is `NULL` and logs the process and any issues that occur during execution.

---

### `CsvRow* csv_row_read_next(FileReader *reader, char delimiter)`
**Purpose**: Reads the next row from a file using a `FileReader` object.
**Parameters**:
  - `reader`: Pointer to the `FileReader` object used to read the file.
  - `delimiter`: The character used to separate fields in the row.
**Return Value**: Pointer to a `CsvRow` containing the parsed cells, or `NULL` if an error occurs or the end of the file is reached.
**Usage Case**: Reads a line from the file and splits it into cells based on the specified delimiter. Handles quoted fields and trims newlines from the end of the line. Logs progress and errors during the reading and parsing of each line.

---

### `void csv_file_insert_column(CsvFile *file, size_t colIndex, const CsvRow *colData)`
**Purpose**: Inserts a new column into a `CsvFile` structure at a specified index.
**Parameters**:
  - `file`: Pointer to the `CsvFile` where the column will be inserted.
  - `colIndex`: The index at which to insert the new column.
  - `colData`: Pointer to the `CsvRow` containing the data for the new column.
**Return Value**: None.
**Usage Case**: Inserts a column into each row of the `CsvFile` at the specified index. If a row does not have a corresponding value in `colData`, a `NULL` value is inserted. Handles memory reallocation to ensure enough space for the new column and logs the process, including memory allocation and shifting of cells to make room.

---

### `CsvRow* csv_file_get_header(const CsvFile *file)`
**Purpose**: Retrieves the header row from a `CsvFile`.
**Parameters**:
  - `file`: Pointer to the `CsvFile` from which to retrieve the header.
**Return Value**: Pointer to the header `CsvRow`, or `NULL` if the file is empty or invalid.
**Usage Case**: Returns the first row of the `CsvFile`, which is typically considered the header. Logs the process and any issues, such as empty or `NULL` files.

---

### `void csv_file_set_header(CsvFile *file, CsvRow *header)`
**Purpose**: Replaces the existing header row in a `CsvFile` structure with a new header. If the file has no rows, the header is inserted as the first row.
**Parameters**:
  - `file`: Pointer to the `CsvFile` structure where the new header will be set.
  - `header`: Pointer to the `CsvRow` structure that will be set as the new header.
**Return Value**: None.
**Usage Case**: If the file already contains rows, the current header (first row) is destroyed and replaced by the new header. If the file is empty, it checks the capacity and resizes the row array if necessary. Logs all operations, including memory allocation and row destruction.

---

### `int csv_row_get_cell_as_int(const CsvRow *row, size_t index)`
**Purpose**: Retrieves the value of a specific cell in a `CsvRow` and converts it to an integer.
**Parameters**:
  - `row`: Pointer to the `CsvRow` from which the cell value will be retrieved.
  - `index`: The index of the cell to retrieve within the row.
**Return Value**: The integer value of the cell. Returns 0 if an error occurs (invalid index or null row).
**Usage Case**: Logs the start of the function, checks for invalid input, and converts the string value of the cell at the given index into an integer using `atoi`. Logs any errors, including invalid indices or null row references.

---

### `CsvRow** csv_file_find_rows(const CsvFile *file, const char* searchTerm)`
**Purpose**: Finds and returns all rows in a `CsvFile` that contain a specific search term.
**Parameters**:
  - `file`: Pointer to the `CsvFile` to search.
  - `searchTerm`: The term to search for in the cells of the CSV file.
**Return Value**: An array of pointers to the rows that contain the search term. The array is null-terminated. If no matches are found, the function returns `NULL`.
**Usage Case**: Searches through each row and each cell in the CSV file for the given search term. Allocates memory to store pointers to matching rows and resizes the array to match the exact number of found rows. Logs memory allocation and search progress for each row and cell, as well as when matches are found. The function breaks the inner loop after finding the first occurrence in a row, ensuring only the first match per row is returned.

---

### `bool csv_validate_cell_format(const CsvRow *row, size_t index, const char *format)`
**Purpose**: Validates the format of a specific cell in a `CsvRow` by comparing the content of the cell to a formatted string.
**Parameters**:
  - `row`: Pointer to the `CsvRow` containing the cell to be validated.
  - `index`: The index of the cell in the row.
  - `format`: A format string that specifies the expected format of the cell content.
**Return Value**: Returns `true` if the cell content matches the given format, `false` otherwise.
**Usage Case**: Uses the `snprintf` function to format the cell content into a string and then compares it with the actual cell content. Logs errors for invalid parameters or if the cell doesn't match the format.

---

### `void csv_file_concatenate(CsvFile *file1, const CsvFile *file2)`
**Purpose**: Appends all rows from one `CsvFile` (source) to another `CsvFile` (destination). The rows are copied to ensure the source file remains unchanged.
**Parameters**:
  - `file1`: The target `CsvFile` where rows will be appended.
  - `file2`: The source `CsvFile` from which rows will be copied.
**Return Value**: None.
**Usage Case**: For each row in the source file (`file2`), the function creates a new row in the target file (`file1`) and appends all cells from the source row. The source file is not modified, and memory is dynamically allocated for the new rows in the target file.

---

### `int csv_column_sum(const CsvFile *file, size_t columnIndex)`
**Purpose**: Sums the integer values of a specific column in a `CsvFile`.
**Parameters**:
  - `file`: Pointer to the `CsvFile` containing the data.
  - `columnIndex`: The index of the column to sum.
**Return Value**: The sum of the integer values in the specified column. If there are errors, it returns `0`.
**Usage Case**: Iterates through each row in the CSV file and adds the integer values in the specified column. Logs warnings if a row doesn't contain enough columns for the specified index, indicating that the column is out of range.

---

### `char* csv_export_to_json(const CsvFile *file)`
**Purpose**: Exports the contents of a `CsvFile` to a JSON-formatted string.
**Parameters**:
  - `file`: Pointer to the `CsvFile` to be exported.
**Return Value**: A JSON string representing the CSV file. The caller is responsible for freeing the allocated memory.
**Usage Case**: Loops through each row and cell of the CSV file, converting the data into JSON format. Each cell is represented as a JSON field (`"fieldX": "value"`) where `X` is the cell's index. The function dynamically allocates memory for the JSON string and ensures proper formatting (e.g., commas, curly braces). Logs all operations and errors, including memory allocation failures.

---

## Extensions (DataFrame-lite layer)

The seven functions below turn the library into a usable mini-DataFrame — in-memory loading, name-based cell lookup, column extraction/removal, sort, and generic predicate filtering.

### `void csv_file_load_from_string(CsvFile *file, const char *data)`
**Purpose**: Parse a CSV held in memory (string, HTTP response, embedded asset) into an existing `CsvFile`. Same semantics as `csv_file_read` but no disk needed.
**Parameters**:
  - `file`: Target `CsvFile` (created by `csv_file_create`).
  - `data`: NUL-terminated CSV text. NULL is a safe no-op.
**Return Value**: None.
**Usage Case**: Splits on `\n`; trailing `\r` from CRLF endings is stripped. Empty lines are skipped. Quoted cells with embedded delimiters are honored (re-uses `parse_csv_line`). Rows are **appended** — calling twice concatenates two strings into the same file.

---

### `int csv_file_find_column_index(const CsvFile *file, const char *columnName)`
**Purpose**: Look up a column's index by its header name. The foundation for name-based access.
**Parameters**:
  - `file`: File whose first row is the header.
  - `columnName`: Exact, case-sensitive header to look up.
**Return Value**: The 0-based column index, or `-1` if there's no header / no match / NULL input.
**Usage Case**: Pure header lookup — no signal about whether subsequent rows contain that column. Matching is *exact* (not substring/prefix) and case-sensitive.

---

### `char* csv_row_get_cell_by_name(const CsvFile *file, const CsvRow *row, const char *columnName)`
**Purpose**: Fetch a cell from `row` by header name. The DataFrame-style accessor most callers actually want.
**Parameters**:
  - `file`: Source file (used for header lookup).
  - `row`: Row to read from.
  - `columnName`: Header name of the column.
**Return Value**: Borrowed pointer to the cell string (owned by the row — don't free), or NULL if the file has no header / no such column / the row is shorter than the resolved index.
**Usage Case**: Combines `csv_file_find_column_index` and `csv_row_get_cell` into a single name-based accessor, eliminating the need to track column positions manually across schema changes.

---

### `char** csv_file_get_column(const CsvFile *file, size_t columnIndex, size_t *outCount, bool skipHeader)`
**Purpose**: Extract one column as a `NULL`-terminated array of borrowed cell-string pointers — the standard "give me column X" data-analysis primitive.
**Parameters**:
  - `file`: Source file.
  - `columnIndex`: Column to extract.
  - `outCount`: Receives the number of entries (excluding the trailing NULL). May be NULL.
  - `skipHeader`: If true, the first row is excluded.
**Return Value**: Newly-allocated `NULL`-terminated array, or NULL on bad input / OOM.
**Usage Case**: The strings inside the array are *borrowed* — they're owned by the source rows. **Free the array itself (`free(arr)`), never the individual strings**, and don't outlive the source `CsvFile`. Rows shorter than `columnIndex` contribute a `NULL` entry (the count still includes them — array index aligns with row index).

---

### `void csv_file_remove_column(CsvFile *file, size_t columnIndex)`
**Purpose**: Drop a column from every row. Symmetric to `csv_file_insert_column`.
**Parameters**:
  - `file`: File to modify in place.
  - `columnIndex`: Column to remove.
**Return Value**: None.
**Usage Case**: Frees the removed cell strings and shifts subsequent cells left within each row. Rows that don't reach `columnIndex` are left untouched (safe on jagged CSVs). NULL file and out-of-range index are silent no-ops.

---

### `void csv_file_sort(CsvFile *file, size_t columnIndex, bool ascending, bool skipHeader)`
**Purpose**: Sort rows in place by the string value in `columnIndex`.
**Parameters**:
  - `file`: File to sort.
  - `columnIndex`: Key column.
  - `ascending`: `true` for A→Z, `false` for Z→A.
  - `skipHeader`: `true` keeps row 0 pinned (almost always what you want when the file has a header).
**Return Value**: None.
**Usage Case**: Uses `qsort` under the hood; not stable across equal keys. Missing cells are treated as empty string for ordering. Comparison is `strcmp`, so numeric strings sort lexicographically (`"10" < "2"`). For numeric sort, project the column first or use `csv_file_filter` to build a typed view.

---

### `CsvFile* csv_file_filter(const CsvFile *file, CsvRowPredicate predicate, void *userdata, bool keepHeader)`
**Purpose**: Build a new `CsvFile` containing only rows that satisfy `predicate`. Generic, with a `userdata` pointer that gives you closure-by-convention.
**Parameters**:
  - `file`: Source file.
  - `predicate`: `bool (*)(const CsvRow*, void*)` — called once per non-header row; returning `true` keeps the row.
  - `userdata`: Opaque pointer forwarded to `predicate` on every call. May be NULL.
  - `keepHeader`: If `true` and `file` has rows, the first row is copied unconditionally (without consulting `predicate`).
**Return Value**: Newly-allocated `CsvFile` (caller must `csv_file_destroy`), or NULL on bad input / OOM.
**Usage Case**: Result is a **deep copy** — destroying it does not affect the source. Returned file has the same delimiter as the source.

---

### `bool csv_row_set_cell(CsvRow *row, size_t index, const char *value)`
**Purpose**: Replace the value of an existing cell. Fills the otherwise-glaring gap that `csv_row_append_cell` only appends — there was no way to mutate.
**Parameters**:
  - `row`: Target row.
  - `index`: 0-based cell index; must satisfy `index < row->size`.
  - `value`: New value. Empty string allowed; NULL is not.
**Return Value**: `true` on success, `false` if `row`/`value` is NULL, the index is out of range, or memory allocation fails.
**Usage Case**: The new value is copied (via `string_strdup`) so the caller may free its buffer immediately. The old cell's memory is `free()`d — repeated overwrites do not leak, even when the new string is much shorter than the old one.

---

### `CsvFile* csv_file_unique_by_column(const CsvFile *file, size_t columnIndex, bool keepHeader)`
**Purpose**: Deduplicate by a column. Returns a new `CsvFile` keeping the FIRST occurrence of each distinct value in `columnIndex`.
**Parameters**:
  - `file`: Source file.
  - `columnIndex`: Column whose value is the dedup key.
  - `keepHeader`: If `true` and `file` has rows, row 0 is copied unconditionally and excluded from the dedup pass.
**Return Value**: Newly-allocated `CsvFile` (caller must `csv_file_destroy`), or NULL on bad input / OOM.
**Usage Case**: Source is **untouched** — result is a deep copy. Comparison is `strcmp`-exact (case-sensitive). Rows shorter than `columnIndex` are treated as having the empty-string key — so all "short" rows collapse to one. Complexity is O(N²) (no internal hash table). Fine for typical CSV sizes.

---

### `char* csv_file_export_to_string(const CsvFile *file)`
**Purpose**: Serialize the whole file to a malloc'd in-memory string. Symmetric counterpart to `csv_file_load_from_string` — without it, there's no in-memory round-trip without going through disk.
**Parameters**:
  - `file`: Source file.
**Return Value**: Newly-allocated NUL-terminated CSV string (caller frees with `free()`), or NULL on bad input / OOM. For an empty file the result is `""`.
**Usage Case**: Cells are joined with `file->delimiter`; rows end in a single `\n`. Output is **byte-identical** to what `csv_file_write` produces on every platform — both always emit `\n`-only line endings (the writer opens its file in binary mode to suppress the C-runtime's CRLF translation on Windows). Does NOT add CSV quote-escaping for cells with embedded delimiters/quotes/newlines (matches `csv_file_write` so the same file written two ways stays consistent).

---

## Example 1
```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    const char *filename = "sources/test_10.csv";
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
3,"Cardinal Slant-D Ring Binder, Heavy Gauge Vinyl",Barry French,293,46.71,8.69,2.99,Nunavut,Binders and Binder Accessories,0.39
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
**Result**
```
(no console output — one row written to sources/updated_file.csv:)
New Cell 1,New Cell 2,سلول جدید 3
```

---

## Example 3: Removing Rows from CSV

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    CsvFile* csv = csv_file_create(',');

    csv_file_read(csv, "sources/test_10.csv"); // Read an existing CSV file
    csv_file_remove_row(csv, 1); // Remove the second row (index 1)
    csv_file_write(csv, "sources/reduced_file.csv");

    // Clean up
    csv_file_destroy(csv);
    return 0;
}
```
**Result**
```
(no console output — row 1 removed; result written to sources/reduced_file.csv)
```

---

## Example 4: Exporting CSV to JSON

```c
#include "csv/csv.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    CsvFile* csv = csv_file_create(',');

    csv_file_load_from_string(csv,
        "id,name,city\n"
        "1,Alice,Berlin\n"
        "2,Bob,London\n");

    char* json = csv_export_to_json(csv);
    if (json) {
        fmt_printf("JSON Output:\n%s\n", json);
        free(json);
    }
    else {
        fmt_fprintf(stderr, "Error: Failed to export CSV to JSON.\n");
    }

    csv_file_destroy(csv);
    return 0;
}
```
**Result**
```
JSON Output:
[
  {
    "field0": "id",
    "field1": "name",
    "field2": "city"
  },
  {
    "field0": "1",
    "field1": "Alice",
    "field2": "Berlin"
  },
  {
    "field0": "2",
    "field1": "Bob",
    "field2": "London"
  }
]
```

---

## Example 5: Inserting Columns into CSV

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    CsvFile* csv = csv_file_create(',');
    csv_file_read(csv, "sources/test.csv"); // Read an existing CSV file

    // Create a column to be inserted
    CsvRow* newColumn = csv_row_create();
    csv_row_append_cell(newColumn, "Column Header");
    csv_row_append_cell(newColumn, "Data 1");
    csv_row_append_cell(newColumn, "Data 2");
    // ... add more cells as needed

    csv_file_insert_column(csv, 2, newColumn);  // Insert the column at index 2
    csv_file_write(csv, "sources/file_with_new_column.csv"); // Write the updated CSV file to a new file

    // Clean up
    csv_row_destroy(newColumn);
    csv_file_destroy(csv);
    return 0;
}
```
**Result in sources/file_with_new_column.csv**
```
Order ID,Product Name,Column Header,Customer,Quantity,Discount,Tax,Profit,Region,Category,Rating
1,"Eldon Base for stackable storage shelf, platinum",Data 1,Muhammed MacIntyre,3,-213.25,38.94,35,Nunavut,Storage & Organization,0.8
2,"1.7 Cubic Foot Compact ""Cube"" Office Refrigerators",Data 2,Barry French,293,457.81,208.16,68.02,Nunavut,Appliances,0.58
3,"Cardinal Slant-D Ring Binder, Heavy Gauge Vinyl",(null),Barry French,293,46.71,8.69,2.99,Nunavut,Binders and Binder Accessories,0.39
4,R380,(null),Clay Rozendal,483,1198.97,195.99,3.99,Nunavut,Telephones and Communication,0.58
5,Holmes HEPA Air Purifier,(null),Carlos Soltero,515,30.94,21.78,5.94,Nunavut,Appliances,0.5
```

---

## Example 6: Concatenate Two Files

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
3,"Cardinal Slant-D Ring Binder, Heavy Gauge Vinyl",Barry French,293,46.71,8.69,2.99,Nunavut,Binders and Binder Accessories,0.39
4,R380,Clay Rozendal,483,1198.97,195.99,3.99,Nunavut,Telephones and Communication,0.58
5,Holmes HEPA Air Purifier,Carlos Soltero,515,30.94,21.78,5.94,Nunavut,Appliances,0.5
```

---

## Example 7: Find Value in Rows

```c
#include "csv/csv.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    CsvFile* csv = csv_file_create(',');
    csv_file_read(csv, "sources/test_10.csv");

    const char* searchTerm = "Barry";
    CsvRow** foundRows = csv_file_find_rows(csv, searchTerm);

    if (foundRows) {
        for (size_t i = 0; foundRows[i] != NULL; i++) {
            fmt_printf("Found Row %zu:\n", i);
            for (size_t j = 0; j < foundRows[i]->size; j++) {
                fmt_printf("%s ", foundRows[i]->cells[j]);
            }
            fmt_printf("\n");
        }
        free(foundRows);
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

## Example 8: Validate Cell Format

```c
#include "csv/csv.h"
#include "fmt/fmt.h"
#include <string.h>
#include <stdlib.h>

int main() {
    CsvFile* csv = csv_file_create(',');
    csv_file_load_from_string(csv,
        "order_id,quantity,customer\n"
        "1,42,Alice\n"
        "2,abc,Bob\n"
        "3,100,Carol\n");

    /* Validate the 'quantity' column (index 1): check whether each
       cell string round-trips through atoi unchanged (i.e. is a valid
       plain integer). Skip the header row (index 0). */
    for (size_t i = 1; i < csv->size; ++i) {
        CsvRow* row = csv_file_get_row(csv, i);
        if (row != NULL) {
            char* cell = csv_row_get_cell(row, 1);   /* quantity column */
            int value = atoi(cell);
            char* buffer = fmt_sprintf("%d", value);
            bool isValid = strcmp(buffer, cell) == 0;
            fmt_printf("Row %zu - Quantity '%s' is %s\n",
                       i, cell, isValid ? "valid integer" : "not a valid integer");
            free(buffer);
        }
    }

    csv_file_destroy(csv);
    return 0;
}
```
**Result**
```
Row 1 - Quantity '42' is valid integer
Row 2 - Quantity 'abc' is not a valid integer
Row 3 - Quantity '100' is valid integer
```

---

## Example 9: Header Operations

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    CsvFile* csv = csv_file_create(',');
    CsvRow* header = csv_row_create();

    csv_row_append_cell(header, "Column1");
    csv_row_append_cell(header, "Column2");
    csv_row_append_cell(header, "Column3");

    csv_file_read(csv, "sources/test.csv");

    fmt_printf("**Header Row Before set new header**\n");
    CsvRow* retrievedHeader = csv_file_get_header(csv);
    for (size_t i = 0; i < retrievedHeader->size; ++i) {
        fmt_printf("%s ", retrievedHeader->cells[i]);
    }

    csv_file_set_header(csv, header);
    retrievedHeader = csv_file_get_header(csv);

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

## Example 10: Finding Cell Value and Convert It to Integer

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    CsvFile* csv = csv_file_create(',');
    csv_file_read(csv, "sources/test_10.csv"); // Read a CSV file

    CsvRow* row = csv_file_get_row(csv, 2); // Get the third row (index 2)
    if (row != NULL) {
        int value = csv_row_get_cell_as_int(row, 0);  // Convert the value of the first cell to an integer
        fmt_printf("Value of the first cell in third row as integer: %d\n", value);
    }

    csv_file_destroy(csv);
    return 0;
}
```
**Result**
```
Value of the first cell in third row as integer: 3
```

---

## Example 11: Printing CSV File to Console

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
2,"1.7 Cubic Foot Compact ""Cube"" Office Refrigerators",Barry French,293,457.81,208.16,68.02,Nunavut,Appliances,0.58
3,"Cardinal Slant-D Ring Binder, Heavy Gauge Vinyl",Barry French,293,46.71,8.69,2.99,Nunavut,Binders and Binder Accessories,0.39
4,R380,Clay Rozendal,483,1198.97,195.99,3.99,Nunavut,Telephones and Communication,0.58
5,Holmes HEPA Air Purifier,Carlos Soltero,515,30.94,21.78,5.94,Nunavut,Appliances,0.5
6,G.E. Longer-Life Indoor Recessed Floodlight Bulbs,Carlos Soltero,515,4.43,6.64,4.95,Nunavut,Office Furnishings,0.37
7,"Angle-D Binders with Locking Rings, Label Holders",Carl Jackson,613,-54.04,7.3,7.72,Nunavut,Binders and Binder Accessories,0.38
8,"SAFCO Mobile Desk Side File, Wire Frame",Carl Jackson,613,127.70,42.76,6.22,Nunavut,Storage & Organization,
9,"SAFCO Commercial Wire Shelving, Black",Monica Federle,643,-695.26,138.14,35,Nunavut,Storage & Organization,
10,Xerox 198,Dorothy Badders,678,-226.36,4.98,8.33,Nunavut,Paper,0.38
```

---

## Example 12: Using String Library with CSV

```c
#include "csv/csv.h"
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Function to convert a string to uppercase using the String library
void to_uppercase(String *str) {
    size_t len = string_length(str);
    /* +1 for the NUL terminator: string_replace() treats `data` as a C-string
       (it calls strlen on it), so the buffer must be NUL-terminated. */
    char* data = (char*) malloc((len + 1) * sizeof(char));
    if (!data) {
        fmt_fprintf(stderr, "Error: Can not allocate Memory");
        exit(-1);
    }

    for (size_t i = 0; i < len; ++i) {
        char ch = string_at(str, i);
        data[i] = toupper(ch);
    }
    data[len] = '\0';
    string_replace(str, string_c_str(str), data);
    free(data);
}

int main() {
    const char *input_filename = "sources/test.csv";
    const char *output_filename = "sources/output.csv";
    char delimiter = ',';
    CsvFile* myCSV = csv_file_create(delimiter);

    csv_file_read(myCSV, input_filename);

    // Iterate through each row and cell
    for (size_t rowIndex = 0; rowIndex < myCSV->size; ++rowIndex) {
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
(no console output — modified data written to sources/output.csv:)
ORDER ID MODIFIED,PRODUCT NAME MODIFIED,CUSTOMER MODIFIED,QUANTITY MODIFIED,DISCOUNT MODIFIED,TAX MODIFIED,PROFIT MODIFIED,REGION MODIFIED,CATEGORY MODIFIED,RATING MODIFIED
1 MODIFIED,"ELDON BASE FOR STACKABLE STORAGE SHELF, PLATINUM" MODIFIED,MUHAMMED MACINTYRE MODIFIED,3 MODIFIED,-213.25 MODIFIED,38.94 MODIFIED,35 MODIFIED,NUNAVUT MODIFIED,STORAGE & ORGANIZATION MODIFIED,0.8 MODIFIED
2 MODIFIED,"1.7 CUBIC FOOT COMPACT ""CUBE"" OFFICE REFRIGERATORS" MODIFIED,BARRY FRENCH MODIFIED,293 MODIFIED,457.81 MODIFIED,208.16 MODIFIED,68.02 MODIFIED,NUNAVUT MODIFIED,APPLIANCES MODIFIED,0.58 MODIFIED
3 MODIFIED,"CARDINAL SLANT-D RING BINDER, HEAVY GAUGE VINYL" MODIFIED,BARRY FRENCH MODIFIED,293 MODIFIED,46.71 MODIFIED,8.69 MODIFIED,2.99 MODIFIED,NUNAVUT MODIFIED,BINDERS AND BINDER ACCESSORIES MODIFIED,0.39 MODIFIED
4 MODIFIED,R380 MODIFIED,CLAY ROZENDAL MODIFIED,483 MODIFIED,1198.97 MODIFIED,195.99 MODIFIED,3.99 MODIFIED,NUNAVUT MODIFIED,TELEPHONES AND COMMUNICATION MODIFIED,0.58 MODIFIED
5 MODIFIED,HOLMES HEPA AIR PURIFIER MODIFIED,CARLOS SOLTERO MODIFIED,515 MODIFIED,30.94 MODIFIED,21.78 MODIFIED,5.94 MODIFIED,NUNAVUT MODIFIED,APPLIANCES MODIFIED,0.5 MODIFIED
```

---

### Example 13 — `csv_file_load_from_string`

Parse CSV that already lives in memory (e.g. an HTTP response body or an embedded asset). No temp file required.

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main(void) {
    const char* body =
        "id,name,city\r\n"
        "1,Alice,Berlin\r\n"
        "2,Bob,London\r\n";

    CsvFile* f = csv_file_create(',');
    csv_file_load_from_string(f, body);   /* CRLF tolerated */

    fmt_printf("rows: %zu\n", f->size);
    csv_print(f);

    csv_file_destroy(f);
    return 0;
}
```

**Result**
```
rows: 3
id,name,city
1,Alice,Berlin
2,Bob,London
```

---

### Example 14 — `csv_file_find_column_index`

Headers shift around between IdP exports. Look up by *name* and you stop caring.

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main(void) {
    CsvFile* f = csv_file_create(',');
    csv_file_load_from_string(f, "id,name,email,age\n1,Alice,a@x,30\n");

    fmt_printf("email column is at index %d\n", csv_file_find_column_index(f, "email"));
    fmt_printf("missing column lookup: %d\n", csv_file_find_column_index(f, "phone"));

    csv_file_destroy(f);
    return 0;
}
```

**Result**
```
email column is at index 2
missing column lookup: -1
```

---

### Example 15 — `csv_row_get_cell_by_name`

DataFrame-style cell access — no more "what column was email again?".

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main(void) {
    CsvFile* f = csv_file_create(',');
    csv_file_load_from_string(f,
        "id,name,email\n"
        "1,Alice,alice@x\n"
        "2,Bob,bob@y\n");

    /* Walk every data row by name. */
    for (size_t i = 1; i < f->size; ++i) {
        CsvRow* r = csv_file_get_row(f, i);
        fmt_printf("%s -> %s\n",
                   csv_row_get_cell_by_name(f, r, "name"),
                   csv_row_get_cell_by_name(f, r, "email"));
    }

    csv_file_destroy(f);
    return 0;
}
```

**Result**
```
Alice -> alice@x
Bob -> bob@y
```

---

### Example 16 — `csv_file_get_column`

Yank one column out for downstream analysis. The strings are borrowed — free only the array.

```c
#include "csv/csv.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    CsvFile* f = csv_file_create(',');
    csv_file_load_from_string(f,
        "id,name\n"
        "1,Alice\n"
        "2,Bob\n"
        "3,Carol\n");

    size_t n = 0;
    char** names = csv_file_get_column(f, 1, &n, /*skipHeader=*/true);

    fmt_printf("%zu name(s):\n", n);
    for (size_t i = 0; i < n; ++i) {
      fmt_printf("  - %s\n", names[i]);
    }

    free(names);                 
    csv_file_destroy(f);

    return 0;
}
```

**Result**
```
3 name(s):
  - Alice
  - Bob
  - Carol
```

---


### Example 17 — `csv_file_remove_column`

Symmetric to `csv_file_insert_column`. Useful for dropping PII before export.

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main(void) {
    CsvFile* f = csv_file_create(',');
    csv_file_load_from_string(f,
        "id,name,ssn,city\n"
        "1,Alice,123-45-6789,Berlin\n"
        "2,Bob,987-65-4321,London\n");

    /* Strip the SSN column before sending the file downstream. */
    csv_file_remove_column(f, csv_file_find_column_index(f, "ssn"));

    csv_print(f);
    csv_file_destroy(f);

    return 0;
}
```

**Result**
```
id,name,city
1,Alice,Berlin
2,Bob,London
```

---

### Example 18 — `csv_file_sort`

Sort by any column, header pinned at row 0.

```c
#include "csv/csv.h"
#include "fmt/fmt.h"


int main(void) {
    CsvFile* f = csv_file_create(',');
    csv_file_load_from_string(f,
        "id,name\n"
        "3,Carol\n"
        "1,Alice\n"
        "2,Bob\n");

    /* Sort by 'name', ascending, leaving the header in row 0. */
    csv_file_sort(f, csv_file_find_column_index(f, "name"),
                  /*ascending=*/true, /*skipHeader=*/true);

    csv_print(f);
    csv_file_destroy(f);

    return 0;
}
```

**Result**
```
id,name
1,Alice
2,Bob
3,Carol
```

---

### Example 19 — `csv_file_filter`

Generic predicate-based filter. The `userdata` argument lets you do closure-style "filter by this column equal to this value" without writing a new function per filter.

```c
#include "csv/csv.h"
#include "fmt/fmt.h"
#include <string.h>
#include <stdlib.h>

/* Predicate: keep rows where column[col] > threshold (numeric). */
typedef struct { int col; int threshold; } AgeArgs;
static bool over_threshold(const CsvRow* r, void* ud) {
    AgeArgs* a = (AgeArgs*)ud;
    if (!r || (size_t)a->col >= r->size) {
      return false;
    }

    return atoi(r->cells[a->col]) > a->threshold;
}

int main(void) {
    CsvFile* f = csv_file_create(',');
    csv_file_load_from_string(f,
        "id,name,age\n"
        "1,Alice,25\n"
        "2,Bob,42\n"
        "3,Carol,30\n"
        "4,Dave,55\n");

    AgeArgs args = { csv_file_find_column_index(f, "age"), 30 };
    CsvFile* over_30 = csv_file_filter(f, over_threshold, &args,
                                       /*keepHeader=*/true);

    fmt_printf("Adults > 30:\n");
    csv_print(over_30);

    csv_file_destroy(over_30);   
    csv_file_destroy(f);

    return 0;
}
```

**Result**
```
Adults > 30:
id,name,age
2,Bob,42
4,Dave,55
```

---

### Example 20 — `csv_row_set_cell`

Modify a cell in place. Useful for normalization passes (e.g. uppercase a name column, redact a value, fix a typo).

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main(void) {
    CsvFile* f = csv_file_create(',');
    csv_file_load_from_string(f, "id,name\n1,alice\n2,bob\n");

    /* Uppercase the first letter of every name (sort of — this just
       demonstrates set_cell; do real Unicode upcasing elsewhere). */
    for (size_t i = 1; i < f->size; ++i) {
        CsvRow* r = csv_file_get_row(f, i);

        const char* name = csv_row_get_cell_by_name(f, r, "name");
        if (!name || !*name) {
          continue;
        }

        char buf[64];
        snprintf(buf, sizeof(buf), "%c%s",(name[0] >= 'a' && name[0] <= 'z') ? name[0] - 32 : name[0], name + 1);
        csv_row_set_cell(r, csv_file_find_column_index(f, "name"), buf);
    }

    csv_print(f);
    csv_file_destroy(f);

    return 0;
}
```

**Result**
```
id,name
1,Alice
2,Bob
```

---

### Example 21 — `csv_file_unique_by_column`

Drop duplicates by any column. Keeps the first occurrence.

```c
#include "csv/csv.h"
#include "fmt/fmt.h"

int main(void) {
    CsvFile* f = csv_file_create(',');
    csv_file_load_from_string(f,
        "id,email\n"
        "1,alice@x\n"
        "2,bob@x\n"
        "3,alice@x\n"       /* duplicate email */
        "4,carol@x\n"
        "5,bob@x\n");       /* another duplicate */

    CsvFile* deduped = csv_file_unique_by_column(f, csv_file_find_column_index(f, "email"), /*keepHeader=*/true);

    csv_print(deduped);
    csv_file_destroy(deduped);
    csv_file_destroy(f);

    return 0;
}
```

**Result**
```
id,email
1,alice@x
2,bob@x
4,carol@x
```

---

### Example 22 — `csv_file_export_to_string`

Round-trip through memory without touching disk. The natural pair to `csv_file_load_from_string`.

```c
#include "csv/csv.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

int main(void) {
    /* Build a small file in memory. */
    CsvFile* a = csv_file_create(',');
    csv_file_load_from_string(a, "id,name\n1,Alice\n2,Bob\n");

    /* Serialize to a string. */
    char* s = csv_file_export_to_string(a);
    fmt_printf("---serialized---\n%s---end---\n", s);

    /* re-parse the string into a fresh file and confirm
       it has the same shape. */
    CsvFile* b = csv_file_create(',');
    csv_file_load_from_string(b, s);
    fmt_printf("round-trip rows: %zu (expected %zu)\n", b->size, a->size);

    free(s);
    csv_file_destroy(a);
    csv_file_destroy(b);

    return 0;
}
```

**Result**
```
---serialized---
id,name
1,Alice
2,Bob
---end---
round-trip rows: 3 (expected 3)
```

---

## License

This project is open-source and available under [ISC License].
