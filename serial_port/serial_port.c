/**
 * @author Amin Tahmasebi
 * @class SerialPort
 *
 * Cross-platform serial-port implementation. Doxygen blocks live above
 * each public function.
 */
/* POSIX / GNU declarations (strdup, clock_gettime, localtime_r, ...) must be
   requested before the first #include, so that strict -std=c17 builds
   (CMAKE_C_EXTENSIONS=OFF) see them as well. */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "serial_port.h"

#if defined(_WIN32) || defined(_WIN64)
/* Kept out of the public header: setupapi.h needs the full USER API, so
   exposing it broke builds that combine this header with raylib's
   workaround (NOGDI / NOUSER / WIN32_LEAN_AND_MEAN). */
#include <setupapi.h>
#include <tchar.h>

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
/* The port is opened for overlapped I/O. With a synchronous handle Windows
 * serializes every call on it, so the event thread's pending WaitCommEvent
 * blocked serial_write/serial_read/ClearCommError, and serial_close could hang
 * forever on an idle line. This runs one read or write and waits for it, which
 * keeps serial_read/serial_write blocking and honouring COMMTIMEOUTS. */
static BOOL serial_io_wait(HANDLE h, BOOL is_write, void* buf, DWORD len, DWORD* done) {
    OVERLAPPED ov;
    memset(&ov, 0, sizeof(ov));
    ov.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (!ov.hEvent) {
        return FALSE;
    }
    *done = 0;
    BOOL ok = is_write ? WriteFile(h, buf, len, NULL, &ov) : ReadFile(h, buf, len, NULL, &ov);
    if (ok || GetLastError() == ERROR_IO_PENDING) {
        ok = GetOverlappedResult(h, &ov, done, TRUE);
    }
    DWORD err = GetLastError();
    CloseHandle(ov.hEvent);
    SetLastError(err);
    return ok;
}


static DWORD WINAPI SerialEventThread(LPVOID param) {
    SerialPort* port = (SerialPort*)param;
    if (!port || !handle_is_open_win(port->handle)) return 0;

    SerialPortCallbackData* cd = (SerialPortCallbackData*)port->callback_data;
    HANDLE h = (HANDLE)port->handle;
    OVERLAPPED ov;
    memset(&ov, 0, sizeof(ov));
    ov.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (!cd || !ov.hEvent) {
        if (ov.hEvent) {
            CloseHandle(ov.hEvent);
        }
        return 0;
    }
    HANDLE waits[2] = { cd->stop_event, ov.hEvent };

    while (InterlockedCompareExchange(&cd->stop, 0, 0) == 0) {
        DWORD event_mask = 0;   /* written by the driver; valid until the wait ends */
        ResetEvent(ov.hEvent);
        if (!WaitCommEvent(h, &event_mask, &ov)) {
            if (GetLastError() != ERROR_IO_PENDING) {
                SERIAL_LOG("[SerialEventThread]: WaitCommEvent failed (%lu) — exiting", GetLastError());
                break;
            }
            /* Wait for a port event or for serial_close's stop request. */
            DWORD w = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
            DWORD dummy = 0;
            if (w != WAIT_OBJECT_0 + 1) {
                CancelIo(h);
                GetOverlappedResult(h, &ov, &dummy, TRUE);   /* let the cancel finish before ov/event_mask go away */
                break;
            }
            if (!GetOverlappedResult(h, &ov, &dummy, FALSE)) {
                SERIAL_LOG("[SerialEventThread]: WaitCommEvent completion failed (%lu) — exiting", GetLastError());
                break;
            }
        }
        if (event_mask != 0 && cd->callback && InterlockedCompareExchange(&cd->stop, 0, 0) == 0) {
            cd->callback((int)event_mask, cd->user_data);
        }
    }
    CloseHandle(ov.hEvent);
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

        /* "/dev/" + any d_name fits; a path too long for the name field is
           skipped (a truncated device path would open the wrong file). */
        char full[sizeof("/dev/") + sizeof(entry->d_name)];
        snprintf(full, sizeof(full), "/dev/%s", entry->d_name);
        if (strlen(full) >= sizeof((*ports)[0].name)) continue;

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

        memcpy((*ports)[idx].name, full, strlen(full) + 1);   /* fits: checked above */
        snprintf((*ports)[idx].description, sizeof((*ports)[idx].description), "Serial Port %.*s",
                 (int)(sizeof((*ports)[idx].description) - sizeof("Serial Port ")), entry->d_name);
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
    /* A bare "COMn" is opened as "\\.\COMn": CreateFile only maps COM1..COM9
       without the device prefix, so COM10 and above (as returned by
       serial_list_ports) failed to open. The prefix is valid for COM1..9
       too. Names in any other form are used unchanged. */
    char device[sizeof(port->port_name) + 4];
    const char* open_name = port_name;
    if ((port_name[0] == 'C' || port_name[0] == 'c') && (port_name[1] == 'O' || port_name[1] == 'o') &&
        (port_name[2] == 'M' || port_name[2] == 'm') && port_name[3] >= '0' && port_name[3] <= '9') {
        size_t k = 4;
        while (port_name[k] >= '0' && port_name[k] <= '9') {
            k++;
        }
        if (port_name[k] == '\0' && k < sizeof(port->port_name)) {
            snprintf(device, sizeof(device), "\\\\.\\%s", port_name);
            open_name = device;
        }
    }

    /* Overlapped, so the event thread's WaitCommEvent does not serialize
       (and block) every other call on the handle; see serial_io_wait. */
    HANDLE h = CreateFileA(open_name, GENERIC_READ | GENERIC_WRITE,
                           0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        SERIAL_LOG("[serial_open]: CreateFileA(%s) failed (%lu)", open_name, GetLastError());
        return -1;
    }
    port->handle = h;

    /* Start with the documented blocking read (return once at least one byte
       arrived), like VMIN=1/VTIME=0 on POSIX. The handle otherwise kept the
       driver's previous COMMTIMEOUTS, often all zero, which makes ReadFile
       wait until the WHOLE buffer is filled. Not every device accepts
       timeouts (a pipe does not), so failure here is not fatal. */
    if (serial_set_timeouts(port, 0, 0) != 0) {
        SERIAL_LOG("[serial_open]: SetCommTimeouts not supported by %s (%lu)", open_name, GetLastError());
    }
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
    if (fd == 0) {
        /* A zeroed SerialPort (handle 0) means "closed", so a process with
           stdin closed got a port that serial_is_open rejected (and leaked).
           Move it above the standard descriptors. */
        int moved = fcntl(fd, F_DUPFD, 3);
        close(fd);
        if (moved < 0) {
            return -1;
        }
        fd = moved;
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
    /* Ask the event thread to exit first, then close the handle. */
    SerialPortCallbackData* cd = (SerialPortCallbackData*)port->callback_data;
    if (cd) {
        /* Signal the stop event (the thread waits on it next to its
         * overlapped WaitCommEvent, so it wakes even on an idle line), then
         * JOIN the thread before freeing cd. */
        InterlockedExchange(&cd->stop, 1);
        if (cd->stop_event) {
            SetEvent(cd->stop_event);
        }
        SetCommMask((HANDLE)port->handle, 0);
        if (cd->thread) {
            WaitForSingleObject(cd->thread, INFINITE);
            CloseHandle(cd->thread);
            cd->thread = NULL;
        }
        if (cd->stop_event) {
            CloseHandle(cd->stop_event);
            cd->stop_event = NULL;
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
/* Every rate termios can name (the table used to stop at 230400, so Linux's
   B460800 .. B4000000 failed to configure). */
static const struct {
    int     rate;
    speed_t code;
} serial_baud_table[] = {
    {50, B50}, {75, B75}, {110, B110}, {134, B134}, {150, B150}, {200, B200}, {300, B300},
    {600, B600}, {1200, B1200}, {1800, B1800}, {2400, B2400}, {4800, B4800}, {9600, B9600},
    {19200, B19200}, {38400, B38400}, {57600, B57600}, {115200, B115200}, {230400, B230400},
#ifdef B460800
    {460800, B460800},
#endif
#ifdef B500000
    {500000, B500000},
#endif
#ifdef B576000
    {576000, B576000},
#endif
#ifdef B921600
    {921600, B921600},
#endif
#ifdef B1000000
    {1000000, B1000000},
#endif
#ifdef B1152000
    {1152000, B1152000},
#endif
#ifdef B1500000
    {1500000, B1500000},
#endif
#ifdef B2000000
    {2000000, B2000000},
#endif
#ifdef B2500000
    {2500000, B2500000},
#endif
#ifdef B3000000
    {3000000, B3000000},
#endif
#ifdef B3500000
    {3500000, B3500000},
#endif
#ifdef B4000000
    {4000000, B4000000},
#endif
};

static speed_t get_baud_rate_constant(int baud_rate) {
    for (size_t i = 0; i < sizeof(serial_baud_table) / sizeof(serial_baud_table[0]); i++) {
        if (serial_baud_table[i].rate == baud_rate) {
            return serial_baud_table[i].code;
        }
    }
    return 0;
}

/* The inverse: a termios B* constant to its baud rate, 0 if unknown. */
static int get_baud_rate_value(speed_t code) {
    for (size_t i = 0; i < sizeof(serial_baud_table) / sizeof(serial_baud_table[0]); i++) {
        if (serial_baud_table[i].code == code) {
            return serial_baud_table[i].rate;
        }
    }
    return 0;
}

/* Character size from c_cflag (CS5..CS8; everything but CS7 used to be
   reported as 8). */
static int get_data_bits_value(tcflag_t cflag) {
    switch (cflag & CSIZE) {
        case CS5: return 5;
        case CS6: return 6;
        case CS7: return 7;
        default:  return 8;
    }
}
#endif


/**
 * @brief Apply a configuration to an open port.
 *
 * @return 0 on success, -1 on bad arg / OS failure / unsupported
 *         settings: data_bits outside 5..8, an unknown parity, stop-bits or
 *         flow-control value, a baud rate <= 0, or (POSIX) a rate termios
 *         cannot name. Nothing is substituted silently (data_bits 9 used to
 *         become 8 on POSIX).
 */
int serial_configure(SerialPort* port, const SerialConfig* config) {
    if (!port || !config || !serial_is_open(port)) {
        SERIAL_LOG("[serial_configure]: bad arg or port closed");
        return -1;
    }
    if (config->baud_rate <= 0 || config->data_bits < 5 || config->data_bits > 8 ||
        (config->parity != SERIAL_PARITY_NONE && config->parity != SERIAL_PARITY_ODD && config->parity != SERIAL_PARITY_EVEN) ||
        (config->stop_bits != SERIAL_STOP_BITS_ONE && config->stop_bits != SERIAL_STOP_BITS_TWO) ||
        (config->flow_control != SERIAL_FLOW_CONTROL_NONE && config->flow_control != SERIAL_FLOW_CONTROL_HARDWARE &&
         config->flow_control != SERIAL_FLOW_CONTROL_SOFTWARE)) {
        SERIAL_LOG("[serial_configure]: unsupported configuration");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    DCB dcb = {0};
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState((HANDLE)port->handle, &dcb)) {
        SERIAL_LOG("[serial_configure]: GetCommState failed (%lu)", GetLastError());
        return -1;
    }
    dcb.BaudRate = (DWORD)config->baud_rate;
    dcb.ByteSize = (BYTE)config->data_bits;
    dcb.Parity   = (BYTE)(config->parity == SERIAL_PARITY_ODD ? ODDPARITY :
                          config->parity == SERIAL_PARITY_EVEN ? EVENPARITY : NOPARITY);
    dcb.fParity  = (config->parity != SERIAL_PARITY_NONE);   /* check parity on receive (was never set) */
    dcb.fBinary  = TRUE;
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
        default:   /* only 8 gets here: other values were rejected above */
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
    /* cfgetispeed returns a B* code (B9600 is 13 on Linux), not a rate. */
    config->baud_rate = get_baud_rate_value(cfgetispeed(&options));
    config->data_bits = get_data_bits_value(options.c_cflag);
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
    if (size > MAXDWORD) {
        size = MAXDWORD;
    }
    if (!serial_io_wait((HANDLE)port->handle, TRUE, (void*)data, (DWORD)size, &written)) {
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
    if (size > MAXDWORD) {
        size = MAXDWORD;
    }
    if (!serial_io_wait((HANDLE)port->handle, FALSE, buffer, (DWORD)size, &got)) {
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
    if (!serial_io_wait((HANDLE)port->handle, FALSE, buffer, want, &got)) {
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
 * Windows: spawns a background thread that waits in an overlapped
 * WaitCommEvent; reads, writes and serial_close keep working while it waits.
 * Only one callback can be registered per open port.
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
    if (port->callback_data) {
        /* A second thread would race the first for the same events, and the
           first one's data would leak. */
        SERIAL_LOG("[serial_set_event_callback]: a callback is already registered");
        return -1;
    }
    SerialPortCallbackData* cd = (SerialPortCallbackData*)calloc(1, sizeof(*cd));
    if (!cd) return -1;
    cd->callback  = callback;
    cd->user_data = user_data;
    cd->stop      = 0;
    cd->stop_event = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (!cd->stop_event) {
        free(cd);
        return -1;
    }
    port->callback_data = cd;

    if (!SetCommMask((HANDLE)port->handle, EV_RXCHAR | EV_ERR)) {
        SERIAL_LOG("[serial_set_event_callback]: SetCommMask failed (%lu)", GetLastError());
        CloseHandle(cd->stop_event);
        free(cd);
        port->callback_data = NULL;
        return -1;
    }

    HANDLE t = CreateThread(NULL, 0, SerialEventThread, port, 0, NULL);
    if (!t) {
        SERIAL_LOG("[serial_set_event_callback]: CreateThread failed (%lu)", GetLastError());
        CloseHandle(cd->stop_event);
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
 * the kernel drains its buffer.) VTIME counts tenths of a second up to
 * 255, so on POSIX read_ms above 25500 is rejected (it used to be cut
 * to 25.5 s silently).
 *
 * @return 0 on success, -1 on error.
 */
int serial_set_timeouts(SerialPort* port, int read_ms, int write_ms) {
    if (!port || !serial_is_open(port) || read_ms < 0 || write_ms < 0) return -1;
#if defined(_WIN32) || defined(_WIN64)
    /* ReadIntervalTimeout = ReadTotalTimeoutMultiplier = MAXDWORD with a
       constant 0 < c < MAXDWORD is the documented "return as soon as one
       byte is available, or after c ms with nothing" mode (read_ms == 0
       uses c = MAXDWORD - 1, about 49.7 days, as "forever"). All-zero fields
       (used before for read_ms == 0) instead wait for the whole buffer, so a
       read of 127 bytes never returned a 19-byte reply. */
    COMMTIMEOUTS t = {0};
    t.ReadIntervalTimeout         = MAXDWORD;
    t.ReadTotalTimeoutMultiplier  = MAXDWORD;
    t.ReadTotalTimeoutConstant    = (read_ms == 0) ? MAXDWORD - 1 : (DWORD)read_ms;
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
    if (read_ms > 25500) {
        SERIAL_LOG("[serial_set_timeouts]: read_ms %d exceeds the termios limit of 25500", read_ms);
        return -1;
    }
    int tenths = (read_ms + 99) / 100;
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
