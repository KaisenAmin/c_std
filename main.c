#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "test", "postgres", "amin1375");

        if (postgres_connect(pg)) {
            const char* tableName = "cars";
            PostgresResult* pgRes = postgres_get_table_columns(pg, tableName);

            if (pgRes != NULL) {
                fmt_printf("Columns in table '%s':\n", tableName);
                postgres_print_result(pgRes); 
                postgres_clear_result(pgRes);
            } 
            else {
                fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
            }

            postgres_disconnect(pg);
        } 
        else {
            fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
        }

        postgres_deallocate(pg);
    } 
    else {
        fmt_fprintf(stderr, "Error: Unable to create PostgreSQL object.\n");
    }

    return 0;
}
