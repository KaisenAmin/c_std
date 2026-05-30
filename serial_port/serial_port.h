/**
 * @author Amin Tahmasebi
 * @class SerialPort
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in serial_port.c (file-level convention).
 *
 * Cross-platform serial-port wrapper for Windows (Win32 CreateFileA +
 * setupapi enumeration) and POSIX (termios).
 */

#ifndef SERIAL_PORT_H_
#define SERIAL_PORT_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <setupapi.h>
#include <tchar.h>
#else
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif



/* #define SERIAL_LOGGING_ENABLE */

#ifdef SERIAL_LOGGING_ENABLE
    #define SERIAL_LOG(fmt, ...) \
        do { fprintf(stderr, "[SERIAL LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define SERIAL_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Public types                                                       */
/* ------------------------------------------------------------------ */

/* User callback invoked from the event thread on RX / TX / line-state
 * changes. `event` is a bitmask; `user_data` is the cookie passed to
 * `serial_set_event_callback`. */
typedef void (*SerialEventCallback)(int event, void* user_data);

/* Parity options. */
typedef enum {
    SERIAL_PARITY_NONE,
    SERIAL_PARITY_ODD,
    SERIAL_PARITY_EVEN
} SerialParity;


/* Hardware / software flow-control options. */
typedef enum {
    SERIAL_FLOW_CONTROL_NONE,
    SERIAL_FLOW_CONTROL_HARDWARE,
    SERIAL_FLOW_CONTROL_SOFTWARE
} SerialFlowControl;


/* Stop-bit options. */
typedef enum {
    SERIAL_STOP_BITS_ONE,
    SERIAL_STOP_BITS_TWO
} SerialBitOption;


/* Full port configuration. */
typedef struct {
    int               baud_rate;
    int               data_bits;
    SerialParity      parity;
    SerialBitOption   stop_bits;
    SerialFlowControl flow_control;
} SerialConfig;


/* Internal: callback + user_data captured for the Windows event thread.
 * Exposed here only because `SerialPort.callback_data` carries it. */
typedef struct {
    SerialEventCallback callback;
    void*               user_data;
#if defined(_WIN32) || defined(_WIN64)
    volatile LONG       stop;     /* non-zero asks the event thread to exit */
#endif
} SerialPortCallbackData;


/* A handle on an open serial port. Zero-initialise before use. */
typedef struct {
    char         port_name[256];
#if defined(_WIN32) || defined(_WIN64)
    HANDLE       handle;
#else
    int          handle;
#endif
    SerialConfig config;
    void*        callback_data;
} SerialPort;


/* Information about a discovered serial port. */
typedef struct {
    char name[256];
    char description[256];
    char hardware_id[256];
} SerialPortInformation;


/* ------------------------------------------------------------------ */
/* Lifecycle                                                          */
/* ------------------------------------------------------------------ */

int   serial_init                     (void);
int   serial_cleanup                  (void);


/* ------------------------------------------------------------------ */
/* Enumeration                                                        */
/* ------------------------------------------------------------------ */

int   serial_list_ports               (SerialPortInformation** ports, size_t* count);
void  serial_free_ports               (SerialPortInformation* ports);


/* ------------------------------------------------------------------ */
/* Open / close                                                       */
/* ------------------------------------------------------------------ */

int   serial_open                     (const char* port_name, SerialPort* port);
int   serial_close                    (SerialPort* port);
bool  serial_is_open                  (const SerialPort* port);


/* ------------------------------------------------------------------ */
/* Configuration                                                      */
/* ------------------------------------------------------------------ */

int   serial_configure                (SerialPort* port, const SerialConfig* config);
int   serial_get_config               (SerialPort* port, SerialConfig* config);
int   serial_set_timeouts             (SerialPort* port, int read_ms, int write_ms);


/* ------------------------------------------------------------------ */
/* I/O                                                                */
/* ------------------------------------------------------------------ */

int   serial_write                    (SerialPort* port, const void* data, size_t size);
int   serial_read                     (SerialPort* port, void* buffer, size_t size);
int   serial_read_nonblocking         (SerialPort* port, void* buffer, size_t size);
int   serial_bytes_available          (SerialPort* port);
int   serial_flush_input              (SerialPort* port);
int   serial_flush_output             (SerialPort* port);


/* ------------------------------------------------------------------ */
/* Events                                                             */
/* ------------------------------------------------------------------ */

int   serial_set_event_callback       (SerialPort* port,
                                       SerialEventCallback callback,
                                       void* user_data);


#ifdef __cplusplus
}
#endif

#endif 