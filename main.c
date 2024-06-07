#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "test", "postgres", "amin1375");

        if (postgres_connect(pg)) {
            int backend_pid = postgres_backend_pid(pg);

            if (backend_pid != -1) {
                fmt_printf("Backend PID: %d\n", backend_pid);
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