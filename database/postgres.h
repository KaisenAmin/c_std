#ifndef POSTGRES_H_
#define POSTGRES_H_

#include <stdbool.h>
#include <libpq-fe.h>


typedef struct {
    char* database;
    char* password;
    char* user;
    PGconn *connection;

} Postgres;

typedef struct {
    PGresult *result;

} PostgresResult;

Postgres* postgres_create();
PostgresResult* postgres_query(Postgres* pg, const char* query);

bool postgres_connect(Postgres* pg);
bool postgres_execute_non_query(Postgres* pg, const char* command);
bool postgres_begin_transaction(Postgres* pg);
bool postgres_commit_transaction(Postgres* pg);
bool postgres_rollback_transaction(Postgres* pg);

void postgres_init(Postgres* pg, const char* database, const char* user, const char* password);
void postgres_disconnect(Postgres* pg);
void postgres_clear_result(PostgresResult* pgResult);
void postgres_deallocate(Postgres* pg);
void postgres_print_result(PostgresResult* pgRes);

const char* postgres_get_last_error(Postgres* pg);
int postgres_get_affected_rows(Postgres* pg, PostgresResult *pgRes);

#endif 