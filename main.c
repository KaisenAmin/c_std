#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "test", "postgres", "amin1375", "localhost", "5432");

        if (postgres_connect(pg)) {
            fmt_printf("Database name is %s\n", postgres_db_value(pg));
            fmt_printf("user name is %s\n", postgres_user_value(pg));
            fmt_printf("password name is %s\n", postgres_password_value(pg));
            fmt_printf("host is %s\n", postgres_host_value(pg));
            fmt_printf("port is %s\n", postgres_port_value(pg));

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