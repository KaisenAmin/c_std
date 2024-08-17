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
- **Non-Query Execution:** function to execute SQL commands that do not return data (INSERT, UPDATE, DELETE).
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
- `bool postgres_begin_transaction(Postgres* pg)`: Executes the SQL command `BEGIN` to start a transaction.
- `bool postgres_commit_transaction(Postgres* pg)`: Executes the SQL command `COMMIT` to commit the current transaction.
- `postgres_rollback_transaction(Postgres* pg)`: Execures the SQL command `ROLLBACK` to rollback the current transaction.
- `const char* postgres_get_last_error(Postgres* pg)`: retrieves the last error message from the PostgreSQL connection.
- `int postgres_get_affected_rows(Postgres* pg, PostgresResult* pgRes)`: Returns the number of rows affected by the SQL command. If the SQL command that generated the PostgresResult was INSERT, UPDATE or DELETE, this returns a string containing the number of rows affected.
- `void postgres_print_result(PostgresResult* pgRes)`: function takes a PostgresResult object and prints the results.
- `int postgres_get_table_row_count(Postgres* pg, const char* tableName)`: function to retrieve the number of rows in a table.
- `bool postgres_table_exists(Postgres* pg, const char* tableName)`: function to check if table is exists or not.
- `PostgresResult* postgres_list_tables(Postgres* pg)`: this function list all the tables in the current database.
- `PostgresResult* postgres_get_table_schema(Postgres* pg, const char* tableName)`: this function retrieve the column names and their data types for a given table in the database.
- `bool postgres_execute_prepared(Postgres* pg, const char* stmtName, int nParams, const char* const* paramValues)`: this function execute a parameterized query.This can help prevent SQL injection and make your queries more flexible.
- `PostgresResult* postgres_get_table_columns(Postgres* pg, const char* tableName)`: this function fetch the names of all the columns in a given table.This can be useful if you want to know the structure of a table without fetching all its data.
- `PostgresResult* postgres_get_table_primary_keys(Postgres* pg, const char* tableName)`: this function retrieves the primary keys of a given table. This can be useful for understanding the structure of a table and its unique constraints.
- `PostgresResult* postgres_get_table_foreign_keys(Postgres* pg, const char* tableName)`: this function retrieve foreign keys from a given table This function will help you understand the relationships between tables by identifying foreign keys and the tables they reference.
- `PostgresResult* postgres_get_table_indexes(Postgres* pg, const char* tableName)`: this function retrieve the indexes of a given table. This function will help you understand the indexing strategy used in the table, which is important for optimizing query performance.
- `PostgresResult* postgres_get_table_size(Postgres* pg, const char* tableName)`: this function will help you understand the storage footprint of a table, which is useful for database maintenance and optimization.
- `int postgres_get_table_index_count(Postgres* pg, const char* tableName)`: This function will help you understand the indexing strategy used in the table, which is crucial for query performance optimization.
- `PostgresResult* postgres_get_column_details(Postgres* pg, const char* tableName)`: This function will be similar to postgres_get_table_schema, but it will return more detailed information, including whether the column allows NULL values and the default value for the column if any.
- `const char* postgres_get_value(PostgresResult* pgRes, int row, int col)`: This function will provide a more convenient way to get values from the PostgresResult structure.
- `PostgresResult* postgres_get_table_constraints(Postgres* pg, const char* tableName)`: This function retrieves all the constraints (PRIMARY KEY, FOREIGN KEY, UNIQUE, CHECK) of a given table. This function can be very useful for understanding the structure and constraints of a table.
- `int postgres_num_tuples(PostgresResult* pgRes)`: this function retrieve number of tuples (row) in a PostgresResult object.
- `int postgres_num_fields(PostgresResult* pgRes)`: this function retrieve number of fields (columns) in a PostgresResult object.
- `int postgres_backend_pid(Postgres* pg)`: this function retrieves the backend process ID.
- `int postgres_command_tuples(PostgresResult* pgRes)`: this function will return the number of rows affected by the most recent command executed.
- `int postgres_binary_tuples(const PostgresResult* pgRes)`: this function will returns 1 if the PGresult contains binary data and 0 if it contains text data. 
- `int potgres_bytes_size(const PostgresResult* pgRes)`: this function return the size in bytes of the column associated with the given column number. Column number start at 0.[returns the space allocated for this column in a database row]
- `bool postgres_is_null(const PostgresResult* pgRes, int row, int col)`: Tests a field for a null value. Row and column numbers start at 0. this function return true if field is null and false if it contains a non-null value.
- `int postgres_reset_start(Postgres* pg)`:  If it returns 0, the reset has failed.
- `char* postgres_db_value(const Postgres* pg)`: this function return postgres database value.
- `char* postgres_user_value(const Postgres* pg)`: this function return username value.
- `char* postgres_password_value(const Postgres* pg)`: this function return password value.
- `char* postgres_host_value(const Postgres* pg)`: this function return host value.
- `char* postgres_port_value(const Postgres* pg)`: this function return port number.
- `char* postgres_object_id_status(const PostgresResult* pgRes)`: Returns a string with the object ID of the inserted row, if the SQL command was an INSERT. (The string will be 0 if the INSERT did not insert exactly one row, or if the target table does not have OIDs.) If the command was not an INSERT, returns an empty string.

- `char* postgres_command_status(PostgresResult* pgRes)`: Returns the command status string from the SQL command that generated the PostgresResutl.
- `int postgres_protocol_version(const Postgres* pg)`: return the frontend/backend protocol being used .Currently, the possible values are 2 (2.0 protocol), 3 (3.0 protocol), or zero (connection bad). This will not change after connection startup is complete, but it could theoretically change during a connection reset. The 3.0 protocol will normally be used when communicating with PostgreSQL 7.4 or later servers; pre-7.4 servers support only protocol 2.0. 

- `int postgres_server_version(cosnt Postgres* pg)`: Returns an integer representing the backend version. Applications may use this to determine the version of the database server they are connected to. The number is formed by converting the major, minor, and revision numbers into two-decimal-digit numbers and appending them together. For example, version 7.4.2 will be returned as 70402, and version 8.1 will be returned as 80100 (leading zeroes are not shown). Zero is returned if the connection is bad.

- `int postgres_socket_descriptor(const Postgres* pg)`: obtained the file descriptor number of the connection socket to the server. a valid descriptor is equal or grater than 0 and -1 indicates that no server connection is currently open.

- `int postgres_is_busy(Postgres* pg)`: Returns 'true' if a command is busy, that is, 'false' return indicates that PQgetResult can be called with assurance of not blocking.

- `bool postgres_is_non_blocking(const Postgres* pg)`: this functio return the blocking status of the database connection. Returns 'true' if the connection is set to nonblocking mode and 'false' if blocking.

- `int postgres_flush(Postgres* pg)`: this function Attempts to flush any queued output data to the server. Returns 0 if successful (or if the send queue is empty), -1 if it failed for some reason, or 1 if it was unable to send all the data in the send queue yet (this case can only occur if the connection is nonblocking). After sending any command or data on a nonblocking connection, call 'postgres_flush'. If it returns 1, wait for the socket to be write-ready and call it again; repeat until it returns 0. Once 'postgres_flush' returns 0, wait for the socket to be read-ready and then read the response as described above.

- `int postgres_set_non_blocking(Postgres* pg, int state)`: this function the nonblocking status of the connection. Sets the state of the connection to nonblocking if arg is 1, or blocking if arg is 0. Returns 0 if OK, -1 if error.

- `int postgres_get_line(Postgres* pg, char* buffer, int length)`: this function Reads a newline-terminated line of characters (transmitted by the server) into a buffer string of size length. also copies up to length-1 characters into the buffer and converts the terminating newline into a zero byte. PQgetline returns EOF at the end of input, 0 if the entire line has been read, and 1 if the buffer is full but the terminating newline has not yet been read.
Note that the application must check to see if a new line consists of the two characters \., which indicates that the server has finished sending the results of the COPY command. If the application might receive lines that are more than length-1 characters long, care is needed to be sure it recognizes the \. line correctly (and does not, for example, mistake the end of a long data line for a terminator line).

- `int postgres_put_line(Postgres* pg, const char* buffer)`: Sends a null-terminated string to the server. Returns 0 if OK and EOF if unable to send the string and return -1 if some kind of error happened .The COPY data stream sent by a series of calls to 'postgres_put_line' has the same format as that returned by 'postgres_get_line_async', except that applications are not obliged to send exactly one data row per 'postgres_put_line' call; it is okay to send a partial line or multiple lines per call.

- `int postgres_put_bytes(Postgres* pg, const char* buffer, int bytes)`: Sends a non-null-terminated string to the server. Returns 0 if OK and EOF if unable to send the string. This is exactly like 'postgres_put_line', except that the data buffer need not be null-terminated since the number of bytes to send is specified directly. Use this procedure when sending binary data.

- `void postgres_trace(Postgres* pg, FILE* stream)` : Enables tracing of the client/server communication to a debugging file stream.
- `void postgres_un_trace(Postgres* pg)` : Disables tracing started by 'postgres_trace'.
- `PostgresResult* postgres_get_result(Postgres* pg)`: 
- `int postgres_request_cancle(Postgres* pg)`: this function Request that PostgreSQL abandon processing of the current command. The return value is 1 if the cancel request was successfully dispatched, 0 if not. (If not, 'postgres_get_last_error' tells why not.) Successful dispatch is no guarantee that the request will have any effect, however. Regardless of the return value of 'postgres_request_cancle', the application must continue with the normal result-reading sequence using 'postgres_get_result'. If the cancellation is effective, the current command will terminate early and return an error result. If the cancellation fails (say, because the backend was already done processing the command), then there will be no visible result at all.

- `bool postgres_reconnect(Postgres* pg)` : Disconnects from the PostgreSQL database and attempts to reconnect using the stored connection parameters.

- `bool postgres_ping(Postgres* pg)`: Checks if the connection to the PostgreSQL database server is still alive.

- `PostgresResult* postgres_query_execution_time(Postgres* pg, const char* query)`: This function executes a given SQL query on the  
PostgreSQL database, measures the time taken for the query to execute, and returns the result along with the execution time.

- `bool postgres_create_function(Postgres* pg, const char* functionName, const char* returnType, const char* language, const char* functionBody, const char* paramDefinitions)` : This function constructs and executes a SQL `CREATE FUNCTION` command to create a new user-defined function in the PostgreSQL database using the specified parameters.

- `bool postgres_drop_function(Postgres* pg, const char* functionName, const char* paramDefinitions)`: This function constructs and executes a SQL `DROP FUNCTION` command to remove a user-defined function from the PostgreSQL database.

- `bool postgres_create_view(Postgres* pg, const char* viewName, const char* query)` : This function constructs and executes a SQL `CREATE VIEW` command to create a new view in the PostgreSQL database using the specified name and query.

- `bool postgres_drop_view(Postgres* pg, const char* viewName)` : This function constructs and executes a SQL `DROP VIEW` command to remove a view from the PostgreSQL database.

- `bool postgres_create_trigger(Postgres* pg, const char* triggerName, const char* tableName, const char* timing, const char* event, const char* function)` : This function constructs and executes a SQL `CREATE TRIGGER` command to create a trigger on the specified table. The trigger will call a specified function when a specified event occurs.

- `bool postgres_drop_trigger(Postgres* pg, const char* triggerName, const char* tableName)` : This function constructs and executes a SQL `DROP TRIGGER` command to remove a trigger from the specified table.

- `bool postgres_create_schema(Postgres* pg, const char* schemaName)` : This function constructs and executes a SQL `CREATE SCHEMA` command to create a new schema in the PostgreSQL database using the specified schema name.

- `bool postgres_drop_schema(Postgres* pg, const char* schemaName, bool cascade)`: This function constructs and executes a SQL `DROP SCHEMA` command to drop an existing schema in the PostgreSQL database. The schema can be dropped with the `CASCADE` option to drop all dependent objects.

- `PostgresResult* postgres_query_params(Postgres* pg, const char* query, int nParams, const char* const* paramValues)` : This function allows the execution of parameterized SQL queries to prevent SQL injection. The parameters are passed separately from the query, and PostgreSQL will handle their proper escaping.

- `bool postgres_prepare_statement(Postgres* pg, const char* stmtName, const char* query)`: This function prepares a SQL statement, which can be executed multiple times with different parameters.

- `bool postgres_clear_prepared_statement(Postgres* pg, const char* stmtName)`: This function clears a prepared statement, which frees up the associated resources in the PostgreSQL database.

- `bool postgres_savepoint(Postgres* pg, const char* savepointName)` : This function creates a savepoint within the current transaction, allowing you to rollback to this point without affecting the entire transaction.

- `bool postgres_rollback_to_savepoint(Postgres* pg, const char* savepointName)` : This function rolls back the current transaction to a specific savepoint, undoing any changes made after the savepoint was created.

- `bool postgres_send_async_query(Postgres* pg, const char* query)` : This function sends a query to the PostgreSQL server in a non-blocking manner, allowing the application to perform other tasks while the server processes the query.

- `PostgresResult* postgres_get_async_result(Postgres* pg)` : This function retrieves the result of a previously sent asynchronous query. It should be called repeatedly until the result is fully available.

- `bool postgres_copy_from_csv(Postgres* pg, const char* tableName, const char* csvFilePath, const char* delimiter)` : Copies data from a CSV file into a specified table using PostgreSQL's COPY command.


## Examples

Several examples are provided to demonstrate the usage of the PostgreSQL library in various scenarios, including creating tables and handling query results and etc ... 

### Example 1: Basic Connection and Query

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres *pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");
        
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
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");
        
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

### Example 3: Using Transactions 

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres *pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");
        
        if (postgres_connect(pg)) {
            if (postgres_begin_transaction(pg)) {
                fmt_printf("Transaction started successfully.\n");

                const char *insertCmd1 = "INSERT INTO cars (brand, model, year) VALUES ('Toyota', 'Corolla', 2021);";
                const char *insertCmd2 = "INSERT INTO cars (brand, model, year) VALUES ('Honda', 'Civic', 2020);";

                bool success1 = postgres_execute_non_query(pg, insertCmd1);
                bool success2 = postgres_execute_non_query(pg, insertCmd2);

                if (success1 && success2) {
                    if (postgres_commit_transaction(pg)) {
                        fmt_printf("Transaction committed successfully.\n");
                    } 
                    else {
                        fmt_fprintf(stderr, "Error: Failed to commit transaction.\n");
                    }
                } 
                else {
                    fmt_fprintf(stderr, "Error: Failed to execute one or more commands.\n");
                }
            } 
            else {
                fmt_fprintf(stderr, "Error: Failed to start transaction.\n");
            }

            postgres_disconnect(pg);
        } 
        else {
            fmt_fprintf(stderr, "Error: Failed to connect to the database.\n");
        }

        postgres_deallocate(pg);
    } 
    else {
        fmt_fprintf(stderr, "Error: Unable to create PostgreSQL object.\n");
    }

    return 0;
}
```

### Example 4: Using Rollback transaction. 

`in this example just i delete qoute from Toyota in first insertion for checking rollback transaction`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres *pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");
        
        if (postgres_connect(pg)) {
            if (postgres_begin_transaction(pg)) {
                fmt_printf("Transaction started successfully.\n");

                const char *insertCmd1 = "INSERT INTO cars (brand, model, year) VALUES (Toyota, 'Corolla', 2021);";
                const char *insertCmd2 = "INSERT INTO cars (brand, model, year) VALUES ('Honda', 'Civic', 2020);";

                bool success1 = postgres_execute_non_query(pg, insertCmd1);
                bool success2 = postgres_execute_non_query(pg, insertCmd2);

                if (success1 && success2) {
                    if (postgres_commit_transaction(pg)) {
                        fmt_printf("Transaction committed successfully.\n");
                    } 
                    else {
                        fmt_fprintf(stderr, "Error: Failed to commit transaction. Rolling back.\n");
                        postgres_rollback_transaction(pg);
                    }
                } 
                else {
                    fmt_fprintf(stderr, "Error: Failed to execute one or more commands. Rolling back.\n");
                    postgres_rollback_transaction(pg);
                }
            } 
            else {
                fmt_fprintf(stderr, "Error: Failed to start transaction.\n");
            }

            postgres_disconnect(pg);
        } 
        else {
            fmt_fprintf(stderr, "Error: Failed to connect to the database.\n");
        }

        postgres_deallocate(pg);
    } 
    else {
        fmt_fprintf(stderr, "Error: Unable to create PostgreSQL object.\n");
    }

    return 0;
}
```

### Example 5: get last error 

`in this example just i delete qoute from Toyota in first insertion for checking rollback transaction`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres *pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");
        
        if (postgres_connect(pg)) {
            if (postgres_begin_transaction(pg)) {
                fmt_printf("Transaction started successfully.\n");

                const char *insertCmd1 = "INSERT INTO cars (brand, model, year) VALUES (Toyota, 'Corolla', 2021);";
                const char *insertCmd2 = "INSERT INTO cars (brand, model, year) VALUES ('Honda', 'Civic', 2020);";

                bool success1 = postgres_execute_non_query(pg, insertCmd1);
                bool success2 = postgres_execute_non_query(pg, insertCmd2);

                if (success1 && success2) {
                    if (postgres_commit_transaction(pg)) {
                        fmt_printf("Transaction committed successfully.\n");
                    } 
                    else {
                        fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
                        postgres_rollback_transaction(pg);
                    }
                } 
                else {
                    fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
                    postgres_rollback_transaction(pg);
                }
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
```

### Example 6: Using get affected rows 

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres *pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");
        
        if (postgres_connect(pg)) {
            if (postgres_begin_transaction(pg)) {
                fmt_printf("Transaction started successfully.\n");

                const char *insertCmd1 = "INSERT INTO cars (brand, model, year) VALUES ('Toyota', 'Corolla', 2021);";
                const char *insertCmd2 = "INSERT INTO cars (brand, model, year) VALUES ('Honda', 'Civic', 2020);";

                PostgresResult *res1 = postgres_query(pg, insertCmd1);
                PostgresResult *res2 = postgres_query(pg, insertCmd2);

                if (res1 != NULL && res2 != NULL) {
                    int affectedRows1 = postgres_get_affected_rows(pg, res1);
                    int affectedRows2 = postgres_get_affected_rows(pg, res2);

                    postgres_clear_result(res1);
                    postgres_clear_result(res2);

                    fmt_printf("Number of affected rows for first command: %d\n", affectedRows1);
                    fmt_printf("Number of affected rows for second command: %d\n", affectedRows2);
                    
                    if (postgres_commit_transaction(pg)) {
                        fmt_printf("Transaction committed successfully.\n");
                    } 
                    else {
                        fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
                        postgres_rollback_transaction(pg);
                    }
                } 
                else {
                    fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
                    postgres_rollback_transaction(pg);
                }
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
```

### Example 7: print result of PostgresResult object. 

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres *pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");
        
        if (postgres_connect(pg)) {
            PostgresResult *pgRes = postgres_query(pg, "SELECT * FROM cars");

            if (pgRes != NULL) {
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
```

### Example 8: get table row count 

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

        if (postgres_connect(pg)) {
            int rowCount = postgres_get_table_row_count(pg, "cars");

            if (rowCount >= 0) {
                fmt_printf("Number of rows in cars table is %d\n", rowCount);
            }
            else {
                fmt_fprintf(stderr, "Error: %s.\n", postgres_get_last_error(pg));
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
```

### Example 9 : check if table exists or not 

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

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
```

### Example 10 : get list of table names and print them with `postgres_list_tables`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

        if (postgres_connect(pg)) {
            PostgresResult* pgRes = postgres_list_tables(pg);

            if (pg) {
                fmt_printf("Tables in the `public` Schema:\n");
                postgres_print_result(pgRes);
                postgres_clear_result(pgRes);
            }
            else {
                fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
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
```

### Example 11 : how to use `postgres_get_table_schema`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

        if (postgres_connect(pg)) {
            const char* tableName = "cars";
            PostgresResult* pgRes = postgres_get_table_schema(pg, tableName);

            if (pgRes != NULL) {
                fmt_printf("Schema of table '%s':\n", tableName);
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
        fmt_fprintf(stderr, "Error: Unable to create postgres object.\n");
    }

    return 0;
}
```

### Example 12 : Executes a prepared SQL statement with the given parameters

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

        if (postgres_connect(pg)) {
            const char* stmtName = "insert_car";
            const char* query = "INSERT INTO cars (brand, model, year) VALUES ($1, $2, $3)";

            // First, prepare the SQL statement
            if (postgres_prepare_statement(pg, stmtName, query)) {
                fmt_printf("Statement '%s' prepared successfully.\n", stmtName);

                const char* paramValues[3] = {"BMW", "I8", "2020"};

                // Then, execute the prepared statement with the provided parameters
                if (postgres_execute_prepared(pg, stmtName, 3, paramValues)) {
                    fmt_printf("Prepared statement executed successfully.\n");
                } 
                else {
                    fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
                }

                // Clear the prepared statement
                if (postgres_clear_prepared_statement(pg, stmtName)) {
                    fmt_printf("Statement '%s' cleared successfully.\n", stmtName);
                } 
                else {
                    fmt_fprintf(stderr, "Failed to clear statement '%s'.\n", stmtName);
                }
            } 
            else {
                fmt_fprintf(stderr, "Failed to prepare statement '%s'.\n", stmtName);
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
```

### Example 13: get primary keys of tables with `postgres_get_table_primary_keys`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

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
            } else {
                fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
            }

            const char* createTableCmd2 = 
                "CREATE TABLE IF NOT EXISTS owners ("
                "owner_id SERIAL PRIMARY KEY, "
                "name VARCHAR(255), "
                "bus_id INT, "
                "FOREIGN KEY (bus_id) REFERENCES bus(id)"
                ");";
            if (postgres_execute_non_query(pg, createTableCmd2)) {
                fmt_printf("Table 'owners' created successfully.\n");
            } 
            else {
                fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
            }

            const char* tableName1 = "bus";
            PostgresResult* pgRes1 = postgres_get_table_primary_keys(pg, tableName1);

            if (pgRes1) {
                fmt_printf("Primary Keys of table '%s':\n", tableName1);
                postgres_print_result(pgRes1);
                postgres_clear_result(pgRes1);
            } 
            else {
                fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
            }

            const char* tableName2 = "owners";
            PostgresResult* pgRes2 = postgres_get_table_primary_keys(pg, tableName2);

            if (pgRes2) {
                fmt_printf("Primary Keys of table '%s':\n", tableName2);
                postgres_print_result(pgRes2);
                postgres_clear_result(pgRes2);
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
```

### Example 14 : get list of primary keys with `postgres_get_table_foreign_keys`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

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

            const char* createTableCmd2 = 
                "CREATE TABLE IF NOT EXISTS owners ("
                "owner_id SERIAL PRIMARY KEY, "
                "name VARCHAR(255), "
                "bus_id INT, "
                "FOREIGN KEY (bus_id) REFERENCES bus(id)"
                ");";
            if (postgres_execute_non_query(pg, createTableCmd2)) {
                fmt_printf("Table 'owners' created successfully.\n");
            } 
            else {
                fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
            }

            const char* tableName = "owners";
            PostgresResult* pgRes = postgres_get_table_foreign_keys(pg, tableName);

            if (pgRes) {
                fmt_printf("Foreign Keys of table '%s':\n", tableName);
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
        fmt_fprintf(stderr, "Error: Unable to create postgres object.\n");
    }

    return 0;
}
```

### Example 15 : get list of table indexes with `postgres_get_table_indexes`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

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

            // Create an index on the 'brand' column
            const char* createIndexCmd = "CREATE INDEX IF NOT EXISTS idx_bus_brand ON bus(brand);";
            if (postgres_execute_non_query(pg, createIndexCmd)) {
                fmt_printf("Index on 'brand' column created successfully.\n");
            } 
            else {
                fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
            }

            const char* tableName = "bus";
            PostgresResult* pgRes = postgres_get_table_indexes(pg, tableName);

            if (pgRes) {
                fmt_printf("Indexes of table '%s':\n", tableName);
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
        fmt_fprintf(stderr, "Error: Unable to create postgres object.\n");
    }

    return 0;
}
```

### Example 16 : get size of tables with `postgres_get_table_size`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

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
            PostgresResult* pgRes = postgres_get_table_size(pg, tableName);

            if (pgRes) {
                fmt_printf("Size of table '%s':\n", tableName);
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
        fmt_fprintf(stderr, "Error: Unable to create postgres object.\n");
    }

    return 0;
}
```

### Example 17 : get index count of tables with `postgres_get_table_index_count`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

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
```

### Example 18 : get details of columns by `postgres_get_column_details`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

        if (postgres_connect(pg)) {
            const char* tableName = "bus";
            PostgresResult* pgRes = postgres_get_column_details(pg, tableName);

            if (pgRes != NULL) {
                fmt_printf("Column details of table '%s':\n", tableName);
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
        fmt_fprintf(stderr, "Error: Unable to create postgres object.\n");
    }

    return 0;
}
```

### Example 19 : get value from table with `postgres_get_value`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

        if (postgres_connect(pg)) {
            postgres_execute_non_query(pg, "CREATE TABLE IF NOT EXISTS cars (id SERIAL PRIMARY KEY, brand VARCHAR(255), model VARCHAR(255), year INT);");
            postgres_execute_non_query(pg, "INSERT INTO cars (brand, model, year) VALUES ('Toyota', 'Corolla', 2021), ('Honda', 'Civic', 2020);");

            PostgresResult* pgRes = postgres_query(pg, "SELECT * FROM cars");

            if (pgRes != NULL) {
                fmt_printf("Query Results:\n");
                postgres_print_result(pgRes);

                const char* value = postgres_get_value(pgRes, 0, 1); 
                if (value) {
                    fmt_printf("Brand of the first car: %s\n", value);
                }

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
        fmt_fprintf(stderr, "Error: Unable to create postgres object.\n");
    }

    return 0;
}
```

### Example 20 : get table constraints with `postgres_get_table_constraints`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

        if (postgres_connect(pg)) {
            postgres_execute_non_query(pg, "DROP TABLE IF EXISTS cars;");

            // table with various constraints
            if (postgres_execute_non_query(pg, "CREATE TABLE cars (id SERIAL PRIMARY KEY, brand VARCHAR(255) NOT NULL, model VARCHAR(255), year INT CHECK (year > 1885));")) {
                fmt_printf("Table 'cars' created successfully.\n");
            } 
            else {
                fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
            }

            postgres_execute_non_query(pg, "INSERT INTO cars (brand, model, year) VALUES ('Toyota', 'Corolla', 2021);");
            postgres_execute_non_query(pg, "INSERT INTO cars (brand, model, year) VALUES ('Honda', 'Civic', 2020);");

            if (postgres_execute_non_query(pg, "CREATE UNIQUE INDEX unique_brand_model ON cars (brand, model);")) {
                fmt_printf("Unique index created successfully.\n");
            } 
            else {
                fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
            }

            PostgresResult* pgRes = postgres_get_table_constraints(pg, "cars");

            if (pgRes != NULL) {
                fmt_printf("Constraints of table 'cars':\n");
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
        fmt_fprintf(stderr, "Error: Unable to create postgres object.\n");
    }

    return 0;
}
```

### Example 21 : get number of rows and cols with `postgres_num_tuples` and `postgres_num_fields`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

        if (postgres_connect(pg)) {
            postgres_execute_non_query(pg, "DROP TABLE IF EXISTS cars;");
            postgres_execute_non_query(pg, "CREATE TABLE cars (id SERIAL PRIMARY KEY, brand VARCHAR(255), model VARCHAR(255), year INT CHECK (year > 1885));");

            postgres_execute_non_query(pg, "INSERT INTO cars (brand, model, year) VALUES ('Toyota', 'Corolla', 2021);");
            postgres_execute_non_query(pg, "INSERT INTO cars (brand, model, year) VALUES ('Benz', 'S500', 2020);");

            PostgresResult* pgRes = postgres_query(pg, "SELECT * FROM cars");

            if (pgRes != NULL) {
                int num_tuples = postgres_num_tuples(pgRes);
                int num_fields = postgres_num_fields(pgRes);

                fmt_printf("Number of tuples (rows): %d\n", num_tuples);
                fmt_printf("Number of fields (columns): %d\n", num_fields);

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
        fmt_fprintf(stderr, "Error: Unable to create postgres object.\n");
    }

    return 0;
}
```

### Example 22 : get command tuples with `postgres_command_tuples`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

        if (postgres_connect(pg)) {
            postgres_execute_non_query(pg, "DROP TABLE IF EXISTS cars;");
            postgres_execute_non_query(pg, "CREATE TABLE cars (id SERIAL PRIMARY KEY, brand VARCHAR(255), model VARCHAR(255), year INT CHECK (year > 1885));");
            postgres_execute_non_query(pg, "INSERT INTO cars (brand, model, year) VALUES ('Toyota', 'Corolla', 2021);");
            postgres_execute_non_query(pg, "INSERT INTO cars (brand, model, year) VALUES ('Honda', 'Civic', 2020);");

            // Execute a command that affects rows and get the number of affected rows
            PostgresResult* pgRes = postgres_query(pg, "DELETE FROM cars WHERE brand='Toyota';");
            if (pgRes != NULL) {
                int affected_rows = postgres_command_tuples(pgRes);
                fmt_printf("Number of rows affected by the delete command: %d\n", affected_rows);
                postgres_clear_result(pgRes);
            } 
            else {
                fmt_fprintf(stderr, "Error: %s\n", postgres_get_last_error(pg));
            }

            pgRes = postgres_query(pg, "SELECT * FROM cars");
            if (pgRes != NULL) {
                fmt_printf("Query Results:\n");
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
        fmt_fprintf(stderr, "Error: Unable to create postgres object.\n");
    }

    return 0;
}
```

### Example 23 : check data is binary or text with `postgres_binary_tuples`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

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
```

### Example 24 : get name, user, password of connection

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

        if (postgres_connect(pg)) {
            fmt_printf("Database name is %s\n", postgres_db_value(pg));
            fmt_printf("username is %s\n", postgres_user_value(pg));
            fmt_printf("password is %s\n", postgres_password_value(pg));

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
```

### Example 25 : Reconnecting to the PostgreSQL Database with `postgres_reconnect`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"

int main() {
    Postgres* pg = postgres_create();
    postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

    if (postgres_connect(pg)) {
        fmt_printf("Connected to the database successfully.\n");

        if (!postgres_reconnect(pg)) {
            fmt_fprintf(stderr, "Reconnection failed.\n");
        }
        else {
            fmt_printf("Reconnected to the database successfully.\n");
        }

        postgres_disconnect(pg);
    } 
    else {
        fmt_fprintf(stderr, "Initial connection failed.\n");
    }

    postgres_deallocate(pg);

    return 0;
}
```

### Example 26 : Cheking the Connection Status to the PostgreSQL Database with `postgres_ping`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"

int main() {
    Postgres* pg = postgres_create();
    postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

    if (postgres_connect(pg)) {
        fmt_printf("Connected to the database successfully.\n");

        if (postgres_ping(pg)) {
            fmt_printf("Connection to the database is alive.\n");

            PostgresResult* result = postgres_list_tables(pg);
            if (result) {
                fmt_printf("Tables in the database:\n");
                postgres_print_result(result);
                postgres_clear_result(result);
            } 
            else {
                fmt_fprintf(stderr, "Failed to list tables.\n");
            }

            const char* tableName = "cars"; 
            int rowCount = postgres_get_table_row_count(pg, tableName);

            if (rowCount >= 0) {
                fmt_printf("The table '%s' has %d rows.\n", tableName, rowCount);
            } 
            else {
                fmt_fprintf(stderr, "Failed to retrieve row count for table '%s'.\n", tableName);
            }

        } 
        else {
            fmt_fprintf(stderr, "Connection to the database is lost or server is unreachable.\n");
        }

        postgres_disconnect(pg);
    } 
    else {
        fmt_fprintf(stderr, "Initial connection failed.\n");
    }

    postgres_deallocate(pg);

    return 0;
}
```

### Example 27 : Measuring how long a specific query takes to execute with `postgres_query_execution_time`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"

int main() {
    Postgres* pg = postgres_create();
    postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

    if (postgres_connect(pg)) {
        fmt_printf("Connected to the database successfully.\n");

        const char* query = "SELECT * FROM cars"; 
        PostgresResult* result = postgres_query_execution_time(pg, query);

        if (result) {
            fmt_printf("Query executed successfully.\n");
            postgres_print_result(result);
            postgres_clear_result(result);
        } 
        else {
            fmt_fprintf(stderr, "Failed to execute query.\n");
        }

        postgres_disconnect(pg);
    } 
    else {
        fmt_fprintf(stderr, "Initial connection failed.\n");
    }

    postgres_deallocate(pg);

    return 0;
}
```

### Example 28 : create and drop function with `postgres_create_function` and `postgres_drop_function`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <string.h>

int main() {
    Postgres* pg = postgres_create();
    postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

    if (postgres_connect(pg)) {
        fmt_printf("Connected to the database successfully.\n");

        const char* functionName = "add_numbers";
        const char* returnType = "integer";
        const char* language = "plpgsql";
        const char* functionBody = "BEGIN RETURN $1 + $2; END;";
        const char* paramDefinitions = "param1 integer, param2 integer";  

        if (postgres_create_function(pg, functionName, returnType, language, functionBody, paramDefinitions)) {
            fmt_printf("Function '%s' created successfully.\n", functionName);

            const char* testQuery = "SELECT add_numbers(2, 3);";
            PostgresResult* result = postgres_query(pg, testQuery);

            if (result) {
                const char* value = postgres_get_value(result, 0, 0);
                fmt_printf("Function result: %s\n", value);

                if (strcmp(value, "5") == 0) {
                    fmt_printf("Test passed: Function works as expected.\n");
                } 
                else {
                    fmt_fprintf(stderr, "Test failed: Unexpected result from function.\n");
                }

                postgres_clear_result(result);
            } 
            else {
                fmt_fprintf(stderr, "Failed to execute test query.\n");
            }

            if (postgres_drop_function(pg, functionName, paramDefinitions)) {
                fmt_printf("Function '%s' dropped successfully.\n", functionName);
            } 
            else {
                fmt_fprintf(stderr, "Failed to drop function '%s'.\n", functionName);
            }
        } 
        else {
            fmt_fprintf(stderr, "Failed to create function '%s'.\n", functionName);
        }

        postgres_disconnect(pg);
    } 
    else {
        fmt_fprintf(stderr, "Failed to connect to the database.\n");
    }

    postgres_deallocate(pg);

    return 0;
}
```

### Example 29 : Create and Drop View with `postgres_create_view` and `postgres_drop_view`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"

int main() {
    Postgres* pg = postgres_create();
    postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

    if (postgres_connect(pg)) {
        fmt_printf("Connected to the database successfully.\n");

        const char* viewName = "my_view";
        const char* query = "SELECT id, brand FROM cars WHERE year = 2020;";

        if (postgres_create_view(pg, viewName, query)) {
            fmt_printf("View '%s' created successfully.\n", viewName);

            const char* selectQuery = "SELECT * FROM my_view;";
            PostgresResult* result = postgres_query(pg, selectQuery);

            if (result) {
                fmt_printf("View '%s' query executed successfully.\n", viewName);
                postgres_print_result(result);
                postgres_clear_result(result);
            } 
            else {
                fmt_fprintf(stderr, "Failed to query the view '%s'.\n", viewName);
            }

            if (postgres_drop_view(pg, viewName)) {
                fmt_printf("View '%s' dropped successfully.\n", viewName);
            } 
            else {
                fmt_fprintf(stderr, "Failed to drop the view '%s'.\n", viewName);
            }
        } 
        else {
            fmt_fprintf(stderr, "Failed to create view '%s'.\n", viewName);
        }

        postgres_disconnect(pg);
    } 
    else {
        fmt_fprintf(stderr, "Failed to connect to the database.\n");
    }

    postgres_deallocate(pg);
    return 0;
}
```

### Example 30 : How to craete trigger and drop it with usage of functions `postgres_create_trigger` and `postgres_drop_trigger`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"

int main() {
    Postgres* pg = postgres_create();
    postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

    if (postgres_connect(pg)) {
        fmt_printf("Connected to the database successfully.\n");

        // Step 1: Create the 'info' table
        const char* createTableQuery = 
            "CREATE TABLE IF NOT EXISTS info ("
            "id SERIAL PRIMARY KEY, "
            "name VARCHAR(255));";
        
        if (postgres_execute_non_query(pg, createTableQuery)) {
            fmt_printf("Table 'info' created successfully.\n");
        } 
        else {
            fmt_fprintf(stderr, "Failed to create table 'info'.\n");
            postgres_disconnect(pg);
            postgres_deallocate(pg);
            return 1;
        }

        // Step 2: Create the 'audit_log' table
        const char* createAuditLogTableQuery = 
            "CREATE TABLE IF NOT EXISTS audit_log ("
            "id SERIAL PRIMARY KEY, "
            "table_name VARCHAR(255), "
            "operation VARCHAR(50), "
            "old_data TEXT, "
            "new_data TEXT, "
            "operation_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

        if (postgres_execute_non_query(pg, createAuditLogTableQuery)) {
            fmt_printf("Table 'audit_log' created successfully.\n");
        } 
        else {
            fmt_fprintf(stderr, "Failed to create table 'audit_log'.\n");
            postgres_disconnect(pg);
            postgres_deallocate(pg);
            return 1;
        }

        // Step 3: Create a function to be triggered
        const char* functionName = "log_update";
        const char* returnType = "trigger";
        const char* language = "plpgsql";
        const char* functionBody = 
            "BEGIN "
            "INSERT INTO audit_log(table_name, operation, old_data, new_data) "
            "VALUES (TG_TABLE_NAME, TG_OP, OLD::text, NEW::text); "
            "RETURN NEW; "
            "END;";
        const char* paramDefinitions = ""; // No parameters for this trigger function

        if (postgres_create_function(pg, functionName, returnType, language, functionBody, paramDefinitions)) {
            fmt_printf("Function '%s' created successfully.\n", functionName);

            // Step 4: Create the trigger
            const char* triggerName = "trigger_log_update";
            const char* tableName = "info";
            const char* timing = "AFTER";
            const char* event = "UPDATE";

            if (postgres_create_trigger(pg, triggerName, tableName, timing, event, functionName)) {
                fmt_printf("Trigger '%s' created successfully on table '%s'.\n", triggerName, tableName);

                // Step 5: Insert a sample row
                const char* insertQuery = "INSERT INTO info (name) VALUES ('old_name');";
                if (postgres_execute_non_query(pg, insertQuery)) {
                    fmt_printf("Sample row inserted into 'info'.\n");
                } 
                else {
                    fmt_fprintf(stderr, "Failed to insert sample row into 'info'.\n");
                }

                // Step 6: Update the row to trigger the function
                const char* updateQuery = "UPDATE info SET name = 'new_name' WHERE id = 1;";
                if (postgres_execute_non_query(pg, updateQuery)) {
                    fmt_printf("Update query executed successfully, trigger should have fired.\n");
                } 
                else {
                    fmt_fprintf(stderr, "Failed to execute update query.\n");
                }

                // Step 7: Drop the trigger
                if (postgres_drop_trigger(pg, triggerName, tableName)) {
                    fmt_printf("Trigger '%s' dropped successfully from table '%s'.\n", triggerName, tableName);
                } 
                else {
                    fmt_fprintf(stderr, "Failed to drop trigger '%s' from table '%s'.\n", triggerName, tableName);
                }
            } 
            else {
                fmt_fprintf(stderr, "Failed to create trigger '%s' on table '%s'.\n", triggerName, tableName);
            }

            // Step 8: Drop the function
            if (postgres_drop_function(pg, functionName, "")) {
                fmt_printf("Function '%s' dropped successfully.\n", functionName);
            } 
            else {
                fmt_fprintf(stderr, "Failed to drop function '%s'.\n", functionName);
            }
        } 
        else {
            fmt_fprintf(stderr, "Failed to create function '%s'.\n", functionName);
        }

        postgres_disconnect(pg);
    } 
    else {
        fmt_fprintf(stderr, "Failed to connect to the database.\n");
    }

    postgres_deallocate(pg);
    return 0;
}
```

### Example 31 : Create and Drop Schema with `postgres_crate_schema` and `postgres_drop_schema`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"

int main() {
    Postgres* pg = postgres_create();
    postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

    if (postgres_connect(pg)) {
        fmt_printf("Connected to the database successfully.\n");

        const char* schemaName = "test_schema";

        // Create a new schema
        if (postgres_create_schema(pg, schemaName)) {
            fmt_printf("Schema '%s' created successfully.\n", schemaName);
        } 
        else {
            fmt_fprintf(stderr, "Failed to create schema '%s'.\n", schemaName);
        }

        // Drop the schema with CASCADE option
        if (postgres_drop_schema(pg, schemaName, true)) {
            fmt_printf("Schema '%s' dropped successfully.\n", schemaName);
        } 
        else {
            fmt_fprintf(stderr, "Failed to drop schema '%s'.\n", schemaName);
        }

        postgres_disconnect(pg);
    } 
    else {
        fmt_fprintf(stderr, "Failed to connect to the database.\n");
    }

    postgres_deallocate(pg);
    return 0;
}
```

### Example 32 : Parameterized query with `postgres_query_params` 

```c
#include "database/postgres.h"
#include "fmt/fmt.h"

int main() {
    Postgres* pg = postgres_create();
    postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

    if (postgres_connect(pg)) {
        fmt_printf("Connected to the database successfully.\n");

        const char* insertQuery = "INSERT INTO info (name, age) VALUES ($1, $2)";
        const char* paramValues[2] = {"John Doe", "30"};
        
        PostgresResult* insertRes = postgres_query_params(pg, insertQuery, 2, paramValues);
        if (insertRes) {
            fmt_printf("Row inserted successfully.\n");
            postgres_clear_result(insertRes);
        } 
        else {
            fmt_fprintf(stderr, "Failed to insert row.\n");
        }

        const char* selectQuery = "SELECT * FROM info WHERE age = $1";
        const char* selectParamValues[1] = {"30"};
        
        PostgresResult* selectRes = postgres_query_params(pg, selectQuery, 1, selectParamValues);
        if (selectRes) {
            int nRows = postgres_num_tuples(selectRes);
            for (int i = 0; i < nRows; i++) {
                const char* name = postgres_get_value(selectRes, i, 0);
                const char* age = postgres_get_value(selectRes, i, 1);

                fmt_printf("Row %d: Name = %s, Age = %s\n", i + 1, name, age);
            }
            postgres_clear_result(selectRes);
        } 
        else {
            fmt_fprintf(stderr, "Failed to execute select query.\n");
        }

        postgres_disconnect(pg);
    } 
    else {
        fmt_fprintf(stderr, "Failed to connect to the database.\n");
    }

    postgres_deallocate(pg);
    return 0;
}
```

### Example 33 : Prepare Stament and Execute the clear with `postgres_prepare_statement` and `postgres_clear_prepared_statement`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"

int main() {
    Postgres* pg = postgres_create();
    postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

    if (postgres_connect(pg)) {
        fmt_printf("Connected to the database successfully.\n");

        const char* stmtName = "insert_info";
        const char* query = "INSERT INTO info (name, age) VALUES ($1, $2)";

        // Prepare the statement
        if (postgres_prepare_statement(pg, stmtName, query)) {
            fmt_printf("Statement '%s' prepared successfully.\n", stmtName);

            const char* paramValues[2] = {"Jane Doe", "25"};

            // Execute the prepared statement using the correct function call
            if (postgres_execute_prepared(pg, stmtName, 2, paramValues)) {
                fmt_printf("Row inserted successfully using prepared statement.\n");
            } 
            else {
                fmt_fprintf(stderr, "Failed to insert row using prepared statement.\n");
            }

            // Clear the prepared statement
            if (postgres_clear_prepared_statement(pg, stmtName)) {
                fmt_printf("Statement '%s' cleared successfully.\n", stmtName);
            } 
            else {
                fmt_fprintf(stderr, "Failed to clear statement '%s'.\n", stmtName);
            }
        } 
        else {
            fmt_fprintf(stderr, "Failed to prepare statement '%s'.\n", stmtName);
        }

        postgres_disconnect(pg);
    } 
    else {
        fmt_fprintf(stderr, "Failed to connect to the database.\n");
    }

    postgres_deallocate(pg);
    return 0;
}
```

### Example 34 : how to manage transactions in PostgreSQL with savepoints, allowing for fine-grained control over rollback operations. with `postgres_savepoint` and `postgres_rollback_to_savepoint`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

        if (postgres_connect(pg)) {
            fmt_printf("Connected to the database successfully.\n");

            if (postgres_begin_transaction(pg)) {
                fmt_printf("Transaction started.\n");

                const char* insertQuery1 = "INSERT INTO cars (brand, model, year) VALUES ('Toyota', 'Camry', '2021');";
                if (!postgres_execute_non_query(pg, insertQuery1)) {
                    fmt_fprintf(stderr, "Failed to insert data.\n");
                }

                // Create a savepoint
                const char* savepointName = "sp1";
                if (postgres_savepoint(pg, savepointName)) {
                    fmt_printf("Savepoint '%s' created.\n", savepointName);
                }

                const char* insertQuery2 = "INSERT INTO cars (brand, model, year) VALUES ('Ford', 'Mustang', '2021');";
                if (!postgres_execute_non_query(pg, insertQuery2)) {
                    fmt_fprintf(stderr, "Failed to insert data.\n");
                }

                // Rollback to savepoint
                if (postgres_rollback_to_savepoint(pg, savepointName)) {
                    fmt_printf("Rolled back to savepoint '%s'.\n", savepointName);
                }

                if (postgres_commit_transaction(pg)) {
                    fmt_printf("Transaction committed.\n");
                } 
                else {
                    fmt_fprintf(stderr, "Failed to commit transaction.\n");
                }
            } 
            else {
                fmt_fprintf(stderr, "Failed to start transaction.\n");
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
```

### Example 35 : send and get query result in async way `postgres_send_query` and `postgres_get_async_result`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

        if (postgres_connect(pg)) {
            fmt_printf("Connected to the database successfully.\n");

            const char* query = "SELECT * FROM cars;";

            // Send query asynchronously
            if (postgres_send_async_query(pg, query)) {
                fmt_printf("Query sent successfully.\n");

                // Check if the query result is ready
                PostgresResult* result;
                while ((result = postgres_get_async_result(pg)) != NULL) {
                    if (result) {
                        int nRows = postgres_num_tuples(result);
                        for (int i = 0; i < nRows; i++) {
                            const char* brand = postgres_get_value(result, i, 0);
                            const char* model = postgres_get_value(result, i, 1);
                            const char* year = postgres_get_value(result, i, 2);
                            fmt_printf("Row %d: Brand = %s, Model = %s, Year = %s\n", i + 1, brand, model, year);
                        }
                        postgres_clear_result(result);
                    }
                }
            } 
            else {
                fmt_fprintf(stderr, "Failed to send query.\n");
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
```

### Example 36 : copy data from csv file with `postgres_copy_from_csv`

```c
#include "database/postgres.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Postgres* pg = postgres_create();

    if (pg) {
        postgres_init(pg, "mydatabase", "myuser", "mypassword", "localhost", "5432");

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
```
## Conclusion

This PostgreSQL C library simplifies database interactions in C projects, providing an easy-to-use interface for connecting to PostgreSQL databases, executing queries, and managing results. The provided examples illustrate how to use the library for common database operations.