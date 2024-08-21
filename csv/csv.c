/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class CsvFile, CsvRow
*/

#include "csv.h"
#include "../fmt/fmt.h"
#include "../string/std_string.h"
#include <stdlib.h> // For dynamic memory allocation
#include <string.h> // For string manipulation

/**
 * @brief Creates a new, empty CsvRow structure.
 *
 * This function allocates memory for a new CsvRow structure and initializes its members.
 *
 * @return Pointer to the newly created CsvRow structure.
 */
CsvRow* csv_row_create() {
    CsvRow* row = malloc(sizeof(CsvRow));
    if (!row) {
        fmt_fprintf(stderr, "Error: Mamory allocation failed in csv_row_create.\n");
        exit(-1);
    }
    row->cells = NULL;
    row->size = 0;
    row->capacity = 0;

    return row;
}

/**
 * @brief Destroys a CsvRow structure and frees its memory.
 *
 * This function frees all the memory associated with the cells of the CsvRow,
 * as well as the memory for the CsvRow structure itself.
 *
 * @param row Pointer to the CsvRow structure to be destroyed.
 */
void csv_row_destroy(CsvRow *row) {
    if (!row) {
        fmt_fprintf(stderr, "Error: Row object of CsvRow is Null and Invalid in csv_row_destroy.\n");
        return;
    }

    // Free each cell string
    for (size_t i = 0; i < row->size; ++i) {
        free(row->cells[i]);
    }
    free(row->cells); // Free the array of cell pointers
    free(row);  // Free the CsvRow itself
}

/**
 * @brief Appends a cell to a CsvRow.
 *
 * This function adds a new cell with the specified value to the end of the CsvRow.
 * If necessary, it resizes the internal array of cells to accommodate the new cell.
 *
 * @param row Pointer to the CsvRow structure to which the cell will be appended.
 * @param value The string value of the cell to append.
 */
void csv_row_append_cell(CsvRow *row, const char *value) {
    if (!row || !value) {
        fmt_fprintf(stderr, "Error: NULL parameter passed to csv_row_append_cell.\n");
        return;
    }

    // Resize the cells array if necessary
    if (row->size >= row->capacity) {
        size_t newCapacity = row->capacity == 0 ? 1 : row->capacity * 2;
        char **newCells = realloc(row->cells, newCapacity * sizeof(char *));

        if (!newCells) {
            fmt_fprintf(stderr, "Error: Memory allocation failed for new cells in csv_row_append_cell.\n");
            return;
        }
        row->cells = newCells;
        row->capacity = newCapacity;
    }

    row->cells[row->size] = string_strdup(value); // Duplicate the string to store in the cells array

    if (!row->cells[row->size]) {
        fmt_fprintf(stderr, "Error: Unable to allocate memory for cell value in csv_row_append_cell.\n");
        return;
    }
    row->size++;
}

/**
 * @brief Retrieves the value of a cell at a specified index in a CsvRow.
 *
 * This function returns a pointer to the string value of the cell at the specified index in the CsvRow.
 * If the index is out of bounds or the row is null, it returns NULL.
 *
 * @param row Pointer to the CsvRow structure.
 * @param index Index of the cell to retrieve.
 * 
 * @return Pointer to the cell value, or NULL if the index is invalid or the row is null.
 */
char* csv_row_get_cell(const CsvRow *row, size_t index) {
    if (row == NULL || index >= row->size) {
        fmt_fprintf(stderr, "Error: Invalid index or null row in csv_row_get_cell.\n");
        return NULL;
    }
    return row->cells[index];
}

/**
 * @brief Creates a new CsvFile structure with a specified delimiter.
 *
 * This function allocates memory for a new CsvFile structure and initializes its members,
 * including setting the delimiter character.
 *
 * @param delimiter The delimiter character to use for separating fields in the CSV file.
 * @return Pointer to the newly created CsvFile structure.
 */
CsvFile* csv_file_create(char delimiter) {
    CsvFile* file = malloc(sizeof(CsvFile));
    if (!file) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in csv_file_create.\n");
        exit(-1);
    }
    file->rows = NULL;
    file->size = 0;
    file->capacity = 0;
    file->delimiter = delimiter;

    return file;
}

/**
 * @brief Destroys a CsvFile structure and frees its memory.
 *
 * This function frees all the memory associated with the rows of the CsvFile,
 * as well as the memory for the CsvFile structure itself.
 *
 * @param file Pointer to the CsvFile structure to be destroyed.
 */
void csv_file_destroy(CsvFile *file) {
    if (!file) {
        fmt_fprintf(stderr, "Error: file object of CsvFile is Null and Invalid in csv_file_destroy.\n");
        return;
    }
    for (size_t i = 0; i < file->size; ++i) {
        csv_row_destroy(file->rows[i]);
    }

    free(file->rows);
    free(file);
}

static void parse_csv_line(const char *line, char delimiter, CsvRow *row) {
    if (!line || !row) {
        fmt_fprintf(stderr, "Error: NULL parameter passed to parse_csv_line.\n");
        return;
    }

    bool inQuotes = false;
    size_t start = 0;

    for (size_t i = 0; line[i] != '\0'; ++i) {
        if (line[i] == '"') 
            inQuotes = !inQuotes;
        else if (line[i] == delimiter && !inQuotes) {
            size_t len = i - start;
            char *cell = (char *)malloc(len + 1);
            if (!cell) {
                fmt_fprintf(stderr, "Error: Memory allocation failed in parse_csv_line.\n");
                return; // Memory allocation error
            }

            strncpy(cell, line + start, len);
            cell[len] = '\0';
            csv_row_append_cell(row, cell);
            free(cell);
            start = i + 1;
        }
    }
    // Add the last cell
    char *cell = string_strdup(line + start);
    if (!cell) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in parse_csv_line for last cell.\n");
        return; // Memory allocation error
    }

    csv_row_append_cell(row, cell);
    free(cell);
}

/**
 * @brief Reads the contents of a CSV file into a CsvFile structure.
 *
 * This function reads the specified CSV file line by line, parses each line into a CsvRow,
 * and appends it to the CsvFile structure. It uses the specified delimiter to separate the fields.
 *
 * @param file Pointer to the CsvFile structure where the file contents will be stored.
 * @param filename Name of the CSV file to read.
 */
void csv_file_read(CsvFile *file, const char *filename) {
    if (!file || !filename) {
        fmt_fprintf(stderr, "Error: NULL parameter passed to csv_file_read.\n");
        return;
    }

    FileReader* fr = file_reader_open(filename, READ_TEXT);
    if (!fr) {
        fmt_fprintf(stderr, "Error: Unable to open file '%s' in csv_file_read.\n", filename);
        return;
    }

    char buffer[BUFFER_SIZE] = {0};

    while (file_reader_read_line(buffer, BUFFER_SIZE, fr)) {
        CsvRow *row = csv_row_create();
        buffer[strcspn(buffer, "\r\n")] = 0; // Remove newline character
        parse_csv_line(buffer, file->delimiter, row);
        csv_file_append_row(file, row);
    }

    file_reader_close(fr);
}

/**
 * @brief Writes the contents of a CSV file to a specified file.
 *
 * This function takes a CsvFile object and writes its contents to a file
 * specified by the filename. Each row in the CSV file is written to the file,
 * with cells separated by the specified delimiter.
 *
 * @param file The CsvFile object to write to a file.
 * @param filename The name of the file where the CSV data will be written.
 */
void csv_file_write(const CsvFile *file, const char *filename) {
    if (!file || !filename) {
        fmt_fprintf(stderr, "Error: NULL parameter passed to csv_file_write.\n");
        return;
    }
    FileWriter* fw = file_writer_open(filename, WRITE_UNICODE); // Use WRITE_UNICODE for unicode support

    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        for (size_t j = 0; j < row->size; ++j) {
            file_writer_write_fmt(fw, "%s", row->cells[j]);
            if (j < row->size - 1)
                file_writer_write_fmt(fw, "%c", file->delimiter);
        }
        char newline[] = "\n";
        file_writer_write_line(newline, 1, fw); // Corrected call
    }
    file_writer_close(fw);
}

/**
 * @brief Appends a new row to a CSV file.
 *
 * This function adds a new row to the end of a CsvFile. If the file's capacity is
 * insufficient, it automatically resizes the internal storage to accommodate the new row.
 *
 * @param file The CsvFile to which the row will be appended.
 * @param row The CsvRow to append to the file.
 */
void csv_file_append_row(CsvFile *file, CsvRow *row) {
    if (!file || !row) {
        fmt_fprintf(stderr, "Error: NULL parameter passed to csv_file_append_row.\n");
        return;
    }

    if (file->size >= file->capacity) {
        size_t newCapacity = file->capacity == 0 ? 1 : file->capacity * 2;
        CsvRow **newRows = realloc(file->rows, newCapacity * sizeof(CsvRow *));
        if (!newRows) {
            fmt_fprintf(stderr, "Error: Unable to allocate memory for new rows in csv_file_append_row.\n");
            return;
        }
        file->rows = newRows;
        file->capacity = newCapacity;
    }
    file->rows[file->size++] = row;
}

/**
 * @brief Retrieves a specific row from a CSV file by index.
 *
 * This function returns a pointer to the row at the specified index within the CsvFile.
 * If the index is out of bounds or the file is null, it returns NULL.
 *
 * @param file The CsvFile from which to retrieve the row.
 * @param index The index of the row to retrieve.
 * @return A pointer to the requested CsvRow, or NULL if the index is invalid or the file is null.
 */
CsvRow* csv_file_get_row(const CsvFile *file, size_t index) {
    if (!file || index >= file->size) {
        fmt_fprintf(stderr, "Error: Invalid index or null file in csv_file_get_row.\n");
        return NULL;
    }
    return file->rows[index]; // Retrieve the row at the specified index
}

/**
 * @brief Removes a row from a CSV file at the specified index.
 *
 * This function removes the row at the given index from the CsvFile. The subsequent
 * rows are shifted to fill the gap left by the removed row. The size of the file is
 * decremented accordingly.
 *
 * @param file The CsvFile from which to remove the row.
 * @param index The index of the row to remove.
 */
void csv_file_remove_row(CsvFile *file, size_t index) {
    if (!file || index >= file->size) {
        fmt_fprintf(stderr, "Error: Invalid index or null file in csv_file_remove_row.\n");
        return;
    }
    csv_row_destroy(file->rows[index]);   // Free the memory allocated for the row to be removed

    // Shift the remaining rows to fill the gap
    for (size_t i = index; i < file->size - 1; i++) {
        file->rows[i] = file->rows[i + 1];
    }
    // Decrease the size of the file
    file->size--;
}

/**
 * @brief Prints the contents of a CSV file to the standard output.
 *
 * This function iterates through each row and cell of the CsvFile and prints the content
 * to the standard output, with cells separated by the delimiter.
 *
 * @param file The CsvFile to print.
 */
void csv_print(const CsvFile *file) {
    if (!file) {
        fmt_fprintf(stderr, "Error: Null file pointer in csv_print.\n");
        return;
    }

    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];

        for (size_t j = 0; j < row->size; ++j) {
            fmt_printf("%s", row->cells[j]);
            if (j < row->size - 1) { 
                fmt_printf("%c", file->delimiter);
            }
        }
        fmt_printf("\n");
    }
}

/**
 * @brief Reads the next row from a file using a FileReader object.
 *
 * This function reads the next line from the file associated with the given FileReader object,
 * splits it into cells based on the specified delimiter, and returns the resulting CsvRow.
 * 
 * @param reader The FileReader object used to read the file.
 * @param delimiter The delimiter character used to split the row into cells.
 * @return A pointer to a CsvRow containing the parsed cells, or NULL if an error occurs or EOF is reached.
 */
CsvRow* csv_row_read_next(FileReader *reader, char delimiter) {
    if (!reader || !file_reader_is_open(reader)) {
        fmt_fprintf(stderr, "Error: FileReader is null or not open in csv_row_read_next.\n");
        return NULL;
    }

    char buffer[BUFFER_SIZE];
    if (!file_reader_read_line(buffer, BUFFER_SIZE, reader)) {
        return NULL; // No more lines to read or error occurred
    }

    buffer[strcspn(buffer, "\r\n")] = 0; // Remove newline character

    CsvRow *row = csv_row_create();
    char *token = strtok(buffer, &delimiter);

    while (token) {
        csv_row_append_cell(row, token);
        token = strtok(NULL, &delimiter);
    }
    return row;
}

/**
 * @brief Inserts a new column into a CSV file at the specified index.
 *
 * This function inserts a column into each row of the CSV file at the specified column index.
 * The values of the new column are taken from the `colData` row. If a row in the CSV file does not have 
 * a corresponding value in `colData`, a NULL value is inserted.
 *
 * @param file The CsvFile in which the column is to be inserted.
 * @param colIndex The index at which the new column should be inserted.
 * @param colData The CsvRow containing the data for the new column.
 */
void csv_file_insert_column(CsvFile *file, size_t colIndex, const CsvRow *colData) {
    if (!file || !colData || colIndex > colData->size) {
        fmt_fprintf(stderr, "Error: Invalid parameters in csv_file_insert_column.\n");
        return;
    }

    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        char *cellValue = NULL;

        if (i < colData->size) {
            cellValue = string_strdup(colData->cells[i]); // Duplicate the cell value
            if (cellValue == NULL) {
                fmt_fprintf(stderr, "Error: Unable to allocate memory for cell value in csv_file_insert_column.\n");
                continue;
            }
        }

        // Check if we need to resize the row's cells array
        if (row->size == row->capacity) {
            size_t newCapacity = row->capacity == 0 ? 1 : row->capacity * 2;
            char **newCells = realloc(row->cells, newCapacity * sizeof(char *));

            if (newCells == NULL) {
                fmt_fprintf(stderr, "Error: Unable to allocate memory for new cells in csv_file_insert_column.\n");
                free(cellValue);
                continue;
            }
            row->cells = newCells;
            row->capacity = newCapacity;
        }

        // Shift cells to the right to make space for the new cell
        for (size_t j = row->size; j > colIndex; --j) {
            row->cells[j] = row->cells[j - 1];
        }
        row->cells[colIndex] = cellValue; // Insert the new cell
        row->size++;
    }
}

/**
 * @brief Retrieves the header row from a CSV file.
 *
 * This function returns the first row of the CSV file, which is typically considered the header.
 * If the file is empty or null, an error is reported and NULL is returned.
 *
 * @param file The CsvFile from which to retrieve the header.
 * @return The header row as a CsvRow pointer, or NULL if the file is empty or invalid.
 */
CsvRow* csv_file_get_header(const CsvFile *file) {
    if (!file || file->size == 0) {
        fmt_fprintf(stderr, "Error: File is empty or null in csv_file_get_header.\n");
        return NULL;
    }
    return file->rows[0]; // Return the first row as the header
}

/**
 * @brief Sets a new header row for a CSV file.
 *
 * This function replaces the existing header row of the CSV file with a new header.
 * If the CSV file has no rows, the header is inserted as the first row.
 *
 * @param file The CsvFile in which to set the header.
 * @param header The CsvRow to set as the header.
 */
void csv_file_set_header(CsvFile *file, CsvRow *header) {
    if (!file || !header) {
        fmt_fprintf(stderr, "Error: Null parameters passed to csv_file_set_header.\n");
        return;
    }

    if (file->size > 0) {
        csv_row_destroy(file->rows[0]); // Free the existing header row before replacing
    }
    else {
        // Increase the array size if there's no space
        if (file->size == file->capacity) {
            size_t newCapacity = file->capacity == 0 ? 1 : file->capacity * 2;
            CsvRow **newRows = realloc(file->rows, newCapacity * sizeof(CsvRow *));
            if (!newRows) {
                fmt_fprintf(stderr, "Error: Unable to allocate memory for new rows in csv_file_set_header.\n");
                return;
            }
            file->rows = newRows;
            file->capacity = newCapacity;
        }
    }

    // Set the new header
    file->rows[0] = header;
    file->size = (file->size > 0) ? file->size : 1;
}

/**
 * @brief Retrieves a cell value from a CSV row as an integer.
 *
 * This function returns the value of a specific cell in a row, converted to an integer.
 * If the index is out of bounds or the row is null, an error is reported and 0 is returned.
 *
 * @param row The CsvRow from which to retrieve the cell value.
 * @param index The index of the cell within the row.
 * 
 * @return The integer value of the cell, or 0 if an error occurs.
 */
int csv_row_get_cell_as_int(const CsvRow *row, size_t index) {
    if (!row || index >= row->size) {
        fmt_fprintf(stderr, "Error: Invalid index or null row in csv_row_get_cell_as_int.\n");
        return 0;
    }
    return atoi(row->cells[index]); // Convert the string to an integer
}

/**
 * @brief Finds rows in a CSV file that contain a specific search term.
 *
 * This function searches through all rows in a CSV file to find those that contain the specified search term.
 * It returns an array of pointers to the matching rows. The array is null-terminated.
 *
 * @param file The CsvFile to search.
 * @param searchTerm The term to search for in the file's rows.
 * 
 * @return An array of pointers to the rows containing the search term, or NULL if no matches are found.
 */
CsvRow** csv_file_find_rows(const CsvFile *file, const char* searchTerm) {
    if (!file || !searchTerm) {
        fmt_fprintf(stderr, "Error: Null parameters passed to csv_file_find_rows.\n");
        return NULL;
    }

    size_t foundCount = 0;
    CsvRow **foundRows = malloc((file->size + 1) * sizeof(CsvRow *)); // Allocate one extra for null termination
    if (!foundRows) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for foundRows in csv_file_find_rows.\n");
        return NULL;
    }

    for (size_t i = 0; i < file->size; ++i) {
        CsvRow* row = file->rows[i];
        for (size_t j = 0; j < row->size; ++j) {
            if (strstr(row->cells[j], searchTerm)) {
                foundRows[foundCount++] = row;
                break; // Break after finding the first occurrence in a row
            }
        }
    }
    foundRows[foundCount] = NULL; // Null-terminate the array

    // Optional: resize foundRows to match foundCount + 1 for null termination
    CsvRow **resizedFoundRows = realloc(foundRows, (foundCount + 1) * sizeof(CsvRow *));
    if (!resizedFoundRows) {
        // If realloc fails, foundRows is still valid and contains the data
        fmt_fprintf(stderr, "Error: Memory allocation failed for resizedFoundRows in csv_file_find_rows.\n");
        return foundRows; // Return the original foundRows to avoid memory leak
    }
    return resizedFoundRows; // Return the array of found rows
}

/**
 * @brief Validates the format of a specific cell in a CSV row.
 *
 * This function checks if the content of a specified cell in a row matches
 * the given format string. It uses the format to create a formatted string
 * and compares it to the actual cell content.
 *
 * @param row The CsvRow containing the cell to be validated.
 * @param index The index of the cell in the row.
 * @param format The format string to validate against.
 * 
 * @return True if the cell content matches the format, false otherwise.
 */
bool csv_validate_cell_format(const CsvRow *row, size_t index, const char *format) {
    if (!row || !format || index >= row->size) {
        fmt_fprintf(stderr, "Error: Invalid parameters in csv_validate_cell_format.\n");
        return false;
    }
    char* cell = row->cells[index];
    char buffer[256]; // Assuming a buffer size sufficient for formatted string
    
    snprintf(buffer, sizeof(buffer), format, cell);
    return strcmp(cell, buffer) == 0;
}

/**
 * @brief Concatenates the rows of one CSV file into another.
 *
 * This function appends all rows from one CSV file (file2) to another CSV file (file1).
 * The rows are copied, ensuring that the original rows in file2 are not modified.
 *
 * @param file1 The target CsvFile where rows will be appended.
 * @param file2 The source CsvFile from which rows will be copied.
 */
void csv_file_concatenate(CsvFile *file1, const CsvFile *file2) {
    if (!file1 || !file2) {
        fmt_fprintf(stderr, "Error: Null parameters in csv_file_concatenate.\n");
        return;
    }

    for (size_t i = 0; i < file2->size; ++i) {
        CsvRow *row2 = file2->rows[i];
        CsvRow *newRow = csv_row_create();   // Create a new row for file1 and copy each cell of row2 into it
        
        for (size_t j = 0; j < row2->size; ++j) { 
            csv_row_append_cell(newRow, row2->cells[j]);
        }
        csv_file_append_row(file1, newRow);    // Append the new row to file1
    }
}

/**
 * @brief Sums the values of a specific column in a CSV file.
 *
 * This function calculates the sum of all the integer values in a specified column
 * of a CSV file. It assumes that the column contains valid integers.
 *
 * @param file The CsvFile containing the data.
 * @param columnIndex The index of the column to sum.
 * 
 * @return The sum of the integer values in the specified column.
 */
int csv_column_sum(const CsvFile *file, size_t columnIndex) {
    if (!file) {
        fmt_fprintf(stderr, "Error: File is NULL in csv_column_sum.\n");
        return 0;
    }

    int sum = 0;
    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        if (columnIndex < row->size) {
            char *cell = row->cells[columnIndex];
            int cellValue = atoi(cell);
            sum += cellValue;
        }
        else {
            fmt_fprintf(stderr, "Warning: Column index out of range in row %zu.\n", i);
        }
    }
    return sum;
}

/**
 * @brief Exports a CSV file to a JSON format string.
 *
 * This function converts the data from a CsvFile into a JSON formatted string.
 * Each row in the CSV file is represented as a JSON object, with field names
 * generated based on the column index.
 *
 * @param file The CsvFile to be exported.
 * 
 * @return A string containing the JSON representation of the CSV file.
 * The caller is responsible for freeing the allocated memory.
 */
char* csv_export_to_json(const CsvFile *file) {
    if (!file) {
        fmt_fprintf(stderr, "Error: Null CsvFile passed to csv_export_to_json.\n");
        return NULL;
    }

    char *json = malloc(BUFFER_SIZE);
    if (!json) {
        fmt_fprintf(stderr, "Error: Memory allocation failed for JSON export in csv_export_to_json.\n");
        return NULL;
    }

    strcpy(json, "[\n");
    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        strcat(json, "  {\n");
        
        for (size_t j = 0; j < row->size; ++j) {
            char *cell = row->cells[j];
            char line[128]; // Assuming each cell content and field name fit in 128 characters
            sprintf(line, "    \"field%zu\": \"%s\"%s\n", j, cell, j < row->size - 1 ? "," : "");
            strcat(json, line);
        }
        strcat(json, i < file->size - 1 ? "  },\n" : "  }\n");
    }

    strcat(json, "]\n");
    return json;
}
