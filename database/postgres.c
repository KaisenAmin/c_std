#include "postgres.h"
#include "../string/string.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

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

Postgres* postgres_create() {
    Postgres* pg = (Postgres*) malloc(sizeof(Postgres));

    if (pg) {
        return pg;
    }
    else {
        return NULL;
    }
}

void postgres_init(Postgres* pg, const char* database, const char* user, const char* password, const char* host, const char* port) {
    pg->database = string_strdup(database);
    pg->user = string_strdup(user);
    pg->password = string_strdup(password);
    pg->host = string_strdup(host);
    pg->port = string_strdup(port);
    pg->connection = NULL;
}

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

void postgres_disconnect(Postgres* pg) {
    if (pg->connection != NULL) {
        PQfinish(pg->connection);
        pg->connection = NULL;
    }
}

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

void postgres_clear_result(PostgresResult* pgResult) {
    if (pgResult) {
        PQclear(pgResult->result);
        free(pgResult);
    }
}

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

const char* postgres_get_last_error(Postgres* pg) {
    if (pg->connection != NULL) {
        return PQerrorMessage(pg->connection);
    } 
    else {
        return "Connection of postgres is null.";
    }
}

int postgres_get_affected_rows(Postgres* pg, PostgresResult *pgRes) {
    if (pg->connection != NULL && pgRes->result != NULL) {
        return atoi(PQcmdTuples(pgRes->result));
    } 
    else {
        fmt_fprintf(stderr, "Error: connection of postgres or result is null.\n");
        return -1;
    }
}

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

bool postgres_execute_prepared(Postgres* pg, const char* stmtName, const char* query, int nParams, const char* const* paramValues) {
    if (pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: postgres connection is null.\n");
        return false;
    } 
    else if (stmtName == NULL || query == NULL || paramValues == NULL) {
        fmt_fprintf(stderr, "Error: statement name, query, or parameters are null.\n");
        return false;
    }

    PGresult* res = PQprepare(pg->connection, stmtName, query, nParams, NULL);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Statement preparation failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }
    PQclear(res);

    res = PQexecPrepared(pg->connection, stmtName, nParams, paramValues, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fmt_fprintf(stderr, "Error: Statement execution failed %s\n", PQerrorMessage(pg->connection));
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

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

int postgres_num_tuples(const PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: PostgresResult or its result is null.\n");
        return -1;
    } 

    return PQntuples(pgRes->result);
}

int postgres_num_fields(const PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: PostgresResult or its result is null.\n");
        return -1;
    }

    return PQnfields(pgRes->result);
}

int postgres_command_tuples(PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: PostgresResult or its result is null.\n");
        return -1;
    }

    return atoi(PQcmdTuples(pgRes->result));
}

int postgres_backend_pid(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: Postgres or its connection is null.\n");
        return -1;
    }  

    return PQbackendPID(pg->connection);
}

int postgres_binary_tuples(const PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: PostgresResult or result fields is null.\n");
        return -1;
    }

    return PQbinaryTuples(pgRes->result);
}

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

void postgres_reset(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return;
    }
    else {
        PQreset(pg->connection);
    }
}

int postgres_reset_start(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }
    else {
        return PQresetStart(pg->connection);
    }
}

char* postgres_db_value(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return NULL;
    }
    else {
        return PQdb(pg->connection);
    }
}

char* postgres_user_value(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return NULL;
    }
    else {
        return PQuser(pg->connection);
    }
}

char* postgres_password_value(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return NULL;
    }
    else {
        return PQpass(pg->connection);
    }
}

char* postgres_host_value(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return NULL;
    }
    else {
        return PQhost(pg->connection);
    }
}

char* postgres_port_value(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return NULL;
    }
    else {
        return PQport(pg->connection);
    }
}

char* postgres_object_id_status(const PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: pgRes or pgRes->result is null.\n");
        return NULL;
    }
    else {
        return PQoidStatus(pgRes->result);
    }
}

char* postgres_command_status(PostgresResult* pgRes) {
    if (pgRes == NULL || pgRes->result == NULL) {
        fmt_fprintf(stderr, "Error: pgRes or pgRes->result is null.\n");
        return NULL;
    }
    else {
        return PQcmdStatus(pgRes->result);
    }
}

int postgres_protocol_version(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }

    return PQprotocolVersion(pg->connection);
}

int postgres_server_version(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }

    return PQserverVersion(pg->connection);
}

int postgres_socket_descriptor(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }
    
    return PQsocket(pg->connection);
}

bool postgres_is_busy(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return false;
    }

    return (bool)PQisBusy(pg->connection);
}

bool postgres_is_non_blocking(const Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return false;
    }

    return (bool)PQisnonblocking(pg->connection);
}

int postgres_flush(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }

    return PQflush(pg->connection);
}

int postgres_set_non_blocking(Postgres* pg, int state) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return -1;
    }

    return PQsetnonblocking(pg->connection, state);
}

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

void postgres_un_trace(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return;
    }

    PQuntrace(pg->connection);
}

PostgresResult* postgres_get_result(Postgres* pg) {
    if (pg == NULL || pg->connection == NULL) {
        fmt_fprintf(stderr, "Error: pg connection is null.\n");
        return;
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