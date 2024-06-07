#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "test", "postgres", "amin1375");

        if (postgres_connect(pg)) {
            PostgresResult* pgRes = postgres_query(pg, "SELECT * FROM bus");

            if (pgRes != NULL) {
                int is_binary = postgres_binary_tuples(pgRes);

                if (is_binary) {
                    fmt_printf("Yes is binary data and value is %d\n", is_binary);
                } 
                else if (is_binary == -1) {
                    fmt_fprintf(stderr, "Error: some kind of unknow error happened.\n");
                }    
                else {
                    fmt_printf("data is text not binary.\n");
                }
            }
            else {
                fmt_fprintf(stderr, "Error: PostgresResult object failed.\n");
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