/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Tcp
*/

#include <stdlib.h>
#include <string.h>
#include "tcp.h"
#include "../fmt/fmt.h"


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
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (host == NULL || strcmp(host, "") == 0 || strcmp(host, "0.0.0.0") == 0) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } 
    else {
        if (!inet_pton(AF_INET, host, &addr.sin_addr)) {
            TCP_LOG("[tcp_bind] Error: Invalid host address %s in tcp_bind.\n", host);
            return TCP_ERR_RESOLVE;
        }
    }

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info); 
        TCP_LOG("[tcp_bind] Error: Binding failed with error: %s in tcp_bind\n", status_info.message);
        
        return TCP_ERR_BIND;
    }

    TCP_LOG("[tcp_bind] Socket bound successfully to %s:%u in tcp_bind.\n", host ? host : "ANY", port);
    return TCP_SUCCESS;
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

    struct sockaddr_in client_addr;
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
            return TCP_ERR_WOULD_BLOCK; // Assuming you define TCP_ERR_WOULD_BLOCK
        }
        #else
        if (status_info.sys_errno == EAGAIN || status_info.sys_errno == EWOULDBLOCK) {
            TCP_LOG("[tcp_accept] Non-blocking socket operation could not be completed immediately in tcp_accept.\n");
            return TCP_ERR_WOULD_BLOCK; // Assuming you define TCP_ERR_WOULD_BLOCK
        }
        #endif
        TCP_LOG("[tcp_accept] Error: Accepting connection failed with error: %s in tcp_accept\n", status_info.message);
        
        return TCP_ERR_ACCEPT;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    TCP_LOG("[tcp_accept] Accepted connection from %s:%d in tcp_accept.\n", client_ip, ntohs(client_addr.sin_port));
    
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
    if (host == NULL || host[0] == '\0') {
        TCP_LOG("[tcp_connect] Error: Host parameter is null or empty in tcp_connect.\n");
        return TCP_ERR_RESOLVE;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        TCP_LOG("[tcp_connect] Error: Invalid server address %s in tcp_connect.\n", host);
        return TCP_ERR_RESOLVE;
    }
    if (connect(socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info);
        TCP_LOG("[tcp_connect] Error: Connecting to server failed with error: %s in tcp_connect\n", status_info.message);
        
        return TCP_ERR_CONNECT;
    }

    TCP_LOG("[tcp_connect] Successfully connected to server %s:%d in tcp_connect.\n", host, port);
    return TCP_SUCCESS;
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
                if (sent) {
                    *sent = (size_t)bytes_sent;
                }
                
                TcpStatusInfo status_info;
                tcp_get_last_error(&status_info);
                TCP_LOG("[tcp_send] Error: Sending data failed with error: %s in tcp_send\n", status_info.message);
                
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
                if (sent) {
                    *sent = (size_t)bytes_sent;
                }
                
                TcpStatusInfo status_info;
                tcp_get_last_error(&status_info);
                TCP_LOG("[tcp_send] Error: Sending data failed with error: %s in tcp_send\n", status_info.message);
                
                return TCP_ERR_SEND;
            }
        #endif
        buf = (const char*)buf + bytes_sent;
        len -= bytes_sent;
    }
    if (sent) {
        *sent = (size_t)bytes_sent;
    }

    TCP_LOG("[tcp_send] Data sent successfully (%zu bytes) in tcp_send.\n", bytes_sent);
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
        } 
        else {
            bytes_received = (size_t)bytesRecvd;
        }
    #else
        bytes_received = recv(socket, buf, len, 0);
    #endif

    if (bytes_received > 0) {
        if (received) {
            *received = (size_t)bytes_received;
        }

        TCP_LOG("[tcp_recv] Data received successfully (%zu bytes) in tcp_recv.\n", bytes_received);
        return TCP_SUCCESS;
    } 
    else if (bytes_received == 0) {
        TCP_LOG("[tcp_recv] Connection closed by peer in tcp_recv.\n");
        return TCP_ERR_CLOSE;
    } 
    else {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info);
        TCP_LOG("[tcp_recv] Error: Receiving data failed with error: %s in tcp_recv\n", status_info.message);
        
        return TCP_ERR_RECV;
    }
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
        tcp_get_last_error(&status_info); // Populate status_info with the last error
        TCP_LOG("[tcp_close] Error: Closing socket failed with error: %s in tcp_close.\n", status_info.message);
        
        return TCP_ERR_CLOSE;
    }
    #else
    result = close(socket);
    if (result < 0) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info); // Populate status_info with the last error
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
        // On Windows, use ioctlsocket
        u_long mode = enable ? 1 : 0; // Non-zero value for non-blocking mode
        if (ioctlsocket(socket, FIONBIO, &mode) != NO_ERROR) {
            TcpStatusInfo status_info;
            tcp_get_last_error(&status_info);
            TCP_LOG("[tcp_set_non_blocking] Error: Setting non-blocking mode failed with error: %s\n", status_info.message);
            
            return TCP_ERR_GENERIC;
        }
    #else
        // On Unix-like systems, use fcntl
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
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    if (!address || address_len == 0 || !port) {
        TCP_LOG("[tcp_get_local_address] Error: Invalid parameter provided to tcp_get_local_address.\n");
        return TCP_ERR_GENERIC;
    }
    memset(&addr, 0, sizeof(addr));

    // Attempt to retrieve the local address of the socket
    if (getsockname(socket, (struct sockaddr*)&addr, &len) == -1) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info);
        TCP_LOG("[tcp_get_local_address] Error: getsockname() failed with error: %s\n", status_info.message);
        
        return TCP_ERR_GENERIC;
    }
    if (addr.ss_family == AF_INET) {
        // IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in*)&addr;
        if (inet_ntop(AF_INET, &ipv4->sin_addr, address, address_len) == NULL) {
            TCP_LOG("[tcp_get_local_address] Error: inet_ntop() failed for IPv4 address.\n");
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(ipv4->sin_port);
    } 
    else if (addr.ss_family == AF_INET6) {
        // IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)&addr;
        if (inet_ntop(AF_INET6, &ipv6->sin6_addr, address, address_len) == NULL) {
            TCP_LOG("[tcp_get_local_address] Error: inet_ntop() failed for IPv6 address.\n");
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(ipv6->sin6_port);
    } 
    else {
        TCP_LOG("[tcp_get_local_address] Error: Unknown socket family.\n");
        return TCP_ERR_GENERIC;
    }

    TCP_LOG("[tcp_get_local_address] Local address: %s, port: %u\n", address, *port);
    return TCP_SUCCESS;
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
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);

    // Validate input parameters
    if (!address || address_len == 0 || !port) {
        TCP_LOG("[tcp_get_remote_address] Error: Invalid parameter provided to tcp_get_remote_address.\n");
        return TCP_ERR_GENERIC;
    }

    // Clear the memory for safety
    memset(&addr, 0, sizeof(addr));

    // Get remote address of the socket
    if (getpeername(socket, (struct sockaddr*)&addr, &len) == -1) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info); // Populate status_info with the last error
        TCP_LOG("[tcp_get_remote_address] Error: getpeername() failed with error: %s\n", status_info.message);
        
        return TCP_ERR_GENERIC;
    }
    if (addr.ss_family == AF_INET) {
        // Handle IPv4
        struct sockaddr_in *ipv4 = (struct sockaddr_in*)&addr;
        if (inet_ntop(AF_INET, &ipv4->sin_addr, address, address_len) == NULL) {
            TCP_LOG("[tcp_get_remote_address] Error: inet_ntop() failed for IPv4 address.\n");
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(ipv4->sin_port);
    } 
    else if (addr.ss_family == AF_INET6) {
        // Handle IPv6
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)&addr;
        if (inet_ntop(AF_INET6, &ipv6->sin6_addr, address, address_len) == NULL) {
            TCP_LOG("[tcp_get_remote_address] Error: inet_ntop() failed for IPv6 address.\n");
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(ipv6->sin6_port);
    } 
    else {
        TCP_LOG("[tcp_get_remote_address] Error: Unknown socket family.\n");
        return TCP_ERR_GENERIC;
    }

    TCP_LOG("[tcp_get_remote_address] Remote address: %s, port: %u\n", address, *port);
    return TCP_SUCCESS;
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
        // On Windows, setsockopt uses a char* for optval
        result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
    #else
        // On Unix-like systems, setsockopt uses a void* for optval
        result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    #endif

    if (result < 0) {
        TcpStatusInfo status_info;
        tcp_get_last_error(&status_info); // Populate status_info with the last error
        TCP_LOG("[tcp_set_reuse_addr] Error: Setting SO_REUSEADDR failed with error: %s in tcp_set_reuse_addr.\n", status_info.message);
        
        return TCP_ERR_GENERIC; // Using TCP_ERR_GENERIC to indicate a failure in setting the option
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
        *port = ntohs(s->sin_port);
    } 
    else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
        if (inet_ntop(AF_INET6, &s->sin6_addr, host, host_len) == NULL) {
            TCP_LOG("[tcp_get_peer_name] Error: inet_ntop() failed for IPv6 address.\n");
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(s->sin6_port);
    } 
    else {
        TCP_LOG("[tcp_get_peer_name] Error: Unknown socket family.\n");
        return TCP_ERR_GENERIC;
    }

    TCP_LOG("[tcp_get_peer_name] Peer name: %s, port: %u\n", host, *port);
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
        *port = ntohs(s->sin_port);
    } 
    else if (addr.ss_family == AF_INET6) {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
        if (inet_ntop(AF_INET6, &s->sin6_addr, host, host_len) == NULL) {
            TCP_LOG("[tcp_get_sock_name] Error: inet_ntop() failed for IPv6 address.\n");
            return TCP_ERR_GENERIC;
        }
        *port = ntohs(s->sin6_port);
    } 
    else {
        TCP_LOG("[tcp_get_sock_name] Error: Unknown socket family.\n");
        return TCP_ERR_GENERIC;
    }

    TCP_LOG("[tcp_get_sock_name] Socket name: %s, port: %u\n", host, *port);
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
    SocketSSLMapping* mapping = get_ssl_mapping(socket, true);
    if (mapping) {
        mapping->ssl = ssl;
    }
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
    SocketSSLMapping* mapping = get_ssl_mapping(socket, false);
    return mapping ? mapping->ssl : NULL;
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
    if (ssl_ctx == NULL) {
        TCP_LOG("[tcp_enable_ssl] SSL context is not initialized.\n");
        return TCP_ERR_SETUP;
    }

    // Create an SSL object for the socket
    SSL* ssl = SSL_new(ssl_ctx);
    if (!ssl) {
        TCP_LOG("[tcp_enable_ssl] Failed to create SSL object.\n");
        return TCP_ERR_GENERIC;
    }

    // Associate the socket with the SSL object
    if (SSL_set_fd(ssl, socket) == 0) {
        TCP_LOG("[tcp_enable_ssl] Failed to associate socket with SSL.\n");
        SSL_free(ssl);

        return TCP_ERR_GENERIC;
    }

    tcp_set_ssl(socket, ssl);
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
    SSL* ssl = tcp_get_ssl(socket);

    if (ssl == NULL) {
        TCP_LOG("[tcp_disable_ssl] No SSL object associated with the socket.\n");
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
    if (ssl_ctx != NULL) {
        TCP_LOG("[tcp_ssl_init] SSL context is already initialized.\n");
        return TCP_SUCCESS; 
    }

    // Initialize OpenSSL
    SSL_load_error_strings();   
    OpenSSL_add_ssl_algorithms(); // Register SSL/TLS ciphers and digests
    
    // Create a new SSL_CTX object as framework for TLS/SSL enabled functions
    ssl_ctx = SSL_CTX_new(TLS_server_method()); 
    if (!ssl_ctx) {
        TCP_LOG("[tcp_ssl_init] Error creating SSL context: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return TCP_ERR_SETUP; 
    }

    // Load certificate and private key files into the SSL context
    if (SSL_CTX_use_certificate_file(ssl_ctx, cert, SSL_FILETYPE_PEM) <= 0) {
        TCP_LOG("[tcp_ssl_init] Error loading certificate from file: %s\n", ERR_error_string(ERR_get_error(), NULL));
        
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL; 

        return TCP_ERR_SSL;
    }
    if (SSL_CTX_use_PrivateKey_file(ssl_ctx, key, SSL_FILETYPE_PEM) <= 0) {
        TCP_LOG("[tcp_ssl_init] Error loading private key from file: %s\n", ERR_error_string(ERR_get_error(), NULL));
        
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL; 

        return TCP_ERR_SSL;
    }

    // Verify private key
    if (!SSL_CTX_check_private_key(ssl_ctx)) {
        TCP_LOG("[tcp_ssl_init] Private key does not match the public certificate: %s\n", ERR_error_string(ERR_get_error(), NULL));
        
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL; 

        return TCP_ERR_SSL;
    }

    // Initialize SSL mappings for SSL/TLS session management
    initialize_ssl_mappings();
    TCP_LOG("[tcp_ssl_init] OpenSSL and SSL context initialized successfully.\n");

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
    if (ssl_ctx != NULL) {
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL;
        TCP_LOG("[tcp_ssl_cleanup] SSL context cleaned up successfully.\n");
    }

    // Clean up OpenSSL
    EVP_cleanup();
    ERR_free_strings();

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
    if (ssl_ctx == NULL) {
        TCP_LOG("[tcp_ssl_connect] SSL context is not initialized. Call tcp_ssl_init first.\n");
        return TCP_ERR_SETUP; // SSL context is not initialized
    }
    if (host == NULL || host[0] == '\0') {
        TCP_LOG("[tcp_ssl_connect] Host parameter is null or empty.\n");
        return TCP_ERR_RESOLVE; // Host parameter is invalid
    }

    SSL* ssl = SSL_new(ssl_ctx);
    if (ssl == NULL) {
        TCP_LOG("[tcp_ssl_connect] Failed to create SSL object: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return TCP_ERR_SSL; // Failed to create SSL object
    }

    // Set the file descriptor for the SSL object
    if (SSL_set_fd(ssl, socket) == 0) {
        TCP_LOG("[tcp_ssl_connect] Failed to set file descriptor for SSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
        SSL_free(ssl); 

        return TCP_ERR_SSL; 
    }

    // Set the hostname for Server Name Indication (SNI)
    if (SSL_set_tlsext_host_name(ssl, host) == 0) {
        TCP_LOG("[tcp_ssl_connect] Failed to set SNI Hostname: %s\n", ERR_error_string(ERR_get_error(), NULL));
        SSL_free(ssl); 

        return TCP_ERR_SSL; 
    }

    // Perform the SSL handshake
    if (SSL_connect(ssl) != 1) {
        TCP_LOG("[tcp_ssl_connect] SSL handshake failed: %s\n", ERR_error_string(ERR_get_error(), NULL));
        SSL_free(ssl); // Clean up the SSL object on failure

        return TCP_ERR_SSL_HANDSHAKE; // SSL handshake failed
    }

    // Successfully established SSL connection, map the SSL object with the socket
    SocketSSLMapping* mapping = get_ssl_mapping(socket, true);
    if (!mapping) {
        TCP_LOG("[tcp_ssl_connect] Failed to map SSL object with socket.\n");
        SSL_free(ssl); // Clean up the SSL object on failure

        return TCP_ERR_SSL; // Failed to store SSL object
    }
    mapping->ssl = ssl;

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
    if (ssl_ctx == NULL) {
        TCP_LOG("[tcp_ssl_accept] SSL context is not initialized. Call tcp_ssl_init first.\n");
        return TCP_ERR_SETUP; // SSL context is not initialized
    }

    // Create an SSL object from the context
    SSL* ssl = SSL_new(ssl_ctx);
    if (ssl == NULL) {
        TCP_LOG("[tcp_ssl_accept] Failed to create SSL object: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return TCP_ERR_SSL; // Failed to create SSL object
    }

    // Set the file descriptor for the SSL object
    if (SSL_set_fd(ssl, socket) == 0) {
        TCP_LOG("[tcp_ssl_accept] Failed to set file descriptor for SSL: %s\n", ERR_error_string(ERR_get_error(), NULL));
        SSL_free(ssl); // Clean up the SSL object on failure

        return TCP_ERR_SSL; // Failed to associate socket with SSL
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
            if (errno != 0) { // Check if errno has been set
                TCP_LOG("[tcp_ssl_accept] Syscall error: %s\n", strerror(errno));
            }
        }
        SSL_free(ssl);
        return TCP_ERR_SSL_HANDSHAKE;
    }

    // Handshake was successful, store the SSL object with the socket for further operations
    SocketSSLMapping* mapping = get_ssl_mapping(socket, true);
    if (!mapping) {
        TCP_LOG("Failed to map SSL object with socket.\n");
        SSL_free(ssl); 

        return TCP_ERR_SSL; 
    }
    mapping->ssl = ssl;

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
    SSL* ssl = tcp_get_ssl(socket);

    if (!ssl) {
        TCP_LOG("[tcp_ssl_close] No SSL object associated with the socket. Closing socket without SSL shutdown.\n");
        tcp_close(socket);

        return TCP_ERR_NO_SSL; // Indicate that there was no SSL to shutdown
    }

    // Initiate the SSL/TLS shutdown sequence
    int shutdownResult = SSL_shutdown(ssl);
    if (shutdownResult == 0) {
        shutdownResult = SSL_shutdown(ssl);
        if (shutdownResult != 1) {
            TCP_LOG("[tcp_ssl_close] SSL shutdown did not complete cleanly: %s\n", ERR_error_string(SSL_get_error(ssl, shutdownResult), NULL));
            // Even if shutdown did not complete, continue to free SSL and close socket
        }
    } 
    else if (shutdownResult < 0) {
        TCP_LOG("[tcp_ssl_close] SSL shutdown failed: %s\n", ERR_error_string(SSL_get_error(ssl, shutdownResult), NULL));
        // Even in case of failure, we need to free SSL and close the socket
    }

    // Free the SSL object
    SSL_free(ssl);
    tcp_set_ssl(socket, NULL);

    // Close the socket
    TcpStatus closeStatus = tcp_close(socket);
    if (closeStatus != TCP_SUCCESS) {
        TCP_LOG("[tcp_ssl_close] Socket close failed.\n");
        return closeStatus;
    }

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
    if (!buf || len == 0) {
        TCP_LOG("[tcp_ssl_send] Invalid buffer or length for SSL send.\n");
        return TCP_ERR_SEND; // Invalid parameters
    }

    SSL* ssl = tcp_get_ssl(socket);
    if (!ssl) {
        TCP_LOG("[tcp_ssl_send] No SSL object associated with the socket. Cannot send data.\n");
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
                    TCP_LOG("[tcp_ssl_send] SSL_write needs to be called again.\n");
                    
                    if (sent) {
                        *sent = totalSent;
                    }
                    return TCP_SUCCESS; // Not an error, but the send needs to be retried
                case SSL_ERROR_ZERO_RETURN:
                    // The connection was closed
                    TCP_LOG("[tcp_ssl_send] SSL connection closed by peer.\n");
                    return TCP_ERR_CLOSE;

                case SSL_ERROR_SYSCALL:
                    TCP_LOG("[tcp_ssl_send] SSL write syscall error: %s\n", strerror(errno));
                    return TCP_ERR_SEND;

                default:
                    TCP_LOG("[tcp_ssl_send] SSL write error: %s\n", ERR_error_string(ERR_get_error(), NULL));
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
    if (!buf || len == 0) {
        TCP_LOG("[tcp_ssl_recv] Invalid buffer or length for SSL receive.\n");
        return TCP_ERR_RECV; // Invalid parameters
    }

    SSL* ssl = tcp_get_ssl(socket);
    if (!ssl) {
        TCP_LOG("[tcp_ssl_recv] No SSL object associated with the socket. Cannot receive data.\n");
        return TCP_ERR_NO_SSL; // SSL context not found for the socket
    }

    int result = SSL_read(ssl, buf, len);
    if (result > 0) {
        if (received) {
            *received = result;
        }
        
        TCP_LOG("[tcp_ssl_recv] Received %d bytes over SSL.\n", result);
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
                return TCP_SUCCESS; // Not an error, but the read needs to be retried

            case SSL_ERROR_ZERO_RETURN:
                // The connection was closed
                TCP_LOG("[tcp_ssl_recv] SSL connection closed by peer.\n");
                return TCP_ERR_CLOSE;

            case SSL_ERROR_SYSCALL: {
                if (ERR_peek_error() == 0) { // No specific SSL error
                    if (result == 0 || errno == 0) {
                        TCP_LOG("[tcp_ssl_recv] SSL connection closed by peer or EOF reached.\n");
                        SSL_free(ssl);
                        tcp_set_ssl(socket, NULL); // Ensure to clear the SSL mapping
                        tcp_close(socket); // Close the socket as the connection is done
                        return TCP_ERR_CLOSE;
                    } 
                    else {
                        TCP_LOG("[tcp_ssl_recv] SSL syscall error: %s\n", strerror(errno));
                    }
                }
                
                TCP_LOG("[tcp_ssl_recv] SSL read syscall error: %s\n", strerror(errno));
                return TCP_ERR_RECV;
            }
            default:
                TCP_LOG("[tcp_ssl_recv] SSL read error: %s\n", ERR_error_string(ERR_get_error(), NULL));
                return TCP_ERR_RECV;
        }
    }
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
    // Custom implementation or estimation might be needed.
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
    size_t result = send(socket, buf, len, 0);
    if (result == (size_t)TCP_INVALID_SOCKET) {
        // Check if the error is EWOULDBLOCK (or its equivalent), which is normal for non-blocking sockets
        #ifdef _WIN32
            int lastError = WSAGetLastError();
            if (lastError == WSAEWOULDBLOCK) {
                TCP_LOG("[tcp_async_send] Error: connection is blocked.\n");
                return TCP_ERR_WOULD_BLOCK;
            }
        #else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                TCP_LOG("[tcp_async_send] Error: connection is blocked.\n");
                return TCP_ERR_WOULD_BLOCK;
            }
        #endif
        return TCP_ERR_SEND;
    }

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
    size_t result = recv(socket, buf, len, 0);
    if (result == (size_t)TCP_INVALID_SOCKET) {
        // Check if the error is EWOULDBLOCK or its equivalent, which is normal for non-blocking sockets
        #ifdef _WIN32
            int lastError = WSAGetLastError();
            if (lastError == WSAEWOULDBLOCK) {
                TCP_LOG("[tcp_async_recv] Error: No data available to read; non-blocking operation.\n");
                return TCP_ERR_WOULD_BLOCK;
            }
        #else
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                TCP_LOG("[tcp_async_recv] Error: No data available to read; non-blocking operation.\n");
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