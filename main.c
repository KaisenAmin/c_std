#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres *pg = (Postgres*) malloc(sizeof(Postgres));

    postgres_init(pg, "test", "test2", "123465689");
    
    if (postgres_connect(pg)) {
        PostgresResult *pgRes = postgres_query(pg, "CREATE TABLE cars (brand VARCHAR(255), model VARCHAR(255), year INT);");

        if (pgRes) {
            fmt_printf("Create Table Done\n");
            postgres_clear_result(pgRes);
        }
    }

    postgres_disconnect(pg);
    postgres_deallocate(pg);

    return 0;
}
