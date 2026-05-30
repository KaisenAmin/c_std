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
        return NULL;
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
        return NULL;
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
 * Each row is emitted as `cell<delim>cell<delim>...<delim>cell\n`. The
 * file is opened in BINARY mode (`"wb"`) so the output is byte-identical
 * across platforms — no C-runtime CRLF translation on Windows. This also
 * means `csv_file_write` and `csv_file_export_to_string` produce
 * **byte-equal** output, so disk and in-memory round-trips behave the
 * same everywhere.
 *
 * @param file     The CsvFile object to write to a file.
 * @param filename The name of the file where the CSV data will be written.
 */
void csv_file_write(const CsvFile *file, const char *filename) {
    CSV_LOG("[csv_file_write]: Function start.");
    if (!file || !filename) {
        CSV_LOG("[csv_file_write]: Error - NULL parameter passed.");
        return;
    }

    CSV_LOG("[csv_file_write]: Opening file '%s' for writing (binary).", filename);

    FileWriter* fw = file_writer_open(filename, WRITE_BINARY);
    if (!fw) {
        CSV_LOG("[csv_file_write]: Error - Unable to open file '%s' for writing.", filename);
        return;
    }

    CSV_LOG("[csv_file_write]: Writing rows to file.");
    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        for (size_t j = 0; j < row->size; ++j) {
            file_writer_write_fmt(fw, "%s", row->cells[j]);
            if (j < row->size - 1) {
                file_writer_write_fmt(fw, "%c", file->delimiter);
            }
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
    if (!row) {
        return NULL;
    }
    parse_csv_line(buffer, delimiter, row);

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

    {
        const char* p = format;
        int percent_count = 0;
        bool has_other = false;

        while (*p) {
            if (*p == '%') {
                ++p;
                if (*p == '%') { 
                    ++p; 
                    continue; 
                }  
                while (*p && (*p == '-' || *p == '+' || *p == ' ' || *p == '#' || *p == '0' || (*p >= '0' && *p <= '9') || *p == '.')) {
                    ++p; 
                }
                if (*p != 's') {
                    has_other = true;
                    break;
                }
                ++percent_count;
            }
            if (*p) {
                ++p;
            }
        }
        if (has_other || percent_count != 1) {
            CSV_LOG("[csv_validate_cell_format]: Format must contain exactly one %%s specifier.");
            return false;
        }
    }

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
/* Helper: append `s` to the end of *json (cap is its current allocation,
 * len is current strlen). Grows *json on demand. Returns false on OOM. */
static bool json_append(char** json, size_t* cap, size_t* len, const char* s) {
    size_t slen = strlen(s);
    if (*len + slen + 1 > *cap) {
        size_t new_cap = (*len + slen + 1) * 2;
        char* new_json = (char*)realloc(*json, new_cap);

        if (!new_json) {
            return false;
        }

        *json = new_json;
        *cap = new_cap;
    }
    memcpy(*json + *len, s, slen + 1);  // include terminator
    *len += slen;
    return true;
}

/**
 * @brief Serialize a CsvFile to a JSON array string.
 *
 * Produces a JSON array of objects: every CsvRow becomes one object,
 * and each cell within that row is exposed as `"field<n>": "<value>"`
 * where `<n>` is the zero-based column index. The output is indented
 * for readability and terminated with a newline.
 *
 * Example output for a 2-row, 3-column file:

 * @note Column names are positional — `field0`, `field1`, ... —
 *       not derived from a header row. If you want the header's cell
 *       values as JSON keys, transform the result yourself with the
 *       `json` library, or pre-process the CsvFile first.
 *
 * @note Cell values are inserted verbatim into the JSON string. If a
 *       cell contains a literal `"` or `\\`, the output will not be
 *       strictly RFC 8259 compliant. Quote and escape your input
 *       upstream if you need bulletproof JSON.
 *
 * @warning The returned string is heap-allocated. The caller takes
 *          ownership and MUST release it with `free()`.
 *
 * @param file Source CsvFile. May not be NULL.
 *
 * @return Newly-allocated NUL-terminated JSON string on success;
 *         NULL if @p file is NULL or any internal allocation failed.
 *
 * @code
 * CsvFile* f = csv_file_create(',');
 * csv_file_read(f, "people.csv");
 *
 * char* j = csv_export_to_json(f);
 * if (j) {
 *     puts(j);
 *     free(j);
 * }
 * csv_file_destroy(f);
 * @endcode
 */
char* csv_export_to_json(const CsvFile *file) {
    CSV_LOG("[csv_export_to_json]: Function start.");
    if (!file) {
        CSV_LOG("[csv_export_to_json]: Error - Null CsvFile passed.");
        return NULL;
    }

    size_t cap = 256;
    size_t len = 0;
    char* json = (char*)malloc(cap);
    if (!json) {
        return NULL;
    }
    json[0] = '\0';

    if (!json_append(&json, &cap, &len, "[\n")) { free(json); return NULL; }

    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        if (!json_append(&json, &cap, &len, "  {\n")) { 
            free(json); 
            return NULL; 
        }

        for (size_t j = 0; j < row->size; ++j) {
            const char *cell = row->cells[j];
            // Size each line precisely: field name (up to 32 digits) + cell length + 12 fixed chars.
            size_t cell_len = strlen(cell);
            size_t line_cap = cell_len + 64;
            char *line = (char*)malloc(line_cap);
            if (!line) { 
                free(json); 
                return NULL; 
            }
            snprintf(line, line_cap, "    \"field%zu\": \"%s\"%s\n", j, cell, (j < row->size - 1) ? "," : "");
            bool ok = json_append(&json, &cap, &len, line);
            free(line);

            if (!ok) { 
                free(json); 
                return NULL; 
            }
        }
        if (!json_append(&json, &cap, &len, (i < file->size - 1) ? "  },\n" : "  }\n")) {
            free(json);
            return NULL;
        }
    }

    if (!json_append(&json, &cap, &len, "]\n")) { 
        free(json); 
        return NULL; 
    }

    CSV_LOG("[csv_export_to_json]: Function end.");
    return json;
}


/**
 * @brief Parse a CSV document held in memory.
 *
 * Identical semantics to `csv_file_read`, but reads from a NUL-terminated
 * string instead of a file. Splits on `\n`, with trailing `\r` (from
 * CRLF) tolerated. Quoted cells containing the delimiter are honored
 * via the existing `parse_csv_line` helper. Rows are APPENDED to `file`,
 * so calling this twice with two strings concatenates them.
 *
 * NULL inputs and the empty string are safe no-ops.
 *
 * @param file Target CsvFile (already created via `csv_file_create`).
 * @param data NUL-terminated CSV text.
 */
void csv_file_load_from_string(CsvFile *file, const char *data) {
    CSV_LOG("[csv_file_load_from_string]: Function start.");
    if (!file || !data) {
        CSV_LOG("[csv_file_load_from_string]: NULL input; no-op.");
        return;
    }
    const char *p = data;
    while (*p) {
        const char *line_end = strchr(p, '\n');
        size_t line_len = line_end ? (size_t)(line_end - p) : strlen(p);

        /* Skip wholly-empty lines (matches csv_file_read which also
           appends an empty row only when the line had content). */
        if (line_len == 0) {
            if (!line_end) {
                break;
            }
            p = line_end + 1;
            continue;
        }

        /* Allocate a temporary buffer just big enough for this line. */
        char *line = (char*)malloc(line_len + 1);
        if (!line) {
            CSV_LOG("[csv_file_load_from_string]: malloc(%zu) failed.", line_len + 1);
            return;
        }
        memcpy(line, p, line_len);
        line[line_len] = '\0';

        /* Trim trailing \r left over from CRLF line endings. */
        if (line_len > 0 && line[line_len - 1] == '\r') {
            line[line_len - 1] = '\0';
        }

        CsvRow *row = csv_row_create();
        if (row) {
            parse_csv_line(line, file->delimiter, row);
            csv_file_append_row(file, row);
        }
        free(line);

        if (!line_end) {
            break;
        }
        p = line_end + 1;
    }
    CSV_LOG("[csv_file_load_from_string]: parsed %zu row(s) (cumulative).", file->size);
}


/**
 * @brief Find the index of the column whose header equals `columnName`.
 *
 * The header is the first row of `file`. Comparison is case-sensitive
 * and exact.
 *
 * @return The column index on match, or -1 if there's no header, no
 *         match, or `file` / `columnName` is NULL.
 */
int csv_file_find_column_index(const CsvFile *file, const char *columnName) {
    CSV_LOG("[csv_file_find_column_index]: Function start.");

    if (!file || !columnName || file->size == 0) {
        CSV_LOG("[csv_file_find_column_index]: missing file/name/header; returning -1.");
        return -1;
    }

    CsvRow *header = file->rows[0];
    for (size_t i = 0; i < header->size; ++i) {
        if (header->cells[i] && strcmp(header->cells[i], columnName) == 0) {
            CSV_LOG("[csv_file_find_column_index]: '%s' found at index %zu.", columnName, i);
            return (int)i;
        }
    }

    CSV_LOG("[csv_file_find_column_index]: '%s' not found.", columnName);
    return -1;
}


/**
 * @brief Fetch a cell by COLUMN NAME using `file`'s header row.
 *
 * Equivalent to `csv_row_get_cell(row, csv_file_find_column_index(file, name))`,
 * but with proper NULL handling at each step. Returns NULL if the file
 * has no header, the column isn't present, or the row is shorter than
 * the resolved column index. The returned pointer is borrowed (owned by
 * `row`) — do not free it.
 */
char* csv_row_get_cell_by_name(const CsvFile *file, const CsvRow *row, const char *columnName) {
    CSV_LOG("[csv_row_get_cell_by_name]: Function start.");
    if (!file || !row || !columnName) {
        CSV_LOG("[csv_row_get_cell_by_name]: NULL input.");
        return NULL;
    }

    int idx = csv_file_find_column_index(file, columnName);
    if (idx < 0) {
        CSV_LOG("[csv_row_get_cell_by_name]: column '%s' not in header.", columnName);
        return NULL;
    }
    if ((size_t)idx >= row->size) {
        CSV_LOG("[csv_row_get_cell_by_name]: row too short for column index %d.", idx);
        return NULL;
    }

    return row->cells[idx];
}


/**
 * @brief Extract one column as a NULL-terminated array of borrowed strings.
 *
 * The returned array is malloc'd and ends in a NULL sentinel. Each entry
 * points into one of the source rows — do NOT free the individual
 * strings; just `free()` the array itself when done. Rows shorter than
 * `columnIndex` contribute a NULL entry (count is still incremented),
 * so the array index aligns with the row index.
 *
 * @param file        Source file.
 * @param columnIndex Index of the column to extract.
 * @param outCount    Receives the number of entries (excluding the
 *                    trailing NULL). May be NULL.
 * @param skipHeader  If true, the first row (header) is excluded.
 * @return Newly-allocated NULL-terminated array, or NULL on bad input.
 */
char** csv_file_get_column(const CsvFile *file, size_t columnIndex, size_t *outCount, bool skipHeader) {
    CSV_LOG("[csv_file_get_column]: Function start.");
    if (outCount) {
        *outCount = 0;
    }
    if (!file) {
        CSV_LOG("[csv_file_get_column]: NULL file.");
        return NULL;
    }
    size_t start = (skipHeader && file->size > 0) ? 1 : 0;
    size_t n = (file->size > start) ? (file->size - start) : 0;

    char **out = (char**)malloc((n + 1) * sizeof(char*));
    if (!out) {
        CSV_LOG("[csv_file_get_column]: malloc failed.");
        return NULL;
    }

    size_t k = 0;
    for (size_t i = start; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        out[k++] = (row && columnIndex < row->size) ? row->cells[columnIndex] : NULL;
    }
    out[k] = NULL;
    if (outCount) { 
        *outCount = k;
    }

    CSV_LOG("[csv_file_get_column]: column %zu has %zu entries (skipHeader=%d).", columnIndex, k, (int)skipHeader);
    return out;
}


/**
 * @brief Drop a column from every row in the file.
 *
 * Symmetric to `csv_file_insert_column`. Frees the removed cell strings
 * and shifts subsequent cells left within each row. Rows that don't
 * reach `columnIndex` are left untouched (so this is safe on jagged
 * CSVs). NULL file and out-of-range index are silent no-ops.
 */
void csv_file_remove_column(CsvFile *file, size_t columnIndex) {
    CSV_LOG("[csv_file_remove_column]: Function start.");
    if (!file) {
        CSV_LOG("[csv_file_remove_column]: NULL file.");
        return;
    }
    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        if (!row || columnIndex >= row->size) {
            continue;
        }
        free(row->cells[columnIndex]);

        for (size_t j = columnIndex; j + 1 < row->size; ++j) {
            row->cells[j] = row->cells[j + 1];
        }
        row->size--;
    }
    CSV_LOG("[csv_file_remove_column]: removed column %zu from %zu row(s).", columnIndex, file->size);
}


/* qsort thunk for csv_file_sort — read the index + direction from a
   small fixed struct passed via the (POSIX) qsort_s isn't portable, so
   we use file-static state instead. Not thread-safe, but neither is the
   rest of this library. */
static size_t g_sort_col   = 0;
static int    g_sort_sign  = 1;   /* +1 ascending, -1 descending */

static int csv_sort_cmp(const void *a, const void *b) {
    CsvRow * const *pa = (CsvRow * const *)a;
    CsvRow * const *pb = (CsvRow * const *)b;
    const CsvRow *ra = *pa;
    const CsvRow *rb = *pb;
    /* Treat missing cells as empty string for ordering purposes. */
    const char *va = (ra && g_sort_col < ra->size) ? ra->cells[g_sort_col] : "";
    const char *vb = (rb && g_sort_col < rb->size) ? rb->cells[g_sort_col] : "";
    return g_sort_sign * strcmp(va ? va : "", vb ? vb : "");
}


/**
 * @brief Sort the rows of `file` by the string value in `columnIndex`.
 *
 * In-place. Uses `qsort`; not stable across equal keys. Missing cells
 * are treated as empty strings for ordering purposes. Comparison is by
 * `strcmp`, so `"10"` < `"2"` — for numeric sort, project or pad the
 * column first.
 *
 * @param file        File to sort.
 * @param columnIndex Column whose string value is the sort key.
 * @param ascending   true → A→Z; false → Z→A.
 * @param skipHeader  true → row 0 stays put; the rest are sorted.
 *                    false → every row participates.
 */
void csv_file_sort(CsvFile *file, size_t columnIndex, bool ascending, bool skipHeader) {
    CSV_LOG("[csv_file_sort]: Function start.");
    if (!file || file->size < 2) {
        CSV_LOG("[csv_file_sort]: nothing to sort.");
        return;
    }

    size_t start = (skipHeader && file->size > 0) ? 1 : 0;
    size_t n     = file->size - start;
    if (n < 2) { 
        return;
    }

    g_sort_col  = columnIndex;
    g_sort_sign = ascending ? 1 : -1;
    qsort(file->rows + start, n, sizeof(CsvRow*), csv_sort_cmp);

    CSV_LOG("[csv_file_sort]: sorted %zu row(s) on column %zu (asc=%d).", n, columnIndex, (int)ascending);
}


/**
 * @brief Build a new CsvFile containing only rows that satisfy `predicate`.
 *
 * Generic, closure-by-convention filter — the predicate receives a
 * `userdata` pointer the caller supplies, so you can filter by any
 * criterion (column value, regex match, range check, …) without writing
 * a new function per filter.
 *
 * Each kept row is DEEP-copied — the result is independently
 * destroyable and the source is not modified.
 *
 * @param file       Source file.
 * @param predicate  Called once per non-header row. Returning true keeps
 *                   the row in the result.
 * @param userdata   Opaque pointer forwarded to `predicate`. May be NULL.
 * @param keepHeader If true and the source has a header, the header is
 *                   copied unconditionally (without consulting `predicate`).
 * @return Newly-allocated CsvFile (caller must `csv_file_destroy` it),
 *         or NULL on bad input / OOM.
 */
CsvFile* csv_file_filter(const CsvFile *file, CsvRowPredicate predicate, void *userdata, bool keepHeader) {
    CSV_LOG("[csv_file_filter]: Function start.");
    if (!file || !predicate) {
        CSV_LOG("[csv_file_filter]: NULL file or predicate.");
        return NULL;
    }

    CsvFile *out = csv_file_create(file->delimiter);
    if (!out) {
        return NULL;
    }

    size_t start = 0;
    if (keepHeader && file->size > 0) {
        /* Deep-copy the header. */
        CsvRow *src = file->rows[0];
        CsvRow *dst = csv_row_create();
        if (!dst) { csv_file_destroy(out); return NULL; }
        for (size_t j = 0; j < src->size; ++j) {
            csv_row_append_cell(dst, src->cells[j] ? src->cells[j] : "");
        }
        csv_file_append_row(out, dst);
        start = 1;
    }

    for (size_t i = start; i < file->size; ++i) {
        CsvRow *src = file->rows[i];
        if (!predicate(src, userdata)) {
            continue;
        }

        CsvRow *dst = csv_row_create();
        if (!dst) { 
            csv_file_destroy(out); 
            return NULL; 
        }

        for (size_t j = 0; j < src->size; ++j) {
            csv_row_append_cell(dst, src->cells[j] ? src->cells[j] : "");
        }
        csv_file_append_row(out, dst);
    }

    CSV_LOG("[csv_file_filter]: kept %zu of %zu row(s).", out->size, file->size);
    return out;
}


/**
 * @brief Replace the value at cell `index` of `row` in place.
 *
 * Fills the glaring gap that without this function the library can
 * APPEND cells (`csv_row_append_cell`) but never modify an existing
 * one. The new value is copied (`string_strdup`) so the caller may
 * free its buffer immediately after. The old value's memory is
 * released, so there's no leak from repeated overwrites — even when
 * the new string is much shorter than the old one.
 *
 * @param row   Target row.
 * @param index 0-based cell index. Must be < `row->size`.
 * @param value New cell value. Empty string is allowed; NULL is not.
 * @return true on success, false if `row`/`value` is NULL, the index
 *         is out of range, or memory allocation fails.
 */
bool csv_row_set_cell(CsvRow *row, size_t index, const char *value) {
    CSV_LOG("[csv_row_set_cell]: Function start.");
    if (!row || !value || index >= row->size) {
        CSV_LOG("[csv_row_set_cell]: invalid input (row=%p, value=%p, index=%zu, size=%zu).", (void*)row, (const void*)value, index, row ? row->size : 0);
        return false;
    }

    char *dup = string_strdup(value);
    if (!dup) {
        CSV_LOG("[csv_row_set_cell]: string_strdup failed.");
        return false;
    }
    free(row->cells[index]);
    row->cells[index] = dup;

    CSV_LOG("[csv_row_set_cell]: cell %zu set to '%s'.", index, value);
    return true;
}

/**
 * @brief Build a new CsvFile with duplicate rows removed by one column's value.
 *
 * Keeps the FIRST occurrence of every distinct value seen in
 * `columnIndex`. Subsequent rows with the same key value are dropped.
 * Rows shorter than `columnIndex` are treated as if their key were the
 * empty string (so all "short" rows collapse to one).
 *
 * Comparison is `strcmp`-exact and case-sensitive. The result is a deep
 * copy — the source file is untouched and the result is independently
 * destroyable.
 *
 * Complexity is O(N²) in the number of rows (no hash table). Fine for
 * the typical CSV sizes this library targets; if you need O(N) on
 * millions of rows, sort by the key first and walk linearly.
 *
 * @param file        Source file.
 * @param columnIndex Column whose value is the dedup key.
 * @param keepHeader  If true and `file` has rows, row 0 is copied
 *                    unconditionally (not considered for dedup).
 * @return Newly-allocated CsvFile (caller must `csv_file_destroy`), or
 *         NULL on bad input / OOM.
 */
CsvFile* csv_file_unique_by_column(const CsvFile *file, size_t columnIndex, bool keepHeader) {
    CSV_LOG("[csv_file_unique_by_column]: Function start.");
    if (!file) {
        CSV_LOG("[csv_file_unique_by_column]: NULL file.");
        return NULL;
    }
    CsvFile *out = csv_file_create(file->delimiter);
    if (!out) { 
        return NULL;
    }

    size_t start = 0;
    if (keepHeader && file->size > 0) {
        CsvRow *src = file->rows[0];
        CsvRow *dst = csv_row_create();
        if (!dst) { csv_file_destroy(out); return NULL; }
        for (size_t j = 0; j < src->size; ++j) {
            csv_row_append_cell(dst, src->cells[j] ? src->cells[j] : "");
        }
        csv_file_append_row(out, dst);
        start = 1;
    }

    for (size_t i = start; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        const char *key = (columnIndex < row->size && row->cells[columnIndex]) ? row->cells[columnIndex] : "";

        bool seen = false;
        size_t scan_start = (keepHeader && out->size > 0) ? 1 : 0;
        for (size_t k = scan_start; k < out->size && !seen; ++k) {
            CsvRow *prev = out->rows[k];
            const char *prev_key = (columnIndex < prev->size && prev->cells[columnIndex]) ? prev->cells[columnIndex] : "";
            if (strcmp(key, prev_key) == 0) {
                seen = true;
            }
        }
        if (seen) { 
            continue;
        }

        CsvRow *dst = csv_row_create();
        if (!dst) { 
            csv_file_destroy(out); 
            return NULL; 
        }
        for (size_t j = 0; j < row->size; ++j) {
            csv_row_append_cell(dst, row->cells[j] ? row->cells[j] : "");
        }
        csv_file_append_row(out, dst);
    }

    CSV_LOG("[csv_file_unique_by_column]: %zu unique row(s) (from %zu).", out->size, file->size);
    return out;
}


/**
 * @brief Serialize the whole file to a malloc'd in-memory string.
 *
 * Symmetric to `csv_file_load_from_string`. Cells are joined with
 * `file->delimiter`; rows end in a single `\n`. The returned string is
 * NUL-terminated and owned by the caller — free it with `free()`.
 *
 * Does NOT add CSV-style quote escaping for cells that contain the
 * delimiter, quotes, or newlines (matches `csv_file_write`'s policy).
 *
 * Output is **byte-identical** to what `csv_file_write` produces on
 * every platform: both always emit `\n`-only line endings. `csv_file_write`
 * opens the destination in binary mode precisely so this guarantee
 * holds on Windows too.
 *
 * @param file Source file.
 * @return Newly-allocated NUL-terminated CSV string, or NULL on bad
 *         input / OOM. For an empty file the string is `""` (length 0).
 */
char* csv_file_export_to_string(const CsvFile *file) {
    CSV_LOG("[csv_file_export_to_string]: Function start.");
    if (!file) {
        CSV_LOG("[csv_file_export_to_string]: NULL file.");
        return NULL;
    }

    size_t cap = 256;
    size_t len = 0;
    char *out = (char*)malloc(cap);
    if (!out) { 
        return NULL;
    }
    out[0] = '\0';

    for (size_t i = 0; i < file->size; ++i) {
        CsvRow *row = file->rows[i];
        for (size_t j = 0; j < row->size; ++j) {
            const char *cell = row->cells[j] ? row->cells[j] : "";
            size_t cell_len = strlen(cell);

            /* Reserve room for the cell + delimiter + newline + NUL. */
            if (len + cell_len + 2 >= cap) {
                size_t new_cap = (len + cell_len + 2) * 2;
                char *nb = (char*)realloc(out, new_cap);
                if (!nb) { 
                    free(out); 
                    return NULL; 
                }
                out = nb;
                cap = new_cap;
            }
            memcpy(out + len, cell, cell_len);
            len += cell_len;
            if (j + 1 < row->size) {
                out[len++] = file->delimiter;
            }
        }

        if (len + 2 >= cap) {
            size_t new_cap = (len + 2) * 2;
            char *nb = (char*)realloc(out, new_cap);

            if (!nb) { 
                free(out); 
                return NULL; 
            }
            out = nb;
            cap = new_cap;
        }
        out[len++] = '\n';
    }
    out[len] = '\0';
    CSV_LOG("[csv_file_export_to_string]: emitted %zu bytes from %zu row(s).", len, file->size);
    return out;
}
