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
