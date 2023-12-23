#include "csv.h"
#include <stdlib.h> // For dynamic memory allocation
#include <string.h> // For string manipulation
#include <stdio.h>

CSVRow* csv_row_create() 
{
    CSVRow* row = malloc(sizeof(CSVRow));
    if (!row) 
        return NULL;

    row->cells = NULL;
    row->size = 0;
    row->capacity = 0;

    return row;
}

void csv_row_destroy(CSVRow *row) 
{
    if (!row) 
        return;

    for (size_t i = 0; i < row->size; ++i)  // Free each cell string
        free(row->cells[i]);

    free(row->cells); // Free the array of cell pointers
    free(row);  // Free the CSVRow itself
}

void csv_row_append_cell(CSVRow *row, const char *value) 
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

char* csv_row_get_cell(const CSVRow *row, size_t index) 
{
    if (row == NULL || index >= row->size) 
    {
        fprintf(stderr, "Error: Invalid index or null row in csv_row_get_cell.\n");
        return NULL;
    }

    return row->cells[index];
}


CSVFile* csv_file_create(char delimiter) 
{
    CSVFile* file = malloc(sizeof(CSVFile));
    if (!file) 
        return NULL;

    file->rows = NULL;
    file->size = 0;
    file->capacity = 0;
    file->delimiter = delimiter;

    return file;
}

void csv_file_destroy(CSVFile *file) 
{
    if (!file) 
        return;
    for (size_t i = 0; i < file->size; ++i) 
        csv_row_destroy(file->rows[i]);
    
    free(file->rows);
    free(file);
}

void csv_file_read(CSVFile *file, const char *filename) 
{
    FILE *fp = fopen(filename, "r");
    if (!fp) 
    {
        perror("Unable to open file");
        return;
    }

    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, fp)) 
    {
        CSVRow *row = csv_row_create(); // Assuming csv_row_create allocates and initializes a CSVRow

        buffer[strcspn(buffer, "\r\n")] = 0; // Replace newline character with null terminator
        char *token = strtok(buffer, &file->delimiter); // Tokenize the line using the specified delimiter
        
        while (token) 
        {
            csv_row_append_cell(row, token); // Append each cell to the row
            token = strtok(NULL, &file->delimiter);
        }
        csv_file_append_row(file, row); // Append the row to the CSV file
    }

    fclose(fp);
}

void csv_file_write(const CSVFile *file, const char *filename)
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
        CSVRow *row = file->rows[i];

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


void csv_file_append_row(CSVFile *file, CSVRow *row) {
    if (!file || !row) {
        fprintf(stderr, "Error: NULL parameter passed to csv_file_append_row.\n");
        return;
    }

    // Resize the rows array if necessary
    if (file->size >= file->capacity) {
        size_t newCapacity = file->capacity == 0 ? 1 : file->capacity * 2;
        CSVRow **newRows = realloc(file->rows, newCapacity * sizeof(CSVRow *));
        if (!newRows) {
            perror("Unable to allocate memory for new rows");
            return;
        }
        file->rows = newRows;
        file->capacity = newCapacity;
    }

    // Append the row
    file->rows[file->size++] = row;
}

CSVRow* csv_file_get_row(const CSVFile *file, size_t index) 
{
    if (!file || index >= file->size) 
    {
        fprintf(stderr, "Error: Invalid index or null file in csv_file_get_row.\n");
        return NULL;
    }
    
    return file->rows[index]; // Retrieve the row at the specified index
}

void csv_file_remove_row(CSVFile *file, size_t index) 
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

void csv_print(const CSVFile *file) 
{
    if (!file) 
    {
        fprintf(stderr, "Error: Null file pointer in csv_print.\n");
        return;
    }

    for (size_t i = 0; i < file->size; ++i) 
    {
        CSVRow *row = file->rows[i];

        for (size_t j = 0; j < row->size; ++j) 
        {
            printf("%s", row->cells[j]);
            if (j < row->size - 1) 
                printf("%c", file->delimiter);
        }
        printf("\n");
    }
}

CSVRow* csv_row_read_next(FILE *file, char delimiter) 
{
    char buffer[BUFFER_SIZE];
    if (!fgets(buffer, BUFFER_SIZE, file)) 
        return NULL; // No more lines to read or error occurred

    buffer[strcspn(buffer, "\r\n")] = 0; // Remove newline character

    CSVRow *row = csv_row_create();
    char *token = strtok(buffer, &delimiter);

    while (token) 
    {
        csv_row_append_cell(row, token);
        token = strtok(NULL, &delimiter);
    }

    return row;
}

void csv_file_insert_column(CSVFile *file, size_t colIndex, const CSVRow *colData) 
{
    if (!file || !colData || colIndex > colData->size) {
        fprintf(stderr, "Error: Invalid parameters in csv_file_insert_column.\n");
        return;
    }

    for (size_t i = 0; i < file->size; ++i)
    {
        CSVRow *row = file->rows[i];
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

CSVRow* csv_file_get_header(const CSVFile *file) 
{
    if (!file || file->size == 0) 
    {
        fprintf(stderr, "Error: File is empty or null in csv_file_get_header.\n");
        return NULL;
    }

    return file->rows[0]; // Return the first row as the header
}

void csv_file_set_header(CSVFile *file, CSVRow *header) 
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
            CSVRow **newRows = realloc(file->rows, newCapacity * sizeof(CSVRow *));
            
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

int csv_row_get_cell_as_int(const CSVRow *row, size_t index) 
{
    if (!row || index >= row->size) 
    {
        fprintf(stderr, "Error: Invalid index or null row in csv_row_get_cell_as_int.\n");
        return 0;
    }

    return atoi(row->cells[index]); // Convert the string to an integer
}

CSVRow** csv_file_find_rows(const CSVFile *file, const char* searchTerm) 
{
    if (!file || !searchTerm) 
    {
        fprintf(stderr, "Error: Null parameters passed to csv_file_find_rows.\n");
        return NULL;
    }

    size_t foundCount = 0;
    CSVRow **foundRows = malloc(file->size * sizeof(CSVRow *)); // Assuming worst case, all rows match
    
    if (!foundRows) 
    {
        perror("Memory allocation failed for foundRows");
        return NULL;
    }

    for (size_t i = 0; i < file->size; ++i) 
    {
        CSVRow* row = file->rows[i];
        for (size_t j = 0; j < row->size; ++j) 
        {
            if (strstr(row->cells[j], searchTerm)) 
            {
                foundRows[foundCount++] = row;
                break; // Break after finding the first occurrence in a row
            }
        }
    }

    CSVRow **resizedFoundRows = realloc(foundRows, foundCount * sizeof(CSVRow *)); // Optional: resize foundRows to match foundCount
    if (!resizedFoundRows) 
    {
        free(foundRows);
        perror("Memory allocation failed for resizedFoundRows");

        return NULL;
    }

    return resizedFoundRows; // Return the array of found rows
}

bool csv_validate_cell_format(const CSVRow *row, size_t index, const char *format) 
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

void csv_file_join(const CSVFile *file1, const CSVFile *file2, size_t keyColumnIndex) 
{
    if (!file1 || !file2) 
    {
        fprintf(stderr, "Error: Null parameters in csv_file_join.\n");
        return;
    }

    for (size_t i = 0; i < file1->size; ++i) 
    {
        CSVRow* row1 = file1->rows[i];
        if (keyColumnIndex >= row1->size) 
            continue;

        char* key = row1->cells[keyColumnIndex];
        for (size_t j = 0; j < file2->size; ++j) 
        {
            CSVRow* row2 = file2->rows[j];

            if (keyColumnIndex >= row2->size) 
                continue;

            char* key2 = row2->cells[keyColumnIndex];
            if (strcmp(key, key2) == 0) 
            {
                for (size_t k = 0; k < row2->size; ++k) // Append cells from row2 to row1
                {
                    if (k != keyColumnIndex) 
                        csv_row_append_cell(row1, row2->cells[k]);
                }
                break; // Break after finding the first match
            }
        }
    }
}

int csv_column_sum(const CSVFile *file, size_t columnIndex) 
{
    if (!file) 
    {
        fprintf(stderr, "Error: File is NULL in csv_column_sum.\n");
        return 0;
    }

    int sum = 0;
    for (size_t i = 0; i < file->size; ++i) 
    {
        CSVRow *row = file->rows[i];
        if (columnIndex < row->size) 
        {
            char *cell = row->cells[columnIndex];
            sum += atoi(cell); // Convert the cell value to an integer and add to sum
        }
        else 
            fprintf(stderr, "Warning: Column index out of range in row %zu.\n", i);
    }

    return sum;
}

char* csv_export_to_json(const CSVFile *file) 
{
    if (!file) 
    {
        fprintf(stderr, "Error: Null CSVFile passed to csv_export_to_json.\n");
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
        CSVRow *row = file->rows[i];
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
