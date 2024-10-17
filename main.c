#include "csv/csv.h"
#include "fmt/fmt.h"

int main() {
    const char *filename = "C:\\Users\\asus\\OneDrive\\Desktop\\project\\c_std\\sources\\test_10.csv"; 
    char delimiter = ','; // Specify the delimiter used in your CSV file
    CsvFile* myCSV = csv_file_create(delimiter);  // Create a new CsvFile instance

    csv_file_read(myCSV, filename); // Read the CSV file

    fmt_printf("Contents of the CSV file:\n"); // Print the contents of the CSV file
    csv_print(myCSV);

    csv_file_destroy(myCSV);
    return 0;
}