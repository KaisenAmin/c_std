/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Postgres
*/

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "postgres.h"
#include "../string/std_string.h"



#define CON_INFO_SIZE 256 

static void print_line(int nFields, int *widths) {
    for (int i = 0; i < nFields; i++) {
        printf("+");
        for (int j = 0; j < widths[i] + 2; j++) {
            printf("-");
        }
    }
    printf("+\n");
}

/**
 * @brief Creates a new Postgres structure.
 * 
 * @return A pointer to the newly allocated Postgres structure, or NULL if the allocation fails.
 */
Postgres* postgres_create() {
    Postgres* pg = (Postgres*) malloc(sizeof(Postgres));

    if (pg) {
        POSTGRES_LOG("[postgres_create] Postgres structure created successfully.");
        return pg;
    } else {
        POSTGRES_LOG("[postgres_create] Error: Failed to allocate memory for Postgres structure.");
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

    POSTGRES_LOG("[postgres_init] Postgres initialized with database: %s, user: %s, host: %s, port: %s", database, user, host, port);
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
        POSTGRES_LOG("[postgres_connect] Error: Connection to database failed: %s", PQerrorMessage(pg->connection));
        PQfinish(pg->connection);
        return false;
    }

    POSTGRES_LOG("[postgres_connect] Successfully connected to database: %s", pg->database);
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
            POSTGRES_LOG("[postgres_execute_non_query] Error: Command execution failed: %s", PQerrorMessage(pg->connection));
            PQclear(res);
            return false;
        }

        POSTGRES_LOG("[postgres_execute_non_query] Command executed successfully: %s", command);
        PQclear(res);
        return true;
    } 
    else {
        POSTGRES_LOG("[postgres_execute_non_query] Error: Connection is NULL.");
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
        POSTGRES_LOG("[postgres_disconnect] Disconnected from the database.");
    } 
    else {
        POSTGRES_LOG("[postgres_disconnect] Warning: Connection is already NULL.");
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
        POSTGRES_LOG("[postgres_query] Error: Query is NULL.");
        return NULL;
    }

    if (pg->connection != NULL) {
        PGresult *res = PQexec(pg->connection, query);

        if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
            POSTGRES_LOG("[postgres_query] Error: Query failed: %s", PQerrorMessage(pg->connection));
            PQclear(res);
            return NULL;
        }

        PostgresResult* pgRes = (PostgresResult*) malloc(sizeof(PostgresResult));
        pgRes->result = res;
        POSTGRES_LOG("[postgres_query] Query executed successfully: %s", query);
        return pgRes;
    } 
    else {
        POSTGRES_LOG("[postgres_query] Error: Connection is NULL.");
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
        POSTGRES_LOG("[postgres_clear_result] Result cleared successfully.");
    }
    else {
        POSTGRES_LOG("[postgres_clear_result] Warning: PostgresResult is NULL.");
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

        POSTGRES_LOG("[postgres_deallocate] Postgres structure deallocated successfully.");
    } 
    else {
        POSTGRES_LOG("[postgres_deallocate] Warning: Postgres structure is NULL.");
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
            POSTGRES_LOG("[postgres_begin_transaction] Error: Begin transaction failed: %s", PQerrorMessage(pg->connection));
            PQclear(res);
            return false;
        }

        PQclear(res);
        POSTGRES_LOG("[postgres_begin_transaction] Transaction begun successfully.");

        return true;
    } 
    else {
        POSTGRES_LOG("[postgres_begin_transaction] Error: Connection is NULL.");
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
            POSTGRES_LOG("[postgres_commit_transaction] Error: Commit transaction failed: %s", PQerrorMessage(pg->connection));
            PQclear(res);
            return false;
        }

        PQclear(res);
        POSTGRES_LOG("[postgres_commit_transaction] Transaction committed successfully.");

        return true;
    } 
    else {
        POSTGRES_LOG("[postgres_commit_transaction] Error: Connection is NULL.");
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
            POSTGRES_LOG("[postgres_rollback_transaction] Error: Rollback transaction failed: %s", PQerrorMessage(pg->connection));
            PQclear(res);
            return false;
        }

        PQclear(res);
        POSTGRES_LOG("[postgres_rollback_transaction] Transaction rolled back successfully.");
        
        return true;
    } 
    else {
        POSTGRES_LOG("[postgres_rollback_transaction] Error: Connection is NULL.");
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
        const char* errorMsg = PQerrorMessage(pg->connection);
        POSTGRES_LOG("[postgres_get_last_error] Last error: %s", errorMsg);
        return errorMsg;
    } 
    else {
        POSTGRES_LOG("[postgres_get_last_error] Error: Connection is NULL.");
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
        int affectedRows = atoi(PQcmdTuples(pgRes->result));
        POSTGRES_LOG("[postgres_get_affected_rows] Rows affected: %d", affectedRows);
        return affectedRows;
    } 
    else {
        POSTGRES_LOG("[postgres_get_affected_rows] Error: Connection or result is NULL.");
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

        // Calculate column widths
        for (int i = 0; i < nFields; i++) {
            widths[i] = string_length_cstr(PQfname(pgRes->result, i));

            for (int j = 0; j < nRows; j++) {
                int len = string_length_cstr(PQgetvalue(pgRes->result, j, i));
                if (len > widths[i]) {
                    widths[i] = len;
                }
            }
        }

        // Print header
        print_line(nFields, widths);
        for (int i = 0; i < nFields; i++) {
            printf("| %-*s ", widths[i], PQfname(pgRes->result, i));
        }
        printf("|\n");
        print_line(nFields, widths);

        // Print rows
        for (int i = 0; i < nRows; i++) {
            for (int j = 0; j < nFields; j++) {
                printf("| %-*s ", widths[j], PQgetvalue(pgRes->result, i, j));
            }
            printf("|\n");
        }
        print_line(nFields, widths);

        POSTGRES_LOG("[postgres_print_result] Query result printed successfully.");
    } 
    else {
        POSTGRES_LOG("[postgres_print_result] Error: pgRes or pgRes->result is NULL.");
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
        POSTGRES_LOG("[postgres_get_table_row_count] Error: Postgres connection is NULL.");
        return -1;
    }
    if (tableName == NULL) {
        POSTGRES_LOG("[postgres_get_table_row_count] Error: Table name is NULL.");
        return -1;
    }

    char query[CON_INFO_SIZE];
    snprintf(query, sizeof(query), "SELECT COUNT(*) from %s", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);
    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        int rowCount = atoi(PQgetvalue(pgRes->result, 0, 0));
        postgres_clear_result(pgRes);
        POSTGRES_LOG("[postgres_get_table_row_count] Row count for table '%s': %d", tableName, rowCount);
        return rowCount;
    } 
    else {
        POSTGRES_LOG("[postgres_get_table_row_count] Error: Query failed: %s", postgres_get_last_error(pg));
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
        POSTGRES_LOG("[postgres_table_exists] Error: Postgres connection is NULL.");
        return false;
    }
    if (tableName == NULL) {
        POSTGRES_LOG("[postgres_table_exists] Error: Table name is NULL.");
        return false;
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
        bool exists = strcmp(PQgetvalue(pgRes->result, 0, 0), "t") == 0;
        postgres_clear_result(pgRes);
        POSTGRES_LOG("[postgres_table_exists] Table '%s' exists: %s", tableName, exists ? "true" : "false");
        return exists;
    } 
    else {
        POSTGRES_LOG("[postgres_table_exists] Error: Query failed: %s", postgres_get_last_error(pg));
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
        POSTGRES_LOG("[postgres_list_tables] Error: Postgres connection is NULL.");
        return NULL;
    }

    const char* query = 
        "SELECT table_name "
        "FROM information_schema.tables "
        "WHERE table_schema = 'public' "
        "ORDER BY table_name;";
    
    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        POSTGRES_LOG("[postgres_list_tables] Successfully retrieved list of tables.");
        return pgRes;
    } 
    else {
        POSTGRES_LOG("[postgres_list_tables] Error: Query failed: %s", postgres_get_last_error(pg));
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
        POSTGRES_LOG("[postgres_get_table_schema] Error: Postgres connection is NULL.");
        return NULL;
    }
    if (tableName == NULL) {
        POSTGRES_LOG("[postgres_get_table_schema] Error: Table name is NULL.");
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
        POSTGRES_LOG("[postgres_get_table_schema] Successfully retrieved schema for table '%s'.", tableName);
        return pgRes;
    } 
    else {
        POSTGRES_LOG("[postgres_get_table_schema] Error: Query failed: %s", postgres_get_last_error(pg));
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
        POSTGRES_LOG("[postgres_execute_prepared] Error: Postgres connection is NULL.");
        return false;
    }
    if (stmtName == NULL || paramValues == NULL) {
        POSTGRES_LOG("[postgres_execute_prepared] Error: Statement name or parameters are NULL.");
        return false;
    }

    PGresult* res = PQexecPrepared(pg->connection, stmtName, nParams, paramValues, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_execute_prepared] Error: Statement execution failed: %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    POSTGRES_LOG("[postgres_execute_prepared] Statement executed successfully.");
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
        POSTGRES_LOG("[postgres_get_table_columns] Error: Postgres connection object is NULL.");
        return NULL;
    }
    if (tableName == NULL) {
        POSTGRES_LOG("[postgres_get_table_columns] Error: Table name is NULL.");
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
        POSTGRES_LOG("[postgres_get_table_columns] Successfully retrieved columns for table '%s'.", tableName);
        return pgRes;
    } 
    else {
        POSTGRES_LOG("[postgres_get_table_columns] Error: Query failed: %s", postgres_get_last_error(pg));
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
        POSTGRES_LOG("[postgres_get_table_primary_keys] Error: Postgres connection is NULL.");
        return NULL;
    }
    if (tableName == NULL) {
        POSTGRES_LOG("[postgres_get_table_primary_keys] Error: Table name is NULL.");
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
        POSTGRES_LOG("[postgres_get_table_primary_keys] Successfully retrieved primary keys for table '%s'.", tableName);
        return pgRes;
    } 
    else {
        POSTGRES_LOG("[postgres_get_table_primary_keys] Error: Query failed: %s", postgres_get_last_error(pg));
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
        POSTGRES_LOG("[postgres_get_table_foreign_keys] Error: Postgres connection is NULL.");
        return NULL;
    }
    if (tableName == NULL) {
        POSTGRES_LOG("[postgres_get_table_foreign_keys] Error: Table name is NULL.");
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
        POSTGRES_LOG("[postgres_get_table_foreign_keys] Successfully retrieved foreign keys for table '%s'.", tableName);
        return pgRes;
    } 
    else {
        POSTGRES_LOG("[postgres_get_table_foreign_keys] Error: Query failed: %s", postgres_get_last_error(pg));
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
        POSTGRES_LOG("[postgres_get_table_indexes] Error: Postgres connection is NULL.");
        return NULL;
    }
    if (tableName == NULL) {
        POSTGRES_LOG("[postgres_get_table_indexes] Error: Table name is NULL.");
        return NULL;
    }

    char query[CON_INFO_SIZE * 4];
    snprintf(query, sizeof(query),
        "SELECT indexname, indexdef "
        "FROM pg_indexes "
        "WHERE tablename='%s';", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        POSTGRES_LOG("[postgres_get_table_indexes] Successfully retrieved indexes for table '%s'.", tableName);
        return pgRes;
    } 
    else {
        POSTGRES_LOG("[postgres_get_table_indexes] Error: Query failed: %s", postgres_get_last_error(pg));
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
        POSTGRES_LOG("[postgres_get_table_size] Error: Postgres connection is NULL.");
        return NULL;
    }
    if (tableName == NULL) {
        POSTGRES_LOG("[postgres_get_table_size] Error: Table name is NULL.");
        return NULL;
    }

    char query[CON_INFO_SIZE * 4];
    snprintf(query, sizeof(query),
        "SELECT pg_size_pretty(pg_total_relation_size('%s')) AS size;", tableName);

    PostgresResult* pgRes = postgres_query(pg, query);

    if (pgRes && PQresultStatus(pgRes->result) == PGRES_TUPLES_OK) {
        POSTGRES_LOG("[postgres_get_table_size] Successfully retrieved size for table '%s'.", tableName);
        return pgRes;
    } 
    else {
        POSTGRES_LOG("[postgres_get_table_size] Error: Query failed: %s", postgres_get_last_error(pg));
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
        POSTGRES_LOG("[postgres_get_table_index_count] Error: Postgres connection is NULL.");
        return -1;
    }
    if (tableName == NULL) {
        POSTGRES_LOG("[postgres_get_table_index_count] Error: Table name is NULL.");
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
        POSTGRES_LOG("[postgres_get_table_index_count] Successfully retrieved index count for table '%s': %d", tableName, index_count);
        return index_count;
    } 
    else {
        POSTGRES_LOG("[postgres_get_table_index_count] Error: Query failed: %s", postgres_get_last_error(pg));
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
        POSTGRES_LOG("[postgres_get_column_details] Error: Postgres connection is NULL.");
        return NULL;
    }
    if (tableName == NULL) {
        POSTGRES_LOG("[postgres_get_column_details] Error: Table name is NULL.");
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
        POSTGRES_LOG("[postgres_get_column_details] Successfully retrieved column details for table '%s'.", tableName);
        return pgRes;
    } 
    else {
        POSTGRES_LOG("[postgres_get_column_details] Error: Query failed: %s", postgres_get_last_error(pg));
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
        POSTGRES_LOG("[postgres_get_value] Error: PostgresResult or its result is NULL.");
        return NULL;
    }
    if (row < 0 || row >= PQntuples(pgRes->result)) {
        POSTGRES_LOG("[postgres_get_value] Error: Row index %d is out of bounds.", row);
        return NULL;
    }
    if (col < 0 || col >= PQnfields(pgRes->result)) {
        POSTGRES_LOG("[postgres_get_value] Error: Column index %d is out of bounds.", col);
        return NULL;
    }

    POSTGRES_LOG("[postgres_get_value] Successfully retrieved value at row %d, column %d.", row, col);
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
        POSTGRES_LOG("[postgres_get_table_constraints] Error: Postgres connection is NULL.");
        return NULL;
    }
    if (tableName == NULL) {
        POSTGRES_LOG("[postgres_get_table_constraints] Error: Table name is NULL.");
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
        POSTGRES_LOG("[postgres_get_table_constraints] Successfully retrieved constraints for table '%s'.", tableName);
        return pgRes;
    } 
    else {
        POSTGRES_LOG("[postgres_get_table_constraints] Error: Query failed: %s", postgres_get_last_error(pg));
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
        POSTGRES_LOG("[postgres_num_tuples] Error: PostgresResult or its result is NULL.");
        return -1;
    } 

    POSTGRES_LOG("[postgres_num_tuples] Successfully retrieved number of tuples.");
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
        POSTGRES_LOG("[postgres_num_fields] Error: PostgresResult or its result is NULL.");
        return -1;
    }

    POSTGRES_LOG("[postgres_num_fields] Successfully retrieved number of fields.");
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
        POSTGRES_LOG("[postgres_command_tuples] Error: PostgresResult or its result is NULL.");
        return -1;
    }

    POSTGRES_LOG("[postgres_command_tuples] Successfully retrieved affected tuples.");
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
        POSTGRES_LOG("[postgres_backend_pid] Error: Postgres or its connection is NULL.");
        return -1;
    }  

    POSTGRES_LOG("[postgres_backend_pid] Successfully retrieved backend PID.");
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
        POSTGRES_LOG("[postgres_binary_tuples] Error: PostgresResult or result fields are NULL.");
        return -1;
    }

    POSTGRES_LOG("[postgres_binary_tuples] Successfully determined if tuples are binary.");
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
        POSTGRES_LOG("[postgres_bytes_size] Error: PostgresResult or result fields are NULL.");
        return -1;
    }
    if (colsNumber < 0) {
        POSTGRES_LOG("[postgres_bytes_size] Error: Column index is out of bounds (must be >= 0).");
        return -1;
    }

    POSTGRES_LOG("[postgres_bytes_size] Successfully retrieved size for column %d.", colsNumber);
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
        POSTGRES_LOG("[postgres_is_null] Error: PostgresResult or its result is NULL.");
        return false;
    }
    if (row < 0 || row >= postgres_num_tuples(pgRes)) {
        POSTGRES_LOG("[postgres_is_null] Error: Row index %d is out of bounds.", row);
        return false;
    }
    if (col < 0 || col >= postgres_num_fields(pgRes)) {
        POSTGRES_LOG("[postgres_is_null] Error: Column index %d is out of bounds.", col);
        return false;
    }

    POSTGRES_LOG("[postgres_is_null] Successfully determined if field is NULL at row %d, column %d.", row, col);
    return (bool)PQgetisnull(pgRes->result, row, col);
}

/**
 * @brief Resets the PostgreSQL connection by closing the existing connection and attempting to reestablish it.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 */
void postgres_reset(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        POSTGRES_LOG("[postgres_reset] Error: Postgres connection is NULL.");
        return;
    } 
    else {
        POSTGRES_LOG("[postgres_reset] Resetting PostgreSQL connection.");
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
        POSTGRES_LOG("[postgres_reset_start] Error: Postgres connection is NULL.");
        return -1;
    } 
    else {
        POSTGRES_LOG("[postgres_reset_start] Starting non-blocking PostgreSQL connection reset.");
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
        POSTGRES_LOG("[postgres_db_value] Error: Postgres connection is NULL.");
        return NULL;
    } 
    else {
        POSTGRES_LOG("[postgres_db_value] Successfully retrieved database name.");
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
        POSTGRES_LOG("[postgres_user_value] Error: Postgres connection is NULL.");
        return NULL;
    } 
    else {
        POSTGRES_LOG("[postgres_user_value] Successfully retrieved username.");
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
        POSTGRES_LOG("[postgres_password_value] Error: Postgres connection is NULL.");
        return NULL;
    } 
    else {
        POSTGRES_LOG("[postgres_password_value] Successfully retrieved password.");
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
        POSTGRES_LOG("[postgres_host_value] Error: Postgres connection is NULL.");
        return NULL;
    } 
    else {
        POSTGRES_LOG("[postgres_host_value] Successfully retrieved host value.");
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
        POSTGRES_LOG("[postgres_port_value] Error: Postgres connection is NULL.");
        return NULL;
    } 
    else {
        POSTGRES_LOG("[postgres_port_value] Successfully retrieved port value.");
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
        POSTGRES_LOG("[postgres_object_id_status] Error: PostgresResult or its result is NULL.");
        return NULL;
    }

    POSTGRES_LOG("[postgres_object_id_status] Successfully retrieved object ID status.");
    return PQoidStatus(pgRes->result);
}

/**
 * @brief Retrieves the command status string from the SQL command that generated the PostgresResult.
 * 
 * @param pgRes Pointer to the PostgresResult structure containing the result of the last command.
 * @return The command status string, or NULL if an error occurred.
 */
char* postgres_command_status(PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        POSTGRES_LOG("[postgres_command_status] Error: PostgresResult or its result is NULL.");
        return NULL;
    }

    POSTGRES_LOG("[postgres_command_status] Successfully retrieved command status.");
    return PQcmdStatus(pgRes->result);
}

/**
 * @brief Retrieves the protocol version used by the PostgreSQL connection.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return The protocol version, or -1 if an error occurred.
 */
int postgres_protocol_version(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        POSTGRES_LOG("[postgres_protocol_version] Error: Postgres connection is NULL.");
        return -1;
    }

    POSTGRES_LOG("[postgres_protocol_version] Successfully retrieved protocol version.");
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
        POSTGRES_LOG("[postgres_server_version] Error: Postgres connection is NULL.");
        return -1;
    }

    POSTGRES_LOG("[postgres_server_version] Successfully retrieved server version.");
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
        POSTGRES_LOG("[postgres_socket_descriptor] Error: Postgres connection is NULL.");
        return -1;
    }
    
    POSTGRES_LOG("[postgres_socket_descriptor] Successfully retrieved socket descriptor.");
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
        POSTGRES_LOG("[postgres_is_busy] Error: Postgres connection is NULL.");
        return false;
    }

    POSTGRES_LOG("[postgres_is_busy] Successfully checked if the connection is busy.");
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
        POSTGRES_LOG("[postgres_is_non_blocking] Error: Postgres connection is NULL.");
        return false;
    }

    POSTGRES_LOG("[postgres_is_non_blocking] Successfully checked if the connection is in non-blocking mode.");
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
        POSTGRES_LOG("[postgres_flush] Error: pg connection is NULL.");
        return -1;
    }

    POSTGRES_LOG("[postgres_flush] Flushing output data to the PostgreSQL server.");
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
        POSTGRES_LOG("[postgres_set_non_blocking] Error: pg connection is NULL.");
        return -1;
    }

    POSTGRES_LOG("[postgres_set_non_blocking] Setting connection to non-blocking mode: %d.", state);
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
        POSTGRES_LOG("[postgres_get_line] Error: pg connection is NULL.");
        return -1;
    }
    else if (buffer == NULL) {
        POSTGRES_LOG("[postgres_get_line] Error: buffer is NULL.");
        return -1;
    }

    POSTGRES_LOG("[postgres_get_line] Reading a line from the PostgreSQL server.");
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
        POSTGRES_LOG("[postgres_get_line_async] Error: pg connection is NULL.");
        return -1;
    }
    else if (buffer == NULL) {
        POSTGRES_LOG("[postgres_get_line_async] Error: buffer is NULL.");
        return -1;
    }

    POSTGRES_LOG("[postgres_get_line_async] Asynchronously reading a line from the PostgreSQL server.");
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
        POSTGRES_LOG("[postgres_put_line] Error: pg connection is NULL.");
        return -1;
    }
    else if (buffer == NULL) {
        POSTGRES_LOG("[postgres_put_line] Error: buffer is NULL.");
        return -1;
    }

    POSTGRES_LOG("[postgres_put_line] Sending a line to the PostgreSQL server.");
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
        POSTGRES_LOG("[postgres_put_bytes] Error: pg connection is NULL.");
        return -1;
    }
    else if (buffer == NULL) {
        POSTGRES_LOG("[postgres_put_bytes] Error: buffer is NULL.");
        return -1;
    }

    POSTGRES_LOG("[postgres_put_bytes] Sending bytes to the PostgreSQL server.");
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
        POSTGRES_LOG("[postgres_trace] Error: pg connection is NULL.");
        return;
    }
    else if (stream == NULL) {
        POSTGRES_LOG("[postgres_trace] Error: stream is NULL.");
        return;
    }

    POSTGRES_LOG("[postgres_trace] Enabling trace output to the specified stream.");
    PQtrace(pg->connection, stream);
}

/**
 * @brief Disables tracing of the client/server communication.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 */
void postgres_un_trace(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        POSTGRES_LOG("[postgres_un_trace] Error: pg connection is null.");
        return;
    }

    POSTGRES_LOG("[postgres_un_trace] Disabling trace output.");
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
        POSTGRES_LOG("[postgres_get_result] Error: pg connection is null.");
        return NULL;
    }

    PGresult *res = PQgetResult(pg->connection);
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        POSTGRES_LOG("[postgres_get_result] Error: Query failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return NULL;
    }

    PostgresResult* pgRes = (PostgresResult*) malloc(sizeof(PostgresResult));
    if (!pgRes) {
        POSTGRES_LOG("[postgres_get_result] Error: Object creation failed in PostgresResult.");
        PQclear(res);
        return NULL;
    }

    pgRes->result = res;
    POSTGRES_LOG("[postgres_get_result] Query result retrieved successfully.");
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
        POSTGRES_LOG("[postgres_request_cancle] Error: pg connection is null.");
        return -1;
    }

    POSTGRES_LOG("[postgres_request_cancle] Sending cancel request to the PostgreSQL server.");
    return PQrequestCancel(pg->connection);
}

/**
 * @brief Disconnects from the PostgreSQL database and attempts to reconnect using the stored connection parameters.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return true if reconnection is successful, false otherwise.
 */
bool postgres_reconnect(Postgres* pg) {
    if (pg == NULL) {
        POSTGRES_LOG("[postgres_reconnect] Error: Postgres structure is null.");
        return false;
    }
    POSTGRES_LOG("[postgres_reconnect] Disconnecting and attempting to reconnect to the database.");
    postgres_disconnect(pg);

    // Attempt to reconnect
    if (!postgres_connect(pg)) {
        POSTGRES_LOG("[postgres_reconnect] Error: Reconnection failed.");
        return false;
    }

    POSTGRES_LOG("[postgres_reconnect] Reconnection successful.");
    return true;
}

/**
 * @brief Checks if the connection to the PostgreSQL database server is still alive.
 * 
 * @param pg Pointer to the Postgres structure representing the connection.
 * @return true if the connection is alive, false otherwise.
 */
bool postgres_ping(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        POSTGRES_LOG("[postgres_ping] Error: Postgres structure or connection is null.");
        return false;
    }

    char conninfo[CON_INFO_SIZE];
    snprintf(conninfo, sizeof(conninfo), "dbname=%s user=%s password=%s host=%s port=%s", 
             pg->database, pg->user, pg->password, pg->host, pg->port);

    POSTGRES_LOG("[postgres_ping] Pinging the database server.");
    PGPing ping_status = PQping(conninfo);

    if (ping_status == PQPING_OK) {
        POSTGRES_LOG("[postgres_ping] Database server is reachable.");
        return true;
    } 
    else {
        POSTGRES_LOG("[postgres_ping] Error: Database server is unreachable.");
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
        POSTGRES_LOG("[postgres_query_execution_time] Error: query is null.");
        return NULL;
    } 
    else if (pg->connection == NULL) {
        POSTGRES_LOG("[postgres_query_execution_time] Error: connection of postgres is null.");
        return NULL;
    }

    clock_t start_time = clock();
    PGresult *res = PQexec(pg->connection, query);
    clock_t end_time = clock();

    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        POSTGRES_LOG("[postgres_query_execution_time] Error: Query failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return NULL;
    }

    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    POSTGRES_LOG("[postgres_query_execution_time] Query executed in %.6f seconds.", execution_time);

    PostgresResult* pgRes = (PostgresResult*) malloc(sizeof(PostgresResult));
    if (!pgRes) {
        POSTGRES_LOG("[postgres_query_execution_time] Error: Memory allocation failed in PostgresResult.");
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
        POSTGRES_LOG("[postgres_create_function] Error: connection of postgres is null.");
        return false;
    } 
    else if (functionName == NULL || returnType == NULL || language == NULL || functionBody == NULL || paramDefinitions == NULL) {
        POSTGRES_LOG("[postgres_create_function] Error: One or more function parameters are null.");
        return false;
    }

    // First, drop the function if it already exists
    char dropQuery[4096];
    snprintf(dropQuery, sizeof(dropQuery), "DROP FUNCTION IF EXISTS %s(%s);", functionName, paramDefinitions);
    POSTGRES_LOG("[postgres_create_function] Executing: %s", dropQuery);
    
    PGresult *dropRes = PQexec(pg->connection, dropQuery);
    if (PQresultStatus(dropRes) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_create_function] Error: Failed to drop existing function %s", PQerrorMessage(pg->connection));
        PQclear(dropRes);
        return false;
    }
    PQclear(dropRes);

    // Now, create the new function
    char createQuery[4096];
    snprintf(createQuery, sizeof(createQuery),
        "CREATE FUNCTION %s(%s) RETURNS %s AS $$ %s $$ LANGUAGE %s;",
        functionName, paramDefinitions, returnType, functionBody, language);
    
    POSTGRES_LOG("[postgres_create_function] Executing: %s", createQuery);
    PGresult *createRes = PQexec(pg->connection, createQuery);

    if (PQresultStatus(createRes) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_create_function] Error: Function creation failed %s", PQerrorMessage(pg->connection));
        PQclear(createRes);
        return false;
    }

    POSTGRES_LOG("[postgres_create_function] Function created successfully.");
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
        POSTGRES_LOG("[postgres_drop_function] Error: connection of postgres is null.");
        return false;
    } 
    else if (functionName == NULL || paramDefinitions == NULL) {
        POSTGRES_LOG("[postgres_drop_function] Error: functionName or paramDefinitions is null.");
        return false;
    }

    char dropQuery[4096];
    snprintf(dropQuery, sizeof(dropQuery), "DROP FUNCTION IF EXISTS %s(%s);", functionName, paramDefinitions);
    POSTGRES_LOG("[postgres_drop_function] Executing: %s", dropQuery);

    PGresult *dropRes = PQexec(pg->connection, dropQuery);
    if (PQresultStatus(dropRes) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_drop_function] Error: Failed to drop function %s", PQerrorMessage(pg->connection));
        PQclear(dropRes);
        return false;
    }

    PQclear(dropRes);
    POSTGRES_LOG("[postgres_drop_function] Function dropped successfully.");
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
        POSTGRES_LOG("[postgres_create_view] Error: connection of postgres is null.");
        return false;
    } 
    else if (viewName == NULL || query == NULL) {
        POSTGRES_LOG("[postgres_create_view] Error: viewName or query is null.");
        return false;
    }

    char createQuery[4096];
    snprintf(createQuery, sizeof(createQuery), "CREATE VIEW %s AS %s;", viewName, query);
    POSTGRES_LOG("[postgres_create_view] Executing: %s", createQuery);

    PGresult *res = PQexec(pg->connection, createQuery);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_create_view] Error: View creation failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    POSTGRES_LOG("[postgres_create_view] View created successfully.");
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
        POSTGRES_LOG("[postgres_drop_view] Error: connection of postgres is null.");
        return false;
    } 
    else if (viewName == NULL) {
        POSTGRES_LOG("[postgres_drop_view] Error: viewName is null.");
        return false;
    }

    char dropQuery[4096];
    snprintf(dropQuery, sizeof(dropQuery), "DROP VIEW IF EXISTS %s;", viewName);
    POSTGRES_LOG("[postgres_drop_view] Executing: %s", dropQuery);

    PGresult *res = PQexec(pg->connection, dropQuery);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_drop_view] Error: View drop failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    POSTGRES_LOG("[postgres_drop_view] View dropped successfully.");
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
        POSTGRES_LOG("[postgres_create_trigger] Error: connection of postgres is null.");
        return false;
    } 
    else if (triggerName == NULL || tableName == NULL || timing == NULL || event == NULL || function == NULL) {
        POSTGRES_LOG("[postgres_create_trigger] Error: One or more trigger parameters are null.");
        return false;
    }

    char createQuery[4096];
    snprintf(createQuery, sizeof(createQuery),
        "CREATE TRIGGER %s %s %s ON %s FOR EACH ROW EXECUTE FUNCTION %s();",
        triggerName, timing, event, tableName, function);
    
    POSTGRES_LOG("[postgres_create_trigger] Executing: %s", createQuery);

    PGresult *res = PQexec(pg->connection, createQuery);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_create_trigger] Error: Trigger creation failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    POSTGRES_LOG("[postgres_create_trigger] Trigger created successfully.");
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
        POSTGRES_LOG("[postgres_drop_trigger] Error: connection of postgres is null.");
        return false;
    } 
    else if (triggerName == NULL || tableName == NULL) {
        POSTGRES_LOG("[postgres_drop_trigger] Error: triggerName or tableName is null.");
        return false;
    }

    char dropQuery[4096];
    snprintf(dropQuery, sizeof(dropQuery), "DROP TRIGGER IF EXISTS %s ON %s;", triggerName, tableName);
    
    POSTGRES_LOG("[postgres_drop_trigger] Executing: %s", dropQuery);

    PGresult *res = PQexec(pg->connection, dropQuery);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_drop_trigger] Error: Trigger drop failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    POSTGRES_LOG("[postgres_drop_trigger] Trigger dropped successfully.");
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
        POSTGRES_LOG("[postgres_create_schema] Error: Connection of postgres is null.");
        return false;
    } 
    else if (schemaName == NULL) {
        POSTGRES_LOG("[postgres_create_schema] Error: Schema name is null.");
        return false;
    }

    char query[1024];
    snprintf(query, sizeof(query), "CREATE SCHEMA %s;", schemaName);

    POSTGRES_LOG("[postgres_create_schema] Executing: %s", query);

    PGresult *res = PQexec(pg->connection, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_create_schema] Error: Schema creation failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    POSTGRES_LOG("[postgres_create_schema] Schema created successfully.");
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
        POSTGRES_LOG("[postgres_drop_schema] Error: Connection of postgres is null.");
        return false;
    } 
    else if (schemaName == NULL) {
        POSTGRES_LOG("[postgres_drop_schema] Error: Schema name is null.");
        return false;
    }

    char query[1024];
    snprintf(query, sizeof(query), "DROP SCHEMA %s%s;", schemaName, cascade ? " CASCADE" : "");

    POSTGRES_LOG("[postgres_drop_schema] Executing: %s", query);

    PGresult *res = PQexec(pg->connection, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_drop_schema] Error: Schema drop failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    POSTGRES_LOG("[postgres_drop_schema] Schema dropped successfully.");
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
        POSTGRES_LOG("[postgres_query_params] Error: Connection of postgres is null.");
        return NULL;
    } 
    else if (query == NULL) {
        POSTGRES_LOG("[postgres_query_params] Error: Query string is null.");
        return NULL;
    } 
    else if (nParams > 0 && paramValues == NULL) {
        POSTGRES_LOG("[postgres_query_params] Error: Parameter values are null.");
        return NULL;
    }

    POSTGRES_LOG("[postgres_query_params] Executing: %s", query);

    PGresult* res = PQexecParams(pg->connection, query, nParams, NULL, paramValues, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_TUPLES_OK && PQresultStatus(res) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_query_params] Error: Query failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return NULL;
    }

    PostgresResult* pgRes = (PostgresResult*)malloc(sizeof(PostgresResult));
    if (pgRes == NULL) {
        POSTGRES_LOG("[postgres_query_params] Error: Memory allocation for PostgresResult failed.");
        PQclear(res);
        return NULL;
    }

    pgRes->result = res;
    POSTGRES_LOG("[postgres_query_params] Query executed successfully.");
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
        POSTGRES_LOG("[postgres_prepare_statement] Error: connection of postgres is null.");
        return false;
    } 
    else if (stmtName == NULL || query == NULL) {
        POSTGRES_LOG("[postgres_prepare_statement] Error: Statement name or query is null.");
        return false;
    }

    POSTGRES_LOG("[postgres_prepare_statement] Preparing statement: %s", stmtName);

    PGresult *res = PQprepare(pg->connection, stmtName, query, 0, NULL);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_prepare_statement] Error: Statement preparation failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    POSTGRES_LOG("[postgres_prepare_statement] Statement %s prepared successfully.", stmtName);
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
        POSTGRES_LOG("[postgres_clear_prepared_statement] Error: connection of postgres is null.");
        return false;
    } 
    else if (stmtName == NULL) {
        POSTGRES_LOG("[postgres_clear_prepared_statement] Error: Statement name is null.");
        return false;
    }

    char query[256];
    snprintf(query, sizeof(query), "DEALLOCATE %s;", stmtName);

    POSTGRES_LOG("[postgres_clear_prepared_statement] Deallocating statement: %s", stmtName);

    PGresult *res = PQexec(pg->connection, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_clear_prepared_statement] Error: Statement deallocation failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    POSTGRES_LOG("[postgres_clear_prepared_statement] Statement %s deallocated successfully.", stmtName);
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
        POSTGRES_LOG("[postgres_savepoint] Error: Postgres connection is null.");
        return false;
    }
    if (savepointName == NULL) {
        POSTGRES_LOG("[postgres_savepoint] Error: Savepoint name is null.");
        return false;
    }

    char query[256];
    snprintf(query, sizeof(query), "SAVEPOINT %s;", savepointName);

    POSTGRES_LOG("[postgres_savepoint] Creating savepoint: %s", savepointName);

    PGresult* res = PQexec(pg->connection, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_savepoint] Error: Failed to create savepoint %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    POSTGRES_LOG("[postgres_savepoint] Savepoint %s created successfully.", savepointName);
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
        POSTGRES_LOG("[postgres_rollback_to_savepoint] Error: Postgres connection is null.");
        return false;
    }
    if (savepointName == NULL) {
        POSTGRES_LOG("[postgres_rollback_to_savepoint] Error: Savepoint name is null.");
        return false;
    }

    char query[256];
    snprintf(query, sizeof(query), "ROLLBACK TO SAVEPOINT %s;", savepointName);

    POSTGRES_LOG("[postgres_rollback_to_savepoint] Rolling back to savepoint: %s", savepointName);

    PGresult* res = PQexec(pg->connection, query);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        POSTGRES_LOG("[postgres_rollback_to_savepoint] Error: Failed to rollback to savepoint %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    POSTGRES_LOG("[postgres_rollback_to_savepoint] Rolled back to savepoint %s successfully.", savepointName);
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
        POSTGRES_LOG("[postgres_send_async_query] Error: Postgres connection is null.");
        return false;
    }
    if (query == NULL) {
        POSTGRES_LOG("[postgres_send_async_query] Error: Query is null.");
        return false;
    }

    POSTGRES_LOG("[postgres_send_async_query] Sending async query: %s", query);

    if (PQsendQuery(pg->connection, query) == 0) {
        POSTGRES_LOG("[postgres_send_async_query] Error: Failed to send query %s", PQerrorMessage(pg->connection));
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
        POSTGRES_LOG("[postgres_get_async_result] Error: Postgres connection is null.");
        return NULL;
    }

    PGresult* res = PQgetResult(pg->connection);
    if (res == NULL) {
        return NULL;  // No more results
    }
    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK) {
        POSTGRES_LOG("[postgres_get_async_result] Error: Query failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return NULL;
    }

    PostgresResult* pgRes = (PostgresResult*) malloc(sizeof(PostgresResult));
    if (pgRes == NULL) {
        POSTGRES_LOG("[postgres_get_async_result] Error: Failed to allocate memory for PostgresResult.");
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
    if (pg == NULL || pg->connection == NULL) {
        POSTGRES_LOG("[postgres_copy_from_csv] Error: Postgres connection is null.");
        return false;
    }
    if (tableName == NULL || csvFilePath == NULL || delimiter == NULL) {
        POSTGRES_LOG("[postgres_copy_from_csv] Error: One or more parameters are null.");
        return false;
    }

    char query[1024];
    snprintf(query, sizeof(query),
        "COPY %s FROM STDIN WITH CSV HEADER DELIMITER '%c';", tableName, delimiter[0]);

    POSTGRES_LOG("[postgres_copy_from_csv] Executing COPY command to load data into table: %s", tableName);

    PGresult* res = PQexec(pg->connection, query);
    if (PQresultStatus(res) != PGRES_COPY_IN) {
        POSTGRES_LOG("[postgres_copy_from_csv] Error: COPY operation failed %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    FILE* file = fopen(csvFilePath, "r");
    if (file == NULL) {
        POSTGRES_LOG("[postgres_copy_from_csv] Error: Could not open CSV file %s", csvFilePath);
        PQclear(res);
        return false;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (PQputCopyData(pg->connection, buffer, strlen(buffer)) != 1) {
            POSTGRES_LOG("[postgres_copy_from_csv] Error: Failed to send CSV data to server %s", PQerrorMessage(pg->connection));
            fclose(file);
            PQclear(res);
            return false;
        }
    }
    fclose(file);

    if (PQputCopyEnd(pg->connection, NULL) != 1) {
        POSTGRES_LOG("[postgres_copy_from_csv] Error: Failed to complete COPY operation %s", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    POSTGRES_LOG("[postgres_copy_from_csv] Data successfully copied from CSV file %s to table %s", csvFilePath, tableName);
    return true;
}
