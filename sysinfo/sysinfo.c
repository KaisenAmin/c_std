#include "sysinfo.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

#include <windows.h>

static char* get_windows_version() {
    static char version[128];
    DWORD dwVersion = 0;
    DWORD dwMajorVersion = 0;
    DWORD dwMinorVersion = 0;

    dwVersion = GetVersion();

    dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
    dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

    if (dwVersion < 0x80000000) {
        sprintf(version, "%ld.%ld", dwMajorVersion, dwMinorVersion);
    } 
    else {
        strcpy(version, "unknown");
    }
    return version;
}

static char* get_windows_product_type() {
    static char product_type[] = "Windows";
    return product_type;
}

static char* get_windows_kernel_version() {
    static char kernel_version[128];
    OSVERSIONINFOEX osvi;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (GetVersionEx((OSVERSIONINFO*)&osvi)) {
        sprintf(kernel_version, "%ld.%ld.%ld", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
    } 
    else {
        strcpy(kernel_version, "unknown");
    }

    return kernel_version;
}

static char* get_windows_kernel_type() {
    static char kernel_type[] = "Windows NT";
    return kernel_type;
}

static char* get_windows_boot_unique_id() {
    static char boot_id[128] = {0};
    ULONGLONG uptime = GetTickCount64();

    sprintf(boot_id, "%llu", uptime);

    return boot_id;
}

static char* get_windows_cpu_architecture() {
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
    return architecture;
}

static char* get_windows_machine_host_name() {
    static char hostname[150] = {0};
    TCHAR infoBuf[150];
    DWORD bufCharCount = 150;

    if (GetComputerName(infoBuf, &bufCharCount)) {
        for (int i = 0; i < (int)bufCharCount; i++) {
            hostname[i] = infoBuf[i];
        }
    } else {
        strcpy(hostname, "Unknown_Host_Name");
    }
    return hostname;
}

#elif __linux__

#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

static char* get_linux_version() {
    static char version[128];
    FILE* fp = popen("lsb_release -r | awk '{print $2}'", "r");

    if (fp == NULL) {
        strcpy(version, "unknown");
        return version;
    }
    if (fgets(version, sizeof(version) - 1, fp) != NULL) {
        version[strcspn(version, "\n")] = 0;  // remove newline 
    } 
    else {
        strcpy(version, "unknown");
    }

    pclose(fp);
    return version;
}

static char* get_linux_product_type() {
    static char product_type[128];
    FILE* fp = popen("lsb_release -i | awk -F: '{print $2}'", "r");

    if (fp == NULL) {
        strcpy(product_type, "unknown");
        return product_type;
    }
    if (fgets(product_type, sizeof(product_type) - 1, fp) != NULL) {
        product_type[strcspn(product_type, "\n")] = 0; 

        // trim whitespace
        char* start = product_type;
        while (*start == ' ' || *start == '\t') {
            start++;
        }
        memmove(product_type, start, strlen(start) + 1);
    } 
    else {
        strcpy(product_type, "unknown");
    }

    pclose(fp);
    return product_type;
}

static char* get_linux_kernel_version() {
    static char kernel_version[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        strncpy(kernel_version, buffer.release, sizeof(kernel_version));
    } 
    else {
        strcpy(kernel_version, "unknown");
    }

    return kernel_version;
}

static char* get_linux_kernel_type() {
    static char kernel_type[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        strncpy(kernel_type, buffer.sysname, sizeof(kernel_type));
    } 
    else {
        strcpy(kernel_type, "unknown");
    }

    return kernel_type;
}

static char* get_linux_boot_unique_id() {
    static char boot_id[128];
    FILE* fp = fopen("/proc/sys/kernel/random/boot_id", "r");

    if (fp == NULL) {
        strcpy(boot_id, "unknown");
        return boot_id;
    }
    if (fgets(boot_id, sizeof(boot_id) - 1, fp) != NULL) {
        boot_id[strcspn(boot_id, "\n")] = 0;  
    } 
    else {
        strcpy(boot_id, "unknown");
    }

    fclose(fp);
    return boot_id;
}

static char* get_linux_cpu_architecture() {
    static char architecture[128];
    struct utsname buffer;

    if (uname(&buffer) == 0) {
        strncpy(architecture, buffer.machine, sizeof(architecture));
    } 
    else {
        strcpy(architecture, "unknown");
    }

    return architecture;
}

static char* get_linux_machine_host_name() {
    static char hostname[1024];
    hostname[1023] = '\0';

    if (gethostname(hostname, sizeof(hostname) - 1) != 0) {
        strcpy(hostname, "Unknown_Host_Name");
        return hostname;
    }

    struct addrinfo hints, *info, *p;
    int gai_result;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;

    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
        strcpy(hostname, "Unknown_Host_Name");
        return hostname;
    }

    for (p = info; p != NULL; p = p->ai_next) {
        if (p->ai_canonname) {
            strncpy(hostname, p->ai_canonname, sizeof(hostname) - 1);
            hostname[sizeof(hostname) - 1] = '\0';
            break;
        }
    }

    freeaddrinfo(info);
    return hostname;
}

#else

static char* get_unknown_version() {
    static char version[] = "unknown";
    return version;
}

static char* get_unknown_product_type() {
    static char product_type[] = "unknown";
    return product_type;
}

static char* get_unknown_kernel_version() {
    static char kernel_version[] = "unknown";
    return kernel_version;
}

static char* get_unknown_kernel_type() {
    static char kernel_type[] = "unknown";
    return kernel_type;
}

static char* get_unknown_boot_unique_id() {
    static char boot_id[] = "";
    return boot_id;
}

static char* get_unknown_cpu_architecture() {
    static char architecture[] = "unknown";
    return architecture;
}

static char* get_unknown_machine_host_name() {
    static char hostname[] = "Unknown_Host_Name";
    return hostname;
}

#endif


char* sysinfo_product_version() {
#ifdef _WIN32
    return get_windows_version();
#elif __linux__
    return get_linux_version();
#else
    return get_unknown_version();
#endif
}

char* sysinfo_product_type() {
#ifdef _WIN32
    return get_windows_product_type();
#elif __linux__
    return get_linux_product_type();
#else
    return get_unknown_product_type();
#endif
}

char* sysinfo_kernel_version() {
#ifdef _WIN32
    return get_windows_kernel_version();
#elif __linux__
    return get_linux_kernel_version();
#else
    return get_unknown_kernel_version();
#endif
}

char* sysinfo_kernel_type() {
#ifdef _WIN32
    return get_windows_kernel_type();
#elif __linux__
    return get_linux_kernel_type();
#else
    return get_unknown_kernel_type();
#endif
}

char* sysinfo_boot_unique_id() {
#ifdef _WIN32
    return get_windows_boot_unique_id();
#elif __linux__
    return get_linux_boot_unique_id();
#else
    return get_unknown_boot_unique_id();
#endif
}

char* sysinfo_cpu_architecture() {
#ifdef _WIN32
    return get_windows_cpu_architecture();
#elif __linux__
    return get_linux_cpu_architecture();
#else
    return get_unknown_cpu_architecture();
#endif
}

char* sysinfo_machine_host_name() {
#ifdef _WIN32
    return get_windows_machine_host_name();
#elif __linux__
    return get_linux_machine_host_name();
#else
    return get_unknown_machine_host_name();
#endif
}
