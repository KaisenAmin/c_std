#include "tcp.h"
#include "../fmt/fmt.h"
#include <stdlib.h>
#include <string.h>

#define TCP_INVALID_SOCKET (~(TcpSocket)0)

static SocketSSLMapping sslMappings[MAX_SSL_CONNECTIONS];
static SSL_CTX* ssl_ctx = NULL;

// Initialize the SSL mappings
static void initialize_ssl_mappings() {
    for (int i = 0; i < MAX_SSL_CONNECTIONS; i++) {
        sslMappings[i].socket = -1; // Indicates an unused entry
        sslMappings[i].ssl = NULL;
    }
}

// Find or allocate a mapping entry for a given socket
static SocketSSLMapping* get_ssl_mapping(TcpSocket socket, bool allocate) {
    SocketSSLMapping* emptySlot = NULL;
    for (int i = 0; i < MAX_SSL_CONNECTIONS; i++) {
        if (sslMappings[i].socket == socket) {
            return &sslMappings[i];
        } 
        else if (!emptySlot && sslMappings[i].socket == TCP_INVALID_SOCKET && allocate) {
            emptySlot = &sslMappings[i];
        }
    }
    if (allocate && emptySlot) {
        emptySlot->socket = socket;
        return emptySlot;
    }
    return NULL;
}

// Utility function to populate human-readable error message
static void tcp_format_error_message(TcpStatusInfo* status_info) {
    if (!status_info) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: status infor is null and invalid in tcp_format_error_message.\n");
        #endif 
        return;
    }

#if defined(_WIN32) || defined(_WIN64)
    // Retrieves the last Winsock error description
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL, status_info->sys_errno,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPSTR)status_info->message, sizeof(status_info->message), NULL);
#else
    // Uses strerror_r for thread-safe error string retrieval on Unix-like systems
    strerror_r(status_info->sys_errno, status_info->message, sizeof(status_info->message));
#endif
}

void tcp_get_last_error(TcpStatusInfo* status_info) {
    if (!status_info) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Status info param is null and invalid in tcp_get_last_error.\n");
        #endif
        return;
    }

    #if defined(_WIN32) || defined(_WIN64)
        status_info->sys_errno = WSAGetLastError();
    #else
        status_info->sys_errno = errno;
    #endif

    tcp_format_error_message(status_info);
}

bool tcp_is_valid_address(const char* address) {
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;

    // Try to convert the address as IPv4
    if (inet_pton(AF_INET, address, &(sa.sin_addr)) == 1) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stdout, "Address %s is a valid IPv4 address.\n", address);
        #endif
        return true;
    }

    // Try to convert the address as IPv6
    if (inet_pton(AF_INET6, address, &(sa6.sin6_addr)) == 1) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stdout, "Address %s is a valid IPv6 address.\n", address);
        #endif
        return true;
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stderr, "Address %s is not a valid IPv4 or IPv6 address.\n", address);
    #endif
    return false;
}

TcpStatus tcp_socket_create(TcpSocket* socket) {
    if (!socket) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Socket parameter is null in tcp_socket_create.\n");
        #endif
        return TCP_ERR_SOCKET;
    }

    #if defined(_WIN32) || defined(_WIN64)
        *socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
        if (*socket == INVALID_SOCKET) {
            #ifdef TCP_LOGGING_ENABLE
                TcpStatusInfo status_info;
                tcp_get_last_error(&status_info); // Populate status_info with the last error
                fmt_fprintf(stderr, "Error: Socket creation failed with error: %s in tcp_socket_create\n", status_info.message);
            #endif
            return TCP_ERR_SOCKET;
        }
    #else
    *socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*socket < 0) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info); 
            fmt_fprintf(stderr, "Error: Socket creation failed with error: %s in tcp_socket_create\n", status_info.message);
        #endif
        return TCP_ERR_SOCKET;
    }
    #endif

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Socket created successfully in tcp_socket_create.\n");
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_bind(TcpSocket socket, const char* host, unsigned short port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (host == NULL || strcmp(host, "") == 0 || strcmp(host, "0.0.0.0") == 0) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } 
    else {
        if (!inet_pton(AF_INET, host, &addr.sin_addr)) {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: Invalid host address %s in tcp_bind.\n", host);
            #endif
            return TCP_ERR_RESOLVE;
        }
    }

    if (bind(socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info); 
            fmt_fprintf(stderr, "Error: Binding failed with error: %s in tcp_bind\n", status_info.message);
        #endif
        return TCP_ERR_BIND;
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Socket bound successfully to %s:%u in tcp_bind.\n", host ? host : "ANY", port);
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_listen(TcpSocket socket, int backlog) {
    if (listen(socket, backlog) < 0) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info); 
            fmt_fprintf(stderr, "Error: Listening on socket failed with error: %s in tcp_listen\n", status_info.message);
        #endif
        return TCP_ERR_LISTEN;
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Socket is now listening with a backlog of %d in tcp_listen.\n", backlog);
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_accept(TcpSocket socket, TcpSocket* client_socket) {
    if (!client_socket) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Client socket parameter is null in tcp_accept.\n");
        #endif
        return TCP_ERR_ACCEPT;
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));

    *client_socket = accept(socket, (struct sockaddr*)&client_addr, &client_addr_len);
    if (*client_socket == INVALID_SOCKET) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            // Check if the error is due to the operation that would block
            #if defined(_WIN32) || defined(_WIN64)
            if (status_info.sys_errno == WSAEWOULDBLOCK) {
                fmt_fprintf(stderr, "Non-blocking socket operation could not be completed immediately in tcp_accept.\n");
                return TCP_ERR_WOULD_BLOCK; // Assuming you define TCP_ERR_WOULD_BLOCK
            }
            #else
            if (status_info.sys_errno == EAGAIN || status_info.sys_errno == EWOULDBLOCK) {
                fmt_fprintf(stderr, "Non-blocking socket operation could not be completed immediately in tcp_accept.\n");
                return TCP_ERR_WOULD_BLOCK; // Assuming you define TCP_ERR_WOULD_BLOCK
            }
            #endif
            fmt_fprintf(stderr, "Error: Accepting connection failed with error: %s in tcp_accept\n", status_info.message);
        #endif
        return TCP_ERR_ACCEPT;
    }

    #ifdef TCP_LOGGING_ENABLE
        // convert IP to string to log it
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        fmt_fprintf(stdout, "Accepted connection from %s:%d in tcp_accept.\n", client_ip, ntohs(client_addr.sin_port));
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_connect(TcpSocket socket, const char* host, unsigned short port) {
    if (host == NULL || host[0] == '\0') {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Host parameter is null or empty in tcp_connect.\n");
        #endif
        return TCP_ERR_RESOLVE;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Invalid server address %s in tcp_connect.\n", host);
        #endif
        return TCP_ERR_RESOLVE;
    }

    if (connect(socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            fmt_fprintf(stderr, "Error: Connecting to server failed with error: %s in tcp_connect\n", status_info.message);
        #endif
        return TCP_ERR_CONNECT;
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Successfully connected to server %s:%d in tcp_connect.\n", host, port);
    #endif
    return TCP_SUCCESS;
}

// thsi is just for windows before creating socket in unix like system we dont need this .
TcpStatus tcp_init(void) {
    #if defined(_WIN32) || defined(_WIN64)
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (result != 0) {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "WSAStartup failed with error: %d\n", result);
            #endif
            return TCP_ERR_SETUP;
        }
    #endif

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Network API initialized successfully.\n");
    #endif
    return TCP_SUCCESS;
}

// also this one is for windows 
TcpStatus tcp_cleanup(void) {
    #if defined(_WIN32) || defined(_WIN64)
        WSACleanup();
    #endif

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Network API cleaned up successfully.\n");
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_send(TcpSocket socket, const void* buf, size_t len, size_t* sent) {
    if (!buf || len == 0) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Buffer is null or length is 0 in tcp_send.\n");
        #endif
        return TCP_ERR_SEND;
    }

    size_t bytes_sent = 0;
    while (len > 0) {
        #if defined(_WIN32) || defined(_WIN64)
            WSABUF wsaBuf;
            wsaBuf.buf = (CHAR*)buf;
            wsaBuf.len = (ULONG)len;
            DWORD flags = 0;
            DWORD bytesSent = 0;
            int result = WSASend(socket, &wsaBuf, 1, &bytesSent, flags, NULL, NULL);
            if (result == SOCKET_ERROR) {
                if (sent) *sent = (size_t)bytes_sent;
                #ifdef TCP_LOGGING_ENABLE
                    TcpStatusInfo status_info;
                    tcp_get_last_error(&status_info);
                    fmt_fprintf(stderr, "Error: Sending data failed with error: %s in tcp_send\n", status_info.message);
                #endif
                return TCP_ERR_SEND;
            }
            bytes_sent += bytesSent;
        #else
            bytes_sent = send(socket, buf, len, 0);
            if (bytes_sent <= 0) {
                if (bytes_sent == 0) {
                    // Connection has been gracefully closed
                    return TCP_ERR_CLOSE;
                }
                if (sent) *sent = (size_t)bytes_sent;
                #ifdef TCP_LOGGING_ENABLE
                    TcpStatusInfo status_info;
                    tcp_get_last_error(&status_info);
                    fmt_fprintf(stderr, "Error: Sending data failed with error: %s in tcp_send\n", status_info.message);
                #endif
                return TCP_ERR_SEND;
            }
        #endif
        buf = (const char*)buf + bytes_sent;
        len -= bytes_sent;
    }

    if (sent) *sent = (size_t)bytes_sent;

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Data sent successfully (%zu bytes) in tcp_send.\n", bytes_sent);
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_recv(TcpSocket socket, void* buf, size_t len, size_t* received) {
    if (!buf || len == 0) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Buffer is null or length is 0 in tcp_recv.\n");
        #endif
        return TCP_ERR_RECV;
    }

    size_t bytes_received = 0;
    #if defined(_WIN32) || defined(_WIN64)
        WSABUF wsaBuf;
        wsaBuf.buf = (CHAR*)buf;
        wsaBuf.len = (ULONG)len;
        DWORD flags = 0;
        DWORD bytesRecvd = 0;
        int result = WSARecv(socket, &wsaBuf, 1, &bytesRecvd, &flags, NULL, NULL);
        if (result == SOCKET_ERROR) {
            bytes_received = SOCKET_ERROR;
        } else {
            bytes_received = (size_t)bytesRecvd;
        }
    #else
        bytes_received = recv(socket, buf, len, 0);
    #endif

    if (bytes_received > 0) {
        if (received) *received = (size_t)bytes_received;
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stdout, "Data received successfully (%zu bytes) in tcp_recv.\n", bytes_received);
        #endif
        return TCP_SUCCESS;
    } 
    else if (bytes_received == 0) {
        // Connection has been gracefully closed
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stdout, "Connection closed by peer in tcp_recv.\n");
        #endif
        return TCP_ERR_CLOSE;
    } 
    else {
        // An error occurred during recv
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            fmt_fprintf(stderr, "Error: Receiving data failed with error: %s in tcp_recv\n", status_info.message);
        #endif
        return TCP_ERR_RECV;
    }
}

TcpStatus tcp_close(TcpSocket socket) {
    int result = 0;

    #if defined(_WIN32) || defined(_WIN64)
    result = closesocket(socket);
    if (result == SOCKET_ERROR) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info); // Populate status_info with the last error
            fmt_fprintf(stderr, "Error: Closing socket failed with error: %s in tcp_close.\n", status_info.message);
        #endif
        return TCP_ERR_CLOSE;
    }
    #else
    result = close(socket);
    if (result < 0) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info); // Populate status_info with the last error
            fmt_fprintf(stderr, "Error: Closing socket failed with error: %s in tcp_close.\n", status_info.message);
        #endif
        return TCP_ERR_CLOSE;
    }
    #endif

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Socket closed successfully in tcp_close.\n");
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_shutdown(TcpSocket socket, TcpShutdownHow how) {
    int shutdownHow;

    #if defined(_WIN32) || defined(_WIN64)
    switch (how) {
        case TCP_SHUTDOWN_RECEIVE:
            shutdownHow = SD_RECEIVE;
            break;
        case TCP_SHUTDOWN_SEND:
            shutdownHow = SD_SEND;
            break;
        case TCP_SHUTDOWN_BOTH:
            shutdownHow = SD_BOTH;
            break;
        default:
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: Invalid shutdown operation specified in tcp_shutdown.\n");
            #endif
            return TCP_ERR_GENERIC;
    }
    #else
    // Map the enumeration directly to POSIX constants
    switch (how) {
        case TCP_SHUTDOWN_RECEIVE:
            shutdownHow = SHUT_RD;
            break;
        case TCP_SHUTDOWN_SEND:
            shutdownHow = SHUT_WR;
            break;
        case TCP_SHUTDOWN_BOTH:
            shutdownHow = SHUT_RDWR;
            break;
        default:
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: Invalid shutdown operation specified in tcp_shutdown.\n");
            #endif
            return TCP_ERR_GENERIC;
    }
    #endif

    if (shutdown(socket, shutdownHow) == -1) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            fmt_fprintf(stderr, "Error: Shutting down socket failed with error: %s in tcp_shutdown.\n", status_info.message);
        #endif
        return TCP_ERR_GENERIC;
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Socket shutdown successfully in tcp_shutdown.\n");
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_set_timeout(TcpSocket socket, TcpTimeoutOperation operation, long timeout_ms) {
    int result;
    #ifdef _WIN32
        DWORD timeout = (DWORD)timeout_ms;
    #else
        struct timeval timeout;
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = (timeout_ms % 1000) * 1000;
    #endif

    if (operation == TCP_TIMEOUT_RECV || operation == TCP_TIMEOUT_BOTH) {
        result = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
        if (result < 0) {
            #ifdef TCP_LOGGING_ENABLE
                TcpStatusInfo status_info;
                tcp_get_last_error(&status_info);
                fmt_fprintf(stderr, "Error: Setting receive timeout failed with error: %s in tcp_set_timeout.\n", status_info.message);
            #endif
            return TCP_ERR_GENERIC;
        }
    }

    if (operation == TCP_TIMEOUT_SEND || operation == TCP_TIMEOUT_BOTH) {
        result = setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
        if (result < 0) {
            #ifdef TCP_LOGGING_ENABLE
                TcpStatusInfo status_info;
                tcp_get_last_error(&status_info);
                fmt_fprintf(stderr, "Error: Setting send timeout failed with error: %s in tcp_set_timeout.\n", status_info.message);
            #endif
            return TCP_ERR_GENERIC;
        }
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Timeout set successfully in tcp_set_timeout.\n");
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_resolve_hostname(const char* hostname, char* ip_address, size_t ip_address_len) {
    struct addrinfo hints, *res, *p;
    int status;
    void *addr;

    if (!hostname || !ip_address) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Null parameter provided to tcp_resolve_hostname.\n");
        #endif
        return TCP_ERR_GENERIC;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // allows for any IP family
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error in getaddrinfo: %s\n", gai_strerror(status));
        #endif
        return TCP_ERR_RESOLVE;
    }

    for(p = res; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } 
        else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        // Convert the IP to a string and copy to ip_address
        if (inet_ntop(p->ai_family, addr, ip_address, ip_address_len) == NULL) {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error converting IP address to string in tcp_resolve_hostname.\n");
            #endif
            freeaddrinfo(res); 
            return TCP_ERR_GENERIC;
        }

        break; // if we get here, we have our first IP address
    }

    freeaddrinfo(res); 

    if (p == NULL) { 
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "No addresses found for %s\n", hostname);
        #endif
        return TCP_ERR_RESOLVE;
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Resolved hostname %s to IP address %s\n", hostname, ip_address);
    #endif

    return TCP_SUCCESS;
}

TcpStatus tcp_set_non_blocking(TcpSocket socket, bool enable) {
    #if defined(_WIN32) || defined(_WIN64)
        // On Windows, use ioctlsocket
        u_long mode = enable ? 1 : 0; // Non-zero value for non-blocking mode
        if (ioctlsocket(socket, FIONBIO, &mode) != NO_ERROR) {
            #ifdef TCP_LOGGING_ENABLE
                TcpStatusInfo status_info;
                tcp_get_last_error(&status_info);
                fmt_fprintf(stderr, "Error: Setting non-blocking mode failed with error: %s\n", status_info.message);
            #endif
            return TCP_ERR_GENERIC;
        }
    #else
        // On Unix-like systems, use fcntl
        int flags = fcntl(socket, F_GETFL, 0);
        if (flags == -1) {
            #ifdef TCP_LOGGING_ENABLE
                TcpStatusInfo status_info;
                tcp_get_last_error(&status_info);
                fmt_fprintf(stderr, "Error: Getting socket flags failed with error: %s\n", status_info.message);
            #endif
            return TCP_ERR_GENERIC;
        }

        flags = enable ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
        if (fcntl(socket, F_SETFL, flags) == -1) {
            #ifdef TCP_LOGGING_ENABLE
                TcpStatusInfo status_info;
                tcp_get_last_error(&status_info);
                fmt_fprintf(stderr, "Error: Setting non-blocking mode failed with error: %s\n", status_info.message);
            #endif
            return TCP_ERR_GENERIC;
        }
    #endif

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Non-blocking mode has been %s for the socket.\n", enable ? "enabled" : "disabled");
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_get_local_address(TcpSocket socket, char* address, size_t address_len, unsigned short* port) {
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    if (!address || address_len == 0 || !port) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Invalid parameter provided to tcp_get_local_address.\n");
        #endif
        return TCP_ERR_GENERIC;
    }
    memset(&addr, 0, sizeof(addr));

    // Attempt to retrieve the local address of the socket
    if (getsockname(socket, (struct sockaddr*)&addr, &len) == -1) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            fmt_fprintf(stderr, "Error: getsockname() failed with error: %s\n", status_info.message);
        #endif
        return TCP_ERR_GENERIC;
    }
    if (addr.ss_family == AF_INET) {
        // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in*)&addr;
        if (inet_ntop(AF_INET, &ipv4->sin_addr, address, address_len) == NULL) {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: inet_ntop() failed for IPv4 address.\n");
            #endif
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(ipv4->sin_port);
    } 
    else if (addr.ss_family == AF_INET6) {
        // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)&addr;
        if (inet_ntop(AF_INET6, &ipv6->sin6_addr, address, address_len) == NULL) {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: inet_ntop() failed for IPv6 address.\n");
            #endif
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(ipv6->sin6_port);
    } 
    else {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Unknown socket family.\n");
        #endif
        return TCP_ERR_GENERIC;
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Local address: %s, port: %u\n", address, *port);
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_get_remote_address(TcpSocket socket, char* address, size_t address_len, unsigned short* port) {
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    // Validate input parameters
    if (!address || address_len == 0 || !port) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Invalid parameter provided to tcp_get_remote_address.\n");
        #endif
        return TCP_ERR_GENERIC;
    }

    // Clear the memory for safety
    memset(&addr, 0, sizeof(addr));

    // Get remote address of the socket
    if (getpeername(socket, (struct sockaddr*)&addr, &len) == -1) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info); // Populate status_info with the last error
            fmt_fprintf(stderr, "Error: getpeername() failed with error: %s\n", status_info.message);
        #endif
        return TCP_ERR_GENERIC;
    }

    if (addr.ss_family == AF_INET) {
        // Handle IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in*)&addr;
        if (inet_ntop(AF_INET, &ipv4->sin_addr, address, address_len) == NULL) {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: inet_ntop() failed for IPv4 address.\n");
            #endif
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(ipv4->sin_port);
    } 
    else if (addr.ss_family == AF_INET6) {
        // Handle IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)&addr;
        if (inet_ntop(AF_INET6, &ipv6->sin6_addr, address, address_len) == NULL) {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: inet_ntop() failed for IPv6 address.\n");
            #endif
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(ipv6->sin6_port);
    } 
    else {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Unknown socket family.\n");
        #endif
        return TCP_ERR_GENERIC;
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Remote address: %s, port: %u\n", address, *port);
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_set_reuse_addr(TcpSocket socket, bool enabled) {
    int optval = enabled ? 1 : 0;
    int result;

    #if defined(_WIN32) || defined(_WIN64)
        // On Windows, setsockopt uses a char* for optval
        result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
    #else
        // On Unix-like systems, setsockopt uses a void* for optval
        result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    #endif

    if (result < 0) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info); // Populate status_info with the last error
            fmt_fprintf(stderr, "Error: Setting SO_REUSEADDR failed with error: %s in tcp_set_reuse_addr.\n", status_info.message);
        #endif
        return TCP_ERR_GENERIC; // Using TCP_ERR_GENERIC to indicate a failure in setting the option
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "SO_REUSEADDR has been %s for the socket in tcp_set_reuse_addr.\n", enabled ? "enabled" : "disabled");
    #endif

    return TCP_SUCCESS;
}

TcpStatus tcp_get_peer_name(TcpSocket socket, char* host, size_t host_len, unsigned short* port) {
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    if (getpeername(socket, (struct sockaddr*)&addr, &len) != 0) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            fmt_fprintf(stderr, "Error: getpeername() failed with error: %s\n", status_info.message);
        #endif
        return TCP_ERR_GENERIC;
    }

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in*)&addr;
        if (inet_ntop(AF_INET, &s->sin_addr, host, host_len) == NULL) {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: inet_ntop() failed for IPv4 address.\n");
            #endif
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(s->sin_port);
    } 
    else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
        if (inet_ntop(AF_INET6, &s->sin6_addr, host, host_len) == NULL) {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: inet_ntop() failed for IPv6 address.\n");
            #endif
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(s->sin6_port);
    } 
    else {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Unknown socket family.\n");
        #endif
        return TCP_ERR_GENERIC;
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Peer name: %s, port: %u\n", host, *port);
    #endif
    return TCP_SUCCESS;
}

TcpStatus tcp_get_sock_name(TcpSocket socket, char* host, size_t host_len, unsigned short* port) {
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    if (getsockname(socket, (struct sockaddr*)&addr, &len) != 0) {
        #ifdef TCP_LOGGING_ENABLE
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            fmt_fprintf(stderr, "Error: getsockname() failed with error: %s\n", status_info.message);
        #endif
        return TCP_ERR_GENERIC;
    }

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in*)&addr;
        if (inet_ntop(AF_INET, &s->sin_addr, host, host_len) == NULL) {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: inet_ntop() failed for IPv4 address.\n");
            #endif
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(s->sin_port);
    } 
    else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
        if (inet_ntop(AF_INET6, &s->sin6_addr, host, host_len) == NULL) {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: inet_ntop() failed for IPv6 address.\n");
            #endif
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(s->sin6_port);
    } 
    else {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Unknown socket family.\n");
        #endif
        return TCP_ERR_GENERIC;
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Socket name: %s, port: %u\n", host, *port);
    #endif
    return TCP_SUCCESS;
}

// Associate an SSL object with a TcpSocket
void tcp_set_ssl(TcpSocket socket, SSL* ssl) {
    SocketSSLMapping* mapping = get_ssl_mapping(socket, true);
    if (mapping) {
        mapping->ssl = ssl;
    }
}

// Retrieve the SSL object associated with a TcpSocket
SSL* tcp_get_ssl(TcpSocket socket) {
    SocketSSLMapping* mapping = get_ssl_mapping(socket, false);
    return mapping ? mapping->ssl : NULL;
}

TcpStatus tcp_enable_ssl(TcpSocket socket) {
    if (ssl_ctx == NULL) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "SSL context is not initialized.\n");
        #endif
        return TCP_ERR_SETUP;
    }

    // Create an SSL object for the socket
    SSL* ssl = SSL_new(ssl_ctx);
    if (!ssl) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Failed to create SSL object.\n");
        #endif
        return TCP_ERR_GENERIC;
    }

    // Associate the socket with the SSL object
    if (SSL_set_fd(ssl, socket) == 0) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Failed to associate socket with SSL.\n");
        #endif
        SSL_free(ssl);
        return TCP_ERR_GENERIC;
    }
    tcp_set_ssl(socket, ssl);

    return TCP_SUCCESS;
}

TcpStatus tcp_disable_ssl(TcpSocket socket) {
    SSL* ssl = tcp_get_ssl(socket);
    if (ssl == NULL) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "No SSL object associated with the socket.\n");
        #endif
        return TCP_ERR_NO_SSL;
    }

    // Perform SSL shutdown sequence
    int shutdownResult = SSL_shutdown(ssl);
    if (shutdownResult == 0) {
        SSL_shutdown(ssl);
    }

    // Free the SSL object and clear any mappings
    SSL_free(ssl);
    tcp_set_ssl(socket, NULL);

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "SSL shutdown completed.\n");
    #endif

    return TCP_SUCCESS;
}

TcpStatus tcp_ssl_init(const char* cert, const char* key) {
    if (ssl_ctx != NULL) {
        // SSL context is already initialized
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stdout, "SSL context is already initialized.\n");
        #endif
        return TCP_SUCCESS; 
    }

    // Initialize OpenSSL
    SSL_load_error_strings();   
    OpenSSL_add_ssl_algorithms(); // Register SSL/TLS ciphers and digests
    
    // Create a new SSL_CTX object as framework for TLS/SSL enabled functions
    ssl_ctx = SSL_CTX_new(TLS_server_method()); 
    if (!ssl_ctx) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error creating SSL context: %s\n", ERR_error_string(ERR_get_error(), NULL));
        #endif
        return TCP_ERR_SETUP; 
    }

    // Load certificate and private key files into the SSL context
    if (SSL_CTX_use_certificate_file(ssl_ctx, cert, SSL_FILETYPE_PEM) <= 0) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error loading certificate from file: %s\n", ERR_error_string(ERR_get_error(), NULL));
        #endif
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL; 
        return TCP_ERR_SSL;
    }

    if (SSL_CTX_use_PrivateKey_file(ssl_ctx, key, SSL_FILETYPE_PEM) <= 0) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error loading private key from file: %s\n", ERR_error_string(ERR_get_error(), NULL));
        #endif
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL; 
        return TCP_ERR_SSL;
    }

    // Verify private key
    if (!SSL_CTX_check_private_key(ssl_ctx)) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Private key does not match the public certificate: %s\n", ERR_error_string(ERR_get_error(), NULL));
        #endif
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL; 
        return TCP_ERR_SSL;
    }

    // Initialize SSL mappings for SSL/TLS session management
    initialize_ssl_mappings();

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "OpenSSL and SSL context initialized successfully.\n");
    #endif

    return TCP_SUCCESS;
}

TcpStatus tcp_ssl_cleanup(void) {
    if (ssl_ctx != NULL) {
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL;
        
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stdout, "SSL context cleaned up successfully.\n");
        #endif
    }

    // Clean up OpenSSL
    EVP_cleanup();
    ERR_free_strings();

    return TCP_SUCCESS;
}

TcpStatus tcp_ssl_connect(TcpSocket socket, const char* host) {
    if (ssl_ctx == NULL) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "SSL context is not initialized. Call tcp_ssl_init first.\n");
        #endif
        return TCP_ERR_SETUP; // SSL context is not initialized
    }

    if (host == NULL || host[0] == '\0') {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Host parameter is null or empty.\n");
        #endif
        return TCP_ERR_RESOLVE; // Host parameter is invalid
    }

    SSL* ssl = SSL_new(ssl_ctx);
    if (ssl == NULL) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Failed to create SSL object: %s\n", ERR_error_string(ERR_get_error(), NULL));
        #endif
        return TCP_ERR_SSL; // Failed to create SSL object
    }

    // Set the file descriptor for the SSL object
    if (SSL_set_fd(ssl, socket) == 0) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Failed to set file descriptor for SSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
        #endif
        SSL_free(ssl); 
        return TCP_ERR_SSL; 
    }

    // Set the hostname for Server Name Indication (SNI)
    if (SSL_set_tlsext_host_name(ssl, host) == 0) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Failed to set SNI Hostname: %s\n", ERR_error_string(ERR_get_error(), NULL));
        #endif
        SSL_free(ssl); 
        return TCP_ERR_SSL; 
    }

    // Perform the SSL handshake
    if (SSL_connect(ssl) != 1) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "SSL handshake failed: %s\n", ERR_error_string(ERR_get_error(), NULL));
        #endif
        SSL_free(ssl); // Clean up the SSL object on failure
        return TCP_ERR_SSL_HANDSHAKE; // SSL handshake failed
    }

    // Successfully established SSL connection, map the SSL object with the socket
    SocketSSLMapping* mapping = get_ssl_mapping(socket, true);
    if (!mapping) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Failed to map SSL object with socket.\n");
        #endif
        SSL_free(ssl); // Clean up the SSL object on failure
        return TCP_ERR_SSL; // Failed to store SSL object
    }
    mapping->ssl = ssl;

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Successfully established SSL connection to %s.\n", host);
    #endif

    return TCP_SUCCESS;
}

TcpStatus tcp_ssl_accept(TcpSocket socket) {
    if (ssl_ctx == NULL) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "SSL context is not initialized. Call tcp_ssl_init first.\n");
        #endif
        return TCP_ERR_SETUP; // SSL context is not initialized
    }

    // Create an SSL object from the context
    SSL* ssl = SSL_new(ssl_ctx);
    if (ssl == NULL) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Failed to create SSL object: %s\n", ERR_error_string(ERR_get_error(), NULL));
        #endif
        return TCP_ERR_SSL; // Failed to create SSL object
    }

    // Set the file descriptor for the SSL object
    if (SSL_set_fd(ssl, socket) == 0) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Failed to set file descriptor for SSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
        #endif
        SSL_free(ssl); // Clean up the SSL object on failure
        return TCP_ERR_SSL; // Failed to associate socket with SSL
    }

    // Wait for a TLS/SSL client to initiate the TLS/SSL handshake
    int acceptResult = SSL_accept(ssl);
    if (acceptResult <= 0) {
        
        #ifdef TCP_LOGGING_ENABLE
            int sslError = SSL_get_error(ssl, acceptResult);
            fmt_fprintf(stderr, "SSL_accept failed with SSL error: %d\n", sslError);
            if (sslError == SSL_ERROR_SYSCALL) {
                unsigned long err;
                while ((err = ERR_get_error()) != 0) {
                    fmt_fprintf(stderr, "OpenSSL Error: %s\n", ERR_error_string(err, NULL));
                }
                if (errno != 0) { // Check if errno has been set
                    fmt_fprintf(stderr, "Syscall error: %s\n", strerror(errno));
                }
            }
        #endif
        SSL_free(ssl);
        return TCP_ERR_SSL_HANDSHAKE;
    }


    // Handshake was successful, store the SSL object with the socket for further operations
    SocketSSLMapping* mapping = get_ssl_mapping(socket, true);
    if (!mapping) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Failed to map SSL object with socket.\n");
        #endif
        SSL_free(ssl); 
        return TCP_ERR_SSL; 
    }
    mapping->ssl = ssl;

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "SSL handshake completed successfully.\n");
    #endif

    return TCP_SUCCESS;
}

TcpStatus tcp_ssl_close(TcpSocket socket) {
    SSL* ssl = tcp_get_ssl(socket);
    if (!ssl) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "No SSL object associated with the socket. Closing socket without SSL shutdown.\n");
        #endif
        
        tcp_close(socket);
        return TCP_ERR_NO_SSL; // Indicate that there was no SSL to shutdown
    }

    // Initiate the SSL/TLS shutdown sequence
    int shutdownResult = SSL_shutdown(ssl);
    if (shutdownResult == 0) {
        shutdownResult = SSL_shutdown(ssl);
        if (shutdownResult != 1) {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "SSL shutdown did not complete cleanly: %s\n", ERR_error_string(SSL_get_error(ssl, shutdownResult), NULL));
            #endif
            // Even if shutdown did not complete, continue to free SSL and close socket
        }
    } 
    else if (shutdownResult < 0) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "SSL shutdown failed: %s\n", ERR_error_string(SSL_get_error(ssl, shutdownResult), NULL));
        #endif
        // Even in case of failure, we need to free SSL and close the socket
    }

    // Free the SSL object
    SSL_free(ssl);
    tcp_set_ssl(socket, NULL);

    // Close the socket
    TcpStatus closeStatus = tcp_close(socket);
    if (closeStatus != TCP_SUCCESS) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Socket close failed.\n");
        #endif
        return closeStatus;
    }

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "SSL connection and socket closed successfully.\n");
    #endif

    return TCP_SUCCESS;
}

TcpStatus tcp_ssl_send(TcpSocket socket, const void* buf, size_t len, size_t* sent) {
    if (!buf || len == 0) {
        #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stderr, "Invalid buffer or length for SSL send.\n");
        #endif
        return TCP_ERR_SEND; // Invalid parameters
    }

    SSL* ssl = tcp_get_ssl(socket);
    if (!ssl) {
        #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stderr, "No SSL object associated with the socket. Cannot send data.\n");
        #endif
        return TCP_ERR_NO_SSL; // SSL context not found for the socket
    }

    int totalSent = 0;
    int result = 0;
    const char* dataPtr = (const char*)buf;

    while (len > 0) {
        result = SSL_write(ssl, dataPtr, len);
        if (result <= 0) {
            int sslError = SSL_get_error(ssl, result);
            switch (sslError) {
                case SSL_ERROR_WANT_WRITE:
                case SSL_ERROR_WANT_READ:
                    // The operation did not complete; the same call can be retried later
                    #ifdef TCP_LOGGING_ENABLE
                        fmt_fprintf(stderr, "SSL_write needs to be called again.\n");
                    #endif
                    if (sent) *sent = totalSent;
                    return TCP_SUCCESS; // Not an error, but the send needs to be retried
                case SSL_ERROR_ZERO_RETURN:
                    // The connection was closed
                    #ifdef TCP_LOGGING_ENABLE
                        fmt_fprintf(stderr, "SSL connection closed by peer.\n");
                    #endif
                    return TCP_ERR_CLOSE;
                case SSL_ERROR_SYSCALL:
                    #ifdef TCP_LOGGING_ENABLE
                        fmt_fprintf(stderr, "SSL write syscall error: %s\n", strerror(errno));
                    #endif
                    return TCP_ERR_SEND;
                default:
                    // An error occurred
                    #ifdef TCP_LOGGING_ENABLE
                        fmt_fprintf(stderr, "SSL write error: %s\n", ERR_error_string(ERR_get_error(), NULL));
                    #endif
                    return TCP_ERR_SEND;
            }
        }

        totalSent += result;
        dataPtr += result;
        len -= result;
    }

    if (sent) *sent = totalSent;

    #ifdef TCP_LOGGING_ENABLE
        fmt_fprintf(stdout, "Sent %d bytes over SSL.\n", totalSent);
    #endif

    return TCP_SUCCESS;
}

TcpStatus tcp_ssl_recv(TcpSocket socket, void* buf, size_t len, size_t* received) {
    if (!buf || len == 0) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Invalid buffer or length for SSL receive.\n");
        #endif
        return TCP_ERR_RECV; // Invalid parameters
    }

    SSL* ssl = tcp_get_ssl(socket);
    if (!ssl) {
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "No SSL object associated with the socket. Cannot receive data.\n");
        #endif
        return TCP_ERR_NO_SSL; // SSL context not found for the socket
    }

    int result = SSL_read(ssl, buf, len);
    if (result > 0) {
        if (received) *received = result;
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stdout, "Received %d bytes over SSL.\n", result);
        #endif
        return TCP_SUCCESS;
    } 
    else {
        int sslError = SSL_get_error(ssl, result);
        switch (sslError) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                // The operation did not complete; the same call can be retried later
                if (received) *received = 0;
                #ifdef TCP_LOGGING_ENABLE
                    fmt_fprintf(stderr, "SSL_read needs to be called again.\n");
                #endif
                return TCP_SUCCESS; // Not an error, but the read needs to be retried
            case SSL_ERROR_ZERO_RETURN:
                // The connection was closed
                #ifdef TCP_LOGGING_ENABLE
                    fmt_fprintf(stdout, "SSL connection closed by peer.\n");
                #endif
                return TCP_ERR_CLOSE;
            case SSL_ERROR_SYSCALL: {
                if (ERR_peek_error() == 0) { // No specific SSL error
                    if (result == 0 || errno == 0) {
                        fmt_fprintf(stdout, "SSL connection closed by peer or EOF reached.\n");
                        SSL_free(ssl);
                        tcp_set_ssl(socket, NULL); // Ensure to clear the SSL mapping
                        tcp_close(socket); // Close the socket as the connection is done
                        return TCP_ERR_CLOSE;
                    } 
                    else {
                        fmt_fprintf(stderr, "SSL syscall error: %s\n", strerror(errno));
                    }
                }
                #ifdef TCP_LOGGING_ENABLE
                    fmt_fprintf(stderr, "SSL read syscall error: %s\n", strerror(errno));
                #endif
                return TCP_ERR_RECV;
            }
            default:
                // An error occurred
                #ifdef TCP_LOGGING_ENABLE
                    fmt_fprintf(stderr, "SSL read error: %s\n", ERR_error_string(ERR_get_error(), NULL));
                #endif
                return TCP_ERR_RECV;
        }
    }
}

TcpStatus tcp_get_connection_quality(TcpSocket socket, float* rtt, float* variance) {
    #if defined(__linux__)
        struct tcp_info info;
        socklen_t len = sizeof(info);
        memset(&info, 0, sizeof(info));

        if (getsockopt(socket, IPPROTO_TCP, TCP_INFO, (void *)&info, &len) == 0) {
            // Successfully retrieved TCP_INFO, extract RTT and variance (in microseconds)
            *rtt = info.tcpi_rtt / 1000.0; // Convert to milliseconds
            *variance = info.tcpi_rttvar / 1000.0; // Convert to milliseconds
            return TCP_SUCCESS;
        } 
        else {
            #ifdef TCP_LOGGING_ENABLE
                fmt_fprintf(stderr, "Error: Failed to get TCP connection quality.\n");
            #endif 
            return TCP_ERR_GENERIC;
        }
    #elif defined(_WIN32)
    // Windows does not provide a direct way to get RTT and its variance via socket options.
    // Custom implementation or estimation might be needed.
        (void)socket;
        (void)rtt;
        (void)variance;
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Direct RTT measurement not supported on Windows.\n");
        #endif 
        return TCP_ERR_UNSUPPORTED;
    #else
        #ifdef TCP_LOGGING_ENABLE
            fmt_fprintf(stderr, "Error: Your platform does not support direct RTT measurement via socket options.\n");
        #endif 
        return TCP_ERR_UNSUPPORTED;
    #endif
}

// Ensure the socket is in non-blocking mode before using tcp_async_send.
TcpStatus tcp_async_send(TcpSocket socket, const void* buf, size_t len) {
    size_t result = send(socket, buf, len, 0);
    if (result == TCP_INVALID_SOCKET) {
        // Check if the error is EWOULDBLOCK (or its equivalent), which is normal for non-blocking sockets
        #ifdef _WIN32
            int lastError = WSAGetLastError();
            if (lastError == WSAEWOULDBLOCK) {
                #ifdef TCP_LOGGING_ENABLE
                    fmt_fprintf(stderr, "Error: connection is blocked.\n");
                #endif 
                return TCP_ERR_WOULD_BLOCK;
            }
        #else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                #ifdef TCP_LOGGING_ENABLE
                    fmt_fprintf(stderr, "Error: connection is blocked.\n");
                #endif 
                return TCP_ERR_WOULD_BLOCK;
            }
        #endif
        return TCP_ERR_SEND;
    }

    return TCP_SUCCESS;
}

// Ensure the socket is in non-blocking mode before using tcp_async_recv. The tcp_set_non_blocking function facilitates this
TcpStatus tcp_async_recv(TcpSocket socket, void* buf, size_t len) {
    size_t result = recv(socket, buf, len, 0);
    if (result == TCP_INVALID_SOCKET) {
        // Check if the error is EWOULDBLOCK or its equivalent, which is normal for non-blocking sockets
        #ifdef _WIN32
            int lastError = WSAGetLastError();
            if (lastError == WSAEWOULDBLOCK) {
                #ifdef TCP_LOGGING_ENABLE
                    fmt_fprintf(stderr, "Error: No data available to read; non-blocking operation.\n");
                #endif 
                return TCP_ERR_WOULD_BLOCK;
            }
        #else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                #ifdef TCP_LOGGING_ENABLE
                    fmt_fprintf(stderr, "Error: No data available to read; non-blocking operation.\n");
                #endif 
                return TCP_ERR_WOULD_BLOCK;
            }
        #endif
        return TCP_ERR_RECV;
    } 
    else if (result == 0) {
        // Connection has been gracefully closed by the peer
        return TCP_ERR_RECV;
    }

    return TCP_SUCCESS;
}