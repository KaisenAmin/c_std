/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class CsvFile, CsvRow
*/

#ifndef CSV_H_
#define CSV_H_

#include "../file_io/file_reader.h"

#define BUFFER_SIZE 1024

// Forward declarations
typedef struct CsvRow CsvRow;
typedef struct CsvFile CsvFile;

typedef struct CsvRow {
    char **cells; // Array of strings
    size_t size;  // Current number of cells
    size_t capacity; // Total capacity of the cells array

} CsvRow;

typedef struct CsvFile {
    CsvRow **rows; // Array of pointers to CsvRow
    size_t size;   // Current number of rows
    size_t capacity; // Total capacity of the rows array
    char delimiter; // Delimiter character

} CsvFile;

// CSV Row functions
CsvRow* csv_row_create();
CsvRow* csv_file_get_row(const CsvFile *file, size_t index);
// CsvRow* csv_row_read_next(FILE *file, char delimiter);
CsvRow* csv_row_read_next(FileReader *reader, char delimiter);
CsvRow* csv_file_get_header(const CsvFile *file); // Header Handling
CsvRow** csv_file_find_rows(const CsvFile *file, const char* searchTerm); // Search and Filter

char* csv_row_get_cell(const CsvRow *row, size_t index);
char* csv_export_to_json(const CsvFile *file); // Export to Other Formats

CsvFile* csv_file_create(char delimiter);

void csv_file_destroy(CsvFile *file);
void csv_file_read(CsvFile *file, const char *filename);
void csv_file_write(const CsvFile *file, const char *filename);
void csv_file_append_row(CsvFile *file, CsvRow *row);
void csv_file_remove_row(CsvFile *file, size_t index);
void csv_print(const CsvFile *file);
void csv_file_insert_column(CsvFile *file, size_t colIndex, const CsvRow *colData); // Column Operations
void csv_file_set_header(CsvFile *file, CsvRow *header);
void csv_file_concatenate(CsvFile *file1, const CsvFile *file2); // Join and Merge Operations
void csv_row_destroy(CsvRow *row);
void csv_row_append_cell(CsvRow *row, const char *value);

int csv_row_get_cell_as_int(const CsvRow *row, size_t index); // Type Conversion
int csv_column_sum(const CsvFile *file, size_t columnIndex); // Data Aggregation

bool csv_validate_cell_format(const CsvRow *row, size_t index, const char *format); // Data Validation

#endif
