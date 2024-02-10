

## Example 1 : First server in `TcpSocket`

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

## Example 2 : client Echo with python example  `TcpSocket`

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


## Example 3 : Non Blocking `TcpSocket` client 

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

## Example 4 : get remote and local address with tcp server in `TcpSocket` 

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

## Example 5 : `SSl` Echo Server in `TcpSocket`

*Create server.ccrt and server.key*
`openssl req -x509 -newkey rsa:4096 -keyout server.key -out server.crt -days 365 -nodes -subj "/C=US/ST=YourState/L=YourCity/O=YourOrganization/CN=yourdomain.com"`

*ssl client connect to our echo server*
`openssl s_client -connect localhost:8443 -debug -showcerts`

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