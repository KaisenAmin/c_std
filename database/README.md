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


### `Postgres* postgres_create()`  
**Purpose**: Allocates and creates a new `Postgres` structure.  
**Parameters**:  
  - None.  
**Return Value**: A pointer to the newly allocated `Postgres` structure, or `NULL` if the allocation fails.  
**Usage Case**: Used to initialize a `Postgres` structure before setting connection parameters and connecting to a database.

---

### `void postgres_init(Postgres* pg, const char* database, const char* user, const char* password, const char* host, const char* port)`  
**Purpose**: Initializes the `Postgres` structure with database connection parameters, such as database name, user, password, host, and port.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure to be initialized.  
  - `database`: The name of the database.  
  - `user`: The username for database access.  
  - `password`: The password for database access.  
  - `host`: The hostname or IP address of the database server.  
  - `port`: The port number on which the database server is listening.  
**Return Value**: Nothing (void).  
**Usage Case**: Used to set the connection parameters for the `Postgres` structure before attempting to connect to the database.

---

### `bool postgres_connect(Postgres* pg)`  
**Purpose**: Connects to the PostgreSQL database using the initialized connection parameters.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
**Return Value**: `true` if the connection is successful, `false` otherwise.  
**Usage Case**: Used to establish a connection to the PostgreSQL database after initializing the connection parameters.

---

### `bool postgres_execute_non_query(Postgres* pg, const char* command)`  
**Purpose**: Executes a non-query SQL command (e.g., `INSERT`, `UPDATE`, `DELETE`) on the PostgreSQL database.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `command`: The SQL command to be executed.  
**Return Value**: `true` if the command is executed successfully, `false` otherwise.  
**Usage Case**: Used to execute non-query SQL commands that modify the database but do not return results (e.g., `INSERT`, `UPDATE`, `DELETE`).

---

### `void postgres_disconnect(Postgres* pg)`  
**Purpose**: Disconnects from the PostgreSQL database.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
**Return Value**: Nothing (void).  
**Usage Case**: Used to close the connection to the database when it is no longer needed.

---

### `PostgresResult* postgres_query(Postgres* pg, const char* query)`  
**Purpose**: Executes a SQL query on the PostgreSQL database and returns the result.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `query`: The SQL query to be executed.  
**Return Value**: A pointer to the `PostgresResult` structure containing the query result, or `NULL` if an error occurs.  
**Usage Case**: Used to execute SQL queries that return data (e.g., `SELECT` statements).

---

### `void postgres_clear_result(PostgresResult* pgResult)`  
**Purpose**: Clears the result set returned by a query and frees the associated memory.  
**Parameters**:  
  - `pgResult`: A pointer to the `PostgresResult` structure to be cleared.  
**Return Value**: Nothing (void).  
**Usage Case**: Used to free up memory after processing a query result.

---

### `void postgres_deallocate(Postgres* pg)`  
**Purpose**: Deallocates memory and resources associated with the `Postgres` structure.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure to be deallocated.  
**Return Value**: Nothing (void).  
**Usage Case**: Used to clean up and free memory when the `Postgres` structure is no longer needed.

---

### `bool postgres_begin_transaction(Postgres* pg)`  
**Purpose**: Begins a new transaction on the PostgreSQL connection.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
**Return Value**: `true` if the transaction begins successfully, `false` otherwise.  
**Usage Case**: Used to start a transaction, allowing multiple queries to be executed as part of a single unit of work.

---

### `bool postgres_commit_transaction(Postgres* pg)`  
**Purpose**: Commits the current transaction on the PostgreSQL connection.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
**Return Value**: `true` if the transaction is committed successfully, `false` otherwise.  
**Usage Case**: Used to commit all the changes made during the current transaction.

---

### `bool postgres_rollback_transaction(Postgres* pg)`  
**Purpose**: Rolls back the current transaction on the PostgreSQL connection.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
**Return Value**: `true` if the transaction is rolled back successfully, `false` otherwise.  
**Usage Case**: Used to undo all changes made during the current transaction.

---

### `const char* postgres_get_last_error(Postgres* pg)`  
**Purpose**: Retrieves the last error message reported by the PostgreSQL connection.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
**Return Value**: A string containing the last error message, or a message indicating the connection is null.  
**Usage Case**: Used to get detailed error information from the PostgreSQL connection.

---

### `int postgres_get_affected_rows(Postgres* pg, PostgresResult* pgRes)`  
**Purpose**: Retrieves the number of rows affected by the last command executed.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `pgRes`: A pointer to the `PostgresResult` structure containing the result of the last command.  
**Return Value**: The number of affected rows, or `-1` if an error occurred.  
**Usage Case**: Used to determine how many rows were modified by a non-query command such as `INSERT`, `UPDATE`, or `DELETE`.

---

### `void postgres_print_result(PostgresResult* pgRes)`  
**Purpose**: Prints the result of a query in a tabular format.  
**Parameters**:  
  - `pgRes`: A pointer to the `PostgresResult` structure containing the query result.  
**Return Value**: Nothing (void).  
**Usage Case**: Used to display query results in a formatted table for easy readability.

---

### `int postgres_get_table_row_count(Postgres* pg, const char* tableName)`  
**Purpose**: Retrieves the number of rows in a specified table.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `tableName`: The name of the table whose row count is to be retrieved.  
**Return Value**: The number of rows in the table, or `-1` if an error occurred.  
**Usage Case**: Used to quickly get the total number of rows in a specified database table.

---

### `bool postgres_table_exists(Postgres* pg, const char* tableName)`  
**Purpose**: Checks if a table exists in the PostgreSQL database.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `tableName`: The name of the table to check for existence.  
**Return Value**: `true` if the table exists, `false` otherwise.  
**Usage Case**: Used to verify the existence of a specific table in the database before performing operations on it.

---

### `PostgresResult* postgres_list_tables(Postgres* pg)`  
**Purpose**: Lists all tables in the 'public' schema of the connected PostgreSQL database.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
**Return Value**: A `PostgresResult` structure containing the list of table names, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve a list of all tables in the `public` schema.

---

### `PostgresResult* postgres_get_table_schema(Postgres* pg, const char* tableName)`  
**Purpose**: Retrieves the schema of a specified table, including column names and data types.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `tableName`: The name of the table whose schema is to be retrieved.  
**Return Value**: A `PostgresResult` structure containing the table schema, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve detailed information about the columns and data types of a specific table.

---

### `bool postgres_execute_prepared(Postgres* pg, const char* stmtName, int nParams, const char* const* paramValues)`  
**Purpose**: Executes a prepared SQL statement with the given parameters.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `stmtName`: The name of the prepared statement.  
  - `nParams`: The number of parameters in the query.  
  - `paramValues`: An array of parameter values.  
**Return Value**: `true` if the statement was executed successfully, `false` otherwise.  
**Usage Case**: Used to execute precompiled SQL statements with specific parameter values.

---

### `PostgresResult* postgres_get_table_columns(Postgres* pg, const char* tableName)`  
**Purpose**: Retrieves the names of all columns in a specified table.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `tableName`: The name of the table whose columns are to be retrieved.  
**Return Value**: A `PostgresResult` structure containing the column names, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve a list of columns from a specific table in the database.

---

### `PostgresResult* postgres_get_table_primary_keys(Postgres* pg, const char* tableName)`  
**Purpose**: Retrieves the primary keys of a specified table.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `tableName`: The name of the table whose primary keys are to be retrieved.  
**Return Value**: A `PostgresResult` structure containing the primary keys, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve the primary keys of a table for validation, constraints checking, or schema understanding.

---

### `PostgresResult* postgres_get_table_foreign_keys(Postgres* pg, const char* tableName)`  
**Purpose**: Retrieves the foreign keys of a specified table.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `tableName`: The name of the table whose foreign keys are to be retrieved.  
**Return Value**: A `PostgresResult` structure containing the foreign keys, or `NULL` if an error occurred.  
**Usage Case**: Used to understand the relationships between tables by retrieving the foreign keys.

---

### `PostgresResult* postgres_get_table_indexes(Postgres* pg, const char* tableName)`  
**Purpose**: Retrieves the indexes of a specified table.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `tableName`: The name of the table whose indexes are to be retrieved.  
**Return Value**: A `PostgresResult` structure containing the indexes, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve the index definitions on a table to improve query performance or understand the indexing strategy.

---

### `PostgresResult* postgres_get_table_size(Postgres* pg, const char* tableName)`  
**Purpose**: Retrieves the size of a specified table in a human-readable format.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `tableName`: The name of the table whose size is to be retrieved.  
**Return Value**: A `PostgresResult` structure containing the table size, or `NULL` if an error occurred.  
**Usage Case**: Used to determine the physical storage size of a table for performance and resource management.

---

### `int postgres_get_table_index_count(Postgres* pg, const char* tableName)`  
**Purpose**: Retrieves the number of indexes on a specified table.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `tableName`: The name of the table for which the index count is to be retrieved.  
**Return Value**: The number of indexes on the table, or `-1` if an error occurred.  
**Usage Case**: Used to get the number of indexes on a table for optimizing database performance and query execution.

---

### `PostgresResult* postgres_get_column_details(Postgres* pg, const char* tableName)`  
**Purpose**: Retrieves detailed information about the columns in a specified table, including column names, data types, nullability, and default values.  
**Parameters**:  
  - `pg`: A pointer to the `Postgres` structure representing the connection.  
  - `tableName`: The name of the table whose column details are to be retrieved.  
**Return Value**: A `PostgresResult` structure containing the column details, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve comprehensive information about table columns for schema understanding and validation purposes.

---

### `const char* postgres_get_value(PostgresResult* pgRes, int row, int col)`  
**Purpose**: Retrieves the value of a specific field in a result set.  
**Parameters**:  
  - `pgRes`: Pointer to the `PostgresResult` structure containing the result set.  
  - `row`: Row index of the field to retrieve.  
  - `col`: Column index of the field to retrieve.  
**Return Value**: The value of the field as a string, or `NULL` if an error occurred.  
**Usage Case**: Used to access individual cell values in the result set from a query.

---

### `PostgresResult* postgres_get_table_constraints(Postgres* pg, const char* tableName)`  
**Purpose**: Retrieves all constraints on a specified table.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
  - `tableName`: Name of the table whose constraints are to be retrieved.  
**Return Value**: A `PostgresResult` structure containing the table constraints, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve information on primary keys, foreign keys, and other constraints for a table.

---

### `int postgres_num_tuples(const PostgresResult* pgRes)`  
**Purpose**: Retrieves the number of tuples (rows) in a result set.  
**Parameters**:  
  - `pgRes`: Pointer to the `PostgresResult` structure containing the result set.  
**Return Value**: The number of tuples (rows), or `-1` if an error occurred.  
**Usage Case**: Used to determine the number of rows in a result set after a query.

---

### `int postgres_num_fields(const PostgresResult* pgRes)`  
**Purpose**: Retrieves the number of fields (columns) in a result set.  
**Parameters**:  
  - `pgRes`: Pointer to the `PostgresResult` structure containing the result set.  
**Return Value**: The number of fields (columns), or `-1` if an error occurred.  
**Usage Case**: Used to determine the number of columns returned by a query.

---

### `int postgres_command_tuples(PostgresResult* pgRes)`  
**Purpose**: Retrieves the number of tuples (rows) affected by the last command.  
**Parameters**:  
  - `pgRes`: Pointer to the `PostgresResult` structure containing the result set.  
**Return Value**: The number of affected tuples, or `-1` if an error occurred.  
**Usage Case**: Used to check how many rows were affected by commands like `INSERT`, `UPDATE`, or `DELETE`.

---

### `int postgres_backend_pid(Postgres* pg)`  
**Purpose**: Retrieves the backend process ID of the PostgreSQL connection.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: The backend process ID, or `-1` if an error occurred.  
**Usage Case**: Used to get the process ID of the PostgreSQL backend that handles the current connection.

---

### `int postgres_binary_tuples(const PostgresResult* pgRes)`  
**Purpose**: Checks if the result set contains binary data.  
**Parameters**:  
  - `pgRes`: Pointer to the `PostgresResult` structure containing the result set.  
**Return Value**: `1` if the result set contains binary data, `0` if it contains text data, or `-1` if an error occurred.  
**Usage Case**: Used to determine if the data retrieved by a query is in binary format.

---

### `int postgres_bytes_size(const PostgresResult* pgRes, int colsNumber)`  
**Purpose**: Retrieves the size in bytes of the specified column in the result set.  
**Parameters**:  
  - `pgRes`: Pointer to the `PostgresResult` structure containing the result set.  
  - `colsNumber`: The column index to retrieve the size for.  
**Return Value**: The size in bytes of the specified column, or `-1` if an error occurred.  
**Usage Case**: Used to get the size of specific columns in the result set for memory or performance considerations.

---

### `bool postgres_is_null(const PostgresResult* pgRes, int row, int col)`  
**Purpose**: Checks if a specific field in the result set is `NULL`.  
**Parameters**:  
  - `pgRes`: Pointer to the `PostgresResult` structure containing the result set.  
  - `row`: Row index of the field to check.  
  - `col`: Column index of the field to check.  
**Return Value**: `true` if the field is `NULL`, `false` otherwise.  
**Usage Case**: Used to check for `NULL` values in a result set.

---

### `void postgres_reset(Postgres* pg)`  
**Purpose**: Resets the PostgreSQL connection by closing the existing connection and attempting to reestablish it.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: None.  
**Usage Case**: Used to reset a connection that has become unusable or needs to be reestablished.

---

### `int postgres_reset_start(Postgres* pg)`  
**Purpose**: Starts a reset of the PostgreSQL connection without blocking.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: `1` if reset is in progress, `0` if it is not, and `-1` if an error occurred.  
**Usage Case**: Used to initiate a non-blocking reset of the PostgreSQL connection.

---

### `char* postgres_db_value(const Postgres* pg)`  
**Purpose**: Retrieves the name of the connected database.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: The name of the connected database, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve the name of the current database.

---

### `char* postgres_user_value(const Postgres* pg)`  
**Purpose**: Retrieves the username used to connect to the database.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: The username, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve the username used for the connection.

---

### `char* postgres_password_value(const Postgres* pg)`  
**Purpose**: Retrieves the password used to connect to the database.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: The password, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve the password used for the connection.

---

### `char* postgres_host_value(const Postgres* pg)`  
**Purpose**: Retrieves the host name or IP address of the PostgreSQL server.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: The host name or IP address, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve the server's host information.

---

### `char* postgres_port_value(const Postgres* pg)`  
**Purpose**: Retrieves the port number of the PostgreSQL server.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: The port number, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve the port number on which the PostgreSQL server is running.

---

### `char* postgres_object_id_status(const PostgresResult* pgRes)`  
**Purpose**: Retrieves the object ID of the inserted row if the last command was an `INSERT`.  
**Parameters**:  
  - `pgRes`: Pointer to the `PostgresResult` structure containing the result of the last command.  
**Return Value**: A string representing the object ID, or `NULL` if an error occurred.  
**Usage Case**: Used after an `INSERT` statement to retrieve the ID of the newly inserted row.

---

### `char* postgres_command_status(PostgresResult* pgRes)`  
**Purpose**: Retrieves the command status string from the SQL command that generated the `PostgresResult`.  
**Parameters**:  
  - `pgRes`: Pointer to the `PostgresResult` structure containing the result of the last command.  
**Return Value**: The command status string, or `NULL` if an error occurred.  
**Usage Case**: Used to retrieve the status of the last executed SQL command.

---

### `int postgres_protocol_version(const Postgres* pg)`  
**Purpose**: Retrieves the protocol version used by the PostgreSQL connection.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: The protocol version, or `-1` if an error occurred.  
**Usage Case**: Used to check which version of the PostgreSQL protocol is being used.

---

### `int postgres_server_version(const Postgres* pg)`  
**Purpose**: Retrieves the server version of the connected PostgreSQL database.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: The server version, or `-1` if an error occurred.  
**Usage Case**: Used to check the PostgreSQL server version.

---

### `int postgres_socket_descriptor(const Postgres* pg)`  
**Purpose**: Retrieves the socket file descriptor used by the PostgreSQL connection.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: The socket file descriptor, or `-1` if an error occurred.  
**Usage Case**: Used to obtain the socket descriptor for advanced network operations.

---

### `bool postgres_is_busy(Postgres* pg)`  
**Purpose**: Checks if the PostgreSQL connection is currently busy processing a command.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: `true` if the connection is busy, `false` otherwise.  
**Usage Case**: Used to determine if the connection is processing a query.

---

### `bool postgres_is_non_blocking(const Postgres* pg)`  
**Purpose**: Checks if the PostgreSQL connection is in non-blocking mode.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: `true` if the connection is in non-blocking mode, `false` otherwise.  
**Usage Case**: Used to determine whether the connection is set to non-blocking mode.

---

### `int postgres_flush(Postgres* pg)`  
**Purpose**: Flushes any queued output data to the PostgreSQL server.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: `0` if successful, `-1` if an error occurred, or `1` if the flush could not send all data (in non-blocking mode).  
**Usage Case**: Used to send any unsent data to the PostgreSQL server.

---

### `int postgres_set_non_blocking(Postgres* pg, int state)`  
**Purpose**: Sets the PostgreSQL connection to non-blocking or blocking mode.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
  - `state`: `1` to set the connection to non-blocking mode, `0` for blocking mode.  
**Return Value**: `0` if successful, `-1` if an error occurred.  
**Usage Case**: Used to change the mode of the connection between blocking and non-blocking.

---

### `int postgres_get_line(Postgres* pg, char* buffer, int length)`  
**Purpose**: Reads a newline-terminated line from the PostgreSQL server.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
  - `buffer`: Buffer to store the line read.  
  - `length`: Maximum length of the buffer.  
**Return Value**: `0` if the entire line has been read, `1` if the buffer is full but the newline has not been read, or `-1` if an error occurred.  
**Usage Case**: Used to read lines from the PostgreSQL server output.

---

### `int postgres_get_line_async(Postgres* pg, char* buffer, int length)`  
**Purpose**: Reads a newline-terminated line asynchronously from the PostgreSQL server.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
  - `buffer`: Buffer to store the line read.  
  - `length`: Maximum length of the buffer.  
**Return Value**: `0` if the entire line has been read, `1` if the buffer is full but the newline has not been read, or `-1` if an error occurred.  
**Usage Case**: Used to asynchronously read lines from the PostgreSQL server.

---

### `int postgres_put_line(Postgres* pg, const char* buffer)`  
**Purpose**: Sends a null-terminated string to the PostgreSQL server.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
  - `buffer`: Null-terminated string to send.  
**Return Value**: `0` if successful, `-1` if an error occurred.  
**Usage Case**: Used to send data lines to the PostgreSQL server.

---

### `int postgres_put_bytes(Postgres* pg, const char* buffer, int bytes)`  
**Purpose**: Sends a non-null-terminated string (raw bytes) to the PostgreSQL server.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
  - `buffer`: Buffer containing the data to send.  
  - `bytes`: Number of bytes to send from the buffer.  
**Return Value**: `0` if successful, `-1` if an error occurred.  
**Usage Case**: Used for sending raw binary data to the server.

---

### `void postgres_trace(Postgres* pg, FILE* stream)`  
**Purpose**: Enables tracing of the client/server communication to a debugging file stream.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
  - `stream`: File stream where the trace output will be written.  
**Usage Case**: Useful for debugging communication between the client and the PostgreSQL server.

---

### `void postgres_un_trace(Postgres* pg)`  
**Purpose**: Disables tracing of the client/server communication.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Usage Case**: Used to stop tracing once debugging is completed.

---

### `PostgresResult* postgres_get_result(Postgres* pg)`  
**Purpose**: Retrieves the result of the last executed command.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: A pointer to a `PostgresResult` structure containing the command result, or `NULL` if an error occurred.  
**Usage Case**: Used to fetch the result of queries or commands executed previously.

---

### `int postgres_request_cancle(Postgres* pg)`  
**Purpose**: Sends a request to cancel the currently executed command.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: `0` if the cancellation request was successfully dispatched, or `-1` if an error occurred.  
**Usage Case**: Used to cancel long-running or unresponsive queries.

---

### `bool postgres_reconnect(Postgres* pg)`  
**Purpose**: Disconnects from the PostgreSQL database and attempts to reconnect using the stored connection parameters.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: `true` if reconnection is successful, `false` otherwise.  
**Usage Case**: Used when connection to the server is lost and reconnection is needed.

---

### `bool postgres_ping(Postgres* pg)`  
**Purpose**: Checks if the connection to the PostgreSQL database server is still alive.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
**Return Value**: `true` if the connection is alive, `false` otherwise.  
**Usage Case**: Used to verify whether the database server is reachable.

---

### `PostgresResult* postgres_query_execution_time(Postgres* pg, const char* query)`  
**Purpose**: Measures and returns the execution time of a query in addition to the query result.  
**Parameters**:  
  - `pg`: Pointer to the `Postgres` structure representing the connection.  
  - `query`: The SQL query to be executed.  
**Return Value**: A pointer to the `PostgresResult` structure containing the query result and execution time, or `NULL` if an error occurred.  
**Usage Case**: Useful for benchmarking queries and optimizing database performance.

---

### `bool postgres_create_function(Postgres* pg, const char* functionName, const char* returnType, const char* language, const char* functionBody, const char* paramDefinitions)`  
**Purpose**: Creates a user-defined function in the PostgreSQL database using a `CREATE FUNCTION` SQL command.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `functionName`: Name of the function to be created.
  - `returnType`: The return type of the function (e.g., `integer`, `text`).
  - `language`: The language used to define the function (e.g., `plpgsql`, `sql`).
  - `functionBody`: The logic of the function, written in the specified language.
  - `paramDefinitions`: The definitions of the function parameters (e.g., `param1 integer, param2 integer`).
**Return Value**: `true` if the function is successfully created, `false` otherwise.
**Usage Case**: Useful for adding custom functionality to the database through user-defined functions.

---

### `bool postgres_drop_function(Postgres* pg, const char* functionName, const char* paramDefinitions)`  
**Purpose**: Drops a user-defined function from the PostgreSQL database using a `DROP FUNCTION` SQL command.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `functionName`: Name of the function to be dropped.
  - `paramDefinitions`: The definitions of the function's parameters.
**Return Value**: `true` if the function is successfully dropped, `false` otherwise.
**Usage Case**: Useful for removing user-defined functions from the database when they are no longer needed.

---

### `bool postgres_create_view(Postgres* pg, const char* viewName, const char* query)`  
**Purpose**: Creates a view in the PostgreSQL database using a `CREATE VIEW` SQL command.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `viewName`: Name of the view to be created.
  - `query`: The SQL query that defines the view.
**Return Value**: `true` if the view is successfully created, `false` otherwise.
**Usage Case**: Useful for encapsulating complex SQL queries into reusable views for easier access and abstraction.

---

### `bool postgres_drop_view(Postgres* pg, const char* viewName)`  
**Purpose**: Drops an existing view from the PostgreSQL database using a `DROP VIEW` SQL command.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `viewName`: Name of the view to be dropped.
**Return Value**: `true` if the view is successfully dropped, `false` otherwise.
**Usage Case**: Useful for removing outdated or unnecessary views from the database.

---

### `bool postgres_create_trigger(Postgres* pg, const char* triggerName, const char* tableName, const char* timing, const char* event, const char* function)`  
**Purpose**: Creates a trigger on a specified table that invokes a function when a specified event occurs.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `triggerName`: Name of the trigger to be created.
  - `tableName`: Name of the table where the trigger is applied.
  - `timing`: Trigger timing (e.g., "BEFORE", "AFTER").
  - `event`: Event that activates the trigger (e.g., "INSERT", "UPDATE", "DELETE").
  - `function`: Function called by the trigger.
**Return Value**: `true` if the trigger is successfully created, `false` otherwise.
**Usage Case**: Useful for automating actions in response to database events.

---

### `bool postgres_drop_trigger(Postgres* pg, const char* triggerName, const char* tableName)`  
**Purpose**: Drops an existing trigger from a specified table.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `triggerName`: Name of the trigger to be dropped.
  - `tableName`: Name of the table where the trigger is located.
**Return Value**: `true` if the trigger is successfully dropped, `false` otherwise.
**Usage Case**: Useful for removing database triggers that are no longer needed.

---

### `bool postgres_create_schema(Postgres* pg, const char* schemaName)`  
**Purpose**: Creates a new schema in the PostgreSQL database.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `schemaName`: Name of the schema to be created.
**Return Value**: `true` if the schema is successfully created, `false` otherwise.
**Usage Case**: Useful for organizing and grouping database objects.

---

### `bool postgres_drop_schema(Postgres* pg, const char* schemaName, bool cascade)`  
**Purpose**: Drops an existing schema from the PostgreSQL database.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `schemaName`: Name of the schema to be dropped.
  - `cascade`: Whether to drop all dependent objects with the schema.
**Return Value**: `true` if the schema is successfully dropped, `false` otherwise.
**Usage Case**: Useful for removing schemas and their associated objects from a database.

---

### `PostgresResult* postgres_query_params(Postgres* pg, const char* query, int nParams, const char* const* paramValues)`  
**Purpose**: Executes a parameterized SQL query, which helps to prevent SQL injection by passing parameters separately.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `query`: The SQL query to be executed, with parameter placeholders (`$1`, `$2`, etc.).
  - `nParams`: The number of parameters in the query.
  - `paramValues`: Array of string values representing the parameters.
**Return Value**: A `PostgresResult` structure containing the result of the query, or `NULL` if an error occurred.
**Usage Case**: Ideal for executing safe, parameterized queries.

---

### `bool postgres_prepare_statement(Postgres* pg, const char* stmtName, const char* query)`  
**Purpose**: Prepares a SQL statement for execution, allowing the query to be executed multiple times with different parameters.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `stmtName`: Name for the prepared statement.
  - `query`: SQL query to prepare.
**Return Value**: `true` if the statement is prepared successfully, `false` otherwise.
**Usage Case**: Useful for optimizing repeated query executions by preparing the query once and executing it with different parameters.

---

### `bool postgres_clear_prepared_statement(Postgres* pg, const char* stmtName)`  
**Purpose**: Deallocates a prepared SQL statement, freeing up resources.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `stmtName`: Name of the prepared statement to be deallocated.
**Return Value**: `true` if the statement is cleared successfully, `false` otherwise.
**Usage Case**: Use this to release resources when a prepared statement is no longer needed.

---

### `bool postgres_savepoint(Postgres* pg, const char* savepointName)`  
**Purpose**: Creates a savepoint in a transaction, allowing rollback to this point without affecting the entire transaction.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `savepointName`: Name of the savepoint.
**Return Value**: `true` if the savepoint is created successfully, `false` otherwise.
**Usage Case**: Ideal for saving intermediate states in transactions.

---

### `bool postgres_rollback_to_savepoint(Postgres* pg, const char* savepointName)`  
**Purpose**: Rolls back the transaction to a specific savepoint, undoing changes made after the savepoint was created.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `savepointName`: Name of the savepoint to rollback to.
**Return Value**: `true` if the rollback is successful, `false` otherwise.
**Usage Case**: Useful for error handling within transactions without rolling back the entire transaction.

---

### `bool postgres_send_async_query(Postgres* pg, const char* query)`  
**Purpose**: Sends a SQL query to the PostgreSQL server in a non-blocking way, allowing other tasks to be performed while the query is processed.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `query`: SQL query to be sent.
**Return Value**: `true` if the query was sent successfully, `false` otherwise.
**Usage Case**: Enables non-blocking query execution, useful for high-performance or multi-tasking applications.

---

### `PostgresResult* postgres_get_async_result(Postgres* pg)`  
**Purpose**: Retrieves the result of a previously sent asynchronous query.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
**Return Value**: A `PostgresResult` structure containing the query result, or `NULL` if no result is ready yet or an error occurred.
**Usage Case**: Call this function repeatedly until the query result is available.

---

### `bool postgres_copy_from_csv(Postgres* pg, const char* tableName, const char* csvFilePath, const char* delimiter)`  
**Purpose**: Loads data from a CSV file into a specified PostgreSQL table using the `COPY` command.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `tableName`: Name of the table to copy data into.
  - `csvFilePath`: Path to the CSV file.
  - `delimiter`: Delimiter used in the CSV file (e.g., `,`).
**Return Value**: `true` if the operation is successful, `false` otherwise.
**Usage Case**: Useful for efficiently loading bulk data into a table from a CSV file.

---

---

### `bool postgres_is_connected(const Postgres* pg)`
**Purpose**: Checks if the connection to the PostgreSQL server is currently open and active without sending traffic to the server.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
**Return Value**: `true` if the connection is live, `false` otherwise (including when `pg` is NULL).
**Usage Case**: Use after `postgres_connect` to confirm the connection established successfully, or anywhere you need a quick liveness check.

---

### `const char* postgres_get_field_name(const PostgresResult* pgRes, int col)`
**Purpose**: Returns the name of the column at the given index in the result set.
**Parameters**:
  - `pgRes`: Pointer to the `PostgresResult` structure.
  - `col`: Zero-based column index.
**Return Value**: A borrowed (do not free) C-string with the column name, or NULL if `pgRes` is NULL or `col` is out of range.
**Usage Case**: Useful for self-describing result printing or dynamically building column mappings.

---

### `char* postgres_get_single_value(Postgres* pg, const char* query)`
**Purpose**: Executes a scalar query and returns the value at row 0, column 0 as a freshly-allocated string.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `query`: SQL query expected to return exactly one value.
**Return Value**: A newly allocated string the caller must `free`, or NULL on error, empty rowset, or NULL field.
**Usage Case**: Ideal for scalar queries such as `SELECT count(*) FROM ...` or `SELECT now()`.

---

### `char* postgres_escape_identifier(Postgres* pg, const char* ident)`
**Purpose**: Escapes an identifier (table or column name) using connection-aware quoting.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `ident`: The identifier string to escape.
**Return Value**: A newly allocated double-quoted escaped string the caller must `free`, or NULL on error.
**Usage Case**: Use when an identifier originates from outside your code and needs to be safely spliced into a query string.

---

### `char* postgres_escape_string(Postgres* pg, const char* str)`
**Purpose**: Escapes a string literal using connection-aware quoting to prevent SQL injection.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `str`: The string value to escape.
**Return Value**: A newly allocated single-quoted escaped string the caller must `free`, or NULL on error.
**Usage Case**: For code that builds SQL textually; prefer `postgres_query_params` for parameterised queries.

---

### `PostgresResult* postgres_query_with_time(Postgres* pg, const char* query, double* out_seconds)`
**Purpose**: Executes a SQL query, returns the result, and writes the elapsed wall-clock time in seconds to `*out_seconds`.
**Parameters**:
  - `pg`: Pointer to the `Postgres` structure representing the connection.
  - `query`: The SQL query to execute.
  - `out_seconds`: Pointer to a `double` that receives the elapsed seconds; pass NULL if timing is not needed.
**Return Value**: A pointer to a `PostgresResult` structure, or NULL on error.
**Usage Case**: The honest version of `postgres_query_execution_time` — use when you need both the result and an accurate timing measurement.

---

## Examples


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
**Result**
```
Create Table Done
```

---

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
**Result**
```
NOTICE:  relation "cars" already exists, skipping
Table created successfully.
Data inserted successfully.
```

---

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
**Result**
```
Transaction started successfully.
Transaction committed successfully.
```

---

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

                const char *insertCmd1 = "INSERT INTO cars (brand, model, year) VALUES (Toyota, 'Corolla', 2021);"; // here 
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
**Result**
```
Transaction started successfully.
Error: Failed to execute one or more commands. Rolling back.
```

---

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
**Result**
```
Transaction started successfully.
Error: ERROR:  current transaction is aborted, commands ignored until end of transaction block
```

---

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
**Result**
```
Transaction started successfully.
Number of affected rows for first command: 1
Number of affected rows for second command: 1
Transaction committed successfully.
```

---

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
**Result**
```
+--------+---------+------+
| brand  | model   | year |
+--------+---------+------+
| Toyota | Corolla | 2021 |
| Toyota | Corolla | 2021 |
| Honda  | Civic   | 2020 |
| Toyota | Corolla | 2021 |
| Honda  | Civic   | 2020 |
| Toyota | Corolla | 2021 |
| Honda  | Civic   | 2020 |
+--------+---------+------+
```

---

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
**Result**
```
Number of rows in cars table is 7
```

---

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
**Result**
```
Table 'cars' exists.
```

---

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

            if (pgRes) {
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
**Result**
```
Tables in the `public` Schema:
+------------+
| table_name |
+------------+
| cars       |
+------------+
```

---

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
**Result**
```
Schema of table 'cars':
+-------------+-------------------+
| column_name | data_type         |
+-------------+-------------------+
| brand       | character varying |
| model       | character varying |
| year        | integer           |
+-------------+-------------------+
```

---

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
**Result**
```
Statement 'insert_car' prepared successfully.
Prepared statement executed successfully.
Statement 'insert_car' cleared successfully.
```

---

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
**Result**
```
Table 'bus' created successfully.
Table 'owners' created successfully.
Primary Keys of table 'bus':
+---------+-----------+
| attname | data_type |
+---------+-----------+
| id      | integer   |
+---------+-----------+
Primary Keys of table 'owners':
+----------+-----------+
| attname  | data_type |
+----------+-----------+
| owner_id | integer   |
+----------+-----------+
```

---

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
**Result**
```
NOTICE:  relation "bus" already exists, skipping
Table 'bus' created successfully.
NOTICE:  relation "owners" already exists, skipping
Table 'owners' created successfully.
Foreign Keys of table 'owners':
+--------------------+-------------+--------------------+---------------------+
| constraint_name    | column_name | foreign_table_name | foreign_column_name |
+--------------------+-------------+--------------------+---------------------+
| owners_bus_id_fkey | bus_id      | bus                | id                  |
+--------------------+-------------+--------------------+---------------------+
```

---

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
**Result**
```
NOTICE:  relation "bus" already exists, skipping
Table 'bus' created successfully.
Index on 'brand' column created successfully.
Indexes of table 'bus':
+---------------+--------------------------------------------------------------+
| indexname     | indexdef                                                     |
+---------------+--------------------------------------------------------------+
| bus_pkey      | CREATE UNIQUE INDEX bus_pkey ON public.bus USING btree (id)  |
| idx_bus_brand | CREATE INDEX idx_bus_brand ON public.bus USING btree (brand) |
+---------------+--------------------------------------------------------------+
```

---

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
**Result**
```
NOTICE:  relation "bus" already exists, skipping
Table 'bus' created successfully.
NOTICE:  relation "idx_bus_brand" already exists, skipping
Index on 'brand' column created successfully.
Size of table 'bus':
+-------+
| size  |
+-------+
| 24 kB |
+-------+
```

---

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
**Result**
```
NOTICE:  relation "bus" already exists, skipping
Table 'bus' created successfully.
NOTICE:  relation "idx_bus_brand" already exists, skipping
Index on 'brand' column created successfully.
Number of indexes on table 'bus': 2
```

---

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
**Result**
```
Column details of table 'bus':
+-------------+-------------------+-------------+---------------------------------+
| column_name | data_type         | is_nullable | column_default                  |
+-------------+-------------------+-------------+---------------------------------+
| id          | integer           | NO          | nextval('bus_id_seq'::regclass) |
| brand       | character varying | YES         |                                 |
| model       | character varying | YES         |                                 |
| year        | integer           | YES         |                                 |
+-------------+-------------------+-------------+---------------------------------+
```

---

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

                const char* value = postgres_get_value(pgRes, 0, 0); 
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
**Result**
```
NOTICE:  relation "cars" already exists, skipping
Query Results:
+----+--------+---------+------+
| id | brand  | model   | year |
+----+--------+---------+------+
| 1  | Toyota | Corolla | 2021 |
| 2  | Honda  | Civic   | 2020 |
+----+--------+---------+------+
Brand of the first car: Toyota
```

---

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
**Result**
```
Table 'cars' created successfully.
Unique index created successfully.
Constraints of table 'cars':
+-----------------------+-----------------+-------------+--------------------+---------------------+-------------------+
| constraint_name       | constraint_type | column_name | foreign_table_name | foreign_column_name | check_clause      |
+-----------------------+-----------------+-------------+--------------------+---------------------+-------------------+
| cars_year_check       | CHECK           |             | cars               | year                | ((year > 1885))   |
| cars_pkey             | PRIMARY KEY     | id          | cars               | id                  |                   |
| 2200_68555_1_not_null | CHECK           |             |                    |                     | id IS NOT NULL    |
| 2200_68555_2_not_null | CHECK           |             |                    |                     | brand IS NOT NULL |
+-----------------------+-----------------+-------------+--------------------+---------------------+-------------------+
```


---

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
**Result**
```
Number of tuples (rows): 2
Number of fields (columns): 4
+----+--------+---------+------+
| id | brand  | model   | year |
+----+--------+---------+------+
| 1  | Toyota | Corolla | 2021 |
| 2  | Benz   | S500    | 2020 |
+----+--------+---------+------+
```

---

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
**Result**
```
Number of rows affected by the delete command: 1
Query Results:
+----+-------+-------+------+
| id | brand | model | year |
+----+-------+-------+------+
| 2  | Honda | Civic | 2020 |
+----+-------+-------+------+
```


---

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

                postgres_clear_result(pgRes);
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
**Result**
```
data is text not binary.
```

---

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
**Result**
```
Database name is mydatabase
username is myuser
password is mypassword
```

---

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
**Result**
```
Connected to the database successfully.
Reconnected to the database successfully.
```

---

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
**Result**
```
Connected to the database successfully.
Connection to the database is alive.
Tables in the database:
+------------+
| table_name |
+------------+
| bus        |
| cars       |
| owners     |
+------------+
The table 'cars' has 1 rows.
```

---

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
**Result**
```
Connected to the database successfully.
Query executed successfully.
+----+-------+-------+------+
| id | brand | model | year |
+----+-------+-------+------+
| 2  | Honda | Civic | 2020 |
+----+-------+-------+------+
```

---

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
**Result**
```
Connected to the database successfully.
NOTICE:  function add_numbers(pg_catalog.int4,pg_catalog.int4) does not exist, skipping
Function 'add_numbers' created successfully.
Function result: 5
Test passed: Function works as expected.
Function 'add_numbers' dropped successfully.
```

---

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
**Result**
```
Connected to the database successfully.
View 'my_view' created successfully.
View 'my_view' query executed successfully.
+----+-------+
| id | brand |
+----+-------+
| 2  | Honda |
+----+-------+
View 'my_view' dropped successfully.
```

---

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
            "name VARCHAR(255), "
            "age VARCHAR(255));";
        
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
**Result**
```
Connected to the database successfully.
Table 'info' created successfully.
Table 'audit_log' created successfully.
NOTICE:  function log_update() does not exist, skipping
Function 'log_update' created successfully.
Trigger 'trigger_log_update' created successfully on table 'info'.
Sample row inserted into 'info'.
Update query executed successfully, trigger should have fired.
Trigger 'trigger_log_update' dropped successfully from table 'info'.
Function 'log_update' dropped successfully.
```

---

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
**Result**
```
Connected to the database successfully.
Schema 'test_schema' created successfully.
Schema 'test_schema' dropped successfully.
```

---

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

        const char* selectQuery = "SELECT name, age FROM info WHERE age = $1";
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
**Result**
```
Connected to the database successfully.
Row inserted successfully.
Row 1: Name = John Doe, Age = 30
```


---

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
**Result**
```
Connected to the database successfully.
Statement 'insert_info' prepared successfully.
Row inserted successfully using prepared statement.
Statement 'insert_info' cleared successfully.
```


---

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
**Result**
```
Connected to the database successfully.
Transaction started.
Savepoint 'sp1' created.
Rolled back to savepoint 'sp1'.
Transaction committed.
```

---

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

            const char* query = "SELECT brand, model, year FROM cars;";

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
**Result**
```
Connected to the database successfully.
Query sent successfully.
Row 1: Brand = Honda, Model = Civic, Year = 2020
Row 2: Brand = Toyota, Model = Camry, Year = 2021
```

---

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
**Result**
```
Connected to the database successfully.
Data copied from CSV to table 'cars' successfully.
```

---

## Example A — `postgres_get_single_value` for scalars

```c
char* row_count = postgres_get_single_value(pg,
    "SELECT count(*) FROM cars");
if (row_count) {
    fmt_printf("rows = %s\n", row_count);
    free(row_count);
}
```

**Result**
```
(output depends on cars table row count)
```

---

## Example B — Safe identifier splicing

```c
const char* user_table = /* …from user input… */;
char* tbl = postgres_escape_identifier(pg, user_table);

char query[512];
snprintf(query, sizeof query, "SELECT * FROM %s LIMIT 10", tbl);
free(tbl);

PostgresResult* res = postgres_query(pg, query);
postgres_print_result(res);
postgres_clear_result(res);
```

`postgres_escape_identifier` returns a properly-quoted, escape-aware
string — much safer than `snprintf` against a raw `tableName`.

**Result**
```
(no console output — just uses escaped identifier to build a safe query)
```

---

## Example C — `postgres_is_connected` after every connect

```c
postgres_init(pg, "mydb", "user", "pw", "127.0.0.1", "5432");
postgres_connect(pg);
if (!postgres_is_connected(pg)) {
    fprintf(stderr, "connect failed: %s\n", postgres_get_last_error(pg));
    return 1;
}
```

**Result**
```
(no output if connected successfully)
```

---

## Example D — Read back column names

```c
PostgresResult* res = postgres_query(pg, "SELECT id, name, brand FROM cars LIMIT 5");
int nf = postgres_num_fields(res);
for (int i = 0; i < nf; ++i) {
    printf("col[%d] = %s\n", i, postgres_get_field_name(res, i));
}
postgres_clear_result(res);
```

**Result**
```
(prints column names from the cars table, e.g. col[0] = id, col[1] = name, col[2] = brand)
```

---

## Example E — Honest query timing

```c
double secs = 0;
PostgresResult* res = postgres_query_with_time(pg,
    "SELECT * FROM huge_table", &secs);
fmt_printf("query took %.3f ms\n", secs * 1000.0);
postgres_clear_result(res);
```

**Result**
```
(prints timing in milliseconds, e.g. "query took 1.234 ms")
```

---

## Notes — bug fixes from the audit

- **`postgres_create` left fields uninitialised.** A caller who
  forgot `postgres_init` and went straight to `postgres_deallocate`
  hit undefined behaviour reading garbage pointers. Now uses
  `calloc` so every field starts as NULL.
- **`postgres_connect` left a dangling handle after failure.** It
  called `PQfinish` but kept `pg->connection` set to the freed
  pointer; later NULL-checks would falsely succeed. Now sets it to
  NULL.
- **`CON_INFO_SIZE` was a fixed 256 bytes.** Long
  passwords/hostnames silently truncated. The conninfo buffer is now
  sized to the actual operands.
- **NULL-check on `pg` was missing in many functions** — `postgres_disconnect`,
  `postgres_query`, `postgres_execute_non_query`, `postgres_get_last_error`,
  `postgres_get_affected_rows`, `postgres_begin/commit/rollback_transaction`
  all crashed if you passed `pg == NULL`. Every entry point now
  validates its receiver first.
- **`postgres_execute_prepared` rejected SELECT prepared statements**
  because it only accepted `PGRES_COMMAND_OK`. Now accepts both
  `COMMAND_OK` (DML) and `TUPLES_OK` (queries that return rows).
- **`postgres_execute_prepared` also crashed when `paramValues` was
  NULL** even with `nParams == 0`. The check is now correct: NULL is
  rejected only when `nParams > 0`.
- **`postgres_print_result` used a VLA** (`int widths[nFields]`) which
  is not portable to MSVC and unsafe on the stack for wide rowsets.
  Replaced with a `malloc` + matching `free`.
- **`postgres_query_execution_time` dropped its timing on the floor.**
  The docstring promised "result + execution time" but
  `PostgresResult` has no time field. The old call now delegates to
  the new honest `postgres_query_with_time(pg, q, &secs)` which
  writes the timing to an out-param.

---

## License

This project is open-source and available under [ISC License].