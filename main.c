#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "test", "postgres", "amin1375", "localhost", "5432");

        if (postgres_connect(pg)) {
            fmt_printf("Connected to the database successfully.\n");

            const char* tableName = "cars";
            const char* csvFilePath = "sources/pg_csv.csv";
            const char* delimiter = ",";

            if (postgres_copy_from_csv(pg, tableName, csvFilePath, delimiter)) {
                fmt_printf("Data copied from CSV to table '%s' successfully.\n", tableName);
            } 
            else {
                fmt_fprintf(stderr, "Failed to copy data from CSV to table '%s'.\n", tableName);
            }

            postgres_disconnect(pg);
        } 
        else {
            fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
        }

        postgres_deallocate(pg);
    }
    else {
        fmt_fprintf(stderr, "Error: Unable to create postgres object.\n");
    }

    return 0;
}
