#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "test", "postgres", "amin1375");

        if (postgres_connect(pg)) {
            const char* createTableCmd1 = 
                "CREATE TABLE IF NOT EXISTS bus ("
                "id SERIAL PRIMARY KEY, "
                "brand VARCHAR(255), "
                "model VARCHAR(255), "
                "year INT"
                ");";
            if (postgres_execute_non_query(pg, createTableCmd1)) {
                fmt_printf("Table 'bus' created successfully.\n");
            } 
            else {
                fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
            }

            const char* createIndexCmd = "CREATE INDEX IF NOT EXISTS idx_bus_brand ON bus(brand);";
            if (postgres_execute_non_query(pg, createIndexCmd)) {
                fmt_printf("Index on 'brand' column created successfully.\n");
            } 
            else {
                fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
            }

            const char* tableName = "bus";
            int index_count = postgres_get_table_index_count(pg, tableName); // index count

            if (index_count >= 0) {
                fmt_printf("Number of indexes on table '%s': %d\n", tableName, index_count);
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
        fmt_fprintf(stderr, "Error: Unable to create postgres object.\n");
    }

    return 0;
}
