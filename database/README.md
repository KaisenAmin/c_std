# PostgreSQL C Library

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

This PostgreSQL C library provides a simplified interface for connecting to a PostgreSQL database, executing queries, and managing results. It is designed to be easy to integrate into C projects, providing essential functionality for database interaction without the complexity of the native PostgreSQL C API (libpq).

## Key Features

- **Connection Management:** Functions to initialize, connect, and disconnect from a PostgreSQL database.
- **Query Execution:** Simple interface to execute SQL queries and handle results.
- **Memory Management:** Utilities to manage memory for database connections and results.
- **Non-Query Execution:** function to execute SQL commands that do not return data (e.g., INSERT, UPDATE, DELETE).
## Installation and Compilation

To use this library, include `postgres.h` in your project and compile the source files with your C compiler. For GCC, the following command can be used:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./database/postgres.c ./string/string.c ./fmt/fmt.c -lpq
```

## Documentation

The documentation includes detailed descriptions of all the functions provided by the library, along with their usage examples. It covers basic operations like initializing a connection, executing queries, and handling results.

### Function Descriptions

- `Postgres* postgres_create()`: Allocates and returns a new `Postgres` object.
- `void postgres_init(Postgres* pg, const char* database, const char* user, const char* password)`: Initializes the `Postgres` object with the given database, user, and password.
- `bool postgres_connect(Postgres* pg)`: Establishes a connection to the PostgreSQL database.
- `void postgres_disconnect(Postgres* pg)`: Disconnects from the PostgreSQL database.
- `PostgresResult* postgres_query(Postgres* pg, const char* query)`: Executes an SQL query and returns the result.
- `void postgres_clear_result(PostgresResult* pgResult)`: Clears the result of a query.
- `void postgres_deallocate(Postgres* pg)`: Frees the memory occupied by the `Postgres` object.
- `bool postgres_execute_non_query(Postgres* pg, const char* command)`: Executes an SQL command that does not return data (e.g., INSERT, UPDATE, DELETE).

## Examples

Several examples are provided to demonstrate the usage of the PostgreSQL library in various scenarios, including creating tables and handling query results.

### Example 1: Basic Connection and Query

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres *pg = postgres_create();

    if (pg) {
        postgres_init(pg, "test", "postgres", "12356");
        
        if (postgres_connect(pg)) {
            PostgresResult *pgRes = postgres_query(pg, "CREATE TABLE cars (brand VARCHAR(255), model VARCHAR(255), year INT);");

            if (pgRes) {
                fmt_printf("Create Table Done\n");
                postgres_clear_result(pgRes);
            }
            postgres_disconnect(pg);
        }

        postgres_deallocate(pg);
    } 
    else {
        fmt_fprintf(stderr, "Error: Unable to create PostgreSQl object.\n");
    }

    return 0;
}
```

### Example 2: Executing Non-Query Commands 

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres *pg = postgres_create();

    if (pg) {
        postgres_init(pg, "test", "postgres", "123654");
        
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
```

## Conclusion

This PostgreSQL C library simplifies database interactions in C projects, providing an easy-to-use interface for connecting to PostgreSQL databases, executing queries, and managing results. The provided examples illustrate how to use the library for common database operations.