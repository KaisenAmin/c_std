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

void postgres_init(Postgres* pg, const char* database, const char* user, const char* password) {
    pg->database = string_strdup(database);
    pg->user = string_strdup(user);
    pg->password = string_strdup(password);
    pg->connection = NULL;
}

bool postgres_connect(Postgres* pg) {
    char conninfo[CON_INFO_SIZE];

    snprintf(conninfo, sizeof(conninfo), "dbname=%s user=%s password=%s", pg->database, pg->user, pg->password);
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
    if (pg->connection != NULL) {
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