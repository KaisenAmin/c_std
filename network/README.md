# Network Lib in C

**Author:** amin tahmasebi
**Release Date:** 2024
**License:** ISC License


## Thread Safety

All functions that access SSL context or SSL mappings are thread-safe. The library uses a cross-platform mutex to protect global state. You can safely use the TCP library in a multithreaded server or client.

## IPv6 Support

The functions `tcp_connect` and `tcp_bind` now support both IPv4 and IPv6 addresses. You can use hostnames or IPv6 literals (e.g., `::1`) in all connection and binding operations.

## Usage Patterns

- Always call `tcp_init()` before using any TCP functions, and `tcp_cleanup()` at program end.
- For SSL, call `tcp_ssl_init()` before using SSL functions, and `tcp_ssl_cleanup()` at program end.
- For multithreaded servers, you can safely call all TCP/SSL functions from multiple threads.
- For IPv6, simply use IPv6 addresses or hostnames that resolve to IPv6.

## Error Handling and Resource Management

All resource allocations and error paths are robust; SSL objects and mappings are always cleaned up. If an error occurs during SSL handshake or socket operations, all resources are released and mappings are cleared.

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

### `TcpStatus tcp_ssl_init_client(void)`

**Purpose**:  
Initializes a **client-side** SSL context (`TLS_client_method`) — no certificate or key required, unlike `tcp_ssl_init` (which builds a *server* context and mandates a cert/key). It also loads the system CA store so `tcp_ssl_get_verify_result` can confirm the peer's certificate chain. No-op if a context already exists. This is what makes outbound `https://` (and the HTTP client) possible.

**Parameters**:  
None.

**Return Value**:  
- `TCP_SUCCESS`: client context ready (or already initialized).  
- `TCP_ERR_SETUP`: failed to create the context.

**Usage Case**:  
Call once before `tcp_ssl_connect` on the client side (the HTTP client calls it for you).

---

### `long tcp_ssl_get_verify_result(TcpSocket socket)`

**Purpose**:  
Returns the verification result of the peer certificate chain after a handshake, so a client can enforce certificate validity.

**Parameters**:  
- `socket`: A socket on which `tcp_ssl_connect` has completed.

**Return Value**:  
- `0` (`X509_V_OK`): the chain verified.  
- non-zero: an `X509_V_*` failure code (untrusted, expired, self-signed, …).  
- `-1`: the socket has no SSL object.

**Usage Case**:  
After `tcp_ssl_connect`, reject the connection unless the result is `0` (what the HTTP client does in secure mode). Note: the system CA store is present on typical Linux installs but may be **empty on a bare MinGW/Windows** OpenSSL build, where verification fails closed. As of the hostname-verification update, `tcp_ssl_connect` binds the expected identity (the `host` argument — DNS SAN/CN, or the IP SAN for an IP literal), so `X509_V_OK` now also means the certificate is valid **for that host**, not merely a valid certificate for some domain. This closes the "any valid cert is accepted" MITM hole.

---

### `TcpStatus tcp_ssl_set_verify(bool verify_peer)`

**Purpose**:  
Make the TLS handshake itself **fail closed** on an untrusted or hostname-mismatched certificate (`SSL_VERIFY_PEER`). By default verification is off — the handshake succeeds and you inspect `tcp_ssl_get_verify_result` yourself.

**Parameters**:  
- `verify_peer`: `true` to require a valid, trusted, correctly-named certificate; `false` to restore the lenient default.

**Return Value**:  
`TCP_SUCCESS`, or `TCP_ERR_SETUP` if no SSL context has been created yet (call `tcp_ssl_init_client` first).

**Usage Case**:  
Production clients that should refuse to talk to a server presenting a bad certificate. Process-global (affects the shared context). `tcp_connect_tls` calls this for you.

---

### `TcpStatus tcp_ssl_load_verify_locations(const char* ca_file, const char* ca_path)`

**Purpose**:  
Load trusted CA certificates — a PEM bundle file and/or a hashed-cert directory — into the SSL context, for environments where the OS trust store is unavailable (notably a bare MinGW build).

**Parameters**:  
- `ca_file`: path to a PEM CA bundle (e.g. a shipped `cacert.pem`), or `NULL`.  
- `ca_path`: path to an OpenSSL hashed-cert directory, or `NULL`. **At least one must be non-NULL.**

**Return Value**:  
`TCP_SUCCESS`; `TCP_ERR_SETUP` (no context); `TCP_ERR_GENERIC` (both arguments `NULL`); `TCP_ERR_SSL` (the file/dir could not be loaded).

**Usage Case**:  
Ship a CA bundle with your Windows binary so `verify=true` HTTPS works without the OS store; or pin a private CA.

---

### `TcpStatus tcp_connect_tls(const char* host, unsigned short port, long timeout_ms, bool verify, TcpSocket* out_socket)`

**Purpose**:  
One-shot **secure client connect**: create a socket, connect to `host:port` within `timeout_ms`, then negotiate TLS with **SNI + certificate hostname verification** — all in a single call. Initializes the client SSL context for you.

**Parameters**:  
- `host` / `port`: the server to dial.  
- `timeout_ms`: connect timeout (`< 0` blocks).  
- `verify`: `true` requires the certificate chain **and** hostname to validate (fail-closed); `false` establishes TLS without trust checks (self-signed / testing).  
- `out_socket`: receives the ready TLS socket on success.

**Return Value**:  
`TCP_SUCCESS` with `*out_socket` set; otherwise an error (`TCP_ERR_CONNECT`, `TCP_ERR_SSL_HANDSHAKE`, `TCP_ERR_SSL`, …) and the socket is closed for you.

**Usage Case**:  
The production way to open a verified TLS connection. Read/write with `tcp_ssl_send` / `tcp_ssl_recv`; close with `tcp_ssl_close`.

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

### `const char* tcp_status_to_string(TcpStatus code)`

**Purpose**:  
Returns a static human-readable label for a `TcpStatus` error code.

**Parameters**:  
- `code`: Any `TcpStatus` enumeration value.

**Return Value**:  
Static string such as `"TCP_SUCCESS"`, `"TCP_ERR_BIND"`, or `"TCP_ERR_SSL_HANDSHAKE"`. Unknown codes return `"TCP_UNKNOWN_CODE"`. Never `NULL`.

**Usage Case**:  
Structured logging and diagnostics without pulling in the full `TcpStatusInfo` from `tcp_get_last_error`.

---

### `TcpStatus tcp_set_keep_alive(TcpSocket socket, bool enable)`

**Purpose**:  
Enables or disables `SO_KEEPALIVE` on the socket. When enabled, the kernel periodically probes idle connections and tears them down if the peer stops responding.

**Parameters**:  
- `socket`: The TCP socket to configure.  
- `enable`: `true` to enable keep-alive, `false` to disable.

**Return Value**:  
- `TCP_SUCCESS`: Option set successfully.  
- `TCP_ERR_GENERIC`: `setsockopt` failed.

**Usage Case**:  
Recommended for long-lived service connections so half-open connections (peer crashed, cable yanked) are detected and cleaned up rather than blocking `recv()` forever.

---

### `TcpStatus tcp_set_nodelay(TcpSocket socket, bool enable)`

**Purpose**:  
Enables or disables `TCP_NODELAY`, controlling Nagle's algorithm.

**Parameters**:  
- `socket`: The TCP socket to configure.  
- `enable`: `true` disables Nagle (small writes sent immediately); `false` re-enables coalescing.

**Return Value**:  
- `TCP_SUCCESS`: Option set successfully.  
- `TCP_ERR_GENERIC`: `setsockopt` failed.

**Usage Case**:  
Set `enable = true` for low-latency request/response workloads (RPC, gaming, live trading). Set `enable = false` for bulk transfers where extra packet overhead matters.

---

### `TcpStatus tcp_set_linger(TcpSocket socket, bool enable, int seconds)`

**Purpose**:  
Configures `SO_LINGER`, which controls what `close()` does with unsent data.

**Parameters**:  
- `socket`: The TCP socket to configure.  
- `enable`: `false` — `close()` returns immediately and the kernel flushes in the background; `true` — see `seconds`.  
- `seconds`: If `enable = true` and `seconds > 0`, `close()` blocks up to `seconds` waiting for queued data to be ACK'd. If `seconds = 0`, an abortive close is performed: queued bytes are discarded and RST is sent (no TIME_WAIT).

**Return Value**:  
- `TCP_SUCCESS`: Option set successfully.  
- `TCP_ERR_GENERIC`: `setsockopt` failed.

**Usage Case**:  
Use `enable = true, seconds = 0` to slam a misbehaving connection shut immediately and avoid the TIME_WAIT state on rapid server restarts.

---

### `TcpStatus tcp_set_buffer_size(TcpSocket socket, size_t send_bytes, size_t recv_bytes)`

**Purpose**:  
Tunes `SO_SNDBUF` and `SO_RCVBUF`. Larger buffers improve bulk-transfer throughput on high bandwidth-delay paths; smaller buffers cap per-connection memory on busy servers.

**Parameters**:  
- `socket`: The TCP socket to configure.  
- `send_bytes`: Desired send-buffer size in bytes. Pass `0` to leave unchanged.  
- `recv_bytes`: Desired receive-buffer size in bytes. Pass `0` to leave unchanged.

**Return Value**:  
- `TCP_SUCCESS`: Buffer sizes updated successfully.  
- `TCP_ERR_GENERIC`: `setsockopt` failed.

**Usage Case**:  
Call once after socket creation to pre-tune buffers for bulk-transfer or high-throughput scenarios.

---

### `TcpStatus tcp_connect_timeout(TcpSocket socket, const char* host, unsigned short port, long timeout_ms)`

**Purpose**:  
A bounded-time alternative to `tcp_connect` that **never blocks longer than `timeout_ms`**. It resolves `host` (IPv4/IPv6), then connects in non-blocking mode (issue `connect` → wait with `tcp_wait_writable` → confirm via `tcp_get_socket_error`) and leaves the socket in blocking mode on return.

**Parameters**:  
- `socket`: A fresh socket from `tcp_socket_create`.  
- `host`: Remote host (numeric or DNS name); `NULL`/empty → resolve error.  
- `port`: Remote port.  
- `timeout_ms`: Maximum time to wait for the handshake. `< 0` waits indefinitely.

**Return Value**:  
- `TCP_SUCCESS`: Connected.  
- `TCP_ERR_RESOLVE`: Host could not be resolved.  
- `TCP_ERR_WOULD_BLOCK`: Timed out before the handshake completed.  
- `TCP_ERR_CONNECT`: Every resolved address was refused/unreachable.  
- `TCP_ERR_GENERIC`: Socket-mode or `select()` error.

**Usage Case**:  
**Essential in production.** A plain blocking `connect` to an unreachable host can stall for the OS default (often minutes) and wedge a request thread — this bounds it (e.g. a 2-second connect budget for an upstream HTTP call).

---

### `TcpStatus tcp_wait_readable(TcpSocket socket, long timeout_ms)`

**Purpose**:  
Blocks in `select()` until the socket is readable (data arrived, or the peer closed) or `timeout_ms` elapses — poll-with-timeout without putting the socket in non-blocking mode. `timeout_ms < 0` waits indefinitely; `0` polls.

**Parameters**:  
- `socket`: Connected TCP socket.  
- `timeout_ms`: Timeout in milliseconds.

**Return Value**:  
- `TCP_SUCCESS`: Readable.  
- `TCP_ERR_WOULD_BLOCK`: Timed out.  
- `TCP_ERR_GENERIC`: `select()` error.

**Usage Case**:  
Drive a responsive receive loop that can also do periodic work (timeouts, heartbeats, shutdown checks) instead of blocking forever in `tcp_recv`.

---

### `TcpStatus tcp_wait_writable(TcpSocket socket, long timeout_ms)`

**Purpose**:  
Blocks in `select()` until the socket can accept a write or `timeout_ms` elapses. Two main uses: bound a send against a slow/stalled peer, and detect completion of a non-blocking `connect` (the socket becomes writable when the handshake finishes — then check `tcp_get_socket_error`).

**Parameters**:  
- `socket`: TCP socket.  
- `timeout_ms`: Timeout in milliseconds (`< 0` indefinite, `0` poll).

**Return Value**:  
- `TCP_SUCCESS`: Writable.  
- `TCP_ERR_WOULD_BLOCK`: Timed out.  
- `TCP_ERR_GENERIC`: `select()` error.

**Usage Case**:  
Implement your own connect-with-timeout or back-pressure-aware sender.

---

### `TcpStatus tcp_bytes_available(TcpSocket socket, size_t* available)`

**Purpose**:  
Reports how many bytes can be read without blocking (via `FIONREAD`), so you can size a receive buffer before `tcp_recv`. Writes `0` when the queue is empty.

**Parameters**:  
- `socket`: Connected TCP socket.  
- `available`: Receives the byte count. Must not be `NULL`.

**Return Value**:  
- `TCP_SUCCESS` on success.  
- `TCP_ERR_GENERIC` on a `NULL` output or query error.

**Usage Case**:  
Allocate an exactly-sized buffer for pending data, or check whether bytes are queued without blocking.

---

### `TcpStatus tcp_get_socket_error(TcpSocket socket, int* error_code)`

**Purpose**:  
Reads (and clears) the pending socket error via `SO_ERROR`. The canonical way to learn the outcome of a non-blocking `connect`: once the socket is writable, `0` means success while a non-zero value is the OS failure code (e.g. `ECONNREFUSED`).

**Parameters**:  
- `socket`: TCP socket.  
- `error_code`: Receives the OS error code (`0` = none). Must not be `NULL`.

**Return Value**:  
- `TCP_SUCCESS`: Option read (regardless of its value).  
- `TCP_ERR_GENERIC`: `NULL` output or `getsockopt` failure.

**Usage Case**:  
Confirm a non-blocking connect succeeded, or surface the exact reason an async operation failed.

---

### `TcpStatus tcp_send_all(TcpSocket socket, const void* buf, size_t len)`

**Purpose**:  
Sends **exactly** `len` bytes by looping over partial writes. Unlike `tcp_send`, which may write fewer bytes than requested, this wrapper keeps looping until all bytes are delivered or an error occurs.

**Parameters**:  
- `socket`: The TCP socket to send through.  
- `buf`: Pointer to the data to send.  
- `len`: Exact number of bytes to transmit. `len = 0` is a no-op success.

**Return Value**:  
- `TCP_SUCCESS`: All bytes were sent.  
- Any error code returned by `tcp_send` if a partial write fails mid-loop.

**Usage Case**:  
Use for length-prefixed or fixed-size protocol frames where a short write silently drops tail bytes.

---

### `TcpStatus tcp_recv_all(TcpSocket socket, void* buf, size_t len)`

**Purpose**:  
Receives **exactly** `len` bytes by looping over partial reads. The mirror of `tcp_send_all` for fixed-size frames or length-prefixed protocols. A clean peer close mid-stream is reported as `TCP_ERR_RECV` because an incomplete frame is a protocol error.

**Parameters**:  
- `socket`: The TCP socket to receive from.  
- `buf`: Pointer to the destination buffer (must be at least `len` bytes).  
- `len`: Exact number of bytes to receive. `len = 0` is a no-op success.

**Return Value**:  
- `TCP_SUCCESS`: All `len` bytes were received.  
- `TCP_ERR_RECV`: Peer closed the connection before all bytes arrived, or a receive error occurred.

**Usage Case**:  
Receiving a fixed-size header before reading a variable-length body avoids reassembling split TCP segments manually.

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
#include "time/std_time.h"

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

## UDP Socket

The UDP library provides a cross-platform, thread-safe, robust abstraction for UDP sockets, supporting both IPv4 and IPv6, non-blocking mode, broadcast, timeouts, and hostname resolution. All error handling is robust and per-thread. The API is similar to the TCP API, but for datagram (connectionless) sockets.

### Features
- Thread-safe initialization and cleanup (WSAStartup/cleanup on Windows is reference-counted and mutex-protected)
- Per-thread last error reporting
- IPv4 and IPv6 support (dual-stack by default)
- Non-blocking mode
- Broadcast support
- Timeout support (send/receive)
- Hostname resolution and address validation
- (Multicast support can be added in the future)

### Usage Patterns
- Always call `udp_init()` before using UDP sockets, and `udp_cleanup()` at program end.
- All UDP functions are thread-safe and can be used from multiple threads.
- Use `udp_get_last_error()` to retrieve the last error for the current thread.
- Use IPv6 or IPv4 addresses/hostnames as needed.

### UDP Function Descriptions

---

### `UdpStatus udp_init(void)`

**Purpose**:  
Initialises the UDP subsystem. On Windows performs a reference-counted `WSAStartup`; on POSIX a no-op aside from internal mutex setup.

**Parameters**:  
None.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_SETUP` if `WSAStartup` fails on Windows.

**Usage Case**:  
Must be called before any other UDP function. Pair with `udp_cleanup` at program end.

---

### `UdpStatus udp_cleanup(void)`

**Purpose**:  
Tears down one reference to the UDP subsystem. The final unmatched call invokes `WSACleanup` on Windows.

**Parameters**:  
None.

**Return Value**:  
`UDP_SUCCESS`.

**Usage Case**:  
Call once at program end to pair with `udp_init`.

---

### `UdpStatus udp_socket_create(UdpSocket* sock)`

**Purpose**:  
Creates a dual-stack UDP socket (`AF_INET6` with `IPV6_V6ONLY` disabled) so that both IPv6 and IPv4-mapped-IPv6 destinations work through the same handle.

**Parameters**:  
- `sock`: Output pointer where the new socket handle is stored. Must not be `NULL`.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_SOCKET` if the OS call fails or `sock` is `NULL`.

**Usage Case**:  
First step in any UDP workflow — call before `udp_bind` or `udp_sendto`.

---

### `UdpStatus udp_bind(UdpSocket socket, const char* host, unsigned short port)`

**Purpose**:  
Binds a UDP socket to a local address and port. Binding is required to receive datagrams.

**Parameters**:  
- `socket`: The UDP socket to bind.  
- `host`: Host address to bind to. Pass `NULL` or `""` to bind to the wildcard address.  
- `port`: Port to bind to. Pass `0` to let the OS assign an ephemeral port.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_RESOLVE` if `host` cannot be parsed.  
- `UDP_ERR_BIND` if the OS `bind()` call fails.

**Usage Case**:  
Required for servers and for any socket that needs to receive datagrams from arbitrary senders.

---

### `UdpStatus udp_connect(UdpSocket socket, const char* host, unsigned short port)`

**Purpose**:  
Pins the socket to a fixed remote peer. Calling `connect()` on a UDP socket does not open a connection — it sets the default destination so `udp_send`/`udp_recv` need no per-call addresses, and filters incoming datagrams so only packets from the pinned peer are delivered.

**Parameters**:  
- `socket`: The UDP socket to pin.  
- `host`: Hostname or IP address of the remote peer.  
- `port`: Port of the remote peer.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_RESOLVE` if the host cannot be resolved.  
- `UDP_ERR_SEND` for other errors.

**Usage Case**:  
Write a UDP client with the same ergonomics as a TCP client — no per-call destination arguments and automatic peer filtering.

---

### `UdpStatus udp_close(UdpSocket socket)`

**Purpose**:  
Closes a UDP socket and releases its OS handle.

**Parameters**:  
- `socket`: The UDP socket to close.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_CLOSE` if the close operation fails.

**Usage Case**:  
Always call when done with a socket to avoid resource leaks.

---

### `UdpStatus udp_sendto(UdpSocket socket, const void* buf, size_t len, size_t* sent, const char* dest_host, unsigned short dest_port)`

**Purpose**:  
Sends a single UDP datagram to `dest_host:dest_port`. `dest_host` may be a numeric IPv4/IPv6 address or a DNS name. IPv4 destinations are routed transparently through the IPv4-mapped IPv6 form.

**Parameters**:  
- `socket`: The UDP socket to send from.  
- `buf`: Pointer to the data to send.  
- `len`: Number of bytes to send.  
- `sent`: If non-`NULL`, receives the actual byte count sent.  
- `dest_host`: Destination hostname or IP address.  
- `dest_port`: Destination port number.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_RESOLVE` if the host cannot be resolved.  
- `UDP_ERR_SEND` on send failure or invalid arguments.

**Usage Case**:  
Standard one-shot datagram send for unconnected sockets. For connected sockets, prefer `udp_send`.

---

### `UdpStatus udp_recvfrom(UdpSocket socket, void* buf, size_t len, size_t* received, char* src_host, size_t src_host_len, unsigned short* src_port)`

**Purpose**:  
Receives a single UDP datagram. Optionally writes the sender's address and port. The sender's address may come back in IPv4-mapped form (e.g. `::ffff:127.0.0.1`) — that string is still valid input to `udp_sendto`, so echo servers work without extra translation.

**Parameters**:  
- `socket`: The UDP socket to receive on.  
- `buf`: Buffer for the received data.  
- `len`: Size of `buf` in bytes.  
- `received`: If non-`NULL`, receives the actual byte count.  
- `src_host`: If non-`NULL`, filled with the sender's address string.  
- `src_host_len`: Size of the `src_host` buffer.  
- `src_port`: If non-`NULL`, filled with the sender's port number.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_NO_DATA` if the socket is non-blocking and no datagram is queued.  
- `UDP_ERR_RECV` for other errors.

**Usage Case**:  
Primary receive call for unconnected sockets. For connected sockets, prefer `udp_recv`.

---

### `UdpStatus udp_send(UdpSocket socket, const void* buf, size_t len, size_t* sent)`

**Purpose**:  
Sends on a UDP socket previously pinned with `udp_connect`. Equivalent to libc `send()` — no destination arguments because they are implicit.

**Parameters**:  
- `socket`: A connected UDP socket.  
- `buf`: Pointer to the data to send.  
- `len`: Number of bytes to send.  
- `sent`: If non-`NULL`, receives the actual byte count.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_SEND` if the socket is not connected or send fails.

**Usage Case**:  
Use after `udp_connect` for clean TCP-style send semantics without per-call destinations.

---

### `UdpStatus udp_recv(UdpSocket socket, void* buf, size_t len, size_t* received)`

**Purpose**:  
Receives on a UDP socket previously pinned with `udp_connect`. Only datagrams from the connected peer are delivered; no source-address output is produced.

**Parameters**:  
- `socket`: A connected UDP socket.  
- `buf`: Buffer for the received data.  
- `len`: Size of `buf` in bytes.  
- `received`: If non-`NULL`, receives the actual byte count.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_NO_DATA` on a non-blocking socket with no pending data.  
- `UDP_ERR_RECV` for other errors.

**Usage Case**:  
Use after `udp_connect` for clean TCP-style receive semantics. Use `udp_recvfrom` if you need the sender's address.

---

### `UdpStatus udp_set_non_blocking(UdpSocket socket, bool enable)`

**Purpose**:  
Enables or disables non-blocking I/O on a UDP socket.

**Parameters**:  
- `socket`: The UDP socket to configure.  
- `enable`: `true` to enable non-blocking mode; `false` to revert to blocking.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` on failure.

**Usage Case**:  
When enabled, `udp_recvfrom` returns `UDP_ERR_NO_DATA` immediately instead of blocking when there is nothing to read — suitable for polling loops.

---

### `UdpStatus udp_set_broadcast(UdpSocket socket, bool enable)`

**Purpose**:  
Enables or disables `SO_BROADCAST` on the socket, which is required before sending to a broadcast address.

**Parameters**:  
- `socket`: The UDP socket to configure.  
- `enable`: `true` to allow broadcast sends; `false` to prohibit them.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` on failure.

**Usage Case**:  
Required before sending to `255.255.255.255` or a subnet broadcast address.

---

### `UdpStatus udp_set_timeout(UdpSocket socket, long timeout_ms)`

**Purpose**:  
Applies the same send/receive timeout to both `SO_RCVTIMEO` and `SO_SNDTIMEO`. Pass `0` to clear any existing timeout.

**Parameters**:  
- `socket`: The UDP socket to configure.  
- `timeout_ms`: Timeout in milliseconds; `0` means no timeout.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` on failure.

**Usage Case**:  
Prevents `udp_recvfrom` from blocking indefinitely when waiting for a response in a request/reply protocol.

---

### `UdpStatus udp_set_reuse_addr(UdpSocket socket, bool enable)`

**Purpose**:  
Enables or disables `SO_REUSEADDR` (and `SO_REUSEPORT` on POSIX where available). Allows re-binding to the same port immediately after a previous bind is released, and enables sharing a multicast port across sockets.

**Parameters**:  
- `socket`: The UDP socket to configure.  
- `enable`: `true` to allow address reuse; `false` to disable.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` on failure.

**Usage Case**:  
Set before `udp_bind` to avoid "Address already in use" errors during rapid restarts, and when multiple processes need to share a multicast port.

---

### `UdpStatus udp_set_buffer_size(UdpSocket socket, size_t send_bytes, size_t recv_bytes)`

**Purpose**:  
Tunes the kernel send (`SO_SNDBUF`) and receive (`SO_RCVBUF`) buffer sizes. The kernel typically doubles the requested value internally and may clamp to a system maximum.

**Parameters**:  
- `socket`: The UDP socket to configure.  
- `send_bytes`: Desired send-buffer size. Pass `0` to leave unchanged.  
- `recv_bytes`: Desired receive-buffer size. Pass `0` to leave unchanged.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` on failure.

**Usage Case**:  
Increase the receive buffer for bursty high-throughput workloads to reduce datagram drops at the OS level.

---

### `UdpStatus udp_join_multicast_group(UdpSocket socket, const char* group_addr, const char* iface_addr)`

**Purpose**:  
Joins a multicast group so the socket receives datagrams sent to that group address.

**Parameters**:  
- `socket`: The UDP socket to join the group on.  
- `group_addr`: Multicast group address (IPv4 like `"224.0.0.1"` or IPv6 like `"ff02::1"`).  
- `iface_addr`: Local interface address to join on. Pass `NULL` for the default interface.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` if the join fails.

**Usage Case**:  
Required on receivers before they can receive multicast datagrams.

---

### `UdpStatus udp_leave_multicast_group(UdpSocket socket, const char* group_addr, const char* iface_addr)`

**Purpose**:  
Leaves a multicast group previously joined with `udp_join_multicast_group`.

**Parameters**:  
- `socket`: The UDP socket to leave the group on.  
- `group_addr`: The multicast group address to leave.  
- `iface_addr`: Local interface address. Pass `NULL` for the default interface.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` on failure.

**Usage Case**:  
Call when the application no longer needs to receive from a multicast group, to free OS resources.

---

### `UdpStatus udp_set_multicast_ttl(UdpSocket socket, int ttl)`

**Purpose**:  
Sets the multicast time-to-live (TTL), controlling how many router hops a multicast datagram can traverse.

**Parameters**:  
- `socket`: The UDP socket to configure.  
- `ttl`: TTL value in range 0–255. `1` restricts traffic to the local subnet; higher values cross routers.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` on failure.

**Usage Case**:  
Set `ttl = 1` for local-network-only multicast (LAN discovery). Increase for cross-router multicast delivery.

---

### `UdpStatus udp_resolve_hostname(const char* hostname, char* ip_address, size_t ip_address_len)`

**Purpose**:  
Resolves a DNS name to its first IP address (IPv4 or IPv6) and writes the result in presentation form (e.g. `"127.0.0.1"` or `"::1"`). Does not filter by address family.

**Parameters**:  
- `hostname`: The DNS name or numeric address to resolve.  
- `ip_address`: Buffer where the resolved address string is stored.  
- `ip_address_len`: Size of the `ip_address` buffer. At least 46 bytes is recommended for IPv6.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_RESOLVE` if resolution fails.

**Usage Case**:  
Convert a hostname to an IP before logging the actual address, or before passing it to `udp_sendto`.

---

### `UdpStatus udp_get_local_address(UdpSocket socket, char* host, size_t host_len, unsigned short* port)`

**Purpose**:  
Returns the local address and port the socket is bound to (wraps `getsockname()` and `inet_ntop`).

**Parameters**:  
- `socket`: A bound UDP socket.  
- `host`: Buffer for the address string. Should be at least 64 bytes for IPv6.  
- `host_len`: Size of the `host` buffer.  
- `port`: If non-`NULL`, receives the bound port number.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` on failure.

**Usage Case**:  
Particularly useful when bound to port `0` to discover the kernel-assigned ephemeral port.

---

### `UdpStatus udp_get_peer_address(UdpSocket socket, char* host, size_t host_len, unsigned short* port)`

**Purpose**:  
The counterpart of `udp_get_local_address`: reports the remote peer set by a prior `udp_connect` (wraps `getpeername()` and `inet_ntop`). The address is written in presentation form (e.g. `"::1"`).

**Parameters**:  
- `socket`: A socket previously connected with `udp_connect`.  
- `host`: Buffer for the peer address string (use `INET6_ADDRSTRLEN`).  
- `host_len`: Size of the `host` buffer.  
- `port`: If non-`NULL`, receives the peer port in host byte order.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` on invalid arguments, an **unconnected** socket (`getpeername` fails with `ENOTCONN`), or an unsupported address family.

**Usage Case**:  
Log or display which peer a connected UDP "client" socket is talking to.

---

### `UdpStatus udp_set_ttl(UdpSocket socket, int ttl)`

**Purpose**:  
Sets the unicast hop limit (TTL) for outgoing datagrams — the unicast counterpart of `udp_set_multicast_ttl`. Because the library's sockets are dual-stack IPv6, it sets both `IP_TTL` and `IPV6_UNICAST_HOPS` and succeeds if either applies.

**Parameters**:  
- `socket`: The UDP socket to configure.  
- `ttl`: Hop limit in range 0–255.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` if `ttl` is out of range or the option could not be set.

**Usage Case**:  
Limit how far datagrams propagate (e.g. `ttl = 1` to keep traffic on the local subnet), or raise it for long network paths.

---

### `UdpStatus udp_set_multicast_loopback(UdpSocket socket, bool enable)`

**Purpose**:  
Enables or disables local loopback of outgoing multicast (`IP_MULTICAST_LOOP` / `IPV6_MULTICAST_LOOP`). When enabled (the default), a sender that also joined the group receives its own datagrams.

**Parameters**:  
- `socket`: The UDP socket to configure.  
- `enable`: `true` to receive your own multicast (default), `false` to suppress it.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` on failure.

**Usage Case**:  
Disable loopback in a multicast app that both sends and listens, so it doesn't process the packets it just sent.

---

### `UdpStatus udp_bytes_available(UdpSocket socket, size_t* available)`

**Purpose**:  
Reports how many bytes are immediately readable (via `FIONREAD`). On a datagram socket this is at least the size of the next pending datagram, so it lets you size a receive buffer before `udp_recv` / `udp_recvfrom` and avoid truncation. Writes `0` when the queue is empty.

**Parameters**:  
- `socket`: A valid UDP socket.  
- `available`: Receives the byte count. Must not be `NULL`.

**Return Value**:  
- `UDP_SUCCESS` on success.  
- `UDP_ERR_GENERIC` on a `NULL` output pointer or a query error.

**Usage Case**:  
Allocate an exactly-sized buffer for the next datagram, or check whether data is queued without blocking.

---

### `UdpStatus udp_wait_readable(UdpSocket socket, long timeout_ms)`

**Purpose**:  
Blocks in `select()` until the socket has a datagram ready or the timeout elapses — the building block for poll-with-timeout loops without making the whole socket non-blocking. `timeout_ms < 0` waits indefinitely; `timeout_ms == 0` polls and returns immediately.

**Parameters**:  
- `socket`: A valid UDP socket.  
- `timeout_ms`: Timeout in milliseconds (see above).

**Return Value**:  
- `UDP_SUCCESS` if the socket is readable.  
- `UDP_ERR_NO_DATA` on timeout.  
- `UDP_ERR_GENERIC` on a `select()` error.

**Usage Case**:  
Drive a responsive receive loop that can also do periodic work (heartbeats, shutdown checks) while waiting for packets.

---

### `bool udp_is_valid_address(const char* address)`

**Purpose**:  
Checks whether `address` parses as a numeric IPv4 or IPv6 address without performing DNS resolution.

**Parameters**:  
- `address`: The string to validate. May be `NULL`.

**Return Value**:  
`true` if `address` is a valid numeric IPv4 or IPv6 address; `false` otherwise (including `NULL`).

**Usage Case**:  
Validate user input or configuration values before passing to socket functions.

---

### `void udp_get_last_error(UdpStatusInfo* status_info)`

**Purpose**:  
Retrieves the most recent UDP error for the calling thread. The error is stored per-thread so concurrent threads do not interfere with each other.

**Parameters**:  
- `status_info`: Pointer to a `UdpStatusInfo` struct to populate with the system error code and human-readable message.

**Return Value**:  
None.

**Usage Case**:  
Call after a failed UDP operation to obtain the system-level error code and description for logging or user feedback.

---

### `const char* udp_status_to_string(UdpStatus code)`

**Purpose**:  
Returns a static human-readable label for a `UdpStatus` error code.

**Parameters**:  
- `code`: Any `UdpStatus` enumeration value.

**Return Value**:  
Static string such as `"UDP_SUCCESS"` or `"UDP_ERR_BIND"`. Unknown codes return `"UDP_UNKNOWN_CODE"`. Never `NULL`.

**Usage Case**:  
Structured logging without pulling in the full `UdpStatusInfo` from `udp_get_last_error`.

---

## Example 1: UDP Echo Server

```c


#include "network/udp.h"
#include "fmt/fmt.h"

#define PORT 9000
#define BUFFER_SIZE 1024

int main() {
    udp_init();
    UdpSocket sock;

    udp_socket_create(&sock);
    udp_bind(sock, NULL, PORT);
    fmt_printf("UDP Echo Server listening on port %d\n", PORT);

    char buf[BUFFER_SIZE];
    char src_host[128];
    unsigned short src_port;
    size_t received;

    while (1) {
        UdpStatus status = udp_recvfrom(sock, buf, sizeof(buf), &received, src_host, sizeof(src_host), &src_port);
        if (status == UDP_SUCCESS && received > 0) {
            buf[received] = '\0';
            fmt_printf("Received from %s:%u: %s\n", src_host, src_port, buf);
            udp_sendto(sock, buf, received, NULL, src_host, src_port);
        } 
        else if (status == UDP_ERR_NO_DATA) {
            continue; // Non-blocking mode: just try again
        } 
        else {
            UdpStatusInfo err;
            udp_get_last_error(&err);
            fmt_printf("UDP error: %s\n", err.message);
            break; // or handle error appropriately
        }
    }
    
    udp_close(sock);
    udp_cleanup();
    return 0;
}
```

---

## Example 2: UDP Client

```c
#include "network/udp.h"
#include "fmt/fmt.h"
#include <string.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000
#define BUFFER_SIZE 1024

int main() {
    udp_init();
    UdpSocket sock;

    udp_socket_create(&sock);
    char msg[] = "Hello, UDP Server!";
    udp_sendto(sock, msg, strlen(msg), NULL, SERVER_IP, SERVER_PORT);

    char buf[BUFFER_SIZE];
    char src_host[128];
    unsigned short src_port;
    size_t received;

    udp_recvfrom(sock, buf, sizeof(buf), &received, src_host, sizeof(src_host), &src_port);
    buf[received] = '\0';

    fmt_printf("Received from %s:%u: %s\n", src_host, src_port, buf);
    udp_close(sock);
    udp_cleanup();

    return 0;
}
```

---

## Example 3: UDP Broadcast Sender/Receiver

**Sender:**
```c
#include "network/udp.h"
#include "fmt/fmt.h"
#include <string.h>

#define BROADCAST_PORT 9001

int main() {
    udp_init();
    UdpSocket sock;

    udp_socket_create(&sock);
    udp_set_broadcast(sock, true);

    char msg[] = "Hello, broadcast!";
    udp_sendto(sock, msg, strlen(msg), NULL, "255.255.255.255", BROADCAST_PORT);

    udp_close(sock);
    udp_cleanup();

    return 0;
}
```

**Receiver:**
```c
#include "network/udp.h"
#include "fmt/fmt.h"

#define BROADCAST_PORT 9001
#define BUFFER_SIZE 1024

int main() {
    udp_init();
    UdpSocket sock;

    udp_socket_create(&sock);
    udp_bind(sock, NULL, BROADCAST_PORT);

    char buf[BUFFER_SIZE];
    char src_host[128];
    unsigned short src_port;
    size_t received;

    udp_recvfrom(sock, buf, sizeof(buf), &received, src_host, sizeof(src_host), &src_port);
    buf[received] = '\0';

    fmt_printf("Received broadcast from %s:%u: %s\n", src_host, src_port, buf);

    udp_close(sock);
    udp_cleanup();
    return 0;
}
```

---

## Example 4: UDP Multicast Sender/Receiver

**Sender:**
```c
#include "network/udp.h"
#include "fmt/fmt.h"
#include <string.h>

#define MULTICAST_GROUP "239.255.0.1"
#define MULTICAST_PORT 9002

int main() {
    udp_init();
    UdpSocket sock;

    udp_socket_create(&sock);
    udp_set_multicast_ttl(sock, 1); // Restrict to local network

    char msg[] = "Hello, multicast!";
    udp_sendto(sock, msg, strlen(msg), NULL, MULTICAST_GROUP, MULTICAST_PORT);

    udp_close(sock);
    udp_cleanup();

    return 0;
}
```

**Receiver:**
```c
#include "network/udp.h"
#include "fmt/fmt.h"

#define MULTICAST_GROUP "239.255.0.1"
#define MULTICAST_PORT 9002
#define BUFFER_SIZE 1024

int main() {
    udp_init();
    UdpSocket sock;

    udp_socket_create(&sock);
    udp_bind(sock, NULL, MULTICAST_PORT);
    udp_join_multicast_group(sock, MULTICAST_GROUP, NULL); // NULL = default interface

    char buf[BUFFER_SIZE];
    char src_host[128];
    unsigned short src_port;
    size_t received;

    udp_recvfrom(sock, buf, sizeof(buf), &received, src_host, sizeof(src_host), &src_port);
    buf[received] = '\0';

    fmt_printf("Received multicast from %s:%u: %s\n", src_host, src_port, buf);
    udp_leave_multicast_group(sock, MULTICAST_GROUP, NULL);

    udp_close(sock);
    udp_cleanup();

    return 0;
}
```

---

## Example 5: Non-blocking UDP with Timeout

```c
#include "network/udp.h"
#include "fmt/fmt.h"
#include <time.h>
#include <string.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 9000
#define BUFFER_SIZE 1024

int main() {
    udp_init();
    UdpSocket sock;

    udp_socket_create(&sock);
    udp_set_non_blocking(sock, true);

    char msg[] = "Hello, non-blocking UDP!";
    udp_sendto(sock, msg, strlen(msg), NULL, SERVER_IP, SERVER_PORT);

    char buf[BUFFER_SIZE];
    char src_host[128];
    unsigned short src_port;
    size_t received;
    time_t start = time(NULL);

    while (1) {
        UdpStatus status = udp_recvfrom(sock, buf, sizeof(buf), &received, src_host, sizeof(src_host), &src_port);
        if (status == UDP_SUCCESS && received > 0) {
            buf[received] = '\0';
            fmt_printf("Received from %s:%u: %s\n", src_host, src_port, buf);
            break;
        } 
        else if (status == UDP_ERR_NO_DATA) {
            if (time(NULL) - start > 5) {
                fmt_printf("Timeout waiting for response\n");
                break;
            }
        }
        else {
            UdpStatusInfo err;
            udp_get_last_error(&err);
            fmt_printf("UDP error: %s\n", err.message);
            break;
        }
    }

    udp_close(sock);
    udp_cleanup();

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
                Thread thread_id = 0;   /* Thread is pthread_t (an integer) on POSIX, a HANDLE on Windows; 0 is valid for both */
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
            Thread client_thread = 0;   /* Thread is pthread_t (an integer) on POSIX, a HANDLE on Windows; 0 is valid for both */
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

## HTTP Server

The HTTP layer is a small, single-threaded HTTP/1.1 server built on top of `TcpSocket`. It supports method+path routing, query-string parsing, JSON request/response bodies (via the project's JSON module), and conventional helpers for setting status, headers, and body content.

### Features
- Routes register with method + path; paths may contain a single `{id}` placeholder that is parsed as an integer into `HttpRequest::id`.
- Request parsing extracts method, path, query parameters, headers, and the body. JSON-looking bodies are auto-parsed into `req->json_body`.
- Response builders (`http_set_status`, `http_set_body`, `http_set_json_body`, `http_add_header`) are leak-safe — repeated calls free the previous value.
- 404 fallback is automatic if no route matches.
- All public helpers are NULL-safe; the server never crashes the host on malformed input.

### HTTP Types
- `HttpMethod` — enum of supported verbs (`HTTP_GET`, `HTTP_POST`, `HTTP_PUT`, `HTTP_DELETE`, `HTTP_OPTIONS`, `HTTP_HEAD`, `HTTP_PATCH`, `HTTP_UNKNOWN`).
- `HttpRequest` — parsed request. Holds `method`, `path`, `body` / `json_body`, `headers[]`, `query_params[]`, and `id` for `{id}` routes.
- `HttpResponse` — outgoing response. Holds `status_code`, `status_message`, `headers[]`, `body`, `json_body`.
- `HttpHandler` — function pointer `void (*)(HttpRequest*, HttpResponse*)` registered against routes.

### HTTP Function Descriptions

---

### `HttpMethod http_parse_method(const char* request)`

**Purpose**:  
Identifies the HTTP method at the start of a raw request line. Compares the leading verb (including the trailing space) to avoid prefix collisions such as `"GETSOMETHING"` matching `HTTP_GET`.

**Parameters**:  
- `request`: Raw HTTP request string. May be `NULL`.

**Return Value**:  
The corresponding `HttpMethod` enum value, or `HTTP_UNKNOWN` on no match or `NULL` input.

**Usage Case**:  
Quick method detection when you have the raw request bytes before parsing the full request.

---

### `HttpRequest* http_parse_request(const char* request)`

**Purpose**:  
Parses a raw HTTP/1.1 request into an `HttpRequest` struct. Validates the request-line shape before allocating. Bodies whose first byte is `{` or `[` are auto-parsed into `json_body`. The caller must release the result with `http_free_request`.

**Parameters**:  
- `request`: A NUL-terminated raw HTTP request string.

**Return Value**:  
Pointer to a newly allocated `HttpRequest`, or `NULL` if the request is malformed or allocation fails.

**Usage Case**:  
Primary entry point for turning raw TCP bytes into a structured request object inside a request handler loop.

---

### `HttpRequest* http_parse_request_ex(const char* data, size_t length)`

**Purpose**:  
Length-delimited, **binary-safe** variant of `http_parse_request`. Parses exactly `length` bytes, so the body may contain embedded NULs; the exact size is reported in `req->body_length` (the body is still NUL-terminated for convenience). `http_parse_request` is now a thin wrapper that calls this with `strlen(request)`.

**Parameters**:  
- `data`: Raw request bytes (need not be NUL-terminated within `length`).  
- `length`: Exact number of bytes to parse.

**Return Value**:  
A newly allocated `HttpRequest` (free with `http_free_request`), or `NULL` on a malformed request / allocation failure.

**Usage Case**:  
Parsing a request whose body is binary (file upload, protobuf, image) where `strlen` would truncate at the first NUL.

---

### `bool http_read_full_request(TcpSocket socket, char** out_data, size_t* out_len, int* out_status)`

**Purpose**:  
Reads one **complete** request off a connected socket: loops `recv` until the full header block (`\r\n\r\n`) arrives, then reads the body framed by `Content-Length` or decodes a `Transfer-Encoding: chunked` body into a contiguous buffer. Enforces `HTTP_MAX_HEADER_BYTES` / `HTTP_MAX_BODY_BYTES`, and rejects request-smuggling vectors (both `Content-Length` *and* `Transfer-Encoding`) and unsupported transfer-codings. This is what fixes the old "single 4 KB `recv`" truncation.

**Parameters**:  
- `socket`: A connected client socket.  
- `out_data`: Receives a malloc'd, NUL-terminated buffer (headers + de-framed body); the caller frees it.  
- `out_len`: Receives the exact byte length (body included).  
- `out_status`: On failure, the HTTP status to reply with — `431` (headers too large), `413` (body too large), `400` (truncated/smuggling), `408` (idle timeout), `501` (unsupported coding) — or `0` when the peer simply closed without sending a request (drop silently).

**Return Value**:  
`true` on success (with `*out_data` / `*out_len` set), `false` otherwise (with `*out_status` set).

**Usage Case**:  
The read step inside a custom accept loop when you drive `http_handle_request`-style dispatch yourself.

---

### `void http_free_request(HttpRequest* request)`

**Purpose**:  
Releases every allocation owned by an `HttpRequest`: path, body, JSON tree, all header and query-param strings, and the struct itself. `NULL` is a safe no-op.

**Parameters**:  
- `request`: The `HttpRequest` to free. May be `NULL`.

**Return Value**:  
None.

**Usage Case**:  
Call once at the end of each request handler to avoid memory leaks.

---

### `const char* http_get_header(HttpRequest* req, const char* name)`

**Purpose**:  
Looks up a request header value by name (case-sensitive search over the parsed headers array).

**Parameters**:  
- `req`: The parsed request.  
- `name`: Header name to find (e.g. `"Content-Type"`).

**Return Value**:  
Pointer to the header value string (owned by `req`), or `NULL` if not found.

**Usage Case**:  
Read `Content-Type`, `Authorization`, `Host`, or any other request header inside a handler.

---

### `const char* http_get_query_param(HttpRequest* req, const char* name)`

**Purpose**:  
Looks up a query-string parameter by name (e.g. the `q` in `/search?q=hello`).

**Parameters**:  
- `req`: The parsed request.  
- `name`: Parameter name to find.

**Return Value**:  
Pointer to the parameter value string (owned by `req`), or `NULL` if absent.

**Usage Case**:  
Extract filter, pagination, or search parameters from the URL query string inside a GET handler.

---

### `char* http_get_cookie(HttpRequest* req, const char* name)`

**Purpose**:  
Looks up a single cookie value by name in the request's `Cookie` header. Returns a freshly-allocated copy — the caller must `free()` it.

**Parameters**:  
- `req`: The parsed request.  
- `name`: Cookie name to find. Matches whole names only.

**Return Value**:  
Newly allocated copy of the cookie value, or `NULL` if the header is missing or the cookie is not present.

**Usage Case**:  
Read session tokens, user preferences, or other cookie-stored state inside a request handler.

---

### `void http_set_status(HttpResponse* response, int code, const char* message)`

**Purpose**:  
Sets the HTTP status code and reason phrase. Frees any previously set reason phrase before overwriting — repeated calls do not leak.

**Parameters**:  
- `response`: The response to modify.  
- `code`: HTTP status code (e.g. `200`, `404`).  
- `message`: Reason phrase (e.g. `"OK"`, `"Not Found"`). Copied internally.

**Return Value**:  
None.

**Usage Case**:  
First call inside every handler to declare the response outcome before setting the body.

---

### `void http_set_body(HttpResponse* response, const char* body)`

**Purpose**:  
Copies `body` into the response and appends a `Content-Type: text/plain` header. Frees any previous body so repeated calls do not leak.

**Parameters**:  
- `response`: The response to modify.  
- `body`: NUL-terminated body text. Copied internally.

**Return Value**:  
None.

**Usage Case**:  
Set a plain-text body for simple API endpoints or error messages.

---

### `void http_set_html_body(HttpResponse* response, const char* html)`

**Purpose**:  
Attaches an HTML body to the response and appends `Content-Type: text/html; charset=utf-8`. Frees any previous body so repeated calls do not leak.

**Parameters**:  
- `response`: The response to modify.  
- `html`: NUL-terminated HTML string. Copied internally.

**Return Value**:  
None.

**Usage Case**:  
Return server-rendered HTML pages from handlers.

---

### `void http_set_json_body(HttpResponse* response, JsonElement* json)`

**Purpose**:  
Attaches a JSON document to the response, takes ownership of `json`, frees any previously attached body or JSON tree, serializes the new value, and appends `Content-Type: application/json`. Passing `NULL` turns the response into a 500.

**Parameters**:  
- `response`: The response to modify.  
- `json`: JSON element tree to serialize and attach. Ownership is transferred.

**Return Value**:  
None.

**Usage Case**:  
Return structured JSON data from REST API endpoints.

---

### `void http_add_header(HttpResponse* response, const char* header, const char* value)`

**Purpose**:  
Appends a `Name: Value` response header. No deduplication is performed — calling twice with the same name yields two entries. Headers beyond `MAX_HEADERS` are silently dropped.

**Parameters**:  
- `response`: The response to modify.  
- `header`: Header name string.  
- `value`: Header value string. Both are copied internally.

**Return Value**:  
None.

**Usage Case**:  
Add `Cache-Control`, `Access-Control-Allow-Origin`, or any custom response header.

---

### `void http_set_cookie(HttpResponse* response, const char* name, const char* value, const char* path, int max_age, bool http_only)`

**Purpose**:  
Appends a `Set-Cookie` header in the form `name=value[; Path=...][; Max-Age=N][; HttpOnly]`. Multiple calls produce multiple `Set-Cookie` headers — the standard way to send several cookies.

**Parameters**:  
- `response`: The response to modify.  
- `name`: Cookie name.  
- `value`: Cookie value.  
- `path`: Cookie path. Pass `NULL` to omit the `Path` attribute.  
- `max_age`: Cookie lifetime in seconds. Pass a negative value to omit `Max-Age`.  
- `http_only`: `true` to append the `HttpOnly` flag.

**Return Value**:  
None.

**Usage Case**:  
Issue session cookies or other persistent client-side state after successful authentication.

---

### `void http_redirect(HttpResponse* response, int code, const char* location)`

**Purpose**:  
Builds a redirect response: sets the 3xx status (coerces invalid codes to 302), appends a `Location:` header, and writes a small HTML fallback body with the link.

**Parameters**:  
- `response`: The response to modify.  
- `code`: Redirect status code. Accepted values: 301, 302, 303, 307, 308.  
- `location`: The URL to redirect to.

**Return Value**:  
None.

**Usage Case**:  
Permanently move a resource (`301`), redirect after a POST (`303`), or preserve the HTTP method on redirect (`307`/`308`).

---

### `void http_send_error(HttpResponse* res, int code, const char* message)`

**Purpose**:  
Sets both the status code and the plain-text body to `message` in one call. Shortcut for short 4xx/5xx error replies.

**Parameters**:  
- `res`: The response to modify.  
- `code`: HTTP error status code (e.g. `400`, `404`, `500`).  
- `message`: Error description used as both the reason phrase and the body.

**Return Value**:  
None.

**Usage Case**:  
Return a concise error from validation logic without writing separate `http_set_status` + `http_set_body` calls.

---

### `const char* http_response_get_header(HttpResponse* response, const char* name)`

**Purpose**:  
Looks up a response header value by name (case-sensitive). Mirrors `http_get_header` but for the outgoing response.

**Parameters**:  
- `response`: The response to inspect.  
- `name`: Header name to find.

**Return Value**:  
Pointer to the header value string (owned by `response`), or `NULL` if not found.

**Usage Case**:  
Inspect what headers have already been set before deciding whether to append or override.

---

### `size_t http_response_remove_header(HttpResponse* response, const char* name)`

**Purpose**:  
Removes every response header whose name matches (case-sensitive), frees the matching strings, and shifts remaining headers down to keep storage contiguous.

**Parameters**:  
- `response`: The response to modify.  
- `name`: Header name to remove.

**Return Value**:  
Number of headers removed.

**Usage Case**:  
Override the implicit `Content-Type` that `http_set_body`/`http_set_json_body` adds, or clear duplicate `Set-Cookie` headers before re-issuing them.

---

### `char* http_serialize_response(HttpResponse* response)`

**Purpose**:  
Serializes a response into an HTTP/1.1 wire-format byte buffer. Computes the exact buffer size upfront and writes with bounded operations, so the output cannot overflow. The caller must `free()` the result.

**Parameters**:  
- `response`: The fully-configured response to serialize.

**Return Value**:  
Newly allocated NUL-terminated HTTP/1.1 response string, or `NULL` on allocation failure.

**Usage Case**:  
Called internally by `http_handle_request`. Expose it directly for testing response serialization without a live socket.

---

### `char* http_serialize_response_ex(HttpResponse* response, bool head_only, size_t* out_len)`

**Purpose**:  
Binary-safe, HEAD-aware serializer. Auto-adds a `Content-Length` header (unless the response already has one or a `Transfer-Encoding: chunked` header — in which case the body is chunk-encoded `<hex>\r\n…\r\n0\r\n\r\n`), writes the **exact** byte count to `*out_len` (may be `NULL`), and omits the body entirely when `head_only` is `true` (a `HEAD` reply: headers + `Content-Length`, no body). `http_serialize_response` is a wrapper for `http_serialize_response_ex(r, false, NULL)`.

**Parameters**:  
- `response`: The response to serialize.  
- `head_only`: `true` to emit headers only (HEAD).  
- `out_len`: Receives the exact serialized length (body may contain NULs); pass `NULL` if not needed.

**Return Value**:  
Newly allocated buffer of `*out_len` bytes (NUL-terminated for convenience), or `NULL` on failure.

**Usage Case**:  
Emitting a correct `Content-Length`, chunk-encoding a streamed body, or building a HEAD reply — what the concurrent server uses internally.

---

### `void http_free_response(HttpResponse* response)`

**Purpose**:  
Releases every heap allocation owned by an `HttpResponse` (status message, headers, body, JSON tree) and zeroes the fields. Does NOT free the struct itself — typically a stack variable. `NULL`-safe.

**Parameters**:  
- `response`: The response to clean up. May be `NULL`.

**Return Value**:  
None.

**Usage Case**:  
Called at the end of `http_handle_request` and any path that exits after building a response.

---

### `void http_register_route(const char* path, HttpMethod method, HttpHandler handler)`

**Purpose**:  
Registers a handler function for a given path + method pair. Paths may contain a single `{name}` placeholder that is parsed as an integer into `req->id`. Up to `MAX_ROUTES` registrations are kept; extras are silently dropped.

**Parameters**:  
- `path`: URL path pattern, e.g. `"/users/{id}"` or `"/health"`.  
- `method`: HTTP method this route matches (e.g. `HTTP_GET`).  
- `handler`: `void (*)(HttpRequest*, HttpResponse*)` function to invoke on a match.

**Return Value**:  
None.

**Usage Case**:  
Called at startup to wire up all URL paths before `http_start_server`.

---

### `void http_start_server(int port)`

**Purpose**:  
Binds to `0.0.0.0:port` with `SO_REUSEADDR`, listens, and serves requests **concurrently**: every accepted connection is handled on a worker-thread pool, so one slow client never blocks the others. Each connection is bounded by a per-read idle timeout (slow-loris defense), `SIGPIPE` is ignored on POSIX so a hung-up client can't kill the process, and `http_stop_server` performs a **graceful** shutdown (stop accepting, drain in-flight connections, join the workers). Blocks the calling thread until stopped.

**Parameters**:  
- `port`: TCP port number to listen on (`0` lets the OS pick a free port — read it back with `http_server_get_port`).

**Return Value**:  
None. Use `http_start_server_ex` if you need to know whether the bind succeeded.

**Usage Case**:  
Last call in `main()` after all routes are registered. Run in a dedicated thread if you need the main thread for other work.

---

### `TcpStatus http_start_server_ex(int port)`

**Purpose**:  
Same as `http_start_server`, but **returns the bind status** instead of swallowing it — `TCP_SUCCESS` after a clean shutdown, or the failing `TcpStatus` (e.g. `TCP_ERR_BIND` for a busy port) if init/socket/bind/listen failed. This is the production entry point: it tells you immediately whether the server actually came up.

**Parameters**:  
- `port`: TCP port to listen on (`0` = ephemeral).

**Return Value**:  
- `TCP_SUCCESS`: the server ran and was stopped cleanly.
- otherwise: the error that prevented binding (the server never started).

**Usage Case**:  
```c
TcpStatus st = http_start_server_ex(8080);   // blocks until http_stop_server()
if (st != TCP_SUCCESS) {
    fmt_fprintf(stderr, "server failed to start: %d\n", (int)st);
}
```

---

### `void http_server_set_worker_count(int n)`

**Purpose**:  
Sets the worker-pool size used by the next `http_start_server*`. `n <= 0` selects automatic sizing (hardware concurrency). Call **before** starting the server.

---

### `void http_server_set_client_timeout(long timeout_ms)`

**Purpose**:  
Sets the per-connection idle read/write timeout in milliseconds (slow-loris defense; also bounds how long a graceful shutdown waits to drain). A stalled peer is dropped with `408 Request Timeout`. `timeout_ms <= 0` disables the timeout. Defaults to 30000 ms. Call **before** starting the server.

> Implemented with `select()` (`tcp_wait_readable`/`tcp_wait_writable`) rather than `SO_RCVTIMEO`, because `recv()` silently ignores `SO_RCVTIMEO` on the non-overlapped Winsock sockets this library creates. `select()` is honored on every platform.

---

### `unsigned short http_server_get_port(void)`

**Purpose**:  
Returns the actual port the server is currently listening on (useful after binding to port `0`), or `0` if no server is listening. Thread-safe; typically polled by another thread after spawning the server.

---

### `void http_stop_server(void)`

**Purpose**:  
Initiates a graceful shutdown: flips the internal run flag and then opens a throwaway loopback connection so the server's blocked `accept()` returns immediately, observes the flag, drains in-flight connections, and joins the workers. Safe to call from any thread. A no-op if no server is listening.

**Parameters**:  
None.

**Return Value**:  
None. (`http_start_server_ex` returns `TCP_SUCCESS` once the drain completes.)

**Usage Case**:  
Call from a signal handler or a control thread to stop the server promptly and cleanly — no more "send a dummy request to wake the loop" workaround.

---

### `void http_handle_request(TcpSocket client_socket)`

**Purpose**:  
Processes a single already-accepted client connection: reads the request bytes, dispatches to the registered route (or sends a 404), serializes and sends the response. Every exit path frees both the request and the response.

**Parameters**:  
- `client_socket`: An accepted client socket. The caller is responsible for closing it after this function returns.

**Return Value**:  
None.

**Usage Case**:  
Used internally by `http_start_server`, but exposed for direct testing without a listening socket.

---

### `const char* http_method_to_string(HttpMethod m)`

**Purpose**:  
Returns the canonical wire-format name of an HTTP method enum value.

**Parameters**:  
- `m`: Any `HttpMethod` enum value.

**Return Value**:  
Static string: `"GET"`, `"POST"`, `"PUT"`, `"DELETE"`, `"OPTIONS"`, `"HEAD"`, `"PATCH"`, or `"UNKNOWN"`. Never `NULL`.

**Usage Case**:  
Logging and building outbound request lines.

---

### `const char* http_status_text(int code)`

**Purpose**:  
Returns the default reason phrase for an HTTP status code (e.g. `200` → `"OK"`, `404` → `"Not Found"`). Unknown codes fall back to a class label so the status line is always well-formed.

**Parameters**:  
- `code`: HTTP status code.

**Return Value**:  
Static reason-phrase string. Never `NULL`.

**Usage Case**:  
Pass directly to `http_set_status`: `http_set_status(res, 200, http_status_text(200))`.

---

### `char* http_url_decode(const char* in, size_t in_len)`

**Purpose**:  
Percent-decodes a URL-encoded byte string: `+` becomes space, `%XX` becomes the corresponding byte, everything else is copied verbatim. Malformed `%` escapes are passed through literally. The caller must `free()` the result.

**Parameters**:  
- `in`: Input URL-encoded string.  
- `in_len`: Length of `in` in bytes.

**Return Value**:  
Newly allocated NUL-terminated decoded string, or `NULL` on allocation failure.

**Usage Case**:  
Decode query-string values and `application/x-www-form-urlencoded` bodies.

---

### `char* http_url_encode(const char* in)`

**Purpose**:  
Percent-encodes a NUL-terminated string using the RFC 3986 unreserved set. Alphanumerics and `-`, `_`, `.`, `~` pass through; every other byte becomes `%XX`. The caller must `free()` the result.

**Parameters**:  
- `in`: NUL-terminated input string to encode.

**Return Value**:  
Newly allocated NUL-terminated encoded string (worst case 3× the input), or `NULL` on allocation failure.

**Usage Case**:  
Safely embed user-supplied strings in URLs or form bodies.

---

### `const char* http_get_header_ci(const HttpRequest* req, const char* name)`

**Purpose**:  
Case-insensitive request header lookup. HTTP header field names are case-insensitive (RFC 7230 §3.2), so this is the correct choice in production — a client may send `content-type` while your code looks up `Content-Type`. (The older `http_get_header` is case-sensitive and would miss it.)

**Parameters**:  
- `req`: The parsed request. `NULL` → `NULL`.  
- `name`: Header name to find, matched case-insensitively. `NULL` → `NULL`.

**Return Value**:  
Pointer to the header value (owned by `req`, valid until `http_free_request`), or `NULL` if absent. If the header appears more than once, the first occurrence is returned.

**Usage Case**:  
Robustly read `Authorization`, `Host`, `Content-Type`, etc. regardless of the casing the client used.

---

### `const char* http_content_type(const HttpRequest* req)`

**Purpose**:  
Convenience accessor for the request's `Content-Type` header (looked up case-insensitively). The value may include parameters, e.g. `"application/json; charset=utf-8"`.

**Parameters**:  
- `req`: The parsed request. `NULL` → `NULL`.

**Return Value**:  
The `Content-Type` value (owned by `req`), or `NULL` if the header is absent.

**Usage Case**:  
Branch on the request media type — e.g. parse the body as JSON only when the content type says so.

---

### `long http_content_length(const HttpRequest* req)`

**Purpose**:  
Returns the request's `Content-Length` as a non-negative byte count. The header is looked up case-insensitively and parsed strictly as RFC 7230 `1*DIGIT`: the value must be non-empty and contain digits only (no sign, whitespace, or trailing junk). Integer overflow is detected and rejected.

**Parameters**:  
- `req`: The parsed request. `NULL` → `-1`.

**Return Value**:  
The declared body length (`>= 0`), or `-1` if the header is absent, empty, malformed, or would overflow `long`.

**Usage Case**:  
Validate or size body handling in production — e.g. reject requests whose `Content-Length` exceeds a limit before reading the body.

---

### `bool http_method_from_string(const char* str, HttpMethod* out_method)`

**Purpose**:  
Parses a bare method token (`"GET"`, `"POST"`, `"PUT"`, `"DELETE"`, `"OPTIONS"`, `"HEAD"`, `"PATCH"`) into an `HttpMethod`. HTTP methods are case-sensitive (RFC 7230 §3.1.1), so lowercase input is rejected. This is the inverse of `http_method_to_string`, and differs from `http_parse_method` (which expects a full request line — a verb followed by a space).

**Parameters**:  
- `str`: The method token. `NULL` → `false`.  
- `out_method`: Receives the parsed method on success. `NULL` → `false`.

**Return Value**:  
`true` (and writes `*out_method`) if the token is a recognised verb; otherwise `false`, leaving `*out_method` unchanged.

**Usage Case**:  
Map a method string from a client, proxy rule, or config into the enum your routing switches on.

---

### `int http_status_class(int code)`

**Purpose**:  
Returns the class (leading digit) of an HTTP status code: `1` (1xx informational), `2` (2xx success), `3` (3xx redirection), `4` (4xx client error), `5` (5xx server error). Codes outside the valid 100–599 range return `0`.

**Parameters**:  
- `code`: HTTP status code.

**Return Value**:  
`1`–`5` for a valid code, or `0` if out of range.

**Usage Case**:  
Concise success/error checks and metrics, e.g. `if (http_status_class(code) == 2) { /* success */ }` or bucketing responses by class.

---

## HTTP Client

A blocking client that talks to real servers over `http://` and `https://`. It
parses the URL, connects (with a bounded connect timeout), optionally negotiates
TLS, sends the request, reads the response framed by `Content-Length` / chunked
`Transfer-Encoding` / connection-close, follows redirects, and hands back a
parsed `HttpResponse`. Per-read idle timeouts are enforced with `select()`
(`tcp_wait_readable`), so a stalled server can't wedge the call.

### `bool http_url_parse(const char* url, HttpUrl* out)` / `void http_url_free(HttpUrl*)`

Parse `scheme://[user@]host[:port][/path][?query][#fragment]` (http/https only).
Absent components are `NULL`; `path` defaults to `"/"`; the port defaults to
80/443. IPv6 literals are accepted (`http://[::1]:9000/x` → host `::1`). Returns
`false` on a malformed/unsupported URL. Free the owned fields with `http_url_free`.

### `HttpResponse* http_parse_response(const char* data, size_t length)` / `void http_response_free(HttpResponse*)`

Parse a full response message (status line + headers + already-de-framed body)
into a heap `HttpResponse`. Binary-safe: the exact body size is in
`res->body_length` (the body may contain NULs; it is also NUL-terminated for
convenience). Free with `http_response_free` (releases the contents **and** the
struct — distinct from `http_free_response`, which only releases the contents of
a stack response built on the server side).

### `HttpResponse* http_request(method, url, headers, header_count, body, body_len, opts)`

Perform one request and return the final `HttpResponse` (after redirects), or
`NULL` on a DNS/connect/TLS/transport error. `headers` are extra request headers
(`Host` and `Content-Length` are generated for you); `body` is sent verbatim.
`opts` may be `NULL` for defaults. Free the result with `http_response_free`.

`HttpClientOptions` (zero-initialize for defaults):
- `timeout_ms` — per-read idle timeout; `<= 0` keeps the 30 s default.
- `max_redirects` — follow up to N `3xx` with `Location`; `< 0` ⇒ default 5. To
  disable following entirely, set `follow_redirects_set = true` and
  `max_redirects = 0` (you then get the `3xx` response back).
- `tls_insecure` — skip certificate/hostname verification (self-signed / testing).

Redirects: `301/302/303` downgrade a non-GET/HEAD request to `GET` and drop the
body; `307/308` preserve method and body.

### `http_get` / `http_post` / `http_put` / `http_delete`

Convenience wrappers over `http_request` with default options:

```c
HttpResponse* http_get   (const char* url);
HttpResponse* http_post  (const char* url, const char* content_type, const void* body, size_t body_len);
HttpResponse* http_put   (const char* url, const char* content_type, const void* body, size_t body_len);
HttpResponse* http_delete(const char* url);
```

```c
#include "network/http.h"
#include "fmt/fmt.h"

int main(void) {
    HttpResponse* r = http_get("http://example.com/");
    if (r) {
        fmt_printf("status=%d, %zu body bytes\n", r->status_code, r->body_length);
        http_response_free(r);
    }

    /* POST JSON */
    const char* json = "{\"name\":\"alice\"}";
    r = http_post("http://localhost:8080/users", "application/json", json, strlen(json));
    if (r) { fmt_printf("created: %d\n", r->status_code); http_response_free(r); }
    return 0;
}
```

> **HTTPS / certificate verification.** `https://` is wired through the TCP
> layer's TLS (`tcp_ssl_init_client` + `tcp_ssl_connect`). By default the client
> verifies the server certificate chain against the **system CA store**, which is
> present on typical Linux installs but is **empty on a bare MinGW/Windows**
> OpenSSL build — there `https://` with verification fails closed (secure by
> default). Set `tls_insecure = true` to connect anyway, or install a CA bundle.
> Hostname matching is not yet enforced.

---

## Example 9: Hello-World HTTP Server

The minimum boilerplate to run a single-route HTTP server. Visit `http://localhost:8080/` in any browser.

```c
#include "network/http.h"
#include "fmt/fmt.h"


static void index_handler(HttpRequest* req, HttpResponse* res) {
    (void)req;
    http_set_status(res, 200, "OK");
    http_set_body(res, "Hello from a C HTTP server!\n");
}


int main(void) {
    http_register_route("/", HTTP_GET, index_handler);
    fmt_printf("Listening on http://localhost:8080\n");
    http_start_server(8080);
    
    return 0;
}
```

**Sample session** (from `curl`):
```
$ curl -i http://localhost:8080/
HTTP/1.1 200 OK
Content-Type: text/plain

Hello from a C HTTP server!
```

---

## Example 10: JSON REST endpoint with `{id}` placeholder

Demonstrates path-parameter routing, JSON response bodies, and method-aware dispatch. The same path matches both `GET /users/{id}` and `DELETE /users/{id}`, but each verb gets its own handler.

```c
#include "network/http.h"
#include "json/json.h"
#include "fmt/fmt.h"


static void user_get(HttpRequest* req, HttpResponse* res) {
    /* req->id is parsed automatically from the {id} placeholder. */
    JsonElement* root = json_create(JSON_OBJECT);

    JsonElement* id_val = json_create(JSON_NUMBER);
    id_val->value.number_val = (double)req->id;
    json_add_to_object(root, "id", id_val);

    JsonElement* name = json_create(JSON_STRING);
    name->value.string_val = strdup("Alice");
    json_add_to_object(root, "name", name);

    http_set_status(res, 200, "OK");
    http_set_json_body(res, root);   /* takes ownership of `root` */
}

static void user_delete(HttpRequest* req, HttpResponse* res) {
    char msg[64];
    snprintf(msg, sizeof(msg), "deleted user %d", req->id);
    http_set_status(res, 200, "OK");
    http_set_body(res, msg);
}

int main(void) {
    http_register_route("/users/{id}", HTTP_GET,    user_get);
    http_register_route("/users/{id}", HTTP_DELETE, user_delete);
    http_start_server(8080);

    return 0;
}
```

**Sample session**:
```
$ curl -i http://localhost:8080/users/42
HTTP/1.1 200 OK
Content-Type: application/json

{"id": 42, "name": "Alice"}

$ curl -i -X DELETE http://localhost:8080/users/42
HTTP/1.1 200 OK
Content-Type: text/plain

deleted user 42
```

---

## Example 11: HTTP server with query-string parameters + custom headers

The handler reads two query params and echoes them back in a custom header plus the body. Demonstrates `http_get_query_param` and `http_add_header`.

```c
#include "network/http.h"
#include "fmt/fmt.h"

static void search_handler(HttpRequest* req, HttpResponse* res) {
    const char* q    = http_get_query_param(req, "q");
    const char* page = http_get_query_param(req, "page");

    http_set_status(res, 200, "OK");
    http_add_header(res, "X-Echo-Query", q    ? q    : "");
    http_add_header(res, "X-Echo-Page",  page ? page : "");

    char body[256];
    snprintf(body, sizeof(body),
             "searching for '%s' on page %s\n",
             q ? q : "(none)", page ? page : "1");
    http_set_body(res, body);
}

int main(void) {
    http_register_route("/search", HTTP_GET, search_handler);
    http_start_server(8080);

    return 0;
}
```

**Sample session**:
```
$ curl -i "http://localhost:8080/search?q=hello&page=3"
HTTP/1.1 200 OK
X-Echo-Query: hello
X-Echo-Page: 3
Content-Type: text/plain

searching for 'hello' on page 3
```

---

## Example 12: HTTP server accepting a JSON POST body

Demonstrates `req->json_body` for POST bodies that arrive as JSON. The handler echoes back a summary; `http_set_json_body` builds the response.

```c
#include "network/http.h"
#include "json/json.h"
#include "fmt/fmt.h"

static void create_handler(HttpRequest* req, HttpResponse* res) {
    if (!req->json_body) {
        http_send_error(res, 400, "expected JSON body");
        return;
    }

    JsonElement* name_el = json_get_element(req->json_body, "name");
    const char* name = (name_el && name_el->type == JSON_STRING)
                       ? name_el->value.string_val : "unknown";

    JsonElement* reply = json_create(JSON_OBJECT);
    JsonElement* status = json_create(JSON_STRING);
    status->value.string_val = strdup("created");
    json_add_to_object(reply, "status", status);

    JsonElement* who = json_create(JSON_STRING);
    who->value.string_val = strdup(name);
    json_add_to_object(reply, "name", who);

    http_set_status(res, 201, "Created");
    http_set_json_body(res, reply);
}

int main(void) {
    http_register_route("/items", HTTP_POST, create_handler);
    http_start_server(8080);

    return 0;
}
```

**Sample session**:
```
$ curl -i -X POST http://localhost:8080/items \
       -H 'Content-Type: application/json' \
       -d '{"name":"widget"}'
HTTP/1.1 201 Created
Content-Type: application/json

{"status": "created", "name": "widget"}
```

---

## Example 13: UDP — print local address + bound port

Useful diagnostic: bind to an ephemeral port and ask the kernel which port it picked. Uses `udp_resolve_hostname` to find the local IP and `getsockname` (via the standard sockets API) to read back the assigned port.

```c
#include "network/udp.h"
#include "fmt/fmt.h"
#include <sys/types.h>

#if !defined(_WIN32)
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif


int main(void) {
    udp_init();

    UdpSocket s;
    udp_socket_create(&s);
    udp_bind(s, NULL, 0);   /* wildcard + ephemeral port */

    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);
    getsockname((int)s, (struct sockaddr*)&ss, &slen);

    unsigned short port = 0;

    if (ss.ss_family == AF_INET6) {
        port = ntohs(((struct sockaddr_in6*)&ss)->sin6_port);
    }
    else if (ss.ss_family == AF_INET) {
        port = ntohs(((struct sockaddr_in*)&ss)->sin_port);
    }

    char ip[64] = {0};
    udp_resolve_hostname("localhost", ip, sizeof(ip));

    fmt_printf("Bound to %s:%u\n", ip, port);

    udp_close(s);
    udp_cleanup();
    return 0;
}
```

**Sample output**:
```
Bound to ::1:54213
```

---

## Example 14: UDP — request/response client with timeout

Sends a one-shot request, waits up to 1 second for a reply, prints whatever came back (or `"(timed out)"`). Useful template for ping / NTP-style clients.

```c
#include "network/udp.h"
#include "fmt/fmt.h"
#include <string.h>

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT 9000

int main(void) {
    udp_init();

    UdpSocket sock;
    udp_socket_create(&sock);
    udp_set_timeout(sock, 1000);   /* 1-second recv timeout */

    const char* msg = "PING";
    udp_sendto(sock, msg, strlen(msg), NULL, SERVER_HOST, SERVER_PORT);

    char buf[512];
    char src[128] = {0};
    unsigned short src_port = 0;
    size_t recvd = 0;

    UdpStatus s = udp_recvfrom(sock, buf, sizeof(buf) - 1, &recvd,
                               src, sizeof(src), &src_port);
    if (s == UDP_SUCCESS) {
        buf[recvd] = '\0';
        fmt_printf("reply from %s:%u: %s\n", src, src_port, buf);
    } 
    else {
        fmt_printf("(timed out)\n");
    }

    udp_close(sock);
    udp_cleanup();
    return 0;
}
```

---

## Example 15: UDP — many-client echo server using `udp_set_non_blocking`

A non-blocking echo loop that polls in a tight loop with a 10 ms sleep when there's nothing to read. Equivalent to a `select`-style server but simpler.

```c
#include "network/udp.h"
#include "fmt/fmt.h"
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
  #include <windows.h>
  static void sleep_ms(int ms) { Sleep(ms); }
#else
  #include <unistd.h>
  static void sleep_ms(int ms) { usleep(ms * 1000); }
#endif

int main(void) {
    udp_init();

    UdpSocket sock;
    udp_socket_create(&sock);
    udp_bind(sock, NULL, 9100);
    udp_set_non_blocking(sock, true);

    fmt_printf("Non-blocking echo server on UDP/9100. Ctrl-C to stop.\n");

    char buf[1024];
    char src[128];
    unsigned short src_port;
    size_t recvd;

    for (;;) {
        UdpStatus s = udp_recvfrom(sock, buf, sizeof(buf) - 1, &recvd,
                                   src, sizeof(src), &src_port);
        if (s == UDP_SUCCESS && recvd > 0) {
            buf[recvd] = '\0';
            fmt_printf("from %s:%u -> %s\n", src, src_port, buf);
            udp_sendto(sock, buf, recvd, NULL, src, src_port);
        } 
        else if (s == UDP_ERR_NO_DATA) {
            sleep_ms(10);   /* idle */
        } 
        else {
            UdpStatusInfo err;
            udp_get_last_error(&err);
            fmt_printf("recv error: %s\n", err.message);
            break;
        }
    }

    udp_close(sock);
    udp_cleanup();
    return 0;
}
```

---

## Example 16: HTTP server using `http_status_text`, `http_set_html_body`, and `http_redirect`

Three new helpers all in one handler tree. `/health` returns an HTML page, `/old-home` redirects to `/`, and `/` shows a one-line greeting.

```c
#include "network/http.h"
#include "fmt/fmt.h"


static void index_handler(HttpRequest* req, HttpResponse* res) {
    (void)req;
    http_set_status(res, 200, http_status_text(200));
    http_set_html_body(res, "<h1>Welcome</h1><p>Try /health or /old-home</p>");
}

static void health_handler(HttpRequest* req, HttpResponse* res) {
    (void)req;
    http_set_status(res, 200, http_status_text(200));
    http_set_html_body(res, "<p>OK</p>");
}

static void old_home_handler(HttpRequest* req, HttpResponse* res) {
    (void)req;
    /* Permanent redirect to the new path. */
    http_redirect(res, 301, "/");
}

int main(void) {
    http_register_route("/",         HTTP_GET, index_handler);
    http_register_route("/health",   HTTP_GET, health_handler);
    http_register_route("/old-home", HTTP_GET, old_home_handler);

    fmt_printf("http://localhost:8080  (/, /health, /old-home)\n");
    http_start_server(8080);

    return 0;
}
```

**Verify:**
```
$ curl -i http://localhost:8080/old-home
HTTP/1.1 301 Moved Permanently
Location: /
Content-Type: text/html; charset=utf-8

<html><body>Redirecting to <a href="/">/</a></body></html>
```

---

## Example 17: Cookie-based login flow with `http_set_cookie` / `http_get_cookie`

`POST /login` issues a session cookie; `GET /me` reads it back and reports who's logged in. Notice `http_set_cookie`'s `path=/`, `max_age=3600`, `http_only=true` arguments.

```c
#include "network/http.h"
#include "fmt/fmt.h"
#include <string.h>
#include <stdlib.h>


static void login_handler(HttpRequest* req, HttpResponse* res) {
    /* For demo: any POST body sets the session to "demo-user". */
    (void)req;
    http_set_cookie(res, "session", "demo-user", "/", 3600, true);
    http_set_status(res, 200, http_status_text(200));
    http_set_html_body(res, "<p>Logged in. <a href=\"/me\">/me</a></p>");
}

static void me_handler(HttpRequest* req, HttpResponse* res) {
    char* session = http_get_cookie(req, "session");
    if (!session) {
        http_send_error(res, 401, http_status_text(401));
        return;
    }
    http_set_status(res, 200, http_status_text(200));

    char body[256];
    snprintf(body, sizeof(body), "<p>Hello, <b>%s</b></p>", session);
    http_set_html_body(res, body);
    free(session);
}

int main(void) {
    http_register_route("/login", HTTP_POST, login_handler);
    http_register_route("/me",    HTTP_GET,  me_handler);
    
    fmt_printf("Login: curl -i -X POST http://localhost:8080/login\n");
    http_start_server(8080);

    return 0;
}
```

**Verify:**
```
$ curl -i -X POST http://localhost:8080/login
HTTP/1.1 200 OK
Set-Cookie: session=demo-user; Path=/; Max-Age=3600; HttpOnly
Content-Type: text/html; charset=utf-8

<p>Logged in. <a href="/me">/me</a></p>

$ curl -i -H 'Cookie: session=demo-user' http://localhost:8080/me
HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8

<p>Hello, <b>demo-user</b></p>
```

---

## Example 18: URL encode/decode round-trip + overriding `Content-Type`

Demonstrates `http_url_encode` / `http_url_decode` for safely building URLs, and `http_response_remove_header` / `http_response_get_header` for replacing the `Content-Type` that `http_set_body` adds by default.

```c
#include "network/http.h"
#include "fmt/fmt.h"
#include <string.h>
#include <stdlib.h>

int main(void) {
    /* Build a URL safely from raw user input. */
    const char* q = "hello world & friends?";
    char* enc = http_url_encode(q);
    fmt_printf("encoded:  %s\n", enc);

    char* dec = http_url_decode(enc, strlen(enc));
    fmt_printf("decoded:  %s\n", dec);
    free(enc); free(dec);

    /* Build a response with a custom binary content type. */
    HttpResponse res;
    memset(&res, 0, sizeof(res));
    http_set_status(&res, 200, http_status_text(200));
    http_set_body(&res, "\x89PNG\r\n");                       /* sets Content-Type: text/plain */
    fmt_printf("default Content-Type: %s\n", http_response_get_header(&res, "Content-Type"));

    http_response_remove_header(&res, "Content-Type");        /* drop the wrong one */
    http_add_header(&res, "Content-Type", "image/png");       /* set the right one */
    fmt_printf("override Content-Type: %s\n", http_response_get_header(&res, "Content-Type"));

    char* wire = http_serialize_response(&res);
    fmt_printf("---\n%s\n", wire);
    free(wire);
    http_free_response(&res);
    return 0;
}
```

**Expected output:**
```
encoded:  hello%20world%20%26%20friends%3F
decoded:  hello world & friends?
default Content-Type: text/plain
override Content-Type: image/png
---
HTTP/1.1 200 OK
Content-Type: image/png

<binary PNG bytes>
```

---

## Example 19: "Connected" UDP client using `udp_connect` / `udp_send` / `udp_recv`

`udp_connect` lets you write a UDP client that looks almost like a TCP client — no per-call destination arguments, and incoming packets from unexpected peers are filtered out. Pair with `udp_status_to_string` for clean error logs.

```c
#include "network/udp.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    udp_init();

    UdpSocket s;
    if (udp_socket_create(&s) != UDP_SUCCESS) return 1;

    /* Pin the socket to the server. After this, no more sendto/recvfrom — just send/recv. */
    UdpStatus rc = udp_connect(s, "::1", 9999);
    if (rc != UDP_SUCCESS) {
        fmt_printf("connect: %s\n", udp_status_to_string(rc));
        udp_close(s); udp_cleanup(); return 1;
    }

    const char* req = "ping";
    size_t sent = 0;
    rc = udp_send(s, req, strlen(req), &sent);
    fmt_printf("send: %s (%zu bytes)\n", udp_status_to_string(rc), sent);

    char buf[256];
    size_t got = 0;
    rc = udp_recv(s, buf, sizeof(buf) - 1, &got);
    if (rc == UDP_SUCCESS) {
        buf[got] = '\0';
        fmt_printf("recv: %s\n", buf);
    } else {
        fmt_printf("recv: %s\n", udp_status_to_string(rc));
    }

    udp_close(s);
    udp_cleanup();
    return 0;
}
```

**Why it matters:** without `udp_connect`, every `udp_sendto` call has to re-resolve the destination, and `udp_recvfrom` will deliver datagrams from any peer (including spoofed source addresses). A connected UDP socket sidesteps both.

---

## Example 20: Discovering the ephemeral port and tuning buffers

`udp_get_local_address` is most useful when you bind to port 0 (kernel-assigned) — a common pattern when you don't want to hard-code a port for testing or for one-shot replies. `udp_set_buffer_size` boosts throughput on bursty workloads, and `udp_set_reuse_addr` makes restart cycles painless.

```c
#include "network/udp.h"
#include "fmt/fmt.h"

int main(void) {
    udp_init();

    UdpSocket s;
    udp_socket_create(&s);

    /* Quick-restart hygiene + 1 MiB receive buffer. */
    udp_set_reuse_addr(s, true);
    udp_set_buffer_size(s, 0, 1 << 20);

    /* Wildcard bind, ephemeral port. */
    udp_bind(s, NULL, 0);

    char host[64];
    unsigned short port = 0;
    udp_get_local_address(s, host, sizeof(host), &port);
    fmt_printf("listening on %s:%u\n", host, port);

    /* ... usual recvfrom loop here ... */

    udp_close(s);
    udp_cleanup();
    return 0;
}
```

**Sample output** (port is whatever the kernel picks; the wildcard prints as the IPv6 unspecified address `::`):
```
listening on :::51743
```


---

## Example 21: Low-latency request/response client (`tcp_set_nodelay` + `tcp_set_keep_alive` + `tcp_send_all` / `tcp_recv_all`)

A typical RPC client setup: turn off Nagle to send small requests immediately, enable keep-alive so a dead server gets detected on idle, and use the loop-until-done helpers so a partial write or split TCP segment doesn't silently drop data.

```c
#include "network/tcp.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    tcp_init();

    TcpSocket s;
    if (tcp_socket_create(&s) != TCP_SUCCESS) { tcp_cleanup(); return 1; }

    /* Low-latency posture before connecting. */
    tcp_set_nodelay(s, true);
    tcp_set_keep_alive(s, true);

    TcpStatus rc = tcp_connect(s, "127.0.0.1", 9000);
    if (rc != TCP_SUCCESS) {
        fmt_printf("connect: %s\n", tcp_status_to_string(rc));
        tcp_close(s); tcp_cleanup(); return 1;
    }

    /* Length-prefixed request: 4-byte big-endian length + payload. */
    const char* payload = "PING";
    unsigned char hdr[4] = { 0, 0, 0, (unsigned char)strlen(payload) };

    rc = tcp_send_all(s, hdr, sizeof(hdr));
    if (rc == TCP_SUCCESS) rc = tcp_send_all(s, payload, strlen(payload));
    if (rc != TCP_SUCCESS) {
        fmt_printf("send: %s\n", tcp_status_to_string(rc));
        tcp_close(s); tcp_cleanup(); return 1;
    }

    /* Read length, then exactly that many bytes. */
    unsigned char rh[4];
    if (tcp_recv_all(s, rh, sizeof(rh)) != TCP_SUCCESS) {
        fmt_printf("recv header: failed\n");
        tcp_close(s); tcp_cleanup(); return 1;
    }
    size_t body_len = (size_t)rh[3];

    char body[256] = {0};
    if (tcp_recv_all(s, body, body_len) != TCP_SUCCESS) {
        fmt_printf("recv body: failed\n");
        tcp_close(s); tcp_cleanup(); return 1;
    }
    body[body_len] = '\0';
    fmt_printf("response: %s\n", body);

    tcp_close(s);
    tcp_cleanup();
    return 0;
}
```

**Why it matters:** without `tcp_send_all`, a 4-byte header could be split across two `send()` calls under high load — your "small atomic write" assumption breaks. Without `tcp_set_nodelay`, every small request could be delayed up to 200ms by Nagle waiting for more bytes that never come.

---

## Example 22: Quick-restart server with `tcp_set_linger` (abortive close)

When a request handler decides the client is malicious, you want the connection slammed shut without going through the polite TIME_WAIT state — both to free the port immediately for restarts and to deny the client any in-flight bytes.

```c
#include "network/tcp.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    tcp_init();

    TcpSocket srv;
    tcp_socket_create(&srv);
    tcp_set_reuse_addr(srv, true);
    tcp_set_buffer_size(srv, 1 << 20, 1 << 20);   /* 1 MiB buffers */

    tcp_bind(srv, NULL, 9001);
    tcp_listen(srv, 32);

    fmt_printf("listening on 9001\n");

    while (1) {
        TcpSocket c;
        if (tcp_accept(srv, &c) != TCP_SUCCESS) continue;

        char buf[512] = {0};
        size_t got = 0;
        TcpStatus rc = tcp_recv(c, buf, sizeof(buf) - 1, &got);

        if (rc != TCP_SUCCESS || strncmp(buf, "BAD ", 4) == 0) {
            /* Abortive close: send RST, no TIME_WAIT, no graceful flush. */
            tcp_set_linger(c, true, 0);
            tcp_close(c);
            fmt_printf("rejected (%s)\n", tcp_status_to_string(rc));
            continue;
        }

        /* Normal client: graceful close. */
        const char* reply = "OK\n";
        tcp_send_all(c, reply, strlen(reply));
        tcp_close(c);
    }

    tcp_close(srv);
    tcp_cleanup();
    
    return 0;
}
```

**Verify a normal request:**
```
$ printf 'HELLO\n' | nc 127.0.0.1 9001
OK
```

**Verify the abortive path:** any `BAD …` request gets an immediate RST — `nc` reports `connection reset by peer` rather than a clean EOF. The server can restart instantly without waiting for the kernel to release the port.

---

## Example 23: Read request metadata with `http_content_type`, `http_content_length`, `http_get_header_ci`

```c
#include "network/http.h"
#include "fmt/fmt.h"

int main() {
    const char* raw =
        "POST /upload?debug=1 HTTP/1.1\r\n"
        "Host: api.example.com\r\n"
        "content-type: application/json\r\n"   /* lowercase on purpose */
        "Content-Length: 25\r\n"
        "\r\n"
        "{\"name\":\"ada\",\"id\":12345}";

    HttpRequest* req = http_parse_request(raw);
    if (!req) {
        fmt_printf("parse failed\n");
        return 1;
    }

    /* Case-insensitive lookup finds the header despite the lowercase name. */
    fmt_printf("Content-Type   : %s\n",  http_content_type(req));
    fmt_printf("Content-Length : %ld\n", http_content_length(req));
    fmt_printf("Host           : %s\n",  http_get_header_ci(req, "HOST"));

    http_free_request(req);
    return 0;
}
```
**Result**
```
Content-Type   : application/json
Content-Length : 25
Host           : api.example.com
```

---

## Example 24: Parse method tokens and classify status codes with `http_method_from_string` / `http_status_class`

```c
#include "network/http.h"
#include "fmt/fmt.h"

int main() {
    const char* tokens[] = { "GET", "PATCH", "get", "BREW" };
    for (int i = 0; i < 4; ++i) {
        HttpMethod m;

        if (http_method_from_string(tokens[i], &m)) {
            fmt_printf("%-6s -> %s\n", tokens[i], http_method_to_string(m));
        } 
        else {
            fmt_printf("%-6s -> (rejected)\n", tokens[i]);
        }
    }

    int codes[] = { 200, 301, 404, 503 };
    for (int i = 0; i < 4; ++i) {
        fmt_printf("%d is %dxx\n", codes[i], http_status_class(codes[i]));
    }
    return 0;
}
```
**Result**
```
GET    -> GET
PATCH  -> PATCH
get    -> (rejected)
BREW   -> (rejected)
200 is 2xx
301 is 3xx
404 is 4xx
503 is 5xx
```

---

## Example 25: readiness + peer introspection with `udp_set_ttl`, `udp_set_multicast_loopback`, `udp_get_peer_address`, `udp_wait_readable`, `udp_bytes_available`

A self-contained loopback demo: a receiver bound to an ephemeral port and a connected sender. The sender caps its hop count, pins its peer, and the receiver waits for the datagram, then sizes the read exactly.

```c
#include "network/udp.h"
#include "fmt/fmt.h"

int main() {
    udp_init();

    /* Receiver bound to an ephemeral port on loopback. */
    UdpSocket recv_sock;
    udp_socket_create(&recv_sock);
    udp_bind(recv_sock, "::1", 0);

    char host[INET6_ADDRSTRLEN] = {0};
    unsigned short port = 0;
    udp_get_local_address(recv_sock, host, sizeof(host), &port);

    /* Sender: cap hop count, silence own multicast, pin the peer. */
    UdpSocket send_sock;
    udp_socket_create(&send_sock);
    udp_set_ttl(send_sock, 64);
    udp_set_multicast_loopback(send_sock, false);
    udp_connect(send_sock, "::1", port);

    char peer[INET6_ADDRSTRLEN] = {0};
    unsigned short peer_port = 0;
    udp_get_peer_address(send_sock, peer, sizeof(peer), &peer_port);
    fmt_printf("Sender's peer host: %s\n", peer);
    fmt_printf("Peer port matches receiver: %s\n", (peer_port == port) ? "yes" : "no");

    size_t sent = 0;
    udp_send(send_sock, "telemetry", 9, &sent);

    /* Wait up to 1s for the datagram, then size the read from the queue. */
    if (udp_wait_readable(recv_sock, 1000) == UDP_SUCCESS) {
        size_t avail = 0;
        udp_bytes_available(recv_sock, &avail);
        fmt_printf("Bytes ready: %zu\n", avail);

        char buf[64] = {0};
        size_t got = 0;
        udp_recvfrom(recv_sock, buf, sizeof(buf) - 1, &got, NULL, 0, NULL);
        fmt_printf("Received: %s (%zu bytes)\n", buf, got);
    }

    udp_close(send_sock);
    udp_close(recv_sock);
    udp_cleanup();
    return 0;
}
```
**Result**
```
Sender's peer host: ::1
Peer port matches receiver: yes
Bytes ready: 9
Received: telemetry (9 bytes)
```

---

## Example 26: bounded connect + readiness with `tcp_connect_timeout`, `tcp_get_socket_error`, `tcp_wait_readable`, `tcp_bytes_available`

A self-contained loopback demo: a one-shot echo server on a background thread, and a client that connects with a 2-second budget, confirms success via `SO_ERROR`, then waits for the echo and sizes the read exactly.

```c
#include "network/tcp.h"
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"

static volatile int g_port = 0;

/* Tiny one-shot echo server on loopback. */
static int echo_server(void* arg) {
    (void)arg;
    TcpSocket ls = 0;
    tcp_socket_create(&ls);
    tcp_set_reuse_addr(ls, true);
    tcp_bind(ls, "127.0.0.1", 0);

    char h[64]; unsigned short p = 0;
    tcp_get_sock_name(ls, h, sizeof(h), &p);
    g_port = p;                       /* publish the chosen port */
    tcp_listen(ls, 1);

    TcpSocket cl = 0;
    if (tcp_accept(ls, &cl) == TCP_SUCCESS) {
        char buf[64]; size_t got = 0;
        if (tcp_recv(cl, buf, sizeof(buf), &got) == TCP_SUCCESS && got > 0) {
            tcp_send_all(cl, buf, got);
        }
        tcp_close(cl);
    }
    tcp_close(ls);
    return 0;
}

int main(void) {
    tcp_init();

    Thread srv;
    thread_create(&srv, echo_server, NULL);
    while (g_port == 0) { /* wait for the server to bind and publish its port */ }

    TcpSocket c = 0;
    tcp_socket_create(&c);

    /* Bounded connect: never blocks more than 2 seconds. */
    if (tcp_connect_timeout(c, "127.0.0.1", (unsigned short)g_port, 2000) == TCP_SUCCESS) {
        int err = -1;
        tcp_get_socket_error(c, &err);
        fmt_printf("connected, socket error = %d\n", err);

        tcp_send_all(c, "hello", 5);

        /* Wait up to 1s for the echo, then size the read from the queue. */
        if (tcp_wait_readable(c, 1000) == TCP_SUCCESS) {
            size_t avail = 0;
            tcp_bytes_available(c, &avail);
            fmt_printf("bytes available: %zu\n", avail);

            char buf[64] = {0};
            size_t got = 0;
            tcp_recv(c, buf, sizeof(buf) - 1, &got);
            fmt_printf("echo: %s\n", buf);
        }
    }

    tcp_close(c);
    int r = 0;
    thread_join(srv, &r);
    tcp_cleanup();
    return 0;
}
```
**Result**
```
connected, socket error = 0
bytes available: 5
echo: hello
```

---

## Example 27: HTTP client — `http_get` and inspecting the response

```c
#include "network/http.h"
#include "fmt/fmt.h"

int main(void) {
    HttpResponse* r = http_get("http://example.com/");
    
    if (!r) {
        fmt_printf("request failed\n");
        return 1;
    }
    const char* ct = http_response_get_header(r, "Content-Type");

    fmt_printf("status : %d %s\n", r->status_code, r->status_message);
    fmt_printf("type   : %s\n", ct ? ct : "(none)");
    fmt_printf("body   : %zu bytes\n", r->body_length);
    http_response_free(r);          

    return 0;
}
```
**Result**
```
status : 200 OK
type   : text/html
body   : 528 bytes
```

---

## Example 28: HTTP client — POST a JSON body with `http_post`

```c
#include "network/http.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    const char* json = "{\"name\":\"alice\",\"role\":\"admin\"}";
    HttpResponse* r = http_post("http://localhost:8080/users",
                                "application/json", json, strlen(json));
    if (r) {
        fmt_printf("created -> %d %s\n", r->status_code, r->status_message);
        http_response_free(r);
    }
    return 0;
}
```

---

## Example 29: HTTP client — custom headers and `HttpClientOptions`

```c
#include "network/http.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    HttpHeader hdrs[2] = {
        { (char*)"Authorization", (char*)"Bearer t0ken" },
        { (char*)"Accept",        (char*)"application/json" },
    };

    HttpClientOptions opt;
    memset(&opt, 0, sizeof opt);
    opt.timeout_ms    = 5000;   /* per-read idle timeout (ms) */
    opt.max_redirects = 3;      /* follow up to 3 redirects   */

    HttpResponse* r = http_request(HTTP_GET, "http://localhost:8080/api/me", hdrs, 2, NULL, 0, &opt);
    if (r) {
        fmt_printf("status %d, %zu body bytes\n", r->status_code, r->body_length);
        http_response_free(r);
    }
    return 0;
}
```

---

## Example 30: HTTP client — following (and not following) redirects

```c
#include "network/http.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    /* Default: follow up to 5 redirects, return the final response. */
    HttpResponse* a = http_get("http://localhost:8080/old-path");
    if (a) {
        fmt_printf("followed -> final status %d\n", a->status_code);
        http_response_free(a);
    }

    /* Opt out to inspect the 3xx + Location yourself. */
    HttpClientOptions o;
    memset(&o, 0, sizeof o);
    o.follow_redirects_set = true;
    o.max_redirects        = 0;
    HttpResponse* b = http_request(HTTP_GET, "http://localhost:8080/old-path", NULL, 0, NULL, 0, &o);

    if (b) {
        const char* loc = http_response_get_header(b, "Location");
        fmt_printf("status %d, Location: %s\n", b->status_code, loc ? loc : "(none)");
        http_response_free(b);
    }
    return 0;
}
```

---

## Example 31: HTTP client — `HEAD` and downloading a large/chunked body

```c
#include "network/http.h"
#include "fmt/fmt.h"

int main(void) {
    /* HEAD: metadata only — the client reads no body for a HEAD reply. */
    HttpResponse* h = http_request(HTTP_HEAD, "http://localhost:8080/big.bin", NULL, 0, NULL, 0, NULL);
    if (h) {
        const char* cl = http_response_get_header(h, "Content-Length");
        fmt_printf("HEAD: Content-Length=%s, body read=%zu\n", cl ? cl : "?", h->body_length);
        http_response_free(h);
    }

    /* GET: the whole payload, framed by Content-Length OR chunked
       Transfer-Encoding (decoded transparently), read across many recv(). */
    HttpResponse* g = http_get("http://localhost:8080/big.bin");
    if (g) {
        fmt_printf("GET: downloaded %zu bytes\n", g->body_length);
        http_response_free(g);
    }
    return 0;
}
```

---

## Example 32: HTTPS client — verified and insecure

```c
#include "network/http.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    /* Secure by default: verifies the server certificate chain against the
       system CA store (present on typical Linux; may be empty on bare MinGW). */
    HttpResponse* r = http_get("https://example.com/");
    if (r) { 
        fmt_printf("verified GET -> %d\n", r->status_code); 
        http_response_free(r); 
    }
    else   { 
        fmt_printf("verification failed or offline\n"); 
    }

    /* Skip verification — self-signed servers, or hosts with no local CA store. */
    HttpClientOptions o;
    memset(&o, 0, sizeof o);
    o.tls_insecure = true;
    r = http_request(HTTP_GET, "https://self-signed.local/", NULL, 0, NULL, 0, &o);

    if (r) { 
        fmt_printf("insecure GET -> %d\n", r->status_code); 
        http_response_free(r); 
    }

    return 0;
}
```

---

## Example 33: Parsing a URL with `http_url_parse`

```c
#include "network/http.h"
#include "fmt/fmt.h"

int main(void) {
    HttpUrl u;

    if (http_url_parse("https://user@api.example.com:8443/v1/items?page=2#top", &u)) {
        fmt_printf("scheme=%s  https=%d\n", u.scheme, (int)u.is_https);
        fmt_printf("user=%s  host=%s  port=%u\n", u.userinfo ? u.userinfo : "-", u.host, (unsigned)u.port);
        fmt_printf("path=%s  query=%s  frag=%s\n", u.path, u.query ? u.query : "-", u.fragment ? u.fragment : "-");
        http_url_free(&u);
    }

    return 0;
}
```
**Result**
```
scheme=https  https=1
user=user  host=api.example.com  port=8443
path=/v1/items  query=page=2  frag=top
```

---

## Example 34: Parsing a raw response with `http_parse_response`

```c
#include "network/http.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    const char* raw =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 11\r\n"
        "\r\n"
        "{\"ok\":true}";

    HttpResponse* r = http_parse_response(raw, strlen(raw));
    if (r) {
        fmt_printf("status      : %d %s\n", r->status_code, r->status_message);
        fmt_printf("content-type: %s\n", http_response_get_header(r, "Content-Type"));
        fmt_printf("body (%zu)  : %s\n", r->body_length, r->body);
        http_response_free(r);
    }

    return 0;
}
```
**Result**
```
status      : 200 OK
content-type: application/json
body (11)  : {"ok":true}
```

---

## Example 35: Concurrent server with `http_start_server_ex` + graceful stop

A complete, runnable program: a worker-pool server on an OS-assigned port, a
client round-trip, then a clean shutdown — all in one process.

```c
#include "network/http.h"
#include "network/tcp.h"
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include <string.h>

typedef struct { 
    TcpStatus st; 
} Srv;

static void hello(HttpRequest* q, HttpResponse* r) {
    (void)q;

    http_set_status(r, 200, "OK");
    http_set_body(r, "hi from the pool");
}

static int run(void* a) { 
    ((Srv*)a)->st = http_start_server_ex(0); 
    return 0; 
}  /* 0 = ephemeral */


int main(void) {
    http_register_route("/hello", HTTP_GET, hello);
    http_server_set_worker_count(4);        /* 4 worker threads      */
    http_server_set_client_timeout(5000);   /* 5s per-read timeout    */

    Srv s = {0};
    Thread t;
    thread_create(&t, run, &s);

    /* Wait for the listener, then read back the OS-assigned port. */
    unsigned short port = 0;
    for (int i = 0; i < 200 && port == 0; ++i) {
        port = http_server_get_port();
        struct timespec ms = { 0, 5 * 1000 * 1000 };
        thread_sleep(&ms, NULL);
    }
    fmt_printf("listening on 127.0.0.1:%u\n", (unsigned)port);

    char url[64];
    snprintf(url, sizeof url, "http://127.0.0.1:%u/hello", (unsigned)port);
    HttpResponse* r = http_get(url);
    if (r) {
        fmt_printf("client got: %d \"%s\"\n", r->status_code, r->body);
        http_response_free(r);
    }

    http_stop_server();          /* wakes accept(), drains in-flight, joins workers */
    thread_join(t, NULL);
    fmt_printf("server stopped, status=%d (0 = TCP_SUCCESS)\n", (int)s.st);

    return 0;
}
```
**Result**
```
listening on 127.0.0.1:50321
client got: 200 "hi from the pool"
server stopped, status=0 (0 = TCP_SUCCESS)
```

---

## Example 36: Reporting a bind failure with `http_start_server_ex`

Unlike `http_start_server` (which returns `void`), `http_start_server_ex`
returns the bind status, so you can tell whether the port was actually claimed.

```c
#include "network/http.h"
#include "fmt/fmt.h"

int main(void) {
    /* Blocks serving until http_stop_server() is called from another thread;
       returns immediately with an error if the port can't be bound. */
    TcpStatus st = http_start_server_ex(8080);
    if (st != TCP_SUCCESS) {
        fmt_printf("could not bind :8080 (status=%d) — already in use?\n", (int)st);
        return 1;
    }
    return 0;   /* reached only after a clean http_stop_server() */
}
```

---

## Example 37: `405 Method Not Allowed` + `Allow` (server + client)

```c
#include "network/http.h"
#include "network/tcp.h"
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include <string.h>

typedef struct { 
    TcpStatus st; 
} Srv;


static void submit(HttpRequest* q, HttpResponse* r) {
    (void)q; 
    http_set_status(r, 201, "Created"); 
    http_set_body(r, "ok");
}

static int run(void* a) { 
    ((Srv*)a)->st = http_start_server_ex(0); 
    return 0; 
}


int main(void) {
    http_register_route("/submit", HTTP_POST, submit);   /* POST only */

    Srv s = {0};
    Thread t;
    thread_create(&t, run, &s);
    unsigned short port = 0;

    for (int i = 0; i < 200 && port == 0; ++i) {
        port = http_server_get_port();
        struct timespec ms = { 0, 5 * 1000 * 1000 }; thread_sleep(&ms, NULL);
    }

    char url[64];
    snprintf(url, sizeof url, "http://127.0.0.1:%u/submit", (unsigned)port);
    HttpResponse* r = http_get(url); 
                        /* wrong method */
    if (r) {
        const char* allow = http_response_get_header(r, "Allow");
        fmt_printf("GET /submit -> %d, Allow: %s\n", r->status_code, allow ? allow : "(none)");
        http_response_free(r);
    }

    http_stop_server();
    thread_join(t, NULL);

    return 0;
}
```
**Result**
```
GET /submit -> 405, Allow: POST
```

---

## Example 38: Cookies and `{id}` path parameters

```c
#include "network/http.h"
#include "network/tcp.h"
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include <string.h>


typedef struct { 
    TcpStatus st; 
} Srv;


static void login(HttpRequest* q, HttpResponse* r) {
    (void)q;
    http_set_status(r, 200, "OK");
    http_set_cookie(r, "sid", "abc123", "/", 3600, true);   /* HttpOnly, 1h */
    http_set_body(r, "logged in");
}

static void item(HttpRequest* q, HttpResponse* r) {
    char b[48];
    snprintf(b, sizeof b, "item #%d", q->id);               /* {id} -> req->id */
    http_set_status(r, 200, "OK");
    http_set_body(r, b);
}

static int run(void* a) { 
    ((Srv*)a)->st = http_start_server_ex(0); 
    return 0; 
}

int main(void) {
    http_register_route("/login",      HTTP_GET, login);
    http_register_route("/items/{id}", HTTP_GET, item);

    Srv s = {0};
    Thread t;
    thread_create(&t, run, &s);

    unsigned short port = 0;
    for (int i = 0; i < 200 && port == 0; ++i) {
        port = http_server_get_port();
        struct timespec ms = { 0, 5 * 1000 * 1000 }; thread_sleep(&ms, NULL);
    }

    char base[48];
    snprintf(base, sizeof base, "http://127.0.0.1:%u", (unsigned)port);
    char url[80];
    snprintf(url, sizeof url, "%s/login", base);
    HttpResponse* a = http_get(url);

    if (a) {
        const char* sc = http_response_get_header(a, "Set-Cookie");
        fmt_printf("login Set-Cookie: %s\n", sc ? sc : "(none)");
        http_response_free(a);
    }

    snprintf(url, sizeof url, "%s/items/42", base);
    HttpResponse* b = http_get(url);
    if (b) { 
        fmt_printf("GET /items/42 -> \"%s\"\n", b->body); 
        http_response_free(b); 
    }

    http_stop_server();
    thread_join(t, NULL);

    return 0;
}
```
**Result**
```
login Set-Cookie: sid=abc123; Path=/; Max-Age=3600; HttpOnly
GET /items/42 -> "item #42"
```

---

## Example 39: Binary-safe parse/serialize (`http_parse_request_ex` / `http_serialize_response_ex`)

```c
#include "network/http.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    /* A request whose body has an embedded NUL — strlen would truncate it,
       so use the length-delimited parser. */
    char raw[64];
    const char* head = "POST /upload HTTP/1.1\r\nContent-Length: 5\r\n\r\n";
    size_t off = strlen(head);
    memcpy(raw, head, off);
    raw[off+0] = 'a'; raw[off+1] = 'b'; raw[off+2] = '\0'; raw[off+3] = 'c'; raw[off+4] = 'd';

    HttpRequest* req = http_parse_request_ex(raw, off + 5);
    if (req) {
        fmt_printf("body_length=%zu, byte[2]=%d (NUL preserved)\n", req->body_length, (int)req->body[2]);
        http_free_request(req);
    }

    /* Serialize a response: serialize_ex reports the EXACT wire length via the
       out-param and auto-adds Content-Length. */
    HttpResponse res;
    memset(&res, 0, sizeof res);
    http_set_status(&res, 200, "OK");
    http_set_body(&res, "pong");
    size_t n = 0;
    char* wire = http_serialize_response_ex(&res, false, &n);

    if (wire) {
        fmt_printf("out_len matches strlen : %s\n", (n == strlen(wire)) ? "yes" : "no");
        fmt_printf("auto Content-Length: 4 : %s\n", strstr(wire, "Content-Length: 4") ? "yes" : "no");
        free(wire);
    }
    http_free_response(&res);

    return 0;
}
```
**Result**
```
body_length=5, byte[2]=0 (NUL preserved)
out_len matches strlen : yes
auto Content-Length: 4 : yes
```

---

## Example 40: Query parameters and multiple response headers

```c
#include "network/http.h"
#include "network/tcp.h"
#include "concurrent/concurrent.h"
#include "fmt/fmt.h"
#include <string.h>

typedef struct { 
    TcpStatus st; 
} Srv;

static void search(HttpRequest* q, HttpResponse* r) {
    const char* term = http_get_query_param(q, "q");   /* ?q=... */

    http_set_status(r, 200, "OK");
    http_add_header(r, "X-Result-Count", "1");
    http_set_body(r, term ? term : "(empty)");
}

static int run(void* a) { 
    ((Srv*)a)->st = http_start_server_ex(0); 
    return 0; 
}

int main(void) {
    http_register_route("/search", HTTP_GET, search);

    Srv s = {0};
    Thread t;
    thread_create(&t, run, &s);
    unsigned short port = 0;

    for (int i = 0; i < 200 && port == 0; ++i) {
        port = http_server_get_port();
        struct timespec ms = { 0, 5 * 1000 * 1000 }; thread_sleep(&ms, NULL);
    }

    char url[80];
    snprintf(url, sizeof url, "http://127.0.0.1:%u/search?q=widgets", (unsigned)port);
    HttpResponse* r = http_get(url);

    if (r) {
        const char* cnt = http_response_get_header(r, "X-Result-Count");
        fmt_printf("query -> \"%s\", X-Result-Count: %s\n", r->body, cnt ? cnt : "(none)");
        http_response_free(r);
    }

    http_stop_server();
    thread_join(t, NULL);

    return 0;
}
```
**Result**
```
query -> "widgets", X-Result-Count: 1
```

---

## Example 41: `tcp_connect_tls` — a verified HTTPS request in one call

A production-grade secure fetch: one call connects, negotiates TLS, and verifies
the server certificate's **chain and hostname** before any data is sent.

```c
#include "network/tcp.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    tcp_init();

    TcpSocket s;
    /* verify = true: the cert must chain to a trusted CA AND be valid for the host. */
    TcpStatus st = tcp_connect_tls("example.com", 443, 5000, true, &s);
    if (st != TCP_SUCCESS) {
        fmt_fprintf(stderr, "secure connect failed: %s\n", tcp_status_to_string(st));
        tcp_cleanup();
        return 1;
    }

    const char* req =
        "GET / HTTP/1.0\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    size_t sent = 0;
    tcp_ssl_send(s, req, strlen(req), &sent);

    char   buf[128];
    size_t got = 0;

    if (tcp_ssl_recv(s, buf, sizeof(buf) - 1, &got) == TCP_SUCCESS) {
        buf[got] = '\0';
        char* eol = strstr(buf, "\r\n");
        
        if (eol) { 
            *eol = '\0'; 
        }
        fmt_printf("server said: %s\n", buf);   /* e.g. HTTP/1.0 200 OK */
    }

    tcp_ssl_close(s);   /* tears down TLS and closes the socket */
    tcp_cleanup();
    return 0;
}
```
**Result**
```
server said: HTTP/1.1 200 OK
```
> On a bare MinGW/Windows build with no system CA store, `verify = true` will
> fail closed — load a CA bundle first (next example) or pass `verify = false`
> for self-signed/testing endpoints.

---

## Example 42: `tcp_ssl_load_verify_locations` + `tcp_ssl_set_verify` — pin a CA bundle

Where the OS trust store is unavailable (or you want to pin a private CA), load a
PEM bundle and require verification explicitly.

```c
#include "network/tcp.h"
#include "fmt/fmt.h"
#include <string.h>

int main(void) {
    tcp_init();
    tcp_ssl_init_client();                                  /* create the client context */

    /* Ship cacert.pem alongside the binary and trust exactly it. */
    if (tcp_ssl_load_verify_locations("cacert.pem", NULL) != TCP_SUCCESS) {
        fmt_fprintf(stderr, "could not load CA bundle\n");
    }
    tcp_ssl_set_verify(true);                               /* fail closed on a bad cert */

    TcpSocket s;
    /* tcp_connect_tls re-uses the context (and its loaded CAs) we just set up. */
    TcpStatus st = tcp_connect_tls("example.com", 443, 5000, true, &s);
    fmt_printf("verified connect: %s\n", tcp_status_to_string(st));
    if (st == TCP_SUCCESS) {
        const char* req = "GET / HTTP/1.0\r\nHost: example.com\r\nConnection: close\r\n\r\n";
        size_t sent = 0;
        tcp_ssl_send(s, req, strlen(req), &sent);
        tcp_ssl_close(s);
    }

    tcp_cleanup();
    return 0;
}
```
**Result** (run without a `cacert.pem` present, on a host that has the system CA store)
```
could not load CA bundle
verified connect: TCP_SUCCESS
```
> With a real `cacert.pem` shipped next to the binary, the first line is absent and the connection is validated against exactly the CAs you pinned — which is what makes `verify = true` work on a bare MinGW build that has no system trust store.

---

## License

This project is open-source and available under the ISC License.
