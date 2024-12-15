#ifndef SERIAL_PORT_H_
#define SERIAL_PORT_H_

#include <stddef.h> 
#include <stdint.h> 
#include <stdio.h>

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

#define SERIAL_LOGGING_ENABLE

#ifdef SERIAL_LOGGING_ENABLE
    #define SERIAL_LOG(fmt, ...) \
        do { fprintf(stderr, "[SERIAL LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define SERIAL_LOG(fmt, ...) do { } while (0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*SerialEventCallback)(int event, void* user_data);

// Serial port parity options.
typedef enum {
    SERIAL_PARITY_NONE,
    SERIAL_PARITY_ODD,
    SERIAL_PARITY_EVEN
} SerialParity;

// Serial port flow control options.
typedef enum {
    SERIAL_FLOW_CONTROL_NONE,
    SERIAL_FLOW_CONTROL_HARDWARE,
    SERIAL_FLOW_CONTROL_SOFTWARE
} SerialFlowControl;

// Serial port stop bit options.
typedef enum {
    SERIAL_STOP_BITS_ONE,
    SERIAL_STOP_BITS_TWO
} SerialBitOption;

// Structure to hold serial port configuration.
typedef struct {
    int baud_rate;                 
    int data_bits;                
    SerialParity parity;        
    SerialBitOption stop_bits;  
    SerialFlowControl flow_control; 
} SerialConfig;

// Callable Port Event
typedef struct {
    SerialEventCallback callback;
    void* user_data;
} SerialPortCallbackData;

// Structure to represent a serial port.
typedef struct {
    char port_name[256];           
#if defined(_WIN32) || defined(_WIN64)
    HANDLE handle;                 
#else
    int handle;                    
#endif
    SerialConfig config;           
    void* callback_data;
} SerialPort;

// Structure to hold information about available serial ports.
typedef struct {
    char name[256];        
    char description[256];  
    char hardware_id[256];  
} SerialPortInformation;


void serial_free_ports(SerialPortInformation* ports);

int serial_init();
int serial_cleanup();
int serial_list_ports(SerialPortInformation** ports, size_t* count);
int serial_open(const char* port_name, SerialPort* port);
int serial_close(SerialPort* port);
int serial_configure(SerialPort* port, const SerialConfig* config);
int serial_get_config(SerialPort* port, SerialConfig* config);
int serial_write(SerialPort* port, const void* data, size_t size);
int serial_read(SerialPort* port, void* buffer, size_t size);
int serial_read_nonblocking(SerialPort* port, void* buffer, size_t size);
int serial_set_event_callback(SerialPort* port, SerialEventCallback callback, void* user_data);


#ifdef __cplusplus
}
#endif

#endif 
