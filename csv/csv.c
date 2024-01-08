/**
 * @author Amin Tahmasebi
 * @date 2023 
 * @class CsvFile, CsvRow
*/

#include "csv.h"
#include <stdlib.h> // For dynamic memory allocation
#include <string.h> // For string manipulation
#include <stdio.h>

CsvRow* csv_row_create() 
{
    CsvRow* row = malloc(sizeof(CsvRow));
    if (!row) 
        return NULL;

    row->cells = NULL;
    row->size = 0;
    row->capacity = 0;

    return row;
}

void csv_row_destroy(CsvRow *row) 
{
    if (!row) 
        return;

    for (size_t i = 0; i < row->size; ++i)  // Free each cell string
        free(row->cells[i]);

    free(row->cells); // Free the array of cell pointers
    free(row);  // Free the CsvRow itself
}

void csv_row_append_cell(CsvRow *row, const char *value) 
{
    if (!row || !value) 
    {
        fprintf(stderr, "Error: NULL parameter passed to csv_row_append_cell.\n");
        return;
    }

    if (row->size >= row->capacity) // Resize the cells array if necessary
    {
        size_t newCapacity = row->capacity == 0 ? 1 : row->capacity * 2;
        char **newCells = realloc(row->cells, newCapacity * sizeof(char *));

        if (!newCells) 
        {
            perror("Unable to allocate memory for new cells");
            return;
        }
        row->cells = newCells;
        row->capacity = newCapacity;
    }

    row->cells[row->size] = strdup(value); // Duplicate the string to store in the cells array

    if (!row->cells[row->size]) 
    {
        perror("Unable to allocate memory for cell value");
        return;
    }

    row->size++;
}

char* csv_row_get_cell(const CsvRow *row, size_t index) 
{
    if (row == NULL || index >= row->size) 
    {
        fprintf(stderr, "Error: Invalid index or null row in csv_row_get_cell.\n");
        return NULL;
    }

    return row->cells[index];
}

CsvFile* csv_file_create(char delimiter) 
{
    CsvFile* file = malloc(sizeof(CsvFile));
    if (!file) 
        return NULL;

    file->rows = NULL;
    file->size = 0;
    file->capacity = 0;
    file->delimiter = delimiter;

    return file;
}

void csv_file_destroy(CsvFile *file) 
{
    if (!file) 
        return;
    for (size_t i = 0; i < file->size; ++i) 
        csv_row_destroy(file->rows[i]);
    
    free(file->rows);
    free(file);
}

static void parse_csv_line(const char *line, char delimiter, CsvRow *row) 
{
    bool inQuotes = false;
    size_t start = 0;

    for (size_t i = 0; line[i] != '\0'; ++i) 
    {
        if (line[i] == '"') 
            inQuotes = !inQuotes;
        else if (line[i] == delimiter && !inQuotes) 
        {
            size_t len = i - start;
            char *cell = (char *)malloc(len + 1);

            if (cell) 
            {
                strncpy(cell, line + start, len);
                cell[len] = '\0';
                csv_row_append_cell(row, cell);
                free(cell);
            }
            start = i + 1;
        }
    }
    // Add the last cell
    char *cell = strdup(line + start);
    csv_row_append_cell(row, cell);
    free(cell);
}

void csv_file_read(CsvFile *file, const char *filename) 
{
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Unable to open file");
        return;
    }

    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, fp)) {
        CsvRow *row = csv_row_create();

        buffer[strcspn(buffer, "\r\n")] = 0; // Remove newline character
        parse_csv_line(buffer, file->delimiter, row); // Use the custom parsing function

        csv_file_append_row(file, row);
    }

    fclose(fp);
}

void csv_file_write(const CsvFile *file, const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) 
    {
        perror("Unable to open file for writing");
        return;
    }

    // Iterate over each row in the CSV file
    for (size_t i = 0; i < file->size; ++i) 
    {
        CsvRow *row = file->rows[i];

        for (size_t j = 0; j < row->size; ++j)  // Iterate over each cell in the row
        {
            fprintf(fp, "%s", row->cells[j]);
            if (j < row->size - 1) 
                fputc(file->delimiter, fp); // Delimiter between cells
        }

        fputc('\n', fp); // Newline at the end of each row
    }

    fclose(fp);
}

void csv_file_append_row(CsvFile *file, CsvRow *row) 
{
    if (!file || !row) 
    {
        fprintf(stderr, "Error: NULL parameter passed to csv_file_append_row.\n");
        return;
    }

    if (file->size >= file->capacity)  // Resize the rows array if necessary
    {
        size_t newCapacity = file->capacity == 0 ? 1 : file->capacity * 2;
        CsvRow **newRows = realloc(file->rows, newCapacity * sizeof(CsvRow *));

        if (!newRows) 
        {
            perror("Unable to allocate memory for new rows");
            return;
        }
        file->rows = newRows;
        file->capacity = newCapacity;
    }

    file->rows[file->size++] = row; // Append the row
}

CsvRow* csv_file_get_row(const CsvFile *file, size_t index) 
{
    if (!file || index >= file->size) 
    {
        fprintf(stderr, "Error: Invalid index or null file in csv_file_get_row.\n");
        return NULL;
    }
    
    return file->rows[index]; // Retrieve the row at the specified index
}

void csv_file_remove_row(CsvFile *file, size_t index) 
{
    if (!file || index >= file->size) 
    {
        fprintf(stderr, "Error: Invalid index or null file in csv_file_remove_row.\n");
        return;
    }

    csv_row_destroy(file->rows[index]);   // Free the memory allocated for the row to be removed

    for (size_t i = index; i < file->size - 1; i++) // Shift the remaining rows to fill the gap
        file->rows[i] = file->rows[i + 1];

    // Decrease the size of the file
    file->size--;
}

void csv_print(const CsvFile *file) 
{
    if (!file) 
    {
        fprintf(stderr, "Error: Null file pointer in csv_print.\n");
        return;
    }

    for (size_t i = 0; i < file->size; ++i) 
    {
        CsvRow *row = file->rows[i];

        for (size_t j = 0; j < row->size; ++j) 
        {
            printf("%s", row->cells[j]);
            if (j < row->size - 1) 
                printf("%c", file->delimiter);
        }
        printf("\n");
    }
}

CsvRow* csv_row_read_next(FILE *file, char delimiter) 
{
    char buffer[BUFFER_SIZE];
    if (!fgets(buffer, BUFFER_SIZE, file)) 
        return NULL; // No more lines to read or error occurred

    buffer[strcspn(buffer, "\r\n")] = 0; // Remove newline character

    CsvRow *row = csv_row_create();
    char *token = strtok(buffer, &delimiter);

    while (token) 
    {
        csv_row_append_cell(row, token);
        token = strtok(NULL, &delimiter);
    }

    return row;
}

void csv_file_insert_column(CsvFile *file, size_t colIndex, const CsvRow *colData) 
{
    if (!file || !colData || colIndex > colData->size) 
    {
        fprintf(stderr, "Error: Invalid parameters in csv_file_insert_column.\n");
        return;
    }

    for (size_t i = 0; i < file->size; ++i)
    {
        CsvRow *row = file->rows[i];
        char *cellValue = NULL;

        if (i < colData->size) 
        {
            cellValue = strdup(colData->cells[i]); // Duplicate the cell value
            if (cellValue == NULL) 
            {
                perror("Unable to allocate memory for cell value");
                continue;
            }
        }

        // Check if we need to resize the row's cells array
        if (row->size == row->capacity) 
        {
            size_t newCapacity = row->capacity == 0 ? 1 : row->capacity * 2;
            char **newCells = realloc(row->cells, newCapacity * sizeof(char *));

            if (newCells == NULL) 
            {
                perror("Unable to allocate memory for new cells");
                free(cellValue);

                continue;
            }

            row->cells = newCells;
            row->capacity = newCapacity;
        }

        // Shift cells to the right to make space for the new cell
        for (size_t j = row->size; j > colIndex; --j) 
            row->cells[j] = row->cells[j - 1];

        row->cells[colIndex] = cellValue; // Insert the new cell
        row->size++;
    }
}

CsvRow* csv_file_get_header(const CsvFile *file) 
{
    if (!file || file->size == 0) 
    {
        fprintf(stderr, "Error: File is empty or null in csv_file_get_header.\n");
        return NULL;
    }

    return file->rows[0]; // Return the first row as the header
}

void csv_file_set_header(CsvFile *file, CsvRow *header) 
{
    if (!file || !header) 
    {
        fprintf(stderr, "Error: Null parameters passed to csv_file_set_header.\n");
        return;
    }

    if (file->size > 0) 
        csv_row_destroy(file->rows[0]); // Free the existing header row before replacing
    else 
    {
        // Increase the array size if there's no space
        if (file->size == file->capacity) 
        {
            size_t newCapacity = file->capacity == 0 ? 1 : file->capacity * 2;
            CsvRow **newRows = realloc(file->rows, newCapacity * sizeof(CsvRow *));
            
            if (!newRows) 
            {
                perror("Unable to allocate memory for new rows");
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

int csv_row_get_cell_as_int(const CsvRow *row, size_t index) 
{
    if (!row || index >= row->size) 
    {
        fprintf(stderr, "Error: Invalid index or null row in csv_row_get_cell_as_int.\n");
        return 0;
    }

    return atoi(row->cells[index]); // Convert the string to an integer
}

CsvRow** csv_file_find_rows(const CsvFile *file, const char* searchTerm) 
{
    if (!file || !searchTerm) 
    {
        fprintf(stderr, "Error: Null parameters passed to csv_file_find_rows.\n");
        return NULL;
    }

    size_t foundCount = 0;
    CsvRow **foundRows = malloc((file->size + 1) * sizeof(CsvRow *)); // Allocate one extra for null termination
    
    if (!foundRows) 
    {
        perror("Memory allocation failed for foundRows");
        return NULL;
    }

    for (size_t i = 0; i < file->size; ++i) 
    {
        CsvRow* row = file->rows[i];
        for (size_t j = 0; j < row->size; ++j) 
        {
            if (strstr(row->cells[j], searchTerm)) 
            {
                foundRows[foundCount++] = row;
                break; // Break after finding the first occurrence in a row
            }
        }
    }

    foundRows[foundCount] = NULL; // Null-terminate the array

    // Optional: resize foundRows to match foundCount + 1 for null termination
    CsvRow **resizedFoundRows = realloc(foundRows, (foundCount + 1) * sizeof(CsvRow *));
    if (!resizedFoundRows) 
    {
        // If realloc fails, foundRows is still valid and contains the data
        perror("Memory allocation failed for resizedFoundRows");
        return foundRows; // Return the original foundRows to avoid memory leak
    }

    return resizedFoundRows; // Return the array of found rows
}


bool csv_validate_cell_format(const CsvRow *row, size_t index, const char *format) 
{
    if (!row || !format || index >= row->size) 
    {
        fprintf(stderr, "Error: Invalid parameters in csv_validate_cell_format.\n");
        return false;
    }

    char* cell = row->cells[index];
    char buffer[256]; // Assuming a buffer size sufficient for formatted string
    
    snprintf(buffer, sizeof(buffer), format, cell);

    return strcmp(cell, buffer) == 0;
}

void csv_file_concatenate(CsvFile *file1, const CsvFile *file2) 
{
    if (!file1 || !file2) 
    {
        fprintf(stderr, "Error: Null parameters in csv_file_concatenate.\n");
        return;
    }

    for (size_t i = 0; i < file2->size; ++i) 
    {
        CsvRow *row2 = file2->rows[i];
        CsvRow *newRow = csv_row_create();   // Create a new row for file1 and copy each cell of row2 into it
        
        for (size_t j = 0; j < row2->size; ++j) 
            csv_row_append_cell(newRow, row2->cells[j]);
    
        csv_file_append_row(file1, newRow);    // Append the new row to file1
    }
}

int csv_column_sum(const CsvFile *file, size_t columnIndex) 
{
    if (!file) 
    {
        fprintf(stderr, "Error: File is NULL in csv_column_sum.\n");
        return 0;
    }

    int sum = 0;
    for (size_t i = 0; i < file->size; ++i) 
    {
        CsvRow *row = file->rows[i];
        if (columnIndex < row->size) 
        {
            char *cell = row->cells[columnIndex];
            int cellValue = atoi(cell);
            sum += cellValue;
        }
        else 
            fprintf(stderr, "Warning: Column index out of range in row %zu.\n", i);
    }

    return sum;
}

char* csv_export_to_json(const CsvFile *file) 
{
    if (!file) 
    {
        fprintf(stderr, "Error: Null CsvFile passed to csv_export_to_json.\n");
        return NULL;
    }

    char *json = malloc(BUFFER_SIZE);
    if (!json) 
    {
        perror("Memory allocation failed for JSON export");
        return NULL;
    }

    strcpy(json, "[\n");
    for (size_t i = 0; i < file->size; ++i) 
    {
        CsvRow *row = file->rows[i];
        strcat(json, "  {\n");
        
        for (size_t j = 0; j < row->size; ++j) 
        {
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
