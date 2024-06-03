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

void postgres_init(Postgres* pg, const char* database, const char* user, const char* password);
bool postgres_connect(Postgres* pg);
void postgres_disconnect(Postgres* pg);
PostgresResult* postgres_query(Postgres* pg, const char* query);
void postgres_clear_result(PostgresResult* pgResult);
void postgres_deallocate(Postgres* pg);

#endif 