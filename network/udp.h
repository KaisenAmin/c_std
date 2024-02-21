#ifndef UDP_H_
#define UDP_H_

#include <stddef.h>
#include <stdbool.h>

// Define platform-specific includes and types
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
    typedef int UdpSocket;
#endif

typedef enum {
    UDP_SUCCESS = 0,       // No error
    UDP_ERR_SOCKET,        // Socket creation failed
    UDP_ERR_BIND,          // Binding socket failed
    UDP_ERR_SEND,          // Sending data failed
    UDP_ERR_RECV,          // Receiving data failed
    UDP_ERR_CLOSE,         // Closing socket failed
    UDP_ERR_SETUP,         // Initial setup failed (e.g., WSAStartup on Windows)
    UDP_ERR_RESOLVE,       // Resolving hostname failed
    UDP_ERR_GENERIC,       // Generic error
    UDP_ERR_NO_DATA,       // No data available for non-blocking receive
    UDP_ERR_BUFFER_TOO_SMALL, // Provided buffer is too small for received data
    UDP_ERR_UNSUPPORTED,   // Unsupported feature
} UdpStatus;

typedef struct {
    UdpStatus code;    // Basic error code
    int sys_errno;     // System-specific error code (e.g., errno on Unix, WSAGetLastError() on Windows)
    char message[256]; // Human-readable error message
} UdpStatusInfo;

// UDP socket initialization and cleanup
UdpStatus udp_init(void);
UdpStatus udp_cleanup(void);

// Socket creation and management
UdpStatus udp_socket_create(UdpSocket* socket);
UdpStatus udp_bind(UdpSocket socket, const char* host, unsigned short port);
UdpStatus udp_close(UdpSocket socket);

// Data transmission and reception
UdpStatus udp_sendto(UdpSocket socket, const void* buf, size_t len, size_t* sent, const char* dest_host, unsigned short dest_port);
UdpStatus udp_recvfrom(UdpSocket socket, void* buf, size_t len, size_t* received, char* src_host, size_t src_host_len, unsigned short* src_port);

// Utility functions
UdpStatus udp_set_non_blocking(UdpSocket socket, bool enable);
UdpStatus udp_set_broadcast(UdpSocket socket, bool enable);
UdpStatus udp_set_timeout(UdpSocket socket, long timeout_ms);
UdpStatus udp_resolve_hostname(const char* hostname, char* ip_address, size_t ip_address_len);
bool udp_is_valid_address(const char* address);

// Error handling
void udp_get_last_error(UdpStatusInfo* status_info);

#endif 
