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
