/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class CsvFile, CsvRow
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in csv.c (file-level convention).
 *
 * Minimal CSV reader/writer + a DataFrame-lite layer (find/sort/
 * filter/unique/export).
 */

#ifndef CSV_H_
#define CSV_H_

#include "../file_io/file_reader.h"

#ifdef __cplusplus
extern "C" {
#endif


/* #define CSV_LOGGING_ENABLE */

#ifdef CSV_LOGGING_ENABLE
    #define CSV_LOG(fmt, ...) \
        do { fprintf(stderr, "[CSV LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define CSV_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Limits                                                             */
/* ------------------------------------------------------------------ */

#define BUFFER_SIZE 1024


typedef struct CsvRow  CsvRow;
typedef struct CsvFile CsvFile;

struct CsvRow {
    char**  cells;        /* array of strings                                 */
    size_t  size;         /* current number of cells                          */
    size_t  capacity;     /* total capacity of the cells array                */
};

struct CsvFile {
    CsvRow** rows;        /* array of pointers to CsvRow                      */
    size_t   size;        /* current number of rows                           */
    size_t   capacity;    /* total capacity of the rows array                 */
    char     delimiter;   /* delimiter character                              */
};

/* User predicate for `csv_file_filter`. Return true to keep, false to drop. */
typedef bool (*CsvRowPredicate)(const CsvRow* row, void* userdata);


/* ------------------------------------------------------------------ */
/* CsvRow — construction / destruction                                */
/* ------------------------------------------------------------------ */

CsvRow*    csv_row_create                  (void);
void       csv_row_destroy                 (CsvRow* row);


/* ------------------------------------------------------------------ */
/* CsvRow — element access / mutation                                 */
/* ------------------------------------------------------------------ */

char*      csv_row_get_cell                (const CsvRow* row, size_t index);
int        csv_row_get_cell_as_int         (const CsvRow* row, size_t index);
bool       csv_row_set_cell                (CsvRow* row, size_t index, const char* value);
void       csv_row_append_cell             (CsvRow* row, const char* value);
bool       csv_validate_cell_format        (const CsvRow* row, size_t index, const char* format);


/* ------------------------------------------------------------------ */
/* CsvFile — construction / destruction                               */
/* ------------------------------------------------------------------ */

CsvFile*   csv_file_create                 (char delimiter);
void       csv_file_destroy                (CsvFile* file);


/* ------------------------------------------------------------------ */
/* CsvFile — I/O                                                      */
/* ------------------------------------------------------------------ */

void       csv_file_read                   (CsvFile* file, const char* filename);
void       csv_file_write                  (const CsvFile* file, const char* filename);
void       csv_file_load_from_string       (CsvFile* file, const char* data);
char*      csv_file_export_to_string       (const CsvFile* file);
char*      csv_export_to_json              (const CsvFile* file);
CsvRow*    csv_row_read_next               (FileReader* reader, char delimiter);
void       csv_print                       (const CsvFile* file);


/* ------------------------------------------------------------------ */
/* CsvFile — RFC 4180 (quote-aware) I/O                               */
/* ------------------------------------------------------------------ */

void       csv_file_load_from_string_rfc4180 (CsvFile* file, const char* data);
bool       csv_file_read_rfc4180             (CsvFile* file, const char* filename);
char*      csv_file_export_to_string_rfc4180 (const CsvFile* file);
bool       csv_file_write_rfc4180            (const CsvFile* file, const char* filename);


/* ------------------------------------------------------------------ */
/* CsvFile — row access / mutation                                    */
/* ------------------------------------------------------------------ */

CsvRow*    csv_file_get_row                (const CsvFile* file, size_t index);
CsvRow*    csv_file_get_header             (const CsvFile* file);
void       csv_file_set_header             (CsvFile* file, CsvRow* header);
void       csv_file_append_row             (CsvFile* file, CsvRow* row);
void       csv_file_remove_row             (CsvFile* file, size_t index);
void       csv_file_concatenate            (CsvFile* file1, const CsvFile* file2);


/* ------------------------------------------------------------------ */
/* CsvFile — column operations                                        */
/* ------------------------------------------------------------------ */

int        csv_file_find_column_index      (const CsvFile* file, const char* columnName);
char**     csv_file_get_column             (const CsvFile* file, size_t columnIndex, size_t* outCount, bool skipHeader);
char*      csv_row_get_cell_by_name        (const CsvFile* file, const CsvRow* row, const char* columnName);
void       csv_file_insert_column          (CsvFile* file, size_t colIndex, const CsvRow* colData);
void       csv_file_remove_column          (CsvFile* file, size_t columnIndex);
int        csv_column_sum                  (const CsvFile* file, size_t columnIndex);


/* ------------------------------------------------------------------ */
/* CsvFile — query / transform                                        */
/* ------------------------------------------------------------------ */

CsvRow**   csv_file_find_rows              (const CsvFile* file, const char* searchTerm);
void       csv_file_sort                   (CsvFile* file, size_t columnIndex, bool ascending, bool skipHeader);
CsvFile*   csv_file_filter                 (const CsvFile* file, CsvRowPredicate predicate, void* userdata, bool keepHeader);
CsvFile*   csv_file_unique_by_column       (const CsvFile* file, size_t columnIndex, bool keepHeader);


#ifdef __cplusplus
}
#endif

#endif 
