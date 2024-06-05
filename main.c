#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "test", "kaisen", "986523");

        if (postgres_connect(pg)) {
            const char* tableName = "cars";
            if (postgres_table_exists(pg, tableName)) {
                fmt_printf("Table '%s' exists.\n", tableName);
            } 
            else {
                fmt_printf("Table '%s' does not exist.\n", tableName);
            }

            postgres_disconnect(pg);
        } 
        else {
            fmt_fprintf(stderr, "Error: %s.\n", postgres_get_last_error(pg));
        }

        postgres_deallocate(pg);
    } 
    else {
        fmt_fprintf(stderr, "Error: Unable to create postgres object.\n");
    }

    return 0;
}
