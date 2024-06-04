#include "postgres.h"
#include "../string/string.h"
#include "../fmt/fmt.h"
#include <stdlib.h>

#define CON_INFO_SIZE 256 

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
