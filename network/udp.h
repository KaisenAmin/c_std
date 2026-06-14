/**
 * @author Amin Tahmasebi
 * @class Udp
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in udp.c (file-level convention).
 */

#ifndef UDP_H_
#define UDP_H_

#include <stddef.h>
#include <stdbool.h>


/* #define UDP_LOGGING_ENABLE */

#ifdef UDP_LOGGING_ENABLE
    #include <stdio.h>
    #define UDP_LOG(fmt, ...) \
        do { fprintf(stderr, "[UDP LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define UDP_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Platform-specific socket handle                                    */
/* ------------------------------------------------------------------ */

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef SOCKET UdpSocket;
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <fcntl.h>      /* fcntl(), F_GETFL, F_SETFL, O_NONBLOCK */
    #include <net/if.h>     /* if_nametoindex() for multicast interface selection */
    typedef int UdpSocket;
#endif


/* ------------------------------------------------------------------ */
/* Public types                                                       */
/* ------------------------------------------------------------------ */

typedef enum {
    UDP_SUCCESS = 0,              /* No error                                  */
    UDP_ERR_SOCKET,               /* Socket creation failed                    */
    UDP_ERR_BIND,                 /* Binding socket failed                     */
    UDP_ERR_SEND,                 /* Sending data failed                       */
    UDP_ERR_RECV,                 /* Receiving data failed                     */
    UDP_ERR_CLOSE,                /* Closing socket failed                     */
    UDP_ERR_SETUP,                /* Initial setup failed (WSAStartup etc.)    */
    UDP_ERR_RESOLVE,              /* Resolving hostname failed                 */
    UDP_ERR_GENERIC,              /* Generic error                             */
    UDP_ERR_NO_DATA,              /* No data available (non-blocking receive)  */
    UDP_ERR_BUFFER_TOO_SMALL,     /* Provided buffer too small                 */
    UDP_ERR_UNSUPPORTED           /* Unsupported feature                       */
} UdpStatus;


typedef struct {
    UdpStatus code;               /* Basic error code                          */
    int       sys_errno;          /* System-specific code (errno / WSALastErr) */
    char      message[256];       /* Human-readable error message              */
} UdpStatusInfo;


/* A pre-resolved peer address. Resolve a host:port ONCE with
 * udp_resolve_endpoint, then send to it repeatedly with udp_sendto_endpoint
 * (no per-packet getaddrinfo), or capture the sender of a datagram via
 * udp_recvfrom_endpoint and reply to it directly. Treat the fields as opaque
 * and use the helpers (udp_endpoint_to_string / udp_endpoint_equal). `len == 0`
 * means "unset". */
typedef struct {
    struct sockaddr_storage addr;
    socklen_t               len;
} UdpEndpoint;


/* ------------------------------------------------------------------ */
/* Lifecycle                                                          */
/* ------------------------------------------------------------------ */

UdpStatus    udp_init                       (void);
UdpStatus    udp_cleanup                    (void);


/* ------------------------------------------------------------------ */
/* Socket open / close                                                */
/* ------------------------------------------------------------------ */

UdpStatus    udp_socket_create              (UdpSocket* socket);
UdpStatus    udp_bind                       (UdpSocket socket, const char* host, unsigned short port);
UdpStatus    udp_connect                    (UdpSocket socket, const char* host, unsigned short port);
UdpStatus    udp_close                      (UdpSocket socket);


/* ------------------------------------------------------------------ */
/* Datagram I/O                                                       */
/* ------------------------------------------------------------------ */

UdpStatus    udp_sendto                     (UdpSocket socket, const void* buf, size_t len, size_t* sent, const char* dest_host, unsigned short dest_port);
UdpStatus    udp_recvfrom                   (UdpSocket socket, void* buf, size_t len, size_t* received, char* src_host, size_t src_host_len, unsigned short* src_port);
UdpStatus    udp_send                       (UdpSocket socket, const void* buf, size_t len, size_t* sent);
UdpStatus    udp_recv                       (UdpSocket socket, void* buf, size_t len, size_t* received);


/* ------------------------------------------------------------------ */
/* Pre-resolved endpoints (avoid per-packet DNS; reply to senders)    */
/* ------------------------------------------------------------------ */

UdpStatus    udp_resolve_endpoint           (const char* host, unsigned short port, UdpEndpoint* out);
UdpStatus    udp_sendto_endpoint            (UdpSocket socket, const void* buf, size_t len, size_t* sent, const UdpEndpoint* dest);
UdpStatus    udp_recvfrom_endpoint          (UdpSocket socket, void* buf, size_t len, size_t* received, UdpEndpoint* src);
UdpStatus    udp_endpoint_to_string         (const UdpEndpoint* ep, char* host, size_t host_len, unsigned short* port);
bool         udp_endpoint_equal             (const UdpEndpoint* a, const UdpEndpoint* b);
UdpStatus    udp_request_reply              (UdpSocket socket, const UdpEndpoint* dest, const void* req, size_t req_len, void* reply, size_t reply_cap, size_t* reply_len,
                                             long timeout_ms, int retries, bool match_source);


/* ------------------------------------------------------------------ */
/* Socket options                                                     */
/* ------------------------------------------------------------------ */

UdpStatus    udp_set_non_blocking           (UdpSocket socket, bool enable);
UdpStatus    udp_set_broadcast              (UdpSocket socket, bool enable);
UdpStatus    udp_set_timeout                (UdpSocket socket, long timeout_ms);
UdpStatus    udp_set_reuse_addr             (UdpSocket socket, bool enable);
UdpStatus    udp_set_buffer_size            (UdpSocket socket, size_t send_bytes, size_t recv_bytes);


/* ------------------------------------------------------------------ */
/* Multicast                                                          */
/* ------------------------------------------------------------------ */

UdpStatus    udp_join_multicast_group       (UdpSocket socket, const char* group_addr, const char* iface_addr);
UdpStatus    udp_leave_multicast_group      (UdpSocket socket, const char* group_addr, const char* iface_addr);
UdpStatus    udp_set_multicast_ttl          (UdpSocket socket, int ttl);


/* ------------------------------------------------------------------ */
/* Address / peer queries                                             */
/* ------------------------------------------------------------------ */

UdpStatus    udp_resolve_hostname           (const char* hostname, char* ip_address, size_t ip_address_len);
UdpStatus    udp_get_local_address          (UdpSocket socket, char* host, size_t host_len, unsigned short* port);
UdpStatus    udp_get_peer_address           (UdpSocket socket, char* host, size_t host_len, unsigned short* port);
bool         udp_is_valid_address           (const char* address);


/* ------------------------------------------------------------------ */
/* Extra socket options / readiness (production helpers)              */
/* ------------------------------------------------------------------ */

UdpStatus    udp_set_ttl                    (UdpSocket socket, int ttl);
UdpStatus    udp_set_multicast_loopback     (UdpSocket socket, bool enable);
UdpStatus    udp_bytes_available            (UdpSocket socket, size_t* available);
UdpStatus    udp_wait_readable              (UdpSocket socket, long timeout_ms);
UdpStatus    udp_wait_writable              (UdpSocket socket, long timeout_ms);


/* ------------------------------------------------------------------ */
/* Diagnostics                                                        */
/* ------------------------------------------------------------------ */

void         udp_get_last_error             (UdpStatusInfo* status_info);
const char*  udp_status_to_string           (UdpStatus code);


#endif 
