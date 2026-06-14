/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Tcp
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in tcp.c (file-level convention).
 *
 * THREAD SAFETY:
 *   All functions that access SSL context or SSL mappings are
 *   thread-safe. The library uses a cross-platform mutex to protect
 *   global state.
 *
 * IPv6 SUPPORT:
 *   `tcp_connect` and `tcp_bind` support both IPv4 and IPv6 addresses.
 *
 * USAGE:
 *   Always call `tcp_init()` before using any TCP functions and
 *   `tcp_cleanup()` at program end. For SSL, call `tcp_ssl_init()`
 *   before using SSL functions and `tcp_ssl_cleanup()` at program end.
 */

#ifndef TCP_H_
#define TCP_H_

#include <stddef.h>
#include <stdbool.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


/* #define TCP_LOGGING_ENABLE */

#ifdef TCP_LOGGING_ENABLE
    #define TCP_LOG(fmt, ...) \
        do { fprintf(stderr, "[TCP LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define TCP_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Limits                                                             */
/* ------------------------------------------------------------------ */

#define MAX_SSL_CONNECTIONS 256


#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET TcpSocket;
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netinet/tcp.h>      /* For TCP_INFO on Linux */
    #include <unistd.h>
    #include <fcntl.h>
    #include <netdb.h>
    typedef int TcpSocket;
#endif



typedef enum {
    TCP_SUCCESS = 0,              /* No error                                  */
    TCP_ERR_SOCKET,               /* Socket creation failed                    */
    TCP_ERR_BIND,                 /* Binding socket failed                     */
    TCP_ERR_LISTEN,               /* Listening on socket failed                */
    TCP_ERR_ACCEPT,               /* Accepting connection failed               */
    TCP_ERR_CONNECT,              /* Connection to server failed               */
    TCP_ERR_SEND,                 /* Sending data failed                       */
    TCP_ERR_RECV,                 /* Receiving data failed                     */
    TCP_ERR_CLOSE,                /* Closing socket failed                     */
    TCP_ERR_SETUP,                /* Initial setup failed (WSAStartup etc.)    */
    TCP_ERR_RESOLVE,              /* Resolving hostname failed                 */
    TCP_ERR_GENERIC,              /* Generic error                             */
    TCP_ERR_SSL,                  /* SSL connection error                      */
    TCP_ERR_SSL_HANDSHAKE,        /* SSL handshake error                       */
    TCP_ERR_TRY_AGAIN,            /* Failed after retry                        */
    TCP_ERR_NO_SSL,
    TCP_ERR_UNSUPPORTED,          /* Unsupported feature                       */
    TCP_ERR_WOULD_BLOCK
} TcpStatus;


typedef enum {
    TCP_SHUTDOWN_RECEIVE = 0,     /* Further receptions disallowed             */
    TCP_SHUTDOWN_SEND,            /* Further transmissions disallowed          */
    TCP_SHUTDOWN_BOTH             /* Both directions disallowed                */
} TcpShutdownHow;


typedef enum {
    TCP_TIMEOUT_RECV = 1,         /* Set timeout for receive operations        */
    TCP_TIMEOUT_SEND = 2,         /* Set timeout for send operations           */
    TCP_TIMEOUT_BOTH = 3          /* Set timeout for both directions           */
} TcpTimeoutOperation;


typedef struct {
    TcpStatus code;               /* Basic error code                          */
    int       sys_errno;          /* System-specific code (errno / WSALastErr) */
    char      message[256];       /* Human-readable error message              */
} TcpStatusInfo;


/* ------------------------------------------------------------------ */
/* Lifecycle                                                          */
/* ------------------------------------------------------------------ */

TcpStatus    tcp_init                       (void);
TcpStatus    tcp_cleanup                    (void);


/* ------------------------------------------------------------------ */
/* Socket open / close                                                */
/* ------------------------------------------------------------------ */

TcpStatus    tcp_socket_create              (TcpSocket* socket);
TcpStatus    tcp_close                      (TcpSocket socket);
TcpStatus    tcp_shutdown                   (TcpSocket socket, TcpShutdownHow how);


/* ------------------------------------------------------------------ */
/* Server-side: bind / listen / accept                                */
/* ------------------------------------------------------------------ */

TcpStatus    tcp_bind                       (TcpSocket socket, const char* host, unsigned short port);
TcpStatus    tcp_listen                     (TcpSocket socket, int backlog);
TcpStatus    tcp_accept                     (TcpSocket socket, TcpSocket* client_socket);


/* ------------------------------------------------------------------ */
/* Client-side: connect                                               */
/* ------------------------------------------------------------------ */

TcpStatus    tcp_connect                    (TcpSocket socket, const char* host, unsigned short port);


/* ------------------------------------------------------------------ */
/* I/O (plain TCP)                                                    */
/* ------------------------------------------------------------------ */

TcpStatus    tcp_send                       (TcpSocket socket, const void* buf, size_t len, size_t* sent);
TcpStatus    tcp_recv                       (TcpSocket socket, void* buf, size_t len, size_t* received);
TcpStatus    tcp_send_all                   (TcpSocket socket, const void* buf, size_t len);
TcpStatus    tcp_recv_all                   (TcpSocket socket, void* buf, size_t len);
TcpStatus    tcp_async_send                 (TcpSocket socket, const void* buf, size_t len);
TcpStatus    tcp_async_recv                 (TcpSocket socket, void* buf, size_t len);


/* ------------------------------------------------------------------ */
/* Socket options                                                     */
/* ------------------------------------------------------------------ */

TcpStatus    tcp_set_non_blocking           (TcpSocket socket, bool enable);
TcpStatus    tcp_set_timeout                (TcpSocket socket, TcpTimeoutOperation operation, long timeout_ms);
TcpStatus    tcp_set_reuse_addr             (TcpSocket socket, bool enabled);
TcpStatus    tcp_set_keep_alive             (TcpSocket socket, bool enable);
TcpStatus    tcp_set_nodelay                (TcpSocket socket, bool enable);
TcpStatus    tcp_set_linger                 (TcpSocket socket, bool enable, int seconds);
TcpStatus    tcp_set_buffer_size            (TcpSocket socket, size_t send_bytes, size_t recv_bytes);


/* ------------------------------------------------------------------ */
/* Address / peer queries                                             */
/* ------------------------------------------------------------------ */

TcpStatus    tcp_resolve_hostname           (const char* hostname, char* ip_address, size_t ip_address_len);
TcpStatus    tcp_get_local_address          (TcpSocket socket, char* address, size_t address_len, unsigned short* port);
TcpStatus    tcp_get_remote_address         (TcpSocket socket, char* address, size_t address_len, unsigned short* port);
TcpStatus    tcp_get_peer_name              (TcpSocket socket, char* host, size_t host_len, unsigned short* port);
TcpStatus    tcp_get_sock_name              (TcpSocket socket, char* host, size_t host_len, unsigned short* port);
TcpStatus    tcp_get_connection_quality     (TcpSocket socket, float* rtt, float* variance);
bool         tcp_is_valid_address           (const char* address);


/* ------------------------------------------------------------------ */
/* Readiness / non-blocking helpers (production)                      */
/* ------------------------------------------------------------------ */

TcpStatus    tcp_wait_readable              (TcpSocket socket, long timeout_ms);
TcpStatus    tcp_wait_writable              (TcpSocket socket, long timeout_ms);
TcpStatus    tcp_bytes_available            (TcpSocket socket, size_t* available);
TcpStatus    tcp_get_socket_error           (TcpSocket socket, int* error_code);
TcpStatus    tcp_connect_timeout            (TcpSocket socket, const char* host, unsigned short port, long timeout_ms);


/* ------------------------------------------------------------------ */
/* SSL — global setup                                                 */
/* ------------------------------------------------------------------ */

TcpStatus    tcp_ssl_init                   (const char* cert, const char* key);
TcpStatus    tcp_ssl_init_client            (void);
TcpStatus    tcp_ssl_set_verify             (bool verify_peer);
TcpStatus    tcp_ssl_load_verify_locations  (const char* ca_file, const char* ca_path);
TcpStatus    tcp_ssl_cleanup                (void);


/* ------------------------------------------------------------------ */
/* SSL — per-socket                                                   */
/* ------------------------------------------------------------------ */

TcpStatus    tcp_enable_ssl                 (TcpSocket socket);
TcpStatus    tcp_disable_ssl                (TcpSocket socket);
TcpStatus    tcp_ssl_connect                (TcpSocket socket, const char* host);
TcpStatus    tcp_ssl_accept                 (TcpSocket socket);
TcpStatus    tcp_ssl_send                   (TcpSocket socket, const void* buf, size_t len, size_t* sent);
TcpStatus    tcp_ssl_recv                   (TcpSocket socket, void* buf, size_t len, size_t* received);
TcpStatus    tcp_ssl_close                  (TcpSocket socket);
void         tcp_set_ssl                    (TcpSocket socket, SSL* ssl);
SSL*         tcp_get_ssl                    (TcpSocket socket);
long         tcp_ssl_get_verify_result      (TcpSocket socket);
TcpStatus    tcp_connect_tls                (const char* host, unsigned short port, long timeout_ms, bool verify, TcpSocket* out_socket);


/* ------------------------------------------------------------------ */
/* Diagnostics                                                        */
/* ------------------------------------------------------------------ */

void         tcp_get_last_error             (TcpStatusInfo* status_info);
const char*  tcp_status_to_string           (TcpStatus code);


#endif 
