/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class fmt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <wchar.h>
#include <ctype.h>
#include "../encoding/encoding.h"
#include "../string/std_string.h"
#include "fmt.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif



/**
 * @brief Prints a formatted string to the console without a newline.
 * Handles Unicode on Windows and uses standard output on other platforms.
 */
void __fmt_print(const char* str, ...) {
    va_list args;
    va_start(args, str);

#if defined(_WIN32) || defined(_WIN64)
    DWORD written;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        wchar_t* wstr = encoding_utf8_to_wchar(arg);
        if (wstr) {
            WriteConsoleW(hOut, wstr, (DWORD)wcslen(wstr), &written, NULL);
            free(wstr);
        }
    }
#else
    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        fputs(arg, stdout);
    }
#endif

    va_end(args);
}


/**
 * @brief Prints a formatted string to the console with a newline.
 * Adds a space separator between arguments and handles Unicode on Windows.
 */
void __fmt_println(const char* str, ...) {
    va_list args;
    va_start(args, str);

#if defined(_WIN32) || defined(_WIN64)
    DWORD written;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    bool first = true;
    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        if (!first) {
            WriteConsoleW(hOut, L" ", 1, &written, NULL);  // separator between args
        }
        first = false;

        wchar_t* wstr = encoding_utf8_to_wchar(arg);
        if (wstr) {
            WriteConsoleW(hOut, wstr, (DWORD)wcslen(wstr), &written, NULL);
            free(wstr);
        }
    }
    WriteConsoleW(hOut, L"\n", 1, &written, NULL);
#else
    bool first = true;
    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        if (!first) {
            fputc(' ', stdout);
        }
        first = false;
        fputs(arg, stdout);
    }
    fputc('\n', stdout);
#endif

    va_end(args);
}


/**
 * @brief Prints a formatted string to the standard output.
 *
 * This function prints a formatted string to the standard output (stdout). It works similarly to the standard `printf` function,
 * but with additional support for wide characters and Unicode on Windows platforms.
 *
 * On Windows, the function converts the format string from UTF-8 to wide characters (UTF-16) and uses `vfwprintf` to print the formatted output.
 * On other platforms, it uses `vprintf` directly with the provided format string.
 *
 * @param format The format string that specifies how to format the output. It supports standard printf-style formatting.
 * @param ... Additional arguments to be formatted according to the format string.
 */
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


/**
 * @brief Constructs a formatted string with spaces between arguments and a newline at the end.
 * Handles Unicode on Windows and returns a dynamically allocated UTF-8 string.
 */
char* __fmt_sprintln(const char* first_arg, ...) {
    va_list args;
    va_start(args, first_arg);

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wstr_total = NULL;
    size_t wstr_total_len = 0;

    wchar_t* wstr = encoding_utf8_to_wchar(first_arg);
    if (wstr) {
        wstr_total_len = wcslen(wstr);
        wstr_total = (wchar_t*)malloc((wstr_total_len + 1) * sizeof(wchar_t));

        if (!wstr_total) {
            free(wstr);
            va_end(args);
            return NULL;
        }

        wcscpy(wstr_total, wstr);
        free(wstr);
    } 
    else {
        
        wstr_total = (wchar_t*)malloc(sizeof(wchar_t));
        if (!wstr_total) { va_end(args); return NULL; }
        wstr_total[0] = L'\0';
    }

    const char* arg;
    while ((arg = va_arg(args, const char*)) != NULL) {
        wstr = encoding_utf8_to_wchar(arg);
        if (wstr) {
            size_t new_len = wstr_total_len + wcslen(wstr) + 1;
            wchar_t* new_wstr_total = (wchar_t*) realloc(wstr_total, (new_len + 1) * sizeof(wchar_t));

            if (!new_wstr_total) {
                free(wstr);
                free(wstr_total);
                va_end(args);
                return NULL;
            }

            wstr_total = new_wstr_total;
            wcscat(wstr_total, L" ");
            wcscat(wstr_total, wstr);
            wstr_total_len = new_len;

            free(wstr);
        }
    }

    wchar_t* with_nl = (wchar_t*)realloc(wstr_total, (wstr_total_len + 2) * sizeof(wchar_t));
    if (!with_nl) {
        free(wstr_total);
        va_end(args);
        return NULL;
    }
    wstr_total = with_nl;
    wcscat(wstr_total, L"\n");

    char* result = encoding_wchar_to_utf8(wstr_total);
    free(wstr_total);

    va_end(args);
#else
    size_t len = strlen(first_arg);
    size_t cap = len + 16;
    char* result = (char*)malloc(cap);
    
    if (!result) { 
        va_end(args); 
        return NULL; 
    }
    memcpy(result, first_arg, len + 1);

    const char* arg;
    while ((arg = va_arg(args, const char*)) != NULL) {
        size_t arg_len = strlen(arg);
        // Need room for " " + arg + "\n" + '\0' eventually = arg_len + 3.
        if (len + arg_len + 3 > cap) {
            cap = (len + arg_len + 3) * 2;
            char* new_result = (char*)realloc(result, cap);
            if (!new_result) { free(result); va_end(args); return NULL; }
            result = new_result;
        }
        result[len++] = ' ';
        memcpy(result + len, arg, arg_len);
        len += arg_len;
    }

    if (len + 2 > cap) {
        char* new_result = (char*)realloc(result, len + 2);
        if (!new_result) { 
            free(result); 
            va_end(args); 
            return NULL; 
        }
        result = new_result;
    }
    result[len++] = '\n';
    result[len] = '\0';

    va_end(args);
#endif

    return result;
}


/**
 * @brief Constructs a formatted string with spaces between arguments but without a newline at the end.
 * Handles Unicode on Windows and returns a dynamically allocated UTF-8 string.
 */
char* __fmt_sprint(const char* first_arg, ...) {
    va_list args;
    va_start(args, first_arg);

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wstr_total = NULL;
    size_t wstr_total_len = 0;

    wchar_t* wstr = encoding_utf8_to_wchar(first_arg);
    if (wstr) {
        wstr_total_len = wcslen(wstr);
        wstr_total = (wchar_t*) malloc((wstr_total_len + 1) * sizeof(wchar_t));

        if (!wstr_total) { 
            free(wstr); 
            va_end(args); 
            return NULL; 
        }

        wcscpy(wstr_total, wstr);
        free(wstr);
    } 
    else {
        wstr_total = (wchar_t*)malloc(sizeof(wchar_t));
        if (!wstr_total) { va_end(args); return NULL; }
        wstr_total[0] = L'\0';
    }

    const char* arg;
    while ((arg = va_arg(args, const char*)) != NULL) {
        wstr = encoding_utf8_to_wchar(arg);
        if (wstr) {
            size_t new_len = wstr_total_len + wcslen(wstr) + 1;
            wchar_t* new_wstr_total = (wchar_t*) realloc(wstr_total, (new_len + 1) * sizeof(wchar_t));

            if (!new_wstr_total) {
                free(wstr);
                free(wstr_total);
                va_end(args);
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
    va_end(args);
#else
    size_t len = strlen(first_arg);
    size_t cap = len + 16;
    char* result = (char*)malloc(cap);

    if (!result) { 
        va_end(args); 
        return NULL; 
    }
    memcpy(result, first_arg, len + 1);

    const char* arg;
    while ((arg = va_arg(args, const char*)) != NULL) {
        size_t arg_len = strlen(arg);
        // Need " " + arg + '\0' = arg_len + 2.
        if (len + arg_len + 2 > cap) {
            cap = (len + arg_len + 2) * 2;
            char* new_result = (char*)realloc(result, cap);
            if (!new_result) { free(result); va_end(args); return NULL; }
            result = new_result;
        }
        result[len++] = ' ';
        memcpy(result + len, arg, arg_len);
        len += arg_len;
    }
    result[len] = '\0';

    va_end(args);
#endif

    return result;
}


/**
 * @brief Formats a string and returns it as a dynamically allocated string.
 *
 * This function formats a string according to the specified format string and additional arguments,
 * similar to `sprintf`. The resulting string is dynamically allocated and must be freed by the caller.
 *
 * @param format The format string that specifies how to format the output.
 * @param ... Additional arguments to be formatted according to the format string.
 * 
 * @return A pointer to the dynamically allocated string containing the formatted text.
 * The caller is responsible for freeing this memory. Returns `NULL` if memory allocation fails.
 */
char* fmt_sprintf(const char* format, ...) {
    if (!format) return NULL;

    va_list args1, args2;
    va_start(args1, format);
    va_copy(args2, args1);

    int needed = vsnprintf(NULL, 0, format, args1);
    va_end(args1);
    if (needed < 0) {
        va_end(args2);
        return NULL;
    }

    char* result = (char*)malloc((size_t)needed + 1);
    if (!result) {
        va_end(args2);
        return NULL;
    }
    vsnprintf(result, (size_t)needed + 1, format, args2);
    va_end(args2);

    return result;
}


/**
 * @brief Scans a string from standard input until space, newline, or EOF.
 *
 * This function reads input from standard input character by character, stopping at a space, newline, or EOF.
 * The input is stored in a dynamically allocated string, which must be freed by the caller.
 *
 * @param output A pointer to a `char*` that will be set to the dynamically allocated string containing the input.
 * The caller is responsible for freeing this memory.
 * 
 * @return 0 on success, or a negative value if an error occurred. If EOF is reached before any characters are read, the function returns 0.
 */
int fmt_scan(char** output) {
    char buffer[1024];
    int result = 0;
    size_t i = 0;

    for (; i < sizeof(buffer) - 1; ++i) {
        int ch = getchar();
        if (ch == EOF || ch == '\n' || ch == ' ') {
            break;
        }
        buffer[i] = (char)ch;
    }
    buffer[i] = '\0'; 

    if (i == 0) {
        return (feof(stdin)) ? 0 : -1; 
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wbuffer = encoding_utf8_to_wchar(buffer);
    if (!wbuffer) {
        return -1; 
    }
    *output = encoding_wchar_to_utf8(wbuffer);
    free(wbuffer);
#else
    *output = string_strdup(buffer);
#endif

    return result; // Return the result of the reading process
}


/**
 * @brief Reads a line of input from the standard input until a newline or space.
 *
 * This function reads a line from standard input, stopping at a newline or space,
 * and stores it in a dynamically allocated string. The caller is responsible for freeing the allocated memory.
 *
 * @param output A pointer to a char* that will be set to the dynamically allocated string containing the input.
 * @return 0 on success, or a negative value if an error occurred.
 */
int fmt_scanln(char** output) {
    char buffer[1024];
    int result = 0;

    // Read from standard input character by character
    size_t i = 0;
    for (; i < sizeof(buffer) - 1; ++i) {
        int ch = getchar();
        if (ch == EOF || ch == '\n' || ch == ' ') {
            if (ch == ' ') {
                while ((ch = getchar()) != '\n' && ch != EOF);
            }
            break;
        }
        buffer[i] = (char)ch;
    }
    buffer[i] = '\0';

    if (i == 0) {
        return (feof(stdin)) ? 0 : -1; 
    }

#if defined(_WIN32) || defined(_WIN64)
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


/**
 * @brief Scans formatted input from the standard input.
 *
 * This function reads input from the standard input according to the specified format,
 * similarly to the standard `scanf` function.
 *
 * @param format The format string specifying how to parse the input.
 * @param ... Additional arguments corresponding to the format string to store the parsed input.
 * 
 * @return The number of input items successfully matched and assigned, or a negative value if an error occurred.
 */
int fmt_scanf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    int items_scanned = vscanf(format, args);

    va_end(args);
    return items_scanned;
}


/**
 * @brief Writes a formatted string to the specified file stream without adding a newline.
 * Handles multiple arguments, writing them sequentially to the stream.
 */
int __fmt_fprint(FILE* stream, const char* str, ...) {
    if (!stream) {
        FMT_LOG("[__fmt_fprint] Error: Invalid stream object.\n");
        return -1;
    }
    va_list args;
    va_start(args, str);

    size_t bytes_written = 0;

    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        size_t arg_len = strlen(arg);
        size_t written = fwrite(arg, 1, arg_len, stream);
        if (written != arg_len) {
            FMT_LOG("[__fmt_fprint] Short write to stream.\n");
            va_end(args);
            return -1;
        }
        bytes_written += written;
    }

    va_end(args);
    return (int)bytes_written;
}


/**
 * @brief Writes a formatted string to the specified file stream, adding spaces and a newline.
 * Handles multiple arguments, writing them sequentially with a space separator and a newline at the end.
 */
int __fmt_fprintln(FILE* stream, const char* str, ...) {
    if (!stream) {
        FMT_LOG("[__fmt_fprintln] Error: Invalid stream object.\n");
        return -1;
    }

    va_list args;
    va_start(args, str);

    size_t bytes_written = 0;
    bool first = true;

    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        if (!first) {
            if (fputc(' ', stream) == EOF) {
                FMT_LOG("[__fmt_fprintln] Error writing space.\n");
                va_end(args);
                return -1;
            }
            bytes_written++;
        }
        first = false;

        size_t arg_len = strlen(arg);
        size_t written = fwrite(arg, 1, arg_len, stream);
        if (written != arg_len) {
            FMT_LOG("[__fmt_fprintln] Short write to stream.\n");
            va_end(args);
            return -1;
        }
        bytes_written += written;
    }

    if (fputc('\n', stream) == EOF) {
        FMT_LOG("[__fmt_fprintln] Error writing newline.\n");
        va_end(args);
        return -1;
    }
    bytes_written++;

    va_end(args);
    return (int)bytes_written;
}


/**
 * @brief Outputs formatted text to the given file stream.
 *
 * This function behaves similarly to `fprintf` but is part of the custom formatting library.
 * It formats a string according to the specified format and writes it to the provided file stream.
 *
 * @param stream The file stream to write the formatted output to.
 * @param format The format string that specifies how to format the output.
 * @param ... Additional arguments to be formatted and written according to the format string.
 * 
 * @return The number of bytes successfully written, or a negative value if an error occurred.
 */
int fmt_fprintf(FILE* stream, const char* format, ...) {
    if (!stream || !format) {
        FMT_LOG("[fmt_fprintf] Error: Invalid arguments for fmt_fprintf.\n");
        return -1;
    }

    va_list args;
    va_start(args, format);

    int bytes_written = vfprintf(stream, format, args);
    va_end(args);

    if (bytes_written < 0) {
        FMT_LOG("[fmt_fprintf] Error formatting/writing string.\n");
        return -1;
    }
    return bytes_written;
}


/**
 * @brief Scans formatted input from a file stream.
 *
 * This function reads formatted input from the specified file stream
 * based on the provided format string. It works similarly to `fscanf`.
 *
 * @param stream The file stream to read from.
 * @param format The format string specifying how to parse the input.
 * @param ... Additional arguments that correspond to the format string.
 * 
 * @return The number of input items successfully matched and assigned, or a negative value if an error occurred.
 */
int fmt_fscan(FILE* stream, const char* format, ...) {
    if (!stream || !format) {
        FMT_LOG("[fmt_fscan] Error: Invalid arguments for fmt_fscan.\n");
        return -1;
    }

    va_list args;
    va_start(args, format);

    int items_scanned = vfscanf(stream, format, args);

    va_end(args);
    return items_scanned;
}


/**
 * @brief Scans formatted input from a file stream, reading one line at a time.
 *
 * This function reads a single line of input from the specified file stream
 * and parses it according to the provided format string.
 *
 * @param stream The file stream to read from.
 * @param format The format string specifying how to parse the input.
 * @param ... Additional arguments that correspond to the format string.
 * 
 * @return The number of input items successfully matched and assigned, or a negative value if an error occurred.
 */
int fmt_fscanln(FILE* stream, const char* format, ...) {
    if (!stream || !format) {
        FMT_LOG("[fmt_fscanln] Error: Invalid arguments for fmt_fscanln.\n");
        return -1;
    }

    va_list args;
    va_start(args, format);

    char line[1024];
    if (fgets(line, sizeof(line), stream) == NULL) {
        return (feof(stream)) ? 0 : -1; // EOF or error
    }

    int items_scanned = vsscanf(line, format, args);

    va_end(args);
    return items_scanned;
}


/**
 * @brief Scans formatted input from a file stream.
 *
 * This function reads formatted input from the specified file stream
 * based on the provided format string. It works similarly to `fscanf`.
 *
 * @param stream The file stream to read from.
 * @param format The format string specifying how to parse the input.
 * @param ... Additional arguments that correspond to the format string.
 * 
 * @return The number of input items successfully matched and assigned, or a negative value if an error occurred.
 */
int fmt_fscanf(FILE* stream, const char* format, ...) {
    if (!stream || !format) {
        FMT_LOG("[fmt_fscanf] Error: Invalid arguments for fmt_fscanf.\n");
        return -1;
    }

    va_list args;
    va_start(args, format);

    int items_scanned = vfscanf(stream, format, args);

    va_end(args);
    return items_scanned;
}