#ifndef HTTP_H_
#define HTTP_H_

#include "tcp.h"
#include "../json/json.h"

#define MAX_HEADERS 100
#define MAX_QUERY_PARAMS 50
#define MAX_ROUTES 100

typedef enum {
    HTTP_GET,
    HTTP_POST,
    HTTP_PUT,
    HTTP_DELETE,
    HTTP_OPTIONS,
    HTTP_HEAD,
    HTTP_PATCH,
    HTTP_UNKNOWN
} HttpMethod;

typedef struct {
    char* name;
    char* value;
} HttpHeader;

typedef struct {
    char* name;
    char* value;
} HttpQueryParam;

typedef struct {
    HttpMethod method;
    char* path;
    char* body;
    JsonElement* json_body;
    HttpHeader headers[MAX_HEADERS];
    HttpQueryParam query_params[MAX_QUERY_PARAMS];
    size_t header_count;
    size_t query_param_count;
    int id;
} HttpRequest;

typedef struct {
    int status_code;
    char* status_message;
    HttpHeader headers[MAX_HEADERS];
    JsonElement* json_body;
    char* body;
    size_t header_count;
} HttpResponse;

typedef void (*HttpHandler)(HttpRequest* req, HttpResponse* res);

typedef struct {
    char* path;
    HttpMethod method;
    HttpHandler handler;
} HttpRoute;

// Functions to handle HTTP operations
HttpMethod http_parse_method(const char* request);
HttpRequest* http_parse_request(const char* request);
void http_free_request(HttpRequest* request);

void http_set_status(HttpResponse* response, int code, const char* message);
void http_set_json_body(HttpResponse* response, JsonElement* json);
void http_set_body(HttpResponse* response, const char* body);
void http_add_header(HttpResponse* response, const char* header, const char* value);
void http_free_response(HttpResponse* response);

// Server functions
void http_register_route(const char* path, HttpMethod method, HttpHandler handler);
void http_start_server(int port);
void http_stop_server(void);
void http_handle_request(TcpSocket client_socket);

// Utility functions
const char* http_get_header(HttpRequest* req, const char* name);
const char* http_get_query_param(HttpRequest* req, const char* name);
void http_send_error(HttpResponse* res, int code, const char* message);

char* http_serialize_response(HttpResponse* response);
#endif 
