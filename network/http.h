/**
 * @author Amin Tahmasebi
 * @class Http
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in http.c (file-level convention).
 *
 * A minimal HTTP/1.1 server + parser on top of the project's TCP and
 * JSON modules.
 */

#ifndef HTTP_H_
#define HTTP_H_

#include <stdbool.h>
#include <stddef.h>
#include "tcp.h"
#include "../json/json.h"


/* #define HTTP_LOGGING_ENABLE */

#ifdef HTTP_LOGGING_ENABLE
    #include <stdio.h>
    #define HTTP_LOG(fmt, ...) \
        do { fprintf(stderr, "[HTTP] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define HTTP_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Limits                                                             */
/* ------------------------------------------------------------------ */

#define MAX_HEADERS      100
#define MAX_QUERY_PARAMS 50
#define MAX_ROUTES       100


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
    HttpMethod      method;
    char*           path;
    char*           body;
    JsonElement*    json_body;
    HttpHeader      headers[MAX_HEADERS];
    HttpQueryParam  query_params[MAX_QUERY_PARAMS];
    size_t          header_count;
    size_t          query_param_count;
    int             id;
} HttpRequest;


typedef struct {
    int             status_code;
    char*           status_message;
    HttpHeader      headers[MAX_HEADERS];
    JsonElement*    json_body;
    char*           body;
    size_t          header_count;
} HttpResponse;

typedef void (*HttpHandler)(HttpRequest* req, HttpResponse* res);

typedef struct {
    char*       path;
    HttpMethod  method;
    HttpHandler handler;
} HttpRoute;


/* ------------------------------------------------------------------ */
/* Request parsing / lifecycle                                        */
/* ------------------------------------------------------------------ */

HttpMethod   http_parse_method               (const char* request);
HttpRequest* http_parse_request              (const char* request);
void         http_free_request               (HttpRequest* request);


/* ------------------------------------------------------------------ */
/* Request introspection                                              */
/* ------------------------------------------------------------------ */

const char*  http_get_header                 (HttpRequest* req, const char* name);
const char*  http_get_query_param            (HttpRequest* req, const char* name);
char*        http_get_cookie                 (HttpRequest* req, const char* name);


/* ------------------------------------------------------------------ */
/* Response building                                                  */
/* ------------------------------------------------------------------ */

void         http_set_status                 (HttpResponse* response, int code, const char* message);
void         http_set_body                   (HttpResponse* response, const char* body);
void         http_set_html_body              (HttpResponse* response, const char* html);
void         http_set_json_body              (HttpResponse* response, JsonElement* json);
void         http_add_header                 (HttpResponse* response, const char* header, const char* value);
void         http_set_cookie                 (HttpResponse* response, const char* name, const char* value, const char* path, int max_age, bool http_only);
void         http_redirect                   (HttpResponse* response, int code, const char* location);
void         http_send_error                 (HttpResponse* res, int code, const char* message);


/* ------------------------------------------------------------------ */
/* Response introspection                                             */
/* ------------------------------------------------------------------ */

const char*  http_response_get_header        (HttpResponse* response, const char* name);
size_t       http_response_remove_header     (HttpResponse* response, const char* name);


/* ------------------------------------------------------------------ */
/* Response serialization / destruction                               */
/* ------------------------------------------------------------------ */

char*        http_serialize_response         (HttpResponse* response);
void         http_free_response              (HttpResponse* response);


/* ------------------------------------------------------------------ */
/* Server                                                             */
/* ------------------------------------------------------------------ */

void         http_register_route             (const char* path, HttpMethod method, HttpHandler handler);
void         http_start_server               (int port);
void         http_stop_server                (void);
void         http_handle_request             (TcpSocket client_socket);


/* ------------------------------------------------------------------ */
/* Utilities (stateless string helpers)                               */
/* ------------------------------------------------------------------ */

const char*  http_method_to_string           (HttpMethod m);
const char*  http_status_text                (int code);
char*        http_url_decode                 (const char* in, size_t in_len);
char*        http_url_encode                 (const char* in);


/* ------------------------------------------------------------------ */
/* Request accessors / production helpers                             */
/* ------------------------------------------------------------------ */

const char*  http_get_header_ci              (const HttpRequest* req, const char* name);
const char*  http_content_type               (const HttpRequest* req);
long         http_content_length             (const HttpRequest* req);
bool         http_method_from_string         (const char* str, HttpMethod* out_method);
int          http_status_class               (int code);


#endif
