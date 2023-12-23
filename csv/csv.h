#ifndef CSV_H_
#define CSV_H_


#include <stdio.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

// Forward declarations
typedef struct CSVRow CSVRow;
typedef struct CSVFile CSVFile;

typedef struct CSVRow {
    char **cells; // Array of strings
    size_t size;  // Current number of cells
    size_t capacity; // Total capacity of the cells array
} CSVRow;

typedef struct CSVFile {
    CSVRow **rows; // Array of pointers to CSVRow
    size_t size;   // Current number of rows
    size_t capacity; // Total capacity of the rows array
    char delimiter; // Delimiter character
} CSVFile;

// CSV Row functions
CSVRow* csv_row_create();
void csv_row_destroy(CSVRow *row);
void csv_row_append_cell(CSVRow *row, const char *value);
char* csv_row_get_cell(const CSVRow *row, size_t index);

// CSV File functions
CSVFile* csv_file_create(char delimiter);
void csv_file_destroy(CSVFile *file);
void csv_file_read(CSVFile *file, const char *filename);
void csv_file_write(const CSVFile *file, const char *filename);
void csv_file_append_row(CSVFile *file, CSVRow *row);
CSVRow* csv_file_get_row(const CSVFile *file, size_t index);
void csv_file_remove_row(CSVFile *file, size_t index);

// Utility functions
void csv_print(const CSVFile *file);
CSVRow* csv_row_read_next(FILE *file, char delimiter);
void csv_file_insert_column(CSVFile *file, size_t colIndex, const CSVRow *colData); // Column Operations
CSVRow* csv_file_get_header(const CSVFile *file); // Header Handling
void csv_file_set_header(CSVFile *file, CSVRow *header);
int csv_row_get_cell_as_int(const CSVRow *row, size_t index); // Type Conversion
CSVRow** csv_file_find_rows(const CSVFile *file, const char* searchTerm); // Search and Filter
bool csv_validate_cell_format(const CSVRow *row, size_t index, const char *format); // Data Validation
void csv_file_join(const CSVFile *file1, const CSVFile *file2, size_t keyColumnIndex); // Join and Merge Operations
int csv_column_sum(const CSVFile *file, size_t columnIndex); // Data Aggregation
char* csv_export_to_json(const CSVFile *file); // Export to Other Formats


#endif
