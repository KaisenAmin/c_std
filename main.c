#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres *pg = postgres_create();

    if (pg) {
        postgres_init(pg, "test", "postgres", "amin1375");
        
        if (postgres_connect(pg)) {
            const char *createTableCmd = "CREATE TABLE IF NOT EXISTS cars (brand VARCHAR(255), model VARCHAR(255), year INT);";
            if (postgres_execute_non_query(pg, createTableCmd)) {
                fmt_printf("Table created successfully.\n");
            }

            const char *insertCmd = "INSERT INTO cars (brand, model, year) VALUES ('Toyota', 'Corolla', 2021);";
            if (postgres_execute_non_query(pg, insertCmd)) {
                fmt_printf("Data inserted successfully.\n");
            }

            postgres_disconnect(pg);
        }

        postgres_deallocate(pg);
    } 
    else {
        fmt_fprintf(stderr, "Error: Unable to create PostgreSQL object.\n");
    }

    return 0;
}
