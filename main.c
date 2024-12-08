#include "csv/csv.h"
#include "fmt/fmt.h"



int main() {
    CsvFile* csv = csv_file_create(',');
    CsvRow* newRow = csv_row_create();

    csv_row_append_cell(newRow, "New Cell 1");
    csv_row_append_cell(newRow, "New Cell 2");
    csv_row_append_cell(newRow, "سلول جدید 3");

    csv_file_append_row(csv, newRow); // Append the new row to the CSV file
    csv_file_write(csv, "C:\\Users\\asus\\OneDrive\\Desktop\\project\\c_std\\sources\\updated_file.csv");  

    csv_file_destroy(csv);
    return 0;
}