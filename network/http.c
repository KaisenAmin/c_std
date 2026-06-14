#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include "http.h"
#include "../string/std_string.h"
#include "../concurrent/thread_pool.h"
#include "../concurrent/concurrent.h"

/* POSIX raises SIGPIPE (default action: terminate the process) when you write
 * to a socket whose peer has closed its read end. A server must never die
 * because one client hung up, so we ignore the signal once at start-up. Windows
 * has no SIGPIPE (send simply fails), so this is a no-op there. */
#ifndef _WIN32
#include <signal.h>
#endif


/* Resource limits for the request reader (override at build time with -D...).
 * They bound the two unbounded inputs an attacker controls so a single
 * connection can't exhaust memory: the header block and the body. */
#ifndef HTTP_MAX_HEADER_BYTES
#define HTTP_MAX_HEADER_BYTES (64u * 1024u)          /* 64 KiB of headers -> 431 */
#endif
#ifndef HTTP_MAX_BODY_BYTES
#define HTTP_MAX_BODY_BYTES   (16u * 1024u * 1024u)  /* 16 MiB body      -> 413 */
#endif
#ifndef HTTP_READ_CHUNK
#define HTTP_READ_CHUNK       (16u * 1024u)          /* recv() granularity        */
#endif

/* Concurrency / lifecycle tunables (override at build time with -D...). */
#ifndef HTTP_LISTEN_BACKLOG
#define HTTP_LISTEN_BACKLOG   128                    /* pending-connection queue  */
#endif
#ifndef HTTP_DEFAULT_WORKERS
#define HTTP_DEFAULT_WORKERS  8                      /* fallback pool size        */
#endif
#ifndef HTTP_MAX_WORKERS
#define HTTP_MAX_WORKERS      256                    /* clamp auto/explicit sizing*/
#endif
#ifndef HTTP_DEFAULT_TIMEOUT_MS
#define HTTP_DEFAULT_TIMEOUT_MS 30000L               /* per-conn recv/send timeout*/
#endif


static HttpRoute routes[MAX_ROUTES];
static size_t route_count = 0;

/* ------------------------------------------------------------------ */
/* Server lifecycle state (shared across the accept thread, the worker */
/* pool, and whatever thread calls http_stop_server). All mutable      */
/* fields are guarded by `srv_lock`, which is initialized exactly once  */
/* via call_once so start/stop are safe to call in either order.        */
/* The route table is written only by http_register_route (before the   */
/* server starts) and is read-only while serving, so it needs no lock.  */
/* ------------------------------------------------------------------ */
static Mutex          srv_lock;
static OnceFlag       srv_lock_once = ONCE_FLAG_INIT;
static bool           server_running        = false; /* accept loop should run    */
static bool           srv_listening         = false; /* listen socket is live     */
static TcpSocket      srv_listen_sock;                /* valid while srv_listening */
static unsigned short srv_port              = 0;      /* actual bound port         */
static int            srv_worker_count      = 0;      /* 0 => auto (hw concurrency)*/
static long           srv_client_timeout_ms = HTTP_DEFAULT_TIMEOUT_MS;

static void hp_srv_lock_init(void) {
    mutex_init(&srv_lock, MUTEX_PLAIN);
}

/* Ignore SIGPIPE exactly once (POSIX); no-op on Windows. */
#ifndef _WIN32
static OnceFlag srv_sigpipe_once = ONCE_FLAG_INIT;
static void hp_ignore_sigpipe(void) {
    signal(SIGPIPE, SIG_IGN);
}
#endif


/* ------------------------------------------------------------------ */
/* Internal helpers for framing-correct request reading               */
/* ------------------------------------------------------------------ */

static int hp_hex_value(int c);   /* defined later; used by the chunk decoder */

/* True if `s` contains the token "chunked" (case-insensitive). Good enough to
 * detect `Transfer-Encoding: chunked` (possibly after other codings). */
static bool ci_contains_chunked(const char* s) {
    if (!s) {
        return false;
    }
    size_t n = strlen(s);
    for (size_t i = 0; i + 7 <= n; ++i) {
        if (tolower((unsigned char)s[i + 0]) == 'c' && tolower((unsigned char)s[i + 1]) == 'h' &&
            tolower((unsigned char)s[i + 2]) == 'u' && tolower((unsigned char)s[i + 3]) == 'n' &&
            tolower((unsigned char)s[i + 4]) == 'k' && tolower((unsigned char)s[i + 5]) == 'e' &&
            tolower((unsigned char)s[i + 6]) == 'd') {
            return true;
        }
    }
    return false;
}

/* Strict RFC-7230 Content-Length parse: 1*DIGIT, overflow-checked. -1 on miss. */
static long hp_parse_clen(const char* v) {
    if (!v || !*v) {
        return -1;
    }
    long n = 0;
    for (const char* p = v; *p; ++p) {
        if (*p < '0' || *p > '9') {
            return -1;
        }
        int d = *p - '0';
        if (n > (LONG_MAX - d) / 10) {
            return -1;
        }
        n = n * 10 + d;
    }
    return n;
}

/* Case-insensitive header lookup within a RAW header block (bytes [0,header_len),
 * i.e. up to and including the terminating CRLFCRLF). Copies the OWS-trimmed
 * value into `out` (always NUL-terminated, truncated to out_size). Used by the
 * reader before a full HttpRequest exists. */
static bool raw_header_value(const char* buf, size_t header_len, const char* name,
                             char* out, size_t out_size) {
    if (out_size == 0) {
        return false;
    }
    out[0] = '\0';
    size_t namelen = strlen(name);
    const char* first_nl = (const char*)memchr(buf, '\n', header_len);
    if (!first_nl) {
        return false;
    }
    const char* p = first_nl + 1;   /* skip the request line */
    while ((size_t)(p - buf) < header_len) {
        const char* nl  = (const char*)memchr(p, '\n', header_len - (size_t)(p - buf));
        const char* end = nl ? nl : buf + header_len;
        const char* colon = (const char*)memchr(p, ':', (size_t)(end - p));
        
        if (colon && (size_t)(colon - p) == namelen) {
            bool eq = true;
            for (size_t i = 0; i < namelen; ++i) {
                if (tolower((unsigned char)p[i]) != tolower((unsigned char)name[i])) {
                    eq = false;
                    break;
                }
            }
            if (eq) {
                const char* v  = colon + 1;
                const char* ve = end;
                if (ve > v && ve[-1] == '\r') {
                    ve--;
                }
                while (v < ve && (*v == ' ' || *v == '\t')) {
                    v++;
                }
                while (ve > v && (ve[-1] == ' ' || ve[-1] == '\t')) {
                    ve--;
                }
                size_t vl = (size_t)(ve - v);
                if (vl >= out_size) {
                    vl = out_size - 1;
                }
                memcpy(out, v, vl);
                out[vl] = '\0';
                return true;
            }
        }
        if (!nl) {
            break;
        }
        p = nl + 1;
    }
    return false;
}


/* Growable byte buffer (for the decoded chunked body). */
typedef struct { char* data; size_t len; size_t cap; } HttpBuf;

static bool httpbuf_append(HttpBuf* b, const char* src, size_t n, size_t max) {
    if (n == 0) {
        return true;
    }
    if (b->len > max - n) {     /* b->len + n > max, overflow-safe */
        return false;
    }
    if (n > b->cap - b->len) {  /* need to grow */
        size_t nc = b->cap ? b->cap : 4096;
        while (nc < b->len + n) {
            nc *= 2;
        }
        char* nd = (char*)realloc(b->data, nc);
        if (!nd) {
            return false;
        }
        b->data = nd;
        b->cap  = nc;
    }
    memcpy(b->data + b->len, src, n);
    b->len += n;
    return true;
}


/* Incremental (streaming) chunked-transfer decoder — O(n), survives reads that
 * split a chunk anywhere. Feed bytes; decoded data is appended to `out`. */
typedef enum {
    CKD_SIZE, CKD_SIZE_LF, CKD_DATA, CKD_DATA_CR, CKD_DATA_LF,
    CKD_TRAILER, CKD_TRAILER_LF, CKD_TRAILER_SKIP, CKD_TRAILER_SKIP_LF,
    CKD_DONE, CKD_ERROR
} CkdPhase;

typedef struct {
    CkdPhase phase;
    size_t   need;     /* size being parsed, then data bytes remaining */
    size_t   digits;   /* hex digits seen for the current size         */
    bool     in_ext;   /* inside a chunk-extension (after ';')         */
} CkdState;

/* Returns false (and leaves phase == CKD_ERROR) on protocol error / overflow /
 * exceeding `max`. Completion is signalled by st->phase == CKD_DONE. */
static bool ckd_feed(CkdState* st, const char* in, size_t n, HttpBuf* out, size_t max) {
    size_t i = 0;
    while (i < n && st->phase != CKD_DONE && st->phase != CKD_ERROR) {
        char c = in[i];
        switch (st->phase) {
            case CKD_SIZE:
                if (st->in_ext) {
                    if (c == '\r') { st->phase = CKD_SIZE_LF; }
                    i++;
                }
                else if (c == ';') { st->in_ext = true; i++; }
                else if (c == '\r') { st->phase = CKD_SIZE_LF; i++; }
                else {
                    int h = hp_hex_value((unsigned char)c);
                    if (h < 0 || st->need > (SIZE_MAX - (size_t)h) / 16) { st->phase = CKD_ERROR; break; }
                    st->need = st->need * 16 + (size_t)h;
                    st->digits++;
                    i++;
                }
                break;
            case CKD_SIZE_LF:
                if (c != '\n' || st->digits == 0) { st->phase = CKD_ERROR; break; }
                i++;
                st->phase = (st->need == 0) ? CKD_TRAILER : CKD_DATA;
                break;
            case CKD_DATA: {
                size_t take = n - i;
                if (take > st->need) { take = st->need; }
                if (!httpbuf_append(out, in + i, take, max)) { st->phase = CKD_ERROR; break; }
                st->need -= take;
                i += take;
                if (st->need == 0) { st->phase = CKD_DATA_CR; }
                break;
            }
            case CKD_DATA_CR:
                if (c != '\r') { st->phase = CKD_ERROR; break; }
                i++; st->phase = CKD_DATA_LF;
                break;
            case CKD_DATA_LF:
                if (c != '\n') { st->phase = CKD_ERROR; break; }
                i++; st->phase = CKD_SIZE; st->need = 0; st->digits = 0; st->in_ext = false;
                break;
            case CKD_TRAILER:                       /* at start of a trailer line */
                if (c == '\r') { st->phase = CKD_TRAILER_LF; }
                else { st->phase = CKD_TRAILER_SKIP; }
                i++;
                break;
            case CKD_TRAILER_LF:
                if (c != '\n') { st->phase = CKD_ERROR; break; }
                i++; st->phase = CKD_DONE;
                break;
            case CKD_TRAILER_SKIP:                  /* skip a non-empty trailer line */
                if (c == '\r') { st->phase = CKD_TRAILER_SKIP_LF; }
                i++;
                break;
            case CKD_TRAILER_SKIP_LF:
                if (c != '\n') { st->phase = CKD_ERROR; break; }
                i++; st->phase = CKD_TRAILER;
                break;
            default:
                i++;
                break;
        }
    }
    return st->phase != CKD_ERROR;
}



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

    /* 1) Exact (path, method) match. */
    for (size_t i = 0; i < route_count; i++) {
        int id = -1;
        if (routes[i].method == req->method && match_route(routes[i].path, req->path, &id)) {
            HTTP_LOG("route hit: index=%zu template='%s' method=%s id=%d",
                     i, routes[i].path, http_method_to_string(routes[i].method), id);
            req->id = id;
            routes[i].handler(req, res);
            return;
        }
    }

    /* 2) A HEAD with no HEAD route falls back to the matching GET handler; the
     *    serializer strips the body so only headers go on the wire. */
    if (req->method == HTTP_HEAD) {
        for (size_t i = 0; i < route_count; i++) {
            int id = -1;
            if (routes[i].method == HTTP_GET && match_route(routes[i].path, req->path, &id)) {
                HTTP_LOG("HEAD -> GET fallback on '%s'", routes[i].path);
                req->id = id;
                routes[i].handler(req, res);
                return;
            }
        }
    }

    /* 3) Path matched some route but not this method -> 405 + Allow. */
    bool allowed[HTTP_UNKNOWN] = { false };
    bool any = false;
    for (size_t i = 0; i < route_count; i++) {
        if (match_route(routes[i].path, req->path, NULL) && routes[i].method < HTTP_UNKNOWN) {
            allowed[routes[i].method] = true;
            any = true;
        }
    }
    if (any) {
        if (allowed[HTTP_GET]) {
            allowed[HTTP_HEAD] = true;     /* GET implies HEAD */
        }
        char allow[160];
        size_t off = 0;
        allow[0] = '\0';
        static const HttpMethod order[] = {
            HTTP_GET, HTTP_HEAD, HTTP_POST, HTTP_PUT, HTTP_PATCH, HTTP_DELETE, HTTP_OPTIONS
        };
        for (size_t k = 0; k < sizeof(order) / sizeof(order[0]); ++k) {
            if (!allowed[order[k]]) {
                continue;
            }
            int n = snprintf(allow + off, sizeof(allow) - off, "%s%s",
                             off ? ", " : "", http_method_to_string(order[k]));
            if (n > 0 && (size_t)n < sizeof(allow) - off) {
                off += (size_t)n;
            }
        }
        HTTP_LOG("method %s not allowed on '%s' -> 405 (Allow: %s)",
                 http_method_to_string(req->method), req->path ? req->path : "(null)", allow);
        http_set_status(res, 405, "Method Not Allowed");
        http_add_header(res, "Allow", allow);
        http_set_body(res, "Method Not Allowed");
        return;
    }

    /* 4) No route matched the path at all -> 404. */
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
 * @param length  Total number of bytes in `request`; enables length-aware,
 *                binary-safe parsing of bodies that may contain embedded NULs.
 * @return Newly-allocated request, or NULL on malformed input / OOM.
 */
HttpRequest* http_parse_request_ex(const char* request, size_t length) {
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

    // Parse body — length-aware so binary bodies with embedded NULs survive
    // intact (string_strdup would have truncated at the first NUL).
    const char* body_start = strstr(request, "\r\n\r\n");
    if (body_start) {
        body_start += 4;
        size_t body_off = (size_t)(body_start - request);
        size_t body_len = (length > body_off) ? (length - body_off) : 0;

        if (body_len > 0) {
            req->body = (char*)malloc(body_len + 1);
            if (!req->body) {
                HTTP_LOG("parse_request: OOM duplicating body");
                http_free_request(req);
                return NULL;
            }
            memcpy(req->body, body_start, body_len);
            req->body[body_len] = '\0';   /* convenience NUL; body_length is authoritative */
            req->body_length = body_len;
            HTTP_LOG("parse_request: body present (%zu bytes)", body_len);
            // Only attempt JSON parsing if it actually looks like JSON.
            if (*body_start == '{' || *body_start == '[') {
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
 * @brief Back-compat wrapper: parse a NUL-terminated raw request string.
 *
 * Equivalent to `http_parse_request_ex(request, strlen(request))`. Use the
 * `_ex` form when the request bytes are not NUL-terminated or the body may
 * contain embedded NULs.
 *
 * @param request Null-terminated raw request bytes. NULL is rejected.
 * @return Newly-allocated request, or NULL on malformed input / OOM.
 */
HttpRequest* http_parse_request(const char* request) {
    if (!request) {
        return NULL;
    }
    return http_parse_request_ex(request, strlen(request));
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
 * @param response  Response to serialize.
 * @param head_only When true, emit headers only and suppress the body (for
 *                  replying to a HEAD request) while keeping the Content-Length.
 * @param out_len   Output: exact byte length of the returned buffer (the body
 *                  may contain NULs, so this is authoritative). May be NULL.
 * @return Newly-allocated null-terminated string, or NULL on OOM / NULL input.
 */
char* http_serialize_response_ex(HttpResponse* response, bool head_only, size_t* out_len) {
    if (out_len) {
        *out_len = 0;
    }
    if (!response) {
        HTTP_LOG("serialize_response: NULL response -> NULL");
        return NULL;
    }

    const char* status_msg = response->status_message ? response->status_message : "OK";
    char* json_str = NULL;
    const char* body = NULL;
    size_t body_len = 0;

    if (response->json_body) {
        json_str = json_serialize(response->json_body);
        body     = json_str;
        body_len = json_str ? strlen(json_str) : 0;
    }
    else if (response->body) {
        body     = response->body;
        body_len = strlen(response->body);
    }

    /* Framing: respect a handler-set Content-Length or a Transfer-Encoding:
     * chunked header; otherwise auto-add the correct Content-Length. Without
     * this, HTTP/1.1 replies are unframed and only "work" by closing the
     * connection — which breaks keep-alive and strict clients/proxies. */
    bool has_cl = (http_response_get_header(response, "Content-Length") != NULL);
    const char* te = http_response_get_header(response, "Transfer-Encoding");
    bool chunked = ci_contains_chunked(te);
    bool auto_cl = !has_cl && !chunked;

    char   cl_line[48];
    size_t cl_line_len = 0;
    if (auto_cl) {
        int n = snprintf(cl_line, sizeof cl_line, "Content-Length: %zu\r\n", body_len);
        cl_line_len = (n > 0) ? (size_t)n : 0;
    }

    /* For chunked, frame the (fully known) body as a single chunk + terminator. */
    char        chk_hdr[32];
    size_t      chk_hdr_len = 0;
    const char  chk_tail[]  = "\r\n0\r\n\r\n";
    const size_t chk_tail_len = sizeof(chk_tail) - 1;

    if (chunked && !head_only && body_len > 0) {
        int n = snprintf(chk_hdr, sizeof chk_hdr, "%zx\r\n", body_len);
        chk_hdr_len = (n > 0) ? (size_t)n : 0;
    }

    /* Upper bound on the wire size. */
    size_t total = 32 + strlen(status_msg);
    for (size_t i = 0; i < response->header_count; i++) {
        const char* nm = response->headers[i].name  ? response->headers[i].name  : "";
        const char* vl = response->headers[i].value ? response->headers[i].value : "";
        total += strlen(nm) + 2 + strlen(vl) + 2;
    }
    total += cl_line_len;
    total += 2;  /* blank line */
    if (!head_only) {
        if (chunked) {
            total += (body_len > 0) ? (chk_hdr_len + body_len + chk_tail_len) : 5 /* "0\r\n\r\n" */;
        }
        else {
            total += body_len;
        }
    }
    total += 1;  /* NUL */

    char* buffer = (char*)malloc(total);
    if (!buffer) {
        HTTP_LOG("serialize_response: OOM allocating %zu-byte wire buffer", total);
        free(json_str);
        return NULL;
    }

    int n = snprintf(buffer, total, "HTTP/1.1 %d %s\r\n",
                     response->status_code ? response->status_code : 200, status_msg);
    if (n < 0) {
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
    if (auto_cl && cl_line_len && off + cl_line_len < total) {
        memcpy(buffer + off, cl_line, cl_line_len);
        off += cl_line_len;
    }

    if (off + 2 < total) {
        buffer[off++] = '\r';
        buffer[off++] = '\n';
    }

    if (!head_only) {
        if (chunked) {
            if (body_len > 0) {
                if (off + chk_hdr_len <= total) { 
                    memcpy(buffer + off, chk_hdr, chk_hdr_len); 
                    off += chk_hdr_len; 
                }
                if (off + body_len    <= total) { 
                    memcpy(buffer + off, body, body_len);       
                    off += body_len; 
                }
                if (off + chk_tail_len < total) { 
                    memcpy(buffer + off, chk_tail, chk_tail_len); 
                    off += chk_tail_len; 
                }
            }
            else if (off + 5 < total) {
                memcpy(buffer + off, "0\r\n\r\n", 5);
                off += 5;
            }
        }
        else if (body_len > 0 && off + body_len < total) {
            memcpy(buffer + off, body, body_len);
            off += body_len;
        }
    }

    buffer[(off < total) ? off : total - 1] = '\0';
    free(json_str);
    
    HTTP_LOG("serialize_response: wire size = %zu bytes (head_only=%d, chunked=%d)", off, head_only, chunked);
    if (out_len) {
        *out_len = off;
    }
    return buffer;
}


/**
 * @brief Serialize a response to a NUL-terminated HTTP/1.1 wire buffer.
 *
 * Back-compat wrapper over @ref http_serialize_response_ex with `head_only`
 * false; the body is included and a Content-Length header is auto-added (unless
 * the response already frames itself). Caller `free()`s the result.
 *
 * @param response Response to serialize.
 * @return Newly-allocated null-terminated string, or NULL on OOM / NULL input.
 */
char* http_serialize_response(HttpResponse* response) {
    return http_serialize_response_ex(response, false, NULL);
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

/* Defined further down; the worker needs the timeout-aware variant so a slow or
 * stalled client is bounded by a per-read idle timeout instead of pinning a
 * worker thread forever. */
static void hp_handle_request_to(TcpSocket client_socket, long timeout_ms);

/* One queued connection: the accepted socket plus the per-read idle timeout to
 * enforce on it. Allocated by the accept loop, freed by the worker. */
typedef struct {
    TcpSocket sock;
    long      timeout_ms;
} HttpConnJob;

/* Worker-thread entry point: serve exactly one connection to completion under a
 * per-read idle timeout (slow-loris defense), then close and free. Runs
 * concurrently with other connections on the thread pool. The route table is
 * read-only while serving, so dispatch is thread-safe as long as the user's
 * handlers are; per-request state lives entirely in local HttpRequest/Response. */
static int hp_serve_conn(void* arg) {
    HttpConnJob* job = (HttpConnJob*)arg;
    if (!job) {
        return 0;
    }
    hp_handle_request_to(job->sock, job->timeout_ms);
    tcp_close(job->sock);
    free(job);

    return 0;
}


/**
 * @brief Start the HTTP server on the given port, returning the bind status.
 *
 * Binds to `0.0.0.0:port` with `SO_REUSEADDR`, listens, and serves requests
 * concurrently: a thread pool (see http_server_set_worker_count) handles each
 * accepted connection on its own worker, so one slow client never blocks the
 * others. Every connection gets a recv/send timeout (see
 * http_server_set_client_timeout) so a stalled peer can't pin a worker.
 *
 * Blocks the calling thread, running the accept loop until http_stop_server is
 * called (from another thread or a signal handler). On a clean stop it drains
 * every in-flight connection, joins the workers, and returns TCP_SUCCESS.
 *
 * If port is 0 the OS picks a free port; query it with http_server_get_port.
 *
 * @param port TCP port to listen on (0 = ephemeral).
 * @return TCP_SUCCESS after a clean shutdown, or the failing TcpStatus if
 *         init/socket/bind/listen failed (e.g. TCP_ERR_BIND for a busy port).
 */
TcpStatus http_start_server_ex(int port) {
    TcpSocket      server_socket;
    TcpStatus      status;
    ThreadPool*    pool;
    int            workers;
    long           timeout_ms;
    unsigned short bound_port = (unsigned short)port;

    call_once(&srv_lock_once, hp_srv_lock_init);
#ifndef _WIN32
    call_once(&srv_sigpipe_once, hp_ignore_sigpipe);
#endif

    status = tcp_init();
    if (status != TCP_SUCCESS) {
        HTTP_LOG("failed to initialize network");
        return status;
    }

    status = tcp_socket_create(&server_socket);
    if (status != TCP_SUCCESS) {
        HTTP_LOG("failed to create server socket");
        tcp_cleanup();

        return status;
    }
    tcp_set_reuse_addr(server_socket, true);

    status = tcp_bind(server_socket, "0.0.0.0", (unsigned short)port);
    if (status != TCP_SUCCESS) {
        HTTP_LOG("failed to bind server socket on port %d", port);
        tcp_close(server_socket);
        tcp_cleanup();

        return status;
    }

    status = tcp_listen(server_socket, HTTP_LISTEN_BACKLOG);
    if (status != TCP_SUCCESS) {
        HTTP_LOG("failed to listen on server socket");
        tcp_close(server_socket);
        tcp_cleanup();

        return status;
    }

    /* Resolve the actual bound port (matters when port==0 => OS-assigned) so
     * http_server_get_port() and tests can discover it. */
    {
        char           host[64];
        unsigned short actual = 0;
        if (tcp_get_sock_name(server_socket, host, sizeof host, &actual) == TCP_SUCCESS && actual != 0) {
            bound_port = actual;
        }
    }

    /* Publish listening state and snapshot config under the lock. */
    mutex_lock(&srv_lock);
    srv_listen_sock = server_socket;
    srv_port        = bound_port;
    srv_listening   = true;
    server_running  = true;
    workers         = srv_worker_count;
    timeout_ms      = srv_client_timeout_ms;
    mutex_unlock(&srv_lock);

    if (workers <= 0) {
        unsigned long hw = thread_hardware_concurrency();
        workers = (hw > 0) ? (int)hw : HTTP_DEFAULT_WORKERS;
    }
    if (workers < 1) {
        workers = 1;
    }
    if (workers > HTTP_MAX_WORKERS) {
        workers = HTTP_MAX_WORKERS;
    }

    pool = thread_pool_create(workers);
    if (!pool) {
        HTTP_LOG("thread_pool_create(%d) failed", workers);
        mutex_lock(&srv_lock);

        srv_listening  = false;
        server_running = false;

        mutex_unlock(&srv_lock);
        tcp_close(server_socket);
        tcp_cleanup();

        return TCP_ERR_GENERIC;
    }

    HTTP_LOG("HTTP server listening on port %u (%d workers, %ld ms timeout)",
             (unsigned)bound_port, workers, timeout_ms);

    for (;;) {
        TcpSocket client_socket;
        TcpStatus a;
        bool      run;

        a = tcp_accept(server_socket, &client_socket);

        /* http_stop_server flips this flag and then wakes us with a throwaway
         * loopback connection, so we re-read it right after every accept(). */
        mutex_lock(&srv_lock);
        run = server_running;
        mutex_unlock(&srv_lock);

        if (!run) {
            if (a == TCP_SUCCESS) {
                tcp_close(client_socket);   /* the stop-wakeup connection */
            }
            break;
        }

        if (a == TCP_SUCCESS) {
            HttpConnJob* job = (HttpConnJob*)malloc(sizeof(*job));
            if (!job) {
                tcp_close(client_socket);   /* shed load instead of crashing */
                continue;
            }
            job->sock       = client_socket;
            job->timeout_ms = timeout_ms;

            if (!thread_pool_add_task_checked(pool, hp_serve_conn, job)) {
                /* Pool refused (OOM/shutting down): serve inline so the
                 * connection is still handled and the job never leaks. */
                hp_serve_conn(job);
            }
        } 
        else {
            /* Transient accept() error (EINTR / ECONNABORTED / EMFILE ...):
             * yield rather than hot-spin, then loop and re-check the flag. */
            thread_yield();
        }
    }

    /* Graceful shutdown: no new work is queued past the break above, so wait
     * drains every in-flight connection (each bounded by its timeout), then
     * destroy joins the now-idle workers. */
    thread_pool_wait(pool);
    thread_pool_destroy(pool);

    mutex_lock(&srv_lock);
    srv_listening = false;
    mutex_unlock(&srv_lock);

    tcp_close(server_socket);
    tcp_cleanup();
    HTTP_LOG("HTTP server stopped");
    return TCP_SUCCESS;
}


/**
 * @brief Back-compatible void wrapper around http_start_server_ex.
 * @param port TCP port to listen on.
 */
void http_start_server(int port) {
    (void)http_start_server_ex(port);
}


/**
 * @brief Stop the running server and wake its blocked accept().
 *
 * Flips the internal run flag and then opens a throwaway loopback connection so
 * the accept loop returns immediately, observes the flag, and begins a graceful
 * shutdown (drain in-flight connections, join workers). Safe to call from any
 * thread or a signal handler-driven path. A no-op if no server is listening.
 */
void http_stop_server(void) {
    unsigned short port;
    bool           was_listening;

    call_once(&srv_lock_once, hp_srv_lock_init);

    mutex_lock(&srv_lock);
    server_running = false;
    port = srv_port;
    was_listening  = srv_listening;
    mutex_unlock(&srv_lock);

    HTTP_LOG("stop_server: run=false (port=%u listening=%d)", (unsigned)port, (int)was_listening);

    if (was_listening) {
        TcpSocket waker;
        if (tcp_socket_create(&waker) == TCP_SUCCESS) {
            /* We don't care whether this connect/close round-trips cleanly;
             * its only purpose is to unblock the server's accept(). */
            tcp_connect_timeout(waker, "127.0.0.1", port, 1000);
            tcp_close(waker);
        }
    }
}


/**
 * @brief Set the worker-thread pool size used by the next http_start_server*.
 *        n <= 0 selects automatic sizing (hardware concurrency). Call before
 *        starting the server.
 *
 * @param n Desired worker-thread count, or <= 0 for automatic sizing.
 */
void http_server_set_worker_count(int n) {
    call_once(&srv_lock_once, hp_srv_lock_init);
    mutex_lock(&srv_lock);
    srv_worker_count = n;
    mutex_unlock(&srv_lock);
}


/**
 * @brief Set the per-connection recv/send timeout in milliseconds (slow-loris
 *        defense and shutdown-drain bound). timeout_ms <= 0 disables timeouts.
 *        Call before starting the server.
 *
 * @param timeout_ms Per-connection recv/send timeout in milliseconds, or <= 0
 *                   to disable timeouts.
 */
void http_server_set_client_timeout(long timeout_ms) {
    call_once(&srv_lock_once, hp_srv_lock_init);
    mutex_lock(&srv_lock);
    srv_client_timeout_ms = timeout_ms;
    mutex_unlock(&srv_lock);
}


/**
 * @brief Actual port the server is currently listening on (useful after binding
 *        to port 0), or 0 if no server is listening.
 *
 * @return The active listen port in host byte order, or 0 if no server is up.
 */
unsigned short http_server_get_port(void) {
    unsigned short p;
    call_once(&srv_lock_once, hp_srv_lock_init);
    mutex_lock(&srv_lock);
    p = srv_listening ? srv_port : 0;
    mutex_unlock(&srv_lock);
    return p;
}


/* Blocking recv gated by a per-read IDLE timeout. We wait up to timeout_ms for
 * the socket to become readable using select() (via tcp_wait_readable) and only
 * then call tcp_recv. This is the portable way to bound a read: SO_RCVTIMEO is
 * silently ignored by recv() on the non-overlapped Winsock sockets this library
 * creates, whereas select() is honored everywhere. An idle peer is therefore
 * cut after timeout_ms (slow-loris defense) while a peer that keeps sending data
 * is never penalized. timeout_ms <= 0 blocks indefinitely (original behavior).
 * Returns TCP_ERR_WOULD_BLOCK to flag a timeout, distinct from RECV/CLOSE. */
static TcpStatus hp_recv_to(TcpSocket socket, void* buf, size_t len,
                              size_t* got, long timeout_ms) {
    if (timeout_ms > 0) {
        TcpStatus w = tcp_wait_readable(socket, timeout_ms);
        if (w == TCP_ERR_WOULD_BLOCK) {
            return TCP_ERR_WOULD_BLOCK;          /* idle past the deadline */
        }
        if (w != TCP_SUCCESS) {
            return TCP_ERR_RECV;                 /* select error */
        }
    }
    return tcp_recv(socket, buf, len, got);
}

/* Send the whole buffer, but first (when timeout_ms > 0) make sure the peer is
 * actually draining: if it isn't writable within timeout_ms, give up rather than
 * let a client that stopped reading pin a worker thread. */
static TcpStatus hp_send_all_to(TcpSocket socket, const char* data, size_t len,
                                  long timeout_ms) {
    if (timeout_ms > 0) {
        TcpStatus w = tcp_wait_writable(socket, timeout_ms);
        if (w != TCP_SUCCESS) {
            return TCP_ERR_SEND;                 /* peer not draining */
        }
    }
    return tcp_send_all(socket, data, len);
}

/**
 * @brief Read one complete HTTP request off a connected socket (timeout-aware).
 *
 * See the header contract. Loops `tcp_recv` until the full header block has
 * arrived (capped at HTTP_MAX_HEADER_BYTES -> 431), then reads the body framed
 * by Content-Length (capped at HTTP_MAX_BODY_BYTES -> 413) or decodes a
 * Transfer-Encoding: chunked body into a contiguous buffer. Rejects both
 * framings at once (request smuggling -> 400) and unsupported transfer-codings
 * (-> 501). Each read is bounded by a per-read idle timeout (timeout_ms; 0 means
 * block forever) -> 408 on stall. Frees everything on every failure path.
 */
static bool hp_read_full_request_to(TcpSocket socket, long timeout_ms,
                                       char** out_data, size_t* out_len, int* out_status) {
    *out_data = NULL;
    *out_len  = 0;
    *out_status = 0;

    size_t cap = HTTP_READ_CHUNK;
    char*  buf = (char*)malloc(cap + 1);
    if (!buf) {
        *out_status = 500;
        return false;
    }
    size_t len = 0, scanned = 0, header_end = 0;

    /* Phase 1: read until the CRLFCRLF that ends the header block. */
    while (header_end == 0) {
        if (len == cap) {
            if (cap >= HTTP_MAX_HEADER_BYTES) { 
                free(buf); 
                *out_status = 431; 
                return false; 
            }

            size_t nc = cap * 2;
            if (nc > HTTP_MAX_HEADER_BYTES) { 
                nc = HTTP_MAX_HEADER_BYTES;
            }

            char* nb = (char*)realloc(buf, nc + 1);
            if (!nb) { 
                free(buf); 
                *out_status = 500;
                return false; 
            }

            buf = nb; cap = nc;
        }
        size_t got = 0;
        TcpStatus st = hp_recv_to(socket, buf + len, cap - len, &got, timeout_ms);
        if (st != TCP_SUCCESS || got == 0) {
            free(buf);
            /* idle timeout -> 408; clean close before any byte -> drop; otherwise
             * the headers were truncated -> 400. */
            *out_status = (st == TCP_ERR_WOULD_BLOCK) ? 408 : ((len == 0) ? 0 : 400);
            return false;
        }
        len += got;
        size_t s = (scanned > 3) ? scanned - 3 : 0;
        for (size_t i = s; i + 4 <= len; ++i) {
            if (buf[i] == '\r' && buf[i+1] == '\n' && buf[i+2] == '\r' && buf[i+3] == '\n') {
                header_end = i + 4;
                break;
            }
        }
        scanned = len;
    }
    buf[len] = '\0';

    /* Decide framing from the raw header block. */
    char te[80], clbuf[32];
    bool chunked = false;
    if (raw_header_value(buf, header_end, "Transfer-Encoding", te, sizeof te)) {
        chunked = ci_contains_chunked(te);
        if (!chunked) { 
            free(buf); 
            *out_status = 501; 
            return false; 
        }  /* unsupported coding */
    }
    long cl = -1;
    if (raw_header_value(buf, header_end, "Content-Length", clbuf, sizeof clbuf)) {
        cl = hp_parse_clen(clbuf);
        if (cl < 0) { 
            free(buf); 
            *out_status = 400; 
            return false; 
        }
    }
    if (chunked && cl >= 0) { 
        free(buf); 
        *out_status = 400; 
        return false; 
    }  /* smuggling: both framings */

    /* Phase 2: read the body. */
    if (chunked) {
        CkdState ck = { CKD_SIZE, 0, 0, false };
        HttpBuf  body = { NULL, 0, 0 };
        if (len > header_end && !ckd_feed(&ck, buf + header_end, len - header_end, &body, HTTP_MAX_BODY_BYTES)) {
            free(buf); 
            free(body.data);
            *out_status = (body.len >= HTTP_MAX_BODY_BYTES) ? 413 : 400;
            return false;
        }
        char tmp[HTTP_READ_CHUNK];
        while (ck.phase != CKD_DONE) {
            size_t got = 0;
            TcpStatus st = hp_recv_to(socket, tmp, sizeof tmp, &got, timeout_ms);
            if (st != TCP_SUCCESS || got == 0) {
                free(buf); 
                free(body.data);
                *out_status = (st == TCP_ERR_WOULD_BLOCK) ? 408 : 400;
                return false;
            }
            if (!ckd_feed(&ck, tmp, got, &body, HTTP_MAX_BODY_BYTES)) {
                free(buf); 
                free(body.data);
                *out_status = (body.len >= HTTP_MAX_BODY_BYTES) ? 413 : 400;
                return false;
            }
        }

        size_t final_len = header_end + body.len;
        char*  final = (char*)malloc(final_len + 1);
        if (!final) { 
            free(buf); 
            free(body.data); 
            *out_status = 500; 
            return false; 
        }

        memcpy(final, buf, header_end);
        if (body.len) { 
            memcpy(final + header_end, body.data, body.len); 
        }
        final[final_len] = '\0';
        free(buf); free(body.data);
        *out_data = final; *out_len = final_len;

        return true;
    }

    if (cl >= 0) {
        if ((size_t)cl > HTTP_MAX_BODY_BYTES) { 
            free(buf); 
            *out_status = 413; 
            return false; 
        }

        size_t need = header_end + (size_t)cl;
        if (need > cap) {
            char* nb = (char*)realloc(buf, need + 1);
            if (!nb) { 
                free(buf); 
                *out_status = 500; 
                return false; 
            }
            buf = nb; cap = need;
        }
        while (len < need) {
            size_t got = 0;
            TcpStatus st = hp_recv_to(socket, buf + len, cap - len, &got, timeout_ms);

            if (st != TCP_SUCCESS || got == 0) {
                free(buf);
                *out_status = (st == TCP_ERR_WOULD_BLOCK) ? 408 : 400;   /* timeout vs truncated body */
                return false;
            }
            len += got;
        }
        buf[need] = '\0';
        *out_data = buf; *out_len = need;   /* extra (pipelined) bytes are ignored */
        return true;
    }

    /* No body framing -> request has no body (extra bytes, if any, ignored). */
    buf[header_end] = '\0';
    *out_data = buf; *out_len = header_end;
    return true;
}


/**
 * @brief Read a complete HTTP request from a connected socket (blocking).
 *
 * Public, back-compatible entry point that blocks indefinitely (no idle
 * timeout); the server path uses an internal timeout-aware variant. Loops until
 * the headers plus any Content-Length / chunked body have arrived. The returned
 * buffer is heap-allocated and must be `free()`d by the caller.
 *
 * @param socket     A connected TCP socket.
 * @param out_data   Output: receives a malloc'd buffer holding the raw request bytes.
 * @param out_len    Output: number of bytes in `*out_data`.
 * @param out_status Output: on failure, the HTTP status code to return to the
 *                   client (e.g. 400/408/413); 0 if the peer closed before
 *                   sending anything.
 * @return `true` on success; `false` on read error, timeout, or malformed framing.
 */
bool http_read_full_request(TcpSocket socket, char** out_data, size_t* out_len, int* out_status) {
    return hp_read_full_request_to(socket, 0, out_data, out_len, out_status);
}


/**
 * @brief Process a single request from an already-accepted client socket.
 *
 * Reads the FULL request (looped read of headers + Content-Length / chunked
 * body), dispatches to the route handler (404 / 405 / handler), serializes the
 * reply with a correct Content-Length (body suppressed for HEAD), and writes it
 * with `tcp_send_all`. The caller is responsible for closing `client_socket`.
 * Every exit path frees the request, response, and read buffer — no leaks.
 *
 * @param client_socket A connected TCP socket.
 */
static void hp_handle_request_to(TcpSocket client_socket, long timeout_ms) {
    HTTP_LOG("handle_request: entry on socket");

    char*  raw = NULL;
    size_t raw_len = 0;
    int    herr = 0;
    bool   ok = hp_read_full_request_to(client_socket, timeout_ms, &raw, &raw_len, &herr);

    HttpResponse res;
    memset(&res, 0, sizeof(res));
    HttpRequest* req = NULL;
    bool head_only = false;

    if (!ok) {
        if (herr == 0) {                 /* peer closed without sending a request */
            HTTP_LOG("handle_request: no request (peer closed) — nothing to do");
            free(raw);
            return;
        }
        HTTP_LOG("handle_request: read failed -> %d", herr);
        http_send_error(&res, herr, http_status_text(herr));
    }
    else {
        req = http_parse_request_ex(raw, raw_len);
        if (!req) {
            HTTP_LOG("handle_request: parse failed -> 400 Bad Request");
            http_send_error(&res, 400, "Bad Request: Invalid HTTP Request");
        }
        else {
            head_only = (req->method == HTTP_HEAD);
            res.status_code = 200;
            res.status_message = string_strdup("OK");
            handle_request(req, &res);
        }
    }

    size_t wire_len = 0;
    char*  wire = http_serialize_response_ex(&res, head_only, &wire_len);
    if (wire) {
        TcpStatus send_status = hp_send_all_to(client_socket, wire, wire_len, timeout_ms);
        (void)send_status;
        HTTP_LOG("handle_request: send wrote %zu bytes (status=%d)", wire_len, (int)send_status);
        free(wire);
    }
    else {
        HTTP_LOG("handle_request: serialize returned NULL — nothing sent");
    }

    free(raw);
    if (req) {
        http_free_request(req);
    }
    http_free_response(&res);
    HTTP_LOG("handle_request: complete");
}


/**
 * @brief Read, dispatch, and answer a single request on an accepted socket.
 *
 * Public entry point that blocks indefinitely on the client (no idle timeout);
 * the server path uses an internal timeout-aware variant. Reads the full
 * request, routes it (404 / 405 / handler), serializes the reply with a correct
 * Content-Length (body suppressed for HEAD), and sends it. The caller remains
 * responsible for closing `client_socket`. Frees every per-request allocation.
 *
 * @param client_socket A connected, already-accepted TCP client socket.
 */
void http_handle_request(TcpSocket client_socket) {
    hp_handle_request_to(client_socket, 0);
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
static int hp_hex_value(int c) {
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
            int hi = hp_hex_value((unsigned char)in[i + 1]);
            int lo = hp_hex_value((unsigned char)in[i + 2]);

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
static bool hp_ascii_ieq(const char* a, const char* b) {
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
        if (hp_ascii_ieq(req->headers[i].name, name)) {
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


/* ================================================================== */
/* HTTP CLIENT                                                        */
/* ================================================================== */
/* Client helpers use a distinct `cl_` prefix and reuse the framing    */
/* helpers defined above (ckd_feed / HttpBuf / raw_header_value /       */
/* hp_parse_clen / hp_recv_to).                                    */

/* malloc a NUL-terminated copy of s[0..n). */
static char* cl_dupn(const char* s, size_t n) {
    char* p = (char*)malloc(n + 1);
    if (!p) {
        return NULL;
    }
    if (n) {
        memcpy(p, s, n);
    }
    p[n] = '\0';
    return p;
}

/* Parse a 1..65535 port from s[0..n). false on empty / non-digit / out-of-range. */
static bool cl_parse_port(const char* s, size_t n, unsigned short* out) {
    if (n == 0 || n > 5) {
        return false;
    }
    unsigned long v = 0;
    for (size_t i = 0; i < n; ++i) {
        if (s[i] < '0' || s[i] > '9') {
            return false;
        }
        v = v * 10u + (unsigned long)(s[i] - '0');
    }
    if (v == 0 || v > 65535u) {
        return false;
    }
    *out = (unsigned short)v;
    return true;
}

/**
 * @brief Parse scheme://[userinfo@]host[:port][/path][?query][#fragment].
 *        See the header contract. On any error frees partial state and zeroes
 *        *out. Only http/https schemes are accepted.
 *
 * @param url Null-terminated absolute URL to parse. Must not be NULL.
 * @param out Output: receives the parsed components (owns its strings; release
 *            with http_url_free). Must not be NULL.
 * @return `true` on success; `false` on a NULL argument, an unsupported scheme,
 *         or malformed input (with `*out` zeroed).
 */
bool http_url_parse(const char* url, HttpUrl* out) {
    if (!out) {
        return false;
    }
    memset(out, 0, sizeof(*out));
    if (!url) {
        return false;
    }

    const char* sep = strstr(url, "://");
    if (!sep || sep == url) {
        return false;
    }

    out->scheme = cl_dupn(url, (size_t)(sep - url));
    if (!out->scheme) {
        return false;
    }

    for (char* c = out->scheme; *c; ++c) {
        *c = (char)tolower((unsigned char)*c);
    }

    if (strcmp(out->scheme, "http") == 0) {
        out->is_https = false;
    } 
    else if (strcmp(out->scheme, "https") == 0) {
        out->is_https = true;
    } 
    else {
        http_url_free(out);
        return false;
    }

    const char* authority = sep + 3;
    size_t      alen      = strcspn(authority, "/?#");
    const char* rest      = authority + alen;   /* path/query/fragment */

    /* optional userinfo before the last '@' inside the authority */
    const char* hostport = authority;
    size_t      hp_len   = alen;
    const char* at       = NULL;
    for (const char* q = authority; q < authority + alen; ++q) {
        if (*q == '@') {
            at = q;
        }
    }
    if (at) {
        out->userinfo = cl_dupn(authority, (size_t)(at - authority));
        if (!out->userinfo) { 
            http_url_free(out); 
            return false; 
        }
        hostport = at + 1;
        hp_len   = (size_t)((authority + alen) - (at + 1));
    }

    /* host[:port], with [IPv6] literal support */
    bool have_port = false;
    unsigned short port = 0;
    if (hp_len > 0 && hostport[0] == '[') {
        const char* close = (const char*)memchr(hostport, ']', hp_len);
        if (!close) { 
            http_url_free(out); 
            return false; 
        }

        out->host = cl_dupn(hostport + 1, (size_t)(close - (hostport + 1)));
        const char* after = close + 1;
        size_t after_len = (size_t)((hostport + hp_len) - after);

        if (after_len >= 1 && after[0] == ':') {
            if (!cl_parse_port(after + 1, after_len - 1, &port)) { 
                http_url_free(out); 
                return false; 
            }
            have_port = true;
        } 
        else if (after_len != 0) {
            http_url_free(out); 
            return false;     /* junk after ']' */
        }
    } 
    else {
        const char* colon = NULL;
        for (const char* q = hostport; q < hostport + hp_len; ++q) {
            if (*q == ':') {
                colon = q;
            }
        }
        if (colon) {
            out->host = cl_dupn(hostport, (size_t)(colon - hostport));
            if (!cl_parse_port(colon + 1, (size_t)((hostport + hp_len) - (colon + 1)), &port)) {
                http_url_free(out); 
                return false;
            }
            have_port = true;
        } else {
            out->host = cl_dupn(hostport, hp_len);
        }
    }
    if (!out->host || out->host[0] == '\0') { 
        http_url_free(out); 
        return false; 
    }

    /* path / query / fragment */
    {
        size_t rest_len = strlen(rest);
        const char* frag = (const char*)memchr(rest, '#', rest_len);
        size_t pq_len = frag ? (size_t)(frag - rest) : rest_len;
        if (frag) {
            out->fragment = cl_dupn(frag + 1, rest_len - (pq_len + 1));
            if (!out->fragment) { 
                http_url_free(out); 
                return false; 
            }
        }
        const char* qm = (const char*)memchr(rest, '?', pq_len);
        size_t path_len = qm ? (size_t)(qm - rest) : pq_len;

        if (qm) {
            out->query = cl_dupn(qm + 1, pq_len - (path_len + 1));
            if (!out->query) { 
                http_url_free(out); 
                return false; 
            }
        }
        out->path = (path_len == 0) ? string_strdup("/") : cl_dupn(rest, path_len);
        if (!out->path) { 
            http_url_free(out); 
            return false; 
        }
    }

    out->port = have_port ? port : (out->is_https ? 443u : 80u);
    return true;
}


/**
 * @brief Free all strings owned by an HttpUrl and zero the struct.
 *
 * Releases scheme/userinfo/host/path/query/fragment, then memsets `*u` so it can
 * be safely reused or re-freed. Does NOT free the HttpUrl struct itself. NULL is
 * a safe no-op.
 *
 * @param u The URL whose owned strings should be released. May be NULL.
 */
void http_url_free(HttpUrl* u) {
    if (!u) {
        return;
    }
    free(u->scheme);
    free(u->userinfo);
    free(u->host);
    free(u->path);
    free(u->query);
    free(u->fragment);
    memset(u, 0, sizeof(*u));
}


/**
 * @brief Parse a full HTTP response (status line + headers + de-framed body)
 *        into a heap HttpResponse. See the header contract.
 *
 * @param data   Raw response bytes (status line, headers, framed body).
 * @param length Number of bytes in `data`; enables binary-safe body handling.
 * @return Newly-allocated response (release with http_response_free), or NULL on
 *         a NULL/empty buffer or malformed input.
 */
HttpResponse* http_parse_response(const char* data, size_t length) {
    if (!data || length == 0) {
        return NULL;
    }

    /* status line ends at the first LF */
    const char* lf = (const char*)memchr(data, '\n', length);
    if (!lf) {
        return NULL;
    }

    size_t line_len = (size_t)(lf - data);
    if (line_len && data[line_len - 1] == '\r') {
        line_len--;
    }
    if (line_len < 5 || memcmp(data, "HTTP/", 5) != 0) {
        return NULL;                              /* not a status line */
    }

    const char* sp1 = (const char*)memchr(data, ' ', line_len);
    if (!sp1) {
        return NULL;
    }

    const char* code_beg = sp1 + 1;
    while (code_beg < data + line_len && *code_beg == ' ') {
        code_beg++;
    }

    int         code = 0;
    int         ndig = 0;
    const char* c    = code_beg;

    while (c < data + line_len && *c >= '0' && *c <= '9') {
        code = code * 10 + (*c - '0');
        ndig++;
        c++;
    }

    if (ndig == 0) {
        return NULL;
    }

    const char* msg_beg = c;
    while (msg_beg < data + line_len && *msg_beg == ' ') {
        msg_beg++;
    }

    size_t msg_len = (size_t)((data + line_len) - msg_beg);
    HttpResponse* r = (HttpResponse*)calloc(1, sizeof(HttpResponse));

    if (!r) {
        return NULL;
    }

    r->status_code    = code;
    r->status_message = cl_dupn(msg_beg, msg_len);
    if (!r->status_message) {
        free(r);
        return NULL;
    }

    /* headers until the blank line */
    const char* hp  = lf + 1;
    const char* end = data + length;
    while (hp < end) {
        const char* nl       = (const char*)memchr(hp, '\n', (size_t)(end - hp));
        const char* line_end = nl ? nl : end;
        size_t      llen     = (size_t)(line_end - hp);
        if (llen && hp[llen - 1] == '\r') {
            llen--;
        }
        if (llen == 0) {
            hp = nl ? nl + 1 : end;               /* blank line -> body follows */
            break;
        }

        const char* colon = (const char*)memchr(hp, ':', llen);
        if (colon && r->header_count < MAX_HEADERS) {
            size_t nlen = (size_t)(colon - hp);
            const char* vbeg = colon + 1;
            size_t vlen = llen - (nlen + 1);

            while (vlen && (*vbeg == ' ' || *vbeg == '\t')) { 
                vbeg++; 
                vlen--; 
            }
            while (vlen && (vbeg[vlen - 1] == ' ' || vbeg[vlen - 1] == '\t')) { 
                vlen--; 
            }
            while (nlen && (hp[nlen - 1] == ' ' || hp[nlen - 1] == '\t')) { 
                nlen--; 
            }

            char* nm = cl_dupn(hp, nlen);
            char* vl = cl_dupn(vbeg, vlen);

            if (nm && vl) {
                r->headers[r->header_count].name  = nm;
                r->headers[r->header_count].value = vl;
                r->header_count++;
            } 
            else {
                free(nm);
                free(vl);
            }
        }
        if (!nl) {
            hp = end;
            break;
        }
        hp = nl + 1;
    }

    /* body = everything after the blank line (already de-framed by the caller) */
    size_t body_len = (size_t)(end - hp);
    r->body = (char*)malloc(body_len + 1);
    if (!r->body) {
        http_response_free(r);
        return NULL;
    }
    if (body_len) {
        memcpy(r->body, hp, body_len);
    }
    r->body[body_len] = '\0';
    r->body_length = body_len;
    return r;
}


/**
 * @brief Free a heap-allocated HttpResponse, contents and struct.
 *
 * Releases everything owned by the response (via http_free_response) and then
 * frees the struct itself. Use this for responses returned by the client API
 * (http_request / http_get / ...) and http_parse_response. NULL is a safe no-op.
 *
 * @param response Heap-allocated response to destroy. May be NULL.
 */
void http_response_free(HttpResponse* response) {
    if (!response) {
        return;
    }
    http_free_response(response);   /* release contents */
    free(response);                 /* release the struct */
}


/* Case-insensitive ASCII equality (avoids non-portable strcasecmp). */
static bool cl_ci_eq(const char* a, const char* b) {
    for (; *a && *b; ++a, ++b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
            return false;
        }
    }
    return *a == '\0' && *b == '\0';
}

/* Append a C string to a growable buffer (no implicit NUL). */
static bool cl_append_str(HttpBuf* b, const char* s) {
    return httpbuf_append(b, s, strlen(s), SIZE_MAX);
}

/* Close a client socket, tearing down TLS first when present. tcp_ssl_close
 * already closes the fd, so we must not also call tcp_close on an SSL socket. */
static void cl_close(TcpSocket sock) {
    if (tcp_get_ssl(sock) != NULL) {
        tcp_ssl_close(sock);
    } else {
        tcp_close(sock);
    }
}

/* One recv, gated by a per-read idle timeout (select), SSL-aware. Returns
 * TCP_ERR_WOULD_BLOCK on idle timeout. */
static TcpStatus cl_recv(TcpSocket s, void* buf, size_t len, size_t* got, long timeout_ms) {
    if (timeout_ms > 0) {
        TcpStatus w = tcp_wait_readable(s, timeout_ms);
        if (w == TCP_ERR_WOULD_BLOCK) {
            return TCP_ERR_WOULD_BLOCK;
        }
        if (w != TCP_SUCCESS) {
            return TCP_ERR_RECV;
        }
    }
    if (tcp_get_ssl(s) != NULL) {
        return tcp_ssl_recv(s, buf, len, got);
    }
    return tcp_recv(s, buf, len, got);
}

/* Send the whole buffer, SSL-aware, bounded by a writable-timeout per chunk. */
static TcpStatus cl_send_all(TcpSocket s, const void* buf, size_t len, long timeout_ms) {
    if (tcp_get_ssl(s) != NULL) {
        const char* p   = (const char*)buf;
        size_t      rem = len;

        while (rem > 0) {
            if (timeout_ms > 0 && tcp_wait_writable(s, timeout_ms) != TCP_SUCCESS) {
                return TCP_ERR_SEND;
            }

            size_t    sent = 0;
            TcpStatus st   = tcp_ssl_send(s, p, rem, &sent);
            if (st != TCP_SUCCESS) {
                return st;
            }
            if (sent == 0) {
                return TCP_ERR_SEND;
            }
            p   += sent;
            rem -= sent;
        }
        return TCP_SUCCESS;
    }
    if (timeout_ms > 0 && tcp_wait_writable(s, timeout_ms) != TCP_SUCCESS) {
        return TCP_ERR_SEND;
    }
    return tcp_send_all(s, buf, len);
}

/* Establish client TLS on an already-connected socket (SNI = host). When
 * !insecure, require the peer certificate chain to verify. */
static TcpStatus cl_tls_client_handshake(TcpSocket sock, const char* host, bool insecure) {
    if (tcp_ssl_init_client() != TCP_SUCCESS) {
        return TCP_ERR_SSL;
    }

    TcpStatus st = tcp_ssl_connect(sock, host);
    if (st != TCP_SUCCESS) {
        return st;
    }
    if (!insecure) {
        long vr = tcp_ssl_get_verify_result(sock);
        if (vr != 0) {                       /* X509_V_OK == 0 */
            HTTP_LOG("client: TLS certificate verification failed (X509 code %ld) for %s", vr, host);
            return TCP_ERR_SSL;
        }
    }
    return TCP_SUCCESS;
}

/* Serialize the request line + headers + body into `b`. Host and
 * Content-Length are generated; any user-supplied Host/Content-Length/
 * Connection headers are skipped so we stay authoritative. */
static bool cl_build_request(HttpBuf* b, HttpMethod method, const HttpUrl* u,
                             const HttpHeader* headers, size_t header_count,
                             const void* body, size_t body_len) {
    const char* m = http_method_to_string(method);
    if (!m) {
        return false;
    }
    bool ipv6 = (strchr(u->host, ':') != NULL);
    unsigned short defport = u->is_https ? 443u : 80u;

    if (!cl_append_str(b, m) || !cl_append_str(b, " ")) {
        return false;
    }
    if (!cl_append_str(b, (u->path && u->path[0]) ? u->path : "/")) {
        return false;
    }
    if (u->query && u->query[0]) {
        if (!cl_append_str(b, "?") || !cl_append_str(b, u->query)) {
            return false;
        }
    }
    if (!cl_append_str(b, " HTTP/1.1\r\nHost: ")) {
        return false;
    }
    if (ipv6 && !cl_append_str(b, "[")) {
        return false;
    }
    if (!cl_append_str(b, u->host)) {
        return false;
    }
    if (ipv6 && !cl_append_str(b, "]")) {
        return false;
    }
    if (u->port != defport) {
        char ps[16];
        snprintf(ps, sizeof ps, ":%u", (unsigned)u->port);
        if (!cl_append_str(b, ps)) {
            return false;
        }
    }
    if (!cl_append_str(b, "\r\n")) {
        return false;
    }

    for (size_t i = 0; i < header_count; ++i) {
        if (!headers[i].name || !headers[i].value) {
            continue;
        }
        if (cl_ci_eq(headers[i].name, "Host") ||
            cl_ci_eq(headers[i].name, "Content-Length") ||
            cl_ci_eq(headers[i].name, "Connection")) {
            continue;
        }
        if (!cl_append_str(b, headers[i].name) || !cl_append_str(b, ": ") ||
            !cl_append_str(b, headers[i].value) || !cl_append_str(b, "\r\n")) {
            return false;
        }
    }

    if (body_len > 0 || method == HTTP_POST || method == HTTP_PUT || method == HTTP_PATCH) {
        char cl[40];
        snprintf(cl, sizeof cl, "Content-Length: %zu\r\n", body_len);
        if (!cl_append_str(b, cl)) {
            return false;
        }
    }
    if (!cl_append_str(b, "Connection: close\r\n\r\n")) {   /* no keep-alive yet */
        return false;
    }
    if (body_len > 0 && body) {
        if (!httpbuf_append(b, (const char*)body, body_len, SIZE_MAX)) {
            return false;
        }
    }
    return true;
}

/* Read a complete response off the socket into a contiguous "headers\r\n\r\n +
 * de-framed body" buffer. Body framing: no body for HEAD / 1xx / 204 / 304;
 * else Transfer-Encoding: chunked, else Content-Length, else read-to-EOF
 * (Connection: close). Bounded by HTTP_MAX_HEADER_BYTES / HTTP_MAX_BODY_BYTES. */
static bool cl_read_response(TcpSocket s, long timeout_ms, HttpMethod method,
                             char** out_data, size_t* out_len) {
    *out_data = NULL;
    *out_len  = 0;

    size_t cap = HTTP_READ_CHUNK;
    char*  buf = (char*)malloc(cap + 1);
    if (!buf) {
        return false;
    }
    size_t len = 0, scanned = 0, header_end = 0;

    /* Phase 1: status line + headers, up to CRLFCRLF. */
    while (header_end == 0) {
        if (len == cap) {
            if (cap >= HTTP_MAX_HEADER_BYTES) { 
                free(buf); 
                return false; 
            }

            size_t nc = cap * 2;
            if (nc > HTTP_MAX_HEADER_BYTES) { 
                nc = HTTP_MAX_HEADER_BYTES; 
            }

            char* nb = (char*)realloc(buf, nc + 1);
            if (!nb) { 
                free(buf); 
                return false; 
            }
            buf = nb; cap = nc;
        }

        size_t    got = 0;
        TcpStatus st  = cl_recv(s, buf + len, cap - len, &got, timeout_ms);

        if (st != TCP_SUCCESS || got == 0) { 
            free(buf); 
            return false; 
        }

        len += got;
        size_t start = (scanned > 3) ? scanned - 3 : 0;
        for (size_t i = start; i + 4 <= len; ++i) {
            if (buf[i] == '\r' && buf[i+1] == '\n' && buf[i+2] == '\r' && buf[i+3] == '\n') {
                header_end = i + 4;
                break;
            }
        }
        scanned = len;
    }
    buf[len] = '\0';

    /* status code (for no-body rules) */
    int code = 0;
    {
        const char* sp = (const char*)memchr(buf, ' ', header_end);
        if (sp) {
            const char* c = sp + 1;
            while (c < buf + header_end && *c == ' ') { 
                c++; 
            }
            while (c < buf + header_end && *c >= '0' && *c <= '9') { 
                code = code * 10 + (*c - '0'); 
                c++; 
            }
        }
    }
    bool no_body = (method == HTTP_HEAD) || (code / 100 == 1) || code == 204 || code == 304;

    char te[80], clbuf[32];
    bool chunked = false;

    if (!no_body && raw_header_value(buf, header_end, "Transfer-Encoding", te, sizeof te)) {
        chunked = ci_contains_chunked(te);
    }

    long cl = -1;
    if (!no_body && !chunked && raw_header_value(buf, header_end, "Content-Length", clbuf, sizeof clbuf)) {
        cl = hp_parse_clen(clbuf);
    }

    if (no_body) {
        char* fin = (char*)malloc(header_end + 1);
        if (!fin) { 
            free(buf); 
            return false; 
        }

        memcpy(fin, buf, header_end);
        fin[header_end] = '\0';
        free(buf);
        *out_data = fin; *out_len = header_end;

        return true;
    }

    if (chunked) {
        CkdState ck   = { CKD_SIZE, 0, 0, false };
        HttpBuf  body = { NULL, 0, 0 };

        if (len > header_end && !ckd_feed(&ck, buf + header_end, len - header_end, &body, HTTP_MAX_BODY_BYTES)) {
            free(buf); 
            free(body.data); 
            return false;
        }

        char tmp[HTTP_READ_CHUNK];
        while (ck.phase != CKD_DONE) {
            size_t    got = 0;
            TcpStatus st  = cl_recv(s, tmp, sizeof tmp, &got, timeout_ms);

            if (st != TCP_SUCCESS || got == 0) { 
                free(buf); 
                free(body.data); 
                return false; 
            }
            if (!ckd_feed(&ck, tmp, got, &body, HTTP_MAX_BODY_BYTES)) { 
                free(buf); 
                free(body.data); 
                return false; 
            }
        }

        size_t fin_len = header_end + body.len;
        char*  fin     = (char*)malloc(fin_len + 1);
        if (!fin) { 
            free(buf); 
            free(body.data); 
            return false; 
        }

        memcpy(fin, buf, header_end);
        if (body.len) { 
            memcpy(fin + header_end, body.data, body.len); 
        }
        fin[fin_len] = '\0';

        free(buf); 
        free(body.data);
        *out_data = fin; *out_len = fin_len;

        return true;
    }

    if (cl >= 0) {
        if ((size_t)cl > HTTP_MAX_BODY_BYTES) { 
            free(buf); 
            return false; 
        }
        size_t need = header_end + (size_t)cl;

        if (need > cap) {
            char* nb = (char*)realloc(buf, need + 1);
            if (!nb) { 
                free(buf); 
                return false; 
            }
            buf = nb; cap = need;
        }

        while (len < need) {
            size_t    got = 0;
            TcpStatus st  = cl_recv(s, buf + len, cap - len, &got, timeout_ms);

            if (st != TCP_SUCCESS || got == 0) { 
                free(buf); 
                return false; 
            }

            len += got;
        }
        buf[need] = '\0';
        *out_data = buf; *out_len = need;

        return true;
    }

    /* No framing -> read until the server closes the connection. */
    for (;;) {
        if (len == cap) {
            size_t hard = header_end + HTTP_MAX_BODY_BYTES;
            if (cap >= hard) { 
                free(buf); 
                return false; 
            }

            size_t nc = cap * 2;
            if (nc > hard) { 
                nc = hard; 
            }

            char* nb = (char*)realloc(buf, nc + 1);
            if (!nb) { 
                free(buf); 
                return false; 
            }
            buf = nb; cap = nc;
        }

        size_t    got = 0;
        TcpStatus st  = cl_recv(s, buf + len, cap - len, &got, timeout_ms);
        if (st != TCP_SUCCESS || got == 0) {
            break;                              /* EOF / error terminates the body */
        }
        len += got;
    }
    buf[len] = '\0';
    *out_data = buf; *out_len = len;
    
    return true;
}


/* Build the absolute URL a Location header redirects to, relative to `base`.
 * Handles absolute (scheme://...), root-relative (/path) and relative paths.
 * Returns a malloc'd NUL-terminated URL, or NULL on OOM. */
static char* cl_resolve_redirect(const HttpUrl* base, const char* loc) {
    if (strstr(loc, "://")) {
        return string_strdup(loc);              /* already absolute */
    }

    HttpBuf b = { NULL, 0, 0 };
    bool ok = true;
    bool ipv6 = (strchr(base->host, ':') != NULL);
    unsigned short defport = base->is_https ? 443u : 80u;

    ok = ok && cl_append_str(&b, base->scheme) && cl_append_str(&b, "://");
    
    if (ipv6) { 
        ok = ok && cl_append_str(&b, "[");
    }
    ok = ok && cl_append_str(&b, base->host);
    
    if (ipv6) { 
        ok = ok && cl_append_str(&b, "]"); 
    }
    if (base->port != defport) {
        char ps[16];
        snprintf(ps, sizeof ps, ":%u", (unsigned)base->port);
        ok = ok && cl_append_str(&b, ps);
    }

    if (loc[0] == '/') {
        ok = ok && cl_append_str(&b, loc);
    } 
    else {
        const char* path  = (base->path && base->path[0]) ? base->path : "/";
        const char* slash = strrchr(path, '/');
        size_t dirlen = slash ? (size_t)(slash - path + 1) : 0;

        if (dirlen == 0) {
            ok = ok && cl_append_str(&b, "/");
        } 
        else {
            ok = ok && httpbuf_append(&b, path, dirlen, SIZE_MAX);
        }
        ok = ok && cl_append_str(&b, loc);
    }

    ok = ok && httpbuf_append(&b, "", 1, SIZE_MAX);   /* NUL-terminate */
    if (!ok) {
        free(b.data);
        return NULL;
    }
    /* append of "" with n=1 copies one NUL byte from the literal */
    return b.data;
}

/**
 * @brief Perform one HTTP/HTTPS request, following redirects. See the header
 *        contract. Returns the final HttpResponse (free with
 *        http_response_free) or NULL on a transport/DNS/TLS error.
 *
 * @param method       Request method (HTTP_GET, HTTP_POST, ...).
 * @param url          Absolute http/https URL. Must not be NULL.
 * @param headers      Optional array of request headers (may be NULL).
 * @param header_count Number of entries in `headers`.
 * @param body         Optional request body (may be NULL).
 * @param body_len     Length of `body` in bytes.
 * @param opts         Optional client options (timeout, redirect policy, TLS
 *                     verification); NULL selects the defaults.
 * @return Newly-allocated final response (free with http_response_free), or NULL
 *         on a transport / DNS / TLS error or invalid arguments.
 */
HttpResponse* http_request(HttpMethod method, const char* url,
                           const HttpHeader* headers, size_t header_count,
                           const void* body, size_t body_len,
                           const HttpClientOptions* opts) {
    if (!url) {
        return NULL;
    }

    long timeout_ms = (opts && opts->timeout_ms > 0) ? opts->timeout_ms : HTTP_DEFAULT_TIMEOUT_MS;
    int  max_redirects;

    if (!opts) {
        max_redirects = 5;
    } 
    else if (opts->follow_redirects_set) {
        max_redirects = opts->max_redirects;                       /* honor 0 */
    } 
    else {
        max_redirects = (opts->max_redirects > 0) ? opts->max_redirects : 5;
    }
    bool insecure = (opts && opts->tls_insecure);

    if (tcp_init() != TCP_SUCCESS) {
        return NULL;
    }

    char* cur_url = string_strdup(url);
    HttpMethod cur_method = method;
    const void* cur_body = body;
    size_t cur_body_len = body_len;
    HttpResponse* result = NULL;

    if (cur_url) {
        for (int hop = 0; ; ++hop) {
            HttpUrl u;
            if (!http_url_parse(cur_url, &u)) {
                break;
            }

            TcpSocket sock;
            if (tcp_socket_create(&sock) != TCP_SUCCESS) { 
                http_url_free(&u); 
                break; 
            }
            if (tcp_connect_timeout(sock, u.host, u.port, timeout_ms) != TCP_SUCCESS) {
                tcp_close(sock); 
                http_url_free(&u); 
                break;
            }
            if (u.is_https && cl_tls_client_handshake(sock, u.host, insecure) != TCP_SUCCESS) {
                cl_close(sock); 
                http_url_free(&u); 
                break;
            }

            HttpBuf req = { NULL, 0, 0 };
            if (!cl_build_request(&req, cur_method, &u, headers, header_count, cur_body, cur_body_len)) {
                free(req.data); 
                cl_close(sock); 
                http_url_free(&u); 
                break;
            }

            TcpStatus ss = cl_send_all(sock, req.data, req.len, timeout_ms);
            free(req.data);
            if (ss != TCP_SUCCESS) { 
                cl_close(sock); 
                http_url_free(&u); 
                break; 
            }

            char* raw = NULL;
            size_t raw_len = 0;
            bool ok = cl_read_response(sock, timeout_ms, cur_method, &raw, &raw_len);
            cl_close(sock);

            if (!ok) { 
                free(raw); 
                http_url_free(&u); 
                break; 
            }

            HttpResponse* resp = http_parse_response(raw, raw_len);
            free(raw);
            if (!resp) { 
                http_url_free(&u); 
                break; 
            }

            int code = resp->status_code;
            bool redirable = (code == 301 || code == 302 || code == 303 || code == 307 || code == 308);
            const char* loc = redirable ? http_response_get_header(resp, "Location") : NULL;

            if (loc && loc[0] && hop < max_redirects) {
                char* next = cl_resolve_redirect(&u, loc);
                http_url_free(&u);
                if (!next) { 
                    result = resp; 
                    break; 
                }   /* unresolvable -> return the 3xx */
                if (code == 301 || code == 302 || code == 303) {
                    if (cur_method != HTTP_GET && cur_method != HTTP_HEAD) {
                        cur_method   = HTTP_GET;        /* drop the body on method downgrade */
                        cur_body     = NULL;
                        cur_body_len = 0;
                    }
                }
                http_response_free(resp);
                free(cur_url);
                cur_url = next;
                continue;
            }

            http_url_free(&u);
            result = resp;
            break;
        }
        free(cur_url);
    }

    tcp_cleanup();
    return result;
}


/**
 * @brief Convenience wrapper: issue a GET request with default options.
 *
 * @param url Absolute http/https URL. Must not be NULL.
 * @return Newly-allocated response (free with http_response_free), or NULL on error.
 */
HttpResponse* http_get(const char* url) {
    return http_request(HTTP_GET, url, NULL, 0, NULL, 0, NULL);
}


/**
 * @brief Convenience wrapper: issue a POST request with a body.
 *
 * @param url          Absolute http/https URL. Must not be NULL.
 * @param content_type Value for the Content-Type header, or NULL to omit it.
 * @param body         Request body bytes (may be NULL).
 * @param body_len     Length of `body` in bytes.
 * @return Newly-allocated response (free with http_response_free), or NULL on error.
 */
HttpResponse* http_post(const char* url, const char* content_type, const void* body, size_t body_len) {
    HttpHeader h;
    HttpHeader* hp = NULL;
    size_t hc = 0;

    if (content_type) {
        h.name  = (char*)"Content-Type";
        h.value = (char*)content_type;
        hp = &h;
        hc = 1;
    }

    return http_request(HTTP_POST, url, hp, hc, body, body_len, NULL);
}


/**
 * @brief Convenience wrapper: issue a PUT request with a body.
 *
 * @param url          Absolute http/https URL. Must not be NULL.
 * @param content_type Value for the Content-Type header, or NULL to omit it.
 * @param body         Request body bytes (may be NULL).
 * @param body_len     Length of `body` in bytes.
 * @return Newly-allocated response (free with http_response_free), or NULL on error.
 */
HttpResponse* http_put(const char* url, const char* content_type, const void* body, size_t body_len) {
    HttpHeader h;
    HttpHeader* hp = NULL;
    size_t hc = 0;

    if (content_type) {
        h.name  = (char*)"Content-Type";
        h.value = (char*)content_type;
        hp = &h;
        hc = 1;
    }

    return http_request(HTTP_PUT, url, hp, hc, body, body_len, NULL);
}


/**
 * @brief Convenience wrapper: issue a DELETE request with default options.
 *
 * @param url Absolute http/https URL. Must not be NULL.
 * @return Newly-allocated response (free with http_response_free), or NULL on error.
 */
HttpResponse* http_delete(const char* url) {
    return http_request(HTTP_DELETE, url, NULL, 0, NULL, 0, NULL);
}
