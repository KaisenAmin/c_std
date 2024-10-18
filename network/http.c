#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "../string/std_string.h"
#include "../fmt/fmt.h"


static HttpRoute routes[MAX_ROUTES];
static size_t route_count = 0;
static bool server_running = true;

// Function to match routes with dynamic segments
static bool match_route(const char* route, const char* path, int* id_out) {
    fmt_printf("Debug: Matching route '%s' against path '%s'\n", route, path);

    const char* route_ptr = route;
    const char* path_ptr = path;

    while (*route_ptr && *path_ptr) {
        if (*route_ptr == '{') {
            // Skip the segment in the route template (e.g., "{id}")
            while (*route_ptr && *route_ptr != '}') route_ptr++;
            route_ptr++; // Skip '}'

            // Parse the dynamic segment in the path
            *id_out = atoi(path_ptr);
            while (*path_ptr && *path_ptr != '/') path_ptr++;
        } 
        else {
            // Match static segments
            if (*route_ptr != *path_ptr) {
                fmt_printf("Debug: Route segment mismatch: '%c' vs '%c'\n", *route_ptr, *path_ptr);
                return false; // Mismatch
            }
            route_ptr++;
            path_ptr++;
        }
    }

    bool match = *route_ptr == '\0' && *path_ptr == '\0';
    fmt_printf("Debug: Route match result: %d\n", match);
    return match;
}


static void handle_request(HttpRequest* req, HttpResponse* res) {
    for (size_t i = 0; i < route_count; i++) {
        int id = -1;
        if (match_route(routes[i].path, req->path, &id) && routes[i].method == req->method) {
            req->id = id; // Pass the extracted ID to the request
            routes[i].handler(req, res);
            return;
        }
    }
    http_send_error(res, 404, "Route not found");
}

HttpMethod http_parse_method(const char* request) {
    if (strncmp(request, "GET", 3) == 0) { 
        return HTTP_GET;
    }
    else if (strncmp(request, "POST", 4) == 0) { 
        return HTTP_POST;
    }
    else if (strncmp(request, "PUT", 3) == 0) {
        return HTTP_PUT;
    }
    else if (strncmp(request, "DELETE", 6) == 0) { 
        return HTTP_DELETE;
    }
    else if (strncmp(request, "OPTIONS", 7) == 0) { 
        return HTTP_OPTIONS;
    }
    else if (strncmp(request, "HEAD", 4) == 0) { 
        return HTTP_HEAD;
    }
    else if (strncmp(request, "PATCH", 5) == 0) { 
        return HTTP_PATCH;
    }

    return HTTP_UNKNOWN;
}

HttpRequest* http_parse_request(const char* request) {
    HttpRequest* req = (HttpRequest*)malloc(sizeof(HttpRequest));
    memset(req, 0, sizeof(HttpRequest));

    req->method = http_parse_method(request);

    // Parse the path and query string
    const char* path_start = strchr(request, ' ') + 1;
    const char* path_end = strchr(path_start, ' ');
    req->path = string_strndup(path_start, path_end - path_start);

    // Parse query parameters if present
    char* query_start = strchr(req->path, '?');
    if (query_start) {
        *query_start = '\0'; 
        char* query_string = string_strdup(query_start + 1);  // Duplicate the query string
        char* token = strtok(query_string, "&");

        while (token) {
            char* equal_sign = strchr(token, '=');
            if (equal_sign) {
                *equal_sign = '\0';
                req->query_params[req->query_param_count].name = string_strdup(token);
                req->query_params[req->query_param_count].value = string_strdup(equal_sign + 1);
                req->query_param_count++;
            }
            token = strtok(NULL, "&");
        }
        free(query_string);
    }

    // Parse headers
    const char* header_start = strstr(request, "\r\n") + 2;
    while (header_start && *header_start != '\r') {
        const char* header_end = strstr(header_start, "\r\n");
        if (!header_end) break;

        const char* colon = strchr(header_start, ':');
        if (colon && colon < header_end) {
            req->headers[req->header_count].name = string_strndup(header_start, colon - header_start);
            req->headers[req->header_count].value = string_strndup(colon + 2, header_end - colon - 2);
            req->header_count++;
        }

        header_start = header_end + 2;
    }

    // Parse body (assuming it's a JSON payload for POST)
    const char* body_start = strstr(request, "\r\n\r\n");
    if (body_start) {
        body_start += 4;
        req->body = string_strdup(body_start);
        
        fmt_printf("Debug: Request body before JSON parse: %s\n", req->body);
        
        req->json_body = json_parse(req->body);
        if (!req->json_body) {
            fmt_fprintf(stderr, "Error: Failed to parse JSON body\n");
        }
    }


    return req;
}

void http_free_request(HttpRequest* request) {
    if (!request) { 
        return;
    }
    if (request->path) { 
        free(request->path);
    }
    if (request->body) { 
        free(request->body);
    }
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

void http_set_status(HttpResponse* response, int code, const char* message) {
    response->status_code = code;
    response->status_message = string_strdup(message);
}

void http_set_json_body(HttpResponse* response, JsonElement* json) {
    if (json) {
        fmt_printf("Debug: Setting JSON body\n");

        if (response->json_body) {
            fmt_printf("Debug: Deallocating previous JSON body\n");
            json_deallocate(response->json_body);
        }

        response->json_body = json;
        
        if (response->body) {
            free(response->body);
        }
        response->body = json_serialize(json);

        // Set the Content-Type header to application/json
        http_add_header(response, "Content-Type", "application/json");
    } else {
        fmt_fprintf(stderr, "Error: Null JSON body received\n");
        http_set_status(response, 500, "Internal Server Error: Null JSON Body");
        http_set_body(response, "Failed to set JSON body: NULL pointer received");
    }
}

void http_set_body(HttpResponse* response, const char* body) {
    response->body = string_strdup(body);
    http_add_header(response, "Content-Type", "text/plain");
}

void http_add_header(HttpResponse* response, const char* header, const char* value) {
    response->headers[response->header_count].name = string_strdup(header);
    response->headers[response->header_count].value = string_strdup(value);
    response->header_count++;
}

char* http_serialize_response(HttpResponse* response) {
    size_t estimated_size = 1024 + (response->json_body ? 1024 : 0) + (response->body ? strlen(response->body) : 0);

    char* buffer = (char*)malloc(estimated_size);
    snprintf(buffer, estimated_size, "HTTP/1.1 %d %s\r\n", response->status_code, response->status_message);

    for (size_t i = 0; i < response->header_count; i++) {
        strcat(buffer, response->headers[i].name);
        strcat(buffer, ": ");
        strcat(buffer, response->headers[i].value);
        strcat(buffer, "\r\n");
    }

    strcat(buffer, "\r\n");

    if (response->json_body) {
        char* json_str = json_serialize(response->json_body);
        fmt_printf("Debug: Serialized JSON body: %s\n", json_str);

        strcat(buffer, json_str);
        free(json_str);
    } 
    else if (response->body) {
        strcat(buffer, response->body);
    }

    return buffer;
}


void http_free_response(HttpResponse* response) {
    if (response->status_message) {
        free(response->status_message);
    }
    if (response->json_body) { 
        json_deallocate(response->json_body);
    }
    if (response->body) { 
        free(response->body);
    }

    for (size_t i = 0; i < response->header_count; i++) {
        free(response->headers[i].name);
        free(response->headers[i].value);
    }
}

void http_register_route(const char* path, HttpMethod method, HttpHandler handler) {
    if (route_count < MAX_ROUTES) {
        routes[route_count].path = string_strdup(path);
        routes[route_count].method = method;
        routes[route_count].handler = handler;
        route_count++;
    }
}

void http_start_server(int port) {
    TcpSocket server_socket;
    TcpStatus status;

    status = tcp_init();
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to initialize network\n");
        return;
    }

    status = tcp_socket_create(&server_socket);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to create server socket\n");
        return;
    }

    status = tcp_bind(server_socket, "0.0.0.0", port);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to bind server socket\n");
        return;
    }

    status = tcp_listen(server_socket, 10);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to listen on server socket\n");
        return;
    }

    fmt_printf("HTTP Server listening on port %d\n", port);

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
    fmt_printf("HTTP Server stopped\n");
}

void http_stop_server(void) {
    server_running = false;
}

void http_handle_request(TcpSocket client_socket) {
    char buffer[4096];
    size_t received;
    TcpStatus status = tcp_recv(client_socket, buffer, sizeof(buffer), &received);

    if (status == TCP_SUCCESS && received > 0) {
        buffer[received] = '\0';

        fmt_printf("Debug: Received request: %s\n", buffer);  // Log the entire request

        HttpRequest* req = http_parse_request(buffer);
        if (!req) {
            fmt_fprintf(stderr, "Error: Failed to parse HTTP request\n");
            HttpResponse res = {0};
            http_send_error(&res, 400, "Bad Request: Invalid HTTP Request");
            char* response_str = http_serialize_response(&res);
            tcp_send(client_socket, response_str, strlen(response_str), &received);
            free(response_str);
            return;
        }

        HttpResponse res = {0};
        res.status_code = 200;
        res.status_message = string_strdup("OK");

        handle_request(req, &res);

        char* response_str = http_serialize_response(&res);

        if (response_str) {
            size_t sent;
            tcp_send(client_socket, response_str, strlen(response_str), &sent);
            free(response_str);
        }

        http_free_request(req);
        http_free_response(&res);
    } 
    else {
        fmt_fprintf(stderr, "Error: Failed to receive data from socket or no data received\n");
    }
}

const char* http_get_header(HttpRequest* req, const char* name) {
    for (size_t i = 0; i < req->header_count; i++) {
        if (strcmp(req->headers[i].name, name) == 0) {
            return req->headers[i].value;
        }
    }

    return NULL;
}

const char* http_get_query_param(HttpRequest* req, const char* name) {
    for (size_t i = 0; i < req->query_param_count; i++) {
        if (strcmp(req->query_params[i].name, name) == 0) {
            return req->query_params[i].value;
        }
    }
    return NULL;
}

void http_send_error(HttpResponse* res, int code, const char* message) {
    http_set_status(res, code, message);
    http_set_body(res, message);
}
