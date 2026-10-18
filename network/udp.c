/* POSIX / GNU declarations (strdup, clock_gettime, localtime_r, ...) must be
   requested before the first #include, so that strict -std=c17 builds
   (CMAKE_C_EXTENSIONS=OFF) see them as well. */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "udp.h"
#include "../concurrent/concurrent.h"
#include <string.h>
#include <stdio.h>

#include <limits.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64)
#include <iphlpapi.h>     /* if_nametoindex() (Vista+) */
#endif

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;   /* MinGW and POSIX provide ssize_t; MSVC does not */
#endif

#if !defined(_WIN32) && !defined(_WIN64)
#include <errno.h>
#include <poll.h>         /* poll() — udp_wait_readable/writable        */
#include <sys/ioctl.h>    /* ioctl(), FIONREAD — udp_bytes_available    */
#endif

#if defined(_WIN32) || defined(_WIN64)
    #define UDP_INVALID_SOCKET ((UdpSocket)(~0))
#else
    #define UDP_INVALID_SOCKET (-1)
#endif

// Thread-local last error. Use C11's `_Thread_local` so the storage-class
// keyword goes BEFORE `static` — placing the MSVC/GCC vendor-specific
static _Thread_local UdpStatusInfo udp_last_error;

// Mutex for any global state (mainly for WSAStartup/cleanup refcount).
// Initialized exactly once by udp_lock(): udp_cleanup before udp_init used
// to lock an uninitialized mutex (crash on Windows), and the old plain
// "initialized" flag raced when two threads called udp_init at once.
static Mutex    udp_mutex;
static OnceFlag udp_mutex_once = CSTD_ONCE_FLAG_INIT;
static void udp_mutex_init_once(void) {
    mutex_init(&udp_mutex, MUTEX_PLAIN);
}
static void udp_lock(void) {
    concurrent_call_once(&udp_mutex_once, udp_mutex_init_once);
    mutex_lock(&udp_mutex);
}
#if defined(_WIN32) || defined(_WIN64)
static int udp_wsa_refcount = 0;   /* Windows-only: WSAStartup/WSACleanup balance */
#endif

/* Receives one datagram. Returns the byte count stored in `buf` (>= 0) or -1
   on error; *truncated is set when the datagram was larger than `len` (its
   first `len` bytes are in `buf`). Windows reports that as WSAEMSGSIZE (it
   used to become a plain receive error with 0 bytes) and POSIX recvfrom
   truncated silently; recvmsg's MSG_TRUNC flag tells us there. */
static long udp_recv_dgram(UdpSocket socket, void* buf, size_t len,
                           struct sockaddr_storage* addr, socklen_t* addrlen, bool* truncated) {
    *truncated = false;
#if defined(_WIN32) || defined(_WIN64)
    int n = recvfrom(socket, (char*)buf, (int)len, 0, (struct sockaddr*)addr, addrlen);
    if (n == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAEMSGSIZE) {
            *truncated = true;
            return (long)len;
        }
        return -1;
    }
    return (long)n;
#else
    struct iovec iov;
    struct msghdr msg;
    memset(&msg, 0, sizeof msg);
    iov.iov_base = buf;
    iov.iov_len  = len;
    msg.msg_name    = addr;
    msg.msg_namelen = *addrlen;
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;
    ssize_t n;
    do {
        n = recvmsg(socket, &msg, 0);
    } while (n < 0 && errno == EINTR);
    if (n < 0) {
        return -1;
    }
    *addrlen = msg.msg_namelen;
    if (msg.msg_flags & MSG_TRUNC) {
        *truncated = true;
    }
    return (long)n;
#endif
}

/* IPv6 multicast interface: a name ("eth0"; on Windows the interface alias)
   or a numeric index ("12"); empty/NULL = let the OS choose. Returns false
   for an unknown interface - it used to be ignored on Windows and turned
   into index 0 (the default interface) on POSIX, so the join went to the
   wrong interface without an error. */
static bool udp_iface_index(const char* iface, unsigned int* out) {
    *out = 0;
    if (!iface || !*iface) {
        return true;
    }
    char* end = NULL;
    unsigned long v = strtoul(iface, &end, 10);
    if (end != iface && *end == '\0') {
        if (v > UINT_MAX) {
            return false;
        }
        *out = (unsigned int)v;
        return true;
    }
    unsigned int idx = if_nametoindex(iface);
    if (idx == 0) {
        return false;
    }
    *out = idx;
    return true;
}

// Internal: set last error
static void udp_set_last_error(UdpStatus code, int sys_errno, const char* msg) {
    udp_last_error.code = code;
    udp_last_error.sys_errno = sys_errno;
    if (msg) {
        strncpy(udp_last_error.message, msg, sizeof(udp_last_error.message)-1);
        udp_last_error.message[sizeof(udp_last_error.message)-1] = '\0';
    } else {
        udp_last_error.message[0] = '\0';
    }
}

// Internal: set last error from system
static void udp_set_last_error_sys(UdpStatus code) {
#if defined(_WIN32) || defined(_WIN64)
    int err = WSAGetLastError();
    char buf[256] = {0};
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, 0, buf, sizeof(buf), NULL);
    udp_set_last_error(code, err, buf);
#else
    int err = errno;
    udp_set_last_error(code, err, strerror(err));
#endif
}


/**
 * @brief Retrieve the most recent UDP error for the calling thread.
 *
 * Errors are stored per-thread (using `__thread` / `__declspec(thread)`), so
 * each thread sees its own "last error" independent of other threads' I/O.
 * Copies the cached `UdpStatusInfo` into the caller's struct.
 *
 * @param status_info Output: filled with the last error info. NULL is a no-op.
 */
void udp_get_last_error(UdpStatusInfo* status_info) {
    if (status_info) {
        *status_info = udp_last_error;
    }
    UDP_LOG("Get last error: %s", udp_last_error.message);
}


/**
 * @brief Check whether `address` is a valid IPv4 or IPv6 numeric address.
 *
 * Does NOT perform DNS resolution — only parses dotted/colon-notation forms.
 *
 * @param address Null-terminated address string (e.g. "192.168.1.1" or "::1").
 * @return `true` if the address parses as either IPv4 or IPv6, `false` otherwise.
 */
bool udp_is_valid_address(const char* address) {
    struct sockaddr_in sa;
    struct sockaddr_in6 sa6;

    if (!address) {
        // glibc's inet_pton() does strlen(src) and would crash on NULL.
        return false;
    }
    if (inet_pton(AF_INET, address, &(sa.sin_addr)) == 1) {
        return true;
    }
    if (inet_pton(AF_INET6, address, &(sa6.sin6_addr)) == 1) {
        return true;
    }

    return false;
}


// Internal: resolve hostname to sockaddr_storage (for sendto, bind, etc)
static int udp_resolve_sockaddr(const char* host, unsigned short port, struct sockaddr_storage* out_addr, socklen_t* out_len, int family, int socktype, int flags) {
    struct addrinfo hints, *res = NULL;
    char portstr[16];

    snprintf(portstr, sizeof(portstr), "%u", port);
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = family;
    hints.ai_socktype = socktype;
    hints.ai_flags = flags;

    int status = getaddrinfo(host, portstr, &hints, &res);
    if (status != 0) {
        return -1;
    }

    memcpy(out_addr, res->ai_addr, res->ai_addrlen);
    *out_len = (socklen_t)res->ai_addrlen;
    freeaddrinfo(res);

    return 0;
}


/**
 * @brief Initialize the UDP subsystem.
 *
 * On Windows this performs a reference-counted `WSAStartup`; on POSIX it's
 * a no-op aside from the internal mutex setup. Must be called before any
 * other UDP function; safe to call multiple times (each must be paired with
 * a matching `udp_cleanup`).
 *
 * @return `UDP_SUCCESS`, or `UDP_ERR_SETUP` if `WSAStartup` fails on Windows.
 */
UdpStatus udp_init(void) {
    UDP_LOG("udp_init() called");
    udp_lock();

#if defined(_WIN32) || defined(_WIN64)
    if (udp_wsa_refcount == 0) {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2,2), &wsaData);

        if (result != 0) {
            mutex_unlock(&udp_mutex);
            udp_set_last_error(UDP_ERR_SETUP, result, "WSAStartup failed");
            UDP_LOG("WSAStartup failed: %d", result);

            return UDP_ERR_SETUP;
        }
        UDP_LOG("WSAStartup succeeded");
    }
    udp_wsa_refcount++;
#endif
    mutex_unlock(&udp_mutex);
    UDP_LOG("udp_init complete");

    return UDP_SUCCESS;
}


/**
 * @brief Tear down one reference to the UDP subsystem.
 *
 * Pairs with `udp_init`; the final unmatched cleanup call invokes `WSACleanup`
 * on Windows. POSIX has nothing to release.
 *
 * @return `UDP_SUCCESS`.
 */
UdpStatus udp_cleanup(void) {
    UDP_LOG("udp_cleanup() called");
    udp_lock();

#if defined(_WIN32) || defined(_WIN64)
    if (udp_wsa_refcount > 0) {
        udp_wsa_refcount--;

        if (udp_wsa_refcount == 0) {
            WSACleanup();
            UDP_LOG("WSACleanup called");
        }
    }
#endif
    mutex_unlock(&udp_mutex);
    UDP_LOG("udp_cleanup complete");
    return UDP_SUCCESS;
}


/**
 * @brief Create a UDP socket configured for dual-stack IPv4/IPv6 operation.
 *
 * Returns an `AF_INET6` socket with `IPV6_V6ONLY` disabled so that both
 * IPv6 and IPv4-mapped-IPv6 destinations work through the same handle.
 *
 * @param sock Output: receives the new socket on success.
 * @return `UDP_SUCCESS`, or `UDP_ERR_SOCKET` if the OS call fails or `sock`
 *         is NULL.
 */
UdpStatus udp_socket_create(UdpSocket* sock) {
    UDP_LOG("udp_socket_create(%p)", (void*)sock);
    if (!sock) {
        udp_set_last_error(UDP_ERR_SOCKET, 0, "Null socket pointer");
        UDP_LOG("Socket create failed: null pointer");

        return UDP_ERR_SOCKET;
    }

#if defined(_WIN32) || defined(_WIN64)
    /* WSA_FLAG_OVERLAPPED: without it Winsock ignores SO_RCVTIMEO/SO_SNDTIMEO,
       so udp_set_timeout would succeed and blocking recvfrom never time out. */
    *sock = WSASocket(AF_INET6, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (*sock == UDP_INVALID_SOCKET) {
        udp_set_last_error_sys(UDP_ERR_SOCKET);
        UDP_LOG("Socket create failed: %d", WSAGetLastError());

        return UDP_ERR_SOCKET;
    }
    DWORD off = 0;
    setsockopt(*sock, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&off, sizeof(off));
#else
    *sock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (*sock < 0) {
        udp_set_last_error_sys(UDP_ERR_SOCKET);
        UDP_LOG("Socket create failed: %d", errno);

        return UDP_ERR_SOCKET;
    }
    int off = 0;
    setsockopt(*sock, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off));
#endif
    UDP_LOG("Socket created: %d", (int)*sock);
    return UDP_SUCCESS;
}


/**
 * @brief Bind a UDP socket to a local address and port.
 *
 * Pass `host = NULL` or an empty string to bind to the wildcard address
 * (accept datagrams on any local interface). `port = 0` lets the OS pick
 * an ephemeral port.
 *
 * @param socket Socket from `udp_socket_create`.
 * @param host   Local interface address, or NULL for wildcard.
 * @param port   Local port to bind to.
 * @return `UDP_SUCCESS`, `UDP_ERR_RESOLVE` if the address can't be parsed,
 *         or `UDP_ERR_BIND` if the OS bind fails (e.g., port in use).
 */
UdpStatus udp_bind(UdpSocket socket, const char* host, unsigned short port) {
    UDP_LOG("udp_bind(socket=%d, host=%s, port=%u)", (int)socket, host ? host : "NULL", port);
    
    struct sockaddr_storage addr;
    socklen_t addrlen;
    /* Sockets are created dual-stack AF_INET6 (see udp_socket_create), so the
       bind address must resolve to an IPv6 sockaddr. Resolving with AF_UNSPEC
       can yield an IPv4 wildcard (0.0.0.0) first, which fails to bind to an
       AF_INET6 socket. AI_V4MAPPED additionally lets IPv4 literals bind via
       their ::ffff:x.y.z.w form; a NULL host resolves to the :: wildcard. */
    int family = AF_INET6;

    if (!host || !*host) {
        host = NULL;
    }
    if (udp_resolve_sockaddr(host, port, &addr, &addrlen, family, SOCK_DGRAM, AI_PASSIVE | AI_V4MAPPED) != 0) {
        udp_set_last_error(UDP_ERR_RESOLVE, 0, "Failed to resolve bind address");
        UDP_LOG("Bind failed: resolve error for host %s port %u", host ? host : "NULL", port);
        
        return UDP_ERR_RESOLVE;
    }
    if (bind(socket, (struct sockaddr*)&addr, addrlen) != 0) {
        udp_set_last_error_sys(UDP_ERR_BIND);
        UDP_LOG("Bind failed: %d", errno);

        return UDP_ERR_BIND;
    }
    UDP_LOG("Bind to port %u success", port);
    return UDP_SUCCESS;
}


/**
 * @brief Close a UDP socket.
 *
 * @param socket Socket to release.
 * @return `UDP_SUCCESS` or `UDP_ERR_CLOSE`.
 */
UdpStatus udp_close(UdpSocket socket) {
#if defined(_WIN32) || defined(_WIN64)
    if (closesocket(socket) == SOCKET_ERROR) {
        udp_set_last_error_sys(UDP_ERR_CLOSE);
        return UDP_ERR_CLOSE;
    }
#else
    if (close(socket) != 0) {
        udp_set_last_error_sys(UDP_ERR_CLOSE);
        return UDP_ERR_CLOSE;
    }
#endif
    return UDP_SUCCESS;
}


/**
 * @brief Send a single UDP datagram to a destination host:port.
 *
 * Resolves `dest_host` (numeric or DNS) and sends the entire buffer as one
 * datagram. Because the underlying socket is dual-stack IPv6, IPv4
 * destinations are routed through their IPv4-mapped IPv6 form
 * (`::ffff:x.y.z.w`) transparently.
 *
 * @param socket    Socket previously created by `udp_socket_create`.
 * @param buf       Bytes to send. Must not be NULL.
 * @param len       Number of bytes in `buf`. Must be > 0.
 * @param sent      Optional out-param filled with the actual byte count.
 * @param dest_host Destination address or hostname. Must not be NULL.
 * @param dest_port Destination port.
 * @return `UDP_SUCCESS`, `UDP_ERR_SEND` for invalid args or send failure,
 *         or `UDP_ERR_RESOLVE` if the host can't be resolved.
 */
UdpStatus udp_sendto(UdpSocket socket, const void* buf, size_t len, size_t* sent, const char* dest_host, unsigned short dest_port) {
    UDP_LOG("udp_sendto(socket=%d, buf=%p, len=%zu, dest_host=%s, dest_port=%u)", (int)socket, buf, len, dest_host, dest_port);
    
    if (!buf || len == 0 || !dest_host) {
        udp_set_last_error(UDP_ERR_SEND, 0, "Invalid sendto parameters");
        UDP_LOG("Sendto failed: invalid parameters");

        return UDP_ERR_SEND;
    }
    struct sockaddr_storage addr;
    socklen_t addrlen;

    int resolve_status = udp_resolve_sockaddr(dest_host, dest_port, &addr, &addrlen, AF_INET6, SOCK_DGRAM, AI_V4MAPPED | AI_ALL);
    if (resolve_status != 0) {
        resolve_status = udp_resolve_sockaddr(dest_host, dest_port, &addr, &addrlen, AF_UNSPEC, SOCK_DGRAM, 0);
    }
    if (resolve_status != 0) {
        udp_set_last_error(UDP_ERR_RESOLVE, 0, "Failed to resolve destination address");
        UDP_LOG("Sendto failed: resolve error for %s:%u", dest_host, dest_port);

        return UDP_ERR_RESOLVE;
    }

#if defined(_WIN32) || defined(_WIN64)
    /* Winsock takes an int length: refuse what does not fit (no datagram is
       that large; the (int) conversion would have sent a wrong length). */
    if (len > (size_t)INT_MAX) {
        udp_set_last_error(UDP_ERR_SEND, 0, "Datagram too large");
        return UDP_ERR_SEND;
    }
    int n = sendto(socket, (const char*)buf, (int)len, 0, (struct sockaddr*)&addr, (int)addrlen);
#else
    ssize_t n = sendto(socket, buf, len, 0, (struct sockaddr*)&addr, addrlen);
#endif
    if (n < 0) {
        udp_set_last_error_sys(UDP_ERR_SEND);
        UDP_LOG("Sendto failed: %d", errno);
        
        if (sent) { 
            *sent = 0;
        }

        return UDP_ERR_SEND;
    }
    if (sent) {
        *sent = (size_t)n;
    }

    UDP_LOG("Sendto %s:%u %zu bytes", dest_host, dest_port, len);
    return UDP_SUCCESS;
}


/**
 * @brief Receive a single UDP datagram, optionally capturing the source.
 *
 * Reads up to `len` bytes into `buf`. If `src_host` is non-NULL and
 * `src_host_len` is non-zero, the sender's address is written there as a
 * presentation string (IPv4 dotted, IPv6 colon-separated, possibly in
 * IPv4-mapped form like `::ffff:127.0.0.1`).
 *
 * @param socket       Socket previously created (and usually bound).
 * @param buf          Destination buffer for the datagram.
 * @param len          Size of `buf`.
 * @param received     Optional out-param: actual bytes written. Set to 0 on failure.
 * @param src_host     Optional out-param: sender's address as string.
 * @param src_host_len Size of `src_host`.
 * @param src_port     Optional out-param: sender's port.
 * @return `UDP_SUCCESS` on a complete datagram, `UDP_ERR_NO_DATA` if the
 *         socket is non-blocking and no data is available, otherwise an
 *         `UDP_ERR_*` value.
 */
UdpStatus udp_recvfrom(UdpSocket socket, void* buf, size_t len, size_t* received, char* src_host, size_t src_host_len, unsigned short* src_port) {
    UDP_LOG("udp_recvfrom(socket=%d, buf=%p, len=%zu, src_host=%p, src_host_len=%zu, src_port=%p)", (int)socket, buf, len, src_host, src_host_len, src_port);

    if (!buf || len == 0) {
        udp_set_last_error(UDP_ERR_RECV, 0, "Invalid recvfrom parameters");
        UDP_LOG("Recvfrom failed: invalid parameters");
        return UDP_ERR_RECV;
    }

    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);
    bool truncated = false;
    long n = udp_recv_dgram(socket, buf, len, &addr, &addrlen, &truncated);

    if (n < 0) {
#if defined(_WIN32) || defined(_WIN64)
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
            udp_set_last_error(UDP_ERR_NO_DATA, err, "No data available (would block)");
            UDP_LOG("Recvfrom would block");
            
            if (received) {
                *received = 0;
            }

            return UDP_ERR_NO_DATA;
        }
#else
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            udp_set_last_error(UDP_ERR_NO_DATA, errno, "No data available (would block)");
            UDP_LOG("Recvfrom would block");

            if (received) {
                *received = 0;
            }

            return UDP_ERR_NO_DATA;
        }
#endif
        udp_set_last_error_sys(UDP_ERR_RECV);
        UDP_LOG("Recvfrom failed: %d", errno);
        
        if (received) {
            *received = 0;
        }

        return UDP_ERR_RECV;
    }
    if (received) {
        *received = (size_t)n;
    }
  
    void* addrptr = NULL;
    if (addr.ss_family == AF_INET) {
        addrptr = &((struct sockaddr_in*)&addr)->sin_addr;
        if (src_port) {
            *src_port = ntohs(((struct sockaddr_in*)&addr)->sin_port);
        }
    } 
    else if (addr.ss_family == AF_INET6) {
        addrptr = &((struct sockaddr_in6*)&addr)->sin6_addr;
        if (src_port) {
            *src_port = ntohs(((struct sockaddr_in6*)&addr)->sin6_port);
        }
    }
    if (src_host && src_host_len > 0) {
        if (addrptr) {
            inet_ntop(addr.ss_family, addrptr, src_host, src_host_len);
        } 
        else {
            src_host[0] = '\0';
        }
    }

    UDP_LOG("Recvfrom %s:%u %zu bytes", src_host ? src_host : "?", src_port ? *src_port : 0, received ? *received : 0);
    if (truncated) {
        udp_set_last_error(UDP_ERR_BUFFER_TOO_SMALL, 0, "Datagram larger than the buffer (truncated)");
        return UDP_ERR_BUFFER_TOO_SMALL;
    }

    return UDP_SUCCESS;
}


/**
 * @brief Toggle non-blocking I/O on a UDP socket.
 *
 * When enabled, `udp_recvfrom` returns `UDP_ERR_NO_DATA` instead of
 * blocking when there's nothing to read; `udp_sendto` similarly returns
 * an error rather than blocking on a full kernel buffer.
 *
 * @param socket Socket to configure.
 * @param enable `true` for non-blocking, `false` for the default blocking mode.
 * @return `UDP_SUCCESS` or `UDP_ERR_GENERIC`.
 */
UdpStatus udp_set_non_blocking(UdpSocket socket, bool enable) {
#if defined(_WIN32) || defined(_WIN64)
    u_long mode = enable ? 1 : 0;

    if (ioctlsocket(socket, FIONBIO, &mode) != 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        UDP_LOG("Set non-blocking failed: %d", WSAGetLastError());

        return UDP_ERR_GENERIC;
    }
#else
    int flags = fcntl(socket, F_GETFL, 0);

    if (flags == -1) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        UDP_LOG("Set non-blocking failed: %d", errno);

        return UDP_ERR_GENERIC;
    }

    if (enable) {
        flags |= O_NONBLOCK; 
    }
    else {
        flags &= ~O_NONBLOCK;
    }

    if (fcntl(socket, F_SETFL, flags) == -1) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        UDP_LOG("Set non-blocking failed: %d", errno);

        return UDP_ERR_GENERIC;
    }
#endif
    UDP_LOG("Set non-blocking: %s", enable ? "enabled" : "disabled");
    return UDP_SUCCESS;
}


/**
 * @brief Enable or disable broadcast sends on a UDP socket.
 *
 * Required before sending to a broadcast address such as `255.255.255.255`.
 *
 * @param socket Socket to configure.
 * @param enable `true` to allow broadcast packets.
 * @return `UDP_SUCCESS` or `UDP_ERR_GENERIC`.
 */
UdpStatus udp_set_broadcast(UdpSocket socket, bool enable) {
    int opt = enable ? 1 : 0;

    if (setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(opt)) != 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        UDP_LOG("Set broadcast failed: %d", errno);

        return UDP_ERR_GENERIC;
    }

    UDP_LOG("Set broadcast: %s", enable ? "enabled" : "disabled");
    return UDP_SUCCESS;
}


/**
 * @brief Apply the same send/receive timeout (in milliseconds) to a UDP socket.
 *
 * Sets both `SO_RCVTIMEO` and `SO_SNDTIMEO`. After this, blocking calls on
 * the socket will return with an error if they wait longer than the
 * specified duration.
 *
 * @param socket     Socket to configure.
 * @param timeout_ms Timeout in milliseconds. Use 0 to remove the timeout.
 * @return `UDP_SUCCESS` or `UDP_ERR_GENERIC`.
 */
UdpStatus udp_set_timeout(UdpSocket socket, long timeout_ms) {
#if defined(_WIN32) || defined(_WIN64)
    DWORD timeout = (DWORD)timeout_ms;

    if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) != 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        UDP_LOG("Set timeout failed: %d", WSAGetLastError());

        return UDP_ERR_GENERIC;
    }
    if (setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)) != 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        UDP_LOG("Set timeout failed: %d", WSAGetLastError());

        return UDP_ERR_GENERIC;
    }
#else
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        UDP_LOG("Set timeout failed: %d", errno);

        return UDP_ERR_GENERIC;
    }
    if (setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) != 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        UDP_LOG("Set timeout failed: %d", errno);

        return UDP_ERR_GENERIC;
    }
#endif
    UDP_LOG("Set timeout: %ld ms", timeout_ms);
    return UDP_SUCCESS;
}


/**
 * @brief Resolve a hostname to its first IP address (IPv4 or IPv6).
 *
 * Uses `getaddrinfo` with `AF_UNSPEC` and writes the resulting address in
 * presentation form (e.g., `"127.0.0.1"` or `"::1"`) to `ip_address`.
 *
 * @param hostname        Name or numeric address to resolve.
 * @param ip_address      Output buffer for the resolved address.
 * @param ip_address_len  Size of `ip_address`.
 * @return `UDP_SUCCESS`, `UDP_ERR_GENERIC` for invalid args, or
 *         `UDP_ERR_RESOLVE` if the lookup fails.
 */
UdpStatus udp_resolve_hostname(const char* hostname, char* ip_address, size_t ip_address_len) {
    if (!hostname || !ip_address) {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "Null parameter");
        return UDP_ERR_GENERIC;
    }

    struct addrinfo hints, *res, *p;
    int status;
    void* addr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((status = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
        udp_set_last_error(UDP_ERR_RESOLVE, 0, gai_strerror(status));
        return UDP_ERR_RESOLVE;
    }
    for (p = res; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } 
        else {
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        if (inet_ntop(p->ai_family, addr, ip_address, ip_address_len) == NULL) {
            freeaddrinfo(res);
            udp_set_last_error(UDP_ERR_GENERIC, 0, "inet_ntop failed");

            return UDP_ERR_GENERIC;
        }
        break;
    }
    freeaddrinfo(res);
    return UDP_SUCCESS;
}


/**
 * @brief Join a multicast group (IPv4 or IPv6).
 * @param socket UDP socket
 * @param group_addr Multicast group address (e.g., "224.0.0.1" or "ff02::1")
 * @param iface_addr Local interface address (can be NULL for default)
 * @return UDP_SUCCESS or error
 */
UdpStatus udp_join_multicast_group(UdpSocket socket, const char* group_addr, const char* iface_addr) {
    UDP_LOG("Join multicast group %s on iface %s", group_addr, iface_addr ? iface_addr : "default");
    if (!group_addr) {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "Null group_addr");
        return UDP_ERR_GENERIC;
    }

    struct in_addr local4;
    struct ip_mreq mreq4;
    struct ipv6_mreq mreq6;

    if (strchr(group_addr, ':')) { // IPv6
#if defined(IPV6_JOIN_GROUP)
        if (inet_pton(AF_INET6, group_addr, &mreq6.ipv6mr_multiaddr) != 1) {
            udp_set_last_error(UDP_ERR_GENERIC, 0, "Invalid IPv6 multicast address");
            return UDP_ERR_GENERIC;
        }
        unsigned int ifindex = 0;
        if (!udp_iface_index(iface_addr, &ifindex)) {
            udp_set_last_error(UDP_ERR_GENERIC, 0, "Unknown IPv6 multicast interface (use a name or an index)");
            return UDP_ERR_GENERIC;
        }
        mreq6.ipv6mr_interface = ifindex;
        if (setsockopt(socket, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char*)&mreq6, sizeof(mreq6)) != 0) {
            udp_set_last_error_sys(UDP_ERR_GENERIC);
            return UDP_ERR_GENERIC;
        }
        return UDP_SUCCESS;
#else
        udp_set_last_error(UDP_ERR_UNSUPPORTED, 0, "IPv6 multicast not supported on this platform");
        return UDP_ERR_UNSUPPORTED;
#endif
    } 
    else { // IPv4
        if (inet_pton(AF_INET, group_addr, &mreq4.imr_multiaddr) != 1) {
            udp_set_last_error(UDP_ERR_GENERIC, 0, "Invalid IPv4 multicast address");
            return UDP_ERR_GENERIC;
        }
        if (iface_addr && *iface_addr) {
            if (inet_pton(AF_INET, iface_addr, &local4) != 1) {
                udp_set_last_error(UDP_ERR_GENERIC, 0, "Invalid IPv4 interface address");
                return UDP_ERR_GENERIC;
            }
            mreq4.imr_interface = local4;
        } 
        else {
            mreq4.imr_interface.s_addr = htonl(INADDR_ANY);
        }
        if (setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq4, sizeof(mreq4)) != 0) {
            udp_set_last_error_sys(UDP_ERR_GENERIC);
            return UDP_ERR_GENERIC;
        }
        return UDP_SUCCESS;
    }
}


/**
 * @brief Leave a multicast group (IPv4 or IPv6).
 * @param socket UDP socket
 * @param group_addr Multicast group address
 * @param iface_addr Local interface address (can be NULL for default)
 * @return UDP_SUCCESS or error
 */
UdpStatus udp_leave_multicast_group(UdpSocket socket, const char* group_addr, const char* iface_addr) {
    UDP_LOG("Leave multicast group %s on iface %s", group_addr, iface_addr ? iface_addr : "default");
    if (!group_addr) {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "Null group_addr");
        return UDP_ERR_GENERIC;
    }

    struct in_addr local4;
    struct ip_mreq mreq4;
    struct ipv6_mreq mreq6;

    if (strchr(group_addr, ':')) { // IPv6
#if defined(IPV6_LEAVE_GROUP)
        if (inet_pton(AF_INET6, group_addr, &mreq6.ipv6mr_multiaddr) != 1) {
            udp_set_last_error(UDP_ERR_GENERIC, 0, "Invalid IPv6 multicast address");
            return UDP_ERR_GENERIC;
        }
        unsigned int ifindex = 0;
        if (!udp_iface_index(iface_addr, &ifindex)) {
            udp_set_last_error(UDP_ERR_GENERIC, 0, "Unknown IPv6 multicast interface (use a name or an index)");
            return UDP_ERR_GENERIC;
        }
        mreq6.ipv6mr_interface = ifindex;
        if (setsockopt(socket, IPPROTO_IPV6, IPV6_LEAVE_GROUP, (char*)&mreq6, sizeof(mreq6)) != 0) {
            udp_set_last_error_sys(UDP_ERR_GENERIC);
            return UDP_ERR_GENERIC;
        }
        return UDP_SUCCESS;
#else
        udp_set_last_error(UDP_ERR_UNSUPPORTED, 0, "IPv6 multicast not supported on this platform");
        return UDP_ERR_UNSUPPORTED;
#endif
    } 
    else { // IPv4
        if (inet_pton(AF_INET, group_addr, &mreq4.imr_multiaddr) != 1) {
            udp_set_last_error(UDP_ERR_GENERIC, 0, "Invalid IPv4 multicast address");
            return UDP_ERR_GENERIC;
        }
        if (iface_addr && *iface_addr) {
            if (inet_pton(AF_INET, iface_addr, &local4) != 1) {
                udp_set_last_error(UDP_ERR_GENERIC, 0, "Invalid IPv4 interface address");
                return UDP_ERR_GENERIC;
            }
            mreq4.imr_interface = local4;
        } 
        else {
            mreq4.imr_interface.s_addr = htonl(INADDR_ANY);
        }
        if (setsockopt(socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&mreq4, sizeof(mreq4)) != 0) {
            udp_set_last_error_sys(UDP_ERR_GENERIC);
            return UDP_ERR_GENERIC;
        }
        return UDP_SUCCESS;
    }
}

/**
 * @brief Set multicast TTL (IPv4 or IPv6).
 * @param socket UDP socket
 * @param ttl Time-to-live value
 * @return UDP_SUCCESS or error
 */
UdpStatus udp_set_multicast_ttl(UdpSocket socket, int ttl) {
    UDP_LOG("Set multicast TTL: %d", ttl);
    if (ttl < 0 || ttl > 255) {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "Invalid TTL");
        return UDP_ERR_GENERIC;
    }
    // Try IPv4 first
    if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl)) == 0) {
        return UDP_SUCCESS;
    }
#if defined(IPPROTO_IPV6) && defined(IPV6_MULTICAST_HOPS)
    // Try IPv6
    if (setsockopt(socket, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char*)&ttl, sizeof(ttl)) == 0) {
        return UDP_SUCCESS;
    }
#endif
    udp_set_last_error_sys(UDP_ERR_GENERIC);
    return UDP_ERR_GENERIC;
}


/**
 * @brief Enable or disable address reuse (`SO_REUSEADDR`) on a UDP socket.
 *
 * Lets you re-bind to the same port immediately after a previous bind is
 * released (avoids the "Address already in use" hiccup during quick
 * restarts) and is required for joining multicast groups on multiple
 * sockets bound to the same port. On POSIX where it's defined, also
 * sets `SO_REUSEPORT` so multiple processes on different sockets can
 * share the same port for load balancing.
 *
 * @param socket Valid UDP socket.
 * @param enable `true` to enable reuse, `false` to disable.
 * @return `UDP_SUCCESS`, `UDP_ERR_GENERIC` on socket error.
 */
UdpStatus udp_set_reuse_addr(UdpSocket socket, bool enable) {
    int opt = enable ? 1 : 0;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) != 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        return UDP_ERR_GENERIC;
    }
#if !defined(_WIN32) && !defined(_WIN64) && defined(SO_REUSEPORT)
    setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, (const char*)&opt, sizeof(opt));
#endif
    udp_set_last_error(UDP_SUCCESS, 0, NULL);
    return UDP_SUCCESS;
}


/**
 * @brief Set the kernel send and/or receive buffer sizes on a UDP socket.
 *
 * Tunes `SO_SNDBUF` and `SO_RCVBUF`. Pass `0` for either argument to
 * leave that direction untouched, so you can adjust just one with
 * `udp_set_buffer_size(s, 0, 1 << 20)`. Note that the kernel typically
 * doubles the value internally and may clamp to a system maximum —
 * use `udp_get_local_address`-style introspection (here, `getsockopt`)
 * if you need to confirm the final size.
 *
 * @param socket    Valid UDP socket.
 * @param send_bytes Requested `SO_SNDBUF` in bytes, or 0 to skip.
 * @param recv_bytes Requested `SO_RCVBUF` in bytes, or 0 to skip.
 * @return `UDP_SUCCESS`, `UDP_ERR_GENERIC` on either setsockopt failure.
 */
UdpStatus udp_set_buffer_size(UdpSocket socket, size_t send_bytes, size_t recv_bytes) {
    if (send_bytes > 0) {
        int v = (int)send_bytes;

        if (setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (const char*)&v, sizeof(v)) != 0) {
            udp_set_last_error_sys(UDP_ERR_GENERIC);
            return UDP_ERR_GENERIC;
        }
    }
    if (recv_bytes > 0) {
        int v = (int)recv_bytes;
        if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (const char*)&v, sizeof(v)) != 0) {
            udp_set_last_error_sys(UDP_ERR_GENERIC);
            return UDP_ERR_GENERIC;
        }
    }

    udp_set_last_error(UDP_SUCCESS, 0, NULL);
    return UDP_SUCCESS;
}


/**
 * @brief Retrieve the local address a UDP socket is bound to.
 *
 * Wraps `getsockname()` and converts the result to a printable host
 * string (`inet_ntop`) plus the bound port. Particularly useful when
 * you've bound to port `0` (let the kernel pick an ephemeral port) and
 * need to learn which one you got, or when binding to "0.0.0.0" /
 * "::" and want the actual concrete IP.
 *
 * @param socket   Bound UDP socket.
 * @param host     Buffer to receive the address string (e.g. 64 bytes
 *                 is plenty for IPv6).
 * @param host_len Capacity of `host` in bytes.
 * @param port     Output: bound port in host byte order. May be NULL.
 * @return `UDP_SUCCESS`, `UDP_ERR_GENERIC` on getsockname / formatting failure.
 */
UdpStatus udp_get_local_address(UdpSocket socket, char* host, size_t host_len, unsigned short* port) {
    if (!host || host_len == 0) {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "Invalid arguments");
        return UDP_ERR_GENERIC;
    }

    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);

    if (getsockname(socket, (struct sockaddr*)&ss, &slen) != 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        return UDP_ERR_GENERIC;
    }
    if (ss.ss_family == AF_INET) {
        struct sockaddr_in* sin = (struct sockaddr_in*)&ss;

        if (!inet_ntop(AF_INET, &sin->sin_addr, host, (socklen_t)host_len)) {
            udp_set_last_error_sys(UDP_ERR_GENERIC);
            return UDP_ERR_GENERIC;
        }
        if (port) {
            *port = ntohs(sin->sin_port);
        }
    } 
    else if (ss.ss_family == AF_INET6) {
        struct sockaddr_in6* sin6 = (struct sockaddr_in6*)&ss;
        if (!inet_ntop(AF_INET6, &sin6->sin6_addr, host, (socklen_t)host_len)) {
            udp_set_last_error_sys(UDP_ERR_GENERIC);
            return UDP_ERR_GENERIC;
        }
        if (port) {
            *port = ntohs(sin6->sin6_port);
        }
    } 
    else {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "Unsupported address family");
        return UDP_ERR_GENERIC;
    }
    udp_set_last_error(UDP_SUCCESS, 0, NULL);
    return UDP_SUCCESS;
}


/**
 * @brief Pin a UDP socket to a fixed remote peer.
 *
 * Calling `connect()` on a UDP socket doesn't open a connection (UDP is
 * connectionless) — instead it (a) sets the default destination so
 * subsequent `udp_send` / `udp_recv` no longer need an address, and
 * (b) filters incoming datagrams: only packets from the connected peer
 * will be delivered. Lets you write a UDP "client" with the same
 * send/recv ergonomics as TCP.
 *
 * @param socket Valid UDP socket.
 * @param host   Remote host (numeric or DNS name).
 * @param port   Remote port.
 * @return `UDP_SUCCESS`, `UDP_ERR_RESOLVE` on hostname lookup failure,
 *         `UDP_ERR_GENERIC` on connect() failure.
 */
UdpStatus udp_connect(UdpSocket socket, const char* host, unsigned short port) {
    if (!host) {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "Host is NULL");
        return UDP_ERR_GENERIC;
    }

    struct sockaddr_storage addr;
    socklen_t addr_len = 0;

    /* Resolve the way udp_sendto does: the library's sockets are dual-stack
       AF_INET6, so an IPv4 peer must be given as a v4-mapped address (a plain
       sockaddr_in made connect() fail). The AF_UNSPEC retry covers sockets
       the caller created as AF_INET. */
    int rs = udp_resolve_sockaddr(host, port, &addr, &addr_len, AF_INET6, SOCK_DGRAM, AI_V4MAPPED | AI_ALL);
    if (rs != 0) {
        rs = udp_resolve_sockaddr(host, port, &addr, &addr_len, AF_UNSPEC, SOCK_DGRAM, 0);
    }
    if (rs != 0) {
        udp_set_last_error(UDP_ERR_RESOLVE, 0, "Failed to resolve peer");
        return UDP_ERR_RESOLVE;
    }
    if (connect(socket, (struct sockaddr*)&addr, addr_len) != 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        return UDP_ERR_GENERIC;
    }
    udp_set_last_error(UDP_SUCCESS, 0, NULL);

    return UDP_SUCCESS;
}


/**
 * @brief Send on a UDP socket previously pinned with `udp_connect`.
 *
 * Equivalent to `send()` — the kernel knows the destination thanks to
 * the prior `connect()`, so no address arguments are required. If the
 * socket has not been connected, the OS will usually return an error
 * (ENOTCONN / WSAENOTCONN) which surfaces as `UDP_ERR_SEND`.
 *
 * @param socket Connected UDP socket.
 * @param buf    Bytes to send.
 * @param len    Number of bytes in `buf`.
 * @param sent   Output: actual bytes sent. May be NULL.
 * @return `UDP_SUCCESS`, `UDP_ERR_SEND` on send failure.
 */
UdpStatus udp_send(UdpSocket socket, const void* buf, size_t len, size_t* sent) {
    if (!buf) {
        udp_set_last_error(UDP_ERR_SEND, 0, "buf is NULL");
        return UDP_ERR_SEND;
    }

    int n = send(socket, (const char*)buf, (int)len, 0);
    if (n < 0) {
        udp_set_last_error_sys(UDP_ERR_SEND);
        return UDP_ERR_SEND;
    }
    if (sent) {
        *sent = (size_t)n;
    }

    udp_set_last_error(UDP_SUCCESS, 0, NULL);
    return UDP_SUCCESS;
}


/**
 * @brief Receive on a UDP socket previously pinned with `udp_connect`.
 *
 * Equivalent to `recv()` — only datagrams from the connected peer are
 * delivered, and there's no source-address output (use the unpinned
 * `udp_recvfrom` if you need it). On a non-blocking socket with no
 * data available, returns `UDP_ERR_NO_DATA`.
 *
 * @param socket   Connected UDP socket.
 * @param buf      Destination buffer.
 * @param len      Capacity of `buf` in bytes.
 * @param received Output: actual bytes received. May be NULL.
 * @return `UDP_SUCCESS`, `UDP_ERR_NO_DATA` (would-block on non-blocking
 *         socket), `UDP_ERR_RECV` on recv failure.
 */
UdpStatus udp_recv(UdpSocket socket, void* buf, size_t len, size_t* received) {
    if (!buf || len == 0) {
        udp_set_last_error(UDP_ERR_RECV, 0, "Invalid buffer");
        return UDP_ERR_RECV;
    }

    int n = recv(socket, (char*)buf, (int)len, 0);
    if (n < 0) {
#if defined(_WIN32) || defined(_WIN64)
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
            udp_set_last_error(UDP_ERR_NO_DATA, err, "No data available");
            return UDP_ERR_NO_DATA;
        }
#else
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            udp_set_last_error(UDP_ERR_NO_DATA, errno, "No data available");
            return UDP_ERR_NO_DATA;
        }
#endif
        udp_set_last_error_sys(UDP_ERR_RECV);
        return UDP_ERR_RECV;
    }
    if (received) {
        *received = (size_t)n;
    }
    udp_set_last_error(UDP_SUCCESS, 0, NULL);
    
    return UDP_SUCCESS;
}


/**
 * @brief Human-readable label for a `UdpStatus` code.
 *
 * Returns a static, immutable string suitable for printing or logging
 * (`"UDP_SUCCESS"`, `"UDP_ERR_BIND"`, …). Useful when a caller wants
 * structured logging without dragging in `udp_get_last_error`.
 * Unknown codes return `"UDP_UNKNOWN_CODE"`.
 *
 * @param code Status code returned by any `udp_*` function.
 * @return Static string. Never NULL.
 */
const char* udp_status_to_string(UdpStatus code) {
    switch (code) {
        case UDP_SUCCESS:              
            return "UDP_SUCCESS";
        case UDP_ERR_SOCKET:           
            return "UDP_ERR_SOCKET";
        case UDP_ERR_BIND:             
            return "UDP_ERR_BIND";
        case UDP_ERR_SEND:             
            return "UDP_ERR_SEND";
        case UDP_ERR_RECV:             
            return "UDP_ERR_RECV";
        case UDP_ERR_CLOSE:            
            return "UDP_ERR_CLOSE";
        case UDP_ERR_SETUP:            
            return "UDP_ERR_SETUP";
        case UDP_ERR_RESOLVE:          
            return "UDP_ERR_RESOLVE";
        case UDP_ERR_GENERIC:          
            return "UDP_ERR_GENERIC";
        case UDP_ERR_NO_DATA:          
            return "UDP_ERR_NO_DATA";
        case UDP_ERR_BUFFER_TOO_SMALL: 
            return "UDP_ERR_BUFFER_TOO_SMALL";
        case UDP_ERR_UNSUPPORTED:
            return "UDP_ERR_UNSUPPORTED";
        default:
            return "UDP_UNKNOWN_CODE";
    }
}


/**
 * @brief Get the remote peer address of a connected UDP socket.
 *
 * The counterpart of `udp_get_local_address`: it reports the peer set by a
 * prior `udp_connect` via `getpeername`. The address is written in
 * presentation form (e.g. `"127.0.0.1"` or `"::1"`).
 *
 * @param socket   A socket that has been connected with `udp_connect`.
 * @param host     Buffer receiving the peer address. Must be non-NULL with
 *                 `host_len > 0` (use `INET6_ADDRSTRLEN` to be safe).
 * @param host_len Size of @p host in bytes.
 * @param port     Optional; receives the peer port in host byte order.
 * @return `UDP_SUCCESS`, or `UDP_ERR_GENERIC` on invalid arguments, an
 *         unconnected socket (`getpeername` fails with `ENOTCONN`), or an
 *         unsupported address family.
 */
UdpStatus udp_get_peer_address(UdpSocket socket, char* host, size_t host_len, unsigned short* port) {
    if (!host || host_len == 0) {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "Invalid arguments");
        return UDP_ERR_GENERIC;
    }

    struct sockaddr_storage ss;
    socklen_t slen = sizeof(ss);

    if (getpeername(socket, (struct sockaddr*)&ss, &slen) != 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        return UDP_ERR_GENERIC;
    }
    if (ss.ss_family == AF_INET) {
        struct sockaddr_in* sin = (struct sockaddr_in*)&ss;
        if (!inet_ntop(AF_INET, &sin->sin_addr, host, (socklen_t)host_len)) {
            udp_set_last_error_sys(UDP_ERR_GENERIC);
            return UDP_ERR_GENERIC;
        }
        if (port) {
            *port = ntohs(sin->sin_port);
        }
    }
    else if (ss.ss_family == AF_INET6) {
        struct sockaddr_in6* sin6 = (struct sockaddr_in6*)&ss;
        if (!inet_ntop(AF_INET6, &sin6->sin6_addr, host, (socklen_t)host_len)) {
            udp_set_last_error_sys(UDP_ERR_GENERIC);
            return UDP_ERR_GENERIC;
        }
        if (port) {
            *port = ntohs(sin6->sin6_port);
        }
    }
    else {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "Unsupported address family");
        return UDP_ERR_GENERIC;
    }
    udp_set_last_error(UDP_SUCCESS, 0, NULL);
    return UDP_SUCCESS;
}


/**
 * @brief Set the unicast hop limit (TTL) for outgoing datagrams.
 *
 * The unicast counterpart of `udp_set_multicast_ttl`. Because the library's
 * sockets are dual-stack IPv6, this sets both the IPv4 `IP_TTL` and the IPv6
 * `IPV6_UNICAST_HOPS` options and succeeds if either applies, so it works for
 * IPv4-mapped and native IPv6 destinations alike.
 *
 * @param socket Valid UDP socket.
 * @param ttl    Hop limit in the range 0–255.
 * @return `UDP_SUCCESS`, or `UDP_ERR_GENERIC` if @p ttl is out of range or the
 *         option could not be set.
 */
UdpStatus udp_set_ttl(UdpSocket socket, int ttl) {
    UDP_LOG("Set unicast TTL: %d", ttl);
    
    if (ttl < 0 || ttl > 255) {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "Invalid TTL");
        return UDP_ERR_GENERIC;
    }

    bool ok = false;
    if (setsockopt(socket, IPPROTO_IP, IP_TTL, (const char*)&ttl, sizeof(ttl)) == 0) {
        ok = true;
    }
#if defined(IPPROTO_IPV6) && defined(IPV6_UNICAST_HOPS)
    if (setsockopt(socket, IPPROTO_IPV6, IPV6_UNICAST_HOPS, (const char*)&ttl, sizeof(ttl)) == 0) {
        ok = true;
    }
#endif
    if (!ok) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        return UDP_ERR_GENERIC;
    }
    udp_set_last_error(UDP_SUCCESS, 0, NULL);

    return UDP_SUCCESS;
}


/**
 * @brief Enable or disable local loopback of outgoing multicast datagrams.
 *
 * Controls `IP_MULTICAST_LOOP` / `IPV6_MULTICAST_LOOP`. When enabled (the
 * default on most systems), a sender that has also joined the group receives
 * its own datagrams; disable it to avoid processing your own multicast. Sets
 * both the IPv4 and IPv6 options on the dual-stack socket.
 *
 * @param socket Valid UDP socket.
 * @param enable `true` to receive own multicast (default), `false` to suppress.
 * @return `UDP_SUCCESS`, or `UDP_ERR_GENERIC` if the option could not be set.
 */
UdpStatus udp_set_multicast_loopback(UdpSocket socket, bool enable) {
    UDP_LOG("Set multicast loopback: %s", enable ? "on" : "off");

    int value = enable ? 1 : 0;
    bool ok = false;
    if (setsockopt(socket, IPPROTO_IP, IP_MULTICAST_LOOP, (const char*)&value, sizeof(value)) == 0) {
        ok = true;
    }

#if defined(IPPROTO_IPV6) && defined(IPV6_MULTICAST_LOOP)
    if (setsockopt(socket, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, (const char*)&value, sizeof(value)) == 0) {
        ok = true;
    }
#endif
    if (!ok) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        return UDP_ERR_GENERIC;
    }
    udp_set_last_error(UDP_SUCCESS, 0, NULL);

    return UDP_SUCCESS;
}


/**
 * @brief Report how many bytes are immediately available to read.
 *
 * Queries the socket via `FIONREAD`. On a datagram socket this is at least the
 * size of the next pending datagram, so it is useful for sizing a receive
 * buffer before calling `udp_recv` / `udp_recvfrom` to avoid truncation.
 * Writes `0` when the receive queue is empty.
 *
 * @param socket    Valid UDP socket.
 * @param available Receives the byte count. Must not be NULL.
 * @return `UDP_SUCCESS`, or `UDP_ERR_GENERIC` on a NULL output or query error.
 */
UdpStatus udp_bytes_available(UdpSocket socket, size_t* available) {
    if (!available) {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "NULL output pointer");
        return UDP_ERR_GENERIC;
    }
    *available = 0;

#if defined(_WIN32) || defined(_WIN64)
    u_long n = 0;
    if (ioctlsocket(socket, FIONREAD, &n) != 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        return UDP_ERR_GENERIC;
    }

    *available = (size_t)n;
#else
    int n = 0;
    if (ioctl(socket, FIONREAD, &n) != 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        return UDP_ERR_GENERIC;
    }

    *available = (n > 0) ? (size_t)n : 0;
#endif
    udp_set_last_error(UDP_SUCCESS, 0, NULL);
    return UDP_SUCCESS;
}


/* Waits for readability or writability: 1 ready, 0 timeout, -1 error. */
static int udp_wait_ready(UdpSocket socket, long timeout_ms, int want_write) {
#if defined(_WIN32) || defined(_WIN64)
    /* Winsock's fd_set is a counted array of handles, so any socket fits. */
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
    int rc = want_write ? select(0, NULL, &fds, NULL, tvp) : select(0, &fds, NULL, NULL, tvp);
#else
    /* poll(): FD_SET on a descriptor >= FD_SETSIZE would write past fd_set. */
    struct pollfd pfd;
    pfd.fd = (int)socket;
    pfd.events = want_write ? POLLOUT : POLLIN;
    pfd.revents = 0;
    int wait = (timeout_ms < 0) ? -1 : (timeout_ms > INT_MAX ? INT_MAX : (int)timeout_ms);
    int rc;
    do {
        rc = poll(&pfd, 1, wait);
    } while (rc < 0 && errno == EINTR);
#endif
    return rc < 0 ? -1 : (rc == 0 ? 0 : 1);
}


/**
 * @brief Wait until the socket has a datagram ready to read, or time out.
 *
 * Blocks in `select()` until data arrives or @p timeout_ms elapses — the
 * building block for poll-with-timeout loops without putting the whole socket
 * into non-blocking mode.
 *
 *  - `timeout_ms < 0`  → wait indefinitely.
 *  - `timeout_ms == 0` → poll and return immediately.
 *
 * (Uses `select` on Windows and `poll` on POSIX, so any descriptor number
 * works, including ones at or above `FD_SETSIZE`.)
 *
 * @param socket     Valid UDP socket.
 * @param timeout_ms Timeout in milliseconds (see above).
 * @return `UDP_SUCCESS` if the socket is readable, `UDP_ERR_NO_DATA` on
 *         timeout, or `UDP_ERR_GENERIC` on a select()/poll() error.
 */
UdpStatus udp_wait_readable(UdpSocket socket, long timeout_ms) {
    int rc = udp_wait_ready(socket, timeout_ms, 0);
    if (rc < 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        return UDP_ERR_GENERIC;
    }
    if (rc == 0) {
        udp_set_last_error(UDP_ERR_NO_DATA, 0, "Timeout waiting for data");
        return UDP_ERR_NO_DATA;
    }
    udp_set_last_error(UDP_SUCCESS, 0, NULL);

    return UDP_SUCCESS;
}


/* ================================================================== */
/* Pre-resolved endpoints, request/reply, and writable-readiness      */
/* (production additions). They reuse the existing udp_resolve_sockaddr */
/* and udp_set_last_error helpers and follow the same error model.     */
/* ================================================================== */

/**
 * @brief Resolve host:port into a reusable UdpEndpoint (one getaddrinfo).
 *
 * Unlike udp_sendto (which resolves on every call), resolve once here and then
 * send to the endpoint repeatedly with udp_sendto_endpoint. Prefers an
 * IPv6/IPv4-mapped result and falls back to a family-agnostic lookup. On any
 * failure `out->len` is reset to 0 ("unset").
 *
 * @param host Hostname or numeric IP to resolve. Must not be NULL.
 * @param port Destination port in host byte order.
 * @param out  Receives the resolved endpoint. Must not be NULL.
 * @return `UDP_SUCCESS` on success; `UDP_ERR_GENERIC` if `host` or `out` is NULL;
 *         `UDP_ERR_RESOLVE` if the name cannot be resolved.
 */
UdpStatus udp_resolve_endpoint(const char* host, unsigned short port, UdpEndpoint* out) {
    if (!host || !out) {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "Invalid udp_resolve_endpoint arguments");
        return UDP_ERR_GENERIC;
    }
    out->len = 0;

    struct sockaddr_storage addr;
    socklen_t               addrlen = 0;
    int rs = udp_resolve_sockaddr(host, port, &addr, &addrlen, AF_INET6, SOCK_DGRAM, AI_V4MAPPED | AI_ALL);
    if (rs != 0) {
        rs = udp_resolve_sockaddr(host, port, &addr, &addrlen, AF_UNSPEC, SOCK_DGRAM, 0);
    }
    if (rs != 0) {
        udp_set_last_error(UDP_ERR_RESOLVE, 0, "Failed to resolve endpoint");
        return UDP_ERR_RESOLVE;
    }
    memcpy(&out->addr, &addr, sizeof(out->addr));
    out->len = addrlen;
    return UDP_SUCCESS;
}

/**
 * @brief Send a buffer to a pre-resolved endpoint (no name resolution).
 *
 * Fast-path counterpart to udp_sendto; also used to reply to a sender captured
 * by udp_recvfrom_endpoint.
 *
 * @param socket A valid UDP socket.
 * @param buf    Data to send. Must not be NULL.
 * @param len    Number of bytes to send; must be non-zero.
 * @param sent   Output: number of bytes actually sent. May be NULL.
 * @param dest   Pre-resolved destination with `dest->len != 0`. Must not be NULL.
 * @return `UDP_SUCCESS` on success (`*sent` set when provided);
 *         `UDP_ERR_SEND` on invalid arguments or a send failure (`*sent` set to 0).
 */
UdpStatus udp_sendto_endpoint(UdpSocket socket, const void* buf, size_t len, size_t* sent, const UdpEndpoint* dest) {
    /* A zero-length datagram is valid UDP (e.g. an empty probe for
       udp_request_reply); only a NULL buffer with a non-zero length is not. */
    static const char empty = 0;
    if ((!buf && len > 0) || !dest || dest->len == 0) {
        udp_set_last_error(UDP_ERR_SEND, 0, "Invalid udp_sendto_endpoint arguments");
        return UDP_ERR_SEND;
    }
    if (!buf) {
        buf = &empty;
    }
#if defined(_WIN32) || defined(_WIN64)
    if (len > (size_t)INT_MAX) {
        udp_set_last_error(UDP_ERR_SEND, 0, "Datagram too large");
        return UDP_ERR_SEND;
    }
    int n = sendto(socket, (const char*)buf, (int)len, 0, (const struct sockaddr*)&dest->addr, (int)dest->len);
#else
    ssize_t n = sendto(socket, buf, len, 0, (const struct sockaddr*)&dest->addr, dest->len);
#endif
    if (n < 0) {
        udp_set_last_error_sys(UDP_ERR_SEND);
        if (sent) { *sent = 0; }
        return UDP_ERR_SEND;
    }
    if (sent) { *sent = (size_t)n; }
    return UDP_SUCCESS;
}

/**
 * @brief Receive a datagram and capture the sender into src (may be NULL).
 *        Mirrors udp_recvfrom but yields a reusable endpoint, not a string.
 *
 * @param socket   A valid UDP socket.
 * @param buf      Receive buffer. Must not be NULL.
 * @param len      Buffer size in bytes; must be non-zero.
 * @param received Output: number of bytes received. May be NULL.
 * @param src      Output: sender's endpoint, reusable with udp_sendto_endpoint
 *                 and udp_endpoint_equal. May be NULL.
 * @return `UDP_SUCCESS` on success; `UDP_ERR_NO_DATA` if the socket is
 *         non-blocking (or has a timeout set) and no datagram is ready;
 *         `UDP_ERR_RECV` on invalid arguments or a receive error.
 */
UdpStatus udp_recvfrom_endpoint(UdpSocket socket, void* buf, size_t len, size_t* received, UdpEndpoint* src) {
    if (!buf || len == 0) {
        udp_set_last_error(UDP_ERR_RECV, 0, "Invalid udp_recvfrom_endpoint arguments");
        return UDP_ERR_RECV;
    }
    struct sockaddr_storage addr;
    socklen_t               addrlen = sizeof(addr);
    bool truncated = false;
    long n = udp_recv_dgram(socket, buf, len, &addr, &addrlen, &truncated);
    if (n < 0) {
#if defined(_WIN32) || defined(_WIN64)
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK) {
            udp_set_last_error(UDP_ERR_NO_DATA, err, "No data available (would block)");
            if (received) { *received = 0; }
            return UDP_ERR_NO_DATA;
        }
#else
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            udp_set_last_error(UDP_ERR_NO_DATA, errno, "No data available (would block)");
            if (received) { *received = 0; }
            return UDP_ERR_NO_DATA;
        }
#endif
        udp_set_last_error_sys(UDP_ERR_RECV);
        if (received) { *received = 0; }
        return UDP_ERR_RECV;
    }
    if (received) { *received = (size_t)n; }
    if (src) {
        memcpy(&src->addr, &addr, sizeof(src->addr));
        src->len = addrlen;
    }
    if (truncated) {
        udp_set_last_error(UDP_ERR_BUFFER_TOO_SMALL, 0, "Datagram larger than the buffer (truncated)");
        return UDP_ERR_BUFFER_TOO_SMALL;
    }
    return UDP_SUCCESS;
}

/**
 * @brief Printable host (and port) for an endpoint. host/port may be NULL.
 *
 * Renders the numeric address via inet_ntop. Both outputs are optional, so the
 * call can fetch only the port, only the address, or both.
 *
 * @param ep       A set endpoint (`ep->len != 0`). Must not be NULL.
 * @param host     Output buffer for the address text; ignored when NULL or
 *                 `host_len == 0`.
 * @param host_len Size of `host` in bytes.
 * @param port     Output: port in host byte order. May be NULL.
 * @return `UDP_SUCCESS` on success; `UDP_ERR_GENERIC` if `ep` is NULL/unset, the
 *         address family is neither IPv4 nor IPv6, or inet_ntop fails.
 */
UdpStatus udp_endpoint_to_string(const UdpEndpoint* ep, char* host, size_t host_len, unsigned short* port) {
    if (!ep || ep->len == 0) {
        return UDP_ERR_GENERIC;
    }
    const void*    addrptr = NULL;
    unsigned short p       = 0;
    if (ep->addr.ss_family == AF_INET) {
        const struct sockaddr_in* s4 = (const struct sockaddr_in*)&ep->addr;
        addrptr = &s4->sin_addr;
        p       = ntohs(s4->sin_port);
    }
    else if (ep->addr.ss_family == AF_INET6) {
        const struct sockaddr_in6* s6 = (const struct sockaddr_in6*)&ep->addr;
        addrptr = &s6->sin6_addr;
        p       = ntohs(s6->sin6_port);
    }
    else {
        return UDP_ERR_GENERIC;
    }
    if (port) { *port = p; }
    if (host && host_len > 0) {
        if (!inet_ntop(ep->addr.ss_family, addrptr, host, (socklen_t)host_len)) {
            host[0] = '\0';
            return UDP_ERR_GENERIC;
        }
    }
    return UDP_SUCCESS;
}

/**
 * @brief True if two endpoints share family, address and port.
 *
 * @param a First endpoint. May be NULL.
 * @param b Second endpoint. May be NULL.
 * @return `true` only if both are set, share the same address family, and have
 *         identical address and port; `false` otherwise (including any NULL or
 *         unset operand).
 */
bool udp_endpoint_equal(const UdpEndpoint* a, const UdpEndpoint* b) {
    if (!a || !b || a->len == 0 || b->len == 0 || a->addr.ss_family != b->addr.ss_family) {
        return false;
    }
    if (a->addr.ss_family == AF_INET) {
        const struct sockaddr_in* x = (const struct sockaddr_in*)&a->addr;
        const struct sockaddr_in* y = (const struct sockaddr_in*)&b->addr;
        return x->sin_port == y->sin_port &&
               memcmp(&x->sin_addr, &y->sin_addr, sizeof(x->sin_addr)) == 0;
    }
    if (a->addr.ss_family == AF_INET6) {
        const struct sockaddr_in6* x = (const struct sockaddr_in6*)&a->addr;
        const struct sockaddr_in6* y = (const struct sockaddr_in6*)&b->addr;
        return x->sin6_port == y->sin6_port &&
               memcmp(&x->sin6_addr, &y->sin6_addr, sizeof(x->sin6_addr)) == 0;
    }
    return false;
}

/**
 * @brief Request/response with timeout and retries (the UDP reliability pattern).
 *
 * Sends the request, waits up to `timeout_ms` for a reply, and resends up to
 * `retries` times on timeout. Total attempts are `retries + 1`; a negative
 * `retries` is clamped to 0. With `match_source`, replies from any peer other
 * than `dest` are ignored (treated as "keep waiting").
 *
 * @param socket       A valid UDP socket (bind first if you need a fixed local port).
 * @param dest         Pre-resolved destination (`dest->len != 0`). Must not be NULL.
 * @param req          Request payload (may be NULL only when `req_len == 0`).
 * @param req_len      Request length in bytes.
 * @param reply        Buffer for the response. Must not be NULL.
 * @param reply_cap    Capacity of `reply` in bytes; must be non-zero.
 * @param reply_len    Output: reply length on success, 0 otherwise. Must not be NULL.
 * @param timeout_ms   Per-attempt timeout in milliseconds.
 * @param retries      Number of additional attempts after the first.
 * @param match_source When true, accept only a reply whose source equals `dest`.
 * @return `UDP_SUCCESS` if a reply arrived within the retry budget (`*reply_len`
 *         set); `UDP_ERR_NO_DATA` if every attempt timed out; `UDP_ERR_GENERIC`
 *         on invalid arguments; or a propagated send/wait error code.
 */
UdpStatus udp_request_reply(UdpSocket socket, const UdpEndpoint* dest,
                            const void* req, size_t req_len,
                            void* reply, size_t reply_cap, size_t* reply_len,
                            long timeout_ms, int retries, bool match_source) {
    if (!dest || dest->len == 0 || (!req && req_len > 0) || !reply || reply_cap == 0 || !reply_len) {
        udp_set_last_error(UDP_ERR_GENERIC, 0, "Invalid udp_request_reply arguments");
        return UDP_ERR_GENERIC;
    }
    if (retries < 0) {
        retries = 0;
    }
    *reply_len = 0;

    for (int attempt = 0; attempt <= retries; ++attempt) {
        size_t    sent = 0;
        UdpStatus s = udp_sendto_endpoint(socket, req, req_len, &sent, dest);
        if (s != UDP_SUCCESS) {
            return s;
        }

        UdpStatus w = udp_wait_readable(socket, timeout_ms);
        if (w == UDP_ERR_NO_DATA) {
            continue;                          /* timed out, resend */
        }
        if (w != UDP_SUCCESS) {
            return w;
        }

        UdpEndpoint src;
        size_t      got = 0;
        UdpStatus   r = udp_recvfrom_endpoint(socket, reply, reply_cap, &got, &src);
        if (r == UDP_ERR_BUFFER_TOO_SMALL && (!match_source || udp_endpoint_equal(&src, dest))) {
            *reply_len = got;                  /* reply truncated to reply_cap */
            return r;
        }
        if (r != UDP_SUCCESS) {
            continue;                          /* transient recv issue, resend */
        }
        if (match_source && !udp_endpoint_equal(&src, dest)) {
            continue;                          /* reply from a different peer, resend */
        }
        *reply_len = got;
        return UDP_SUCCESS;
    }

    udp_set_last_error(UDP_ERR_NO_DATA, 0, "No reply within the retry budget");
    return UDP_ERR_NO_DATA;
}

/**
 * @brief Wait until the socket is writable (or timeout). Mirrors
 *        udp_wait_readable on the write fd-set.
 *
 * @param socket     A valid UDP socket.
 * @param timeout_ms Timeout in milliseconds; `< 0` waits indefinitely, `0` polls
 *                   and returns immediately.
 * @return `UDP_SUCCESS` if the socket is writable; `UDP_ERR_NO_DATA` on timeout;
 *         `UDP_ERR_GENERIC` on a select() error.
 */
UdpStatus udp_wait_writable(UdpSocket socket, long timeout_ms) {
    int rc = udp_wait_ready(socket, timeout_ms, 1);
    if (rc < 0) {
        udp_set_last_error_sys(UDP_ERR_GENERIC);
        return UDP_ERR_GENERIC;
    }
    if (rc == 0) {
        udp_set_last_error(UDP_ERR_NO_DATA, 0, "Timeout waiting for writability");
        return UDP_ERR_NO_DATA;
    }
    udp_set_last_error(UDP_SUCCESS, 0, NULL);
    return UDP_SUCCESS;
}
