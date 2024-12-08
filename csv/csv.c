/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class CsvFile, CsvRow
*/

#include <stdlib.h> 
#include <string.h> 
#include "csv.h"
#include "../string/std_string.h"


/**
 * @brief Creates a new, empty CsvRow structure.
 *
 * This function allocates memory for a new CsvRow structure and initializes its members.
 *
 * @return Pointer to the newly created CsvRow structure.
 */
CsvRow* csv_row_create() {
    CSV_LOG("[csv_row_create]: Function start.");

    CsvRow* row = (CsvRow*) malloc(sizeof(CsvRow));
    if (!row) {
        CSV_LOG("[csv_row_create]: Error - Memory allocation failed.");
        exit(-1);
    }
    row->cells = NULL;
    row->size = 0;
    row->capacity = 0;

    CSV_LOG("[csv_row_create]: CsvRow created successfully.");
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
    CSV_LOG("[csv_row_destroy]: Function start.");
    if (!row) {
        CSV_LOG("[csv_row_destroy]: Error - Row object is NULL and invalid.");
        return;
    }

    for (size_t i = 0; i < row->size; ++i) {
        CSV_LOG("[csv_row_destroy]: Freeing cell %zu.", i);
        free(row->cells[i]);
    }

    CSV_LOG("[csv_row_destroy]: Freeing cells array.");
    free(row->cells); 

    CSV_LOG("[csv_row_destroy]: Freeing row object.");
    free(row);  

    CSV_LOG("[csv_row_destroy]: Function end.");
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
    CSV_LOG("[csv_row_append_cell]: Function start.");
    if (!row || !value) {
        CSV_LOG("[csv_row_append_cell]: Error - NULL parameter passed.");
        return;
    }

    // Resize the cells array if necessary
    if (row->size >= row->capacity) {
        size_t newCapacity = row->capacity == 0 ? 1 : row->capacity * 2;
        CSV_LOG("[csv_row_append_cell]: Resizing cells array to new capacity %zu.", newCapacity);

        char **newCells = (char **)realloc(row->cells, newCapacity * sizeof(char *));
        if (!newCells) {
            CSV_LOG("[csv_row_append_cell]: Error - Memory allocation failed for new cells.");
            return;
        }
        row->cells = newCells;
        row->capacity = newCapacity;
    }

    row->cells[row->size] = string_strdup(value); 
    if (!row->cells[row->size]) {
        CSV_LOG("[csv_row_append_cell]: Error - Memory allocation failed for cell value.");
        return;
    }

    CSV_LOG("[csv_row_append_cell]: Appended cell with value: %s", value);
    row->size++;
    CSV_LOG("[csv_row_append_cell]: Function end.");
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
    CSV_LOG("[csv_row_get_cell]: Function start.");
    if (row == NULL || index >= row->size) {
        CSV_LOG("[csv_row_get_cell]: Error - Invalid index or NULL row (index: %zu).", index);
        return NULL;
    }

    CSV_LOG("[csv_row_get_cell]: Returning value of cell at index %zu.", index);
    CSV_LOG("[csv_row_get_cell]: Function end.");

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
    CSV_LOG("[csv_file_create]: Function start.");

    CsvFile* file = (CsvFile*)malloc(sizeof(CsvFile));
    if (!file) {
        CSV_LOG("[csv_file_create]: Error - Memory allocation failed.");
        exit(-1);
    }

    file->rows = NULL;
    file->size = 0;
    file->capacity = 0;
    file->delimiter = delimiter;

    CSV_LOG("[csv_file_create]: CsvFile created successfully with delimiter '%c'.", delimiter);
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
    CSV_LOG("[csv_file_destroy]: Function start.");
    if (!file) {
        CSV_LOG("[csv_file_destroy]: Error - file object of CsvFile is NULL and invalid.");
        return;
    }

    for (size_t i = 0; i < file->size; ++i) {
        CSV_LOG("[csv_file_destroy]: Destroying row %zu.", i);
        csv_row_destroy(file->rows[i]);
    }

    CSV_LOG("[csv_file_destroy]: Freeing rows array.");
    free(file->rows);

    CSV_LOG("[csv_file_destroy]: Freeing file object.");
    free(file);
    CSV_LOG("[csv_file_destroy]: Function end.");
}

static void parse_csv_line(const char *line, char delimiter, CsvRow *row) {
    CSV_LOG("[parse_csv_line]: Function start.");
    if (!line || !row) {
        CSV_LOG("[parse_csv_line]: Error - NULL parameter passed.");
        return;
    }

    bool inQuotes = false;
    size_t start = 0;

    for (size_t i = 0; line[i] != '\0'; ++i) {
        if (line[i] == '"') {
            inQuotes = !inQuotes;
        } 
        else if (line[i] == delimiter && !inQuotes) {
            size_t len = i - start;
            char *cell = (char *)malloc(len + 1);

            if (!cell) {
                CSV_LOG("[parse_csv_line]: Error - Memory allocation failed for cell.");
                return; 
            }

            strncpy(cell, line + start, len);
            cell[len] = '\0';
            CSV_LOG("[parse_csv_line]: Adding cell: %s", cell);

            csv_row_append_cell(row, cell);
            free(cell);
            start = i + 1;
        }
    }

    CSV_LOG("[parse_csv_line]: Adding last cell.");
    char *cell = string_strdup(line + start);
    if (!cell) {
        CSV_LOG("[parse_csv_line]: Error - Memory allocation failed for last cell.");
        return; 
    }

    csv_row_append_cell(row, cell);
    free(cell);

    CSV_LOG("[parse_csv_line]: Function end.");
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
    CSV_LOG("[csv_file_read]: Function start.");
    if (!file || !filename) {
        CSV_LOG("[csv_file_read]: Error - NULL parameter passed.");
        return;
    }

    CSV_LOG("[csv_file_read]: Opening file '%s' for reading.", filename);
    FileReader* fr = file_reader_open(filename, READ_TEXT);
    if (!fr) {
        CSV_LOG("[csv_file_read]: Error - Unable to open file '%s'.", filename);
        return;
    }

    char buffer[BUFFER_SIZE] = {0};

    CSV_LOG("[csv_file_read]: Reading lines from file.");
    while (file_reader_read_line(buffer, BUFFER_SIZE, fr)) {
        CsvRow *row = csv_row_create();
        buffer[strcspn(buffer, "\r\n")] = 0; 
        CSV_LOG("[csv_file_read]: Read line: %s", buffer);

        parse_csv_line(buffer, file->delimiter, row);
        CSV_LOG("[csv_file_read]: Appending row to file.");
        csv_file_append_row(file, row);
    }

    CSV_LOG("[csv_file_read]: Closing file.");
    file_reader_close(fr);

    CSV_LOG("[csv_file_read]: Function end.");
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
    CSV_LOG("[csv_file_write]: Function start.");
    if (!file || !filename) {
        CSV_LOG("[csv_file_write]: Error - NULL parameter passed.");
        return;
    }

    CSV_LOG("[csv_file_write]: Opening file '%s' for writing.", filename);
    FileWriter* fw = file_writer_open(filename, WRITE_TEXT); 
    if (!fw) {
        CSV_LOG("[csv_file_write]: Error - Unable to open file '%s' for writing.", filename);
        return;
    }

    CSV_LOG("[csv_file_write]: Writing rows to file.");
    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        for (size_t j = 0; j < row->size; ++j) {
            file_writer_write_fmt(fw, "%s", row->cells[j]);
            if (j < row->size - 1)
                file_writer_write_fmt(fw, "%c", file->delimiter);
        }
        file_writer_write_fmt(fw, "\n");
        CSV_LOG("[csv_file_write]: Finished writing row %zu.", i);
    }

    CSV_LOG("[csv_file_write]: Closing file.");
    file_writer_close(fw);

    CSV_LOG("[csv_file_write]: Function end.");
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
    CSV_LOG("[csv_file_append_row]: Function start.");
    if (!file || !row) {
        CSV_LOG("[csv_file_append_row]: Error - NULL parameter passed.");
        return;
    }

    if (file->size >= file->capacity) {
        size_t newCapacity = file->capacity == 0 ? 1 : file->capacity * 2;
        CSV_LOG("[csv_file_append_row]: Resizing rows array to new capacity %zu.", newCapacity);
        CsvRow **newRows = (CsvRow**)realloc(file->rows, newCapacity * sizeof(CsvRow *));
        if (!newRows) {
            CSV_LOG("[csv_file_append_row]: Error - Unable to allocate memory for new rows.");
            return;
        }
        file->rows = newRows;
        file->capacity = newCapacity;
    }

    CSV_LOG("[csv_file_append_row]: Appending row at position %zu.", file->size);
    file->rows[file->size++] = row;

    CSV_LOG("[csv_file_append_row]: Function end.");
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
    CSV_LOG("[csv_file_get_row]: Function start.");

    if (!file || index >= file->size) {
        CSV_LOG("[csv_file_get_row]: Error - Invalid index %zu or null file.", index);
        return NULL;
    }

    CSV_LOG("[csv_file_get_row]: Returning row at index %zu.", index);
    return file->rows[index];
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
    CSV_LOG("[csv_file_remove_row]: Function start.");
    if (!file || index >= file->size) {
        CSV_LOG("[csv_file_remove_row]: Error - Invalid index or NULL file (index: %zu).", index);
        return;
    }

    CSV_LOG("[csv_file_remove_row]: Removing row at index %zu.", index);
    csv_row_destroy(file->rows[index]);  

    // Shift the remaining rows to fill the gap
    CSV_LOG("[csv_file_remove_row]: Shifting rows after index %zu.", index);
    for (size_t i = index; i < file->size - 1; i++) {
        file->rows[i] = file->rows[i + 1];
    }

    // Decrease the size of the file
    file->size--;
    CSV_LOG("[csv_file_remove_row]: Function end.");
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
    CSV_LOG("[csv_print]: Function start.");

    if (!file) {
        CSV_LOG("[csv_print]: Error - Null file pointer.");
        return;
    }

    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        CSV_LOG("[csv_print]: Printing row %zu.", i);

        for (size_t j = 0; j < row->size; ++j) {
            printf("%s", row->cells[j]);
            if (j < row->size - 1) { 
                printf("%c", file->delimiter);
            }
        }
        printf("\n");
    }

    CSV_LOG("[csv_print]: Function end.");
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
    CSV_LOG("[csv_row_read_next]: Function start.");

    if (!reader || !file_reader_is_open(reader)) {
        CSV_LOG("[csv_row_read_next]: Error - FileReader is null or not open.");
        return NULL;
    }

    char buffer[BUFFER_SIZE];
    if (!file_reader_read_line(buffer, BUFFER_SIZE, reader)) {
        CSV_LOG("[csv_row_read_next]: No more lines to read or error occurred.");
        return NULL; // No more lines to read or error occurred
    }

    buffer[strcspn(buffer, "\r\n")] = 0; 
    CSV_LOG("[csv_row_read_next]: Read line: %s", buffer);

    CsvRow *row = csv_row_create();
    char *token = strtok(buffer, &delimiter);

    while (token) {
        CSV_LOG("[csv_row_read_next]: Adding cell: %s", token);
        csv_row_append_cell(row, token);
        token = strtok(NULL, &delimiter);
    }

    CSV_LOG("[csv_row_read_next]: Function end.");
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
    CSV_LOG("[csv_file_insert_column]: Function start.");

    if (!file || !colData || colIndex > colData->size) {
        CSV_LOG("[csv_file_insert_column]: Error - Invalid parameters (file, colData, or colIndex).");
        return;
    }

    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        CSV_LOG("[csv_file_insert_column]: Processing row %zu.", i);
        char *cellValue = NULL;

        if (i < colData->size) {
            cellValue = string_strdup(colData->cells[i]);
            if (cellValue == NULL) {
                CSV_LOG("[csv_file_insert_column]: Error - Memory allocation failed for cell value in row %zu.", i);
                continue;
            }
        }

        if (row->size == row->capacity) {
            size_t newCapacity = row->capacity == 0 ? 1 : row->capacity * 2;
            CSV_LOG("[csv_file_insert_column]: Resizing row %zu cells to new capacity %zu.", i, newCapacity);
            char **newCells = (char**)realloc(row->cells, newCapacity * sizeof(char *));

            if (!newCells) {
                CSV_LOG("[csv_file_insert_column]: Error - Memory allocation failed for new cells in row %zu.", i);
                free(cellValue);
                continue;
            }
            row->cells = newCells;
            row->capacity = newCapacity;
        }

        // Shift cells to the right to make space for the new cell
        CSV_LOG("[csv_file_insert_column]: Shifting cells in row %zu to make space for the new cell at index %zu.", i, colIndex);
        for (size_t j = row->size; j > colIndex; --j) {
            row->cells[j] = row->cells[j - 1];
        }

        row->cells[colIndex] = cellValue; // Insert the new cell
        row->size++;

        CSV_LOG("[csv_file_insert_column]: Inserted new cell into row %zu at index %zu.", i, colIndex);
    }

    CSV_LOG("[csv_file_insert_column]: Function end.");
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
    CSV_LOG("[csv_file_get_header]: Function start.");

    if (!file || file->size == 0) {
        CSV_LOG("[csv_file_get_header]: Error - File is empty or null.");
        return NULL;
    }

    CSV_LOG("[csv_file_get_header]: Returning the first row as the header.");
    return file->rows[0]; 
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
    CSV_LOG("[csv_file_set_header]: Function start.");

    if (!file || !header) {
        CSV_LOG("[csv_file_set_header]: Error - Null parameters passed.");
        return;
    }

    if (file->size > 0) {
        CSV_LOG("[csv_file_set_header]: Destroying existing header row.");
        csv_row_destroy(file->rows[0]); // Free the existing header row before replacing
    } 
    else {
        CSV_LOG("[csv_file_set_header]: File is empty, checking capacity.");

        if (file->size == file->capacity) {
            size_t newCapacity = file->capacity == 0 ? 1 : file->capacity * 2;
            CSV_LOG("[csv_file_set_header]: Resizing rows array to new capacity %zu.", newCapacity);
            CsvRow **newRows = (CsvRow**)realloc(file->rows, newCapacity * sizeof(CsvRow *));

            if (!newRows) {
                CSV_LOG("[csv_file_set_header]: Error - Memory allocation failed for new rows.");
                return;
            }
            file->rows = newRows;
            file->capacity = newCapacity;
        }
    }

    CSV_LOG("[csv_file_set_header]: Setting new header row.");
    file->rows[0] = header;
    file->size = (file->size > 0) ? file->size : 1;

    CSV_LOG("[csv_file_set_header]: Function end.");
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
    CSV_LOG("[csv_row_get_cell_as_int]: Function start.");

    if (!row || index >= row->size) {
        CSV_LOG("[csv_row_get_cell_as_int]: Error - Invalid index or null row (index: %zu).", index);
        return 0;
    }

    CSV_LOG("[csv_row_get_cell_as_int]: Converting cell at index %zu to integer.", index);
    int value = atoi(row->cells[index]);

    CSV_LOG("[csv_row_get_cell_as_int]: Cell value: %d", value);
    CSV_LOG("[csv_row_get_cell_as_int]: Function end.");
    
    return value;
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
    CSV_LOG("[csv_file_find_rows]: Function start.");
    if (!file || !searchTerm) {
        CSV_LOG("[csv_file_find_rows]: Error - Null parameters passed.");
        return NULL;
    }

    size_t foundCount = 0;
    CSV_LOG("[csv_file_find_rows]: Allocating memory for foundRows.");
    CsvRow **foundRows = (CsvRow**)malloc((file->size + 1) * sizeof(CsvRow *)); 

    if (!foundRows) {
        CSV_LOG("[csv_file_find_rows]: Error - Memory allocation failed for foundRows.");
        return NULL;
    }

    for (size_t i = 0; i < file->size; ++i) {
        CsvRow* row = file->rows[i];
        CSV_LOG("[csv_file_find_rows]: Processing row %zu.", i);
        for (size_t j = 0; j < row->size; ++j) {
            CSV_LOG("[csv_file_find_rows]: Checking cell %zu in row %zu.", j, i);
            if (strstr(row->cells[j], searchTerm)) {
                CSV_LOG("[csv_file_find_rows]: Found match in row %zu, cell %zu.", i, j);
                foundRows[foundCount++] = row;
                break; // Break after finding the first occurrence in a row
            }
        }
    }

    CSV_LOG("[csv_file_find_rows]: Null-terminating foundRows array.");
    foundRows[foundCount] = NULL; 

    CSV_LOG("[csv_file_find_rows]: Resizing foundRows to match the exact size.");
    CsvRow **resizedFoundRows = (CsvRow**)realloc(foundRows, (foundCount + 1) * sizeof(CsvRow *));
    if (!resizedFoundRows) {
        CSV_LOG("[csv_file_find_rows]: Error - Memory allocation failed during resizing, returning original foundRows.");
        return foundRows; 
    }

    CSV_LOG("[csv_file_find_rows]: Function end.");
    return resizedFoundRows; 
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
    CSV_LOG("[csv_validate_cell_format]: Function start.");

    if (!row || !format || index >= row->size) {
        CSV_LOG("[csv_validate_cell_format]: Error - Invalid parameters.");
        return false;
    }

    CSV_LOG("[csv_validate_cell_format]: Validating cell at index %zu.", index);
    char* cell = row->cells[index];
    char buffer[256];

    CSV_LOG("[csv_validate_cell_format]: Formatting the cell value with the given format.");
    snprintf(buffer, sizeof(buffer), format, cell);

    CSV_LOG("[csv_validate_cell_format]: Comparing formatted cell value with actual cell value.");
    bool result = strcmp(cell, buffer) == 0;

    CSV_LOG("[csv_validate_cell_format]: Validation result: %s.", result ? "Valid" : "Invalid");
    CSV_LOG("[csv_validate_cell_format]: Function end.");
    
    return result;
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
    CSV_LOG("[csv_file_concatenate]: Function start.");

    if (!file1 || !file2) {
        CSV_LOG("[csv_file_concatenate]: Error - Null parameters provided.");
        return;
    }

    CSV_LOG("[csv_file_concatenate]: Concatenating file2 into file1.");
    for (size_t i = 0; i < file2->size; ++i) {
        CsvRow *row2 = file2->rows[i];
        CSV_LOG("[csv_file_concatenate]: Processing row %zu from file2.", i);

        CsvRow *newRow = csv_row_create();  
        CSV_LOG("[csv_file_concatenate]: Created new row for file1.");

        for (size_t j = 0; j < row2->size; ++j) {
            CSV_LOG("[csv_file_concatenate]: Appending cell %zu from row %zu.", j, i);
            csv_row_append_cell(newRow, row2->cells[j]);
        }

        CSV_LOG("[csv_file_concatenate]: Appending new row to file1.");
        csv_file_append_row(file1, newRow);  // Append the new row to file1
    }

    CSV_LOG("[csv_file_concatenate]: Function end.");
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
    CSV_LOG("[csv_column_sum]: Function start.");
    if (!file) {
        CSV_LOG("[csv_column_sum]: Error - Null file provided.");
        return 0;
    }

    CSV_LOG("[csv_column_sum]: Calculating sum for column %zu.", columnIndex);
    int sum = 0;

    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        CSV_LOG("[csv_column_sum]: Processing row %zu.", i);

        if (columnIndex < row->size) {
            char *cell = row->cells[columnIndex];
            int cellValue = atoi(cell);
            CSV_LOG("[csv_column_sum]: Adding value %d from row %zu, column %zu.", cellValue, i, columnIndex);
            sum += cellValue;
        } 
        else {
            CSV_LOG("[csv_column_sum]: Warning - Column index out of range in row %zu.", i);
        }
    }

    CSV_LOG("[csv_column_sum]: Final sum is %d.", sum);
    CSV_LOG("[csv_column_sum]: Function end.");
    
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
    CSV_LOG("[csv_export_to_json]: Function start.");
    if (!file) {
        CSV_LOG("[csv_export_to_json]: Error - Null CsvFile passed.");
        return NULL;
    }

    CSV_LOG("[csv_export_to_json]: Allocating memory for JSON export.");
    char *json = (char*)malloc(BUFFER_SIZE);
    if (!json) {
        CSV_LOG("[csv_export_to_json]: Error - Memory allocation failed.");
        return NULL;
    }

    CSV_LOG("[csv_export_to_json]: Initializing JSON string with '['.");
    strcpy(json, "[\n");

    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];

        CSV_LOG("[csv_export_to_json]: Processing row %zu.", i);
        strcat(json, "  {\n");
        
        for (size_t j = 0; j < row->size; ++j) {
            char *cell = row->cells[j];
            char line[128]; // Assuming each cell content and field name fit in 128 characters
            
            CSV_LOG("[csv_export_to_json]: Processing cell %zu in row %zu.", j, i);
            sprintf(line, "    \"field%zu\": \"%s\"%s\n", j, cell, j < row->size - 1 ? "," : "");
            strcat(json, line);
        }

        CSV_LOG("[csv_export_to_json]: Finished processing row %zu.", i);
        strcat(json, i < file->size - 1 ? "  },\n" : "  }\n");
    }

    CSV_LOG("[csv_export_to_json]: Finalizing JSON string with ']' and newline.");
    strcat(json, "]\n");

    CSV_LOG("[csv_export_to_json]: Function end.");
    return json;
}
