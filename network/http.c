#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <limits.h>
#include "http.h"
#include "../string/std_string.h"




static HttpRoute routes[MAX_ROUTES];
static size_t route_count = 0;
static bool server_running = true;



/* Match a registered route template against a concrete request path.
 * Supports a single `{name}` placeholder that parses as an integer into
 * *id_out. Returns true on full match. */
static bool match_route(const char* route, const char* path, int* id_out) {
    if (!route || !path) {
        return false;
    }
    HTTP_LOG("matching route '%s' against path '%s'", route, path);

    const char* route_ptr = route;
    const char* path_ptr = path;

    while (*route_ptr && *path_ptr) {
        if (*route_ptr == '{') {
            // Skip the placeholder segment in the route template..
            while (*route_ptr && *route_ptr != '}') {
                route_ptr++;
            }
            if (*route_ptr == '}') {
                route_ptr++;  // advance past '}' only if present
            }

            // Parse the dynamic segment in the path as an integer.
            if (id_out) {
                *id_out = atoi(path_ptr);
            }

            while (*path_ptr && *path_ptr != '/') {
                path_ptr++;
            }
        }
        else {
            if (*route_ptr != *path_ptr) {
                HTTP_LOG("route segment mismatch: '%c' vs '%c'", *route_ptr, *path_ptr);
                return false;
            }
            route_ptr++;
            path_ptr++;
        }
    }

    bool match = (*route_ptr == '\0' && *path_ptr == '\0');
    HTTP_LOG("route match result: %d", match);
    return match;
}


static void handle_request(HttpRequest* req, HttpResponse* res) {
    HTTP_LOG("dispatching %s %s (%zu routes registered)", http_method_to_string(req->method), req->path ? req->path : "(null)", route_count);

    for (size_t i = 0; i < route_count; i++) {
        int id = -1;

        if (match_route(routes[i].path, req->path, &id) && routes[i].method == req->method) {
            HTTP_LOG("route hit: index=%zu template='%s' method=%s id=%d",
                     i, routes[i].path, http_method_to_string(routes[i].method), id);
            req->id = id;
            routes[i].handler(req, res);
            HTTP_LOG("handler returned for '%s'; status_code=%d",
                     req->path, res->status_code);

            return;
        }
    }

    HTTP_LOG("no route matched %s %s -> 404", http_method_to_string(req->method), req->path ? req->path : "(null)");
    http_send_error(res, 404, "Route not found");
}


/**
 * @brief Identify the HTTP method at the start of a raw request line.
 *
 * Compares the leading verb of `request` against the supported method
 * names. Each comparison includes the trailing space so prefix collisions
 * like `"GETSOMETHING"` no longer match `HTTP_GET`. Returns `HTTP_UNKNOWN`
 * on no match or NULL input.
 *
 * @param request Pointer to a raw HTTP request buffer.
 * @return The matching `HttpMethod` enum, or `HTTP_UNKNOWN`.
 */
HttpMethod http_parse_method(const char* request) {
    if (!request) {
        HTTP_LOG("parse_method: NULL input -> HTTP_UNKNOWN");
        return HTTP_UNKNOWN;
    }
    if (strncmp(request, "GET ", 4) == 0) {
        HTTP_LOG("parse_method: GET");
        return HTTP_GET;
    }
    if (strncmp(request, "POST ", 5) == 0) {
        HTTP_LOG("parse_method: POST");
        return HTTP_POST;
    }
    if (strncmp(request, "PUT ", 4) == 0) {
        HTTP_LOG("parse_method: PUT");
        return HTTP_PUT;
    }
    if (strncmp(request, "DELETE ", 7) == 0) {
        HTTP_LOG("parse_method: DELETE");
        return HTTP_DELETE;
    }
    if (strncmp(request, "OPTIONS ", 8) == 0) {
        HTTP_LOG("parse_method: OPTIONS");
        return HTTP_OPTIONS;
    }
    if (strncmp(request, "HEAD ", 5) == 0) {
        HTTP_LOG("parse_method: HEAD");
        return HTTP_HEAD;
    }
    if (strncmp(request, "PATCH ", 6) == 0) {
        HTTP_LOG("parse_method: PATCH");
        return HTTP_PATCH;
    }

    HTTP_LOG("parse_method: unrecognized verb in first 8 bytes -> HTTP_UNKNOWN");
    return HTTP_UNKNOWN;
}


/**
 * @brief Parse a raw HTTP request into an `HttpRequest` struct.
 *
 * Extracts the method, path, query parameters, headers, and body. If the
 * body's first non-whitespace byte is `{` or `[` it is parsed into
 * `json_body`; otherwise only `body` (the raw bytes) is populated. The
 * returned struct must be released with `http_free_request`.
 *
 * Validates the request-line shape (two spaces) BEFORE allocating, so
 * malformed input returns NULL instead of crashing.
 *
 * @param request Null-terminated raw request bytes. NULL is rejected.
 * @return Newly-allocated request, or NULL on malformed input / OOM.
 */
HttpRequest* http_parse_request(const char* request) {
    if (!request) {
        HTTP_LOG("parse_request: NULL input");
        return NULL;
    }

    // Validate the request line shape FIRST so we never deref a NULL from
    // strchr below. The minimum valid line is "GET / HTTP/1.1\r\n".
    const char* sp1 = strchr(request, ' ');
    if (!sp1) {
        HTTP_LOG("parse_request: malformed request (no space after method)");
        return NULL;
    }

    const char* path_start = sp1 + 1;
    const char* sp2 = strchr(path_start, ' ');
    if (!sp2) {
        HTTP_LOG("parse_request: malformed request (no space after path)");
        return NULL;
    }

    HttpRequest* req = (HttpRequest*)malloc(sizeof(HttpRequest));
    if (!req) {
        HTTP_LOG("parse_request: OOM allocating HttpRequest");
        return NULL;
    }
    memset(req, 0, sizeof(HttpRequest));

    req->method = http_parse_method(request);
    req->path = string_strndup(path_start, (size_t)(sp2 - path_start));

    if (!req->path) {
        HTTP_LOG("parse_request: OOM duplicating path");
        free(req);
        return NULL;
    }
    HTTP_LOG("parse_request: method=%s raw_path='%s'", http_method_to_string(req->method), req->path);

    // Parse query parameters if present
    char* query_start = strchr(req->path, '?');
    if (query_start) {
        *query_start = '\0';
        HTTP_LOG("parse_request: path split at '?' -> path='%s' query='%s'", req->path, query_start + 1);
        char* query_string = string_strdup(query_start + 1);
        if (query_string) {
            char* token = strtok(query_string, "&");

            while (token && req->query_param_count < MAX_QUERY_PARAMS) {
                char* equal_sign = strchr(token, '=');
                if (equal_sign) {
                    *equal_sign = '\0';
                    req->query_params[req->query_param_count].name = string_strdup(token);
                    req->query_params[req->query_param_count].value = string_strdup(equal_sign + 1);
                    req->query_param_count++;

                    HTTP_LOG("parse_request: query[%zu] '%s'='%s'", req->query_param_count, token, equal_sign + 1);
                }
                token = strtok(NULL, "&");
            }
            if (req->query_param_count == MAX_QUERY_PARAMS && token) {
                HTTP_LOG("parse_request: hit MAX_QUERY_PARAMS=%d, trailing params dropped",
                         MAX_QUERY_PARAMS);
            }
            free(query_string);
        }
    }

    // Parse headers (start of first \r\n after the request line)
    const char* eol = strstr(request, "\r\n");
    if (eol) {
        const char* header_start = eol + 2;

        while (header_start && *header_start != '\r' && req->header_count < MAX_HEADERS) {
            const char* header_end = strstr(header_start, "\r\n");
            if (!header_end) {
                break;
            }

            const char* colon = strchr(header_start, ':');
            if (colon && colon < header_end) {
                req->headers[req->header_count].name = string_strndup(header_start, (size_t)(colon - header_start));
                // Skip leading space after ":" if present.

                const char* val_start = colon + 1;
                if (val_start < header_end && *val_start == ' ') {
                    val_start++;
                }
                req->headers[req->header_count].value = string_strndup(val_start, (size_t)(header_end - val_start));
                HTTP_LOG("parse_request: header[%zu] '%s' = '%s'", req->header_count,
                         req->headers[req->header_count].name ? req->headers[req->header_count].name : "(null)",
                         req->headers[req->header_count].value ? req->headers[req->header_count].value : "(null)");
                req->header_count++;
            }
            else {
                HTTP_LOG("parse_request: header line without ':' — skipped");
            }

            header_start = header_end + 2;
        }
        if (req->header_count == MAX_HEADERS) {
            HTTP_LOG("parse_request: hit MAX_HEADERS=%d, trailing headers dropped", MAX_HEADERS);
        }
    }
    HTTP_LOG("parse_request: parsed %zu header(s), %zu query param(s)", req->header_count, req->query_param_count);

    // Parse body
    const char* body_start = strstr(request, "\r\n\r\n");
    if (body_start) {
        body_start += 4;

        if (*body_start != '\0') {
            req->body = string_strdup(body_start);
            HTTP_LOG("parse_request: body present (%zu bytes)", req->body ? strlen(req->body) : 0);
            // Only attempt JSON parsing if it actually looks like JSON.
            if (req->body && (*body_start == '{' || *body_start == '[')) {
                HTTP_LOG("parse_request: body looks like JSON, attempting parse");
                req->json_body = json_parse(req->body);
                if (!req->json_body) {
                    HTTP_LOG("parse_request: body looked like JSON but parse failed");
                }
                else {
                    HTTP_LOG("parse_request: JSON body parsed successfully");
                }
            }
        }
        else {
            HTTP_LOG("parse_request: empty body");
        }
    }

    HTTP_LOG("parse_request: success -> request %p", (void*)req);
    return req;
}

/**
 * @brief Release every allocation owned by an `HttpRequest`.
 *
 * Frees the path, body, parsed JSON tree, all header name/value strings,
 * all query-param name/value strings, and finally the request struct
 * itself. NULL is a safe no-op.
 *
 * @param request Request to free.
 */
void http_free_request(HttpRequest* request) {
    if (!request) {
        HTTP_LOG("free_request: NULL — no-op");
        return;
    }
    HTTP_LOG("free_request: releasing request %p (path='%s', %zu headers, %zu query params)", (void*)request, request->path ? request->path : "(null)",
             request->header_count, request->query_param_count);

    free(request->path);
    free(request->body);

    if (request->json_body) {
        json_deallocate(request->json_body);
    }

    for (size_t i = 0; i < request->header_count; i++) {
        free(request->headers[i].name);
        free(request->headers[i].value);
    }
    for (size_t i = 0; i < request->query_param_count; i++) {
        free(request->query_params[i].name);
        free(request->query_params[i].value);
    }
    free(request);
}


/**
 * @brief Set the response status code and (strdup'd) reason phrase.
 *
 * Frees any previous status message before overwriting — repeated calls
 * do not leak.
 *
 * @param response Response to update. NULL is a no-op.
 * @param code     HTTP status code (e.g. 200, 404).
 * @param message  Reason phrase (`"OK"`, `"Not Found"`, …). Copied; caller
 *                 keeps ownership.
 */
void http_set_status(HttpResponse* response, int code, const char* message) {
    if (!response || !message) {
        HTTP_LOG("set_status: NULL response or message — no-op");
        return;
    }
    HTTP_LOG("set_status: %d -> %d, message='%s'", response->status_code, code, message);
    response->status_code = code;

    // Free any previous status_message
    free(response->status_message);
    response->status_message = string_strdup(message);
}


/**
 * @brief Attach a JSON document as the response body.
 *
 * Takes ownership of `json` — frees any previously-attached body or JSON
 * tree, serializes the new value into the body, and appends a
 * `Content-Type: application/json` header. Passing NULL transforms the
 * response into a 500 error instead.
 *
 * @param response Response to update.
 * @param json     JSON tree (ownership transferred). NULL emits a 500.
 */
void http_set_json_body(HttpResponse* response, JsonElement* json) {
    if (!response) {
        HTTP_LOG("set_json_body: NULL response — no-op");
        return;
    }
    if (!json) {
        HTTP_LOG("set_json_body: received NULL JSON -> emitting 500");
        http_set_status(response, 500, "Internal Server Error: Null JSON Body");
        http_set_body(response, "Failed to set JSON body: NULL pointer received");

        return;
    }

    if (response->json_body) {
        HTTP_LOG("set_json_body: freeing previous JSON body");
        json_deallocate(response->json_body);
    }
    response->json_body = json;

    free(response->body);
    response->body = json_serialize(json);
    HTTP_LOG("set_json_body: serialized JSON body (%zu bytes), added Content-Type: application/json", response->body ? strlen(response->body) : 0);
    http_add_header(response, "Content-Type", "application/json");
}


/**
 * @brief Attach a plain-text body to the response.
 *
 * Copies `body` and appends a `Content-Type: text/plain` header. Frees
 * any previous body so repeated calls don't leak.
 *
 * @param response Response to update. NULL is a no-op.
 * @param body     Null-terminated body string. Copied; caller keeps ownership.
 */
void http_set_body(HttpResponse* response, const char* body) {
    if (!response || !body) {
        HTTP_LOG("set_body: NULL response or body — no-op");
        return;
    }

    // Free any previous body — the original code leaked on overwrite.
    free(response->body);
    response->body = string_strdup(body);
    HTTP_LOG("set_body: set %zu bytes, added Content-Type: text/plain", strlen(body));
    http_add_header(response, "Content-Type", "text/plain");

}


/**
 * @brief Append a `Name: Value` header to the response.
 *
 * No deduplication — calling twice with the same name yields two header
 * entries. Once `MAX_HEADERS` has been reached, additional headers are
 * silently dropped (logged when `HTTP_LOGGING_ENABLE` is on).
 *
 * @param response Response to update. NULL is a no-op.
 * @param header   Header name. Copied; caller keeps ownership.
 * @param value    Header value. Copied; caller keeps ownership.
 */
void http_add_header(HttpResponse* response, const char* header, const char* value) {
    if (!response || !header || !value) {
        HTTP_LOG("add_header: NULL argument (response=%p header=%p value=%p) — no-op", (void*)response, (const void*)header, (const void*)value);
        return;
    }
    if (response->header_count >= MAX_HEADERS) {
        HTTP_LOG("add_header: MAX_HEADERS=%d reached, dropping '%s'", MAX_HEADERS, header);
        return;
    }
    response->headers[response->header_count].name = string_strdup(header);
    response->headers[response->header_count].value = string_strdup(value);
    response->header_count++;

    HTTP_LOG("add_header: [%zu] '%s' = '%s'", response->header_count, header, value);
}


/**
 * @brief Serialize a response into a freshly-allocated HTTP/1.1 wire-format
 *        buffer. Caller must `free()` the result.
 *
 * Computes the exact required size upfront (status line + every header's
 * actual length + body length) and writes with bounded `snprintf` /
 * `memcpy`, so the output can never overflow regardless of how many
 * headers or how large the body.
 *
 * @param response Response to serialize.
 * @return Newly-allocated null-terminated string, or NULL on OOM / NULL input.
 */
char* http_serialize_response(HttpResponse* response) {
    if (!response) {
        HTTP_LOG("serialize_response: NULL response -> NULL");
        return NULL;
    }
    HTTP_LOG("serialize_response: status=%d, %zu header(s), body=%zu bytes, json=%s", response->status_code, response->header_count,
             response->body ? strlen(response->body) : 0,
             response->json_body ? "yes" : "no");

    // Compute an upper bound on the output size — the previous code used a
    // fixed 1024 + body-length estimate that ignored header lengths and
    // would overflow under strcat with many or large headers.
    const char* status_msg = response->status_message ? response->status_message : "OK";
    size_t total = 32 + strlen(status_msg);  // "HTTP/1.1 <code> <msg>\r\n"
    for (size_t i = 0; i < response->header_count; i++) {
        const char* n = response->headers[i].name  ? response->headers[i].name  : "";
        const char* v = response->headers[i].value ? response->headers[i].value : "";
        total += strlen(n) + 2 /* ": " */ + strlen(v) + 2 /* CRLF */;
    }

    total += 2;  // empty line before body
    char* json_str = NULL;
    if (response->json_body) {
        json_str = json_serialize(response->json_body);
        if (json_str) {
            total += strlen(json_str);
        }
    }
    else if (response->body) {
        total += strlen(response->body);
    }
    total += 1;  // NUL

    char* buffer = (char*)malloc(total);
    if (!buffer) {
        HTTP_LOG("serialize_response: OOM allocating %zu-byte wire buffer", total);
        free(json_str);
        return NULL;
    }
    HTTP_LOG("serialize_response: allocated %zu-byte wire buffer", total);

    int n = snprintf(buffer, total, "HTTP/1.1 %d %s\r\n", response->status_code ? response->status_code : 200, status_msg);
    if (n < 0) {
        HTTP_LOG("serialize_response: snprintf failed on status line");
        free(buffer);
        free(json_str);
        return NULL;
    }
    size_t off = (size_t)n;

    for (size_t i = 0; i < response->header_count; i++) {
        const char* hn = response->headers[i].name  ? response->headers[i].name  : "";
        const char* hv = response->headers[i].value ? response->headers[i].value : "";
        n = snprintf(buffer + off, total - off, "%s: %s\r\n", hn, hv);

        if (n < 0 || (size_t)n >= total - off) {
            break;
        }

        off += (size_t)n;
    }

    if (off + 2 < total) { 
        buffer[off++] = '\r'; 
        buffer[off++] = '\n'; 
    }
    buffer[off] = '\0';

    if (json_str) {
        size_t jl = strlen(json_str);

        if (off + jl < total) { 
            memcpy(buffer + off, json_str, jl); 
            off += jl; 
        }
        free(json_str);
    }
    else if (response->body) {
        size_t bl = strlen(response->body);

        if (off + bl < total) { 
            memcpy(buffer + off, response->body, bl); 
            off += bl; 
        }
    }

    if (off < total) {
        buffer[off] = '\0';
    }
    else {
        buffer[total - 1] = '\0';
    }
    HTTP_LOG("serialize_response: wire size = %zu bytes", off);

    return buffer;
}


/**
 * @brief Release every allocation owned by an `HttpResponse`.
 *
 * Frees the status message, JSON tree, body, and all header strings, then
 * zeroes the corresponding fields so the same struct can be reused without
 * a fresh `memset`. Does NOT free the `HttpResponse` struct itself —
 * typically it's a stack value. NULL is a safe no-op.
 *
 * @param response Response to clean up.
 */
void http_free_response(HttpResponse* response) {
    if (!response) {
        HTTP_LOG("free_response: NULL — no-op");
        return;
    }
    HTTP_LOG("free_response: releasing response (status=%d, %zu header(s))", response->status_code, response->header_count);
    free(response->status_message);

    response->status_message = NULL;
    if (response->json_body) {
        json_deallocate(response->json_body);
        response->json_body = NULL;
    }
    free(response->body);
    response->body = NULL;

    for (size_t i = 0; i < response->header_count; i++) {
        free(response->headers[i].name);
        free(response->headers[i].value);
    }

    response->header_count = 0;
}


/**
 * @brief Register a handler for a given path + method.
 *
 * Paths may contain a single `{name}` placeholder that the framework
 * parses as an integer and stores in `HttpRequest::id`. Once
 * `MAX_ROUTES` is reached additional registrations are silently dropped.
 * NULL `path` or `handler` is rejected.
 *
 * @param path    URL path (e.g. `/users/{id}`).
 * @param method  HTTP method to match.
 * @param handler Callback invoked when the route matches.
 */
void http_register_route(const char* path, HttpMethod method, HttpHandler handler) {
    if (!path || !handler) {
        HTTP_LOG("register_route: NULL path or handler — rejected");
        return;
    }
    if (route_count < MAX_ROUTES) {
        routes[route_count].path = string_strdup(path);
        routes[route_count].method = method;
        routes[route_count].handler = handler;
        HTTP_LOG("register_route: [%zu] %s %s -> handler %p", route_count, http_method_to_string(method), path, (void*)handler);
        route_count++;
    }
    else {
        HTTP_LOG("register_route: MAX_ROUTES=%d reached, dropping %s %s", MAX_ROUTES, http_method_to_string(method), path);
    }
}

/**
 * @brief Start the HTTP server on the given port and block until stopped.
 *
 * Binds to `0.0.0.0:port` with `SO_REUSEADDR`, listens, and serves
 * requests on the calling thread until `http_stop_server` is called from
 * another thread. Every error path closes the socket and runs
 * `tcp_cleanup` before returning.
 *
 * @param port TCP port to listen on.
 */
void http_start_server(int port) {
    TcpSocket server_socket;
    TcpStatus status;
    server_running = true;

    status = tcp_init();
    if (status != TCP_SUCCESS) {
        HTTP_LOG("failed to initialize network");
        return;
    }

    status = tcp_socket_create(&server_socket);
    if (status != TCP_SUCCESS) {
        HTTP_LOG("failed to create server socket");
        tcp_cleanup();
        return;
    }
    tcp_set_reuse_addr(server_socket, true);

    status = tcp_bind(server_socket, "0.0.0.0", (unsigned short)port);
    if (status != TCP_SUCCESS) {
        HTTP_LOG("failed to bind server socket on port %d", port);
        tcp_close(server_socket);
        tcp_cleanup();
        return;
    }

    status = tcp_listen(server_socket, 10);
    if (status != TCP_SUCCESS) {
        HTTP_LOG("failed to listen on server socket");
        tcp_close(server_socket);
        tcp_cleanup();
        return;
    }

    HTTP_LOG("HTTP Server listening on port %d", port);

    while (server_running) {
        TcpSocket client_socket;
        status = tcp_accept(server_socket, &client_socket);
        if (status == TCP_SUCCESS) {
            http_handle_request(client_socket);
            tcp_close(client_socket);
        }
    }

    tcp_close(server_socket);
    tcp_cleanup();
    HTTP_LOG("HTTP Server stopped");
}


/**
 * @brief Ask the server loop to stop after the next accepted connection.
 *
 * Just flips the internal `server_running` flag — it doesn't interrupt a
 * blocked `accept()`. Send one more dummy request to the server (or
 * arrange a separate signaling mechanism) to make the loop wake up and
 * see the flag.
 */
void http_stop_server(void) {
    HTTP_LOG("stop_server: setting server_running=false");
    server_running = false;
}


/**
 * @brief Process a single request from an already-accepted client socket.
 *
 * Reads one buffer of request bytes (up to 4 KiB), dispatches to the
 * registered route handler, serializes the response, and writes it back.
 * Used internally by `http_start_server`; can be invoked directly for
 * testing. The caller is responsible for closing `client_socket`.
 *
 * Every exit path (success, parse failure, send failure) frees both the
 * `HttpRequest` and `HttpResponse` — no leaks on the error branches.
 *
 * @param client_socket A connected TCP socket.
 */
void http_handle_request(TcpSocket client_socket) {
    HTTP_LOG("handle_request: entry on socket");
    char buffer[4096];
    size_t received = 0;
    TcpStatus status = tcp_recv(client_socket, buffer, sizeof(buffer) - 1, &received);

    if (status != TCP_SUCCESS || received == 0) {
        HTTP_LOG("handle_request: tcp_recv failed or returned 0 bytes (status=%d)", (int)status);
        return;
    }
    buffer[received] = '\0';
    HTTP_LOG("handle_request: received %zu bytes", received);

    HttpRequest* req = http_parse_request(buffer);
    HttpResponse res;
    memset(&res, 0, sizeof(res));

    if (!req) {
        HTTP_LOG("handle_request: parse failed -> 400 Bad Request");
        http_send_error(&res, 400, "Bad Request: Invalid HTTP Request");
    }
    else {
        res.status_code = 200;
        res.status_message = string_strdup("OK");
        handle_request(req, &res);
    }

    char* response_str = http_serialize_response(&res);
    if (response_str) {
        size_t total = strlen(response_str);
        size_t sent = 0;
        TcpStatus send_status = tcp_send(client_socket, response_str, total, &sent);
        (void)send_status;   /* only referenced inside HTTP_LOG */

        HTTP_LOG("handle_request: tcp_send wrote %zu/%zu bytes (status=%d)", sent, total, (int)send_status);
        free(response_str);
    }
    else {
        HTTP_LOG("handle_request: serialize_response returned NULL — nothing sent");
    }

    // Free resources on EVERY exit path — the previous code leaked the
    // response on the parse-failure branch.
    if (req) {
        http_free_request(req);
    }
    http_free_response(&res);
    HTTP_LOG("handle_request: complete");
}


/**
 * @brief Find a header value on a parsed request by name (case-sensitive).
 *
 * @param req  Request to search. NULL → NULL.
 * @param name Header name.
 * @return Pointer to the value (still owned by the request), or NULL if
 *         not present.
 */
const char* http_get_header(HttpRequest* req, const char* name) {
    if (!req || !name) {
        HTTP_LOG("get_header: NULL request or name");
        return NULL;
    }

    for (size_t i = 0; i < req->header_count; i++) {
        if (req->headers[i].name && strcmp(req->headers[i].name, name) == 0) {
            HTTP_LOG("get_header: '%s' = '%s' (index %zu)",
                     name, req->headers[i].value ? req->headers[i].value : "(null)", i);
            return req->headers[i].value;
        }
    }
    HTTP_LOG("get_header: '%s' not found", name);

    return NULL;
}


/**
 * @brief Find a query-string parameter on a parsed request by name.
 *
 * @param req  Request to search. NULL → NULL.
 * @param name Parameter name (case-sensitive).
 * @return Pointer to the parameter value (still owned by the request),
 *         or NULL if absent.
 */
const char* http_get_query_param(HttpRequest* req, const char* name) {
    if (!req || !name) {
        HTTP_LOG("get_query_param: NULL request or name");
        return NULL;
    }

    for (size_t i = 0; i < req->query_param_count; i++) {
        if (req->query_params[i].name && strcmp(req->query_params[i].name, name) == 0) {
            HTTP_LOG("get_query_param: '%s' = '%s' (index %zu)", name, req->query_params[i].value ? req->query_params[i].value : "(null)", i);
            return req->query_params[i].value;
        }
    }
    HTTP_LOG("get_query_param: '%s' not found", name);
    return NULL;
}


/**
 * @brief Shortcut: set the response status + body to the same error message.
 *
 * Equivalent to `http_set_status(res, code, message)` followed by
 * `http_set_body(res, message)`. Useful for short 4xx/5xx responses
 * whose reason phrase doubles as the body text.
 *
 * @param res     Response to populate. NULL is a no-op.
 * @param code    HTTP status code.
 * @param message Reason phrase + body text.
 */
void http_send_error(HttpResponse* res, int code, const char* message) {
    if (!res || !message) {
        HTTP_LOG("send_error: NULL response or message — no-op");
        return;
    }
    HTTP_LOG("send_error: %d %s", code, message);

    http_set_status(res, code, message);
    http_set_body(res, message);
}


/**
 * @brief Canonical wire-format name of an HttpMethod.
 *
 * Returns a static string ("GET", "POST", "PUT", "DELETE", "OPTIONS",
 * "HEAD", "PATCH") suitable for logging or building outgoing requests.
 * Unknown / out-of-range values return "UNKNOWN".
 *
 * @param m Method enum.
 * @return Static, immutable string. Never NULL.
 */
const char* http_method_to_string(HttpMethod m) {
    switch (m) {
        case HTTP_GET:     
            return "GET";
        case HTTP_POST:    
            return "POST";
        case HTTP_PUT:     
            return "PUT";
        case HTTP_DELETE:  
            return "DELETE";
        case HTTP_OPTIONS: 
            return "OPTIONS";
        case HTTP_HEAD:    
            return "HEAD";
        case HTTP_PATCH:   
            return "PATCH";
        default:           
            return "UNKNOWN";
    }
}


/**
 * @brief Default reason phrase for a numeric HTTP status code.
 *
 * Covers the codes a typical server returns (1xx informational, 2xx
 * success, 3xx redirection, 4xx client errors, 5xx server errors).
 * Unknown codes fall back to a generic class label ("Informational",
 * "Success", "Redirection", "Client Error", "Server Error") so it can
 * still produce a valid status line.
 *
 * @param code HTTP status code.
 * @return Static, immutable reason phrase. Never NULL.
 */
const char* http_status_text(int code) {
    switch (code) {
        case 100: 
            return "Continue";
        case 101: 
            return "Switching Protocols";
        case 200: 
            return "OK";
        case 201: 
            return "Created";
        case 202: 
            return "Accepted";
        case 204: 
            return "No Content";
        case 206: 
            return "Partial Content";
        case 301: 
            return "Moved Permanently";
        case 302: 
            return "Found";
        case 303: 
            return "See Other";
        case 304: 
            return "Not Modified";
        case 307: 
            return "Temporary Redirect";
        case 308: 
            return "Permanent Redirect";
        case 400: 
            return "Bad Request";
        case 401: 
            return "Unauthorized";
        case 403: 
            return "Forbidden";
        case 404: 
            return "Not Found";
        case 405: 
            return "Method Not Allowed";
        case 408: 
            return "Request Timeout";
        case 409: 
            return "Conflict";
        case 410: 
            return "Gone";
        case 413: 
            return "Payload Too Large";
        case 415: 
            return "Unsupported Media Type";
        case 418: 
            return "I'm a teapot";
        case 422: 
            return "Unprocessable Entity";
        case 429: 
            return "Too Many Requests";
        case 500: 
            return "Internal Server Error";
        case 501: 
            return "Not Implemented";
        case 502: 
            return "Bad Gateway";
        case 503: 
            return "Service Unavailable";
        case 504: 
            return "Gateway Timeout";
        default:
            if (code >= 100 && code < 200) {
                return "Informational";
            }
            if (code >= 200 && code < 300) {
                return "Success";
            }
            if (code >= 300 && code < 400) {
                return "Redirection";
            }
            if (code >= 400 && code < 500) {
                return "Client Error";
            }
            if (code >= 500 && code < 600) {
                return "Server Error";
            }
            return "Unknown";
    }
}


/* Hex digit (0-9, a-f, A-F) to integer value, or -1 on miss. */
static int http_hex_value(int c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return 10 + (c - 'a');
    }
    if (c >= 'A' && c <= 'F') {
        return 10 + (c - 'A');
    }

    return -1;
}


/**
 * @brief Percent-decode a URL-encoded byte string.
 *
 * Translates `+` to space, `%XX` to the corresponding byte, and copies
 * every other byte verbatim. Malformed `%` escapes (truncated or
 * non-hex) are passed through literally so the function never reports
 * an error — useful when decoding mixed real-world input.
 *
 * The returned buffer is always NUL-terminated and at most `in_len`
 * bytes long (decoding only shortens). Caller owns the buffer and must
 * `free()` it.
 *
 * @param in     Source bytes. May contain embedded NULs only if
 *               `in_len` indicates the true length.
 * @param in_len Source length in bytes.
 * @return Newly-allocated decoded string, or NULL on `in == NULL` /
 *         allocation failure.
 */
char* http_url_decode(const char* in, size_t in_len) {
    if (!in) {
        return NULL;
    }

    char* out = (char*)malloc(in_len + 1);
    if (!out) {
        return NULL;
    }

    size_t o = 0;
    for (size_t i = 0; i < in_len; ++i) {
        char c = in[i];

        if (c == '+') {
            out[o++] = ' ';
        } 
        else if (c == '%' && i + 2 < in_len) {
            int hi = http_hex_value((unsigned char)in[i + 1]);
            int lo = http_hex_value((unsigned char)in[i + 2]);

            if (hi >= 0 && lo >= 0) {
                out[o++] = (char)((hi << 4) | lo);
                i += 2;
            } 
            else {
                out[o++] = c;  /* malformed escape: copy literally */
            }
        } 
        else {
            out[o++] = c;
        }
    }
    out[o] = '\0';
    return out;
}


/**
 * @brief Percent-encode a NUL-terminated string for use in URLs.
 *
 * Unreserved characters (`A-Z`, `a-z`, `0-9`, `-`, `_`, `.`, `~`) pass
 * through unchanged; every other byte is encoded as `%XX` (uppercase
 * hex). This matches the "unreserved" set defined in RFC 3986 §2.3,
 * which is what every server expects for query strings and path
 * segments.
 *
 * Worst case the output is 3× the input; the helper allocates that
 * upper bound up front and shrinks via NUL termination.
 *
 * @param in NUL-terminated input. NULL is rejected.
 * @return Newly-allocated encoded string. Caller frees.
 */
char* http_url_encode(const char* in) {
    if (!in) {
        return NULL;
    }

    size_t n = strlen(in);
    char* out = (char*)malloc(n * 3 + 1);
    if (!out) {
        return NULL;
    }

    static const char hex[] = "0123456789ABCDEF";
    size_t o = 0;
    for (size_t i = 0; i < n; ++i) {
        unsigned char c = (unsigned char)in[i];
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            out[o++] = (char)c;
        } 
        else {
            out[o++] = '%';
            out[o++] = hex[c >> 4];
            out[o++] = hex[c & 0xF];
        }
    }
    out[o] = '\0';
    return out;
}


/**
 * @brief Attach an HTML body to the response.
 *
 * Equivalent to `http_set_body` but appends
 * `Content-Type: text/html; charset=utf-8` instead of `text/plain`.
 * Frees any previous body, so repeated calls don't leak.
 *
 * @param response Response to update. NULL is a no-op.
 * @param html     Null-terminated HTML payload. Copied.
 */
void http_set_html_body(HttpResponse* response, const char* html) {
    if (!response || !html) {
        HTTP_LOG("set_html_body: NULL response or html — no-op");
        return;
    }
    free(response->body);

    response->body = string_strdup(html);
    HTTP_LOG("set_html_body: set %zu bytes, added Content-Type: text/html", strlen(html));
    http_add_header(response, "Content-Type", "text/html; charset=utf-8");
}


/**
 * @brief Append a `Set-Cookie` header with optional attributes.
 *
 * Builds the cookie string in the form
 * `name=value[; Path=...][; Max-Age=N][; HttpOnly]` and appends it via
 * `http_add_header`. Multiple calls produce multiple `Set-Cookie`
 * headers (which is allowed and is the standard way to send several
 * cookies on one response).
 *
 * @param response  Response to update. NULL is a no-op.
 * @param name      Cookie name. Required (NULL → no-op).
 * @param value     Cookie value. Required.
 * @param path      Optional `Path` attribute (NULL to omit).
 * @param max_age   Optional `Max-Age` in seconds. Use a negative value
 *                  to omit. `0` is a valid expiry that deletes the
 *                  cookie immediately.
 * @param http_only If true, appends `HttpOnly`.
 */
void http_set_cookie(HttpResponse* response, const char* name, const char* value, const char* path, int max_age, bool http_only) {
    if (!response || !name || !value) {
        HTTP_LOG("set_cookie: NULL response/name/value — no-op");
        return;
    }
    HTTP_LOG("set_cookie: name='%s' value='%s' path='%s' max_age=%d http_only=%d", name, value, path ? path : "(none)", max_age, http_only);

    /* Compute upper bound on the buffer: name=value + "; Path=..." (max
       ~64 chars worth of path) + "; Max-Age=-2147483648" + "; HttpOnly". */
    size_t need = strlen(name) + 1 + strlen(value) + 1;
    if (path) {     
        need += 8 + strlen(path);
    }
    if (max_age >= 0) {
        need += 32;
    }
    if (http_only) {   
        need += 12;
    }

    char* buf = (char*)malloc(need);
    if (!buf) {
        return;
    }

    size_t off = (size_t)snprintf(buf, need, "%s=%s", name, value);
    if (path) {
        off += (size_t)snprintf(buf + off, need - off, "; Path=%s", path);
    }
    if (max_age >= 0) {
        off += (size_t)snprintf(buf + off, need - off, "; Max-Age=%d", max_age);
    }
    if (http_only) {
        snprintf(buf + off, need - off, "; HttpOnly");
    }

    http_add_header(response, "Set-Cookie", buf);
    free(buf);
}

/**
 * @brief Look up a single cookie value on a parsed request.
 *
 * Reads the request's `Cookie` header and walks the
 * `name1=value1; name2=value2` list looking for `name`. Trims optional
 * surrounding whitespace. Returns a freshly-allocated copy of the
 * value (caller frees), or NULL if the header is missing, the named
 * cookie isn't present, or `name` is NULL.
 *
 * @param req  Parsed request. NULL → NULL.
 * @param name Cookie name (case-sensitive).
 * @return Heap-allocated cookie value (caller frees), or NULL.
 */
char* http_get_cookie(HttpRequest* req, const char* name) {
    if (!req || !name) {
        HTTP_LOG("get_cookie: NULL request or name");
        return NULL;
    }

    const char* cookie_header = http_get_header(req, "Cookie");
    if (!cookie_header) {
        HTTP_LOG("get_cookie: no Cookie header in request");
        return NULL;
    }
    HTTP_LOG("get_cookie: searching for '%s' in 'Cookie: %s'", name, cookie_header);

    size_t name_len = strlen(name);
    const char* p = cookie_header;
    while (*p) {
        /* Skip leading whitespace. */
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        const char* segment_end = strchr(p, ';');
        const char* end = segment_end ? segment_end : p + strlen(p);

        const char* eq = (const char*)memchr(p, '=', (size_t)(end - p));
        if (eq && (size_t)(eq - p) == name_len && memcmp(p, name, name_len) == 0) {
            const char* val_start = eq + 1;
            char* out = string_strndup(val_start, (size_t)(end - val_start));
            HTTP_LOG("get_cookie: '%s' = '%s'", name, out ? out : "(null)");
            return out;
        }

        if (!segment_end) {
            break;
        }
        p = segment_end + 1;
    }
    HTTP_LOG("get_cookie: '%s' not found", name);
    return NULL;
}


/**
 * @brief Emit a redirect response in one call.
 *
 * Sets the status code (default 302 if `code` is not a 3xx redirect
 * code), appends a `Location:` header, and writes a short HTML body
 * with the link as a fallback for clients that ignore headers. Any
 * previous body / status are overwritten.
 *
 * @param response Response to populate. NULL is a no-op.
 * @param code     3xx redirect code (301, 302, 303, 307, 308). Any
 *                 other value is coerced to 302.
 * @param location Target URL. Required (NULL → no-op).
 */
void http_redirect(HttpResponse* response, int code, const char* location) {
    if (!response || !location) {
        HTTP_LOG("redirect: NULL response or location — no-op");
        return;
    }
    if (code < 300 || code >= 400) {
        HTTP_LOG("redirect: non-3xx code %d coerced to 302", code);
        code = 302;
    }
    HTTP_LOG("redirect: %d -> '%s'", code, location);

    http_set_status(response, code, http_status_text(code));
    http_add_header(response, "Location", location);

    /* Minimal HTML fallback body. */
    size_t need = 64 + 2 * strlen(location);
    char* body = (char*)malloc(need);
    if (body) {
        snprintf(body, need,"<html><body>Redirecting to <a href=\"%s\">%s</a></body></html>", location, location);
        http_set_html_body(response, body);
        free(body);
    }
}


/**
 * @brief Look up a header on a response by name (case-sensitive).
 *
 * Mirrors `http_get_header` but for the response side, so handlers can
 * inspect what's been set so far (e.g. before deciding whether to add
 * a Content-Type).
 *
 * @param response Response to search. NULL → NULL.
 * @param name     Header name.
 * @return Pointer to the value still owned by the response, or NULL if
 *         not present.
 */
const char* http_response_get_header(HttpResponse* response, const char* name) {
    if (!response || !name) {
        HTTP_LOG("response_get_header: NULL response or name");
        return NULL;
    }

    for (size_t i = 0; i < response->header_count; ++i) {
        if (response->headers[i].name && strcmp(response->headers[i].name, name) == 0) {
            HTTP_LOG("response_get_header: '%s' = '%s' (index %zu)", name, response->headers[i].value ? response->headers[i].value : "(null)", i);
            return response->headers[i].value;
        }
    }
    HTTP_LOG("response_get_header: '%s' not found", name);
    return NULL;
}


/**
 * @brief Remove every response header with the given name.
 *
 * Frees the matching `name`/`value` strings, shifts the remaining
 * headers down to keep them contiguous, and decrements
 * `header_count`. Useful when `http_set_json_body` or `http_set_body`
 * unconditionally added a `Content-Type` header and you want to
 * override it.
 *
 * @param response Response to modify. NULL → 0.
 * @param name     Header name to remove (case-sensitive).
 * @return Number of headers removed (0 if none matched).
 */
size_t http_response_remove_header(HttpResponse* response, const char* name) {
    if (!response || !name) {
        HTTP_LOG("response_remove_header: NULL response or name");
        return 0;
    }
    HTTP_LOG("response_remove_header: searching '%s' (header_count=%zu)", name, response->header_count);

    size_t removed = 0;
    size_t i = 0;

    while (i < response->header_count) {
        if (response->headers[i].name &&
            strcmp(response->headers[i].name, name) == 0) {
            free(response->headers[i].name);
            free(response->headers[i].value);

            for (size_t j = i + 1; j < response->header_count; ++j) {
                response->headers[j - 1] = response->headers[j];
            }
            response->header_count--;
            removed++;
        }
        else {
            i++;
        }
    }
    HTTP_LOG("response_remove_header: removed %zu instance(s) of '%s'", removed, name);
    return removed;
}


/* Case-insensitive (ASCII) comparison of two NUL-terminated strings. */
static bool http_ascii_ieq(const char* a, const char* b) {
    if (!a || !b) {
        return false;
    }
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return false;
        }
        ++a;
        ++b;
    }
    return *a == '\0' && *b == '\0';
}


/**
 * @brief Case-insensitive header lookup.
 *
 * Like `http_get_header`, but matches @p name case-insensitively, as required
 * by RFC 7230 §3.2 (HTTP header field names are case-insensitive). Prefer this
 * in production: a client may send `content-type` while your code looks up
 * `Content-Type`, and the case-sensitive `http_get_header` would miss it.
 *
 * The returned pointer is owned by the request — do not free it; it is valid
 * until `http_free_request`.
 *
 * @param req  Parsed request to search. NULL → NULL.
 * @param name Header name to find (case-insensitive). NULL → NULL.
 * @return The matching header value, or NULL if absent. If the same header
 *         appears more than once, the first occurrence is returned.
 */
const char* http_get_header_ci(const HttpRequest* req, const char* name) {
    if (!req || !name) {
        HTTP_LOG("get_header_ci: NULL request or name");
        return NULL;
    }
    for (size_t i = 0; i < req->header_count; ++i) {
        if (http_ascii_ieq(req->headers[i].name, name)) {
            HTTP_LOG("get_header_ci: '%s' matched at index %zu", name, i);
            return req->headers[i].value;
        }
    }
    HTTP_LOG("get_header_ci: '%s' not found", name);
    return NULL;
}


/**
 * @brief Convenience accessor for the request's `Content-Type` header.
 *
 * Equivalent to `http_get_header_ci(req, "Content-Type")`. The value may
 * include parameters (e.g. `"application/json; charset=utf-8"`).
 *
 * @param req Parsed request. NULL → NULL.
 * @return The Content-Type value (owned by the request), or NULL if absent.
 */
const char* http_content_type(const HttpRequest* req) {
    return http_get_header_ci(req, "Content-Type");
}


/**
 * @brief Returns the request's `Content-Length` as a non-negative byte count.
 *
 * Looks the header up case-insensitively and parses it strictly as RFC 7230
 * `1*DIGIT`: the value must be non-empty and contain digits only (no sign,
 * no whitespace, no trailing junk). Overflow is detected and rejected.
 *
 * @param req Parsed request. NULL → -1.
 * @return The declared body length (>= 0), or -1 if the header is absent,
 *         empty, malformed, or would overflow `long`.
 */
long http_content_length(const HttpRequest* req) {
    const char* v = http_get_header_ci(req, "Content-Length");
    if (!v || !*v) {
        return -1;
    }
    long n = 0;
    for (const char* p = v; *p; ++p) {
        if (*p < '0' || *p > '9') {
            return -1;                          /* digits only */
        }
        int d = *p - '0';
        if (n > (LONG_MAX - d) / 10) {
            return -1;                          /* would overflow */
        }
        n = n * 10 + d;
    }
    return n;
}


/**
 * @brief Parse a bare HTTP method token into an `HttpMethod`.
 *
 * Recognises the canonical, uppercase verbs `GET`, `POST`, `PUT`, `DELETE`,
 * `OPTIONS`, `HEAD` and `PATCH`. HTTP methods are case-sensitive (RFC 7230
 * §3.1.1), so lowercase input is rejected. This is the inverse of
 * `http_method_to_string`, and differs from `http_parse_method`, which expects
 * a full request line (a verb followed by a space).
 *
 * @param str        The method token (e.g. "GET"). NULL → false.
 * @param out_method Receives the parsed method on success. NULL → false.
 * @return `true` and writes @p out_method if recognised; `false` otherwise
 *         (in which case @p out_method is left unchanged).
 */
bool http_method_from_string(const char* str, HttpMethod* out_method) {
    if (!str || !out_method) {
        return false;
    }
    static const struct { const char* name; HttpMethod method; } table[] = {
        { "GET",     HTTP_GET     },
        { "POST",    HTTP_POST    },
        { "PUT",     HTTP_PUT     },
        { "DELETE",  HTTP_DELETE  },
        { "OPTIONS", HTTP_OPTIONS },
        { "HEAD",    HTTP_HEAD    },
        { "PATCH",   HTTP_PATCH   },
    };
    for (size_t i = 0; i < sizeof(table) / sizeof(table[0]); ++i) {
        if (strcmp(str, table[i].name) == 0) {
            *out_method = table[i].method;
            return true;
        }
    }
    return false;
}


/**
 * @brief Returns the class (leading digit) of an HTTP status code.
 *
 * Maps a status code to its category: 1 (1xx informational), 2 (2xx success),
 * 3 (3xx redirection), 4 (4xx client error) or 5 (5xx server error). Codes
 * outside the valid 100–599 range yield 0.
 *
 * @param code HTTP status code.
 * @return 1..5 for a valid code, or 0 if out of range.
 */
int http_status_class(int code) {
    if (code < 100 || code > 599) {
        return 0;
    }
    return code / 100;
}
