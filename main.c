#include "network/tcp.h"
#include "fmt/fmt.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#define SERVER_PORT 8051
#define BUFFER_SIZE 4096 

int handle_client_request(void *req) {
    TcpSocket client = *(TcpSocket*)req;
    char buffer[BUFFER_SIZE];
    size_t receive;

    TcpStatus status = tcp_recv(client, buffer, BUFFER_SIZE, &receive);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Error : Receive Data from Client Failed\n");
        return -1;
    }

    buffer[receive] = '\0';
    fmt_printf("Receive Data is : %s\n", buffer);

    if (strncmp(buffer, "GET", 3) == 0) {
        const char* response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<!DOCTYPE HTML>"
            "<head><title>Http Server in Kaisen Channel</title></head>"
            "<body><h1>Hello C Programmers</h1><p>This is C_STD framework in C language</p>"
            "</body>"
            "</html>";

        size_t send_data;
        status = tcp_send(client, response, strlen(response), &send_data);

        if (status != TCP_SUCCESS || strlen(response) != send_data) {
            fmt_fprintf(stderr, "Error : Failed in Sending response\n");
            return -1;
        }
    }
    return 0;
}

int main() {
    TcpSocket server;
    TcpStatus status;

    // initialization network
    status = tcp_init();
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Error : Network initialization Failed\n");
        tcp_cleanup();
        return -1;
    }

    // socket Creation
    status = tcp_socket_create(&server);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Error : Socket Creation Failed\n");
        tcp_cleanup();
        return -1;
    }

    // binding operation 
    status = tcp_bind(server, "0.0.0.0", SERVER_PORT);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Error : bind operation failed\n");
        tcp_close(server);
        tcp_cleanup();
        return -1;
    }

    // listening operation 
    status = tcp_listen(server, 5);
    if (status != TCP_SUCCESS) {
        fmt_fprintf(stderr, "Error : Listen operation Failed\n");
        tcp_close(server);
        tcp_cleanup();
        return -1;
    }

    fmt_printf("HTTP Server Open and Listen in \'localhost\' on Port %d", SERVER_PORT);

    while (true) {
        TcpSocket* client = (TcpSocket*) malloc(sizeof(TcpSocket));

        if (client == NULL) {
            fmt_fprintf(stderr, "Error : Failed in memory allocation for client");
            continue;
        }

        status = tcp_accept(server, client);
        if (status != TCP_SUCCESS) {
            fmt_fprintf(stderr, "Error : Acception Failed for Client request");
            free(client);
            continue;
        }

        handle_client_request(client);

        tcp_close(*client);
        free(client);
    }

    tcp_close(server);
    tcp_cleanup();

    return EXIT_SUCCESS;
}