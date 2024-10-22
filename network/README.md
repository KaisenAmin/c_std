# Network Lib in C


## TcpSocket 

The `TcpSocket` in your network library serves as a key component for establishing and managing TCP connections. It abstracts the platform-specific details of socket handling, allowing your library to perform operations like connecting, sending, receiving, and managing socket states across both Windows and Unix-like systems.

### Key Roles of `TcpSocket`:
1. **Cross-platform Socket Management**: It handles differences in socket creation, binding, and communication between Windows and Unix-like systems, using platform-specific API calls (e.g., `WSASocket` for Windows and `socket` for Unix).
2. **Connection Establishment**: It manages connection processes, including setting up client-server communication with features like `tcp_bind`, `tcp_listen`, and `tcp_connect`.
3. **Data Transmission**: The `TcpSocket` is used to send and receive data over TCP in both blocking and non-blocking modes.
4. **SSL/TLS Support**: With integrated SSL/TLS support, `TcpSocket` also facilitates secure communication by associating SSL objects with sockets, performing SSL handshakes, and managing encrypted data transfer.
5. **Error Handling**: The `TcpSocket` integrates platform-specific error management and reporting, providing feedback and logging for debugging and reliability.

By using `TcpSocket`, your library offers a clean and robust API for handling TCP connections with flexibility and cross-platform compatibility.

### TcpSocket Functions Descriptions 

Certainly! Here's the complete explanation for `tcp_get_last_error`:

---

### `void tcp_get_last_error(TcpStatusInfo* status_info)`

**Purpose**:  
The `tcp_get_last_error` function retrieves the most recent system-specific error code and formats it into a human-readable message. It stores the error code and a corresponding description in a `TcpStatusInfo` structure. This function is essential for understanding the cause of failures in network operations.

**Parameters**:  
- `status_info`: A pointer to a `TcpStatusInfo` structure. The function populates this structure with the system error code and a human-readable message.

**Return Value**:  
This function does not return any value. Instead, it directly modifies the `TcpStatusInfo` structure by setting its `sys_errno` field to the system-specific error code, and the `message` field is filled with a formatted error message.

**Usage Case**:  
Whenever a network operation (like socket creation, connection, or data transmission) fails, you can call `tcp_get_last_error` to retrieve and log the last error that occurred in the system. It helps in debugging and provides detailed information about the failure.

---

### `bool tcp_is_valid_address(const char* address)`

**Purpose**:  
The `tcp_is_valid_address` function checks if the given string is a valid IPv4 or IPv6 address. It utilizes the `inet_pton` function to perform the validation. This function helps ensure that network operations are performed with valid IP addresses.

**Parameters**:  
- `address`: A constant pointer to a string that holds the IP address to be validated.

**Return Value**:  
- Returns `true` if the `address` is a valid IPv4 or IPv6 address.
- Returns `false` if the address is invalid or if `inet_pton` fails to convert the address.

**Usage Case**:  
This function is useful when validating user input or any IP addresses before performing network-related tasks like binding, connecting, or sending data. For instance, if you are writing a server and the user provides an IP address for binding, you can call `tcp_is_valid_address` to ensure that the IP address is valid before proceeding.

---

### `TcpStatus tcp_socket_create(TcpSocket* sock)`

**Purpose**:  
The `tcp_socket_create` function creates a new TCP socket. It handles the socket creation process for both Windows and Unix-like systems. The created socket is stored in the `sock` parameter.

**Parameters**:  
- `sock`: A pointer to the `TcpSocket` where the created socket will be stored.

**Return Value**:  
- Returns `TCP_SUCCESS` if the socket is successfully created.
- Returns `TCP_ERR_SOCKET` if an error occurs during the creation of the socket.

**Usage Case**:  
This function is typically used in any application that requires TCP communication. For example, when writing a server or client, you first need to create a TCP socket before binding, listening, or connecting. By calling `tcp_socket_create`, you ensure that the socket is created and ready for further operations.

### `TcpStatus tcp_bind(TcpSocket sock, const char* host, unsigned short port)`

**Purpose**:  
The `tcp_bind` function binds a TCP socket to a specific host and port. It allows the socket to listen for incoming connections on the given address. If no specific host is provided, or if the host is set to "0.0.0.0", it binds the socket to all available network interfaces.

**Parameters**:  
- `sock`: The TCP socket to bind.
- `host`: The host address to bind to. It can be an IP address or a hostname. Passing NULL, an empty string, or "0.0.0.0" binds the socket to all available interfaces.
- `port`: The port number to bind the socket to.

**Return Value**:  
- `TCP_SUCCESS`: The socket was successfully bound to the address.
- `TCP_ERR_RESOLVE`: The provided host address was invalid.
- `TCP_ERR_BIND`: The socket could not be bound to the specified address.

**Usage Case**:  
When setting up a server that listens for incoming connections, this function is used to bind a socket to a specific IP address and port before starting to listen for connections.

---

### `TcpStatus tcp_listen(TcpSocket socket, int backlog)`

**Purpose**:  
The `tcp_listen` function sets a TCP socket to listen for incoming connections. It makes the socket passive, which means it can now accept connections.

**Parameters**:  
- `socket`: The TCP socket that will be used for listening.
- `backlog`: The maximum number of pending connections that the socket can queue.

**Return Value**:  
- `TCP_SUCCESS`: The socket was successfully set to listen.
- `TCP_ERR_LISTEN`: An error occurred while setting the socket to listen.

**Usage Case**:  
This function is typically called after binding a socket to a specific host and port, to allow the socket to start listening for incoming connection requests.

---

### `TcpStatus tcp_accept(TcpSocket socket, TcpSocket* client_socket)`

**Purpose**:  
The `tcp_accept` function accepts an incoming connection on a listening TCP socket. It creates a new socket for the accepted client connection, allowing communication with the connected client.

**Parameters**:  
- `socket`: The listening socket that is ready to accept a connection.
- `client_socket`: A pointer to a `TcpSocket` where the new socket for the accepted connection will be stored.

**Return Value**:  
- `TCP_SUCCESS`: A connection was successfully accepted.
- `TCP_ERR_WOULD_BLOCK`: The operation would block if the socket is non-blocking and no connections are currently available.
- `TCP_ERR_ACCEPT`: An error occurred while accepting the connection.

**Usage Case**:  
In server applications, this function is used after a socket starts listening for connections. When a client attempts to connect, `tcp_accept` is called to establish the connection and create a socket for communication with the client.

---

### `TcpStatus tcp_connect(TcpSocket socket, const char* host, unsigned short port)`

**Purpose**:  
The `tcp_connect` function establishes a connection to a remote server using a TCP socket. It resolves the provided host address and connects to the server on the specified port.

**Parameters**:  
- `socket`: The TCP socket to be used for the connection.
- `host`: The hostname or IP address of the remote server.
- `port`: The port number on the remote server to connect to.

**Return Value**:  
- `TCP_SUCCESS`: The connection was successfully established.
- `TCP_ERR_RESOLVE`: The host could not be resolved.
- `TCP_ERR_CONNECT`: An error occurred while trying to establish the connection.

**Usage Case**:  
This function is commonly used in client applications to connect to a server. For example, a client connecting to a web server would use `tcp_connect` to establish the connection before sending requests.

### `TcpStatus tcp_init(void)`

**Purpose**:  
The `tcp_init` function is used to initialize the network API on Windows systems. It calls `WSAStartup`, which is required for using sockets in Windows. On Unix-like systems, no initialization is required, so the function does nothing on those platforms.

**Parameters**:  
This function does not take any parameters.

**Return Value**:  
- `TCP_SUCCESS`: The network API was initialized successfully.
- `TCP_ERR_SETUP`: The `WSAStartup` call failed on Windows.

**Usage Case**:  
This function must be called before creating or using any sockets on Windows systems. It is not necessary on Unix-like systems, but calling it does no harm.

---

### `TcpStatus tcp_cleanup(void)`

**Purpose**:  
The `tcp_cleanup` function cleans up the network API on Windows systems by calling `WSACleanup`. On Unix-like systems, it does nothing, as no cleanup is required.

**Parameters**:  
This function does not take any parameters.

**Return Value**:  
- `TCP_SUCCESS`: The cleanup operation was successful.

**Usage Case**:  
This function is called at the end of the program or when network operations are no longer needed to release resources associated with Winsock on Windows.

---

### `TcpStatus tcp_send(TcpSocket socket, const void* buf, size_t len, size_t* sent)`

**Purpose**:  
The `tcp_send` function sends data over a TCP socket. It handles sending data for both Windows and Unix-like systems with platform-specific implementations. The function continues to send data until the entire buffer is transmitted.

**Parameters**:  
- `socket`: The TCP socket through which the data will be sent.
- `buf`: Pointer to the buffer containing the data to send.
- `len`: The number of bytes to send from the buffer.
- `sent`: A pointer to a `size_t` variable where the total number of bytes successfully sent will be stored.

**Return Value**:  
- `TCP_SUCCESS`: The data was successfully sent.
- `TCP_ERR_CLOSE`: The connection was closed by the peer.
- `TCP_ERR_SEND`: An error occurred during sending.

**Usage Case**:  
This function is typically used in client-server communication to send data to a remote peer. It ensures that the entire buffer is sent before returning.

---

### `TcpStatus tcp_recv(TcpSocket socket, void* buf, size_t len, size_t* received)`

**Purpose**:  
The `tcp_recv` function receives data from a TCP socket. It reads incoming data into the provided buffer and returns the number of bytes received. The function supports both Windows and Unix-like systems.

**Parameters**:  
- `socket`: The TCP socket from which to receive data.
- `buf`: Pointer to the buffer where the received data will be stored.
- `len`: The length of the buffer.
- `received`: A pointer to a `size_t` variable where the number of bytes received will be stored.

**Return Value**:  
- `TCP_SUCCESS`: Data was received successfully.
- `TCP_ERR_CLOSE`: The connection was closed by the peer.
- `TCP_ERR_RECV`: An error occurred during receiving.

**Usage Case**:  
This function is used in client-server applications to receive data from a connected peer. It reads the available data and stores it in the provided buffer for further processing.

---

### `TcpStatus tcp_close(TcpSocket socket)`

**Purpose**:  
This function closes a TCP socket. It handles the platform-specific logic for closing a socket on both Windows and Unix-like systems.

**Parameters**:  
- `socket`: The TCP socket that needs to be closed.

**Return Value**:  
- `TCP_SUCCESS`: The socket was closed successfully.
- `TCP_ERR_CLOSE`: An error occurred while closing the socket.

**Usage Case**:  
Use this function to release a TCP socket after it is no longer needed, ensuring that system resources are properly freed.

---

### `TcpStatus tcp_shutdown(TcpSocket socket, TcpShutdownHow how)`

**Purpose**:  
This function shuts down a TCP socket for receiving, sending, or both. It is useful for signaling that no more data will be transmitted or received on the socket.

**Parameters**:  
- `socket`: The TCP socket that needs to be shut down.
- `how`: Specifies whether to shut down receiving (`TCP_SHUTDOWN_RECEIVE`), sending (`TCP_SHUTDOWN_SEND`), or both (`TCP_SHUTDOWN_BOTH`).

**Return Value**:  
- `TCP_SUCCESS`: The socket was successfully shut down.
- `TCP_ERR_GENERIC`: An error occurred during the shutdown.

**Usage Case**:  
Use this function when you want to gracefully close one or both communication channels of a TCP socket.

---

### `TcpStatus tcp_set_timeout(TcpSocket socket, TcpTimeoutOperation operation, long timeout_ms)`

**Purpose**:  
This function sets the timeout for receiving or sending data on a TCP socket. The timeout value is specified in milliseconds, and it applies to either sending, receiving, or both operations.

**Parameters**:  
- `socket`: The TCP socket on which to set the timeout.
- `operation`: The operation to apply the timeout to, either receiving (`TCP_TIMEOUT_RECV`), sending (`TCP_TIMEOUT_SEND`), or both (`TCP_TIMEOUT_BOTH`).
- `timeout_ms`: The timeout value in milliseconds.

**Return Value**:  
- `TCP_SUCCESS`: The timeout was set successfully.
- `TCP_ERR_GENERIC`: An error occurred while setting the timeout.

**Usage Case**:  
Use this function when you need to control how long a TCP socket should wait before timing out during data transfer operations. This is helpful when dealing with unreliable or slow network connections.

---

### `TcpStatus tcp_resolve_hostname(const char* hostname, char* ip_address, size_t ip_address_len)`

**Purpose**:  
This function resolves a given hostname (e.g., "www.example.com") to its corresponding IP address (IPv4 or IPv6) and stores the result in a buffer.

**Parameters**:  
- `hostname`: The hostname to resolve.
- `ip_address`: A buffer where the resolved IP address will be stored as a string.
- `ip_address_len`: The size of the `ip_address` buffer.

**Return Value**:  
- `TCP_SUCCESS`: The hostname was successfully resolved.
- `TCP_ERR_RESOLVE`: Failed to resolve the hostname.
- `TCP_ERR_GENERIC`: Failed to convert the IP address to a string or encountered another error during resolution.

**Usage Case**:  
Use this function when you need to connect to a server by hostname rather than an IP address. It converts the hostname to an IP address so that the application can establish a connection.

---

### `TcpStatus tcp_set_non_blocking(TcpSocket socket, bool enable)`

**Purpose**:  
This function enables or disables non-blocking mode on a TCP socket. In non-blocking mode, socket operations return immediately, even if they would normally block (e.g., waiting for data).

**Parameters**:  
- `socket`: The TCP socket on which to set non-blocking mode.
- `enable`: A boolean flag to enable or disable non-blocking mode (`true` to enable, `false` to disable).

**Return Value**:  
- `TCP_SUCCESS`: The non-blocking mode was successfully set.
- `TCP_ERR_GENERIC`: Failed to set or retrieve the socket flags.

**Usage Case**:  
Use this function when implementing non-blocking network operations, such as in event-driven or asynchronous programming models.

---

### `TcpStatus tcp_get_local_address(TcpSocket socket, char* address, size_t address_len, unsigned short* port)`

**Purpose**:  
This function retrieves the local IP address and port number to which the socket is bound.

**Parameters**:  
- `socket`: The TCP socket from which to retrieve the local address.
- `address`: A buffer where the local IP address will be stored.
- `address_len`: The size of the `address` buffer.
- `port`: A pointer to store the local port number.

**Return Value**:  
- `TCP_SUCCESS`: Successfully retrieved the local address and port.
- `TCP_ERR_GENERIC`: Failed to retrieve the local address or port.

**Usage Case**:  
Use this function to determine the local network address and port that a socket is bound to, which can be useful for logging, debugging, or verifying network configurations.

---

### `TcpStatus tcp_get_remote_address(TcpSocket socket, char* address, size_t address_len, unsigned short* port)`

**Purpose**:  
This function retrieves the remote IP address and port number associated with a connected TCP socket.

**Parameters**:  
- `socket`: The TCP socket for which the remote address is retrieved.
- `address`: A buffer to store the remote IP address in string form.
- `address_len`: The length of the `address` buffer.
- `port`: A pointer to store the remote port number.

**Return Value**:  
- `TCP_SUCCESS`: The remote address and port were successfully retrieved.
- `TCP_ERR_GENERIC`: An error occurred while retrieving the address or port.

**Usage Case**:  
This function is useful for identifying the remote peer's address in a connected socket, which is often needed for logging or debugging network connections.

---

### `TcpStatus tcp_set_reuse_addr(TcpSocket socket, bool enabled)`

**Purpose**:  
This function sets the `SO_REUSEADDR` option on a TCP socket, allowing the reuse of local addresses. It is commonly used when you need to bind multiple sockets to the same local address and port.

**Parameters**:  
- `socket`: The TCP socket on which to set the `SO_REUSEADDR` option.
- `enabled`: A boolean indicating whether to enable (`true`) or disable (`false`) the option.

**Return Value**:  
- `TCP_SUCCESS`: The `SO_REUSEADDR` option was successfully set.
- `TCP_ERR_GENERIC`: An error occurred while setting the option.

**Usage Case**:  
This function is useful in server applications where multiple sockets need to bind to the same local address for handling different clients.

---

### `TcpStatus tcp_get_peer_name(TcpSocket socket, char* host, size_t host_len, unsigned short* port)`

**Purpose**:  
This function retrieves the remote IP address and port number of a peer connected to the specified TCP socket.

**Parameters**:  
- `socket`: The TCP socket for which the peer address is retrieved.
- `host`: A buffer to store the peer's IP address in string form.
- `host_len`: The length of the `host` buffer.
- `port`: A pointer to store the peer's port number.

**Return Value**:  
- `TCP_SUCCESS`: The peer's address and port were successfully retrieved.
- `TCP_ERR_GENERIC`: An error occurred while retrieving the peer's address or port.

**Usage Case**:  
This function is useful for getting information about the peer connected to a socket, such as for logging or auditing purposes.

---

### `TcpStatus tcp_get_sock_name(TcpSocket socket, char* host, size_t host_len, unsigned short* port)`

**Purpose**:  
This function retrieves the local IP address and port number associated with a TCP socket.

**Parameters**:  
- `socket`: The TCP socket from which to retrieve the local address.
- `host`: A buffer to store the IP address in string form.
- `host_len`: The size of the host buffer.
- `port`: A pointer to store the port number.

**Return Value**:  
- `TCP_SUCCESS`: The local address and port were successfully retrieved.
- `TCP_ERR_GENERIC`: An error occurred while retrieving the address or port.

**Usage Case**:  
This function is useful for retrieving information about the local socket, especially when you need to know which IP and port it is bound to after it has been created.

---

### `void tcp_set_ssl(TcpSocket socket, SSL* ssl)`

**Purpose**:  
This function associates an SSL object with a TCP socket, enabling the socket for SSL/TLS communication.

**Parameters**:  
- `socket`: The TCP socket to associate with an SSL object.
- `ssl`: The SSL object to associate with the socket.

**Return Value**:  
None.

**Usage Case**:  
This function is typically used when setting up SSL for a specific socket after initializing an SSL context. It allows the socket to use the SSL object for encrypted communication.

---

### `SSL* tcp_get_ssl(TcpSocket socket)`

**Purpose**:  
This function retrieves the SSL object that has been associated with a specified TCP socket.

**Parameters**:  
- `socket`: The TCP socket for which to retrieve the associated SSL object.

**Return Value**:  
- The SSL object associated with the socket, or `NULL` if no SSL object is associated.

**Usage Case**:  
Useful for retrieving the SSL object when performing SSL-specific operations on a socket, such as sending or receiving encrypted data.

---

### `TcpStatus tcp_enable_ssl(TcpSocket socket)`

**Purpose**:  
This function creates an SSL object for the specified socket, associates it with the socket, and prepares the socket for secure SSL/TLS communication.

**Parameters**:  
- `socket`: The TCP socket on which to enable SSL/TLS.

**Return Value**:  
- `TCP_SUCCESS`: SSL/TLS was successfully enabled on the socket.
- `TCP_ERR_SETUP`: SSL context is not initialized.
- `TCP_ERR_GENERIC`: Failed to create or associate an SSL object with the socket.

**Usage Case**:  
Typically called when you want to enable encrypted communication on an existing TCP socket, ensuring that data sent and received on the socket is protected by SSL/TLS.

---

### `TcpStatus tcp_disable_ssl(TcpSocket socket)`

**Purpose**:  
This function shuts down SSL/TLS on the specified socket, frees the associated SSL object, and clears any SSL mappings for the socket.

**Parameters**:  
- `socket`: The TCP socket on which SSL/TLS should be disabled.

**Return Value**:  
- `TCP_SUCCESS`: SSL/TLS shutdown completed successfully.
- `TCP_ERR_NO_SSL`: No SSL object was associated with the socket.

**Usage Case**:  
Called when SSL/TLS communication on a socket is no longer needed. It gracefully shuts down the encrypted connection and releases any associated resources.

---

### `TcpStatus tcp_ssl_init(const char* cert, const char* key)`

**Purpose**:  
This function initializes SSL/TLS support by creating an SSL context, loading the specified certificate and private key, and verifying that the private key matches the public certificate.

**Parameters**:  
- `cert`: The path to the PEM-encoded certificate file.
- `key`: The path to the PEM-encoded private key file.

**Return Value**:  
- `TCP_SUCCESS`: SSL context initialized successfully, and the certificate and private key were loaded and verified.
- `TCP_ERR_SETUP`: Failed to create the SSL context.
- `TCP_ERR_SSL`: Failed to load the certificate, private key, or verify the private key.

**Usage Case**:  
Called during application startup to initialize SSL/TLS support and set up the necessary certificate and key for secure communication over TCP sockets.

---

### `TcpStatus tcp_ssl_cleanup(void)`

**Purpose**:  
This function cleans up the SSL context and deallocates OpenSSL resources, freeing any memory associated with the SSL/TLS context and performing OpenSSL cleanup operations.

**Parameters**:  
None.

**Return Value**:  
- `TCP_SUCCESS`: The SSL context and OpenSSL resources were cleaned up successfully.

**Usage Case**:  
Called when SSL/TLS functionality is no longer needed to free up resources and ensure proper cleanup of the SSL context and OpenSSL data.

---

### `TcpStatus tcp_ssl_connect(TcpSocket socket, const char* host)`

**Purpose**:  
This function initiates an SSL/TLS connection to a remote host. It sets up the necessary file descriptor, performs the SSL handshake, and sets the Server Name Indication (SNI) hostname.

**Parameters**:  
- `socket`: The TCP socket through which the SSL/TLS connection is to be established.
- `host`: The remote hostname to connect to, used for SNI and resolving the server.

**Return Value**:  
- `TCP_SUCCESS`: SSL connection was established successfully.
- `TCP_ERR_SETUP`: SSL context is not initialized.
- `TCP_ERR_RESOLVE`: The host parameter is invalid.
- `TCP_ERR_SSL`: Failed to create or set up the SSL object.
- `TCP_ERR_SSL_HANDSHAKE`: SSL handshake failed.

**Usage Case**:  
Used to establish secure communication with a remote server using SSL/TLS after setting up a TCP socket.

---

### `TcpStatus tcp_ssl_accept(TcpSocket socket)`

**Purpose**:  
This function performs the SSL/TLS handshake for a server-side connection, accepting a client connection on the specified TCP socket.

**Parameters**:  
- `socket`: The TCP socket that is awaiting an SSL/TLS connection.

**Return Value**:  
- `TCP_SUCCESS`: SSL handshake completed successfully.
- `TCP_ERR_SETUP`: SSL context is not initialized.
- `TCP_ERR_SSL`: Failed to associate the socket with an SSL object.
- `TCP_ERR_SSL_HANDSHAKE`: SSL handshake failed.

**Usage Case**:  
Used by servers to establish a secure SSL/TLS connection after a client connects to a listening TCP socket.

---

### `TcpStatus tcp_ssl_close(TcpSocket socket)`

**Purpose**:  
This function initiates the SSL/TLS shutdown sequence for the specified socket and frees the associated SSL object. It also closes the underlying TCP socket.

**Parameters**:  
- `socket`: The TCP socket that has an active SSL connection.

**Return Value**:  
- `TCP_SUCCESS`: SSL shutdown and socket closure were successful.
- `TCP_ERR_NO_SSL`: No SSL object was found for the socket, but the socket was closed normally.

**Usage Case**:  
Called to close an SSL/TLS connection cleanly and ensure both the SSL session and TCP socket are closed properly.

---

### `TcpStatus tcp_ssl_send(TcpSocket socket, const void* buf, size_t len, size_t* sent)`

**Purpose**:  
This function sends data over an SSL-encrypted TCP connection. It writes the provided data to the associated SSL connection and handles various SSL errors that may occur during the operation.

**Parameters**:  
- `socket`: The TCP socket through which data is sent.
- `buf`: Pointer to the buffer containing the data to send.
- `len`: Size of the data (in bytes) to be sent.
- `sent`: Pointer to a variable where the number of bytes sent will be stored (optional).

**Return Value**:  
- `TCP_SUCCESS`: Data was successfully sent or the operation needs to be retried.
- `TCP_ERR_SEND`: There was an error during the send operation.
- `TCP_ERR_NO_SSL`: No SSL context was found for the socket.
- `TCP_ERR_CLOSE`: The connection was closed by the peer.

**Usage Case**:  
Use this function to send data securely over an SSL connection, such as for HTTPS or other SSL/TLS-based protocols.

---

### `TcpStatus tcp_ssl_recv(TcpSocket socket, void* buf, size_t len, size_t* received)`

**Purpose**:  
This function receives data over an SSL-encrypted TCP connection. It reads data from the associated SSL connection and handles various SSL errors that may occur during the operation.

**Parameters**:  
- `socket`: The TCP socket from which to receive data.
- `buf`: Pointer to the buffer where the received data will be stored.
- `len`: Size of the buffer (in bytes).
- `received`: Pointer to a variable where the number of bytes received will be stored (optional).

**Return Value**:  
- `TCP_SUCCESS`: Data was successfully received or the operation needs to be retried.
- `TCP_ERR_RECV`: There was an error during the receive operation.
- `TCP_ERR_NO_SSL`: No SSL context was found for the socket.
- `TCP_ERR_CLOSE`: The connection was closed by the peer.

**Usage Case**:  
Use this function to securely receive data over an SSL connection, typically used for encrypted communications like HTTPS.

---

### `TcpStatus tcp_get_connection_quality(TcpSocket socket, float* rtt, float* variance)`

**Purpose**:  
This function retrieves TCP connection quality metrics such as round-trip time (RTT) and its variance for a given socket.

**Parameters**:  
- `socket`: The TCP socket for which to retrieve connection quality metrics.
- `rtt`: Pointer to a float where the round-trip time (RTT) in milliseconds will be stored.
- `variance`: Pointer to a float where the RTT variance (in milliseconds) will be stored.

**Return Value**:  
- `TCP_SUCCESS`: Successfully retrieved RTT and variance.
- `TCP_ERR_GENERIC`: Failed to retrieve TCP connection quality.
- `TCP_ERR_UNSUPPORTED`: The platform does not support RTT measurement.

**Usage Case**:  
Use this function to measure the connection quality for latency-sensitive applications such as video streaming or gaming.

---

### `TcpStatus tcp_async_send(TcpSocket socket, const void* buf, size_t len)`

**Purpose**:  
This function sends data asynchronously through a non-blocking TCP socket. It ensures the function does not block if the send operation would normally block.

**Parameters**:  
- `socket`: The non-blocking TCP socket to send data through.
- `buf`: Pointer to the buffer containing the data to send.
- `len`: The number of bytes to send from the buffer.

**Return Value**:  
- `TCP_SUCCESS`: Data was sent successfully.
- `TCP_ERR_SEND`: An error occurred during sending.
- `TCP_ERR_WOULD_BLOCK`: The operation would block, but the send was not completed.

**Usage Case**:  
Useful for applications that need to send data asynchronously, ensuring the application remains responsive without blocking the send operation.

---

### `TcpStatus tcp_async_recv(TcpSocket socket, void* buf, size_t len)`

**Purpose**:  
This function receives data asynchronously from a non-blocking TCP socket. It ensures the function does not block if no data is available.

**Parameters**:  
- `socket`: The non-blocking TCP socket to receive data from.
- `buf`: Pointer to the buffer where the received data will be stored.
- `len`: The maximum number of bytes to read into the buffer.

**Return Value**:  
- `TCP_SUCCESS`: Data was received successfully.
- `TCP_ERR_RECV`: An error occurred during receiving.
- `TCP_ERR_WOULD_BLOCK`: No data is available, and the operation would block.

**Usage Case**:  
This function is used for non-blocking operations where the application must remain responsive even when no data is available to be received immediately.

---

## Example 1 : First server in `TcpSocket`

`This server listens on a specified port and echoes back any received data to the client. It demonstrates basic server setup, including initialization, socket creation, binding, listening, accepting connections, receiving data, and sending data back.`

```c
#include "fmt/fmt.h"
#include "network/tcp.h"

#define PORT "8080" 
#define BUFFER_SIZE 1024 

int main() {
    TcpSocket listen_socket, client_socket;
    TcpStatus status;
    char recv_buffer[BUFFER_SIZE];
    size_t received, sent;

    // Initialize network API on Windows
    status = tcp_init();
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Network initialization failed.\n");
        return 1;
    }

    // Create TCP socket
    status = tcp_socket_create(&listen_socket);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Socket creation failed.\n");
        tcp_cleanup();
        return 1;
    }

    // Bind socket to local port
    status = tcp_bind(listen_socket, "0.0.0.0", atoi(PORT));
    if (status != TCP_SUCCESS) {
        fprintf(stderr, "Socket bind failed.\n");
        tcp_close(listen_socket);
        tcp_cleanup();
        return 1;
    }

    // Listen for incoming connections
    status = tcp_listen(listen_socket, 10); // Backlog of 10 connections
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Listen failed.\n");
        tcp_close(listen_socket);
        tcp_cleanup();
        return 1;
    }

    fmt_printf("Server listening on port %s\n", PORT);

    // Accept a connection (blocking call)
    status = tcp_accept(listen_socket, &client_socket);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Accept failed.\n");
        tcp_close(listen_socket);
        tcp_cleanup();
        return 1;
    }

    fmt_printf("Client connected.\n");

    // Echo loop: receive data and send it back
    while ((status = tcp_recv(client_socket, recv_buffer, BUFFER_SIZE, &received)) == TCP_SUCCESS && received > 0) {
        recv_buffer[received] = '\0'; 
        fmt_printf("Received: %s\n", recv_buffer);

        status = tcp_send(client_socket, recv_buffer, received, &sent);
        if (status != TCP_SUCCESS || sent != received) {
            fmt_fprintf(stderr, "Send failed.\n");
            break;
        }
    }

    fmt_printf("Closing connection.\n");
    tcp_close(client_socket);
    tcp_close(listen_socket);
    tcp_cleanup();

    return 0;
}
```

---

## Example 2 : client Echo with python example  `TcpSocket`

`A C client connects to a Python server, sends a message, and prints the echoed message received from the server. This example illustrates how to create a TCP client, connect to a server, send and receive data.`

```c
#include "fmt/fmt.h"
#include "network/tcp.h"

#define SERVER_IP "127.0.0.1" 
#define SERVER_PORT "8080"    
#define BUFFER_SIZE 1024      

int main() {
    TcpSocket socket;
    TcpStatus status;
    char send_buffer[BUFFER_SIZE] = "Hello, TCP Server!";
    char recv_buffer[BUFFER_SIZE];
    size_t sent, received;

    // Initialize network API on Windows
    status = tcp_init();
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Network initialization failed.\n");
        return 1;
    }

    // Create TCP socket
    status = tcp_socket_create(&socket);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Socket creation failed.\n");
        tcp_cleanup();
        return 1;
    }

    // Connect to the server
    status = tcp_connect(socket, SERVER_IP, atoi(SERVER_PORT));
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Connection to server failed.\n");
        tcp_close(socket);
        tcp_cleanup();
        return 1;
    }

    fmt_printf("Connected to server at %s:%s\n", SERVER_IP, SERVER_PORT);

    // Send message to the server
    status = tcp_send(socket, send_buffer, strlen(send_buffer), &sent);
    if (status != TCP_SUCCESS || sent != strlen(send_buffer)) {
        fmt_fprintf(stderr, "Failed to send data.\n");
        tcp_close(socket);
        tcp_cleanup();
        return 1;
    }

    fmt_printf("Sent: %s\n", send_buffer);

    // Receive echo back from the server
    status = tcp_recv(socket, recv_buffer, BUFFER_SIZE, &received);
    if (status == TCP_SUCCESS && received > 0) {
        recv_buffer[received] = '\0'; 
        fmt_printf("Received: %s\n", recv_buffer);
    } 
    else {
        fmt_fprintf(stderr, "Failed to receive data.\n");
    }

    // Clean up
    tcp_close(socket);
    tcp_cleanup();

    return 0;
}
```

`Python Server Code`

```py

import socket

SERVER_IP = '127.0.0.1'
SERVER_PORT = 8080
BUFFER_SIZE = 1024

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((SERVER_IP, SERVER_PORT))
        server_socket.listen(1)
        print(f"Server listening on {SERVER_IP}:{SERVER_PORT}")

        conn, addr = server_socket.accept()
        with conn:
            print(f"Connected by {addr}")
            while True:
                data = conn.recv(BUFFER_SIZE)
                if not data:
                    break  
                print(f"Received: {data.decode('utf-8')}")
                conn.sendall(data)  # Echo back to client

if __name__ == "__main__":
    main()
```

---

## Example 3 : Non Blocking `TcpSocket` client 

`This client attempts to connect to a server in non-blocking mode, sending a message and waiting for a response without blocking the execution. It shows how to set a socket to non-blocking mode and handle send/receive operations accordingly.`

```c
#include "fmt/fmt.h"
#include "network/tcp.h"
#include "time/time.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MESSAGE "Hello, non-blocking world!"

int main() {
    TcpSocket socket;
    TcpStatus status;

    // Initialize network API
    status = tcp_init();
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Network initialization failed.\n");
        return -1;
    }

    // Create TCP socket
    status = tcp_socket_create(&socket);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Socket creation failed.\n");
        tcp_cleanup();
        return -1;
    }

    // Set the socket to non-blocking mode
    status = tcp_set_non_blocking(socket, true);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to set socket to non-blocking mode.\n");
        tcp_close(socket);
        tcp_cleanup();
        
        return -1;
    }

    // Attempt to connect to the server
    status = tcp_connect(socket, SERVER_IP, SERVER_PORT);
    if (status != TCP_SUCCESS && status != TCP_ERR_CONNECT) {
        fmt_fprintf(stderr, "Connection attempt failed.\n");
        tcp_close(socket);
        tcp_cleanup();
        return -1;
    }

    fmt_fprintf(stdout, "Connection attempt initiated (non-blocking)...\n");

    // Non-blocking send and receive loop
    char buffer[1024];
    size_t sent, received;
    bool messageSent = false;
    bool responseReceived = false;

    while (!messageSent || !responseReceived) {
        if (!messageSent) {
            status = tcp_send(socket, MESSAGE, strlen(MESSAGE), &sent);
            if (status == TCP_SUCCESS) {
                fmt_fprintf(stdout, "Message sent: %s\n", MESSAGE);
                messageSent = true;
            }
        }

        if (!responseReceived) {
            status = tcp_recv(socket, buffer, sizeof(buffer) - 1, &received);
            if (status == TCP_SUCCESS && received > 0) {
                buffer[received] = '\0'; 
                fmt_fprintf(stdout, "Received response: %s\n", buffer);
                responseReceived = true;
            }
        }
        time_sleep(1);
    }

    // Cleanup
    tcp_close(socket);
    tcp_cleanup();

    return 0;
}
```

---

## Example 4 : get remote and local address with tcp server in `TcpSocket` 

`A server that prints its own and the client's address upon connection. This demonstrates how to retrieve and display the local and remote addresses of a connection`

```c
#include "fmt/fmt.h"
#include "network/tcp.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    TcpSocket listen_socket, client_socket;
    TcpStatus status;
    char client_ip[INET6_ADDRSTRLEN];
    unsigned short client_port;
    char server_ip[INET6_ADDRSTRLEN];
    unsigned short server_port;

    status = tcp_init();
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to initialize network.\n");
        return 1;
    }

    status = tcp_socket_create(&listen_socket);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to create socket.\n");
        tcp_cleanup();
        return 1;
    }

    status = tcp_bind(listen_socket, NULL, PORT);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to bind socket.\n");
        tcp_close(listen_socket);
        tcp_cleanup();
        return 1;
    }

    status = tcp_listen(listen_socket, 5);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to listen on socket.\n");
        tcp_close(listen_socket);
        tcp_cleanup();
        return 1;
    }

    status = tcp_get_local_address(listen_socket, server_ip, sizeof(server_ip), &server_port);
    if (status == TCP_SUCCESS) {
        fmt_fprintf(stdout, "Server is listening on %s:%u\n", server_ip, server_port);
    } 
    else {
        fmt_fprintf(stderr, "Failed to get local address.\n");
    }

    fmt_fprintf(stdout, "Waiting for a connection...\n");
    status = tcp_accept(listen_socket, &client_socket);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to accept connection.\n");
        tcp_close(listen_socket);
        tcp_cleanup();
        return 1;
    }

    status = tcp_get_remote_address(client_socket, client_ip, sizeof(client_ip), &client_port);
    if (status == TCP_SUCCESS) {
        fmt_fprintf(stdout, "Client connected from %s:%u\n", client_ip, client_port);
    } 
    else {
        fmt_fprintf(stderr, "Failed to get client's address.\n");
    }

    tcp_close(client_socket);
    tcp_close(listen_socket);
    tcp_cleanup();

    return 0;
}
```

`python client for connect to server for getting remote address and port`

```py
import socket

SERVER_IP = '127.0.0.1' 
SERVER_PORT = 8080
MESSAGE = "Hello from Python client!"

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        try:
            sock.connect((SERVER_IP, SERVER_PORT))
            print(f"Connected to {SERVER_IP}:{SERVER_PORT}")

            sock.sendall(MESSAGE.encode('utf-8'))
            print(f"Sent: {MESSAGE}")

        except socket.error as e:
            print(f"Socket error: {e}")

        finally:
            sock.close()
            print("Connection closed.")

if __name__ == "__main__":
    main()
```

---

## Example 5 : `SSl` Echo Server in `TcpSocket`

*Create server.ccrt and server.key*
`openssl req -x509 -newkey rsa:4096 -keyout server.key -out server.crt -days 365 -nodes -subj "/C=US/ST=YourState/L=YourCity/O=YourOrganization/CN=yourdomain.com"`

*ssl client connect to our echo server*
`openssl s_client -connect localhost:8443 -debug -showcerts`

`An SSL/TLS server that echoes back received data over a secure connection. This example covers initializing SSL, creating an SSL-enabled socket, performing SSL handshakes, and secure data transmission.`

```c
#include "fmt/fmt.h"
#include "network/tcp.h"

#define PORT "8443"
#define BUFFER_SIZE 1024
#define CERT_FILE "./server.crt"
#define KEY_FILE "./server.key"

int main() {
    TcpSocket listen_socket, client_socket;
    TcpStatus status;
    char recv_buffer[BUFFER_SIZE];
    size_t received, sent;

    // Initialize network and SSL libraries
    status = tcp_init();
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Network initialization failed.\n");
        return 1;
    }

    status = tcp_ssl_init(CERT_FILE, KEY_FILE);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "SSL initialization failed.\n");
        tcp_cleanup();
        return 1;
    }

    // Create TCP socket
    status = tcp_socket_create(&listen_socket);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Socket creation failed.\n");
        tcp_ssl_cleanup();
        tcp_cleanup();
        return 1;
    }

    // Enable SSL for the socket
    status = tcp_enable_ssl(listen_socket);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Enabling SSL failed.\n");
        tcp_close(listen_socket);
        tcp_ssl_cleanup();
        tcp_cleanup();
        return 1;
    }

    // Bind socket to local port
    status = tcp_bind(listen_socket, "0.0.0.0", atoi(PORT));
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Socket bind failed.\n");
        tcp_close(listen_socket);
        tcp_ssl_cleanup();
        tcp_cleanup();
        return 1;
    }

    // Listen for incoming connections
    status = tcp_listen(listen_socket, 10); // Backlog of 10 connections
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Listen failed.\n");
        tcp_close(listen_socket);
        tcp_ssl_cleanup();
        tcp_cleanup();
        return 1;
    }

    fmt_printf("SSL Server listening on port %s\n", PORT);

    // Accept a connection (blocking call)
    status = tcp_accept(listen_socket, &client_socket);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Accept failed.\n");
        tcp_close(listen_socket);
        tcp_ssl_cleanup();
        tcp_cleanup();
        return 1;
    }

    // Perform SSL handshake
    status = tcp_ssl_accept(client_socket);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "SSL handshake failed.\n");
        tcp_close(client_socket);
        tcp_close(listen_socket);
        tcp_ssl_cleanup();
        tcp_cleanup();
        return 1;
    }

    fmt_printf("SSL Client connected.\n");

    // SSL echo loop: receive data and send it back
    while ((status = tcp_ssl_recv(client_socket, recv_buffer, BUFFER_SIZE, &received)) == TCP_SUCCESS && received > 0) {
        recv_buffer[received] = '\0';
        fmt_printf("Received: %s\n", recv_buffer);

        status = tcp_ssl_send(client_socket, recv_buffer, received, &sent);
        if (status != TCP_SUCCESS || sent != received) {
            fmt_fprintf(stderr, "SSL send failed.\n");
            break;
        }
    }

    fmt_printf("Closing SSL connection.\n");
    tcp_ssl_close(client_socket);
    tcp_close(listen_socket);
    tcp_ssl_cleanup();
    tcp_cleanup();

    return 0;
}
```

---

## Example 6 : concurrent server in `TcpSocket`

`This example demonstrates how to set up a secure SSL/TCP server using a custom TCP library, which abstracts the complexity of socket programming and SSL communication. The example covers initializing the network environment, creating a non-blocking TCP socket, configuring SSL, and handling client connections in a multithreaded manner. This approach allows for scalable server applications that can handle multiple client connections concurrently without blocking the main execution thread.`

```c
#include "fmt/fmt.h"
#include "network/tcp.h"
#include "concurrent/concurrent.h"

#define PORT "8443"
#define BUFFER_SIZE 1024
#define CERT_FILE "./server.crt"
#define KEY_FILE "./server.key"

int handle_client(void* arg);

int main() {
    TcpSocket listen_socket;
    
    tcp_init();
    tcp_ssl_init(CERT_FILE, KEY_FILE);

    tcp_socket_create(&listen_socket);
    tcp_set_non_blocking(listen_socket, true);
    tcp_set_reuse_addr(listen_socket, true);
    tcp_bind(listen_socket, NULL, atoi(PORT));
    tcp_listen(listen_socket, 10);

    fmt_printf("SSL Echo Server listening on port %s\n", PORT);

    fd_set master_set;
    FD_ZERO(&master_set);
    FD_SET(listen_socket, &master_set);
    int max_sd = listen_socket;

    while (true) {
        fd_set read_fds = master_set;
        struct timeval timeout;
        timeout.tv_sec = 0; // No wait timeout
        timeout.tv_usec = 10000; // 10ms

        int activity = select(max_sd + 1, &read_fds, NULL, NULL, &timeout);

        if ((activity < 0) && (errno != EINTR)) {
            fmt_printf("select error\n");
        }

        if (FD_ISSET(listen_socket, &read_fds)) {
            TcpSocket* client_socket = malloc(sizeof(TcpSocket)); // Ensure proper error checking in production code
            TcpStatus acceptStatus = tcp_accept(listen_socket, client_socket);
            if (acceptStatus == TCP_SUCCESS) {
                Thread thread_id = NULL;
                thread_create(&thread_id, handle_client, client_socket);
                thread_detach(thread_id);
            } 
            else {
                free(client_socket);
            }
        }
    }

    tcp_close(listen_socket);
    tcp_ssl_cleanup();
    tcp_cleanup();
    return 0;
}

int handle_client(void* arg) {
    TcpSocket client_socket = *(TcpSocket*)arg;
    free(arg); // Free the argument as soon as possible to avoid memory leaks

    tcp_enable_ssl(client_socket);
    if (tcp_ssl_accept(client_socket) != TCP_SUCCESS) {
        tcp_ssl_close(client_socket);
        tcp_close(client_socket);
        return -1;
    }

    char buffer[BUFFER_SIZE];
    size_t received, sent;

    while (tcp_ssl_recv(client_socket, buffer, BUFFER_SIZE, &received) == TCP_SUCCESS && received > 0) {
        tcp_ssl_send(client_socket, buffer, received, &sent);
    }

    tcp_ssl_close(client_socket);
    tcp_close(client_socket);

    return 0;
}
```

---

## Example 7 : Simple HttpServer with `TcpSocket`\

`This example demonstrates how to create a simple HTTP server in C using the custom TcpSocket and concurrent libraries. The server listens on port 8080 and responds to incoming HTTP requests with a basic HTTP response message. This server is capable of handling multiple client connections concurrently through the use of threads.`

```c
#include "network/tcp.h"
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

int handle_client(void* arg) {
    TcpSocket client_socket = *(TcpSocket*)arg;
    free(arg); // Free the dynamically allocated memory for the socket

    char buffer[BUFFER_SIZE] = {0};
    size_t received, sent;

    if (tcp_recv(client_socket, buffer, BUFFER_SIZE, &received) == TCP_SUCCESS) {
        fmt_printf("Request:\n%s\n", buffer);

        // Simple HTTP response
        const char* httpResponse = "HTTP/1.1 200 OK\r\n"
                                    "Content-Type: text/plain\r\n"
                                    "Connection: close\r\n"
                                    "\r\n"
                                    "Hello, world! This is a simple HTTP server response.\r\n";

        tcp_send(client_socket, httpResponse, strlen(httpResponse), &sent);
    }

    tcp_close(client_socket);
    fmt_printf("Client disconnected.\n");
    
    return TCP_SUCCESS;
}

int main(void) {
    TcpSocket listen_socket;
    TcpStatus status = tcp_init();

    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to initialize TCP.\n");
        return 1;
    }

    status = tcp_socket_create(&listen_socket);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to create socket.\n");
        tcp_cleanup();
        return 1;
    }

    status = tcp_bind(listen_socket, NULL, SERVER_PORT);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to bind socket.\n");
        tcp_close(listen_socket);
        tcp_cleanup();
        return 1;
    }

    status = tcp_listen(listen_socket, 10);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Failed to listen on socket.\n");
        tcp_close(listen_socket);
        tcp_cleanup();
        return 1;
    }

    fmt_printf("HTTP Server listening on port %d\n", SERVER_PORT);

    while (1) {
        TcpSocket* client_socket = (TcpSocket*) malloc(sizeof(TcpSocket));
        if (client_socket == NULL) {
            fmt_fprintf(stderr, "Failed to allocate memory for client socket.\n");
            continue;
        }

        if (tcp_accept(listen_socket, client_socket) == TCP_SUCCESS) {
            Thread client_thread;
            if (thread_create(&client_thread, handle_client, client_socket) != THREAD_SUCCESS) {
                fmt_fprintf(stderr, "Failed to create thread.\n");
                free(client_socket); 
            }
            thread_detach(client_thread);
        } 
        else {
            free(client_socket); 
        }
    }

    tcp_close(listen_socket);
    tcp_cleanup();

    return 0;
}
```

---

## Example 8: Asynchronous File Server using `TcpSocket`

`This example demonstrates the creation of an asynchronous file server in C, utilizing the custom TcpSocket, fmt, concurrent, and file_io libraries. The server is designed to listen for incoming file requests on port 8080 and serve the requested files back to the client efficiently and concurrently. This example is particularly useful for understanding how to integrate network programming with file I/O and threading for scalable server applications.`

```c
#include "network/tcp.h"
#include "fmt/fmt.h"
#include "concurrent/concurrent.h"
#include "file_io/file_reader.h"

#define SERVER_PORT 8080
#define BUFFER_SIZE 4096
#define MAX_FILENAME_LENGTH 256

int handle_client(void* arg) {
    TcpSocket client_socket = *(TcpSocket*)arg;
    free(arg); 

    char request[BUFFER_SIZE] = {0};
    size_t received;

    // Receive the file request from the client
    if (tcp_recv(client_socket, request, BUFFER_SIZE, &received) != TCP_SUCCESS || received == 0) {
        fmt_fprintf(stderr, "Failed to receive data from client.\n");
        tcp_close(client_socket);
        return -1;
    }

    // Extract the filename from the request
    char* token = strtok(request, " ");
    if (token == NULL || strcmp(token, "GET") != 0) {
        fmt_fprintf(stderr, "Invalid request format.\n");
        tcp_close(client_socket);
        return -1;
    }

    char* filename = strtok(NULL, " ");
    if (filename == NULL) {
        fmt_fprintf(stderr, "Filename not specified in request.\n");
        tcp_close(client_socket);
        return -1;
    }

    // Attempt to open the requested file
    FileReader* reader = file_reader_open(filename, READ_BINARY);
    if (!reader) {
        char* errMsg = "File not found.\n";
        tcp_send(client_socket, errMsg, strlen(errMsg), NULL);
        tcp_close(client_socket);
        return -1;
    }

    // Read the file content and send it to the client
    char file_buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = file_reader_read(file_buffer, 1, BUFFER_SIZE, reader)) > 0) {
        tcp_send(client_socket, file_buffer, bytes_read, NULL);
    }

    file_reader_close(reader);
    tcp_close(client_socket);
    fmt_printf("File \"%s\" sent successfully.\n", filename);

    return 0;
}

int main(void) {
    TcpSocket listen_socket;
    tcp_init();
    tcp_socket_create(&listen_socket);
    tcp_bind(listen_socket, NULL, SERVER_PORT);
    tcp_listen(listen_socket, 10);

    fmt_printf("File Server listening on port %d\n", SERVER_PORT);

    while (1) {
        TcpSocket* client_socket = malloc(sizeof(TcpSocket));
        if (tcp_accept(listen_socket, client_socket) == TCP_SUCCESS) {
            Thread client_thread = NULL;
            thread_create(&client_thread, handle_client, client_socket);
            thread_detach(client_thread);
        } 
        else {
            free(client_socket);
        }
    }

    tcp_close(listen_socket);
    tcp_cleanup();

    return 0;
}
```

*Client Request in Python*

```py
import socket

# Server configuration
SERVER_IP = '127.0.0.1'
SERVER_PORT = 8080

def request_file(filename):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        sock.connect((SERVER_IP, SERVER_PORT))
        print(f"Connected to {SERVER_IP}:{SERVER_PORT}")

        request_message = f"GET {filename}"
        sock.sendall(request_message.encode('utf-8'))
        print(f"Requested file: {filename}")

        with open(f"received_{filename}", 'wb') as file:
            while True:
                data = sock.recv(4096)
                if not data:
                    break 
                file.write(data)
        
        print(f"File received and saved as received_{filename}")

    except socket.error as e:
        print(f"Socket error: {e}")
    finally:
        sock.close()
        print("Connection closed.")

if __name__ == "__main__":
    filename = input("Enter the filename to request: ")
    request_file(filename)
```

---

## Example 9 : HTTP Server Example

This example demonstrates how to create a simple HTTP server in C using a custom HTTP library. The server listens on port 8051 and provides two endpoints:

- `/`: Responds with a simple "Hello, World!" message.
- `/echo`: Receives a JSON payload via a POST request and echoes it back in the response.

**Test the endpoints**:
     - `curl http://localhost:8051/` should return "Hello, World!".
     - `curl -X POST http://localhost:8051/echo -H "Content-Type: application/json" -d '{"message": "Hello, world!"}'` should return the JSON message `{"message": "Hello, world!"}`.

### Code

```c
#include "network/http.h"
#include "fmt/fmt.h"
#include <stdlib.h>

void handle_root(HttpRequest* req, HttpResponse* res) {
    (void)req;
    http_set_status(res, 200, "OK");
    http_set_body(res, "Hello, World!");
}

void handle_echo(HttpRequest* req, HttpResponse* res) {
    fmt_printf("Received body: %s\n", req->body);

    if (req->json_body) {
        fmt_printf("Debug: Valid JSON body detected\n");
        http_set_status(res, 200, "OK");
        http_set_json_body(res, json_deep_copy(req->json_body));
    } 
    else {
        http_send_error(res, 400, "Bad Request: Expected JSON body");
    }
}

int main() {
    http_register_route("/", HTTP_GET, handle_root);
    http_register_route("/echo", HTTP_POST, handle_echo);

    fmt_printf("Starting HTTP server on port 8051...\n");
    http_start_server(8051);

    return 0;
}
```

---

## Example 10 : json api with http and json (add user and get list of users)

- `/users`: this endpoint base on GET method return list of users
- `/users`: this handler get user as json and add to User struct .

**Test the endpoints**:
     - `curl http://localhost:8051/users` should return "Hello, World!".
     - `curl -X POST http://localhost:8051/users -H "Content-Type: application/json" -d '{"name": "amin", "age": 27}'` should return {"message": "User added successfully"}
### Code

```c
#include "network/http.h"
#include "fmt/fmt.h"
#include "json/json.h"
#include "string/std_string.h"


typedef struct {
    char* name;
    int age;
} User;


static User users[10]; 
static size_t user_count = 0;

void handle_get_users(HttpRequest* req, HttpResponse* res) {
    (void)req; 

    JsonElement* users_array = json_create(JSON_ARRAY);
    for (size_t i = 0; i < user_count; i++) {
        JsonElement* user_obj = json_create(JSON_OBJECT);

        JsonElement* name_element = json_create(JSON_STRING);
        name_element->value.string_val = string_strdup(users[i].name);
        json_set_element(user_obj, "name", name_element);

        JsonElement* age_element = json_create(JSON_NUMBER);
        age_element->value.number_val = users[i].age;
        json_set_element(user_obj, "age", age_element);

        json_add_to_array(users_array, user_obj);
    }

    JsonElement* response_json = json_create(JSON_OBJECT);
    json_set_element(response_json, "users", users_array);

    http_set_status(res, 200, "OK");
    http_set_json_body(res, response_json);
}

void handle_add_user(HttpRequest* req, HttpResponse* res) {
    if (req->json_body) {
        JsonElement* name_element = json_get_element(req->json_body, "name");
        JsonElement* age_element = json_get_element(req->json_body, "age");

        if (name_element && name_element->type == JSON_STRING &&
            age_element && age_element->type == JSON_NUMBER &&
            user_count < 10) {

            const char* name = name_element->value.string_val;
            int age = (int)age_element->value.number_val;

            users[user_count].name = string_strdup(name);
            users[user_count].age = age;
            user_count++;

            http_set_status(res, 201, "User Created");

            JsonElement* response_json = json_create(JSON_OBJECT);
            JsonElement* message_element = json_create(JSON_STRING);
            
            message_element->value.string_val = string_strdup("User added successfully");
            json_set_element(response_json, "message", message_element);
            http_set_json_body(res, response_json);
        } 
        else {
            http_send_error(res, 400, "Bad Request: Invalid user data or user limit reached");
        }

    } else {
        http_send_error(res, 400, "Bad Request: Expected JSON body");
    }
}

int main() {
    http_register_route("/users", HTTP_GET, handle_get_users);
    http_register_route("/users", HTTP_POST, handle_add_user);

    fmt_printf("Starting HTTP server on port 8051...\n");
    http_start_server(8051);

    return 0;
}
```

---

## Example 11 : Create Crud operation and api with http lib (POST, GET, PUT, DELETE)

- `/users`: this endpoint base on GET method return list of users
- `/users`: this handler get user as json and add to User struct .
- `/users/{id}` : this api update user base on id 
- `/users/{id}` : this api delete user base on id

**Test the endpoints**:
- `curl http://localhost:8051/users` should return "Hello, World!".
- `curl -X POST http://localhost:8051/users -H "Content-Type: application/json" -d '{"id": 1, "name": "amin", "age": 27}'` should return {"message": "User added successfully"}
- `curl -X PUT http://localhost:8051/users/1 -H "Content-Type: application/json" -d '{"name": "ali", "age": 25}'` should return {"message": "User updated successfully"}
- `curl -X DELETE http://localhost:8051/users/1` should return {"message": "User deleted successfully"}

    
### code 

```c
#include "network/http.h"
#include "fmt/fmt.h"
#include "json/json.h"
#include "string/std_string.h"

typedef struct {
    int id;
    char* name;
    int age;
} User;

static User users[10];
static size_t user_count = 0;

User* find_user_by_id(int id) {
    for (size_t i = 0; i < user_count; i++) {
        if (users[i].id == id) {
            return &users[i];
        }
    }
    return NULL;
}

int extract_id_from_path(const char* path) {
    const char* last_slash = strrchr(path, '/');
    if (last_slash) {
        return atoi(last_slash + 1);
    }
    return -1;
}

void handle_get_users(HttpRequest* req, HttpResponse* res) {
    (void)req;
    JsonElement* users_array = json_create(JSON_ARRAY);
    for (size_t i = 0; i < user_count; i++) {
        JsonElement* user_obj = json_create(JSON_OBJECT);

        JsonElement* id_element = json_create(JSON_NUMBER);
        id_element->value.number_val = users[i].id;
        json_set_element(user_obj, "id", id_element);

        JsonElement* name_element = json_create(JSON_STRING);
        name_element->value.string_val = string_strdup(users[i].name);
        json_set_element(user_obj, "name", name_element);

        JsonElement* age_element = json_create(JSON_NUMBER);
        age_element->value.number_val = users[i].age;
        json_set_element(user_obj, "age", age_element);

        json_add_to_array(users_array, user_obj);
    }

    JsonElement* response_json = json_create(JSON_OBJECT);
    json_set_element(response_json, "users", users_array);

    http_set_status(res, 200, "OK");
    http_set_json_body(res, response_json);
}

void handle_add_user(HttpRequest* req, HttpResponse* res) {
    if (req->json_body) {
        JsonElement* id_element = json_get_element(req->json_body, "id");
        JsonElement* name_element = json_get_element(req->json_body, "name");
        JsonElement* age_element = json_get_element(req->json_body, "age");

        if (id_element && id_element->type == JSON_NUMBER &&
            name_element && name_element->type == JSON_STRING &&
            age_element && age_element->type == JSON_NUMBER &&
            user_count < 10) {

            int id = (int)id_element->value.number_val;
            if (find_user_by_id(id) == NULL) {
                User new_user;
                new_user.id = id;
                new_user.name = string_strdup(name_element->value.string_val);
                new_user.age = (int)age_element->value.number_val;

                users[user_count++] = new_user;

                http_set_status(res, 201, "User Created");

                JsonElement* response_json = json_create(JSON_OBJECT);
                JsonElement* message_element = json_create(JSON_STRING);

                message_element->value.string_val = string_strdup("User added successfully");

                json_set_element(response_json, "message", message_element);
                http_set_json_body(res, response_json);
            } 
            else {
                http_send_error(res, 400, "User already exists with this ID");
            }
        } 
        else {
            http_send_error(res, 400, "Bad Request: Invalid user data or user limit reached");
        }
    } 
    else {
        http_send_error(res, 400, "Bad Request: Expected JSON body");
    }
}

void handle_update_user(HttpRequest* req, HttpResponse* res) {
    int id = extract_id_from_path(req->path);

    if (id != -1 && req->json_body) {
        User* user = find_user_by_id(id);
        
        if (user) {
            JsonElement* name_element = json_get_element(req->json_body, "name");
            JsonElement* age_element = json_get_element(req->json_body, "age");

            if (name_element && name_element->type == JSON_STRING &&
                age_element && age_element->type == JSON_NUMBER) {

                user->name = string_strdup(name_element->value.string_val);
                if (user->name == NULL) {
                    http_send_error(res, 500, "Internal Server Error: Memory allocation failed");
                    return;
                }
                user->age = (int)age_element->value.number_val;
                http_set_status(res, 200, "User Updated");

                JsonElement* response_json = json_create(JSON_OBJECT);
                JsonElement* message_element = json_create(JSON_STRING);

                message_element->value.string_val = string_strdup("User updated successfully");

                json_set_element(response_json, "message", message_element);
                http_set_json_body(res, response_json);
            } 
            else {
                http_send_error(res, 400, "Bad Request: Invalid user data");
            }
        } 
        else {
            http_send_error(res, 404, "User Not Found");
        }
    } 
    else {
        http_send_error(res, 400, "Bad Request: Missing user ID or JSON body");
    }
}

void handle_delete_user(HttpRequest* req, HttpResponse* res) {
    int id = extract_id_from_path(req->path);
    if (id != -1) {
        for (size_t i = 0; i < user_count; i++) {
            if (users[i].id == id) {
                free(users[i].name);
                for (size_t j = i; j < user_count - 1; j++) {
                    users[j] = users[j + 1];
                }
                user_count--;

                http_set_status(res, 200, "User Deleted");

                JsonElement* response_json = json_create(JSON_OBJECT);
                JsonElement* message_element = json_create(JSON_STRING);

                message_element->value.string_val = string_strdup("User deleted successfully");
                json_set_element(response_json, "message", message_element);
                http_set_json_body(res, response_json);

                return;
            }
        }
        http_send_error(res, 404, "User Not Found");
    } 
    else {
        http_send_error(res, 400, "Bad Request: Missing user ID");
    }
}

int main() {
    http_register_route("/users", HTTP_GET, handle_get_users);
    http_register_route("/users", HTTP_POST, handle_add_user);
    http_register_route("/users/{id}", HTTP_PUT, handle_update_user);
    http_register_route("/users/{id}", HTTP_DELETE, handle_delete_user);

    fmt_printf("Starting HTTP server on port 8051...\n");
    http_start_server(8051);

    return 0;
}
```