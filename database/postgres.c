#include "postgres.h"
#include "../string/std_string.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CON_INFO_SIZE 256 

static void print_line(int nFields, int *widths) {
    for (int i = 0; i < nFields; i++) {
        fmt_printf("+");
        for (int j = 0; j < widths[i] + 2; j++) {
            fmt_printf("-");
        }
    }
    fmt_printf("+\n");
}

/**
 * @brief Creates a new Postgres structure.
 * 
 * @return A pointer to the newly allocated Postgres structure, or NULL if the allocation fails.
 */
Postgres* postgres_create() {
    Postgres* pg = (Postgres*) malloc(sizeof(Postgres));

    if (pg) {
        return pg;
    }
    else {
        return NULL;
    }
}

/**
 * @brief Initializes the Postgres structure with database connection parameters.
 * 
 * @param pg Pointer to the Postgres structure to be initialized.
 * @param database Name of the database.
 * @param user Username for database access.
 * @param password Password for database access.
 * @param host Hostname or IP address of the database server.
 * @param port Port number on which the database server is listening.
 */
void postgres_init(Postgres* pg, const char* database, const char* user, const char* password, const char* host, const char* port) {
    pg->database = string_strdup(database);
    pg->user = string_strdup(user);
    pg->password = string_strdup(password);
    pg->host = string_strdup(host);
    pg->port = string_strdup(port);
    pg->connection = NULL;
}

/**
 * @brief Connects to the PostgreSQL database using the initialized connection parameters.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return true if the connection is successful, false otherwise.
 */
bool postgres_connect(Postgres* pg) {
    char conninfo[CON_INFO_SIZE];

    snprintf(conninfo, sizeof(conninfo), "dbname=%s user=%s password=%s host=%s port=%s", 
             pg->database, pg->user, pg->password, pg->host, pg->port);
    pg->connection = PQconnectdb(conninfo);

    if (PQstatus(pg->connection) != CONNECTION_OK) {
        fmt_fprintf(stderr, "Error: Connection to database failed %s\n", PQerrorMessage(pg->connection));
        PQfinish(pg->connection);
        return false;
    }
    return true;
}

/**
 * @brief Executes a non-query command (e.g., INSERT, UPDATE, DELETE) on the PostgreSQL database.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param command The SQL command to be executed.
 * 
 * @return true if the command is executed successfully, false otherwise.
 */
bool postgres_execute_non_query(Postgres* pg, const char* command) {
    if (pg->connection != NULL) {
        PGresult *res = PQexec(pg->connection, command);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            fmt_fprintf(stderr, "Error: Command execution failed %s\n", PQerrorMessage(pg->connection));
            PQclear(res);

            return false;
        }

        PQclear(res);
        return true;
    }
    else {
        fmt_fprintf(stderr, "Error: connection of postgres is null.\n");
        return false;
    }
}

/**
 * @brief Disconnects from the PostgreSQL database.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 */
void postgres_disconnect(Postgres* pg) {
    if (pg->connection != NULL) {
        PQfinish(pg->connection);
        pg->connection = NULL;
    }
}

/**
 * @brief Executes a SQL query on the PostgreSQL database and returns the result.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param query The SQL query to be executed.
 * 
 * @return A pointer to the PostgresResult structure containing the query result, or NULL if an error occurs.
 */
PostgresResult* postgres_query(Postgres* pg, const char* query) {
    if (query == NULL) {
        fmt_fprintf(stderr, "Error: query is null.\n");
        return NULL;
    }
    else if (pg->connection != NULL) {
        PGresult *res = PQexec(pg->connection, query);

        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            fmt_fprintf(stderr, "Error: Query failed %s\n", PQerrorMessage(pg->connection));
            PQclear(res);
            return NULL;
        }

        PostgresResult* pgRes = (PostgresResult*) malloc(sizeof(PostgresResult));
        pgRes->result = res;
        return pgRes;
    } 
    else {
        fmt_fprintf(stderr, "Error: connection of postgres is null\n");
        return NULL;
    }
}

/**
 * @brief Clears the result set returned by a query and frees the associated memory.
 * 
 * @param pgResult Pointer to the PostgresResult structure to be cleared.
 */
void postgres_clear_result(PostgresResult* pgResult) {
    if (pgResult) {
        PQclear(pgResult->result);
        free(pgResult);
    }
}

/**
 * @brief Deallocates memory and resources associated with the Postgres structure.
 * 
 * @param pg Pointer to the Postgres structure to be deallocated.
 */
void postgres_deallocate(Postgres* pg) {
    if (pg) {
        if (pg->database) {
            free(pg->database);
            pg->database = NULL;
        }
        if (pg->user) {
            free(pg->user);
            pg->user = NULL;
        }
        if (pg->password) {
            free(pg->password);
            pg->password = NULL;
        }
        if (pg->host) {
            free(pg->host);
            pg->host = NULL;
        }
        if (pg->port) {
            free(pg->port);
            pg->port = NULL;
        }
        if (pg->connection) {
            PQfinish(pg->connection);
            pg->connection = NULL;
        }
    }
}

/**
 * @brief Begins a new transaction on the PostgreSQL connection.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return true if the transaction begins successfully, false otherwise.
 */
bool postgres_begin_transaction(Postgres* pg) {
    const char *beginCommand = "BEGIN";
    if (pg->connection != NULL) {
        PGresult *res = PQexec(pg->connection, beginCommand);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            char error_message[512];
            snprintf(error_message, sizeof(error_message), "Begin transaction failed: %s", PQerrorMessage(pg->connection));
            fmt_fprintf(stderr, "%s\n", error_message);

            PQclear(res);
            return false;
        }
        PQclear(res);
        return true;
    } 
    else {
        const char *error_message = "Connection of postgres is null.";
        fmt_fprintf(stderr, "Error: %s\n", error_message);

        return false;
    }
}

/**
 * @brief Commits the current transaction on the PostgreSQL connection.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return true if the transaction was committed successfully, false otherwise.
 */
bool postgres_commit_transaction(Postgres* pg) {
    const char *commitCommand = "COMMIT";

    if (pg->connection != NULL) {
        PGresult *res = PQexec(pg->connection, commitCommand);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            char error_message[512];
            snprintf(error_message, sizeof(error_message), "Commit transaction failed: %s", PQerrorMessage(pg->connection));
            fmt_fprintf(stderr, "%s\n", error_message);

            PQclear(res);
            return false;
        }
        PQclear(res);
        return true;
    } 
    else {
        const char *error_message = "Connection of postgres is null.";
        fmt_fprintf(stderr, "Error: %s\n", error_message);
        return false;
    }
}

/**
 * @brief Rolls back the current transaction on the PostgreSQL connection.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return true if the transaction was rolled back successfully, false otherwise.
 */
bool postgres_rollback_transaction(Postgres* pg) {
    const char *rollbackCommand = "ROLLBACK";

    if (pg->connection != NULL) {
        PGresult *res = PQexec(pg->connection, rollbackCommand);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            char error_message[512];
            snprintf(error_message, sizeof(error_message), "Rollback transaction failed: %s", PQerrorMessage(pg->connection));
            fmt_fprintf(stderr, "%s\n", error_message);
            
            PQclear(res);
            return false;
        }
        PQclear(res);
        return true;
    } 
    else {
        const char *error_message = "Connection of postgres is null.";
        fmt_fprintf(stderr, "Error: %s\n", error_message);
        
        return false;
    }
}

/**
 * @brief Retrieves the last error message reported by the PostgreSQL connection.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return A string containing the last error message, or a message indicating the connection is null.
 */
const char* postgres_get_last_error(Postgres* pg) {
    if (pg->connection != NULL) {
        return PQerrorMessage(pg->connection);
    } 
    else {
        return "Connection of postgres is null.";
    }
}

/**
 * @brief Retrieves the number of rows affected by the last command executed.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param pgRes Pointer to the PostgresResult structure containing the result of the last command.
 * 
 * @return The number of affected rows, or -1 if an error occurred.
 */
int postgres_get_affected_rows(Postgres* pg, PostgresResult *pgRes) {
    if (pg->connection != NULL && pgRes->result != NULL) {
        return atoi(PQcmdTuples(pgRes->result));
    } 
    else {
        fmt_fprintf(stderr, "Error: connection of postgres or result is null.\n");
        return -1;
    }
}

/**
 * @brief Prints the result of a query in a tabular format.
 * 
 * @param pgRes Pointer to the PostgresResult structure containing the query result.
 */
void postgres_print_result(PostgresResult* pgRes) {
    if (pgRes && pgRes->result) {
        int nFields = PQnfields(pgRes->result);
        int nRows = PQntuples(pgRes->result);
        int widths[nFields];

        for (int i = 0; i < nFields; i++) {
            widths[i] = string_length_cstr(PQfname(pgRes->result, i));

            for (int j = 0; j < nRows; j++) {
                int len = string_length_cstr(PQgetvalue(pgRes->result, j, i));
                if (len > widths[i]) {
                    widths[i] = len;
                }
            }
        }

        print_line(nFields, widths);

        for (int i = 0; i < nFields; i++) {
            fmt_printf("| %-*s ", widths[i], PQfname(pgRes->result, i));
        }
        fmt_printf("|\n");

        print_line(nFields, widths);

        for (int i = 0; i < nRows; i++) {
            for (int j = 0; j < nFields; j++) {
                fmt_printf("| %-*s ", widths[j], PQgetvalue(pgRes->result, i, j));
            }
            fmt_printf("|\n");
        }
        print_line(nFields, widths);
    } 
    else {
        fmt_fprintf(stderr, "Error: pgRes or pgRes->result is null.\n");
    }
}

/**
 * @brief Retrieves the number of rows in a specified table.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param tableName Name of the table whose row count is to be retrieved.
 * 
 * @return The number of rows in the table, or -1 if an error occurred.
 */
int postgres_get_table_row_count(Postgres* pg, const char* tableName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return -1;
    }
    else if (tableName == NULL) {
        fmt_fprintf(stderr, "Error: tableName is null.\n");
        return -1;
    }

    char query[CON_INFO_SIZE];
    snprintf(query, sizeof(query), "SELECT COUNT(*) from %s", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        int rowCount = atoi(PQgetvalue(pgRes->result, 0, 0));
        postgres_clear_result(pgRes);

        return rowCount;
    }
    else {
        fmt_fprintf(stderr, "Error: Quert failed %s\n", postgres_get_last_error(pg));
        postgres_clear_result(pgRes);

        return -1;
    }
}

/**
 * @brief Checks if a table exists in the PostgreSQL database.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param tableName Name of the table to check for existence.
 * 
 * @return true if the table exists, false otherwise.
 */
bool postgres_table_exists(Postgres* pg, const char* tableName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return -1;
    }
    else if (tableName == NULL) {
        fmt_fprintf(stderr, "Error: tableName is null.\n");
        return -1;
    }

    char query[CON_INFO_SIZE * 2];
    snprintf(query, sizeof(query), 
        "SELECT EXISTS ("
        "SELECT FROM information_schema.tables "
        "WHERE table_schema = 'public' "
        "AND table_name = '%s'"
        ")", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        // `t` represents the boolean value true in PostgreSQL when checking for existence
        bool exists = strcmp(PQgetvalue(pgRes->result, 0, 0), "t") == 0;
        postgres_clear_result(pgRes);

        return exists;
    }
    else {
        fmt_fprintf(stderr, "Error: Query failed %s.\n", postgres_get_last_error(pg));
        postgres_clear_result(pgRes);

        return false;
    }
}

/**
 * @brief Lists all tables in the 'public' schema of the connected PostgreSQL database.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return A PostgresResult structure containing the list of table names, or NULL if an error occurred.
 */
PostgresResult* postgres_list_tables(Postgres* pg) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return NULL;       
    }
    
    
    const char* query = 
        "SELECT table_name "
        "FROM information_schema.tables "
        "WHERE table_schema = 'public' "
        "ORDER BY table_name;";
    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        return pgRes;
    }
    else {
        fmt_fprintf(stderr, "Error: Query failed %s.\n", postgres_get_last_error(pg));
        if (pgRes) {
            postgres_clear_result(pgRes);
        }
        return NULL;
    }
}

/**
 * @brief Retrieves the schema of a specified table, including column names and data types.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param tableName Name of the table whose schema is to be retrieved.
 * 
 * @return A PostgresResult structure containing the table schema, or NULL if an error occurred.
 */
PostgresResult* postgres_get_table_schema(Postgres* pg, const char* tableName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return NULL;
    } 
    else if (tableName == NULL) {
        fmt_fprintf(stderr, "Error: tableName is null.\n");
        return NULL;
    }

    char query[CON_INFO_SIZE * 2];
    snprintf(query, sizeof(query), 
        "SELECT column_name, data_type "
        "FROM information_schema.columns "
        "WHERE table_schema = 'public' "
        "AND table_name = '%s';", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        return pgRes;
    } 
    else {
        fmt_fprintf(stderr, "Error: Query failed %s.\n", postgres_get_last_error(pg));
        if (pgRes) {
            postgres_clear_result(pgRes);
        }
        return NULL;
    }
}

/**
 * @brief Executes a prepared SQL statement with the given parameters.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param stmtName Name of the prepared statement.
 * @param nParams Number of parameters in the query.
 * @param paramValues Array of parameter values.
 * 
 * @return true if the statement was executed successfully, false otherwise.
 */
bool postgres_execute_prepared(Postgres* pg, const char* stmtName, int nParams, const char* const* paramValues) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return false;
    } 
    else if (stmtName == NULL || paramValues == NULL) {
        fmt_fprintf(stderr, "Error: statement name or parameters are null.\n");
        return false;
    }

    PGresult* res = PQexecPrepared(pg->connection, stmtName, nParams, paramValues, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Statement execution failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}


/**
 * @brief Retrieves the names of all columns in a specified table.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param tableName Name of the table whose columns are to be retrieved.
 * 
 * @return A PostgresResult structure containing the column names, or NULL if an error occurred.
 */
PostgresResult* postgres_get_table_columns(Postgres* pg, const char* tableName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection object is null.\n");
        return NULL;
    }
    else if (tableName == NULL) {
        fmt_fprintf(stderr, "Error: tableName is null.\n");
        return NULL;
    }

    char query[CON_INFO_SIZE * 2];
    snprintf(query, sizeof(query), 
        "SELECT column_name "
        "FROM information_schema.columns "
        "WHERE table_schema = 'public' "
        "AND table_name = '%s';", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        return pgRes;
    } 
    else {
        fmt_fprintf(stderr, "Error: Query failed %s.\n", postgres_get_last_error(pg));
        if (pgRes) {
            postgres_clear_result(pgRes);
        }
        return NULL;
    }
}

/**
 * @brief Retrieves the primary keys of a specified table.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param tableName Name of the table whose primary keys are to be retrieved.
 * 
 * @return A PostgresResult structure containing the primary keys, or NULL if an error occurred.
 */
PostgresResult* postgres_get_table_primary_keys(Postgres* pg, const char* tableName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return NULL;
    } 
    else if (tableName == NULL) {
        fmt_fprintf(stderr, "Error: tableName is null.\n");
        return NULL;
    }

    char query[CON_INFO_SIZE * 2];
    snprintf(query, sizeof(query), 
        "SELECT a.attname, format_type(a.atttypid, a.atttypmod) AS data_type "
        "FROM pg_index i "
        "JOIN pg_attribute a ON a.attrelid = i.indrelid "
        "AND a.attnum = ANY(i.indkey) "
        "WHERE i.indrelid = '%s'::regclass "
        "AND i.indisprimary;", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        return pgRes;
    } 
    else {
        fmt_fprintf(stderr, "Error: Query failed %s.\n", postgres_get_last_error(pg));
        if (pgRes) {
            postgres_clear_result(pgRes);
        }
        return NULL;
    }
}

/**
 * @brief Retrieves the foreign keys of a specified table.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param tableName Name of the table whose foreign keys are to be retrieved.
 * 
 * @return A PostgresResult structure containing the foreign keys, or NULL if an error occurred.
 */
PostgresResult* postgres_get_table_foreign_keys(Postgres* pg, const char* tableName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return NULL;
    } 
    else if (tableName == NULL) {
        fmt_fprintf(stderr, "Error: tableName is null.\n");
        return NULL;
    }

    char query[CON_INFO_SIZE * 4];
    snprintf(query, sizeof(query),
        "SELECT "
        "    tc.constraint_name, "
        "    kcu.column_name, "
        "    ccu.table_name AS foreign_table_name, "
        "    ccu.column_name AS foreign_column_name "
        "FROM "
        "    information_schema.table_constraints AS tc "
        "    JOIN information_schema.key_column_usage AS kcu "
        "      ON tc.constraint_name = kcu.constraint_name "
        "      AND tc.table_schema = kcu.table_schema "
        "    JOIN information_schema.constraint_column_usage AS ccu "
        "      ON ccu.constraint_name = tc.constraint_name "
        "WHERE tc.constraint_type = 'FOREIGN KEY' AND tc.table_name='%s';", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        return pgRes;
    } 
    else {
        fmt_fprintf(stderr, "Error: Query failed %s.\n", postgres_get_last_error(pg));
        if (pgRes) {
            postgres_clear_result(pgRes);
        }
        return NULL;
    }
}

/**
 * @brief Retrieves the indexes of a specified table.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param tableName Name of the table whose indexes are to be retrieved.
 * 
 * @return A PostgresResult structure containing the indexes, or NULL if an error occurred.
 */
PostgresResult* postgres_get_table_indexes(Postgres* pg, const char* tableName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return NULL;
    } 
    else if (tableName == NULL) {
        fmt_fprintf(stderr, "Error: tableName is null.\n");
        return NULL;
    }

    char query[CON_INFO_SIZE * 4];
    snprintf(query, sizeof(query),
        "SELECT indexname, indexdef "
        "FROM pg_indexes "
        "WHERE tablename='%s';", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        return pgRes;
    } 
    else {
        fmt_fprintf(stderr, "Error: Query failed %s.\n", postgres_get_last_error(pg));
        if (pgRes) {
            postgres_clear_result(pgRes);
        }
        return NULL;
    }
}

/**
 * @brief Retrieves the size of a specified table in a human-readable format.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param tableName Name of the table whose size is to be retrieved.
 * 
 * @return A PostgresResult structure containing the table size, or NULL if an error occurred.
 */
PostgresResult* postgres_get_table_size(Postgres* pg, const char* tableName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return NULL;
    } 
    else if (tableName == NULL) {
        fmt_fprintf(stderr, "Error: tableName is null.\n");
        return NULL;
    }

    char query[CON_INFO_SIZE * 4];
    snprintf(query, sizeof(query),
        "SELECT pg_size_pretty(pg_total_relation_size('%s')) AS size;", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        return pgRes;
    } 
    else {
        fmt_fprintf(stderr, "Error: Query failed %s.\n", postgres_get_last_error(pg));
        if (pgRes) {
            postgres_clear_result(pgRes);
        }
        return NULL;
    }
}

/**
 * @brief Retrieves the number of indexes on a specified table.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param tableName Name of the table for which the index count is to be retrieved.
 * @return The number of indexes on the table, or -1 if an error occurred.
 */
int postgres_get_table_index_count(Postgres* pg, const char* tableName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return -1;
    } 
    else if (tableName == NULL) {
        fmt_fprintf(stderr, "Error: tableName is null.\n");
        return -1;
    }

    char query[CON_INFO_SIZE * 4];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) AS index_count "
        "FROM pg_indexes "
        "WHERE tablename='%s';", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        int index_count = atoi(PQgetvalue(pgRes->result, 0, 0));
        postgres_clear_result(pgRes);
        return index_count;
    } 
    else {
        fmt_fprintf(stderr, "Error: Query failed %s.\n", postgres_get_last_error(pg));
        if (pgRes) {
            postgres_clear_result(pgRes);
        }
        return -1;
    }
}

/**
 * @brief Retrieves detailed information about the columns in a specified table.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param tableName Name of the table whose column details are to be retrieved.
 * 
 * @return A PostgresResult structure containing the column details, or NULL if an error occurred.
 */
PostgresResult* postgres_get_column_details(Postgres* pg, const char* tableName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return NULL;
    } 
    else if (tableName == NULL) {
        fmt_fprintf(stderr, "Error: tableName is null.\n");
        return NULL;
    }

    char query[CON_INFO_SIZE * 4];
    snprintf(query, sizeof(query), 
        "SELECT column_name, data_type, is_nullable, column_default "
        "FROM information_schema.columns "
        "WHERE table_schema = 'public' "
        "AND table_name = '%s';", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        return pgRes;
    } 
    else {
        fmt_fprintf(stderr, "Error: Query failed %s.\n", postgres_get_last_error(pg));
        if (pgRes) {
            postgres_clear_result(pgRes);
        }
        return NULL;
    }
}

/**
 * @brief Retrieves the value of a specific field in a result set.
 * 
 * @param pgRes Pointer to the PostgresResult structure containing the result set.
 * @param row Row index of the field to retrieve.
 * @param col Column index of the field to retrieve.
 * 
 * @return The value of the field as a string, or NULL if an error occurred.
 */
const char* postgres_get_value(PostgresResult* pgRes, int row, int col) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: PostgresResult or its result is null.\n");
        return NULL;
    }

    if (row < 0 || row >= PQntuples(pgRes->result)) {
        fmt_fprintf(stderr, "Error: Row index out of bounds.\n");
        return NULL;
    }

    if (col < 0 || col >= PQnfields(pgRes->result)) {
        fmt_fprintf(stderr, "Error: Column index out of bounds.\n");
        return NULL;
    }

    return PQgetvalue(pgRes->result, row, col);
}

/**
 * @brief Retrieves all constraints on a specified table.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param tableName Name of the table whose constraints are to be retrieved.
 * 
 * @return A PostgresResult structure containing the table constraints, or NULL if an error occurred.
 */
PostgresResult* postgres_get_table_constraints(Postgres* pg, const char* tableName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return NULL;
    } 
    else if (tableName == NULL) {
        fmt_fprintf(stderr, "Error: tableName is null.\n");
        return NULL;
    }

    char query[CON_INFO_SIZE * 4];
    snprintf(query, sizeof(query), 
        "SELECT "
        "tc.constraint_name, "
        "tc.constraint_type, "
        "kcu.column_name, "
        "ccu.table_name AS foreign_table_name, "
        "ccu.column_name AS foreign_column_name, "
        "chk.check_clause "
        "FROM information_schema.table_constraints AS tc "
        "LEFT JOIN information_schema.key_column_usage AS kcu "
        "ON tc.constraint_name = kcu.constraint_name "
        "LEFT JOIN information_schema.constraint_column_usage AS ccu "
        "ON ccu.constraint_name = tc.constraint_name "
        "LEFT JOIN information_schema.check_constraints AS chk "
        "ON tc.constraint_name = chk.constraint_name "
        "WHERE tc.table_name = '%s' AND tc.table_schema = 'public';", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        return pgRes;
    } 
    else {
        fmt_fprintf(stderr, "Error: Query failed %s.\n", postgres_get_last_error(pg));
        if (pgRes) {
            postgres_clear_result(pgRes);
        }
        return NULL;
    }
}

/**
 * @brief Retrieves the number of tuples (rows) in a result set.
 * 
 * @param pgRes Pointer to the PostgresResult structure containing the result set.
 * @return The number of tuples (rows), or -1 if an error occurred.
 */
int postgres_num_tuples(const PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: PostgresResult or its result is null.\n");
        return -1;
    } 

    return PQntuples(pgRes->result);
}

/**
 * @brief Retrieves the number of fields (columns) in a result set.
 * 
 * @param pgRes Pointer to the PostgresResult structure containing the result set.
 * @return The number of fields (columns), or -1 if an error occurred.
 */
int postgres_num_fields(const PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: PostgresResult or its result is null.\n");
        return -1;
    }

    return PQnfields(pgRes->result);
}

/**
 * @brief Retrieves the number of tuples (rows) affected by the last command.
 * 
 * @param pgRes Pointer to the PostgresResult structure containing the result set.
 * @return The number of affected tuples, or -1 if an error occurred.
 */
int postgres_command_tuples(PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: PostgresResult or its result is null.\n");
        return -1;
    }

    return atoi(PQcmdTuples(pgRes->result));
}

/**
 * @brief Retrieves the backend process ID of the PostgreSQL connection.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return The backend process ID, or -1 if an error occurred.
 */
int postgres_backend_pid(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: Postgres or its connection is null.\n");
        return -1;
    }  

    return PQbackendPID(pg->connection);
}

/**
 * @brief Checks if the result set contains binary data.
 * 
 * @param pgRes Pointer to the PostgresResult structure containing the result set.
 * @return 1 if the result set contains binary data, 0 if it contains text data, or -1 if an error occurred.
 */
int postgres_binary_tuples(const PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: PostgresResult or result fields is null.\n");
        return -1;
    }

    return PQbinaryTuples(pgRes->result);
}

/**
 * @brief Retrieves the size in bytes of the specified column in the result set.
 * 
 * @param pgRes Pointer to the PostgresResult structure containing the result set.
 * @param colsNumber The column index to retrieve the size for.
 * @return The size in bytes of the specified column, or -1 if an error occurred.
 */
int postgres_bytes_size(const PostgresResult* pgRes, int colsNumber) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: pgRes or pgRes->result is null.\n");
        return -1;
    }
    else if (colsNumber < 0) {
        fmt_fprintf(stderr, "Error: colsNumber start at zero.\n");
        return -1;
    }

    return PQfsize(pgRes->result, colsNumber);
}

/**
 * @brief Checks if a specific field in the result set is NULL.
 * 
 * @param pgRes Pointer to the PostgresResult structure containing the result set.
 * @param row Row index of the field to check.
 * @param col Column index of the field to check.
 * 
 * @return true if the field is NULL, false if it is not NULL.
 */
bool postgres_is_null(const PostgresResult* pgRes, int row, int col) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: pgRes or pgRes->result is null.\n");
        return false;
    } 
    else if (row < 0 || row >= postgres_num_tuples(pgRes)) {
        fmt_fprintf(stderr, "Error: Row index out of bounds.\n");
        return false;
    }
    else if (col < 0 || col >= postgres_num_fields(pgRes)) {
        fmt_fprintf(stderr, "Error: Column index out of bounds.\n");
        return false;
    }

    return (bool)PQgetisnull(pgRes->result, row, col);
}

/**
 * @brief Resets the PostgreSQL connection by closing the existing connection and attempting to reestablish it.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 */
void postgres_reset(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return;
    }
    else {
        PQreset(pg->connection);
    }
}

/**
 * @brief Starts a reset of the PostgreSQL connection without blocking.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return 1 if reset is in progress, 0 if it is not, and -1 if an error occurred.
 */
int postgres_reset_start(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }
    else {
        return PQresetStart(pg->connection);
    }
}

/**
 * @brief Retrieves the name of the connected database.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return The name of the connected database, or NULL if an error occurred.
 */
char* postgres_db_value(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return NULL;
    }
    else {
        return PQdb(pg->connection);
    }
}

/**
 * @brief Retrieves the username used to connect to the database.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return The username, or NULL if an error occurred.
 */
char* postgres_user_value(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return NULL;
    }
    else {
        return PQuser(pg->connection);
    }
}

/**
 * @brief Retrieves the password used to connect to the database.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return The password, or NULL if an error occurred.
 */
char* postgres_password_value(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return NULL;
    }
    else {
        return PQpass(pg->connection);
    }
}

/**
 * @brief Retrieves the host name or IP address of the PostgreSQL server.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return The host name or IP address, or NULL if an error occurred.
 */
char* postgres_host_value(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return NULL;
    }
    else {
        return PQhost(pg->connection);
    }
}

/**
 * @brief Retrieves the port number of the PostgreSQL server.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return The port number, or NULL if an error occurred.
 */
char* postgres_port_value(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return NULL;
    }
    else {
        return PQport(pg->connection);
    }
}

/**
 * @brief Retrieves the object ID of the inserted row, if the last command was an INSERT.
 * 
 * @param pgRes Pointer to the PostgresResult structure containing the result of the last command.
 * @return A string representing the object ID, or NULL if an error occurred.
 */
char* postgres_object_id_status(const PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: pgRes or pgRes->result is null.\n");
        return NULL;
    }
    else {
        return PQoidStatus(pgRes->result);
    }
}

/**
 * @brief Retrieves the command status string from the SQL command that generated the PostgresResult.
 * 
 * @param pgRes Pointer to the PostgresResult structure containing the result of the last command.
 * @return The command status string, or NULL if an error occurred.
 */
char* postgres_command_status(PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: pgRes or pgRes->result is null.\n");
        return NULL;
    }
    else {
        return PQcmdStatus(pgRes->result);
    }
}

/**
 * @brief Retrieves the protocol version used by the PostgreSQL connection.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return The protocol version, or -1 if an error occurred.
 */
int postgres_protocol_version(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }

    return PQprotocolVersion(pg->connection);
}

/**
 * @brief Retrieves the server version of the connected PostgreSQL database.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return The server version, or -1 if an error occurred.
 */
int postgres_server_version(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }

    return PQserverVersion(pg->connection);
}

/**
 * @brief Retrieves the socket file descriptor used by the PostgreSQL connection.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return The socket file descriptor, or -1 if an error occurred.
 */
int postgres_socket_descriptor(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }
    
    return PQsocket(pg->connection);
}

/**
 * @brief Checks if the PostgreSQL connection is currently busy processing a command.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return true if the connection is busy, false otherwise.
 */
bool postgres_is_busy(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return false;
    }

    return (bool)PQisBusy(pg->connection);
}

/**
 * @brief Checks if the PostgreSQL connection is in non-blocking mode.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return true if the connection is in non-blocking mode, false otherwise.
 */
bool postgres_is_non_blocking(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return false;
    }

    return (bool)PQisnonblocking(pg->connection);
}

/**
 * @brief Flushes any queued output data to the PostgreSQL server.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return 0 if successful, -1 if an error occurred, or 1 if the flush could not send all data (in non-blocking mode).
 */
int postgres_flush(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }

    return PQflush(pg->connection);
}

/**
 * @brief Sets the PostgreSQL connection to non-blocking or blocking mode.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param state 1 to set the connection to non-blocking mode, 0 to set it to blocking mode.
 * 
 * @return 0 if successful, -1 if an error occurred.
 */
int postgres_set_non_blocking(Postgres* pg, int state) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }

    return PQsetnonblocking(pg->connection, state);
}

/**
 * @brief Reads a newline-terminated line from the PostgreSQL server.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param buffer Buffer to store the line read.
 * @param length Maximum length of the buffer.
 * 
 * @return 0 if the entire line has been read, 1 if the buffer is full but the newline has not been read, 
 * or -1 if an error occurred.
 */
int postgres_get_line(Postgres* pg, char* buffer, int length) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }
    else if (buffer == NULL) {
        fmt_fprintf(stderr, "Error: buffer is null.\n");
        return -1;
    }

    return PQgetline(pg->connection, buffer, length);
}

/**
 * @brief Reads a newline-terminated line asynchronously from the PostgreSQL server.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param buffer Buffer to store the line read.
 * @param length Maximum length of the buffer.
 * 
 * @return 0 if the entire line has been read, 1 if the buffer is full but the newline has not been read, or -1 if an error occurred.
 */
int postgres_get_line_async(Postgres* pg, char* buffer, int length) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }
    else if (buffer == NULL) {
        fmt_fprintf(stderr, "Error: buffer is null.\n");
        return -1;
    }

    return PQgetlineAsync(pg->connection, buffer, length);
}

/**
 * @brief Sends a null-terminated string to the PostgreSQL server.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param buffer Null-terminated string to send.
 * 
 * @return 0 if successful, -1 if an error occurred.
 */
int postgres_put_line(Postgres* pg, const char* buffer) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }
    else if (buffer == NULL) {
        fmt_fprintf(stderr, "Error: buffer is null.\n");
        return -1;
    }

    return PQputline(pg->connection, buffer);
}

/**
 * @brief Sends a non-null-terminated string to the PostgreSQL server.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param buffer Buffer containing the data to send.
 * @param bytes Number of bytes to send from the buffer.
 * 
 * @return 0 if successful, -1 if an error occurred.
 */
int postgres_put_bytes(Postgres* pg, const char* buffer, int bytes) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }
    else if (buffer == NULL) {
        fmt_fprintf(stderr, "Error: buffer is null.\n");
        return -1;
    }

    return PQputnbytes(pg->connection, buffer, bytes);
}

/**
 * @brief Enables tracing of the client/server communication to a debugging file stream.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param stream File stream where the trace output will be written.
 */
void postgres_trace(Postgres* pg, FILE* stream) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return;
    }
    else if (stream == NULL) {
        fmt_fprintf(stderr, "Error: stream is null.\n");
        return;
    }

    PQtrace(pg->connection, stream);
}

/**
 * @brief Disables tracing of the client/server communication.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 */
void postgres_un_trace(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return;
    }

    PQuntrace(pg->connection);
}

/**
 * @brief Retrieves the result of the last executed command.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return Pointer to a PostgresResult structure containing the command result, or NULL if an error occurred.
 */
PostgresResult* postgres_get_result(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return NULL;
    }

    PGresult *res = PQgetResult(pg->connection);

    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        fmt_fprintf(stderr, "Error: Query failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return NULL;
    }

    PostgresResult* pgRes = (PostgresResult*) malloc(sizeof(PostgresResult));

    if (!pgRes) {
        fmt_fprintf(stderr, "Error: object creationg failed in PostgresResult.\n");
        PQclear(res);
        return NULL;
    }

    pgRes->result = res;
    return pgRes;
}

/**
 * @brief Sends a request to cancel the currently executed command.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return 0 if the cancellation request was successfully dispatched, or -1 if an error occurred.
 */
int postgres_request_cancle(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }

    return PQrequestCancel(pg->connection);
}

/**
 * @brief Disconnects from the PostgreSQL database and attempts to reconnect using the stored connection parameters.
 * 
 * This function is useful for handling scenarios where the connection to the PostgreSQL database is lost 
 * due to network issues, server timeouts, or other reasons. It first disconnects the existing connection
 * (if any), then attempts to reconnect using the previously initialized connection parameters.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return true if reconnection is successful, false otherwise.
 * 
 * @note Ensure that the `Postgres` structure has been properly initialized with the connection parameters 
 *       before calling this function.
 */
bool postgres_reconnect(Postgres* pg) {
    if (pg == NULL) {
        fmt_fprintf(stderr, "Error: Postgres structure is null.\n");
        return false;
    }

    // Disconnect the existing connection if any
    postgres_disconnect(pg);

    // Attempt to reconnect
    if (!postgres_connect(pg)) {
        fmt_fprintf(stderr, "Error: Reconnection failed.\n");
        return false;
    }

    return true;
}

/**
 * @brief Checks if the connection to the PostgreSQL database server is still alive.
 * 
 * This function uses the `PQping` function from the libpq library to check the status of the connection.
 * It does not attempt to reconnect; it simply verifies if the connection is alive or not.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return true if the connection is alive, false otherwise.
 * 
 * @note Ensure that the `Postgres` structure has been properly initialized and connected before calling this function.
 */
bool postgres_ping(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: Postgres structure or connection is null.\n");
        return false;
    }

    char conninfo[CON_INFO_SIZE];
    snprintf(conninfo, sizeof(conninfo), "dbname=%s user=%s password=%s host=%s port=%s", 
             pg->database, pg->user, pg->password, pg->host, pg->port);

    PGPing ping_status = PQping(conninfo);

    if (ping_status == PQPING_OK) {
        return true;
    } 
    else {
        fmt_fprintf(stderr, "Error: Database server is unreachable.\n");
        return false;
    }
}

/**
 * @brief Measures and returns the execution time of a query.
 * 
 * This function executes a given SQL query on the PostgreSQL database, measures the time taken for the query to execute, 
 * and returns the result along with the execution time.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param query The SQL query to be executed.
 * @return A pointer to the PostgresResult structure containing the query result and execution time, or NULL if an error occurs.
 */
PostgresResult* postgres_query_execution_time(Postgres* pg, const char* query) {
    if (query == NULL) {
        fmt_fprintf(stderr, "Error: query is null.\n");
        return NULL;
    } 
    else if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: connection of postgres is null.\n");
        return NULL;
    }

    clock_t start_time = clock();
    PGresult *res = PQexec(pg->connection, query);
    clock_t end_time = clock();

    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        fmt_fprintf(stderr, "Error: Query failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return NULL;
    }

    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    fmt_printf("Query executed in %.6f seconds.\n", execution_time);

    PostgresResult* pgRes = (PostgresResult*) malloc(sizeof(PostgresResult));
    if (!pgRes) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in PostgresResult.\n");
        PQclear(res);
        return NULL;
    }

    pgRes->result = res;

    return pgRes;
}

/**
 * @brief Creates a user-defined function in the PostgreSQL database.
 * 
 * This function constructs and executes a SQL `CREATE FUNCTION` command to create a new user-defined function 
 * in the PostgreSQL database using the specified parameters.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param functionName The name of the function to be created.
 * @param returnType The return type of the function (e.g. "integer", "text").
 * @param language The language used to define the function (e.g., "plpgsql", "sql").
 * @param functionBody The body of the function, containing the function logic.
 * @param paramDefinitions The definitions of the parameters (e.g., "param1 integer, param2 integer").
 * 
 * @return true if the function is created successfully, false otherwise.
 */
bool postgres_create_function(Postgres* pg, const char* functionName, const char* returnType, const char* language, const char* functionBody, const char* paramDefinitions) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: connection of postgres is null.\n");
        return false;
    } 
    else if (functionName == NULL || returnType == NULL || language == NULL || functionBody == NULL || paramDefinitions == NULL) {
        fmt_fprintf(stderr, "Error: One or more function parameters are null.\n");
        return false;
    }

    // First, drop the function if it already exists
    char dropQuery[4096];
    snprintf(dropQuery, sizeof(dropQuery), "DROP FUNCTION IF EXISTS %s(%s);", functionName, paramDefinitions);
    PGresult *dropRes = PQexec(pg->connection, dropQuery);
    if (PQresultStatus(dropRes) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Failed to drop existing function %s\n", PQerrorMessage(pg->connection));
        PQclear(dropRes);
        return false;
    }
    PQclear(dropRes);

    // Now, create the new function
    char createQuery[4096];
    snprintf(createQuery, sizeof(createQuery),
        "CREATE FUNCTION %s(%s) RETURNS %s AS $$ %s $$ LANGUAGE %s;",
        functionName, paramDefinitions, returnType, functionBody, language);

    PGresult *createRes = PQexec(pg->connection, createQuery);

    if (PQresultStatus(createRes) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Function creation failed %s\n", PQerrorMessage(pg->connection));
        PQclear(createRes);
        return false;
    }

    PQclear(createRes);
    return true;
}

/**
 * @brief Drops a user-defined function from the PostgreSQL database.
 * 
 * This function constructs and executes a SQL `DROP FUNCTION` command to remove a user-defined function 
 * from the PostgreSQL database.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param functionName The name of the function to be dropped.
 * @param paramDefinitions The definitions of the parameters (e.g., "param1 integer, param2 integer").
 * 
 * @return true if the function is dropped successfully, false otherwise.
 */
bool postgres_drop_function(Postgres* pg, const char* functionName, const char* paramDefinitions) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: connection of postgres is null.\n");
        return false;
    } 
    else if (functionName == NULL || paramDefinitions == NULL) {
        fmt_fprintf(stderr, "Error: functionName or paramDefinitions is null.\n");
        return false;
    }

    char dropQuery[4096];
    snprintf(dropQuery, sizeof(dropQuery), "DROP FUNCTION IF EXISTS %s(%s);", functionName, paramDefinitions);
    PGresult *dropRes = PQexec(pg->connection, dropQuery);

    if (PQresultStatus(dropRes) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Failed to drop function %s\n", PQerrorMessage(pg->connection));
        PQclear(dropRes);
        return false;
    }

    PQclear(dropRes);
    return true;
}

/**
 * @brief Creates a view in the PostgreSQL database.
 * 
 * This function constructs and executes a SQL `CREATE VIEW` command to create a new view 
 * in the PostgreSQL database using the specified name and query.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param viewName The name of the view to be created.
 * @param query The SQL query that defines the view.
 * 
 * @return true if the view is created successfully, false otherwise.
 */
bool postgres_create_view(Postgres* pg, const char* viewName, const char* query) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: connection of postgres is null.\n");
        return false;
    } 
    else if (viewName == NULL || query == NULL) {
        fmt_fprintf(stderr, "Error: viewName or query is null.\n");
        return false;
    }

    char createQuery[4096];
    snprintf(createQuery, sizeof(createQuery), "CREATE VIEW %s AS %s;", viewName, query);
    PGresult *res = PQexec(pg->connection, createQuery);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: View creation failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

/**
 * @brief Drops an existing view from the PostgreSQL database.
 * 
 * This function constructs and executes a SQL `DROP VIEW` command to remove a view 
 * from the PostgreSQL database.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param viewName The name of the view to be dropped.
 * 
 * @return true if the view is dropped successfully, false otherwise.
 */
bool postgres_drop_view(Postgres* pg, const char* viewName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: connection of postgres is null.\n");
        return false;
    } 
    else if (viewName == NULL) {
        fmt_fprintf(stderr, "Error: viewName is null.\n");
        return false;
    }

    char dropQuery[4096];
    snprintf(dropQuery, sizeof(dropQuery), "DROP VIEW IF EXISTS %s;", viewName);
    PGresult *res = PQexec(pg->connection, dropQuery);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: View drop failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

/**
 * @brief Creates a trigger on a specified table in the PostgreSQL database.
 * 
 * This function constructs and executes a SQL `CREATE TRIGGER` command to create a trigger 
 * on the specified table. The trigger will call a specified function when a specified event occurs.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param triggerName The name of the trigger to be created.
 * @param tableName The name of the table on which the trigger will be created.
 * @param timing The timing of the trigger (e.g., "BEFORE", "AFTER").
 * @param event The event that will fire the trigger (e.g., "INSERT", "UPDATE", "DELETE").
 * @param function The function that will be called by the trigger.
 * 
 * @return true if the trigger is created successfully, false otherwise.
 */
bool postgres_create_trigger(Postgres* pg, const char* triggerName, const char* tableName, const char* timing, const char* event, const char* function) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: connection of postgres is null.\n");
        return false;
    } 
    else if (triggerName == NULL || tableName == NULL || timing == NULL || event == NULL || function == NULL) {
        fmt_fprintf(stderr, "Error: One or more trigger parameters are null.\n");
        return false;
    }

    char createQuery[4096];
    snprintf(createQuery, sizeof(createQuery),
        "CREATE TRIGGER %s %s %s ON %s FOR EACH ROW EXECUTE FUNCTION %s();",
        triggerName, timing, event, tableName, function);
    PGresult *res = PQexec(pg->connection, createQuery);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Trigger creation failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

/**
 * @brief Drops an existing trigger from a specified table in the PostgreSQL database.
 * 
 * This function constructs and executes a SQL `DROP TRIGGER` command to remove a trigger 
 * from the specified table.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param triggerName The name of the trigger to be dropped.
 * @param tableName The name of the table from which the trigger will be dropped.
 * 
 * @return true if the trigger is dropped successfully, false otherwise.
 */
bool postgres_drop_trigger(Postgres* pg, const char* triggerName, const char* tableName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: connection of postgres is null.\n");
        return false;
    } 
    else if (triggerName == NULL || tableName == NULL) {
        fmt_fprintf(stderr, "Error: triggerName or tableName is null.\n");
        return false;
    }

    char dropQuery[4096];
    snprintf(dropQuery, sizeof(dropQuery), "DROP TRIGGER IF EXISTS %s ON %s;", triggerName, tableName);
    PGresult *res = PQexec(pg->connection, dropQuery);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Trigger drop failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

/**
 * @brief Creates a new schema in the PostgreSQL database.
 * 
 * This function constructs and executes a SQL `CREATE SCHEMA` command to create a new schema
 * in the PostgreSQL database using the specified schema name.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param schemaName The name of the schema to be created.
 * 
 * @return true if the schema is created successfully, false otherwise.
 */
bool postgres_create_schema(Postgres* pg, const char* schemaName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: Connection of postgres is null.\n");
        return false;
    } 
    else if (schemaName == NULL) {
        fmt_fprintf(stderr, "Error: Schema name is null.\n");
        return false;
    }

    char query[1024];
    snprintf(query, sizeof(query), "CREATE SCHEMA %s;", schemaName);

    PGresult *res = PQexec(pg->connection, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Schema creation failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

/**
 * @brief Drops an existing schema in the PostgreSQL database.
 * 
 * This function constructs and executes a SQL `DROP SCHEMA` command to drop an existing schema
 * in the PostgreSQL database. The schema can be dropped with the `CASCADE` option to drop all dependent objects.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param schemaName The name of the schema to be dropped.
 * @param cascade If true, all dependent objects will be dropped as well.
 * 
 * @return true if the schema is dropped successfully, false otherwise.
 */
bool postgres_drop_schema(Postgres* pg, const char* schemaName, bool cascade) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: Connection of postgres is null.\n");
        return false;
    } 
    else if (schemaName == NULL) {
        fmt_fprintf(stderr, "Error: Schema name is null.\n");
        return false;
    }

    char query[1024];
    snprintf(query, sizeof(query), "DROP SCHEMA %s%s;", schemaName, cascade ? " CASCADE" : "");

    PGresult *res = PQexec(pg->connection, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Schema drop failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

/**
 * @brief Executes a SQL query with parameters in the PostgreSQL database.
 * 
 * This function allows the execution of parameterized SQL queries to prevent SQL injection.
 * The parameters are passed separately from the query, and PostgreSQL will handle their proper escaping.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param query The SQL query to be executed. Use placeholders like `$1`, `$2`, etc., for the parameters.
 * @param nParams The number of parameters in the query.
 * @param paramValues An array of string values representing the parameters to be bound to the query.
 * 
 * @return A PostgresResult structure containing the result of the query, or NULL if an error occurred.
 */
PostgresResult* postgres_query_params(Postgres* pg, const char* query, int nParams, const char* const* paramValues) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: Connection of postgres is null.\n");
        return NULL;
    } 
    else if (query == NULL) {
        fmt_fprintf(stderr, "Error: Query string is null.\n");
        return NULL;
    } 
    else if (nParams > 0 && paramValues == NULL) {
        fmt_fprintf(stderr, "Error: Parameter values are null.\n");
        return NULL;
    }

    PGresult* res = PQexecParams(pg->connection, query, nParams, NULL, paramValues, NULL, NULL, 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Query failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return NULL;
    }

    PostgresResult* pgRes = (PostgresResult*)malloc(sizeof(PostgresResult));
    if (pgRes == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation for PostgresResult failed.\n");
        PQclear(res);
        return NULL;
    }

    pgRes->result = res;
    return pgRes;
}

/**
 * @brief Prepares a SQL statement with a given name for later execution.
 *
 * This function prepares a SQL statement, which can be executed multiple times with different parameters.
 *
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param stmtName The name to be assigned to the prepared statement.
 * @param query The SQL query to prepare.
 *
 * @return true if the statement is prepared successfully, false otherwise.
 */
bool postgres_prepare_statement(Postgres* pg, const char* stmtName, const char* query) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: connection of postgres is null.\n");
        return false;
    } 
    else if (stmtName == NULL || query == NULL) {
        fmt_fprintf(stderr, "Error: Statement name or query is null.\n");
        return false;
    }

    PGresult *res = PQprepare(pg->connection, stmtName, query, 0, NULL);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Statement preparation failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

/**
 * @brief Deallocates a prepared statement to free up resources.
 *
 * This function clears a prepared statement, which frees up the associated resources in the PostgreSQL database.
 *
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param stmtName The name of the prepared statement to be deallocated.
 *
 * @return true if the statement is cleared successfully, false otherwise.
 */
bool postgres_clear_prepared_statement(Postgres* pg, const char* stmtName) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: connection of postgres is null.\n");
        return false;
    } 
    else if (stmtName == NULL) {
        fmt_fprintf(stderr, "Error: Statement name is null.\n");
        return false;
    }

    char query[256];
    snprintf(query, sizeof(query), "DEALLOCATE %s;", stmtName);

    PGresult *res = PQexec(pg->connection, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Statement deallocation failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

/**
 * @brief Create a savepoint in the current transaction.
 *
 * This function creates a savepoint within the current transaction, allowing you to rollback to this point
 * without affecting the entire transaction.
 *
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param savepointName The name of the savepoint to create.
 * 
 * @return true if the savepoint was created successfully, false otherwise.
 */
bool postgres_savepoint(Postgres* pg, const char* savepointName) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: Postgres connection is null.\n");
        return false;
    }

    if (savepointName == NULL) {
        fmt_fprintf(stderr, "Error: Savepoint name is null.\n");
        return false;
    }

    char query[256];
    snprintf(query, sizeof(query), "SAVEPOINT %s;", savepointName);

    PGresult* res = PQexec(pg->connection, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Failed to create savepoint %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

/**
 * @brief Rollback to a specific savepoint within a transaction.
 *
 * This function rolls back the current transaction to a specific savepoint, undoing any changes made after the savepoint was created.
 *
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param savepointName The name of the savepoint to rollback to.
 * 
 * @return true if the rollback was successful, false otherwise.
 */
bool postgres_rollback_to_savepoint(Postgres* pg, const char* savepointName) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: Postgres connection is null.\n");
        return false;
    }

    if (savepointName == NULL) {
        fmt_fprintf(stderr, "Error: Savepoint name is null.\n");
        return false;
    }

    char query[256];
    snprintf(query, sizeof(query), "ROLLBACK TO SAVEPOINT %s;", savepointName);

    PGresult* res = PQexec(pg->connection, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Failed to rollback to savepoint %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

/**
 * @brief Send a query to the server without waiting for the result.
 *
 * This function sends a query to the PostgreSQL server in a non-blocking manner,
 * allowing the application to perform other tasks while the server processes the query.
 *
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param query The SQL query to be sent.
 * @return true if the query was sent successfully, false otherwise.
 */
bool postgres_send_async_query(Postgres* pg, const char* query) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: Postgres connection is null.\n");
        return false;
    }

    if (query == NULL) {
        fmt_fprintf(stderr, "Error: Query is null.\n");
        return false;
    }

    if (PQsendQuery(pg->connection, query) == 0) {
        fmt_fprintf(stderr, "Error: Failed to send query %s\n", PQerrorMessage(pg->connection));
        return false;
    }

    return true;
}

/**
 * @brief Retrieve the result of an asynchronous query when it's ready.
 *
 * This function retrieves the result of a previously sent asynchronous query. It should be called repeatedly
 * until the result is fully available.
 *
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return A pointer to a PostgresResult structure containing the result, or NULL if an error occurs or no result is ready yet.
 */
PostgresResult* postgres_get_async_result(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: Postgres connection is null.\n");
        return NULL;
    }

    PGresult* res = PQgetResult(pg->connection);

    if (res == NULL) {
        return NULL;  // No more results
    }

    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        fmt_fprintf(stderr, "Error: Query failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return NULL;
    }

    PostgresResult* pgRes = (PostgresResult*) malloc(sizeof(PostgresResult));
    if (pgRes == NULL) {
        fmt_fprintf(stderr, "Error: Failed to allocate memory for PostgresResult.\n");
        PQclear(res);
        return NULL;
    }

    pgRes->result = res;
    return pgRes;
}

/**
 * @brief Copies data from a CSV file into a specified table using PostgreSQL's COPY command.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @param tableName Name of the table to copy data into.
 * @param csvFilePath Path to the CSV file.
 * @param delimiter Character used to separate values in the CSV file (e.g., ',').
 * 
 * @return true if the operation is successful, false otherwise.
 */
bool postgres_copy_from_csv(Postgres* pg, const char* tableName, const char* csvFilePath, const char* delimiter) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return false;
    }

    char query[1024];
    snprintf(query, sizeof(query),
        "COPY %s(brand, model, year) FROM STDIN WITH CSV HEADER DELIMITER '%c';",
        tableName, delimiter[0]);

    PGresult* res = PQexec(pg->connection, query);
    if (PQresultStatus(res) != PGRES_COPY_IN) {
        fmt_fprintf(stderr, "Error: COPY operation failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    FILE* file = fopen(csvFilePath, "r");
    if (file == NULL) {
        fmt_fprintf(stderr, "Error: Could not open CSV file %s\n", csvFilePath);
        PQclear(res);
        return false;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (PQputCopyData(pg->connection, buffer, strlen(buffer)) != 1) {
            fmt_fprintf(stderr, "Error: Failed to send CSV data to server %s\n", PQerrorMessage(pg->connection));
            fclose(file);
            PQclear(res);
            return false;
        }
    }

    fclose(file);

    if (PQputCopyEnd(pg->connection, NULL) != 1) {
        fmt_fprintf(stderr, "Error: Failed to complete COPY operation %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}
