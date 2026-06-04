/**
 * @author Amin Tahmasebi
 * @class SerialPort
 *
 * Cross-platform serial-port implementation. Doxygen blocks live above
 * each public function.
 */
#include "serial_port.h"

#if defined(_WIN32) || defined(_WIN64)
/* Avoid the import lib dependency by carrying our own copy of
 * GUID_DEVINTERFACE_COMPORT (defined in MSDN as
 * {86E0D1E0-8089-11D0-9CE4-08003E301F73}). */
static const GUID SERIAL_DEVINTERFACE_COMPORT = {
    0x86E0D1E0, 0x8089, 0x11D0,
    {0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73}
};
#endif
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if !defined(_WIN32) && !defined(_WIN64)
#include <sys/ioctl.h>
#endif

static int g_initialized = 0;


#if defined(_WIN32) || defined(_WIN64)
/* On Windows an OPEN port has a valid HANDLE; the field is set to
 * NULL/INVALID_HANDLE_VALUE when closed. */
static int handle_is_open_win(HANDLE h) {
    return h != NULL && h != INVALID_HANDLE_VALUE;
}
#endif


/**
 * @brief Reports whether @p port has an active OS handle.
 *
 * Useful before calling any I/O function. Safe with NULL.
 */
bool serial_is_open(const SerialPort* port) {
    if (!port) return false;
#if defined(_WIN32) || defined(_WIN64)
    return handle_is_open_win(port->handle);
#else
    return port->handle > 0;
#endif
}

/* --------------------------------------------------------------------
 * Windows event-thread plumbing
 * -------------------------------------------------------------------- */

#if defined(_WIN32) || defined(_WIN64)
static DWORD WINAPI SerialEventThread(LPVOID param) {
    SerialPort* port = (SerialPort*)param;
    if (!port || !handle_is_open_win(port->handle)) return 0;

    SerialPortCallbackData* cd = (SerialPortCallbackData*)port->callback_data;
    while (cd && InterlockedCompareExchange(&cd->stop, 0, 0) == 0) {
        DWORD event_mask = 0;
        if (!WaitCommEvent((HANDLE)port->handle, &event_mask, NULL)) {
            SERIAL_LOG("[SerialEventThread]: WaitCommEvent failed (%lu) — exiting", GetLastError());
            break;
        }
        if (cd->callback) {
            cd->callback((int)event_mask, cd->user_data);
        }
    }
    return 0;
}
#endif


/**
 * @brief Initialise the library. Must be called once before any other
 *        serial_* call (other than serial_is_open).
 *
 * Returns -1 if already initialised.
 */
int serial_init(void) {
    if (g_initialized) {
        SERIAL_LOG("[serial_init]: already initialised");
        return -1;
    }
#if defined(_WIN32) || defined(_WIN64) || defined(__linux__) || defined(__APPLE__) \
    || defined(__unix__) || defined(__FreeBSD__) || defined(__OpenBSD__)
    g_initialized = 1;
    SERIAL_LOG("[serial_init]: ok");
    return 0;
#else
    SERIAL_LOG("[serial_init]: unsupported platform");
    return -1;
#endif
}

/**
 * @brief Pair of serial_init. Returns -1 if never initialised.
 */
int serial_cleanup(void) {
    if (!g_initialized) {
        SERIAL_LOG("[serial_cleanup]: not initialised");
        return -1;
    }
    g_initialized = 0;
    return 0;
}


/**
 * @brief Enumerate every serial port available on the system.
 *
 * @param[out] ports On success, set to a newly-allocated array of
 *                   SerialPortInformation. Caller frees with
 *                   serial_free_ports.
 * @param[out] count On success, set to the number of entries in
 *                   *ports.
 * @return 0 on success, -1 on failure.
 */
int serial_list_ports(SerialPortInformation** ports, size_t* count) {
    if (!ports || !count) {
        SERIAL_LOG("[serial_list_ports]: NULL out arg");
        return -1;
    }
    *ports = NULL;
    *count = 0;

#if defined(_WIN32) || defined(_WIN64)
    HDEVINFO hDevInfo = SetupDiGetClassDevsA(
        &SERIAL_DEVINTERFACE_COMPORT, NULL, NULL,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        SERIAL_LOG("[serial_list_ports]: SetupDiGetClassDevsA failed (%lu)", GetLastError());
        return -1;
    }

    SP_DEVINFO_DATA devInfoData = {0};
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    size_t cap = 8;
    *ports = (SerialPortInformation*)calloc(cap, sizeof(SerialPortInformation));
    if (!*ports) {
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return -1;
    }

    size_t idx = 0;
    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); ++i) {
        char friendly_name[256] = {0};
        if (!SetupDiGetDeviceRegistryPropertyA(
                hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME,
                NULL, (BYTE*)friendly_name, sizeof(friendly_name), NULL)) {
            continue;
        }

        char* com_pos = strstr(friendly_name, "COM");
        if (!com_pos) {
            continue;
        }

        char com_name[16] = {0};
        size_t j = 0;
        while (com_pos[j] && j < sizeof(com_name) - 1) {
            if (com_pos[j] == ')' || com_pos[j] == ' ') {
                break;
            }
            com_name[j] = com_pos[j];
            j++;
        }
        com_name[j] = '\0';

        if (idx >= cap) {
            size_t new_cap = cap * 2;
            SerialPortInformation* tmp = (SerialPortInformation*)realloc(
                *ports, sizeof(SerialPortInformation) * new_cap);
            if (!tmp) {
                free(*ports);
                *ports = NULL;
                SetupDiDestroyDeviceInfoList(hDevInfo);
                return -1;
            }
            memset(tmp + cap, 0, sizeof(SerialPortInformation) * (new_cap - cap));
            *ports = tmp;
            cap = new_cap;
        }

        snprintf((*ports)[idx].name,        sizeof((*ports)[idx].name),        "%s", com_name);
        snprintf((*ports)[idx].description, sizeof((*ports)[idx].description), "%s", friendly_name);

        char hw[256] = {0};
        if (SetupDiGetDeviceRegistryPropertyA(
                hDevInfo, &devInfoData, SPDRP_HARDWAREID,
                NULL, (BYTE*)hw, sizeof(hw), NULL)) {
            snprintf((*ports)[idx].hardware_id, sizeof((*ports)[idx].hardware_id), "%s", hw);
        } 
        else {
            snprintf((*ports)[idx].hardware_id, sizeof((*ports)[idx].hardware_id), "N/A");
        }
        idx++;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    *count = idx;
    return 0;

#else
    DIR* d = opendir("/dev");
    if (!d) {
        SERIAL_LOG("[serial_list_ports]: opendir /dev failed: %s", strerror(errno));
        return -1;
    }

    size_t cap = 8;
    *ports = (SerialPortInformation*)calloc(cap, sizeof(SerialPortInformation));
    if (!*ports) { 
        closedir(d); 
        return -1; 
    }

    struct dirent* entry;
    size_t idx = 0;
    while ((entry = readdir(d)) != NULL) {
        if (strncmp(entry->d_name, "ttyS",   4) != 0 &&
            strncmp(entry->d_name, "ttyUSB", 6) != 0 &&
            strncmp(entry->d_name, "ttyACM", 6) != 0 &&
            strncmp(entry->d_name, "cu.",    3) != 0 &&   /* macOS */
            strncmp(entry->d_name, "tty.",   4) != 0) {
            continue;
        }

        char full[256];
        snprintf(full, sizeof(full), "/dev/%s", entry->d_name);

        struct stat st;
        if (stat(full, &st) != 0 || !S_ISCHR(st.st_mode)) continue;

        if (idx >= cap) {
            size_t new_cap = cap * 2;
            SerialPortInformation* tmp = (SerialPortInformation*)realloc(*ports, sizeof(SerialPortInformation) * new_cap);

            if (!tmp) { 
                free(*ports); 
                *ports = NULL; 
                closedir(d); 
                return -1; 
            }

            memset(tmp + cap, 0, sizeof(SerialPortInformation) * (new_cap - cap));
            *ports = tmp;
            cap = new_cap;
        }

        snprintf((*ports)[idx].name,        sizeof((*ports)[idx].name),        "%s", full);
        snprintf((*ports)[idx].description, sizeof((*ports)[idx].description), "Serial Port %s", entry->d_name);
        snprintf((*ports)[idx].hardware_id, sizeof((*ports)[idx].hardware_id), "N/A");
        idx++;
    }
    closedir(d);
    *count = idx;
    return 0;
#endif
}


/**
 * @brief Free the array returned by serial_list_ports. Safe with NULL.
 */
void serial_free_ports(SerialPortInformation* ports) {
    free(ports);
}


/**
 * @brief Open the named serial port.
 *
 * The caller-provided @p port is zero-initialised, then populated with
 * the OS handle. After this returns successfully you can call
 * serial_configure, serial_read / serial_write, etc.
 *
 * @return 0 on success, -1 on any error (port_name NULL, OS open
 *         failed, etc.).
 */
int serial_open(const char* port_name, SerialPort* port) {
    if (!port_name || !port) {
        SERIAL_LOG("[serial_open]: NULL arg");
        return -1;
    }
    memset(port, 0, sizeof(SerialPort));
    strncpy(port->port_name, port_name, sizeof(port->port_name) - 1);

#if defined(_WIN32) || defined(_WIN64)
    HANDLE h = CreateFileA(port_name, GENERIC_READ | GENERIC_WRITE,
                           0, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        SERIAL_LOG("[serial_open]: CreateFileA(%s) failed (%lu)", port_name, GetLastError());
        return -1;
    }
    port->handle = h;
    return 0;
#else
    /* Open WITHOUT O_NONBLOCK so the documented blocking serial_read
     * actually blocks. Use O_NONBLOCK on the open() to avoid hanging
     * during DCD-handshake on some hardware, but clear it afterwards. */
    int fd = open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        SERIAL_LOG("[serial_open]: open(%s) failed: %s", port_name, strerror(errno));
        return -1;
    }
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
    }

    struct termios options;
    if (tcgetattr(fd, &options) < 0) {
        SERIAL_LOG("[serial_open]: tcgetattr failed: %s", strerror(errno));
        close(fd);
        return -1;
    }
    cfmakeraw(&options);
    options.c_cflag |= (CLOCAL | CREAD);
    if (tcsetattr(fd, TCSANOW, &options) < 0) {
        SERIAL_LOG("[serial_open]: tcsetattr failed: %s", strerror(errno));
        close(fd);
        return -1;
    }
    port->handle = fd;
    return 0;
#endif
}


/**
 * @brief Close a port previously opened with serial_open.
 *
 * Releases the OS handle and frees the event-callback data (if any).
 * Safe to call more than once.
 */
int serial_close(SerialPort* port) {
    if (!port) {
        SERIAL_LOG("[serial_close]: NULL port");
        return -1;
    }
    if (!serial_is_open(port)) {
        SERIAL_LOG("[serial_close]: already closed");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    /* Ask the event thread to exit first, then close. WaitCommEvent
     * unblocks when the handle is closed. */
    SerialPortCallbackData* cd = (SerialPortCallbackData*)port->callback_data;
    if (cd) {
        /* Ask the worker to stop, force any pending WaitCommEvent to return
         * immediately, then JOIN the thread before freeing cd. */
        InterlockedExchange(&cd->stop, 1);
        SetCommMask((HANDLE)port->handle, 0);
        if (cd->thread) {
            WaitForSingleObject(cd->thread, INFINITE);
            CloseHandle(cd->thread);
            cd->thread = NULL;
        }
    }

    if (!CloseHandle((HANDLE)port->handle)) {
        SERIAL_LOG("[serial_close]: CloseHandle failed (%lu)", GetLastError());
        free(cd);
        port->callback_data = NULL;
        return -1;
    }
    port->handle = NULL;

    free(cd);
    port->callback_data = NULL;
#else
    if (close(port->handle) < 0) {
        SERIAL_LOG("[serial_close]: close failed: %s", strerror(errno));
        return -1;
    }
    port->handle = 0;
#endif
    return 0;
}


#if !defined(_WIN32) && !defined(_WIN64)
/* Translate an integer baud rate to the matching termios B* constant. */
static speed_t get_baud_rate_constant(int baud_rate) {
    switch (baud_rate) {
        case 50:     return B50;
        case 75:     return B75;
        case 110:    return B110;
        case 134:    return B134;
        case 150:    return B150;
        case 200:    return B200;
        case 300:    return B300;
        case 600:    return B600;
        case 1200:   return B1200;
        case 1800:   return B1800;
        case 2400:   return B2400;
        case 4800:   return B4800;
        case 9600:   return B9600;
        case 19200:  return B19200;
        case 38400:  return B38400;
        case 57600:  return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        default:     return 0;
    }
}
#endif


/**
 * @brief Apply a configuration to an open port.
 *
 * @return 0 on success, -1 on bad arg / OS failure / unsupported
 *         baud rate (POSIX only).
 */
int serial_configure(SerialPort* port, const SerialConfig* config) {
    if (!port || !config || !serial_is_open(port)) {
        SERIAL_LOG("[serial_configure]: bad arg or port closed");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    DCB dcb = {0};
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState((HANDLE)port->handle, &dcb)) {
        SERIAL_LOG("[serial_configure]: GetCommState failed (%lu)", GetLastError());
        return -1;
    }
    dcb.BaudRate = config->baud_rate;
    dcb.ByteSize = (BYTE)config->data_bits;
    dcb.Parity   = (BYTE)config->parity;
    dcb.StopBits = (BYTE)(config->stop_bits == SERIAL_STOP_BITS_ONE ? ONESTOPBIT : TWOSTOPBITS);

    if (config->flow_control == SERIAL_FLOW_CONTROL_HARDWARE) {
        dcb.fOutxCtsFlow = TRUE;
        dcb.fRtsControl  = RTS_CONTROL_HANDSHAKE;
        dcb.fInX = FALSE; dcb.fOutX = FALSE;
    } 
    else if (config->flow_control == SERIAL_FLOW_CONTROL_SOFTWARE) {
        dcb.fOutxCtsFlow = FALSE;
        dcb.fRtsControl  = RTS_CONTROL_DISABLE;
        dcb.fInX = TRUE;  dcb.fOutX = TRUE;
    } 
    else {
        dcb.fOutxCtsFlow = FALSE;
        dcb.fRtsControl  = RTS_CONTROL_DISABLE;
        dcb.fInX = FALSE; dcb.fOutX = FALSE;
    }

    if (!SetCommState((HANDLE)port->handle, &dcb)) {
        SERIAL_LOG("[serial_configure]: SetCommState failed (%lu)", GetLastError());
        return -1;
    }
    port->config = *config;
    return 0;
#else
    struct termios options;
    if (tcgetattr(port->handle, &options) < 0) {
        SERIAL_LOG("[serial_configure]: tcgetattr failed: %s", strerror(errno));
        return -1;
    }
    speed_t baud = get_baud_rate_constant(config->baud_rate);
    if (baud == 0) {
        SERIAL_LOG("[serial_configure]: unsupported baud %d", config->baud_rate);
        return -1;
    }
    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);

    options.c_cflag &= ~CSIZE;
    switch (config->data_bits) {
        case 5: 
            options.c_cflag |= CS5; 
            break;
        case 6: 
            options.c_cflag |= CS6; 
            break;
        case 7: 
            options.c_cflag |= CS7; 
            break;
        case 8:
        default: 
            options.c_cflag |= CS8; 
            break;
    }

    options.c_cflag &= ~(PARENB | PARODD);
    if (config->parity == SERIAL_PARITY_ODD)  options.c_cflag |= PARENB | PARODD;
    if (config->parity == SERIAL_PARITY_EVEN) options.c_cflag |= PARENB;

    options.c_cflag &= ~CSTOPB;
    if (config->stop_bits == SERIAL_STOP_BITS_TWO) options.c_cflag |= CSTOPB;

#ifdef CRTSCTS
    if (config->flow_control == SERIAL_FLOW_CONTROL_HARDWARE) options.c_cflag |= CRTSCTS;
    else                                                       options.c_cflag &= ~CRTSCTS;
#endif

    if (config->flow_control == SERIAL_FLOW_CONTROL_SOFTWARE) options.c_iflag |=  (IXON | IXOFF);
    else                                                       options.c_iflag &= ~(IXON | IXOFF);

    options.c_cflag |= (CLOCAL | CREAD);

    if (tcsetattr(port->handle, TCSANOW, &options) < 0) {
        SERIAL_LOG("[serial_configure]: tcsetattr failed: %s", strerror(errno));
        return -1;
    }
    port->config = *config;
    return 0;
#endif
}


/**
 * @brief Read the port's current configuration into @p config.
 *
 * @return 0 on success, -1 on failure.
 */
int serial_get_config(SerialPort* port, SerialConfig* config) {
    if (!port || !config || !serial_is_open(port)) {
        SERIAL_LOG("[serial_get_config]: bad arg or port closed");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    DCB dcb = {0};
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState((HANDLE)port->handle, &dcb)) {
        SERIAL_LOG("[serial_get_config]: GetCommState failed (%lu)", GetLastError());
        return -1;
    }
    config->baud_rate    = dcb.BaudRate;
    config->data_bits    = dcb.ByteSize;
    config->parity       = (dcb.Parity == NOPARITY)  ? SERIAL_PARITY_NONE
                         : (dcb.Parity == ODDPARITY) ? SERIAL_PARITY_ODD
                         :                              SERIAL_PARITY_EVEN;
    config->stop_bits    = (dcb.StopBits == ONESTOPBIT) ? SERIAL_STOP_BITS_ONE
                                                        : SERIAL_STOP_BITS_TWO;
    config->flow_control = dcb.fOutxCtsFlow ? SERIAL_FLOW_CONTROL_HARDWARE
                                            : (dcb.fInX || dcb.fOutX) ? SERIAL_FLOW_CONTROL_SOFTWARE
                                                                      : SERIAL_FLOW_CONTROL_NONE;
    return 0;
#else
    struct termios options;
    if (tcgetattr(port->handle, &options) < 0) {
        SERIAL_LOG("[serial_get_config]: tcgetattr failed: %s", strerror(errno));
        return -1;
    }
    config->baud_rate = (int)cfgetispeed(&options);
    config->data_bits = ((options.c_cflag & CSIZE) == CS7) ? 7 : 8;
    config->parity    = (options.c_cflag & PARENB)
                          ? ((options.c_cflag & PARODD) ? SERIAL_PARITY_ODD : SERIAL_PARITY_EVEN)
                          : SERIAL_PARITY_NONE;
    config->stop_bits = (options.c_cflag & CSTOPB) ? SERIAL_STOP_BITS_TWO : SERIAL_STOP_BITS_ONE;

#ifdef CRTSCTS
    if (options.c_cflag & CRTSCTS) config->flow_control = SERIAL_FLOW_CONTROL_HARDWARE;
    else
#endif
    if (options.c_iflag & (IXON | IXOFF)) {
        config->flow_control = SERIAL_FLOW_CONTROL_SOFTWARE; 
    }
    else {                             
        config->flow_control = SERIAL_FLOW_CONTROL_NONE;
    }
    return 0;
#endif
}


/**
 * @brief Write @p size bytes from @p data to the port.
 *
 * @return Number of bytes actually written, or -1 on error.
 */
int serial_write(SerialPort* port, const void* data, size_t size) {
    if (!port || !data || size == 0 || !serial_is_open(port)) {
        SERIAL_LOG("[serial_write]: bad arg or port closed");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    DWORD written = 0;
    if (!WriteFile((HANDLE)port->handle, data, (DWORD)size, &written, NULL)) {
        SERIAL_LOG("[serial_write]: WriteFile failed (%lu)", GetLastError());
        return -1;
    }
    return (int)written;
#else
    ssize_t n = write(port->handle, data, size);
    if (n < 0) {
        SERIAL_LOG("[serial_write]: write failed: %s", strerror(errno));
        return -1;
    }
    return (int)n;
#endif
}


/**
 * @brief Blocking read of up to @p size bytes into @p buffer.
 *
 * @return Bytes read (0 on EOF), or -1 on error.
 */
int serial_read(SerialPort* port, void* buffer, size_t size) {
    if (!port || !buffer || size == 0 || !serial_is_open(port)) {
        SERIAL_LOG("[serial_read]: bad arg or port closed");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    DWORD got = 0;
    if (!ReadFile((HANDLE)port->handle, buffer, (DWORD)size, &got, NULL)) {
        SERIAL_LOG("[serial_read]: ReadFile failed (%lu)", GetLastError());
        return -1;
    }
    return (int)got;
#else
    ssize_t n = read(port->handle, buffer, size);
    if (n < 0) {
        SERIAL_LOG("[serial_read]: read failed: %s", strerror(errno));
        return -1;
    }
    return (int)n;
#endif
}


/**
 * @brief Non-blocking read: returns 0 immediately when no data is
 *        available rather than blocking.
 *
 * Returns -1 only on a hard error; EAGAIN/no-data is reported as 0.
 */
int serial_read_nonblocking(SerialPort* port, void* buffer, size_t size) {
    if (!port || !buffer || size == 0 || !serial_is_open(port)) {
        SERIAL_LOG("[serial_read_nonblocking]: bad arg or port closed");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    /* Use ClearCommError to peek; only ReadFile what is actually
     * available so we never block. */
    COMSTAT cs;
    DWORD errors = 0;
    if (!ClearCommError((HANDLE)port->handle, &errors, &cs)) {
        SERIAL_LOG("[serial_read_nonblocking]: ClearCommError failed (%lu)", GetLastError());
        return -1;
    }
    if (cs.cbInQue == 0) return 0;
    DWORD want = (DWORD)size;
    if (cs.cbInQue < want) want = cs.cbInQue;
    DWORD got = 0;
    if (!ReadFile((HANDLE)port->handle, buffer, want, &got, NULL)) {
        SERIAL_LOG("[serial_read_nonblocking]: ReadFile failed (%lu)", GetLastError());
        return -1;
    }
    return (int)got;
#else
    /* Temporarily set non-blocking, read, restore. */
    int flags = fcntl(port->handle, F_GETFL, 0);
    if (flags < 0) return -1;
    fcntl(port->handle, F_SETFL, flags | O_NONBLOCK);

    ssize_t n = read(port->handle, buffer, size);
    int err = errno;
    fcntl(port->handle, F_SETFL, flags);

    if (n < 0) {
        if (err == EAGAIN || err == EWOULDBLOCK) {
            return 0;
        }
        SERIAL_LOG("[serial_read_nonblocking]: read failed: %s", strerror(err));
        return -1;
    }
    return (int)n;
#endif
}


/**
 * @brief Register a callback to be invoked when the port receives a
 *        character or signals an error.
 *
 * Windows: spawns a background thread that blocks in WaitCommEvent.
 *
 * POSIX: not implemented (use select()/poll()).
 *
 * @return 0 on success, -1 on failure.
 */
int serial_set_event_callback(SerialPort* port, SerialEventCallback callback, void* user_data) {
    if (!port || !callback || !serial_is_open(port)) {
        SERIAL_LOG("[serial_set_event_callback]: bad arg or port closed");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    SerialPortCallbackData* cd = (SerialPortCallbackData*)calloc(1, sizeof(*cd));
    if (!cd) return -1;
    cd->callback  = callback;
    cd->user_data = user_data;
    cd->stop      = 0;
    port->callback_data = cd;

    if (!SetCommMask((HANDLE)port->handle, EV_RXCHAR | EV_ERR)) {
        SERIAL_LOG("[serial_set_event_callback]: SetCommMask failed (%lu)", GetLastError());
        free(cd);
        port->callback_data = NULL;
        return -1;
    }

    HANDLE t = CreateThread(NULL, 0, SerialEventThread, port, 0, NULL);
    if (!t) {
        SERIAL_LOG("[serial_set_event_callback]: CreateThread failed (%lu)", GetLastError());
        free(cd);
        port->callback_data = NULL;
        return -1;
    }
    cd->thread = t;   /* when serial_close can join the thread */
    return 0;
#else
    (void)callback;
    (void)user_data;
    SERIAL_LOG("[serial_set_event_callback]: not implemented on POSIX");
    return -1;
#endif
}


/**
 * @brief Number of bytes currently waiting in the OS input queue.
 *
 * Useful for `serial_read` callers that want to size their read
 * exactly, or to poll without blocking.
 *
 * @return Bytes available, or -1 on error.
 */
int serial_bytes_available(SerialPort* port) {
    if (!port || !serial_is_open(port)) return -1;
#if defined(_WIN32) || defined(_WIN64)
    COMSTAT cs;
    DWORD errors = 0;
    if (!ClearCommError((HANDLE)port->handle, &errors, &cs)) { 
        return -1;
    }

    return (int)cs.cbInQue;
#else
    int bytes = 0;
    /* TIOCINQ is the portable Linux/BSD/macOS ioctl. */
    #ifdef FIONREAD
    if (ioctl(port->handle, FIONREAD, &bytes) < 0) {
        return -1;
    }
    #else
    return -1;
    #endif
    return bytes;
#endif
}


/**
 * @brief Discard any data sitting in the input buffer.
 *
 * @return 0 on success, -1 on error.
 */
int serial_flush_input(SerialPort* port) {
    if (!port || !serial_is_open(port)) return -1;
#if defined(_WIN32) || defined(_WIN64)
    return PurgeComm((HANDLE)port->handle, PURGE_RXCLEAR | PURGE_RXABORT) ? 0 : -1;
#else
    return tcflush(port->handle, TCIFLUSH);
#endif
}


/**
 * @brief Block until the OS output buffer is empty, then return.
 *
 * Useful before closing a port to make sure your last write reaches
 * the wire.
 *
 * @return 0 on success, -1 on error.
 */
int serial_flush_output(SerialPort* port) {
    if (!port || !serial_is_open(port)) return -1;
#if defined(_WIN32) || defined(_WIN64)
    return FlushFileBuffers((HANDLE)port->handle) ? 0 : -1;
#else
    return tcdrain(port->handle);
#endif
}


/**
 * @brief Configure read and write timeouts in milliseconds.
 *
 * Pass `0` for either value to disable that timeout. On Windows this
 * uses SetCommTimeouts; on POSIX it programs VMIN/VTIME on the input
 * side. (The POSIX write timeout is ignored — write() blocks until
 * the kernel drains its buffer.)
 *
 * @return 0 on success, -1 on error.
 */
int serial_set_timeouts(SerialPort* port, int read_ms, int write_ms) {
    if (!port || !serial_is_open(port) || read_ms < 0 || write_ms < 0) return -1;
#if defined(_WIN32) || defined(_WIN64)
    COMMTIMEOUTS t = {0};
    if (read_ms == 0) {
        /* No timeout — block until at least 1 byte arrives. */
        t.ReadIntervalTimeout         = 0;
        t.ReadTotalTimeoutMultiplier  = 0;
        t.ReadTotalTimeoutConstant    = 0;
    } 
    else {
        t.ReadIntervalTimeout         = MAXDWORD;
        t.ReadTotalTimeoutMultiplier  = MAXDWORD;
        t.ReadTotalTimeoutConstant    = (DWORD)read_ms;
    }
    t.WriteTotalTimeoutMultiplier = 0;
    t.WriteTotalTimeoutConstant   = (DWORD)write_ms;

    return SetCommTimeouts((HANDLE)port->handle, &t) ? 0 : -1;
#else
    (void)write_ms;
    struct termios opt;
    if (tcgetattr(port->handle, &opt) < 0) {
        return -1;
    }

    /* VTIME is in tenths of a second. */
    int tenths = (read_ms + 99) / 100;
    if (tenths > 255) tenths = 255;
    if (read_ms == 0) {
        opt.c_cc[VMIN]  = 1;
        opt.c_cc[VTIME] = 0;
    } 
    else {
        opt.c_cc[VMIN]  = 0;
        opt.c_cc[VTIME] = (cc_t)tenths;
    }
    return tcsetattr(port->handle, TCSANOW, &opt);
#endif
}
