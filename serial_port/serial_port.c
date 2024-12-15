#include "serial_port.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>


static int serial_initialized = 0;


#if defined(_WIN32) || defined(_WIN64)
DWORD WINAPI SerialEventThread(LPVOID param) {
    SerialPort* port = (SerialPort*)param;
    if (!port || port->handle == 0) return 0;

    SerialPortCallbackData* callback_data = (SerialPortCallbackData*)port->callback_data;
    while (1) {
        DWORD event_mask = 0;
        if (!WaitCommEvent((HANDLE)port->handle, &event_mask, NULL)) {
            SERIAL_LOG("[serial_set_event_callback]: Error - WaitCommEvent failed. Error Code: %lu", GetLastError());
            break;
        }
        if (callback_data && callback_data->callback) {
            callback_data->callback(event_mask, callback_data->user_data);
        }
    }
    return 0;
}
#endif

int serial_set_event_callback(SerialPort* port, SerialEventCallback callback, void* user_data) {
    SERIAL_LOG("[serial_set_event_callback]: Setting event callback for port '%s'.", port->port_name);

    if (!port || port->handle == 0 || !callback) {
        SERIAL_LOG("[serial_set_event_callback]: Error - Invalid arguments.");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    SerialPortCallbackData* callback_data = (SerialPortCallbackData*)malloc(sizeof(SerialPortCallbackData));
    if (!callback_data) {
        SERIAL_LOG("[serial_set_event_callback]: Error - Memory allocation failed.");
        return -1;
    }
    callback_data->callback = callback;
    callback_data->user_data = user_data;
    port->callback_data = callback_data;

    if (!SetCommMask((HANDLE)port->handle, EV_RXCHAR | EV_ERR)) {
        SERIAL_LOG("[serial_set_event_callback]: Error - Failed to set comm mask. Error Code: %lu", GetLastError());
        free(callback_data);
        port->callback_data = NULL;
        return -1;
    }

    HANDLE thread_handle = CreateThread(NULL, 0, SerialEventThread, port, 0, NULL);
    if (!thread_handle) {
        SERIAL_LOG("[serial_set_event_callback]: Error - Failed to create event thread. Error Code: %lu", GetLastError());
        free(callback_data);
        port->callback_data = NULL;
        return -1;
    }
    CloseHandle(thread_handle);

#else
    // POSIX-specific implementation
    // For simplicity, no implementation provided here as POSIX platforms usually use select/poll/epoll instead.
    SERIAL_LOG("[serial_set_event_callback]: Error - Not implemented for POSIX.");
    return -1;
#endif

    SERIAL_LOG("[serial_set_event_callback]: Event callback set successfully for port '%s'.", port->port_name);
    return 0;
}

int serial_init() {
    SERIAL_LOG("[serial_init]: Initializing the serial port library.");

    if (serial_initialized) {
        SERIAL_LOG("[serial_init]: Error - Serial port library is already initialized.");
        return -1; 
    }

    #if defined(_WIN32) || defined(_WIN64)
        SERIAL_LOG("[serial_init]: Windows-specific initialization not required.");
    #elif defined(__linux__)
        SERIAL_LOG("[serial_init]: Linux-specific initialization not required.");
    #else
        SERIAL_LOG("[serial_init]: Error - Unsupported platform.");
        return -1;
    #endif

    serial_initialized = 1;
    SERIAL_LOG("[serial_init]: Serial port library initialized successfully.");
    return 0; // Success
}

int serial_cleanup() {
    SERIAL_LOG("[serial_cleanup]: Cleaning up the serial port library.");

    if (!serial_initialized) {
        SERIAL_LOG("[serial_cleanup]: Error - Serial port library is not initialized.");
        return -1; 
    }
    #if defined(_WIN32) || defined(_WIN64)
        SERIAL_LOG("[serial_cleanup]: Windows-specific cleanup not required.");
    #elif defined(__linux__)
        SERIAL_LOG("[serial_cleanup]: Linux-specific cleanup not required.");
    #else
        SERIAL_LOG("[serial_cleanup]: Error - Unsupported platform.");
        return -1;
    #endif

    serial_initialized = 0;
    SERIAL_LOG("[serial_cleanup]: Serial port library cleaned up successfully.");

    return 0; 
}

int serial_open(const char* port_name, SerialPort* port) {
    SERIAL_LOG("[serial_open]: Attempting to open port '%s'.", port_name);

    if (!port_name || !port) {
        SERIAL_LOG("[serial_open]: Error - Invalid arguments (port_name or port is NULL).");
        return -1;
    }

    memset(port, 0, sizeof(SerialPort)); 
    strncpy(port->port_name, port_name, sizeof(port->port_name) - 1);

#if defined(_WIN32) || defined(_WIN64)
    HANDLE handle = CreateFileA(port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (handle == INVALID_HANDLE_VALUE) {
        #if defined(_WIN32) || defined(_WIN64)
        SERIAL_LOG("[serial_open]: Error Code: %lu", (unsigned long)GetLastError());
        #endif
        return -1;
    }

    port->handle = handle;
    SERIAL_LOG("[serial_open]: Port '%s' opened successfully on Windows.", port_name);

#else
    int fd = open(port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        SERIAL_LOG("[serial_open]: Error - Failed to open port '%s'. Error: %s", port_name, strerror(errno));
        return -1;
    }

    struct termios options;
    if (tcgetattr(fd, &options) < 0) {
        SERIAL_LOG("[serial_open]: Error - Failed to get port attributes for '%s'. Error: %s", port_name, strerror(errno));
        close(fd);
        return -1;
    }

    cfmakeraw(&options); 
    options.c_cflag |= (CLOCAL | CREAD); 

    if (tcsetattr(fd, TCSANOW, &options) < 0) {
        SERIAL_LOG("[serial_open]: Error - Failed to set port attributes for '%s'. Error: %s", port_name, strerror(errno));
        close(fd);
        return -1;
    }

    port->handle = fd;
    SERIAL_LOG("[serial_open]: Port '%s' opened successfully on POSIX.", port_name);
#endif

    return 0; 
}

int serial_close(SerialPort* port) {
    SERIAL_LOG("[serial_close]: Attempting to close port '%s'.", port->port_name);

    if (!port || port->handle == 0) {
        SERIAL_LOG("[serial_close]: Error - Invalid port or port is not open.");
        return -1; 
    }

#if defined(_WIN32) || defined(_WIN64)
    if (!CloseHandle(port->handle)) {
        SERIAL_LOG("[serial_close]: Error - Failed to close port '%s'. Error Code: %lu", port->port_name, (unsigned long)GetLastError());
        return -1;
    }

    SERIAL_LOG("[serial_close]: Port '%s' closed successfully on Windows.", port->port_name);
#else
    if (close(port->handle) < 0) {
        SERIAL_LOG("[serial_close]: Error - Failed to close port '%s'. Error: %s", port->port_name, strerror(errno));
        return -1;
    }

    SERIAL_LOG("[serial_close]: Port '%s' closed successfully on POSIX.", port->port_name);
#endif

    port->handle = 0; 
    return 0; 
}

/**
 * @brief List all available serial ports.
 *
 * @param[out] ports Pointer to an array of SerialPortInformation structures.
 * @param[out] count Pointer to store the number of available ports.
 * @return 0 on success, non-zero on failure.
 */
int serial_list_ports(SerialPortInformation** ports, size_t* count) {
    SERIAL_LOG("[serial_list_ports]: Listing all available serial ports.");

    if (!ports || !count) {
        SERIAL_LOG("[serial_list_ports]: Error - Invalid arguments (ports or count is NULL).");
        return -1; 
    }

    *ports = NULL;
    *count = 0;

#if defined(_WIN32) || defined(_WIN64)
    HDEVINFO hDevInfo = SetupDiGetClassDevsA(
        (const GUID*)&GUID_DEVINTERFACE_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        SERIAL_LOG("[serial_list_ports]: Error - Failed to get device list. Error Code: %lu", GetLastError());
        return -1;
    }

    SP_DEVINFO_DATA devInfoData = {0};
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    size_t allocated_ports = 10;
    *ports = (SerialPortInformation*)malloc(sizeof(SerialPortInformation) * allocated_ports);
    if (!*ports) {
        SERIAL_LOG("[serial_list_ports]: Error - Memory allocation failed.");
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return -1;
    }

    size_t port_index = 0;

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); i++) {
        char friendly_name[256] = {0};
        if (!SetupDiGetDeviceRegistryPropertyA(
                hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME,
                NULL, (BYTE*)friendly_name, sizeof(friendly_name), NULL)) {
            SERIAL_LOG("[serial_list_ports]: Warning - Failed to get friendly name. Skipping.");
            continue;
        }

        SERIAL_LOG("[serial_list_ports]: Friendly name detected: %s.", friendly_name);
        char* com_pos = strstr(friendly_name, "COM");

        if (!com_pos) {
            SERIAL_LOG("[serial_list_ports]: Skipping non-COM port: %s.", friendly_name);
            continue;
        }

        char com_name[16] = {0}; 
        size_t j = 0;

        while (com_pos[j] && j < sizeof(com_name) - 1) {
            if (com_pos[j] == ')' || com_pos[j] == ' ') break;
            com_name[j] = com_pos[j];
            j++;
        }
        com_name[j] = '\0'; 
        SERIAL_LOG("[serial_list_ports]: Valid COM port found: %s.", com_name);

        if (port_index >= allocated_ports) {
            allocated_ports *= 2;
            SerialPortInformation* new_ports = (SerialPortInformation*)realloc(*ports, sizeof(SerialPortInformation) * allocated_ports);
            if (!new_ports) {
                SERIAL_LOG("[serial_list_ports]: Error - Memory reallocation failed.");
                free(*ports);
                *ports = NULL;
                *count = 0;
                SetupDiDestroyDeviceInfoList(hDevInfo);
                return -1;
            }
            *ports = new_ports;
        }
        snprintf((*ports)[port_index].name, sizeof((*ports)[port_index].name), "%s", com_name);
        snprintf((*ports)[port_index].description, sizeof((*ports)[port_index].description), "%s", friendly_name);
        
        port_index++;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);

    *count = port_index;
    SERIAL_LOG("[serial_list_ports]: Found %zu serial ports.", *count);
    return 0;

#else
    DIR* dev_dir = opendir("/dev");
    if (!dev_dir) {
        SERIAL_LOG("[serial_list_ports]: Error - Unable to open /dev directory. Error: %s", strerror(errno));
        return -1;
    }

    size_t allocated_ports = 10;
    *ports = (SerialPortInformation*)malloc(sizeof(SerialPortInformation) * allocated_ports);
    if (!*ports) {
        SERIAL_LOG("[serial_list_ports]: Error - Memory allocation failed.");
        closedir(dev_dir);
        return -1;
    }

    struct dirent* entry;
    size_t port_index = 0;

    while ((entry = readdir(dev_dir)) != NULL) {
        if (strncmp(entry->d_name, "ttyS", 4) == 0 || // Built-in serial ports
            strncmp(entry->d_name, "ttyUSB", 6) == 0 || // USB-to-serial converters
            strncmp(entry->d_name, "ttyACM", 6) == 0) { // Modem interfaces

            char full_path[256] = {0};
            snprintf(full_path, sizeof(full_path), "/dev/%s", entry->d_name);

            struct stat stat_buf;
            if (stat(full_path, &stat_buf) == 0 && S_ISCHR(stat_buf.st_mode)) {
                SERIAL_LOG("[serial_list_ports]: Found serial port: %s", full_path);

                if (port_index >= allocated_ports) {
                    allocated_ports *= 2;
                    SerialPortInformation* new_ports = (SerialPortInformation*)realloc(*ports, sizeof(SerialPortInformation) * allocated_ports);
                    if (!new_ports) {
                        SERIAL_LOG("[serial_list_ports]: Error - Memory reallocation failed.");
                        free(*ports);
                        *ports = NULL;
                        *count = 0;
                        closedir(dev_dir);
                        return -1;
                    }
                    *ports = new_ports;
                }

                snprintf((*ports)[port_index].name, sizeof((*ports)[port_index].name), "%s", full_path);
                snprintf((*ports)[port_index].description, sizeof((*ports)[port_index].description), "Serial Port %s", entry->d_name);
                snprintf((*ports)[port_index].hardware_id, sizeof((*ports)[port_index].hardware_id), "N/A");

                port_index++;
            }
        }
    }
    closedir(dev_dir);

    *count = port_index;
    SERIAL_LOG("[serial_list_ports]: Found %zu serial ports.", *count);
    return 0;
#endif
}

/**
 * @brief Free memory allocated for the port list.
 *
 * @param ports Pointer to the array of SerialPortInformation structures.
 */
void serial_free_ports(SerialPortInformation* ports) {
    SERIAL_LOG("[serial_free_ports]: Freeing memory for serial port list.");
    free(ports);
}

/**
 * @brief Configure a serial port.
 *
 * @param port Pointer to the SerialPort structure representing the port.
 * @param config Pointer to the SerialConfig structure with the desired configuration.
 * @return 0 on success, non-zero on failure.
 */
int serial_configure(SerialPort* port, const SerialConfig* config) {
    SERIAL_LOG("[serial_configure]: Configuring serial port '%s'.", port->port_name);

    if (!port || !config) {
        SERIAL_LOG("[serial_configure]: Error - Invalid arguments (port or config is NULL).");
        return -1; 
    }

#if defined(_WIN32) || defined(_WIN64)
    DCB dcb = {0};
    dcb.DCBlength = sizeof(DCB);

    if (!GetCommState((HANDLE)port->handle, &dcb)) {
        SERIAL_LOG("[serial_configure]: Error - Failed to get current COM state. Error Code: %lu", GetLastError());
        return -1;
    }

    dcb.BaudRate = config->baud_rate;
    dcb.ByteSize = (BYTE)config->data_bits;
    dcb.Parity = (BYTE)config->parity;
    dcb.StopBits = (BYTE)(config->stop_bits == SERIAL_STOP_BITS_ONE ? ONESTOPBIT : TWOSTOPBITS);

    if (config->flow_control == SERIAL_FLOW_CONTROL_HARDWARE) {
        dcb.fOutxCtsFlow = TRUE;
        dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
    } 
    else if (config->flow_control == SERIAL_FLOW_CONTROL_SOFTWARE) {
        dcb.fInX = TRUE;
        dcb.fOutX = TRUE;
    } 
    else {
        dcb.fOutxCtsFlow = FALSE;
        dcb.fRtsControl = RTS_CONTROL_DISABLE;
        dcb.fInX = FALSE;
        dcb.fOutX = FALSE;
    }

    if (!SetCommState((HANDLE)port->handle, &dcb)) {
        SERIAL_LOG("[serial_configure]: Error - Failed to set COM state. Error Code: %lu", (unsigned long)GetLastError());
        SERIAL_LOG("[serial_configure]: Attempted Settings - BaudRate: %d, ByteSize: %d, Parity: %d, StopBits: %d",
                (int)dcb.BaudRate, (int)dcb.ByteSize, (int)dcb.Parity, dcb.StopBits);
        return -1; 
    }

    SERIAL_LOG("[serial_configure]: Serial port '%s' configured successfully.", port->port_name);

#else
    struct termios options;
    if (tcgetattr(port->handle, &options) < 0) {
        SERIAL_LOG("[serial_configure]: Error - Failed to get current port attributes. Error: %s", strerror(errno));
        return -1;
    }

    cfsetispeed(&options, config->baud_rate);
    cfsetospeed(&options, config->baud_rate);

    options.c_cflag &= ~CSIZE;
    options.c_cflag |= (config->data_bits == 7 ? CS7 : CS8);

    options.c_cflag &= ~(PARENB | PARODD);
    if (config->parity == SERIAL_PARITY_ODD) {
        options.c_cflag |= PARENB | PARODD;
    } 
    else if (config->parity == SERIAL_PARITY_EVEN) {
        options.c_cflag |= PARENB;
    }

    options.c_cflag &= ~CSTOPB;
    if (config->stop_bits == SERIAL_STOP_BITS_TWO) {
        options.c_cflag |= CSTOPB;
    }

    if (config->flow_control == SERIAL_FLOW_CONTROL_HARDWARE) {
        options.c_cflag |= CRTSCTS;
    } 
    else {
        options.c_cflag &= ~CRTSCTS;
    }

    if (config->flow_control == SERIAL_FLOW_CONTROL_SOFTWARE) {
        options.c_iflag |= (IXON | IXOFF);
    } 
    else {
        options.c_iflag &= ~(IXON | IXOFF);
    }
    options.c_cflag |= (CLOCAL | CREAD);

    if (tcsetattr(port->handle, TCSANOW, &options) < 0) {
        SERIAL_LOG("[serial_configure]: Error - Failed to set port attributes. Error: %s", strerror(errno));
        return -1;
    }

    SERIAL_LOG("[serial_configure]: Serial port '%s' configured successfully.", port->port_name);
#endif

    return 0; 
}

/**
 * @brief Get the current configuration of a serial port.
 *
 * @param port Pointer to the SerialPort structure representing the port.
 * @param[out] config Pointer to a SerialConfig structure to populate.
 * @return 0 on success, non-zero on failure.
 */
int serial_get_config(SerialPort* port, SerialConfig* config) {
    SERIAL_LOG("[serial_get_config]: Retrieving configuration for serial port '%s'.", port->port_name);

    if (!port || !config) {
        SERIAL_LOG("[serial_get_config]: Error - Invalid arguments (port or config is NULL).");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    DCB dcb = {0};
    dcb.DCBlength = sizeof(DCB);

    if (!GetCommState((HANDLE)port->handle, &dcb)) {
        SERIAL_LOG("[serial_get_config]: Error - Failed to get COM state. Error Code: %lu", GetLastError());
        return -1;
    }

    config->baud_rate = dcb.BaudRate;
    config->data_bits = dcb.ByteSize;
    config->parity = dcb.Parity == NOPARITY ? SERIAL_PARITY_NONE : dcb.Parity == ODDPARITY ? SERIAL_PARITY_ODD : SERIAL_PARITY_EVEN;
    config->stop_bits = dcb.StopBits == ONESTOPBIT ? SERIAL_STOP_BITS_ONE : SERIAL_STOP_BITS_TWO;
    config->flow_control = dcb.fOutxCtsFlow ? SERIAL_FLOW_CONTROL_HARDWARE : SERIAL_FLOW_CONTROL_NONE;

    SERIAL_LOG("[serial_get_config]: Retrieved configuration for serial port '%s'.", port->port_name);
#else
    struct termios options;

    if (tcgetattr(port->handle, &options) < 0) {
        SERIAL_LOG("[serial_get_config]: Error - Failed to get current port attributes. Error: %s", strerror(errno));
        return -1;
    }

    config->baud_rate = cfgetispeed(&options);
    config->data_bits = (options.c_cflag & CSIZE) == CS7 ? 7 : 8;

    if (options.c_cflag & PARENB) {
        config->parity = (options.c_cflag & PARODD) ? SERIAL_PARITY_ODD : SERIAL_PARITY_EVEN;
    } 
    else {
        config->parity = SERIAL_PARITY_NONE;
    }

    config->stop_bits = (options.c_cflag & CSTOPB) ? SERIAL_STOP_BITS_TWO : SERIAL_STOP_BITS_ONE;

    if (options.c_cflag & CRTSCTS) {
        config->flow_control = SERIAL_FLOW_CONTROL_HARDWARE;
    } 
    else if (options.c_iflag & (IXON | IXOFF)) {
        config->flow_control = SERIAL_FLOW_CONTROL_SOFTWARE;
    } 
    else {
        config->flow_control = SERIAL_FLOW_CONTROL_NONE;
    }

    SERIAL_LOG("[serial_get_config]: Retrieved configuration for serial port '%s'.", port->port_name);
#endif

    return 0; 
}

/**
 * @brief Write data to a serial port.
 *
 * @param port Pointer to the SerialPort structure representing the port.
 * @param data Pointer to the data to write.
 * @param size Size of the data to write.
 * @return Number of bytes written, or negative value on failure.
 */
int serial_write(SerialPort* port, const void* data, size_t size) {
    SERIAL_LOG("[serial_write]: Writing data to serial port '%s'.", port->port_name);

    if (!port || !data || size == 0) {
        SERIAL_LOG("[serial_write]: Error - Invalid arguments (port, data, or size is invalid).");
        return -1; 
    }

#if defined(_WIN32) || defined(_WIN64)
    DWORD bytes_written = 0;
    if (!WriteFile((HANDLE)port->handle, data, size, &bytes_written, NULL)) {
        SERIAL_LOG("[serial_write]: Error - Failed to write data to port '%s'. Error Code: %lu", port->port_name, GetLastError());
        return -1; 
    }

    SERIAL_LOG("[serial_write]: Successfully wrote %lu bytes to port '%s'.", bytes_written, port->port_name);
    return (int)bytes_written;
#else
    ssize_t result = write(port->handle, data, size);
    if (result < 0) {
        SERIAL_LOG("[serial_write]: Error - Failed to write data to port '%s'. Error: %s", port->port_name, strerror(errno));
        return -1; 
    }

    SERIAL_LOG("[serial_write]: Successfully wrote %zd bytes to port '%s'.", result, port->port_name);
    return (int)result;
#endif
}

/**
 * @brief Read data from a serial port (blocking).
 *
 * @param port Pointer to the SerialPort structure representing the port.
 * @param buffer Pointer to the buffer to store the read data.
 * @param size Size of the buffer.
 * @return Number of bytes read, or negative value on failure.
 */
int serial_read(SerialPort* port, void* buffer, size_t size) {
    SERIAL_LOG("[serial_read]: Reading data from serial port '%s'.", port->port_name);

    if (!port || !buffer || size == 0) {
        SERIAL_LOG("[serial_read]: Error - Invalid arguments (port, buffer, or size is invalid).");
        return -1; 
    }

#if defined(_WIN32) || defined(_WIN64)
    DWORD bytes_read = 0;
    if (!ReadFile((HANDLE)port->handle, buffer, size, &bytes_read, NULL)) {
        SERIAL_LOG("[serial_read]: Error - Failed to read data from port '%s'. Error Code: %lu", port->port_name, GetLastError());
        return -1; 
    }

    SERIAL_LOG("[serial_read]: Successfully read %lu bytes from port '%s'.", bytes_read, port->port_name);
    return (int)bytes_read;
#else
    ssize_t result = read(port->handle, buffer, size);
    if (result < 0) {
        SERIAL_LOG("[serial_read]: Error - Failed to read data from port '%s'. Error: %s", port->port_name, strerror(errno));
        return -1; 
    }

    SERIAL_LOG("[serial_read]: Successfully read %zd bytes from port '%s'.", result, port->port_name);
    return (int)result;
#endif
}

/**
 * @brief Read data from a serial port (non-blocking).
 *
 * @param port Pointer to the serial_port_t structure representing the port.
 * @param buffer Pointer to the buffer to store the read data.
 * @param size Size of the buffer.
 * 
 * @return Number of bytes read, or negative value on failure.
 */
int serial_read_nonblocking(SerialPort* port, void* buffer, size_t size) {
    SERIAL_LOG("[serial_read_nonblocking]: Attempting to read up to %zu bytes from port '%s'.", size, port->port_name);

    if (!port || port->handle == 0 || !buffer || size == 0) {
        SERIAL_LOG("[serial_read_nonblocking]: Error - Invalid arguments.");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    DWORD bytes_read = 0;
    if (!ReadFile((HANDLE)port->handle, buffer, (DWORD)size, &bytes_read, NULL)) {
        SERIAL_LOG("[serial_read_nonblocking]: Error - Failed to read from port '%s'. Error Code: %lu", port->port_name, GetLastError());
        return -1;
    }
    SERIAL_LOG("[serial_read_nonblocking]: Successfully read %lu bytes from port '%s'.", bytes_read, port->port_name);
    return (int)bytes_read;

#else
    ssize_t result = read(port->handle, buffer, size);
    if (result < 0 && errno != EAGAIN) {
        SERIAL_LOG("[serial_read_nonblocking]: Error - Failed to read from port '%s'. Error: %s", port->port_name, strerror(errno));
        return -1;
    }
    SERIAL_LOG("[serial_read_nonblocking]: Successfully read %zd bytes from port '%s'.", result, port->port_name);
    return (int)result;
#endif
}
