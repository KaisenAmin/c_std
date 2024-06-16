#include "sysinfo.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

#include <windows.h>


char* get_windows_version() {
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

#elif __linux__

#include <sys/utsname.h>


char* get_linux_version() {
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

#else

char* get_unknown_version() {
    static char version[] = "unknown";
    return version;
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