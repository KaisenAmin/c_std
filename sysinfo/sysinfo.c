/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Sysinfo
*/

#include "sysinfo.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <rpc.h>
#include <objbase.h>
#include <bluetoothapis.h>
#include <winternl.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#include <wbemidl.h>


// Define the function pointer type for NtQuerySystemInformation
typedef NTSTATUS (WINAPI *NtQuerySystemInformationFunc)(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

typedef LONG NTSTATUS;
typedef NTSTATUS (WINAPI *RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

static char* get_windows_version() {
    SYSINFO_LOG("[get_windows_version]: Entering function.");
    
    static char version[128];
    DWORD dwVersion = 0;
    DWORD dwMajorVersion = 0;
    DWORD dwMinorVersion = 0;

    dwVersion = GetVersion();
    dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

    if (dwVersion < 0x80000000) {
        sprintf(version, "%ld.%ld", dwMajorVersion, dwMinorVersion);
        SYSINFO_LOG("[get_windows_version]: Detected version: %s", version);
    } 
    else {
        strcpy(version, "unknown");
        SYSINFO_LOG("[get_windows_version]: Unable to detect version, setting to unknown.");
    }

    SYSINFO_LOG("[get_windows_version]: Exiting function.");
    return version;
}

static char* get_windows_product_type() {
    SYSINFO_LOG("[get_windows_product_type]: Returning Windows product type.");
    static char product_type[] = "Windows";
    return product_type;
}

static char* get_windows_kernel_version() {
    SYSINFO_LOG("[get_windows_kernel_version]: Entering function.");

    static char kernel_version[128];
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
        sprintf(kernel_version, "%ld.%ld.%ld", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
        SYSINFO_LOG("[get_windows_kernel_version]: Kernel version: %s", kernel_version);
    } 
    else {
        strcpy(kernel_version, "unknown");
        SYSINFO_LOG("[get_windows_kernel_version]: Failed to retrieve kernel version, setting to unknown.");
    }

    SYSINFO_LOG("[get_windows_kernel_version]: Exiting function.");
    return kernel_version;
}

static char* get_windows_kernel_type() {
    SYSINFO_LOG("[get_windows_kernel_type]: Returning Windows NT kernel type.");
    static char kernel_type[] = "Windows NT";

    return kernel_type;
}

static char* get_windows_boot_unique_id() {
    SYSINFO_LOG("[get_windows_boot_unique_id]: Entering function.");

    static char boot_id[128] = {0};
    ULONGLONG uptime = GetTickCount64();
    sprintf(boot_id, "%llu", uptime);
    
    SYSINFO_LOG("[get_windows_boot_unique_id]: Boot ID: %s", boot_id);
    SYSINFO_LOG("[get_windows_boot_unique_id]: Exiting function.");

    return boot_id;
}

static char* get_windows_cpu_architecture() {
    SYSINFO_LOG("[get_windows_cpu_architecture]: Entering function.");
    
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    static char architecture[128];

    switch (sysInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_AMD64:
            strcpy(architecture, "x86_64");
            break;
        case PROCESSOR_ARCHITECTURE_ARM:
            strcpy(architecture, "ARM");
            break;
        case PROCESSOR_ARCHITECTURE_IA64:
            strcpy(architecture, "IA64");
            break;
        case PROCESSOR_ARCHITECTURE_INTEL:
            strcpy(architecture, "x86");
            break;
        default:
            strcpy(architecture, "unknown");
            break;
    }

    SYSINFO_LOG("[get_windows_cpu_architecture]: Detected CPU architecture: %s", architecture);
    SYSINFO_LOG("[get_windows_cpu_architecture]: Exiting function.");
    
    return architecture;
}

static char* get_windows_machine_host_name() {
    SYSINFO_LOG("[get_windows_machine_host_name]: Entering function.");

    static char hostname[150] = {0};
    TCHAR infoBuf[150];
    DWORD bufCharCount = 150;

    if (GetComputerName(infoBuf, &bufCharCount)) {
        for (int i = 0; i < (int)bufCharCount; i++) {
            hostname[i] = infoBuf[i];
        }
        SYSINFO_LOG("[get_windows_machine_host_name]: Host name: %s", hostname);
    } 
    else {
        strcpy(hostname, "Unknown_Host_Name");
        SYSINFO_LOG("[get_windows_machine_host_name]: Failed to retrieve host name, setting to Unknown_Host_Name.");
    }

    SYSINFO_LOG("[get_windows_machine_host_name]: Exiting function.");
    return hostname;
}

static char* get_windows_machine_unique_id() {
    SYSINFO_LOG("[get_windows_machine_unique_id]: Entering function.");

    static char unique_id[128] = {0};
    HKEY hKey;
    DWORD bufferSize = sizeof(unique_id);
    wchar_t buffer[128];

    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueExW(hKey, L"MachineGuid", NULL, NULL, (LPBYTE)buffer, &bufferSize) == ERROR_SUCCESS) {
            wcstombs(unique_id, buffer, sizeof(unique_id));  // convert wchar_t to char
            SYSINFO_LOG("[get_windows_machine_unique_id]: Retrieved Machine GUID: %s", unique_id);
        }
        RegCloseKey(hKey);
    }

    if (strlen(unique_id) == 0) {
        strcpy(unique_id, "unknown");
        SYSINFO_LOG("[get_windows_machine_unique_id]: Failed to retrieve Machine GUID, setting to unknown.");
    }

    SYSINFO_LOG("[get_windows_machine_unique_id]: Exiting function.");
    return unique_id;
}

static void get_windows_pretty_version(char* buffer, size_t buffer_size) {
    SYSINFO_LOG("[get_windows_pretty_version]: Entering function.");
    
    RTL_OSVERSIONINFOEXW osvi;
    memset(&osvi, 0, sizeof(RTL_OSVERSIONINFOEXW));
    osvi.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
    
    HMODULE hNtdll = GetModuleHandle(TEXT("ntdll.dll"));
    if (hNtdll == NULL) {
        SYSINFO_LOG("[get_windows_pretty_version]: Failed to get handle to ntdll.dll.");
        snprintf(buffer, buffer_size, "Unknown Windows Version");
        return;
    }

    union {
        FARPROC proc;
        RtlGetVersionPtr func;
    } rtlGetVersionUnion;
    
    rtlGetVersionUnion.proc = GetProcAddress(hNtdll, "RtlGetVersion");
    if (rtlGetVersionUnion.proc == NULL) {
        SYSINFO_LOG("[get_windows_pretty_version]: Failed to get RtlGetVersion function address.");
        snprintf(buffer, buffer_size, "Unknown Windows Version");
        return;
    }

    rtlGetVersionUnion.func((PRTL_OSVERSIONINFOW)&osvi);

    const char* productName;
    if (osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0) {
        productName = "Windows 10";
    } 
    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3) {
        productName = "Windows 8.1";
    } 
    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2) {
        productName = "Windows 8";
    } 
    else if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1) {
        productName = "Windows 7";
    } 
    else {
        productName = "Unknown Windows Version";
    }

    snprintf(buffer, buffer_size, "%s Version %lu.%lu (Build %lu)", productName, osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);

    SYSINFO_LOG("[get_windows_pretty_version]: OS Version detected: %s", buffer);
    SYSINFO_LOG("[get_windows_pretty_version]: Exiting function.");
}

static char* get_windows_build_abi() {
    SYSINFO_LOG("[get_windows_build_abi]: Entering function.");
    
    static char build_abi[128];
    memset(build_abi, 0, sizeof(build_abi));

    char* architecture = sysinfo_cpu_architecture();
    if (architecture) {
        strcpy(build_abi, architecture);
        strcat(build_abi, "-");
        SYSINFO_LOG("[get_windows_build_abi]: Architecture found: %s", architecture);
    } 
    else {
        strcpy(build_abi, "unknown-");
        SYSINFO_LOG("[get_windows_build_abi]: Architecture not found, using 'unknown'.");
    }

    strcat(build_abi, "little_endian-");
    strcat(build_abi, "llp64");  // LLP64 for Windows

    SYSINFO_LOG("[get_windows_build_abi]: Final build ABI: %s", build_abi);
    return build_abi;
}

static char** get_sysinfo_list_bluetooth_devices_windows(int* count) {
    SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: Entering function.");
    
    *count = 0;
    char** devices = NULL;

    BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams;
    BLUETOOTH_DEVICE_INFO deviceInfo;
    HBLUETOOTH_DEVICE_FIND hFind;

    memset(&searchParams, 0, sizeof(searchParams));
    searchParams.dwSize = sizeof(searchParams);
    searchParams.fReturnAuthenticated = TRUE;
    searchParams.fReturnRemembered = TRUE;
    searchParams.fReturnConnected = TRUE;
    searchParams.fReturnUnknown = TRUE;
    searchParams.fIssueInquiry = TRUE;
    searchParams.cTimeoutMultiplier = 4;

    deviceInfo.dwSize = sizeof(deviceInfo);

    hFind = BluetoothFindFirstDevice(&searchParams, &deviceInfo);
    if (hFind != NULL) {
        SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: Found Bluetooth devices.");
        do {
            (*count)++;
            devices = (char**)realloc(devices, sizeof(char*) * (*count));
            wchar_t* wname = deviceInfo.szName;
            int len = WideCharToMultiByte(CP_UTF8, 0, wname, -1, NULL, 0, NULL, NULL);
            devices[(*count) - 1] = (char*)malloc(len);
            WideCharToMultiByte(CP_UTF8, 0, wname, -1, devices[(*count) - 1], len, NULL, NULL);
            SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: Device found: %s", devices[(*count) - 1]);
        } while (BluetoothFindNextDevice(hFind, &deviceInfo));

        BluetoothFindDeviceClose(hFind);
    } 
    else {
        SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: No Bluetooth devices found.");
    }

    SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_windows]: Exiting function. Devices found: %d", *count);
    return devices;
}

#elif __linux__

#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <locale.h>

static char* get_linux_version() {
    SYSINFO_LOG("[get_linux_version]: Retrieving Linux version.");
    
    static char version[128];
    FILE* fp = popen("lsb_release -r | awk '{print $2}'", "r");

    if (fp == NULL) {
        SYSINFO_LOG("[get_linux_version]: Failed to open command to get Linux version.");
        strcpy(version, "unknown");
        return version;
    }
    
    if (fgets(version, sizeof(version) - 1, fp) != NULL) {
        version[strcspn(version, "\n")] = 0;  // Remove newline
        SYSINFO_LOG("[get_linux_version]: Linux version retrieved: %s", version);
    } 
    else {
        SYSINFO_LOG("[get_linux_version]: Failed to retrieve Linux version.");
        strcpy(version, "unknown");
    }

    pclose(fp);
    return version;
}

static char* get_linux_product_type() {
    SYSINFO_LOG("[get_linux_product_type]: Retrieving Linux product type.");

    static char product_type[128];
    FILE* fp = popen("lsb_release -i | awk -F: '{print $2}'", "r");

    if (fp == NULL) {
        SYSINFO_LOG("[get_linux_product_type]: Failed to open command to get Linux product type.");
        strcpy(product_type, "unknown");
        return product_type;
    }
    
    if (fgets(product_type, sizeof(product_type) - 1, fp) != NULL) {
        product_type[strcspn(product_type, "\n")] = 0;  // Remove newline

        // Trim whitespace
        char* start = product_type;
        while (*start == ' ' || *start == '\t') {
            start++;
        }
        memmove(product_type, start, strlen(start) + 1);
        SYSINFO_LOG("[get_linux_product_type]: Linux product type retrieved: %s", product_type);
    } 
    else {
        SYSINFO_LOG("[get_linux_product_type]: Failed to retrieve Linux product type.");
        strcpy(product_type, "unknown");
    }

    pclose(fp);
    return product_type;
}

static char* get_linux_kernel_version() {
    SYSINFO_LOG("[get_linux_kernel_version]: Retrieving Linux kernel version.");
    
    static char kernel_version[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        strncpy(kernel_version, buffer.release, sizeof(kernel_version));
        SYSINFO_LOG("[get_linux_kernel_version]: Linux kernel version retrieved: %s", kernel_version);
    } 
    else {
        SYSINFO_LOG("[get_linux_kernel_version]: Failed to retrieve Linux kernel version.");
        strcpy(kernel_version, "unknown");
    }

    return kernel_version;
}

static char* get_linux_kernel_type() {
    SYSINFO_LOG("[get_linux_kernel_type]: Retrieving Linux kernel type.");
    
    static char kernel_type[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        strncpy(kernel_type, buffer.sysname, sizeof(kernel_type));
        SYSINFO_LOG("[get_linux_kernel_type]: Linux kernel type retrieved: %s", kernel_type);
    } 
    else {
        SYSINFO_LOG("[get_linux_kernel_type]: Failed to retrieve Linux kernel type.");
        strcpy(kernel_type, "unknown");
    }

    return kernel_type;
}

static char* get_linux_boot_unique_id() {
    SYSINFO_LOG("[get_linux_boot_unique_id]: Retrieving boot unique ID.");
    
    static char boot_id[128];
    FILE* fp = fopen("/proc/sys/kernel/random/boot_id", "r");

    if (fp == NULL) {
        SYSINFO_LOG("[get_linux_boot_unique_id]: Failed to open /proc/sys/kernel/random/boot_id.");
        strcpy(boot_id, "unknown");
        return boot_id;
    }

    if (fgets(boot_id, sizeof(boot_id) - 1, fp) != NULL) {
        boot_id[strcspn(boot_id, "\n")] = 0;  // Remove newline
        SYSINFO_LOG("[get_linux_boot_unique_id]: Boot unique ID retrieved: %s", boot_id);
    } 
    else {
        SYSINFO_LOG("[get_linux_boot_unique_id]: Failed to read boot unique ID.");
        strcpy(boot_id, "unknown");
    }

    fclose(fp);
    return boot_id;
}

static char* get_linux_cpu_architecture() {
    SYSINFO_LOG("[get_linux_cpu_architecture]: Retrieving CPU architecture.");
    
    static char architecture[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        strncpy(architecture, buffer.machine, sizeof(architecture));
        SYSINFO_LOG("[get_linux_cpu_architecture]: CPU architecture retrieved: %s", architecture);
    } 
    else {
        SYSINFO_LOG("[get_linux_cpu_architecture]: Failed to retrieve CPU architecture.");
        strcpy(architecture, "unknown");
    }

    return architecture;
}

static char* get_linux_machine_host_name() {
    SYSINFO_LOG("[get_linux_machine_host_name]: Retrieving machine host name.");
    
    static char hostname[1024];
    hostname[1023] = '\0';

    if (gethostname(hostname, sizeof(hostname) - 1) != 0) {
        SYSINFO_LOG("[get_linux_machine_host_name]: Failed to retrieve hostname using gethostname.");
        strcpy(hostname, "Unknown_Host_Name");
        return hostname;
    }

    struct addrinfo hints, *info, *p;
    int gai_result;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
        SYSINFO_LOG("[get_linux_machine_host_name]: getaddrinfo failed: %s", gai_strerror(gai_result));
        strcpy(hostname, "Unknown_Host_Name");
        return hostname;
    }

    for (p = info; p != NULL; p = p->ai_next) {
        if (p->ai_canonname) {
            strncpy(hostname, p->ai_canonname, sizeof(hostname) - 1);
            hostname[sizeof(hostname) - 1] = '\0';
            SYSINFO_LOG("[get_linux_machine_host_name]: Canonical name retrieved: %s", hostname);
            break;
        }
    }

    freeaddrinfo(info);
    return hostname;
}

static char* get_linux_machine_unique_id() {
    SYSINFO_LOG("[get_linux_machine_unique_id]: Retrieving machine unique ID.");
    
    static char unique_id[128];
    FILE* fp = fopen("/etc/machine-id", "r");

    if (fp == NULL) {
        SYSINFO_LOG("[get_linux_machine_unique_id]: Failed to open /etc/machine-id.");
        strcpy(unique_id, "unknown");
        return unique_id;
    }
    if (fgets(unique_id, sizeof(unique_id) - 1, fp) != NULL) {
        unique_id[strcspn(unique_id, "\n")] = 0;  // Remove newline
        SYSINFO_LOG("[get_linux_machine_unique_id]: Machine ID retrieved: %s", unique_id);
    } 
    else {
        SYSINFO_LOG("[get_linux_machine_unique_id]: Failed to read machine ID.");
        strcpy(unique_id, "unknown");
    }

    fclose(fp);
    return unique_id;
}

static char* get_linux_build_abi() {
    SYSINFO_LOG("[get_linux_build_abi]: Retrieving build ABI information.");

    static char build_abi[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        SYSINFO_LOG("[get_linux_build_abi]: Machine type retrieved: %s", buffer.machine);
        if (strcmp(buffer.machine, "x86_64") == 0) {
            strcpy(build_abi, "x86_64-");
        } 
        else {
            strcpy(build_abi, buffer.machine);
            strcat(build_abi, "-");
        }
    } 
    else {
        SYSINFO_LOG("[get_linux_build_abi]: Failed to retrieve machine type via uname.");
        strcpy(build_abi, "unknown-");
    }

#if __BYTE_ORDER == __LITTLE_ENDIAN
    SYSINFO_LOG("[get_linux_build_abi]: System is little endian.");
    strcat(build_abi, "little_endian-");
#else
    SYSINFO_LOG("[get_linux_build_abi]: System is big endian.");
    strcat(build_abi, "big_endian-");
#endif

    strcat(build_abi, "lp64");
    SYSINFO_LOG("[get_linux_build_abi]: Build ABI: %s", build_abi);

    return build_abi;
}

char** get_sysinfo_list_bluetooth_devices_linux(int* count) {
    SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: Retrieving Bluetooth devices.");
    
    *count = 0;
    char** devices = NULL;
    FILE* pipe = popen("hcitool dev", "r");

    if (pipe) {
        SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: Successfully opened pipe.");
        
        char buffer[128];
        while (fgets(buffer, 128, pipe) != NULL) {
            buffer[strcspn(buffer, "\r\n")] = 0;  // Remove newline characters
            (*count)++;
            SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: Found device: %s", buffer);
            devices = (char**)realloc(devices, sizeof(char*) * (*count));
            if (!devices) {
                SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: Memory allocation failed.");
                pclose(pipe);
                return NULL;
            }
            devices[(*count) - 1] = strdup(buffer);
        }

        pclose(pipe);
        SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: Total devices found: %d", *count);
    }
    else {
        SYSINFO_LOG("[get_sysinfo_list_bluetooth_devices_linux]: Failed to open pipe.");
    }

    return devices;
}

#else

static char* get_unknown_version() {
    SYSINFO_LOG("[get_unknown_version]: Returning unknown version.");
    static char version[] = "unknown";

    return version;
}

static char* get_unknown_product_type() {
    SYSINFO_LOG("[get_unknown_product_type]: Returning unknown product type.");
    static char product_type[] = "unknown";

    return product_type;
}

static char* get_unknown_kernel_version() {
    SYSINFO_LOG("[get_unknown_kernel_version]: Returning unknown kernel version.");
    static char kernel_version[] = "unknown";

    return kernel_version;
}

static char* get_unknown_kernel_type() {
    SYSINFO_LOG("[get_unknown_kernel_type]: Returning unknown kernel type.");
    static char kernel_type[] = "unknown";

    return kernel_type;
}

static char* get_unknown_boot_unique_id() {
    SYSINFO_LOG("[get_unknown_boot_unique_id]: Returning empty boot unique ID.");
    static char boot_id[] = "";

    return boot_id;
}

static char* get_unknown_cpu_architecture() {
    SYSINFO_LOG("[get_unknown_cpu_architecture]: Returning unknown CPU architecture.");
    static char architecture[] = "unknown";

    return architecture;
}

static char* get_unknown_machine_host_name() {
    SYSINFO_LOG("[get_unknown_machine_host_name]: Returning unknown host name.");
    static char hostname[] = "Unknown_Host_Name";

    return hostname;
}

static char* get_unknown_machine_unique_id() {
    SYSINFO_LOG("[get_unknown_machine_unique_id]: Returning unknown machine unique ID.");
    static char unique_id[] = "unknown";

    return unique_id;
}

static char* get_unknown_build_abi() {
    SYSINFO_LOG("[get_unknown_build_abi]: Returning unknown build ABI.");
    static char build_abi[] = "unknown";

    return build_abi;
}

char** sysinfo_list_bluetooth_devices(int* count) {
    SYSINFO_LOG("[sysinfo_list_bluetooth_devices]: No Bluetooth devices found.");
    *count = 0;

    return NULL;
}

#endif

/**
 * @brief This function returns the version of the operating system in a string format. 
 * The version provides important information about the specific release of the operating system being used.
 * 
 * @return char* A static string containing the OS version. If the version cannot 
 *               be determined, the function returns "unknown".
 */
char* sysinfo_product_version() {
    SYSINFO_LOG("[sysinfo_product_version]: Retrieving OS version.");
#ifdef _WIN32
    return get_windows_version();
#elif __linux__
    return get_linux_version();
#else
    SYSINFO_LOG("[sysinfo_product_version]: Returning unknown version.");
    return get_unknown_version();
#endif
}

/**
 * @brief This function returns the product type or name of the operating system. For instance, 
 * it might return "Windows" for a Windows operating system or "Ubuntu" for an Ubuntu 
 * Linux distribution. This information is useful for identifying the specific type of 
 * operating system the application is running on.
 * 
 * @return char* A static string containing the OS product type. If the product type 
 *               cannot be determined, the function returns "unknown".
 */
char* sysinfo_product_type() {
    SYSINFO_LOG("[sysinfo_product_type]: Retrieving product type.");
#ifdef _WIN32
    char* product_type = get_windows_product_type();
#elif __linux__
    char* product_type = get_linux_product_type();
#else
    char* product_type = get_unknown_product_type();
#endif
    SYSINFO_LOG("[sysinfo_product_type]: Product type is %s.", product_type ? product_type : "unknown");
    return product_type;
}

/**
 * @brief This function retrieves the kernel version of the operating system on which the application
 * is running. For example, it may return "10.0.19045" on Windows or "6.8.0-35-generic" on Linux.
 * The kernel version is essential for understanding the system's low-level OS details.
 * 
 * @return char* A static string containing the kernel version. If the version cannot be determined,
 *               the function returns "unknown".
 */
char* sysinfo_kernel_version() {
    SYSINFO_LOG("[sysinfo_kernel_version]: Retrieving kernel version.");
#ifdef _WIN32
    char* kernel_version = get_windows_kernel_version();
#elif __linux__
    char* kernel_version = get_linux_kernel_version();
#else
    char* kernel_version = get_unknown_kernel_version();
#endif
    SYSINFO_LOG("[sysinfo_kernel_version]: Kernel version is %s.", kernel_version ? kernel_version : "unknown");
    return kernel_version;
}

/**
 * @brief This function provides the type of the kernel that the application is running on. For example,
 * it may return "Windows NT" on Windows or "Linux" on Unix-like systems. This information is useful
 * for identifying the general type of the operating system kernel.
 * 
 * @return char* A static string containing the kernel type. If the type cannot be determined,
 *               the function returns "unknown".
 */
char* sysinfo_kernel_type() {
    SYSINFO_LOG("[sysinfo_kernel_type]: Retrieving kernel type.");
#ifdef _WIN32
    char* kernel_type = get_windows_kernel_type();
#elif __linux__
    char* kernel_type = get_linux_kernel_type();
#else
    char* kernel_type = get_unknown_kernel_type();
#endif
    SYSINFO_LOG("[sysinfo_kernel_type]: Kernel type is %s.", kernel_type ? kernel_type : "unknown");
    return kernel_type;
}

/**
 * @brief This function generates a unique identifier that represents the current boot session of the machine.
 * On Linux, this is typically obtained from `/proc/sys/kernel/random/boot_id`, while on Windows, it may
 * be derived from the system uptime. This unique ID can be used to track the current boot session of the system.
 * 
 * @return char* A static string containing the unique boot ID. If the ID cannot be determined,
 *               the function returns "unknown".
 */
char* sysinfo_boot_unique_id() {
    SYSINFO_LOG("[sysinfo_boot_unique_id]: Retrieving boot unique ID.");
#ifdef _WIN32
    char* boot_id = get_windows_boot_unique_id();
#elif __linux__
    char* boot_id = get_linux_boot_unique_id();
#else
    char* boot_id = get_unknown_boot_unique_id();
#endif
    SYSINFO_LOG("[sysinfo_boot_unique_id]: Boot unique ID is %s.", boot_id ? boot_id : "unknown");
    return boot_id;
}

/**
 * @brief This function detects and returns the architecture of the CPU that the application
 * is running on. For example, it may return "x86_64", "ARM", or "IA64", depending on
 * the system. The architecture string is useful for understanding the type of processor
 * the system is using.
 * 
 * @return char* A static string containing the CPU architecture.
 */
char* sysinfo_cpu_architecture() {
    SYSINFO_LOG("[sysinfo_cpu_architecture]: Retrieving CPU architecture.");
#ifdef _WIN32
    char* cpu_arch = get_windows_cpu_architecture();
#elif __linux__
    char* cpu_arch = get_linux_cpu_architecture();
#else
    char* cpu_arch = get_unknown_cpu_architecture();
#endif
    SYSINFO_LOG("[sysinfo_cpu_architecture]: CPU architecture is %s.", cpu_arch ? cpu_arch : "unknown");
    return cpu_arch;
}

/**
 * @brief This function retrieves the fully qualified domain name (FQDN) or the simple host name
 * of the machine on which the application is running. On Linux, it may attempt to resolve
 * the FQDN, while on Windows, it returns the computer name.
 * 
 * @return char* A static string containing the machine's host name.
 */
char* sysinfo_machine_host_name() {
    SYSINFO_LOG("[sysinfo_machine_host_name]: Retrieving machine host name.");
#ifdef _WIN32
    char* host_name = get_windows_machine_host_name();
#elif __linux__
    char* host_name = get_linux_machine_host_name();
#else
    char* host_name = get_unknown_machine_host_name();
#endif
    SYSINFO_LOG("[sysinfo_machine_host_name]: Machine host name is %s.", host_name ? host_name : "unknown");
    return host_name;
}

/**
 * @brief This function provides a unique identifier for the machine, which is useful for identifying
 * the machine in network operations or other scenarios where a persistent unique ID is needed.
 * The method of obtaining this ID varies by platform.
 * 
 * @return char* A static string containing the machine's unique ID. If the ID cannot be determined,
 *               the function returns "unknown".
 */
char* sysinfo_machine_unique_id() {
    SYSINFO_LOG("[sysinfo_machine_unique_id]: Retrieving machine unique ID.");
#ifdef _WIN32
    char* id = get_windows_machine_unique_id();
#elif __linux__
    char* id = get_linux_machine_unique_id();
#else
    char* id = get_unknown_machine_unique_id();
#endif
    SYSINFO_LOG("[sysinfo_machine_unique_id]: Machine unique ID is %s.", id ? id : "unknown");
    return id;
}

/**
 * @brief This function generates a human-readable string that describes the operating system
 * name and version in a friendly format. On Windows, it uses the system's product name
 * and version. On Linux, it combines the product type and version. For other systems,
 * it falls back to the kernel type and version.
 * 
 * @return char* A static string containing the pretty product name.
 */
char* sysinfo_pretty_product_name() {
    static char pretty_name[256];
    memset(pretty_name, 0, sizeof(pretty_name));
    SYSINFO_LOG("[sysinfo_pretty_product_name]: Retrieving pretty product name.");

#ifdef _WIN32
    get_windows_pretty_version(pretty_name, sizeof(pretty_name));
    SYSINFO_LOG("[sysinfo_pretty_product_name]: Product name is %s.", pretty_name);
#elif __linux__
    snprintf(pretty_name, sizeof(pretty_name), "%s %s", sysinfo_product_type(), sysinfo_product_version());
    SYSINFO_LOG("[sysinfo_pretty_product_name]: Product name is %s.", pretty_name);
#else
    snprintf(pretty_name, sizeof(pretty_name), "%s %s", sysinfo_kernel_type(), sysinfo_kernel_version());
    SYSINFO_LOG("[sysinfo_pretty_product_name]: Product name is %s.", pretty_name);
#endif

    return pretty_name;
}

/**
 * @brief This function provides information about the system's ABI, which includes details
 * such as the architecture, endianness, and data model (e.g., LP64, LLP64). The ABI
 * is useful for understanding how the compiled binaries will interact with the system.
 * 
 * @return char* A static string containing the ABI information.
 */
char* sysinfo_build_abi() {
    SYSINFO_LOG("[sysinfo_build_abi]: Retrieving system ABI information.");
#ifdef _WIN32
    char* abi = get_windows_build_abi();
#elif __linux__
    char* abi = get_linux_build_abi();
#else
    char* abi = get_unknown_build_abi();
#endif
    SYSINFO_LOG("[sysinfo_build_abi]: System ABI is %s.", abi ? abi : "unknown");
    return abi;
}

/**
 * @brief This function scans for Bluetooth devices and returns a list of device names that
 * are connected or remembered by the system. The number of devices found is returned
 * through the `count` parameter. The list of devices is dynamically allocated and should
 * be freed by the caller.
 * 
 * @param count Pointer to an integer that will store the number of Bluetooth devices found.
 * 
 * @return char** An array of strings, each representing a Bluetooth device name.
 *               The caller is responsible for freeing each string and the array itself.
 */
char** sysinfo_list_bluetooth_devices(int* count) {
    SYSINFO_LOG("[sysinfo_list_bluetooth_devices]: Retrieving list of Bluetooth devices.");
#ifdef _WIN32
    char** devices = get_sysinfo_list_bluetooth_devices_windows(count);
#elif __linux__
    char** devices = get_sysinfo_list_bluetooth_devices_linux(count);
#else
    char** devices = NULL;
#endif

    if (devices) {
        SYSINFO_LOG("[sysinfo_list_bluetooth_devices]: Found %d Bluetooth devices.", *count);
    } 
    else {
        SYSINFO_LOG("[sysinfo_list_bluetooth_devices]: No Bluetooth devices found.");
    }

    return devices;
}

#ifdef _WIN32
static double get_sysinfo_cpu_usage_windows() {
    SYSINFO_LOG("[get_sysinfo_cpu_usage_windows]: Retrieving CPU usage.");

    FILETIME idleTime, kernelTime, userTime;

    if (GetSystemTimes(&idleTime, &kernelTime, &userTime) == 0) {
        SYSINFO_LOG("[get_sysinfo_cpu_usage_windows]: Error - Failed to get CPU times.");
        return -1.0;
    }

    static ULONGLONG prevIdleTime = 0, prevKernelTime = 0, prevUserTime = 0;

    ULONGLONG idle = ((ULONGLONG)idleTime.dwLowDateTime | ((ULONGLONG)idleTime.dwHighDateTime << 32));
    ULONGLONG kernel = ((ULONGLONG)kernelTime.dwLowDateTime | ((ULONGLONG)kernelTime.dwHighDateTime << 32));
    ULONGLONG user = ((ULONGLONG)userTime.dwLowDateTime | ((ULONGLONG)userTime.dwHighDateTime << 32));

    ULONGLONG idleDiff = idle - prevIdleTime;
    ULONGLONG kernelDiff = kernel - prevKernelTime;
    ULONGLONG userDiff = user - prevUserTime;

    prevIdleTime = idle;
    prevKernelTime = kernel;
    prevUserTime = user;

    if (kernelDiff + userDiff == 0) {
        SYSINFO_LOG("[get_sysinfo_cpu_usage_windows]: CPU usage is 0 (no difference in times).");
        return 0.0;
    }

    double cpuUsage = (double)((kernelDiff + userDiff - idleDiff) * 100.0) / (kernelDiff + userDiff);
    SYSINFO_LOG("[get_sysinfo_cpu_usage_windows]: CPU usage is %.2f%%", cpuUsage);

    return cpuUsage;
}

// Windows specific memory usage function
static double get_sysinfo_memory_usage_windows() {
    SYSINFO_LOG("[get_sysinfo_memory_usage_windows]: Retrieving memory usage.");

    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);

    if (GlobalMemoryStatusEx(&memInfo) == 0) {
        SYSINFO_LOG("[get_sysinfo_memory_usage_windows]: Error - Failed to retrieve memory status.");
        return -1.0;
    }

    DWORDLONG totalMemory = memInfo.ullTotalPhys;
    DWORDLONG usedMemory = totalMemory - memInfo.ullAvailPhys;

    if (totalMemory == 0) {
        SYSINFO_LOG("[get_sysinfo_memory_usage_windows]: Error - Total memory is zero.");
        return -1.0;
    }

    double memoryUsage = (double)(usedMemory * 100.0) / totalMemory;
    SYSINFO_LOG("[get_sysinfo_memory_usage_windows]: Memory usage is %.2f%%", memoryUsage);

    return memoryUsage;
}

static char* get_sysinfo_disk_space_windows(const char* path) {
    SYSINFO_LOG("[get_sysinfo_disk_space_windows]: Retrieving disk space for path: %s", path);

    ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;

    if (GetDiskFreeSpaceExA(path, &freeBytesAvailable, &totalBytes, &totalFreeBytes) == 0) {
        SYSINFO_LOG("[get_sysinfo_disk_space_windows]: Error - Failed to get disk space information for path: %s", path);
        return NULL;
    }

    char* result = (char*)malloc(256);
    if (result == NULL) {
        SYSINFO_LOG("[get_sysinfo_disk_space_windows]: Error - Memory allocation failed.");
        return NULL;
    }

    snprintf(result, 256, "Total: %llu MB, Used: %llu MB, Available: %llu MB",
             totalBytes.QuadPart / (1024 * 1024),
             (totalBytes.QuadPart - totalFreeBytes.QuadPart) / (1024 * 1024),
             totalFreeBytes.QuadPart / (1024 * 1024));

    SYSINFO_LOG("[get_sysinfo_disk_space_windows]: Successfully retrieved disk space info: %s", result);
    return result;
}

// Simpler Windows-specific function to get system uptime
static char* get_sysinfo_system_uptime_windows() {
    SYSINFO_LOG("[get_sysinfo_system_uptime_windows]: Retrieving system uptime.");

    ULONGLONG uptimeMillis = GetTickCount64();  
    ULONGLONG uptimeSeconds = uptimeMillis / 1000; 

    unsigned long days = uptimeSeconds / (24 * 3600);
    uptimeSeconds %= (24 * 3600);
    unsigned long hours = uptimeSeconds / 3600;
    uptimeSeconds %= 3600;
    unsigned long minutes = uptimeSeconds / 60;
    unsigned long seconds = uptimeSeconds % 60;

    char* result = (char*)malloc(128);
    if (result == NULL) {
        SYSINFO_LOG("[get_sysinfo_system_uptime_windows]: Error - Memory allocation failed.");
        return NULL;
    }

    snprintf(result, 128, "%lu days, %lu hours, %lu minutes, %lu seconds", days, hours, minutes, seconds);
    SYSINFO_LOG("[get_sysinfo_system_uptime_windows]: Successfully retrieved uptime: %s", result);

    return result;
}

static void get_running_services_windows(Vector *services) {
    SYSINFO_LOG("[get_running_services_windows]: Retrieving running services.");

    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (!scm) {
        SYSINFO_LOG("[get_running_services_windows]: Error - Unable to open Service Control Manager.");
        return;
    }

    DWORD bytesNeeded = 0, servicesReturned = 0, resumeHandle = 0;
    EnumServicesStatus(scm, SERVICE_WIN32, SERVICE_STATE_ALL, NULL, 0, &bytesNeeded, &servicesReturned, &resumeHandle);

    LPENUM_SERVICE_STATUS serviceStatus = (LPENUM_SERVICE_STATUS)malloc(bytesNeeded);
    if (!serviceStatus) {
        SYSINFO_LOG("[get_running_services_windows]: Error - Memory allocation failed for service status.");
        CloseServiceHandle(scm);
        return;
    }

    if (EnumServicesStatus(scm, SERVICE_WIN32, SERVICE_STATE_ALL, serviceStatus, bytesNeeded, &bytesNeeded, &servicesReturned, &resumeHandle)) {
        for (DWORD i = 0; i < servicesReturned; i++) {
            char *serviceName = strdup(serviceStatus[i].lpServiceName);  // Allocate memory for service name
            if (serviceName) {
                vector_push_back(services, &serviceName);
                SYSINFO_LOG("[get_running_services_windows]: Added service: %s", serviceName);
            } 
            else {
                SYSINFO_LOG("[get_running_services_windows]: Error - Memory allocation failed for service name.");
            }
        }
    } 
    else {
        SYSINFO_LOG("[get_running_services_windows]: Error - Failed to enumerate services.");
    }

    free(serviceStatus);
    CloseServiceHandle(scm);
    SYSINFO_LOG("[get_running_services_windows]: Finished retrieving running services.");
}

static void get_open_ports_windows(Vector *ports) {
    SYSINFO_LOG("[get_open_ports_windows]: Retrieving open TCP ports.");

    PMIB_TCPTABLE tcpTable = NULL;
    DWORD size = 0;

    if (GetTcpTable(NULL, &size, TRUE) == ERROR_INSUFFICIENT_BUFFER) {
        tcpTable = (MIB_TCPTABLE*)malloc(size);
        if (!tcpTable) {
            SYSINFO_LOG("[get_open_ports_windows]: Error - Memory allocation failed for TCP table.");
            return;
        }
    }

    if (GetTcpTable(tcpTable, &size, TRUE) == NO_ERROR) {
        for (DWORD i = 0; i < tcpTable->dwNumEntries; i++) {
            DWORD port = ntohs((u_short)tcpTable->table[i].dwLocalPort);
            int *port_ptr = (int*)malloc(sizeof(int));
            if (port_ptr) {
                *port_ptr = (int)port;
                vector_push_back(ports, &port_ptr);
                SYSINFO_LOG("[get_open_ports_windows]: Added port %d to the list.", *port_ptr);
            } 
            else {
                SYSINFO_LOG("[get_open_ports_windows]: Error - Memory allocation failed for port.");
            }
        }
    } 
    else {
        SYSINFO_LOG("[get_open_ports_windows]: Error - Unable to retrieve TCP table.");
    }

    free(tcpTable);
    SYSINFO_LOG("[get_open_ports_windows]: Finished retrieving open TCP ports.");
}

static void get_disk_partitions_windows(Vector *partitions) {
    SYSINFO_LOG("[get_disk_partitions_windows]: Retrieving disk partitions.");

    DWORD drive_mask = GetLogicalDrives();
    if (drive_mask == 0) {
        SYSINFO_LOG("[get_disk_partitions_windows]: Error - Unable to retrieve logical drives.");
        return;
    }

    char drive_letter[4] = "A:\\";
    for (int i = 0; i < 26; ++i) {
        if (drive_mask & (1 << i)) {
            drive_letter[0] = 'A' + i;

            ULARGE_INTEGER free_bytes_available, total_bytes, total_free_bytes;
            if (GetDiskFreeSpaceExA(drive_letter, &free_bytes_available, &total_bytes, &total_free_bytes)) {
                SysinfoDiskPartition partition;
                partition.mount_point = strdup(drive_letter);
                partition.total_size = total_bytes.QuadPart;  
                partition.free_space = total_free_bytes.QuadPart;

                vector_push_back(partitions, &partition);
                SYSINFO_LOG("[get_disk_partitions_windows]: Added partition %s (Total: %llu, Free: %llu).",
                            partition.mount_point, partition.total_size, partition.free_space);
            } 
            else {
                SYSINFO_LOG("[get_disk_partitions_windows]: Error - Unable to retrieve disk space for drive %s.", drive_letter);
            }
        }
    }

    SYSINFO_LOG("[get_disk_partitions_windows]: Finished retrieving disk partitions.");
}

#elif __linux__

// Linux specific CPU usage function
static double get_sysinfo_cpu_usage_linux() {
    SYSINFO_LOG("[get_sysinfo_cpu_usage_linux]: Retrieving CPU usage.");

    FILE *fp = fopen("/proc/stat", "r");
    if (fp == NULL) {
        SYSINFO_LOG("[get_sysinfo_cpu_usage_linux]: Error - Failed to open /proc/stat.");
        return -1.0;
    }

    unsigned long long int user, nice, system, idle, iowait, irq, softirq, steal;
    if (fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal) != 8) {
        fclose(fp);
        SYSINFO_LOG("[get_sysinfo_cpu_usage_linux]: Error - Failed to read CPU stats from /proc/stat.");
        return -1.0;
    }
    fclose(fp);

    static unsigned long long int prevUser = 0, prevNice = 0, prevSystem = 0, prevIdle = 0, prevIowait = 0, prevIrq = 0, prevSoftirq = 0, prevSteal = 0;

    unsigned long long int idleDiff = (idle + iowait) - (prevIdle + prevIowait);
    unsigned long long int totalDiff = (user + nice + system + idle + iowait + irq + softirq + steal) -
                                       (prevUser + prevNice + prevSystem + prevIdle + prevIowait + prevIrq + prevSoftirq + prevSteal);

    prevUser = user;
    prevNice = nice;
    prevSystem = system;
    prevIdle = idle;
    prevIowait = iowait;
    prevIrq = irq;
    prevSoftirq = softirq;
    prevSteal = steal;

    if (totalDiff == 0) {
        SYSINFO_LOG("[get_sysinfo_cpu_usage_linux]: Warning - Total CPU diff is zero.");
        return 0.0;
    }

    double usage = (double)((totalDiff - idleDiff) * 100.0) / totalDiff;
    SYSINFO_LOG("[get_sysinfo_cpu_usage_linux]: CPU usage: %.2f%%", usage);

    return usage;
}

// Linux specific memory usage function
static double get_sysinfo_memory_usage_linux() {
    SYSINFO_LOG("[get_sysinfo_memory_usage_linux]: Retrieving memory usage.");

    FILE *fp = fopen("/proc/meminfo", "r");
    if (fp == NULL) {
        SYSINFO_LOG("[get_sysinfo_memory_usage_linux]: Error - Failed to open /proc/meminfo.");
        return -1.0;
    }

    unsigned long totalMemory = 0, availableMemory = 0;
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), fp)) {
        if (sscanf(buffer, "MemTotal: %lu kB", &totalMemory) == 1) {
            continue;
        }
        if (sscanf(buffer, "MemAvailable: %lu kB", &availableMemory) == 1) {
            break;
        }
    }

    fclose(fp);

    if (totalMemory == 0 || availableMemory == 0) {
        SYSINFO_LOG("[get_sysinfo_memory_usage_linux]: Error - Failed to retrieve total or available memory.");
        return -1.0;
    }

    unsigned long usedMemory = totalMemory - availableMemory;
    double usage = (double)(usedMemory * 100.0) / totalMemory;

    SYSINFO_LOG("[get_sysinfo_memory_usage_linux]: Memory usage: %.2f%%", usage);

    return usage;
}

static char* get_sysinfo_disk_space_linux(const char* path) {
    SYSINFO_LOG("[get_sysinfo_disk_space_linux]: Retrieving disk space for path: %s", path);

    struct statvfs vfs;
    if (statvfs(path, &vfs) != 0) {
        SYSINFO_LOG("[get_sysinfo_disk_space_linux]: Error - Failed to get disk space information for path: %s", path);
        return NULL;
    }

    unsigned long totalBytes = vfs.f_blocks * vfs.f_frsize;
    unsigned long availableBytes = vfs.f_bavail * vfs.f_frsize;
    unsigned long usedBytes = totalBytes - availableBytes;

    char* result = (char*)malloc(256);
    if (result == NULL) {
        SYSINFO_LOG("[get_sysinfo_disk_space_linux]: Error - Memory allocation failed.");
        return NULL;
    }

    snprintf(result, 256, "Total: %lu MB, Used: %lu MB, Available: %lu MB",
             totalBytes / (1024 * 1024), usedBytes / (1024 * 1024), availableBytes / (1024 * 1024));

    SYSINFO_LOG("[get_sysinfo_disk_space_linux]: Disk space retrieved successfully for path: %s", path);
    return result;
}

static char* get_sysinfo_system_uptime_linux() {
    SYSINFO_LOG("[get_sysinfo_system_uptime_linux]: Retrieving system uptime.");

    FILE* fp = fopen("/proc/uptime", "r");
    if (fp == NULL) {
        SYSINFO_LOG("[get_sysinfo_system_uptime_linux]: Error - Failed to open /proc/uptime.");
        return NULL;
    }

    double uptimeSeconds = 0.0;
    if (fscanf(fp, "%lf", &uptimeSeconds) != 1) {
        fclose(fp);
        SYSINFO_LOG("[get_sysinfo_system_uptime_linux]: Error - Failed to read uptime from /proc/uptime.");
        return NULL;
    }
    fclose(fp);

    unsigned long days = uptimeSeconds / (24 * 3600);
    uptimeSeconds = fmod(uptimeSeconds, (24 * 3600));
    unsigned long hours = uptimeSeconds / 3600;
    uptimeSeconds = fmod(uptimeSeconds, 3600);
    unsigned long minutes = uptimeSeconds / 60;
    unsigned long seconds = (unsigned long)uptimeSeconds % 60;

    char* result = (char*)malloc(128);
    if (result == NULL) {
        SYSINFO_LOG("[get_sysinfo_system_uptime_linux]: Error - Memory allocation failed.");
        return NULL;
    }

    snprintf(result, 128, "%lu days, %lu hours, %lu minutes, %lu seconds", days, hours, minutes, seconds);

    SYSINFO_LOG("[get_sysinfo_system_uptime_linux]: System uptime retrieved successfully.");
    return result;
}

static void get_running_services_linux(Vector *services) {
    SYSINFO_LOG("[get_running_services_linux]: Running systemctl to list running services.");
    
    FILE *fp = popen("systemctl list-units --type=service --state=running --no-pager --no-legend", "r");
    if (!fp) {
        SYSINFO_LOG("[get_running_services_linux]: Error - Failed to run systemctl command.");
        return;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), fp)) {
        char serviceName[256];
        sscanf(buffer, "%255s", serviceName);

        SYSINFO_LOG("[get_running_services_linux]: Found service: %s", serviceName);

        char *service = strdup(serviceName); 
        vector_push_back(services, &service);
    }

    pclose(fp);
    SYSINFO_LOG("[get_running_services_linux]: Completed listing running services.");
}

static void get_open_ports_linux(Vector *ports) {
    SYSINFO_LOG("[get_open_ports_linux]: Running ss command to list open ports.");
    
    FILE *fp = popen("ss -lntu | awk 'NR>1 {print $5}'", "r");
    if (!fp) {
        SYSINFO_LOG("[get_open_ports_linux]: Error - Failed to run ss command.");
        return;
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), fp)) {
        char *ip_port = strchr(buffer, ':');
        if (ip_port) {
            int port;
            sscanf(ip_port + 1, "%d", &port);  // Extract the port number
            SYSINFO_LOG("[get_open_ports_linux]: Found open port: %d", port);

            int *port_ptr = (int*)malloc(sizeof(int));
            *port_ptr = port;
            vector_push_back(ports, &port_ptr);
        }
    }

    pclose(fp);
    SYSINFO_LOG("[get_open_ports_linux]: Completed listing open ports.");
}

// Linux specific virtualization detection remains the same
static bool check_virtualization_in_cpuinfo() {
    SYSINFO_LOG("[check_virtualization_in_cpuinfo]: Checking /proc/cpuinfo for hypervisor.");
    
    FILE *cpuinfo = fopen("/proc/cpuinfo", "r");
    if (!cpuinfo) {
        SYSINFO_LOG("[check_virtualization_in_cpuinfo]: Error - Failed to open /proc/cpuinfo.");
        return false;
    }

    char line[256];
    while (fgets(line, sizeof(line), cpuinfo)) {
        if (strstr(line, "hypervisor")) {
            SYSINFO_LOG("[check_virtualization_in_cpuinfo]: Virtualization detected in /proc/cpuinfo.");
            fclose(cpuinfo);
            return true;  // Found hypervisor flag in /proc/cpuinfo
        }
    }

    fclose(cpuinfo);
    SYSINFO_LOG("[check_virtualization_in_cpuinfo]: No virtualization detected in /proc/cpuinfo.");
    return false;
}

static bool check_virtualization_with_systemd() {
    SYSINFO_LOG("[check_virtualization_with_systemd]: Running systemd-detect-virt.");
    
    FILE *fp = popen("systemd-detect-virt", "r");
    if (!fp) {
        SYSINFO_LOG("[check_virtualization_with_systemd]: Error - Failed to run systemd-detect-virt.");
        return false;
    }

    char buffer[256];
    bool is_virtualized = false;
    if (fgets(buffer, sizeof(buffer), fp)) {
        if (strncmp(buffer, "none", 4) != 0) {
            SYSINFO_LOG("[check_virtualization_with_systemd]: Virtualization detected via systemd.");
            is_virtualized = true;  // systemd-detect-virt detected virtualization
        } 
        else {
            SYSINFO_LOG("[check_virtualization_with_systemd]: No virtualization detected via systemd.");
        }
    }

    pclose(fp);
    return is_virtualized;
}

/**
 * @brief Checks if the system is running in a virtualized environment.
 * 
 * This function attempts to detect if the system is virtualized by 
 * examining `/proc/cpuinfo` or using the `systemd-detect-virt` utility.
 *
 * On Linux, it first checks the `/proc/cpuinfo` file for any indicators 
 * of virtualization. If that fails, it tries to detect virtualization 
 * using `systemd-detect-virt`.
 *
 * @return bool Returns true if virtualization is detected, false otherwise.
 *
 * @note This function is primarily designed for Linux environments.
 *       The `check_virtualization_in_cpuinfo()` function checks for 
 *       known virtualization flags in the CPU information, and 
 *       `check_virtualization_with_systemd()` invokes `systemd-detect-virt`
 *       to identify if the system is running in a virtual machine.
 */
bool sysinfo_is_virtualized() {
    SYSINFO_LOG("[sysinfo_is_virtualized]: Checking for virtualization.");
    
    bool cpu_virtualization = check_virtualization_in_cpuinfo();
    bool systemd_virtualization = check_virtualization_with_systemd();

    if (cpu_virtualization) {
        SYSINFO_LOG("[sysinfo_is_virtualized]: Virtualization detected via CPU info.");
    } 
    else if (systemd_virtualization) {
        SYSINFO_LOG("[sysinfo_is_virtualized]: Virtualization detected via systemd.");
    } 
    else {
        SYSINFO_LOG("[sysinfo_is_virtualized]: No virtualization detected.");
    }

    return cpu_virtualization || systemd_virtualization;
}

static void get_disk_partitions_linux(Vector *partitions) {
    SYSINFO_LOG("[get_disk_partitions_linux]: Entering function.");
    
    FILE *fp = fopen("/proc/mounts", "r");
    if (!fp) {
        SYSINFO_LOG("[get_disk_partitions_linux]: Error - Failed to open /proc/mounts.");
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        char device[256], mount_point[256], fs_type[256];
        sscanf(line, "%255s %255s %255s", device, mount_point, fs_type);

        // Skip non-physical partitions (e.g., tmpfs, devtmpfs)
        if (strcmp(fs_type, "tmpfs") == 0 || strcmp(fs_type, "devtmpfs") == 0) {
            SYSINFO_LOG("[get_disk_partitions_linux]: Skipping non-physical partition: %s", fs_type);
            continue;
        }

        struct statvfs vfs;
        if (statvfs(mount_point, &vfs) == 0) {
            SysinfoDiskPartition partition;
            partition.mount_point = strdup(mount_point);
            partition.total_size = vfs.f_blocks * vfs.f_frsize;  // Total size in bytes
            partition.free_space = vfs.f_bavail * vfs.f_frsize;  // Available space in bytes

            SYSINFO_LOG("[get_disk_partitions_linux]: Found partition at mount point: %s", mount_point);
            vector_push_back(partitions, &partition);
        } 
        else {
            SYSINFO_LOG("[get_disk_partitions_linux]: Error - Failed to get stats for mount point: %s", mount_point);
        }
    }

    fclose(fp);
    SYSINFO_LOG("[get_disk_partitions_linux]: Exiting function.");
}

#endif

/**
 * @brief Retrieves the current CPU usage percentage of the system.
 * 
 * This function provides the CPU usage as a percentage by calculating the proportion
 * of time the CPU has spent performing work (in user mode and kernel mode) as compared to
 * idle time. The calculation is done over time, so repeated calls will provide more accurate
 * usage information.
 * 
 * On Windows, it uses the `GetSystemTimes()` function to get the amount of time spent in idle,
 * kernel, and user mode. On Linux, it reads from the `/proc/stat` file to obtain similar statistics.
 * 
 * @return double Returns the percentage of CPU usage. If an error occurs, it returns -1.0.
 * 
 * @note The function is platform-dependent and works on both Windows and Linux. On Windows, the 
 *       CPU usage is calculated based on time intervals provided by `GetSystemTimes()`. On Linux,
 *       the function parses the `/proc/stat` file to extract CPU usage statistics.
 */
double sysinfo_cpu_usage() {
    SYSINFO_LOG("[sysinfo_cpu_usage]: Entering function.");

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_cpu_usage]: Calling get_sysinfo_cpu_usage_windows.");
    double cpu_usage = get_sysinfo_cpu_usage_windows();
#elif __linux__
    SYSINFO_LOG("[sysinfo_cpu_usage]: Calling get_sysinfo_cpu_usage_linux.");
    double cpu_usage = get_sysinfo_cpu_usage_linux();
#endif
    if (cpu_usage < 0) {
        SYSINFO_LOG("[sysinfo_cpu_usage]: Error - Failed to retrieve CPU usage.");
    } 
    else {
        SYSINFO_LOG("[sysinfo_cpu_usage]: CPU usage retrieved successfully: %.2f%%", cpu_usage);
    }

    return cpu_usage;
}

/**
 * @brief Retrieves the current memory usage percentage of the system.
 * 
 * This function calculates the memory usage as a percentage of the total available memory on the system.
 * It retrieves memory statistics differently depending on the platform. On Windows, it uses the
 * `GlobalMemoryStatusEx()` function, while on Linux, it parses `/proc/meminfo`.
 * 
 * @return double Returns the percentage of memory being used. If an error occurs, it returns -1.0.
 * 
 * @note This function works on both Windows and Linux platforms. On Windows, it uses the 
 *       `GlobalMemoryStatusEx()` function to get the total and available physical memory. On Linux, 
 *       it reads from `/proc/meminfo` to retrieve memory statistics.
 */
double sysinfo_memory_usage() {
    SYSINFO_LOG("[sysinfo_memory_usage]: Entering function.");

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_memory_usage]: Calling get_sysinfo_memory_usage_windows.");
    double memory_usage = get_sysinfo_memory_usage_windows();
#elif __linux__
    SYSINFO_LOG("[sysinfo_memory_usage]: Calling get_sysinfo_memory_usage_linux.");
    double memory_usage = get_sysinfo_memory_usage_linux();
#endif
    if (memory_usage < 0) {
        SYSINFO_LOG("[sysinfo_memory_usage]: Error - Failed to retrieve memory usage.");
    } 
    else {
        SYSINFO_LOG("[sysinfo_memory_usage]: Memory usage retrieved successfully: %.2f%%", memory_usage);
    }

    return memory_usage;
}

/**
 * @brief Retrieves the disk space information for a given path.
 * 
 * This function provides the total, used, and available disk space for the specified path.
 * It retrieves disk space information using platform-specific methods.
 * 
 * On Windows, it uses the `GetDiskFreeSpaceEx()` function to get the total and free space.
 * On Linux, it uses the `statvfs()` system call to get the file system statistics.
 * 
 * @param path The file path to check the disk space for.
 * @return char* A string containing the total, used, and available disk space in MB.
 *               The caller is responsible for freeing the allocated memory.
 *               Returns NULL in case of an error.
 */
char* sysinfo_disk_space(const char* path) {
    SYSINFO_LOG("[sysinfo_disk_space]: Entering function with path: %s", path);

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_disk_space]: Calling get_sysinfo_disk_space_windows.");
    char* disk_space = get_sysinfo_disk_space_windows(path);
#elif __linux__
    SYSINFO_LOG("[sysinfo_disk_space]: Calling get_sysinfo_disk_space_linux.");
    char* disk_space = get_sysinfo_disk_space_linux(path);
#endif
    if (!disk_space) {
        SYSINFO_LOG("[sysinfo_disk_space]: Error - Failed to retrieve disk space for path: %s.", path);
    } 
    else {
        SYSINFO_LOG("[sysinfo_disk_space]: Retrieved disk space for path: %s.", path);
    }

    return disk_space;
}

/**
 * @brief Retrieves the system's uptime since the last boot.
 * 
 * This function returns the system uptime in a human-readable format, including days, hours,
 * minutes, and seconds.
 * 
 * On Windows, it uses the `GetTickCount64()` function to get the uptime in milliseconds.
 * On Linux, it parses the `/proc/uptime` file to get the uptime in seconds.
 * 
 * @return char* A string containing the system uptime in days, hours, minutes, and seconds.
 *               The caller is responsible for freeing the allocated memory.
 *               Returns NULL in case of an error.
 */
char* sysinfo_system_uptime() {
    SYSINFO_LOG("[sysinfo_system_uptime]: Entering function.");

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_system_uptime]: Calling get_sysinfo_system_uptime_windows.");
    char* uptime = get_sysinfo_system_uptime_windows();
#elif __linux__
    SYSINFO_LOG("[sysinfo_system_uptime]: Calling get_sysinfo_system_uptime_linux.");
    char* uptime = get_sysinfo_system_uptime_linux();
#endif
    if (!uptime) {
        SYSINFO_LOG("[sysinfo_system_uptime]: Error - Failed to retrieve system uptime.");
    } 
    else {
        SYSINFO_LOG("[sysinfo_system_uptime]: Retrieved system uptime.");
    }

    return uptime;
}

/**
 * @brief Retrieves the list of currently running services on the system.
 * 
 * This function gathers the running services on both Windows and Linux systems.
 * The services are returned in a dynamically allocated vector of `String*`.
 * 
 * @return Vector* A vector containing the names of the running services.
 * 
 * @note The vector needs to be deallocated by the caller.
 */
Vector* sysinfo_running_services() {
    SYSINFO_LOG("[sysinfo_running_services]: Entering function.");

    Vector *services = vector_create(sizeof(char*));
    if (!services) {
        SYSINFO_LOG("[sysinfo_running_services]: Error - Failed to create services vector.");
        return NULL;
    }

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_running_services]: Calling get_running_services_windows.");
    get_running_services_windows(services);
#elif __linux__
    SYSINFO_LOG("[sysinfo_running_services]: Calling get_running_services_linux.");
    get_running_services_linux(services);
#endif
    SYSINFO_LOG("[sysinfo_running_services]: Exiting function with %zu services.", vector_size(services));
    return services;
}

/**
 * @brief Retrieves the number of CPU cores available on the system.
 * 
 * This function returns the total number of CPU cores available for use on the system.
 * 
 * On Windows, it uses the `GetSystemInfo()` function to get the number of processors.
 * On Linux, it uses `sysconf(_SC_NPROCESSORS_ONLN)` to retrieve the number of available processors.
 * 
 * @return int Number of CPU cores. If an error occurs, it returns -1.
 */
int sysinfo_cpu_cores() {
    SYSINFO_LOG("[sysinfo_cpu_cores]: Entering function.");

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_cpu_cores]: Fetching CPU core count on Windows.");

    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    int cores = sysinfo.dwNumberOfProcessors;

    SYSINFO_LOG("[sysinfo_cpu_cores]: Number of CPU cores on Windows: %d.", cores);
    return cores;

#elif __linux__
    SYSINFO_LOG("[sysinfo_cpu_cores]: Fetching CPU core count on Linux.");

    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    if (nprocs < 1) {
        SYSINFO_LOG("[sysinfo_cpu_cores]: Error - Failed to get CPU core count on Linux.");
        return -1; 
    }

    SYSINFO_LOG("[sysinfo_cpu_cores]: Number of CPU cores on Linux: %ld.", nprocs);
    return (int)nprocs;

#endif
}

/**
 * @brief Retrieves a list of active processes on the system.
 * 
 * This function returns a list of currently running processes on the system. 
 * The processes are returned in a dynamically allocated vector of strings (char*).
 * 
 * @return Vector* A vector containing the names of the active processes.
 * 
 * @note The vector needs to be deallocated by the caller.
 */
Vector* sysinfo_process_list() {
    SYSINFO_LOG("[sysinfo_process_list]: Entering function.");
    
    Vector* processes = vector_create(sizeof(char*));
    if (!processes) {
        SYSINFO_LOG("[sysinfo_process_list]: Error - Failed to create processes vector.");
        return NULL;
    }

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_process_list]: Fetching process list on Windows.");

    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        SYSINFO_LOG("[sysinfo_process_list]: Error - Unable to create process snapshot.");
        return processes;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32)) {
        SYSINFO_LOG("[sysinfo_process_list]: Error - Unable to get the first process in the snapshot.");
        CloseHandle(hProcessSnap);
        return processes;
    }

    do {
        char* processName = strdup(pe32.szExeFile);
        vector_push_back(processes, &processName);
        SYSINFO_LOG("[sysinfo_process_list]: Added process %s.", processName);
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

#elif __linux__
    SYSINFO_LOG("[sysinfo_process_list]: Fetching process list on Linux.");

    DIR* procDir = opendir("/proc");
    if (!procDir) {
        SYSINFO_LOG("[sysinfo_process_list]: Error - Unable to open /proc directory.");
        return processes;
    }

    struct dirent* entry;
    while ((entry = readdir(procDir)) != NULL) {
        if (entry->d_type == DT_DIR && strspn(entry->d_name, "0123456789") == strlen(entry->d_name)) {
            char procPath[300];  // Ensure buffer size is sufficient
            snprintf(procPath, sizeof(procPath), "/proc/%s/comm", entry->d_name);

            FILE* commFile = fopen(procPath, "r");
            if (commFile) {
                char processName[256];
                if (fgets(processName, sizeof(processName), commFile)) {
                    processName[strcspn(processName, "\n")] = 0;  // Strip newline character
                    char* process = strdup(processName);
                    vector_push_back(processes, &process);
                    SYSINFO_LOG("[sysinfo_process_list]: Added process %s.", processName);
                }
                fclose(commFile);
            }
        }
    }

    closedir(procDir);
#endif

    SYSINFO_LOG("[sysinfo_process_list]: Exiting function with %zu processes.", vector_size(processes));
    return processes;
}

/**
 * @brief Retrieves the list of active network interfaces along with their IP addresses.
 * 
 * This function gathers the list of active network interfaces and their associated IP addresses
 * on both Windows and Linux systems. The results are returned in a dynamically allocated vector
 * of `SysinfoNetworkInterface` structures.
 * 
 * @return Vector* A vector containing the network interfaces and their IP addresses.
 * 
 * @note The vector and its contents need to be deallocated by the caller.
 */
Vector* sysinfo_network_interfaces() {
    SYSINFO_LOG("[sysinfo_network_interfaces]: Entering function.");
    
    Vector* interfaces = vector_create(sizeof(SysinfoNetworkInterface));
    if (!interfaces) {
        SYSINFO_LOG("[sysinfo_network_interfaces]: Error - Failed to create interfaces vector.");
        return NULL;
    }

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_network_interfaces]: Fetching network interfaces on Windows.");

    ULONG bufferSize = 15000;
    PIP_ADAPTER_ADDRESSES adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufferSize);
    if (!adapterAddresses) {
        SYSINFO_LOG("[sysinfo_network_interfaces]: Error - Memory allocation failed for adapter addresses.");
        vector_deallocate(interfaces);
        return NULL;
    }

    if (GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapterAddresses, &bufferSize) == ERROR_BUFFER_OVERFLOW) {
        SYSINFO_LOG("[sysinfo_network_interfaces]: Buffer size too small, reallocating...");
        free(adapterAddresses);
        adapterAddresses = (PIP_ADAPTER_ADDRESSES)malloc(bufferSize);
        if (!adapterAddresses) {
            SYSINFO_LOG("[sysinfo_network_interfaces]: Error - Memory allocation failed for larger buffer.");
            vector_deallocate(interfaces);
            return NULL;
        }
    }

    DWORD result = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapterAddresses, &bufferSize);
    if (result == NO_ERROR) {
        SYSINFO_LOG("[sysinfo_network_interfaces]: Successfully retrieved adapter addresses.");
        PIP_ADAPTER_ADDRESSES adapter = adapterAddresses;
        while (adapter) {
            if (adapter->OperStatus == IfOperStatusUp) {
                SYSINFO_LOG("[sysinfo_network_interfaces]: Processing active adapter %s.", adapter->AdapterName);

                SysinfoNetworkInterface iface;
                iface.interface_name = strdup(adapter->AdapterName);

                PIP_ADAPTER_UNICAST_ADDRESS unicast = adapter->FirstUnicastAddress;
                while (unicast) {
                    SOCKADDR* addr = unicast->Address.lpSockaddr;
                    if (addr->sa_family == AF_INET) {  // IPv4
                        struct sockaddr_in* ipv4 = (struct sockaddr_in*)addr;
                        iface.ip_address = strdup(inet_ntoa(ipv4->sin_addr));
                        vector_push_back(interfaces, &iface);
                        SYSINFO_LOG("[sysinfo_network_interfaces]: Added interface %s with IP %s.", iface.interface_name, iface.ip_address);
                        break; 
                    }
                    unicast = unicast->Next;
                }
            }
            adapter = adapter->Next;
        }
    } else {
        SYSINFO_LOG("[sysinfo_network_interfaces]: Error - Failed to retrieve adapter addresses. Error code: %lu.", result);
    }

    free(adapterAddresses);

#elif __linux__
    SYSINFO_LOG("[sysinfo_network_interfaces]: Fetching network interfaces on Linux.");

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        SYSINFO_LOG("[sysinfo_network_interfaces]: Error - Failed to open socket.");
        return interfaces;
    }

    struct ifconf ifc;
    char buffer[4096];
    ifc.ifc_len = sizeof(buffer);
    ifc.ifc_buf = buffer;

    if (ioctl(fd, SIOCGIFCONF, &ifc) == -1) {
        SYSINFO_LOG("[sysinfo_network_interfaces]: Error - Failed to get network interface configuration.");
        close(fd);
        return interfaces;
    }

    struct ifreq* ifr = ifc.ifc_req;
    int interfaceCount = ifc.ifc_len / sizeof(struct ifreq);

    for (int i = 0; i < interfaceCount; i++) {
        struct ifreq* item = &ifr[i];
        SysinfoNetworkInterface iface;
        iface.interface_name = strdup(item->ifr_name);
        SYSINFO_LOG("[sysinfo_network_interfaces]: Found interface %s.", iface.interface_name);

        if (ioctl(fd, SIOCGIFADDR, item) == 0) {
            struct sockaddr_in* addr = (struct sockaddr_in*)&item->ifr_addr;
            iface.ip_address = strdup(inet_ntoa(addr->sin_addr));
            vector_push_back(interfaces, &iface);
            SYSINFO_LOG("[sysinfo_network_interfaces]: Added interface %s with IP %s.", iface.interface_name, iface.ip_address);
        }
    }

    close(fd);
#endif

    SYSINFO_LOG("[sysinfo_network_interfaces]: Exiting function with %zu interfaces.", vector_size(interfaces));
    return interfaces;
}

/**
 * @brief Deallocates the memory used by a vector of SysinfoNetworkInterface structures.
 * 
 * @param interfaces The vector containing the network interfaces.
 */
void sysinfo_deallocate_network_interfaces(Vector* interfaces) {
    SYSINFO_LOG("[sysinfo_deallocate_network_interfaces]: Entering function.");

    if (!interfaces) {
        SYSINFO_LOG("[sysinfo_deallocate_network_interfaces]: NULL interfaces pointer.");
        return;
    }

    for (size_t i = 0; i < vector_size(interfaces); ++i) {
        SysinfoNetworkInterface* iface = (SysinfoNetworkInterface*)vector_at(interfaces, i);
        free(iface->interface_name);
        free(iface->ip_address);

        SYSINFO_LOG("[sysinfo_deallocate_network_interfaces]: Deallocated interface %zu.", i);
    }
    
    vector_deallocate(interfaces);
    SYSINFO_LOG("[sysinfo_deallocate_network_interfaces]: Exiting function.");
}


/**
 * @brief Retrieves a list of open network ports on the system.
 * 
 * This function gathers open ports from both Windows and Linux systems.
 * It returns a dynamically allocated vector of integers representing the open ports.
 * 
 * @return Vector* A vector containing the open ports.
 * 
 * @note The vector needs to be deallocated by the caller.
 */
Vector* sysinfo_open_ports() {
    SYSINFO_LOG("[sysinfo_open_ports]: Entering function.");

    Vector *ports = vector_create(sizeof(int*));
    if (!ports) {
        SYSINFO_LOG("[sysinfo_open_ports]: Error - Failed to create ports vector.");
        return NULL;
    }

#ifdef _WIN32
    SYSINFO_LOG("[sysinfo_open_ports]: Gathering open ports for Windows.");
    get_open_ports_windows(ports);
#elif __linux__
    SYSINFO_LOG("[sysinfo_open_ports]: Gathering open ports for Linux.");
    get_open_ports_linux(ports);
#endif

    SYSINFO_LOG("[sysinfo_open_ports]: Exiting function with %zu open ports.", vector_size(ports));
    return ports;
}

#ifdef _WIN32
/**
 * @brief Checks if the system is running in a virtualized environment.
 *
 * This function detects if the system is running in a virtualized environment 
 * by checking for specific processor features available on the platform.
 *
 * On Windows, it checks the presence of `PF_VIRT_FIRMWARE_ENABLED` and 
 * `PF_HYPERVISOR_PRESENT` features to determine virtualization status.
 *
 * On Linux, the detection is handled via `/proc/cpuinfo` or `systemd-detect-virt`, 
 * although these are not included in this Windows-specific implementation.
 *
 * @return bool Returns true if virtualization is detected, false otherwise.
 *
 * @note This function is platform-specific and currently supports Windows and Linux.
 *       The function checks for the availability of the `PF_VIRT_FIRMWARE_ENABLED` 
 *       feature on all Windows systems. The `PF_HYPERVISOR_PRESENT` feature is 
 *       conditionally checked if available on the platform.
 */
bool sysinfo_is_virtualized() {
    SYSINFO_LOG("[sysinfo_is_virtualized]: Entering function.");

    if (IsProcessorFeaturePresent(PF_VIRT_FIRMWARE_ENABLED)) {
        SYSINFO_LOG("[sysinfo_is_virtualized]: Virtualization detected via PF_VIRT_FIRMWARE_ENABLED.");
        return true;
    }

#ifdef PF_HYPERVISOR_PRESENT
    if (IsProcessorFeaturePresent(PF_HYPERVISOR_PRESENT)) {
        SYSINFO_LOG("[sysinfo_is_virtualized]: Virtualization detected via PF_HYPERVISOR_PRESENT.");
        return true;
    }
#endif
    SYSINFO_LOG("[sysinfo_is_virtualized]: No virtualization detected.");
    return false;
}
#endif

/**
 * @brief Retrieves the current system locale as a string.
 * 
 * This function returns the system locale on both Windows and Linux platforms.
 * 
 * @return char* A dynamically allocated string representing the system locale. 
 *               The caller is responsible for freeing the returned string.
 */
/**
 * @brief Retrieves the current system locale as a string.
 * 
 * This function returns the system locale on both Windows and Linux platforms.
 * 
 * @return char* A dynamically allocated string representing the system locale. 
 *               The caller is responsible for freeing the returned string.
 */
char* sysinfo_system_locale() {
    SYSINFO_LOG("[sysinfo_system_locale]: Entering function.");

#ifdef _WIN32
    wchar_t w_locale_buffer[128]; 
    char locale_buffer[128];

    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SNAME, w_locale_buffer, sizeof(w_locale_buffer) / sizeof(wchar_t)) == 0) {
        SYSINFO_LOG("[sysinfo_system_locale]: Failed to retrieve system locale, setting to 'unknown'.");
        strcpy(locale_buffer, "unknown");
    } 
    else {
        // Convert wide string (wchar_t) to narrow string (char*)
        wcstombs(locale_buffer, w_locale_buffer, sizeof(locale_buffer));
        SYSINFO_LOG("[sysinfo_system_locale]: Locale retrieved successfully: %s.", locale_buffer);
    }

#else
    // On Linux, check the environment variables to determine the locale.
    static char locale_buffer[128];
    const char* locale = getenv("LC_ALL");
    
    if (!locale || strlen(locale) == 0) {
        locale = getenv("LC_MESSAGES");
    }
    if (!locale || strlen(locale) == 0) {
        locale = getenv("LANG");
    }
    if (!locale || strlen(locale) == 0) {
        locale = setlocale(LC_ALL, NULL);
    }

    if (locale) {
        strncpy(locale_buffer, locale, sizeof(locale_buffer) - 1);
        locale_buffer[sizeof(locale_buffer) - 1] = '\0';  // Ensure null-termination
        SYSINFO_LOG("[sysinfo_system_locale]: Locale retrieved successfully: %s.", locale_buffer);
    } 
    else {
        SYSINFO_LOG("[sysinfo_system_locale]: Failed to retrieve system locale, setting to 'unknown'.");
        strcpy(locale_buffer, "unknown");
    }
#endif

    // Return a dynamically allocated copy of the locale string
    char* result = strdup(locale_buffer);
    if (!result) {
        SYSINFO_LOG("[sysinfo_system_locale]: Memory allocation failed for locale string.");
    } 
    else {
        SYSINFO_LOG("[sysinfo_system_locale]: Locale string copied successfully.");
    }

    SYSINFO_LOG("[sysinfo_system_locale]: Exiting function.");
    return result;
}


/**
 * @brief Checks if a specific service or process is running.
 * 
 * This function checks if a service or process by the given name is running on the system.
 * 
 * @param service_name The name of the service or process to check.
 * @return bool Returns true if the service/process is running, false otherwise.
 */
bool sysinfo_is_service_running(const char* service_name) {
    SYSINFO_LOG("[sysinfo_is_service_running]: Checking if service '%s' is running.", service_name);

    #ifdef _WIN32
        SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
        if (!scm) {
            SYSINFO_LOG("Error: Unable to open Service Control Manager.");
            return false;
        }

        SC_HANDLE service = OpenService(scm, service_name, SERVICE_QUERY_STATUS);
        if (!service) {
            SYSINFO_LOG("Error: Unable to open service '%s'.", service_name);
            CloseServiceHandle(scm);
            return false;
        }

        SERVICE_STATUS_PROCESS serviceStatus;
        DWORD bytesNeeded;
        bool isRunning = false;

        if (QueryServiceStatusEx(service, SC_STATUS_PROCESS_INFO, (LPBYTE)&serviceStatus, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {
            if (serviceStatus.dwCurrentState == SERVICE_RUNNING) {
                SYSINFO_LOG("[sysinfo_is_service_running]: Service '%s' is running.", service_name);
                isRunning = true;
            } else {
                SYSINFO_LOG("[sysinfo_is_service_running]: Service '%s' is not running.", service_name);
            }
        } else {
            SYSINFO_LOG("Error: Failed to query service status for '%s'.", service_name);
        }

        CloseServiceHandle(service);
        CloseServiceHandle(scm);
        return isRunning;

    #elif __linux__
        char command[256];
        snprintf(command, sizeof(command), "systemctl is-active --quiet %s", service_name);
        int result = system(command);

        if (result == 0) {
            SYSINFO_LOG("[sysinfo_is_service_running]: Service '%s' is running (systemctl check).", service_name);
            return true;
        }

        // Fallback to checking the process list with `ps`
        SYSINFO_LOG("[sysinfo_is_service_running]: Fallback to 'ps' command for service '%s'.", service_name);
        FILE* proc_fp = popen("ps -e", "r");
        if (!proc_fp) {
            SYSINFO_LOG("Error: Unable to execute 'ps' command.");
            return false;
        }

        char buffer[256];
        bool found = false;

        while (fgets(buffer, sizeof(buffer), proc_fp)) {
            if (strstr(buffer, service_name) != NULL) {
                SYSINFO_LOG("[sysinfo_is_service_running]: Service '%s' found in process list.", service_name);
                found = true;
                break;
            }
        }
        pclose(proc_fp);

        if (!found) {
            SYSINFO_LOG("[sysinfo_is_service_running]: Service '%s' is not running (ps check).", service_name);
        }
        return found;
    #endif
}

/**
 * @brief Retrieves the disk partitions on the system.
 * 
 * This function gathers information about the disk partitions available on the system and stores 
 * them in a vector of `SysinfoDiskPartition` structures. It handles both Windows and Linux platforms. 
 * On unsupported platforms, the function logs an error and returns `NULL`.
 * 
 * On Windows, it calls the `get_disk_partitions_windows` function to populate the vector with partition 
 * information. On Linux, it calls the `get_disk_partitions_linux` function.
 * 
 * @return A pointer to a `Vector` of `SysinfoDiskPartition` structures representing the disk partitions 
 * on the system. If the function fails or the platform is unsupported, it returns `NULL`.
 * 
 * @note The caller is responsible for deallocating the returned vector using `vector_deallocate`.
 */
Vector* sysinfo_disk_partitions() {
    SYSINFO_LOG("[sysinfo_disk_partitions]: Entering function.");

    Vector* partitions = vector_create(sizeof(SysinfoDiskPartition));
    if (!partitions) {
        SYSINFO_LOG("[sysinfo_disk_partitions]: Error - Failed to create partitions vector.");
        return NULL;
    }

    #ifdef _WIN32
        SYSINFO_LOG("[sysinfo_disk_partitions]: Calling get_disk_partitions_windows.");
        get_disk_partitions_windows(partitions);
    #elif __linux__
        SYSINFO_LOG("[sysinfo_disk_partitions]: Calling get_disk_partitions_linux.");
        get_disk_partitions_linux(partitions);
    #else
        SYSINFO_LOG("[sysinfo_disk_partitions]: Error - Unsupported platform.");
        vector_deallocate(partitions);
        return NULL;
    #endif

    SYSINFO_LOG("[sysinfo_disk_partitions]: Exiting function with %zu partitions.", vector_size(partitions));
    return partitions;
}

/**
 * @brief Deallocates the memory used by a vector of SysinfoDiskPartition structures.
 * 
 * This function iterates over a vector of `SysinfoDiskPartition` structures and frees the
 * dynamically allocated memory for each partition's `mount_point`. After freeing the memory
 * for each individual partition, it deallocates the vector itself.
 * 
 * @param partitions A pointer to a `Vector` containing `SysinfoDiskPartition` structures.
 *                   The vector and its contents are deallocated by this function.
 * 
 * @warning Ensure that you do not use the `partitions` vector or its elements after calling
 *          this function, as the memory will have been deallocated.
 */
void sysinfo_deallocate_disk_partitions(Vector* partitions) {
    SYSINFO_LOG("[sysinfo_deallocate_disk_partitions]: Entering function to deallocate disk partitions.");

    if (!partitions) {
        SYSINFO_LOG("[sysinfo_deallocate_disk_partitions]: Warning - NULL partitions vector passed.");
        return;
    }

    for (size_t i = 0; i < vector_size(partitions); ++i) {
        SysinfoDiskPartition* partition = (SysinfoDiskPartition*)vector_at(partitions, i);
        if (partition) {
            SYSINFO_LOG("[sysinfo_deallocate_disk_partitions]: Freeing mount point of partition %zu.", i);
            free(partition->mount_point);
        } 
        else {
            SYSINFO_LOG("[sysinfo_deallocate_disk_partitions]: Warning - NULL partition at index %zu.", i);
        }
    }

    SYSINFO_LOG("[sysinfo_deallocate_disk_partitions]: Deallocating partitions vector.");
    vector_deallocate(partitions);

    SYSINFO_LOG("[sysinfo_deallocate_disk_partitions]: Exiting function.");
}
