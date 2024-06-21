#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <ctype.h>
#include "../encoding/encoding.h"
#include "../string/string.h"
#include "fmt.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif


void __fmt_print(const char* str, ...) {
    va_list args;
    va_start(args, str);
    
#if defined(_WIN32) || defined(_WIN64)
    DWORD written;
    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        wchar_t* wstr = encoding_utf8_to_wchar(arg);
        if (wstr) {
            WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), wstr, wcslen(wstr), &written, NULL);
            WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L"", 1, &written, NULL);  // Space separator
            free(wstr);
        }
    }
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), NULL, 1, &written, NULL);
#else 
    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        printf("%s", arg);  // do not need space seperator
    }
#endif 

    va_end(args);
}

void __fmt_println(const char* str, ...) {
    va_list args;
    va_start(args, str);

#if defined(_WIN32) || defined(_WIN64)
    DWORD written;
    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        wchar_t* wstr = encoding_utf8_to_wchar(arg);
        if (wstr) {
            WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), wstr, wcslen(wstr), &written, NULL);
            WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L" ", 1, &written, NULL); // Space separator
            free(wstr);
        }
    }
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L"\n", 1, &written, NULL); // New line
#else 
    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        printf("%s ", arg);  // Space separator already included
    }
    printf("\n");
#endif 

    va_end(args);
}

void fmt_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wformat = encoding_utf8_to_wchar(format);
    if (wformat) {
        vfwprintf(stdout, wformat, args);
        free(wformat);

        // wchar_t wbuffer[1024];
        // vswprintf(wbuffer, 1024, wformat, args);
        // free(wformat);

        // DWORD written;
        // WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), wbuffer, wcslen(wbuffer), &written, NULL);
    }
#else
    vprintf(format, args);
#endif

    va_end(args);
}

char* __fmt_sprintln(const char* first_arg, ...) {
    va_list args;
    va_start(args, first_arg);

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wstr_total = NULL;
    size_t wstr_total_len = 0;

    wchar_t* wstr = encoding_utf8_to_wchar(first_arg);
    if (wstr) {
        wstr_total_len = wcslen(wstr);
        wstr_total = malloc((wstr_total_len + 1) * sizeof(wchar_t));
        if (!wstr_total) {
            free(wstr);
            return NULL;
        }
        wcscpy(wstr_total, wstr);
        free(wstr);
    }

    const char* arg;
    while ((arg = va_arg(args, const char*)) != NULL) {
        wstr = encoding_utf8_to_wchar(arg);
        if (wstr) {
            size_t new_len = wstr_total_len + wcslen(wstr) + 1;
            wchar_t* new_wstr_total = realloc(wstr_total, (new_len + 1) * sizeof(wchar_t));
            if (!new_wstr_total) {
                free(wstr);
                free(wstr_total);
                return NULL;
            }
            wstr_total = new_wstr_total;
            wcscat(wstr_total, L" ");
            wcscat(wstr_total, wstr);
            wstr_total_len = new_len;
            free(wstr);
        }
    }
    wcscat(wstr_total, L"\n");

    char* result = encoding_wchar_to_utf8(wstr_total);
    free(wstr_total);
#else
    size_t size = 256;
    char* result = malloc(size);
    if (!result) return NULL;

    strcpy(result, first_arg);
    size_t len = strlen(first_arg);

    const char* arg;
    while ((arg = va_arg(args, const char*)) != NULL) {
        size_t arg_len = strlen(arg);
        if (len + arg_len + 2 >= size) {
            size = len + arg_len + 2;
            char* new_result = realloc(result, size);
            if (!new_result) {
                free(result);
                return NULL;
            }
            result = new_result;
        }
        strcat(result, " ");
        strcat(result, arg);
        len += arg_len + 1;
    }

    strcat(result, "\n");

    va_end(args);
#endif

    return result;
}

char* __fmt_sprint(const char* first_arg, ...) {
    va_list args;
    va_start(args, first_arg);

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wstr_total = NULL;
    size_t wstr_total_len = 0;

    wchar_t* wstr = encoding_utf8_to_wchar(first_arg);
    if (wstr) {
        wstr_total_len = wcslen(wstr);
        wstr_total = malloc((wstr_total_len + 1) * sizeof(wchar_t));
        if (!wstr_total) {
            free(wstr);
            return NULL;
        }
        wcscpy(wstr_total, wstr);
        free(wstr);
    }

    const char* arg;
    while ((arg = va_arg(args, const char*)) != NULL) {
        wstr = encoding_utf8_to_wchar(arg);
        if (wstr) {
            size_t new_len = wstr_total_len + wcslen(wstr) + 1;
            wchar_t* new_wstr_total = realloc(wstr_total, (new_len + 1) * sizeof(wchar_t));
            if (!new_wstr_total) {
                free(wstr);
                free(wstr_total);
                return NULL;
            }
            wstr_total = new_wstr_total;
            wcscat(wstr_total, L" ");
            wcscat(wstr_total, wstr);
            wstr_total_len = new_len;
            free(wstr);
        }
    }

    char* result = encoding_wchar_to_utf8(wstr_total);
    free(wstr_total);
#else
    size_t size = 256;
    char* result = malloc(size);
    if (!result) return NULL;

    strcpy(result, first_arg);
    size_t len = strlen(first_arg);

    const char* arg;
    while ((arg = va_arg(args, const char*)) != NULL) {
        size_t arg_len = strlen(arg);
        if (len + arg_len + 2 >= size) {
            size = len + arg_len + 2;
            char* new_result = realloc(result, size);
            if (!new_result) {
                free(result);
                return NULL;
            }
            result = new_result;
        }
        strcat(result, " ");
        strcat(result, arg);
        len += arg_len + 1;
    }

    va_end(args);
#endif

    return result;
}

char* fmt_sprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char format_buffer[2048];
    vsnprintf(format_buffer, 2048, format, args); // Use args as-is

    char* result = string_strdup(format_buffer); // Duplicate the formatted string

    va_end(args);
    return result;
}

int fmt_scan(char** output) {
    char buffer[1024];
    int result = 0;

    // Read from standard input character by character
    size_t i = 0;
    for (; i < sizeof(buffer) - 1; ++i) {
        int ch = getchar();
        if (ch == EOF || ch == '\n' || ch == ' ') {
            break;
        }
        buffer[i] = (char)ch;
    }
    buffer[i] = '\0'; // Null-terminate the string

    if (i == 0) {
        return (feof(stdin)) ? 0 : -1; // Handle EOF or error
    }

#if defined(_WIN32) || defined(_WIN64)
    // Convert input from console's encoding to UTF-8
    wchar_t* wbuffer = encoding_utf8_to_wchar(buffer);
    if (!wbuffer) {
        return -1; // Conversion error
    }
    *output = encoding_wchar_to_utf8(wbuffer);
    free(wbuffer);
#else
    // On non-Windows platforms, assume the input is already UTF-8
    *output = string_strdup(buffer);
#endif

    return result; // Return the result of the reading process
}

int fmt_scanln(char** output) {
    char buffer[1024];
    int result = 0;

    // Read from standard input character by character
    size_t i = 0;
    for (; i < sizeof(buffer) - 1; ++i) {
        int ch = getchar();
        if (ch == EOF || ch == '\n' || ch == ' ') {
            if (ch == ' ') {
                // Consume the rest of the line
                while ((ch = getchar()) != '\n' && ch != EOF);
            }
            break;
        }
        buffer[i] = (char)ch;
    }
    buffer[i] = '\0'; // Null-terminate the string

    if (i == 0) {
        return (feof(stdin)) ? 0 : -1; // Handle EOF or error
    }

#if defined(_WIN32) || defined(_WIN64)
    // Convert input from console's encoding to UTF-8
    wchar_t* wbuffer = encoding_utf8_to_wchar(buffer);
    if (!wbuffer) {
        return -1; // Conversion error
    }
    *output = encoding_wchar_to_utf8(wbuffer);
    free(wbuffer);
#else
    // On non-Windows platforms, assume the input is already UTF-8
    *output = string_strdup(buffer);
#endif

    return result; // Return the result of the reading process
}

int fmt_scanf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    int items_scanned = vscanf(format, args);

    va_end(args);
    return items_scanned;
}

int __fmt_fprint(FILE* stream, const char* str, ...) {
    if (!stream) {
        fprintf(stderr, "Error: Invalid stream object.\n");
        return -1;
    }
    va_list args;
    va_start(args, str);

    int bytes_written = 0;
    int result;

    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        result = fwrite(arg, sizeof(char), strlen(arg), stream);
        if (result < 0) {
            fprintf(stderr, "Error writing to stream.\n");
            va_end(args);
            return -1;
        }
        bytes_written += result;
    }

    va_end(args);
    return bytes_written;
}

int __fmt_fprintln(FILE* stream, const char* str, ...) {
    if (!stream) {
        fprintf(stderr, "Error: Invalid stream object.\n");
        return -1;
    }

    va_list args;
    va_start(args, str);

    int bytes_written = 0;
    int result;

    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        result = fwrite(arg, sizeof(char), strlen(arg), stream);
        if (result < 0) {
            fprintf(stderr, "Error writing to stream.\n");
            va_end(args);
            return -1;
        }
        bytes_written += result;

        const char* space = " ";
        result = fwrite(space, sizeof(char), strlen(space), stream);

        if (result < 0) {
            fprintf(stderr, "Error writing space to stream.\n");
            va_end(args);
            return -1;
        }

        bytes_written += result;
    }

    const char* newline = "\n";
    result = fwrite(newline, sizeof(char), strlen(newline), stream);

    if (result < 0) {
        fprintf(stderr, "Error writing newline to stream.\n");
        va_end(args);
        return -1;
    }

    bytes_written += result;

    va_end(args);
    return bytes_written;
}

int fmt_fprintf(FILE* stream, const char* format, ...) {
    if (!stream || !format) {
        fprintf(stderr, "Error: Invalid arguments for fmt_fprintf.\n");
        return -1;
    }

    va_list args;
    va_start(args, format);

    char buffer[2048]; // Define a suitable buffer size for formatted output
    int formatted_bytes = vsnprintf(buffer, sizeof(buffer), format, args);
    if (formatted_bytes < 0) {
        fprintf(stderr, "Error formatting string for fmt_fprintf.\n");
        va_end(args);
        return -1;
    }

    int bytes_written = fwrite(buffer, sizeof(char), formatted_bytes, stream);
    if (bytes_written < formatted_bytes) {
        fprintf(stderr, "Error writing to file in fmt_fprintf.\n");
        va_end(args);
        return -1;
    }

    va_end(args);
    return bytes_written;
}

int fmt_fscan(FILE* stream, const char* format, ...) {
    if (!stream || !format) {
        fprintf(stderr, "Error: Invalid arguments for fmt_fscan.\n");
        return -1;
    }

    va_list args;
    va_start(args, format);

    // Implementation: Use fscanf or a similar function to read and parse the input
    int items_scanned = vfscanf(stream, format, args);

    va_end(args);
    return items_scanned;
}

int fmt_fscanln(FILE* stream, const char* format, ...) {
    if (!stream || !format) {
        fprintf(stderr, "Error: Invalid arguments for fmt_fscanln.\n");
        return -1;
    }

    va_list args;
    va_start(args, format);

    // Implementation: You'll need a custom implementation to handle newline.
    // Read the line first, then parse it using sscanf.
    char line[1024];
    if (fgets(line, sizeof(line), stream) == NULL) {
        return (feof(stream)) ? 0 : -1; // EOF or error
    }

    // Parse the line
    int items_scanned = vsscanf(line, format, args);

    va_end(args);
    return items_scanned;
}

int fmt_fscanf(FILE* stream, const char* format, ...) {
    if (!stream || !format) {
        fprintf(stderr, "Error: Invalid arguments for fmt_fscanf.\n");
        return -1;
    }

    va_list args;
    va_start(args, format);

    // Use vfscanf to read and parse the input
    int items_scanned = vfscanf(stream, format, args);

    va_end(args);
    return items_scanned;
}