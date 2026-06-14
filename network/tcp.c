/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Tcp
*/

#include <stdlib.h>
#include <string.h>
#include "tcp.h"
#include "../fmt/fmt.h"
#include "../concurrent/concurrent.h"

#if !defined(_WIN32) && !defined(_WIN64)
#include <sys/select.h>   /* select(), fd_set, FD_* — tcp_wait_readable/writable */
#include <sys/ioctl.h>    /* ioctl(), FIONREAD — tcp_bytes_available             */
#include <errno.h>        /* errno, EINPROGRESS — tcp_connect_timeout            */
#endif 


static TcpStatus get_sock_address(int (*fn)(TcpSocket, struct sockaddr*, socklen_t*), TcpSocket socket, char* address, size_t address_len, unsigned short* port);

#define TCP_INVALID_SOCKET (~(TcpSocket)0)

// Thread safety: mutex for SSL context and mapping. Initialized exactly once
// (process lifetime) so nested/concurrent tcp_init/tcp_cleanup calls — e.g. a
// server thread plus client requests in the same process — never re-init or
// destroy a mutex another caller is relying on.
static Mutex    tcp_mutex;
static OnceFlag tcp_mutex_once = ONCE_FLAG_INIT;
static void tcp_mutex_init_once(void) {
    mutex_init(&tcp_mutex, MUTEX_RECURSIVE);
}


// Private SSL mapping struct (now only in tcp.c)
typedef struct {
    TcpSocket socket;
    SSL* ssl;
} SocketSSLMapping;


static SocketSSLMapping sslMappings[MAX_SSL_CONNECTIONS];
static SSL_CTX* ssl_ctx = NULL;


// Initialize the SSL mappings
static void initialize_ssl_mappings() {
    for (int i = 0; i < MAX_SSL_CONNECTIONS; i++) {
        sslMappings[i].socket = TCP_INVALID_SOCKET;
        sslMappings[i].ssl = NULL;
    }
}

// Remove SSL mapping for a socket
static void remove_ssl_mapping(TcpSocket socket) {
    for (int i = 0; i < MAX_SSL_CONNECTIONS; i++) {
        if (sslMappings[i].socket == socket) {
            sslMappings[i].socket = TCP_INVALID_SOCKET;
            sslMappings[i].ssl = NULL;
            break;
        }
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
        TCP_LOG("[tcp_format_error_message] Error: status infor is null and invalid in tcp_format_error_message.\n");
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


/**
 * @brief Retrieves the last system-specific error and formats it into a human-readable message.
 * 
 * This function populates the given `TcpStatusInfo` structure with the last error encountered in the system.
 * On Windows, it retrieves the error code using `WSAGetLastError()`. On Unix-like systems, it uses `errno`.
 * 
 * @param status_info Pointer to the TcpStatusInfo structure where the error information will be stored.
 */
void tcp_get_last_error(TcpStatusInfo* status_info) {
    if (!status_info) {
        TCP_LOG("[tcp_get_last_error] Error: Status info param is null and invalid in tcp_get_last_error.\n");
        return;
    }

    #if defined(_WIN32) || defined(_WIN64)
        status_info->sys_errno = WSAGetLastError();
    #else
        status_info->sys_errno = errno;
    #endif

    tcp_format_error_message(status_info);
}


/**
 * @brief Validates if the given string is a valid IPv4 or IPv6 address.
 * 
 * This function checks if the provided address is a valid IPv4 or IPv6 address by using `inet_pton()`. 
 * It returns `true` if the address is valid, and `false` otherwise.
 * 
 * @param address The address to be validated.
 * 
 * @return `true` if the address is a valid IPv4 or IPv6 address, `false` otherwise.
 */
bool tcp_is_valid_address(const char* address) {
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;

    if (!address) {
        // glibc's inet_pton() does strlen(src) and would crash on NULL.
        TCP_LOG("[tcp_is_valid_address] NULL address -> invalid.\n");
        return false;
    }

    // Try to convert the address as IPv4
    if (inet_pton(AF_INET, address, &(sa.sin_addr)) == 1) {
        TCP_LOG("[tcp_is_valid_address] Address %s is a valid IPv4 address.\n", address);
        return true;
    }

    // Try to convert the address as IPv6
    if (inet_pton(AF_INET6, address, &(sa6.sin6_addr)) == 1) {
        TCP_LOG("[tcp_is_valid_address] Address %s is a valid IPv6 address.\n", address);
        return true;
    }

    TCP_LOG("[tcp_is_valid_address] Address %s is not a valid IPv4 or IPv6 address.\n", address);
    return false;
}

/**
 * @brief Creates a TCP socket.
 * 
 * This function creates a new TCP socket and stores it in the provided `sock` parameter. 
 * It handles socket creation for both Windows and Unix-like systems.
 * 
 * @param sock Pointer to the TcpSocket where the created socket will be stored.
 * 
 * @return `TCP_SUCCESS` if the socket is created successfully, or `TCP_ERR_SOCKET` if an error occurs during socket creation.
 */
TcpStatus tcp_socket_create(TcpSocket* sock) {
    if (!sock) {
        TCP_LOG("[tcp_socket_create] Error: Socket parameter is null in tcp_socket_create.\n");
        return TCP_ERR_SOCKET;
    }

    #if defined(_WIN32) || defined(_WIN64)
        *sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
        if (*sock == TCP_INVALID_SOCKET) {
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info); // Populate status_info with the last error
            TCP_LOG("[tcp_socket_create] Error: Socket creation failed with error: %s in tcp_socket_create\n", status_info.message);
           
            return TCP_ERR_SOCKET;
        }
    #else
    *sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*sock < 0) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info); 
        TCP_LOG("[tcp_socket_create] Error: Socket creation failed with error: %s in tcp_socket_create\n", status_info.message);
    
        return TCP_ERR_SOCKET;
    }
    #endif

    TCP_LOG("[tcp_socket_create] Socket created successfully in tcp_socket_create.\n");
    return TCP_SUCCESS;
}


/**
 * @brief Binds a TCP socket to a specific host and port.
 * 
 * This function binds the provided socket to the specified host and port, allowing the socket to listen for 
 * incoming connections on that address. If the host is NULL, empty, or "0.0.0.0", it will bind to all available interfaces.
 * 
 * @param sock The socket to bind.
 * @param host The host address to bind to. Pass NULL, an empty string, or "0.0.0.0" to bind to all interfaces.
 * @param port The port number to bind to.
 * 
 * @return TCP_SUCCESS if the binding is successful, TCP_ERR_RESOLVE if the host address is invalid, or TCP_ERR_BIND if binding fails.
 */
TcpStatus tcp_bind(TcpSocket sock, const char* host, unsigned short port) {
    struct addrinfo hints, *res = NULL, *p = NULL;
    char port_str[16];
    
    snprintf(port_str, sizeof(port_str), "%u", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    int status = getaddrinfo((host && *host) ? host : NULL, port_str, &hints, &res);
    if (status != 0) {
        return TCP_ERR_RESOLVE;
    }

    TcpStatus result = TCP_ERR_BIND;
    for (p = res; p; p = p->ai_next) {
        if (bind(sock, p->ai_addr, p->ai_addrlen) == 0) {
            result = TCP_SUCCESS;
            break;
        }
    }
    freeaddrinfo(res);
    return result;
}


/**
 * @brief Sets a TCP socket to listen for incoming connections.
 * 
 * This function marks the socket as a passive socket, which means it will be used to accept incoming connection requests.
 * 
 * @param socket The socket to listen on.
 * @param backlog The maximum length to which the queue of pending connections may grow.
 * 
 * @return TCP_SUCCESS if the socket is successfully set to listen, or TCP_ERR_LISTEN if an error occurs.
 */
TcpStatus tcp_listen(TcpSocket socket, int backlog) {
    if (listen(socket, backlog) < 0) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info); 
        TCP_LOG("[tcp_listen] Error: Listening on socket failed with error: %s in tcp_listen\n", status_info.message);
        
        return TCP_ERR_LISTEN;
    }

    TCP_LOG("[tcp_listen] Socket is now listening with a backlog of %d in tcp_listen.\n", backlog);
    return TCP_SUCCESS;
}


/**
 * @brief Accepts a connection on a TCP socket.
 * 
 * This function accepts an incoming connection on the specified listening socket and returns the newly created socket 
 * for the connection. The function can handle both blocking and non-blocking sockets.
 * 
 * @param socket The listening socket that will accept the connection.
 * @param client_socket Pointer to the socket where the accepted client connection will be stored.
 * 
 * @return TCP_SUCCESS if the connection is accepted, TCP_ERR_WOULD_BLOCK if the operation would block, or TCP_ERR_ACCEPT if an error occurs.
 */
TcpStatus tcp_accept(TcpSocket socket, TcpSocket* client_socket) {
    if (!client_socket) {
        TCP_LOG("[tcp_accept] Error: Client socket parameter is null in tcp_accept.\n");
        return TCP_ERR_ACCEPT;
    }

    // Use sockaddr_storage so accept() can populate either IPv4 or IPv6
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));

    *client_socket = accept(socket, (struct sockaddr*)&client_addr, &client_addr_len);
    if (*client_socket == TCP_INVALID_SOCKET) {
        
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info);

        // Check if the error is due to the operation that would block
        #if defined(_WIN32) || defined(_WIN64)
        if (status_info.sys_errno == WSAEWOULDBLOCK) {
            TCP_LOG("[tcp_accept] Non-blocking socket operation could not be completed immediately in tcp_accept.\n");
            return TCP_ERR_WOULD_BLOCK; 
        }
        #else
        if (status_info.sys_errno == EAGAIN || status_info.sys_errno == EWOULDBLOCK) {
            TCP_LOG("[tcp_accept] Non-blocking socket operation could not be completed immediately in tcp_accept.\n");
            return TCP_ERR_WOULD_BLOCK; 
        }
        #endif
        TCP_LOG("[tcp_accept] Error: Accepting connection failed with error: %s in tcp_accept\n", status_info.message);
        
        return TCP_ERR_ACCEPT;
    }

    char client_ip[INET6_ADDRSTRLEN] = "?";
    unsigned short client_port = 0;

    if (client_addr.ss_family == AF_INET) {
        struct sockaddr_in* s4 = (struct sockaddr_in*)&client_addr;

        inet_ntop(AF_INET, &s4->sin_addr, client_ip, sizeof(client_ip));
        client_port = ntohs(s4->sin_port);
    }
    else if (client_addr.ss_family == AF_INET6) {
        struct sockaddr_in6* s6 = (struct sockaddr_in6*)&client_addr;

        inet_ntop(AF_INET6, &s6->sin6_addr, client_ip, sizeof(client_ip));
        client_port = ntohs(s6->sin6_port);
    }

    (void)client_ip;
    (void)client_port;
    TCP_LOG("[tcp_accept] Accepted connection from %s:%u in tcp_accept.\n", client_ip, client_port);
    return TCP_SUCCESS;
}


/**
 * @brief Establishes a connection to a remote server.
 * 
 * This function attempts to establish a TCP connection to the specified remote host and port. 
 * It resolves the host address and attempts to connect to the remote server.
 * 
 * @param socket The socket to use for the connection.
 * @param host The remote host to connect to.
 * @param port The port number on the remote server.
 * 
 * @return TCP_SUCCESS if the connection is successful, TCP_ERR_RESOLVE if the host cannot be resolved, or TCP_ERR_CONNECT if the connection fails.
 */
TcpStatus tcp_connect(TcpSocket socket, const char* host, unsigned short port) {
    if (!host || !*host) {
        return TCP_ERR_RESOLVE;
    }

    struct addrinfo hints, *res = NULL, *p = NULL;
    char port_str[16];
    
    snprintf(port_str, sizeof(port_str), "%u", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo(host, port_str, &hints, &res);

    if (status != 0) { 
        return TCP_ERR_RESOLVE;
    }

    TcpStatus result = TCP_ERR_CONNECT;
    for (p = res; p; p = p->ai_next) {
        if (connect(socket, p->ai_addr, p->ai_addrlen) == 0) {
            result = TCP_SUCCESS;
            break;
        }
    }
    freeaddrinfo(res);

    return result;
}


/**
 * @brief Initializes the network API (Windows only).
 * 
 * This function initializes the Winsock library on Windows systems. On Unix-like systems, 
 * no initialization is required before creating a socket, so this function does nothing on those platforms.
 * 
 * @return TCP_SUCCESS if the initialization is successful, or TCP_ERR_SETUP if WSAStartup fails (Windows only).
 */
TcpStatus tcp_init(void) {
    #if defined(_WIN32) || defined(_WIN64)
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (result != 0) {
            TCP_LOG("[tcp_init] WSAStartup failed with error: %d\n", result);
            return TCP_ERR_SETUP;
        }
    #endif
    // Recursive mutex: tcp_enable_ssl / tcp_disable_ssl / tcp_ssl_close hold
    // the lock and then call tcp_get_ssl / tcp_set_ssl which lock again.
    // Initialized once (never re-initialized) so repeated/concurrent tcp_init
    // calls are safe.
    call_once(&tcp_mutex_once, tcp_mutex_init_once);
    TCP_LOG("[tcp_init] Network API initialized successfully.\n");
    return TCP_SUCCESS;
}


/**
 * @brief Cleans up the network API (Windows only).
 * 
 * This function cleans up the Winsock library on Windows systems. On Unix-like systems, 
 * no cleanup is necessary, so this function does nothing on those platforms.
 * 
 * @return TCP_SUCCESS if the cleanup is successful.
 */
TcpStatus tcp_cleanup(void) {
    #if defined(_WIN32) || defined(_WIN64)
        WSACleanup();
    #endif
    /* tcp_mutex is process-global and intentionally NOT destroyed here: other
     * threads (or a later tcp_init) may still rely on it. It is reclaimed by
     * the OS at process exit. */
    TCP_LOG("[tcp_cleanup] Network API cleaned up successfully.\n");

    return TCP_SUCCESS;
}


/**
 * @brief Sends data over a TCP socket.
 * 
 * This function sends data from the provided buffer over the specified TCP socket. It handles both 
 * Windows and Unix-like systems, with platform-specific implementations.
 * 
 * @param socket The TCP socket to send data through.
 * @param buf Pointer to the buffer containing the data to send.
 * @param len The number of bytes to send from the buffer.
 * @param sent Pointer to a size_t variable where the number of bytes successfully sent will be stored.
 * 
 * @return TCP_SUCCESS if the data is sent successfully, TCP_ERR_CLOSE if the connection is closed, 
 *         or TCP_ERR_SEND if an error occurs during sending.
 */
TcpStatus tcp_send(TcpSocket socket, const void* buf, size_t len, size_t* sent) {
    if (!buf || len == 0) {
        TCP_LOG("[tcp_send] Error: Buffer is null or length is 0 in tcp_send.\n");
        return TCP_ERR_SEND;
    }

    // Plain send() on both platforms. The previous Windows path used
    // WSASend(), which ignores SO_SNDTIMEO and made the -1 error sentinel
    // cast to size_t indistinguishable from a giant valid count.
    size_t total_sent = 0;
    while (len > 0) {
#if defined(_WIN32) || defined(_WIN64)
        int n = send(socket, (const char*)buf, (int)len, 0);
        if (n == SOCKET_ERROR) {
            if (sent) {
                *sent = total_sent;
            }

            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            TCP_LOG("[tcp_send] send() failed: %s\n", status_info.message);

            return TCP_ERR_SEND;
        }
#else
        ssize_t n = send(socket, buf, len, 0);
        if (n <= 0) {
            if (n == 0) {
                return TCP_ERR_CLOSE;
            }
            if (sent) {
                *sent = total_sent;
            }

            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            TCP_LOG("[tcp_send] send() failed: %s\n", status_info.message);

            return TCP_ERR_SEND;
        }
#endif
        total_sent += (size_t)n;
        buf = (const char*)buf + n;
        len -= (size_t)n;
    }
    if (sent) {
        *sent = total_sent;
    }
    TCP_LOG("[tcp_send] Data sent successfully (%zu bytes) in tcp_send.\n", total_sent);

    return TCP_SUCCESS;
}


/**
 * @brief Receives data from a TCP socket.
 * 
 * This function attempts to receive data from the specified TCP socket into the provided buffer.
 * The function handles both Windows and Unix-like systems, with platform-specific implementations.
 * 
 * @param socket The TCP socket from which to receive data.
 * @param buf Pointer to the buffer where received data will be stored.
 * @param len The length of the buffer.
 * @param received Pointer to a size_t variable where the number of bytes received will be stored.
 * 
 * @return TCP_SUCCESS if data is received successfully, TCP_ERR_CLOSE if the connection is closed, 
 *         or TCP_ERR_RECV if an error occurs during receiving.
 */
TcpStatus tcp_recv(TcpSocket socket, void* buf, size_t len, size_t* received) {
    if (!buf || len == 0) {
        TCP_LOG("[tcp_recv] Error: Buffer is null or length is 0 in tcp_recv.\n");
        return TCP_ERR_RECV;
    }

    // Plain recv() on both platforms. The previous Windows path used
    // WSARecv(), which ignores SO_RCVTIMEO (only overlapped I/O honors it)
    // AND stored the SOCKET_ERROR sentinel in a size_t, where it became
    // SIZE_MAX and looked like a giant successful read — masking every
    // hard error and turning timeouts into infinite hangs.
#if defined(_WIN32) || defined(_WIN64)
    int n = recv(socket, (char*)buf, (int)len, 0);
    if (n == SOCKET_ERROR) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info);
        TCP_LOG("[tcp_recv] recv() failed: %s\n", status_info.message);

        return TCP_ERR_RECV;
    }
#else
    ssize_t n = recv(socket, buf, len, 0);

    if (n < 0) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info);
        TCP_LOG("[tcp_recv] recv() failed: %s\n", status_info.message);

        return TCP_ERR_RECV;
    }
#endif
    if (n == 0) {
        TCP_LOG("[tcp_recv] Connection closed by peer.\n");
        return TCP_ERR_CLOSE;
    }
    if (received) {
        *received = (size_t)n;
    }

    TCP_LOG("[tcp_recv] Data received successfully (%zu bytes) in tcp_recv.\n", (size_t)n);
    return TCP_SUCCESS;
}


/**
 * @brief Closes a TCP socket.
 * 
 * This function closes the specified TCP socket. It handles platform-specific socket closure
 * for both Windows and Unix-like systems.
 * 
 * @param socket The TCP socket to be closed.
 * 
 * @return TCP_SUCCESS if the socket is closed successfully, or TCP_ERR_CLOSE if an error occurs during closure.
 */
TcpStatus tcp_close(TcpSocket socket) {
    int result = 0;

    #if defined(_WIN32) || defined(_WIN64)
    result = closesocket(socket);
    if (result == SOCKET_ERROR) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info); 
        TCP_LOG("[tcp_close] Error: Closing socket failed with error: %s in tcp_close.\n", status_info.message);
        
        return TCP_ERR_CLOSE;
    }
    #else
    result = close(socket);
    if (result < 0) {
        TcpStatusInfo status_info;

        tcp_get_last_error(&status_info); 
        TCP_LOG("[tcp_close] Error: Closing socket failed with error: %s in tcp_close.\n", status_info.message);
        
        return TCP_ERR_CLOSE;
    }
    #endif

    TCP_LOG("[tcp_close] Socket closed successfully in tcp_close.\n");
    return TCP_SUCCESS;
}


/**
 * @brief Shuts down a TCP socket.
 * 
 * This function shuts down the specified TCP socket for the specified direction (receive, send, or both).
 * The function handles platform-specific shutdown operations for both Windows and Unix-like systems.
 * 
 * @param socket The TCP socket to be shut down.
 * @param how The direction of shutdown: TCP_SHUTDOWN_RECEIVE, TCP_SHUTDOWN_SEND, or TCP_SHUTDOWN_BOTH.
 * 
 * @return TCP_SUCCESS if the socket is shut down successfully, or TCP_ERR_GENERIC if an error occurs during shutdown.
 */
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
            TCP_LOG("[tcp_shutdown] Error: Invalid shutdown operation specified in tcp_shutdown.\n");
            return TCP_ERR_GENERIC;
    }
    #else
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
            TCP_LOG("[tcp_shutdown] Error: Invalid shutdown operation specified in tcp_shutdown.\n");
            return TCP_ERR_GENERIC;
    }
    #endif

    if (shutdown(socket, shutdownHow) == -1) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info);
        TCP_LOG("[tcp_shutdown] Error: Shutting down socket failed with error: %s in tcp_shutdown.\n", status_info.message);
        
        return TCP_ERR_GENERIC;
    }

    TCP_LOG("[tcp_shutdown] Socket shutdown successfully in tcp_shutdown.\n");
    return TCP_SUCCESS;
}


/**
 * @brief Sets a timeout for TCP socket operations.
 * 
 * This function sets the timeout for receiving or sending data on the given socket.
 * The timeout value is specified in milliseconds.
 * 
 * @param socket The TCP socket for which to set the timeout.
 * @param operation The operation for which to set the timeout. It can be:
 * - `TCP_TIMEOUT_RECV`: Set timeout for receiving data.
 * - `TCP_TIMEOUT_SEND`: Set timeout for sending data.
 * - `TCP_TIMEOUT_BOTH`: Set timeout for both sending and receiving.
 * @param timeout_ms The timeout value in milliseconds.
 * 
 * @return TcpStatus
 * - `TCP_SUCCESS`: Successfully set the timeout.
 * - `TCP_ERR_GENERIC`: Failed to set the timeout.
 */
TcpStatus tcp_set_timeout(TcpSocket socket, TcpTimeoutOperation operation, long timeout_ms) {

    if (operation != TCP_TIMEOUT_RECV && operation != TCP_TIMEOUT_SEND && operation != TCP_TIMEOUT_BOTH) {
        TCP_LOG("[tcp_set_timeout] Error: Invalid operation enum %d.", (int)operation);
        return TCP_ERR_GENERIC;
    }

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
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            TCP_LOG("[tcp_set_timeout] Error: Setting receive timeout failed with error: %s in tcp_set_timeout.\n", status_info.message);
            
            return TCP_ERR_GENERIC;
        }
    }

    if (operation == TCP_TIMEOUT_SEND || operation == TCP_TIMEOUT_BOTH) {
        result = setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
        if (result < 0) {
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            TCP_LOG("[tcp_set_timeout] Error: Setting send timeout failed with error: %s in tcp_set_timeout.\n", status_info.message);
            
            return TCP_ERR_GENERIC;
        }
    }

    TCP_LOG("[tcp_set_timeout] Timeout set successfully in tcp_set_timeout.\n");
    return TCP_SUCCESS;
}


/**
 * @brief Resolves a hostname to an IP address.
 * 
 * This function resolves a given hostname to its corresponding IP address (either IPv4 or IPv6).
 * The result is stored in the provided `ip_address` buffer.
 * 
 * @param hostname The hostname to resolve (e.g., "www.example.com").
 * @param ip_address A buffer to store the resolved IP address as a string.
 * @param ip_address_len The length of the `ip_address` buffer.
 * 
 * @return TcpStatus
 * - `TCP_SUCCESS`: Successfully resolved the hostname.
 * - `TCP_ERR_RESOLVE`: Failed to resolve the hostname.
 * - `TCP_ERR_GENERIC`: Failed to convert the IP address to a string or encountered an error during resolution.
 */
TcpStatus tcp_resolve_hostname(const char* hostname, char* ip_address, size_t ip_address_len) {
    struct addrinfo hints, *res, *p;
    int status;
    void *addr;

    if (!hostname || !ip_address) {
        TCP_LOG("[tcp_resolve_hostname] Error: Null parameter provided to tcp_resolve_hostname.\n");
        return TCP_ERR_GENERIC;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // allows for any IP family
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
        TCP_LOG("[tcp_resolve_hostname] Error in getaddrinfo: %s\n", gai_strerror(status));
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
            TCP_LOG("[tcp_resolve_hostname] Error converting IP address to string in tcp_resolve_hostname.\n");
            freeaddrinfo(res); 

            return TCP_ERR_GENERIC;
        }

        break; // if we get here, we have our first IP address
    }

    freeaddrinfo(res); 

    if (p == NULL) { 
        TCP_LOG("[tcp_resolve_hostname] No addresses found for %s\n", hostname);
        return TCP_ERR_RESOLVE;
    }

    TCP_LOG("[tcp_resolve_hostname] Resolved hostname %s to IP address %s\n", hostname, ip_address);
    return TCP_SUCCESS;
}


/**
 * @brief Sets the non-blocking mode for a TCP socket.
 *
 * This function sets or unsets the non-blocking mode on the given socket. In non-blocking mode,
 * socket operations like `recv` and `send` return immediately, even if they would block under normal circumstances.
 *
 * @param socket The TCP socket on which to set non-blocking mode.
 * @param enable If true, non-blocking mode will be enabled; if false, it will be disabled.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: Successfully set the non-blocking mode.
 * - `TCP_ERR_GENERIC`: Failed to set the non-blocking mode or retrieve socket flags.
 */
TcpStatus tcp_set_non_blocking(TcpSocket socket, bool enable) {
    #if defined(_WIN32) || defined(_WIN64)
        u_long mode = enable ? 1 : 0; // Non-zero value for non-blocking mode

        if (ioctlsocket(socket, FIONBIO, &mode) != NO_ERROR) {
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            TCP_LOG("[tcp_set_non_blocking] Error: Setting non-blocking mode failed with error: %s\n", status_info.message);
            
            return TCP_ERR_GENERIC;
        }
    #else
        int flags = fcntl(socket, F_GETFL, 0);

        if (flags == -1) {
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            TCP_LOG("[tcp_set_non_blocking] Error: Getting socket flags failed with error: %s\n", status_info.message);
           
            return TCP_ERR_GENERIC;
        }

        flags = enable ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);

        if (fcntl(socket, F_SETFL, flags) == -1) {
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            TCP_LOG("[tcp_set_non_blocking] Error: Setting non-blocking mode failed with error: %s\n", status_info.message);
            
            return TCP_ERR_GENERIC;
        }
    #endif

    TCP_LOG("[tcp_set_non_blocking] Non-blocking mode has been %s for the socket.\n", enable ? "enabled" : "disabled");
    return TCP_SUCCESS;
}


/**
 * @brief Retrieves the local IP address and port number of a TCP socket.
 *
 * This function retrieves the local IP address and port number bound to the given socket.
 *
 * @param socket The TCP socket from which to retrieve the local address.
 * @param address A buffer to store the local IP address as a string.
 * @param address_len The size of the address buffer.
 * @param port A pointer to store the local port number.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: Successfully retrieved the local address and port.
 * - `TCP_ERR_GENERIC`: Failed to retrieve the local address or port.
 */
TcpStatus tcp_get_local_address(TcpSocket socket, char* address, size_t address_len, unsigned short* port) {
    return get_sock_address(getsockname, socket, address, address_len, port);
}


/**
 * @brief Retrieves the remote IP address and port of a TCP connection.
 *
 * This function retrieves the remote IP address and port number associated with the specified TCP socket.
 *
 * @param socket The TCP socket for which to retrieve the remote address and port.
 * @param address A buffer to store the remote IP address in string form.
 * @param address_len The length of the address buffer.
 * @param port A pointer to store the remote port number.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: Successfully retrieved the remote address and port.
 * - `TCP_ERR_GENERIC`: Failed to retrieve the remote address or port.
 */
TcpStatus tcp_get_remote_address(TcpSocket socket, char* address, size_t address_len, unsigned short* port) {
    return get_sock_address(getpeername, socket, address, address_len, port);
}


/**
 * @brief Sets the SO_REUSEADDR option on a TCP socket.
 *
 * This function allows the reuse of local addresses on a TCP socket, enabling multiple connections
 * to bind to the same local address and port.
 *
 * @param socket The TCP socket on which to set the SO_REUSEADDR option.
 * @param enabled A boolean indicating whether to enable or disable the SO_REUSEADDR option.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: The SO_REUSEADDR option was set successfully.
 * - `TCP_ERR_GENERIC`: Failed to set the SO_REUSEADDR option.
 */
TcpStatus tcp_set_reuse_addr(TcpSocket socket, bool enabled) {
    int optval = enabled ? 1 : 0;
    int result;

    #if defined(_WIN32) || defined(_WIN64)
        result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
    #else
        result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    #endif

    if (result < 0) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info); // Populate status_info with the last error
        TCP_LOG("[tcp_set_reuse_addr] Error: Setting SO_REUSEADDR failed with error: %s in tcp_set_reuse_addr.\n", status_info.message);
        
        return TCP_ERR_GENERIC; 
    }

    TCP_LOG("[tcp_set_reuse_addr] SO_REUSEADDR has been %s for the socket in tcp_set_reuse_addr.\n", enabled ? "enabled" : "disabled");
    return TCP_SUCCESS;
}


/**
 * @brief Retrieves the peer (remote) address and port of a TCP connection.
 *
 * This function retrieves the remote IP address and port number associated with the specified TCP socket.
 *
 * @param socket The TCP socket for which to retrieve the remote address and port.
 * @param host A buffer to store the remote IP address in string form.
 * @param host_len The length of the host buffer.
 * @param port A pointer to store the remote port number.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: Successfully retrieved the remote address and port.
 * - `TCP_ERR_GENERIC`: Failed to retrieve the remote address or port.
 */
TcpStatus tcp_get_peer_name(TcpSocket socket, char* host, size_t host_len, unsigned short* port) {
    if (!host || host_len == 0) {
        TCP_LOG("[tcp_get_peer_name] Error: host buffer is NULL or zero-length.\n");
        return TCP_ERR_GENERIC;
    }

    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    if (getpeername(socket, (struct sockaddr*)&addr, &len) != 0) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info);
        TCP_LOG("[tcp_get_peer_name] Error: getpeername() failed with error: %s\n", status_info.message);

        return TCP_ERR_GENERIC;
    }

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in*)&addr;
        if (inet_ntop(AF_INET, &s->sin_addr, host, host_len) == NULL) {
            TCP_LOG("[tcp_get_peer_name] Error: inet_ntop() failed for IPv4 address.\n");
            return TCP_ERR_GENERIC;
        }
        if (port) {
            *port = ntohs(s->sin_port);
        }
    }
    else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
        if (inet_ntop(AF_INET6, &s->sin6_addr, host, host_len) == NULL) {
            TCP_LOG("[tcp_get_peer_name] Error: inet_ntop() failed for IPv6 address.\n");
            return TCP_ERR_GENERIC;
        }
        if (port) {
            *port = ntohs(s->sin6_port);
        }
    }
    else {
        TCP_LOG("[tcp_get_peer_name] Error: Unknown socket family.\n");
        return TCP_ERR_GENERIC;
    }

    TCP_LOG("[tcp_get_peer_name] Peer name: %s, port: %u\n", host, port ? *port : 0);
    return TCP_SUCCESS;
}


/**
 * @brief Retrieves the local address and port of a TCP socket.
 *
 * This function retrieves the local IP address and port number associated with the specified TCP socket.
 *
 * @param socket The TCP socket for which to retrieve the local address and port.
 * @param host A buffer to store the IP address in string form.
 * @param host_len The length of the host buffer.
 * @param port A pointer to store the port number.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: Successfully retrieved the socket's local address and port.
 * - `TCP_ERR_GENERIC`: Failed to retrieve the socket information.
 */
TcpStatus tcp_get_sock_name(TcpSocket socket, char* host, size_t host_len, unsigned short* port) {
    if (!host || host_len == 0) {
        TCP_LOG("[tcp_get_sock_name] Error: host buffer is NULL or zero-length.\n");
        return TCP_ERR_GENERIC;
    }

    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    if (getsockname(socket, (struct sockaddr*)&addr, &len) != 0) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info);
        TCP_LOG("[tcp_get_sock_name] Error: getsockname() failed with error: %s\n", status_info.message);

        return TCP_ERR_GENERIC;
    }

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in*)&addr;
        if (inet_ntop(AF_INET, &s->sin_addr, host, host_len) == NULL) {
            TCP_LOG("[tcp_get_sock_name] Error: inet_ntop() failed for IPv4 address.\n");
            return TCP_ERR_GENERIC;
        }
        if (port) {
            *port = ntohs(s->sin_port);
        }
    }
    else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
        if (inet_ntop(AF_INET6, &s->sin6_addr, host, host_len) == NULL) {
            TCP_LOG("[tcp_get_sock_name] Error: inet_ntop() failed for IPv6 address.\n");
            return TCP_ERR_GENERIC;
        }
        if (port) {
            *port = ntohs(s->sin6_port);
        }
    }
    else {
        TCP_LOG("[tcp_get_sock_name] Error: Unknown socket family.\n");
        return TCP_ERR_GENERIC;
    }

    TCP_LOG("[tcp_get_sock_name] Socket name: %s, port: %u\n", host, port ? *port : 0);
    return TCP_SUCCESS;
}


/**
 * @brief Associates an SSL object with a TCP socket.
 *
 * This function maps an SSL object to a specified TCP socket, which allows the socket to be used for secure communication.
 *
 * @param socket The TCP socket to associate with an SSL object.
 * @param ssl The SSL object to associate with the socket.
 */
void tcp_set_ssl(TcpSocket socket, SSL* ssl) {
    mutex_lock(&tcp_mutex);
    SocketSSLMapping* mapping = get_ssl_mapping(socket, true);

    if (mapping) {
        mapping->ssl = ssl;
    }
    if (!ssl) {
        remove_ssl_mapping(socket);
    }

    mutex_unlock(&tcp_mutex);
}


/**
 * @brief Retrieves the SSL object associated with a TCP socket.
 *
 * This function retrieves the SSL object that has been associated with a specified TCP socket.
 *
 * @param socket The TCP socket for which to retrieve the associated SSL object.
 * @return The SSL object associated with the socket, or NULL if no SSL object is associated.
 */
SSL* tcp_get_ssl(TcpSocket socket) {
    mutex_lock(&tcp_mutex);

    SocketSSLMapping* mapping = get_ssl_mapping(socket, false);
    SSL* ssl = mapping ? mapping->ssl : NULL;

    mutex_unlock(&tcp_mutex);
    return ssl;
}


/**
 * @brief Enables SSL/TLS on a specified TCP socket.
 *
 * This function creates an SSL object for the specified socket and associates it with the socket.
 * It prepares the socket for secure SSL/TLS communication.
 *
 * @param socket The TCP socket on which to enable SSL/TLS.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: SSL/TLS was successfully enabled on the socket.
 * - `TCP_ERR_SETUP`: SSL context is not initialized.
 * - `TCP_ERR_GENERIC`: Failed to create or associate an SSL object with the socket.
 */
TcpStatus tcp_enable_ssl(TcpSocket socket) {
    mutex_lock(&tcp_mutex);

    if (ssl_ctx == NULL) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_enable_ssl] SSL context is not initialized.\n");
        return TCP_ERR_SETUP;
    }

    SSL* ssl = SSL_new(ssl_ctx);
    if (!ssl) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_enable_ssl] Failed to create SSL object.\n");
        return TCP_ERR_GENERIC;
    }
    if (SSL_set_fd(ssl, socket) == 0) {
        SSL_free(ssl);
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_enable_ssl] Failed to associate socket with SSL.\n");
        return TCP_ERR_GENERIC;
    }

    tcp_set_ssl(socket, ssl);
    mutex_unlock(&tcp_mutex);

    return TCP_SUCCESS;
}


/**
 * @brief Disables SSL/TLS on a specified TCP socket.
 *
 * This function shuts down the SSL/TLS session for the specified socket, frees the associated SSL object,
 * and clears any SSL mappings for the socket.
 *
 * @param socket The TCP socket on which SSL/TLS should be disabled.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: SSL/TLS shutdown completed successfully.
 * - `TCP_ERR_NO_SSL`: No SSL object was associated with the socket.
 */
TcpStatus tcp_disable_ssl(TcpSocket socket) {
    mutex_lock(&tcp_mutex);
    SSL* ssl = tcp_get_ssl(socket);

    if (!ssl) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_disable_ssl] No SSL object associated with the socket.\n");
        return TCP_ERR_NO_SSL;
    }

    int shutdownResult = SSL_shutdown(ssl);
    if (shutdownResult == 0) {
        SSL_shutdown(ssl);
    }

    SSL_free(ssl);
    tcp_set_ssl(socket, NULL);
    mutex_unlock(&tcp_mutex);

    TCP_LOG("[tcp_disable_ssl] SSL shutdown completed.\n");
    return TCP_SUCCESS;
}


/**
 * @brief Initializes SSL/TLS support and loads a certificate and private key for secure communication.
 *
 * This function initializes OpenSSL, creates an SSL context, and loads the specified certificate and private key.
 * It also verifies that the private key matches the public certificate.
 *
 * @param cert The path to the PEM-encoded certificate file.
 * @param key The path to the PEM-encoded private key file.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: SSL context initialized successfully, and the certificate/private key were loaded and verified.
 * - `TCP_ERR_SETUP`: Failed to create the SSL context.
 * - `TCP_ERR_SSL`: Failed to load the certificate, private key, or verify the private key.
 */
TcpStatus tcp_ssl_init(const char* cert, const char* key) {
    mutex_lock(&tcp_mutex);
    if (ssl_ctx != NULL) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_init] SSL context is already initialized.\n");
        return TCP_SUCCESS; 
    }

    SSL_load_error_strings();   
    OpenSSL_add_ssl_algorithms();
    ssl_ctx = SSL_CTX_new(TLS_server_method()); 

    if (!ssl_ctx) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_init] Error creating SSL context: %s\n", ERR_error_string(ERR_get_error(), NULL));

        return TCP_ERR_SETUP; 
    }
    if (SSL_CTX_use_certificate_file(ssl_ctx, cert, SSL_FILETYPE_PEM) <= 0) {
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL; 
        mutex_unlock(&tcp_mutex);

        TCP_LOG("[tcp_ssl_init] Error loading certificate from file: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return TCP_ERR_SSL;
    }
    if (SSL_CTX_use_PrivateKey_file(ssl_ctx, key, SSL_FILETYPE_PEM) <= 0) {
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL; 
        mutex_unlock(&tcp_mutex);

        TCP_LOG("[tcp_ssl_init] Error loading private key from file: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return TCP_ERR_SSL;
    }
    if (!SSL_CTX_check_private_key(ssl_ctx)) {
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL; 
        mutex_unlock(&tcp_mutex);

        TCP_LOG("[tcp_ssl_init] Private key does not match the public certificate: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return TCP_ERR_SSL;
    }

    initialize_ssl_mappings();
    mutex_unlock(&tcp_mutex);

    TCP_LOG("[tcp_ssl_init] OpenSSL and SSL context initialized successfully.\n");

    return TCP_SUCCESS;
}


/**
 * @brief Initialize a client-side SSL context (TLS_client_method).
 *
 * Unlike tcp_ssl_init (which builds a server context and requires a cert/key),
 * this creates a context suitable for outbound TLS — no cert/key needed — and
 * loads the system CA store so tcp_ssl_get_verify_result can confirm the peer
 * chain. No-op if a context already exists.
 *
 * @return `TCP_SUCCESS` on success (or if a context already exists);
 *         `TCP_ERR_SETUP` if the client SSL context cannot be created.
 */
TcpStatus tcp_ssl_init_client(void) {
    mutex_lock(&tcp_mutex);
    if (ssl_ctx != NULL) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_init_client] SSL context already initialized.\n");
        return TCP_SUCCESS;
    }

    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (!ssl_ctx) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_init_client] Error creating client SSL context: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return TCP_ERR_SETUP;
    }

    /* Best-effort: load the OS trust store. Present on most Linux installs;
       may be empty on a bare MinGW box (callers can then use insecure mode). */
    SSL_CTX_set_default_verify_paths(ssl_ctx);

    initialize_ssl_mappings();
    mutex_unlock(&tcp_mutex);

    TCP_LOG("[tcp_ssl_init_client] client SSL context initialized.\n");
    return TCP_SUCCESS;
}


/**
 * @brief Verification result of the peer certificate chain after a handshake.
 * @param socket A TCP socket with an active TLS session.
 * @return X509_V_OK (0) on success, a non-zero X509_V_* code on failure, or -1
 *         if the socket has no SSL object.
 */
long tcp_ssl_get_verify_result(TcpSocket socket) {
    SSL* ssl = tcp_get_ssl(socket);
    if (!ssl) {
        return -1;
    }
    return (long)SSL_get_verify_result(ssl);
}


/**
 * @brief Enable/disable fail-closed certificate verification on the SSL context.
 *
 * With verify_peer = true the TLS handshake aborts on an untrusted or
 * hostname-mismatched certificate (SSL_VERIFY_PEER); with false it succeeds and
 * the result is available via tcp_ssl_get_verify_result. Process-global.
 *
 * @param verify_peer `true` to fail the handshake on an untrusted or
 *                    hostname-mismatched certificate; `false` for SSL_VERIFY_NONE.
 * @return TCP_SUCCESS, or TCP_ERR_SETUP if no SSL context exists yet.
 */
TcpStatus tcp_ssl_set_verify(bool verify_peer) {
    mutex_lock(&tcp_mutex);
    if (ssl_ctx == NULL) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_set_verify] SSL context not initialized.\n");
        return TCP_ERR_SETUP;
    }
    SSL_CTX_set_verify(ssl_ctx, verify_peer ? SSL_VERIFY_PEER : SSL_VERIFY_NONE, NULL);
    mutex_unlock(&tcp_mutex);
    return TCP_SUCCESS;
}


/**
 * @brief Load trusted CA certificates (a bundle file and/or a directory) into
 *        the SSL context, for environments without a usable OS trust store.
 *
 * @param ca_file PEM bundle path, or NULL.
 * @param ca_path Hashed-cert directory path, or NULL. At least one must be set.
 * @return TCP_SUCCESS, TCP_ERR_SETUP (no context) or TCP_ERR_SSL (load failed).
 */
TcpStatus tcp_ssl_load_verify_locations(const char* ca_file, const char* ca_path) {
    if (!ca_file && !ca_path) {
        return TCP_ERR_GENERIC;
    }
    mutex_lock(&tcp_mutex);
    if (ssl_ctx == NULL) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_load_verify_locations] SSL context not initialized.\n");
        return TCP_ERR_SETUP;
    }
    int rc = SSL_CTX_load_verify_locations(ssl_ctx, ca_file, ca_path);
    mutex_unlock(&tcp_mutex);
    if (rc != 1) {
        TCP_LOG("[tcp_ssl_load_verify_locations] failed: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return TCP_ERR_SSL;
    }
    return TCP_SUCCESS;
}


/**
 * @brief One-shot secure client connect (socket + connect + TLS + verification).
 *
 * Initializes a client SSL context, sets the verify mode, creates a socket,
 * connects with a timeout, performs the TLS handshake (with SNI and hostname
 * verification), and — when `verify` is set — confirms the peer chain. On any
 * failure the socket is closed before returning.
 *
 * @param host       Server hostname or IP. Must not be NULL or empty.
 * @param port       Server port in host byte order.
 * @param timeout_ms Connect timeout in milliseconds.
 * @param verify     When true, abort on an untrusted or hostname-mismatched cert.
 * @param out_socket Output: the connected TLS socket on success. Must not be NULL.
 * @return `TCP_SUCCESS` with `*out_socket` set; `TCP_ERR_GENERIC` on invalid
 *         arguments; `TCP_ERR_SSL` if certificate verification fails; or a
 *         propagated setup/connect/handshake error code.
 */
TcpStatus tcp_connect_tls(const char* host, unsigned short port, long timeout_ms, bool verify, TcpSocket* out_socket) {
    if (!host || host[0] == '\0' || !out_socket) {
        return TCP_ERR_GENERIC;
    }

    TcpStatus st = tcp_ssl_init_client();
    if (st != TCP_SUCCESS) {
        return st;
    }
    /* Fail-closed when verifying: a bad chain/hostname then aborts the handshake. */
    st = tcp_ssl_set_verify(verify);
    if (st != TCP_SUCCESS) {
        return st;
    }

    TcpSocket sock;
    st = tcp_socket_create(&sock);
    if (st != TCP_SUCCESS) {
        return st;
    }
    st = tcp_connect_timeout(sock, host, port, timeout_ms);
    if (st != TCP_SUCCESS) {
        tcp_close(sock);
        return st;
    }
    st = tcp_ssl_connect(sock, host);   /* SNI + hostname verification */
    if (st != TCP_SUCCESS) {
        tcp_close(sock);
        return st;
    }
    if (verify && tcp_ssl_get_verify_result(sock) != X509_V_OK) {
        TCP_LOG("[tcp_connect_tls] certificate verification failed for %s\n", host);
        tcp_ssl_close(sock);            /* tears down TLS and closes the fd */
        return TCP_ERR_SSL;
    }

    *out_socket = sock;
    return TCP_SUCCESS;
}


/**
 * @brief Cleans up the SSL context and deallocates OpenSSL resources.
 *
 * This function frees the SSL context and performs OpenSSL cleanup operations, such as
 * deallocating error strings and EVP cleanup.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: SSL context and OpenSSL resources were cleaned up successfully.
 */
TcpStatus tcp_ssl_cleanup(void) {
    mutex_lock(&tcp_mutex);

    if (ssl_ctx != NULL) {
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL;
        TCP_LOG("[tcp_ssl_cleanup] SSL context cleaned up successfully.\n");
    }

    EVP_cleanup();
    ERR_free_strings();
    mutex_unlock(&tcp_mutex);

    return TCP_SUCCESS;
}


/**
 * @brief Initiates an SSL/TLS connection to a remote host.
 *
 * This function establishes a secure SSL/TLS connection to a remote host on the given socket.
 * It sets up the necessary file descriptor for the SSL object, performs the handshake, and sets
 * the Server Name Indication (SNI) hostname for the connection.
 *
 * @param socket The TCP socket through which the SSL/TLS connection is to be established.
 * @param host The remote hostname to connect to, used for SNI and resolving the server.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: SSL connection established successfully.
 * - `TCP_ERR_SETUP`: SSL context is not initialized (call `tcp_ssl_init` first).
 * - `TCP_ERR_RESOLVE`: The host parameter is null or invalid.
 * - `TCP_ERR_SSL`: Failed to create SSL object or set up file descriptor/SNI.
 * - `TCP_ERR_SSL_HANDSHAKE`: The SSL handshake failed.
 */
TcpStatus tcp_ssl_connect(TcpSocket socket, const char* host) {
    mutex_lock(&tcp_mutex);
    if (ssl_ctx == NULL) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_connect] SSL context is not initialized. Call tcp_ssl_init first.\n");
        return TCP_ERR_SETUP; 
    }
    if (host == NULL || host[0] == '\0') {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_connect] Host parameter is null or empty.\n");
        return TCP_ERR_RESOLVE; 
    }

    SSL* ssl = SSL_new(ssl_ctx);
    if (ssl == NULL) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_connect] Failed to create SSL object: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return TCP_ERR_SSL; 
    }

    if (SSL_set_fd(ssl, socket) == 0) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_connect] Failed to set file descriptor for SSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
        SSL_free(ssl); 

        return TCP_ERR_SSL; 
    }

    if (SSL_set_tlsext_host_name(ssl, host) == 0) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_connect] Failed to set SNI Hostname: %s\n", ERR_error_string(ERR_get_error(), NULL));
        SSL_free(ssl);

        return TCP_ERR_SSL;
    }

    /* Bind the expected identity so the certificate is checked AGAINST `host`,
     * not merely for a valid chain. For an IP literal use the IP SAN; otherwise
     * the DNS name (SAN/CN). With SSL_VERIFY_PEER (tcp_ssl_set_verify) a mismatch
     * aborts the handshake; otherwise it shows up in tcp_ssl_get_verify_result.
     * This closes the "any valid cert for any domain is accepted" MITM hole. */
    {
        unsigned char ip_bytes[16];
        if (inet_pton(AF_INET, host, ip_bytes) == 1 || inet_pton(AF_INET6, host, ip_bytes) == 1) {
            X509_VERIFY_PARAM_set1_ip_asc(SSL_get0_param(ssl), host);
        }
        else {
            SSL_set1_host(ssl, host);
        }
    }

    if (SSL_connect(ssl) != 1) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_connect] SSL handshake failed: %s\n", ERR_error_string(ERR_get_error(), NULL));
        SSL_free(ssl); 

        return TCP_ERR_SSL_HANDSHAKE; 
    }

    // Successfully established SSL connection, map the SSL object with the socket
    SocketSSLMapping* mapping = get_ssl_mapping(socket, true);
    if (!mapping) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_connect] Failed to map SSL object with socket.\n");
        SSL_free(ssl); 

        return TCP_ERR_SSL; 
    }

    mapping->ssl = ssl;
    mutex_unlock(&tcp_mutex);

    TCP_LOG("[tcp_ssl_connect] Successfully established SSL connection to %s.\n", host);
    return TCP_SUCCESS;
}


/**
 * @brief Accepts an SSL/TLS connection on a TCP socket.
 *
 * This function performs the SSL/TLS handshake on the specified TCP socket. It creates an SSL object,
 * associates it with the socket, and waits for a TLS/SSL client to initiate the handshake.
 *
 * @param socket The TCP socket that is awaiting an SSL/TLS connection.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: SSL handshake completed successfully.
 * - `TCP_ERR_SETUP`: SSL context is not initialized (call `tcp_ssl_init` first).
 * - `TCP_ERR_SSL`: Failed to create an SSL object or associate it with the socket.
 * - `TCP_ERR_SSL_HANDSHAKE`: SSL/TLS handshake failed.
 */
TcpStatus tcp_ssl_accept(TcpSocket socket) {
    mutex_lock(&tcp_mutex);
    if (ssl_ctx == NULL) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_accept] SSL context is not initialized. Call tcp_ssl_init first.\n");
        return TCP_ERR_SETUP; 
    }

    SSL* ssl = SSL_new(ssl_ctx);
    if (ssl == NULL) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_accept] Failed to create SSL object: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return TCP_ERR_SSL; 
    }
    if (SSL_set_fd(ssl, socket) == 0) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_accept] Failed to set file descriptor for SSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
        SSL_free(ssl); 

        return TCP_ERR_SSL; 
    }

    // Wait for a TLS/SSL client to initiate the TLS/SSL handshake
    int acceptResult = SSL_accept(ssl);
    if (acceptResult <= 0) {
        
        int sslError = SSL_get_error(ssl, acceptResult);
        TCP_LOG("[tcp_ssl_accept] SSL_accept failed with SSL error: %d\n", sslError);
        if (sslError == SSL_ERROR_SYSCALL) {
            unsigned long err;
            while ((err = ERR_get_error()) != 0) {
                TCP_LOG("[tcp_ssl_accept] OpenSSL Error: %s\n", ERR_error_string(err, NULL));
            }
            if (errno != 0) { 
                TCP_LOG("[tcp_ssl_accept] Syscall error: %s\n", strerror(errno));
            }
        }
        SSL_free(ssl);
        mutex_unlock(&tcp_mutex);
        return TCP_ERR_SSL_HANDSHAKE;
    }

    // Handshake was successful, store the SSL object with the socket for further operations
    SocketSSLMapping* mapping = get_ssl_mapping(socket, true);
    if (!mapping) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("Failed to map SSL object with socket.\n");
        SSL_free(ssl); 

        return TCP_ERR_SSL; 
    }
    mapping->ssl = ssl;
    mutex_unlock(&tcp_mutex);

    TCP_LOG("SSL handshake completed successfully.\n");
    return TCP_SUCCESS;
}


/**
 * @brief Closes an SSL-encrypted TCP connection.
 *
 * This function initiates the SSL/TLS shutdown sequence for the specified socket, frees the associated SSL object,
 * and closes the underlying TCP socket. Even if the SSL shutdown fails, the SSL object is freed and the socket is closed.
 *
 * @param socket The TCP socket that has an active SSL connection.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: SSL shutdown and socket closure were successful.
 * - `TCP_ERR_NO_SSL`: No SSL object was found for the socket, but the socket was closed normally.
 * - Other TcpStatus codes may be returned from the `tcp_close` function if the socket fails to close.
 */
TcpStatus tcp_ssl_close(TcpSocket socket) {
    mutex_lock(&tcp_mutex);
    SSL* ssl = tcp_get_ssl(socket);

    if (!ssl) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_close] No SSL object associated with the socket. Closing socket without SSL shutdown.\n");
        tcp_close(socket);

        return TCP_ERR_NO_SSL; 
    }

    int shutdownResult = SSL_shutdown(ssl);
    if (shutdownResult == 0) {
        shutdownResult = SSL_shutdown(ssl);
        if (shutdownResult != 1) {
            TCP_LOG("[tcp_ssl_close] SSL shutdown did not complete cleanly: %s\n", ERR_error_string(SSL_get_error(ssl, shutdownResult), NULL));
        }
    } 
    else if (shutdownResult < 0) {
        TCP_LOG("[tcp_ssl_close] SSL shutdown failed: %s\n", ERR_error_string(SSL_get_error(ssl, shutdownResult), NULL));
    }

    SSL_free(ssl);
    tcp_set_ssl(socket, NULL);

    TcpStatus closeStatus = tcp_close(socket);
    if (closeStatus != TCP_SUCCESS) {
        TCP_LOG("[tcp_ssl_close] Socket close failed.\n");
        mutex_unlock(&tcp_mutex);
        return closeStatus;
    }
    mutex_unlock(&tcp_mutex);

    TCP_LOG("[tcp_ssl_close] SSL connection and socket closed successfully.\n");
    return TCP_SUCCESS;
}


/**
 * @brief Sends data over an SSL connection.
 *
 * This function writes data to an SSL-encrypted connection associated with the specified socket.
 * It handles various SSL errors and returns the appropriate TcpStatus code.
 *
 * @param socket The TCP socket through which the data is sent.
 * @param buf Pointer to the buffer containing the data to be sent.
 * @param len The size of the data (in bytes) to be sent.
 * @param sent Pointer to a size_t variable where the number of bytes sent will be stored (optional).
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: Data was successfully sent, or the operation needs to be retried.
 * - `TCP_ERR_SEND`: There was an error during the send operation.
 * - `TCP_ERR_NO_SSL`: The SSL context for the socket was not found.
 * - `TCP_ERR_CLOSE`: The connection was closed by the peer.
 */
TcpStatus tcp_ssl_send(TcpSocket socket, const void* buf, size_t len, size_t* sent) {
    mutex_lock(&tcp_mutex);

    if (!buf || len == 0) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_send] Invalid buffer or length for SSL send.\n");
        return TCP_ERR_SEND; 
    }

    SSL* ssl = tcp_get_ssl(socket);
    if (!ssl) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_send] No SSL object associated with the socket. Cannot send data.\n");
        return TCP_ERR_NO_SSL; 
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
                    TCP_LOG("[tcp_ssl_send] SSL_write needs to be called again.\n");
                    
                    if (sent) {
                        *sent = totalSent;
                    }
                    mutex_unlock(&tcp_mutex);
                    return TCP_SUCCESS; // Not an error, but the send needs to be retried
                case SSL_ERROR_ZERO_RETURN:
                    TCP_LOG("[tcp_ssl_send] SSL connection closed by peer.\n");
                    mutex_unlock(&tcp_mutex);
                    return TCP_ERR_CLOSE;

                case SSL_ERROR_SYSCALL:
                    TCP_LOG("[tcp_ssl_send] SSL write syscall error: %s\n", strerror(errno));
                    mutex_unlock(&tcp_mutex);
                    return TCP_ERR_SEND;

                default:
                    TCP_LOG("[tcp_ssl_send] SSL write error: %s\n", ERR_error_string(ERR_get_error(), NULL));
                    mutex_unlock(&tcp_mutex);
                    return TCP_ERR_SEND;
            }
        }

        totalSent += result;
        dataPtr += result;
        len -= result;
    }

    if (sent) {
        *sent = totalSent;
    }
    mutex_unlock(&tcp_mutex);

    TCP_LOG("Sent %d bytes over SSL.\n", totalSent);
    return TCP_SUCCESS;
}


/**
 * @brief Receives data over an SSL connection.
 *
 * This function reads data from an SSL-encrypted connection associated with the specified socket.
 * The function handles various SSL errors and returns the appropriate TcpStatus code.
 *
 * @param socket The TCP socket from which data is received.
 * @param buf Pointer to the buffer where the received data will be stored.
 * @param len The size of the buffer (in bytes).
 * @param received Pointer to a size_t variable where the number of bytes received will be stored (optional).
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: Data was successfully received, or the operation needs to be retried.
 * - `TCP_ERR_RECV`: There was an error during the receive operation.
 * - `TCP_ERR_NO_SSL`: The SSL context for the socket was not found.
 * - `TCP_ERR_CLOSE`: The connection was closed by the peer.
 */
TcpStatus tcp_ssl_recv(TcpSocket socket, void* buf, size_t len, size_t* received) {
    mutex_lock(&tcp_mutex);
    if (!buf || len == 0) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_recv] Invalid buffer or length for SSL receive.\n");
        return TCP_ERR_RECV; 
    }

    SSL* ssl = tcp_get_ssl(socket);
    if (!ssl) {
        mutex_unlock(&tcp_mutex);
        TCP_LOG("[tcp_ssl_recv] No SSL object associated with the socket. Cannot receive data.\n");
        return TCP_ERR_NO_SSL; 
    }

    int result = SSL_read(ssl, buf, len);
    if (result > 0) {
        if (received) {
            *received = result;
        }
        
        TCP_LOG("[tcp_ssl_recv] Received %d bytes over SSL.\n", result);
        mutex_unlock(&tcp_mutex);
        return TCP_SUCCESS;
    } 
    else {
        int sslError = SSL_get_error(ssl, result);
        switch (sslError) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                // The operation did not complete; the same call can be retried later
                if (received) {
                    *received = 0;
                }
                
                TCP_LOG("[tcp_ssl_recv] SSL_read needs to be called again.\n");
                mutex_unlock(&tcp_mutex);
                return TCP_SUCCESS; // Not an error, but the read needs to be retried

            case SSL_ERROR_ZERO_RETURN:
                // The connection was closed
                TCP_LOG("[tcp_ssl_recv] SSL connection closed by peer.\n");
                mutex_unlock(&tcp_mutex);
                return TCP_ERR_CLOSE;

            case SSL_ERROR_SYSCALL: {
                if (ERR_peek_error() == 0) { // No specific SSL error
                    if (result == 0 || errno == 0) {
                        TCP_LOG("[tcp_ssl_recv] SSL connection closed by peer or EOF reached.\n");
                        SSL_free(ssl);
                        tcp_set_ssl(socket, NULL); // Ensure to clear the SSL mapping
                        tcp_close(socket); // Close the socket as the connection is done
                        mutex_unlock(&tcp_mutex);
                        return TCP_ERR_CLOSE;
                    } 
                    else {
                        TCP_LOG("[tcp_ssl_recv] SSL syscall error: %s\n", strerror(errno));
                    }
                }
                
                TCP_LOG("[tcp_ssl_recv] SSL read syscall error: %s\n", strerror(errno));
                mutex_unlock(&tcp_mutex);
                return TCP_ERR_RECV;
            }
            default:
                TCP_LOG("[tcp_ssl_recv] SSL read error: %s\n", ERR_error_string(ERR_get_error(), NULL));
                mutex_unlock(&tcp_mutex);
                return TCP_ERR_RECV;
        }
    }
    mutex_unlock(&tcp_mutex);
}


/**
 * @brief Retrieves the connection quality, including round-trip time (RTT) and its variance, for a given TCP socket.
 *
 * This function extracts TCP connection quality metrics such as the round-trip time (RTT) and its variance for the provided socket. 
 * On Linux, it uses the `TCP_INFO` socket option to retrieve this information.
 * On Windows or unsupported platforms, the function returns `TCP_ERR_UNSUPPORTED` since no direct method is available.
 *
 * @param socket The TCP socket for which connection quality metrics are retrieved.
 * @param rtt Pointer to a float where the round-trip time (RTT) in milliseconds will be stored.
 * @param variance Pointer to a float where the variance of the RTT (in milliseconds) will be stored.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: Successfully retrieved RTT and variance.
 * - `TCP_ERR_GENERIC`: Failed to retrieve TCP connection quality (e.g., due to a socket error on Linux).
 * - `TCP_ERR_UNSUPPORTED`: The platform does not support direct RTT measurement (Windows or other unsupported platforms).
 */
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
            TCP_LOG("[tcp_get_connection_quality] Error: Failed to get TCP connection quality.\n");
            return TCP_ERR_GENERIC;
        }
    #elif defined(_WIN32)
    // Windows does not provide a direct way to get RTT and its variance via socket options.
        (void)socket;
        (void)rtt;
        (void)variance;
        
        TCP_LOG("[tcp_get_connection_quality] Error: Direct RTT measurement not supported on Windows.\n");
        return TCP_ERR_UNSUPPORTED;

    #else
        TCP_LOG("[tcp_get_connection_quality] Error: Your platform does not support direct RTT measurement via socket options.\n");
        return TCP_ERR_UNSUPPORTED;

    #endif
}


/**
 * @brief Sends data asynchronously through a non-blocking TCP socket.
 * 
 * This function attempts to send data to a non-blocking TCP socket. 
 * Ensure the socket is set to non-blocking mode using `tcp_set_non_blocking` before calling this function.
 * If the send operation would block, it returns `TCP_ERR_WOULD_BLOCK` without blocking the application.
 *
 * @param socket The non-blocking TCP socket through which data will be sent.
 * @param buf Pointer to the buffer containing the data to be sent.
 * @param len The number of bytes to send from the buffer.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: Data was sent successfully.
 * - `TCP_ERR_SEND`: An error occurred while sending data.
 * - `TCP_ERR_WOULD_BLOCK`: The connection is currently blocked, and the send operation would block.
 */
TcpStatus tcp_async_send(TcpSocket socket, const void* buf, size_t len) {

    #if defined(_WIN32) || defined(_WIN64)
    int result = send(socket, (const char*)buf, (int)len, 0);

    if (result == SOCKET_ERROR) {
        int lastError = WSAGetLastError();

        if (lastError == WSAEWOULDBLOCK) {
            TCP_LOG("[tcp_async_send] Info: send would block.\n");
            return TCP_ERR_WOULD_BLOCK;
        }
        TCP_LOG("[tcp_async_send] Error: send failed (WSA error %d).\n", lastError);

        return TCP_ERR_SEND;
    }
    #else
    ssize_t result = send(socket, buf, len, 0);

    if (result < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            TCP_LOG("[tcp_async_send] Info: send would block.\n");
            return TCP_ERR_WOULD_BLOCK;
        }
        TCP_LOG("[tcp_async_send] Error: send failed (errno %d).\n", errno);
        return TCP_ERR_SEND;
    }
    #endif

    return TCP_SUCCESS;
}


/**
 * @brief Receives data asynchronously from a non-blocking TCP socket.
 *
 * This function attempts to receive data from a non-blocking TCP socket. 
 * Ensure the socket is set to non-blocking mode using `tcp_set_non_blocking` before calling this function.
 * If no data is available, it returns `TCP_ERR_WOULD_BLOCK` without blocking the application.
 *
 * @param socket The non-blocking TCP socket from which to receive data.
 * @param buf Pointer to the buffer where the received data will be stored.
 * @param len The maximum number of bytes to read into the buffer.
 *
 * @return TcpStatus
 * - `TCP_SUCCESS`: Data was received successfully.
 * - `TCP_ERR_RECV`: An error occurred while receiving data.
 * - `TCP_ERR_WOULD_BLOCK`: No data is available to read; this is expected for non-blocking operations.
 */
TcpStatus tcp_async_recv(TcpSocket socket, void* buf, size_t len) {
    #if defined(_WIN32) || defined(_WIN64)
    int result = recv(socket, (char*)buf, (int)len, 0);

    if (result == SOCKET_ERROR) {
        int lastError = WSAGetLastError();

        if (lastError == WSAEWOULDBLOCK) {
            TCP_LOG("[tcp_async_recv] Info: no data available (would block).\n");
            return TCP_ERR_WOULD_BLOCK;
        }
        TCP_LOG("[tcp_async_recv] Error: recv failed (WSA error %d).\n", lastError);

        return TCP_ERR_RECV;
    }
    #else
    ssize_t result = recv(socket, buf, len, 0);
    if (result < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            TCP_LOG("[tcp_async_recv] Info: no data available (would block).\n");
            return TCP_ERR_WOULD_BLOCK;
        }
        TCP_LOG("[tcp_async_recv] Error: recv failed (errno %d).\n", errno);
        return TCP_ERR_RECV;
    }
    #endif
    if (result == 0) {
        // Connection has been gracefully closed by the peer
        return TCP_ERR_RECV;
    }

    return TCP_SUCCESS;
}


static TcpStatus get_sock_address(int (*fn)(TcpSocket, struct sockaddr*, socklen_t*), TcpSocket socket, char* address, size_t address_len, unsigned short* port) {
    // Validate every output pointer — the previous code would dereference.
    if (!address || address_len == 0) {
        TCP_LOG("[get_sock_address] Error: address buffer is NULL or zero-length.\n");
        return TCP_ERR_GENERIC;
    }

    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    if (fn(socket, (struct sockaddr*)&addr, &len) != 0) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info);

        TCP_LOG("[get_sock_address] Error: %s in get_sock_address.\n", status_info.message);
        return TCP_ERR_GENERIC;
    }

    if (addr.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in*)&addr;

        if (inet_ntop(AF_INET, &s->sin_addr, address, address_len) == NULL) {
            TCP_LOG("[get_sock_address] Error: inet_ntop() failed for IPv4 address.\n");
            return TCP_ERR_GENERIC;
        }
        if (port) {
            *port = ntohs(s->sin_port);
        }
    }
    else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;

        if (inet_ntop(AF_INET6, &s->sin6_addr, address, address_len) == NULL) {
            TCP_LOG("[get_sock_address] Error: inet_ntop() failed for IPv6 address.\n");
            return TCP_ERR_GENERIC;
        }
        if (port) {
            *port = ntohs(s->sin6_port);
        }
    }
    else {
        TCP_LOG("[get_sock_address] Error: Unknown socket family.\n");
        return TCP_ERR_GENERIC;
    }

    TCP_LOG("[get_sock_address] Retrieved address: %s, port: %u\n", address, *port);
    return TCP_SUCCESS;
}


/**
 * @brief Human-readable label for a `TcpStatus` code.
 *
 * Returns a static, immutable string suitable for printing or logging
 * (`"TCP_SUCCESS"`, `"TCP_ERR_BIND"`, …). Useful when you want
 * structured logs without dragging in the full `TcpStatusInfo` from
 * `tcp_get_last_error`. Unknown codes return `"TCP_UNKNOWN_CODE"`.
 *
 * @param code Status code returned by any `tcp_*` function.
 * @return Static string. Never NULL.
 */
const char* tcp_status_to_string(TcpStatus code) {
    switch (code) {
        case TCP_SUCCESS:           
            return "TCP_SUCCESS";
        case TCP_ERR_SOCKET:        
            return "TCP_ERR_SOCKET";
        case TCP_ERR_BIND:          
            return "TCP_ERR_BIND";
        case TCP_ERR_LISTEN:        
            return "TCP_ERR_LISTEN";
        case TCP_ERR_ACCEPT:        
            return "TCP_ERR_ACCEPT";
        case TCP_ERR_CONNECT:       
            return "TCP_ERR_CONNECT";
        case TCP_ERR_SEND:          
            return "TCP_ERR_SEND";
        case TCP_ERR_RECV:          
            return "TCP_ERR_RECV";
        case TCP_ERR_CLOSE:         
            return "TCP_ERR_CLOSE";
        case TCP_ERR_SETUP:         
            return "TCP_ERR_SETUP";
        case TCP_ERR_RESOLVE:       
            return "TCP_ERR_RESOLVE";
        case TCP_ERR_GENERIC:       
            return "TCP_ERR_GENERIC";
        case TCP_ERR_SSL:           
            return "TCP_ERR_SSL";
        case TCP_ERR_SSL_HANDSHAKE: 
            return "TCP_ERR_SSL_HANDSHAKE";
        case TCP_ERR_TRY_AGAIN:     
            return "TCP_ERR_TRY_AGAIN";
        case TCP_ERR_NO_SSL:        
            return "TCP_ERR_NO_SSL";
        case TCP_ERR_UNSUPPORTED:   
            return "TCP_ERR_UNSUPPORTED";
        case TCP_ERR_WOULD_BLOCK:   
            return "TCP_ERR_WOULD_BLOCK";
        default:                    
            return "TCP_UNKNOWN_CODE";
    }
}


/**
 * @brief Enable or disable `SO_KEEPALIVE` on a TCP socket.
 *
 * When enabled, the kernel periodically probes idle connections and
 * tears them down if the peer stops responding. Without this, a
 * half-open connection (peer crashed, network cable yanked) can sit
 * idle indefinitely with no notification — `recv()` just blocks
 * forever. Recommended for any long-lived service connection.
 *
 * On Linux/BSD the probe interval, count, and idle time can be tuned
 * via additional `TCP_KEEP*` socket options not exposed here; this
 * helper just flips the master switch.
 *
 * @param socket Valid TCP socket.
 * @param enable `true` to turn keep-alive on, `false` to turn it off.
 * @return `TCP_SUCCESS`, `TCP_ERR_GENERIC` on setsockopt failure.
 */
TcpStatus tcp_set_keep_alive(TcpSocket socket, bool enable) {
    int opt = enable ? 1 : 0;

    if (setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&opt, sizeof(opt)) != 0) {
        TCP_LOG("[tcp_set_keep_alive] setsockopt failed");
        return TCP_ERR_GENERIC;
    }
    return TCP_SUCCESS;
}


/**
 * @brief Enable or disable Nagle's algorithm (`TCP_NODELAY`).
 *
 * Nagle's algorithm coalesces small writes to reduce packet overhead,
 * but adds latency to interactive request/response workloads. Setting
 * `enable = true` here disables Nagle (`TCP_NODELAY = 1`) — small
 * messages are sent immediately. Crucial for RPC, gaming, live-trading
 * traffic, and other low-latency protocols.
 *
 * @param socket Valid TCP socket.
 * @param enable `true` disables Nagle (low latency); `false` re-enables
 *               it (default kernel behavior).
 * @return `TCP_SUCCESS`, `TCP_ERR_GENERIC` on setsockopt failure.
 */
TcpStatus tcp_set_nodelay(TcpSocket socket, bool enable) {
    int opt = enable ? 1 : 0;

    if (setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&opt, sizeof(opt)) != 0) {
        TCP_LOG("[tcp_set_nodelay] setsockopt failed");
        return TCP_ERR_GENERIC;
    }

    return TCP_SUCCESS;
}


/**
 * @brief Configure `SO_LINGER` — close()-time disposal of unsent data.
 *
 * - `enable = false`: default behavior. `close()` returns immediately;
 *   the kernel transmits any queued data in the background.
 * - `enable = true, seconds > 0`: `close()` blocks up to `seconds`
 *   waiting for queued data to be ACK'd, then sends a FIN.
 * - `enable = true, seconds = 0`: abortive close. `close()` immediately
 *   discards queued data and sends RST. Useful when the peer has
 *   misbehaved and you want the connection torn down without the
 *   half-second TIME_WAIT lingering.
 *
 * @param socket  Valid TCP socket.
 * @param enable  Whether to enable the linger behavior.
 * @param seconds Linger timeout when `enable = true` (0 = abortive).
 * @return `TCP_SUCCESS`, `TCP_ERR_GENERIC` on setsockopt failure.
 */
TcpStatus tcp_set_linger(TcpSocket socket, bool enable, int seconds) {
    struct linger lg;
    lg.l_onoff  = enable ? 1 : 0;
    lg.l_linger = (enable && seconds > 0) ? seconds : 0;

    if (setsockopt(socket, SOL_SOCKET, SO_LINGER, (const char*)&lg, sizeof(lg)) != 0) {
        TCP_LOG("[tcp_set_linger] setsockopt failed");
        return TCP_ERR_GENERIC;
    }

    return TCP_SUCCESS;
}


/**
 * @brief Tune the kernel send and/or receive buffer sizes on a TCP socket.
 *
 * Adjusts `SO_SNDBUF` and `SO_RCVBUF`. Pass `0` for either argument to
 * leave that direction unchanged. Larger buffers help bulk-transfer
 * throughput on high-bandwidth-delay paths; smaller buffers reduce
 * memory pressure on busy servers. The kernel typically doubles the
 * requested value internally and clamps to a system maximum.
 *
 * @param socket     Valid TCP socket.
 * @param send_bytes Requested `SO_SNDBUF` in bytes, or 0 to skip.
 * @param recv_bytes Requested `SO_RCVBUF` in bytes, or 0 to skip.
 * @return `TCP_SUCCESS`, `TCP_ERR_GENERIC` on either setsockopt failure.
 */
TcpStatus tcp_set_buffer_size(TcpSocket socket, size_t send_bytes, size_t recv_bytes) {
    if (send_bytes > 0) {
        int v = (int)send_bytes;
        if (setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (const char*)&v, sizeof(v)) != 0) {
            TCP_LOG("[tcp_set_buffer_size] SO_SNDBUF setsockopt failed");
            return TCP_ERR_GENERIC;
        }
    }
    if (recv_bytes > 0) {
        int v = (int)recv_bytes;
        if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (const char*)&v, sizeof(v)) != 0) {
            TCP_LOG("[tcp_set_buffer_size] SO_RCVBUF setsockopt failed");
            return TCP_ERR_GENERIC;
        }
    }

    return TCP_SUCCESS;
}


/**
 * @brief Send EXACTLY `len` bytes, looping over partial writes.
 *
 * `tcp_send` (and the underlying `send()`) is allowed to return having
 * written fewer bytes than requested — the caller is normally required
 * to keep calling it until everything is flushed. This wrapper does
 * that loop for you. Returns the first failure code (typically
 * `TCP_ERR_SEND`) if the underlying send ever fails mid-loop.
 *
 * Treats zero-length input as a no-op success.
 *
 * @param socket Connected TCP socket.
 * @param buf    Bytes to send. NULL with `len > 0` is rejected.
 * @param len    Total bytes to send.
 * @return `TCP_SUCCESS` when all bytes are sent; first error from
 *         `tcp_send` otherwise.
 */
TcpStatus tcp_send_all(TcpSocket socket, const void* buf, size_t len) {
    if (len == 0) {
        return TCP_SUCCESS;
    }
    if (!buf) {
        return TCP_ERR_SEND;
    }

    const char* p = (const char*)buf;
    size_t remaining = len;

    while (remaining > 0) {
        size_t sent = 0;
        TcpStatus rc = tcp_send(socket, p, remaining, &sent);

        if (rc != TCP_SUCCESS) {
            TCP_LOG("[tcp_send_all] tcp_send failed after %zu/%zu bytes",
                    len - remaining, len);
            return rc;
        }
        if (sent == 0) {
            /* No bytes accepted and no error reported */
            TCP_LOG("[tcp_send_all] tcp_send returned 0 with no error");
            return TCP_ERR_SEND;
        }
        p += sent;
        remaining -= sent;
    }
    return TCP_SUCCESS;
}


/**
 * @brief Receive EXACTLY `len` bytes, looping over partial reads.
 *
 * `tcp_recv` (and the underlying `recv()`) may return fewer bytes than
 * requested; for fixed-size frames or length-prefixed protocols the
 * caller must loop until the full frame arrives. This wrapper does
 * that. A clean peer close mid-stream (recv returns 0 bytes) is
 * reported as `TCP_ERR_RECV` — an incomplete frame is a protocol
 * error, not a happy-path EOF.
 *
 * Treats zero-length input as a no-op success.
 *
 * @param socket Connected TCP socket.
 * @param buf    Destination buffer of at least `len` bytes.
 * @param len    Total bytes to read.
 * @return `TCP_SUCCESS` when all bytes are read; first error from
 *         `tcp_recv` otherwise.
 */
TcpStatus tcp_recv_all(TcpSocket socket, void* buf, size_t len) {
    if (len == 0) {
        return TCP_SUCCESS;
    }
    if (!buf) { 
        return TCP_ERR_RECV;
    }

    char* p = (char*)buf;
    size_t remaining = len;

    while (remaining > 0) {
        size_t got = 0;
        TcpStatus rc = tcp_recv(socket, p, remaining, &got);

        if (rc != TCP_SUCCESS) {
            TCP_LOG("[tcp_recv_all] tcp_recv failed after %zu/%zu bytes", len - remaining, len);
            return rc;
        }
        if (got == 0) {
            /* Peer closed mid-frame. */
            TCP_LOG("[tcp_recv_all] EOF after %zu/%zu bytes", len - remaining, len);
            return TCP_ERR_RECV;
        }

        p += got;
        remaining -= got;
    }
    return TCP_SUCCESS;
}


/* Wait until `socket` is ready for read (want_write=false) or write
 * (want_write=true), or until `timeout_ms` elapses. timeout_ms < 0 blocks
 * indefinitely; 0 polls. Returns TCP_SUCCESS (ready), TCP_ERR_WOULD_BLOCK
 * (timed out), or TCP_ERR_GENERIC (select failed). */
static TcpStatus tcp_wait_ready(TcpSocket socket, long timeout_ms, bool want_write) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(socket, &fds);

    struct timeval tv;
    struct timeval* tvp = NULL;
    if (timeout_ms >= 0) {
        tv.tv_sec  = (long)(timeout_ms / 1000);
        tv.tv_usec = (long)((timeout_ms % 1000) * 1000);
        tvp = &tv;
    }

#if defined(_WIN32) || defined(_WIN64)
    int nfds = 0;                       /* ignored by Winsock */
#else
    int nfds = (int)socket + 1;
#endif

    int rc = want_write
        ? select(nfds, NULL, &fds, NULL, tvp)
        : select(nfds, &fds, NULL, NULL, tvp);

    if (rc < 0) {
        return TCP_ERR_GENERIC;
    }
    if (rc == 0) {
        return TCP_ERR_WOULD_BLOCK;     /* timed out */
    }
    return TCP_SUCCESS;
}


/**
 * @brief Wait until the socket has data ready to read, or time out.
 *
 * Blocks in `select()` until the socket is readable (data has arrived, or the
 * peer closed so a 0-byte read is pending) or @p timeout_ms elapses. The
 * building block for poll-with-timeout receive loops without making the whole
 * socket non-blocking. `timeout_ms < 0` waits indefinitely; `0` polls.
 *
 * @param socket     Connected TCP socket.
 * @param timeout_ms Timeout in milliseconds (see above).
 * @return `TCP_SUCCESS` if readable, `TCP_ERR_WOULD_BLOCK` on timeout, or
 *         `TCP_ERR_GENERIC` on a select() error.
 */
TcpStatus tcp_wait_readable(TcpSocket socket, long timeout_ms) {
    return tcp_wait_ready(socket, timeout_ms, false);
}


/**
 * @brief Wait until the socket can accept a write, or time out.
 *
 * Blocks in `select()` until the socket is writable or @p timeout_ms elapses.
 * Most useful to (a) bound a blocking `tcp_send` against a slow/stalled peer,
 * and (b) detect completion of a non-blocking `connect` (the socket becomes
 * writable when the handshake finishes — then check `tcp_get_socket_error`).
 * `timeout_ms < 0` waits indefinitely; `0` polls.
 *
 * @param socket     TCP socket.
 * @param timeout_ms Timeout in milliseconds (see above).
 * @return `TCP_SUCCESS` if writable, `TCP_ERR_WOULD_BLOCK` on timeout, or
 *         `TCP_ERR_GENERIC` on a select() error.
 */
TcpStatus tcp_wait_writable(TcpSocket socket, long timeout_ms) {
    return tcp_wait_ready(socket, timeout_ms, true);
}


/**
 * @brief Report how many bytes are immediately available to read.
 *
 * Queries the socket via `FIONREAD` — the number of bytes that can be read
 * without blocking. Useful to size a receive buffer before `tcp_recv`. Writes
 * `0` when the receive queue is empty (which, on a readable socket, indicates
 * the peer has closed).
 *
 * @param socket    Connected TCP socket.
 * @param available Receives the byte count. Must not be NULL.
 * @return `TCP_SUCCESS`, or `TCP_ERR_GENERIC` on a NULL output or query error.
 */
TcpStatus tcp_bytes_available(TcpSocket socket, size_t* available) {
    if (!available) {
        return TCP_ERR_GENERIC;
    }
    *available = 0;
#if defined(_WIN32) || defined(_WIN64)
    u_long n = 0;
    if (ioctlsocket(socket, FIONREAD, &n) != 0) {
        return TCP_ERR_GENERIC;
    }
    *available = (size_t)n;
#else
    int n = 0;
    if (ioctl(socket, FIONREAD, &n) != 0) {
        return TCP_ERR_GENERIC;
    }
    *available = (n > 0) ? (size_t)n : 0;
#endif
    return TCP_SUCCESS;
}


/**
 * @brief Retrieve (and clear) the pending socket error via `SO_ERROR`.
 *
 * Reads the `SO_ERROR` socket option, which holds the OS error code of the
 * last asynchronous failure (and is reset to 0 by this read). The canonical
 * way to learn the outcome of a non-blocking `connect`: once the socket is
 * writable, a value of 0 means the connection succeeded, while a non-zero
 * value (e.g. `ECONNREFUSED`/`WSAECONNREFUSED`) is the failure reason.
 *
 * @param socket     TCP socket.
 * @param error_code Receives the OS error code (0 = no pending error). Must
 *                   not be NULL.
 * @return `TCP_SUCCESS` if the option was read (regardless of its value), or
 *         `TCP_ERR_GENERIC` on a NULL output or `getsockopt` failure.
 */
TcpStatus tcp_get_socket_error(TcpSocket socket, int* error_code) {
    if (!error_code) {
        return TCP_ERR_GENERIC;
    }
    int err = 0;
    socklen_t len = sizeof(err);
    if (getsockopt(socket, SOL_SOCKET, SO_ERROR, (char*)&err, &len) != 0) {
        return TCP_ERR_GENERIC;
    }
    *error_code = err;
    return TCP_SUCCESS;
}


/**
 * @brief Connect with a bounded timeout, so a connect never hangs.
 *
 * A drop-in alternative to `tcp_connect` that refuses to block longer than
 * @p timeout_ms. It resolves @p host (IPv4/IPv6), then performs the connect in
 * non-blocking mode: it issues `connect`, waits with `tcp_wait_writable`, and
 * confirms success via `tcp_get_socket_error`. On return the socket is left in
 * blocking mode (matching `tcp_connect`). `timeout_ms < 0` waits indefinitely.
 *
 * Use this in production for both clients and server-to-server calls — a plain
 * blocking `connect` to an unreachable host can stall for the OS default (often
 * minutes) and wedge a request thread.
 *
 * @param socket     A fresh socket from `tcp_socket_create`.
 * @param host       Remote host (numeric or DNS name). NULL/empty → resolve error.
 * @param port       Remote port.
 * @param timeout_ms Maximum time to wait for the handshake, in milliseconds.
 * @return `TCP_SUCCESS` on a completed connection; `TCP_ERR_RESOLVE` if the
 *         host can't be resolved; `TCP_ERR_WOULD_BLOCK` if the timeout elapses
 *         before the handshake completes; `TCP_ERR_CONNECT` if every resolved
 *         address is refused/unreachable; `TCP_ERR_GENERIC` on a socket-mode
 *         or select() error.
 */
TcpStatus tcp_connect_timeout(TcpSocket socket, const char* host, unsigned short port, long timeout_ms) {
    if (!host || !*host) {
        return TCP_ERR_RESOLVE;
    }

    struct addrinfo hints, *res = NULL, *p = NULL;
    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%u", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, port_str, &hints, &res) != 0) {
        return TCP_ERR_RESOLVE;
    }

    if (tcp_set_non_blocking(socket, true) != TCP_SUCCESS) {
        freeaddrinfo(res);
        return TCP_ERR_GENERIC;
    }

    TcpStatus result = TCP_ERR_CONNECT;
    for (p = res; p; p = p->ai_next) {
        int rc = connect(socket, p->ai_addr, (socklen_t)p->ai_addrlen);
        if (rc == 0) {
            result = TCP_SUCCESS;       /* connected immediately (e.g. loopback) */
            break;
        }
#if defined(_WIN32) || defined(_WIN64)
        bool in_progress = (WSAGetLastError() == WSAEWOULDBLOCK);
#else
        bool in_progress = (errno == EINPROGRESS);
#endif
        if (!in_progress) {
            continue;                   /* immediate hard failure; try next address */
        }

        TcpStatus w = tcp_wait_writable(socket, timeout_ms);
        if (w == TCP_ERR_WOULD_BLOCK) {
            result = TCP_ERR_WOULD_BLOCK;  /* timed out — stop, don't try more addrs */
            break;
        }
        if (w != TCP_SUCCESS) {
            result = TCP_ERR_GENERIC;
            break;
        }

        int so_error = -1;
        if (tcp_get_socket_error(socket, &so_error) == TCP_SUCCESS && so_error == 0) {
            result = TCP_SUCCESS;
            break;
        }
        result = TCP_ERR_CONNECT;       /* this address failed; SO_ERROR consumed, try next */
    }
    freeaddrinfo(res);

    /* Restore blocking mode to match tcp_connect's post-state. */
    tcp_set_non_blocking(socket, false);
    return result;
}