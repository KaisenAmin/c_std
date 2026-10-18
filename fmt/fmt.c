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



#if defined(_WIN32) || defined(_WIN64)
/*
 * Writes UTF-8 text to stdout on Windows.
 *
 * On a real console the text is converted to UTF-16 and written with
 * WriteConsoleW, so any Unicode text displays correctly whatever the console
 * code page is.
 */
static void fmt_write_stdout(const char* utf8, size_t len) {
    if (!utf8 || len == 0) {
        return;
    }
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (h != NULL && h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &mode) && len <= (size_t)0x7FFFFFFF) {
        int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8, (int)len, NULL, 0);
        if (wlen > 0) {
            wchar_t* wide = (wchar_t*)malloc((size_t)wlen * sizeof(wchar_t));
            if (wide) {
                MultiByteToWideChar(CP_UTF8, 0, utf8, (int)len, wide, wlen);
                fflush(stdout);
                DWORD written = 0;
                BOOL ok = WriteConsoleW(h, wide, (DWORD)wlen, &written, NULL);
                free(wide);
                if (ok) {
                    return;
                }
            }
        }
    }
    fwrite(utf8, 1, len, stdout);
}
#else
static void fmt_write_stdout(const char* utf8, size_t len) {
    if (utf8 && len) {
        fwrite(utf8, 1, len, stdout);
    }
}
#endif


/**
 * @brief Prints a formatted string to the console without a newline.
 * Handles Unicode on Windows and uses standard output on other platforms.
 */
void fmt_print_impl(const char* str, ...) {
    va_list args;
    va_start(args, str);

    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        fmt_write_stdout(arg, strlen(arg));
    }

    va_end(args);
}


/**
 * @brief Prints a formatted string to the console with a newline.
 * Adds a space separator between arguments and handles Unicode on Windows.
 */
void fmt_println_impl(const char* str, ...) {
    va_list args;
    va_start(args, str);

    bool first = true;
    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        if (!first) {
            fmt_write_stdout(" ", 1);  // separator between args
        }
        first = false;
        fmt_write_stdout(arg, strlen(arg));
    }
    fmt_write_stdout("\n", 1);

    va_end(args);
}


/**
 * @brief Prints a formatted string to the standard output.
 *
 * This function prints a formatted string to the standard output (stdout). It works similarly to the standard `printf` function,
 * but with Unicode-correct console output on Windows platforms.
 *
 * The text is formatted with the narrow `vsnprintf`, so `%s` always means a UTF-8 `char*` on every compiler and C runtime
 * (the previous wide `vfwprintf` path read `%s` arguments as `wchar_t*` on MSVC/UCRT and printed garbage). The resulting
 * UTF-8 text is then written like `fmt_print` does: through the console API on a Windows console, as bytes otherwise.
 *
 * @param format The format string that specifies how to format the output. It supports standard printf-style formatting.
 * @param ... Additional arguments to be formatted according to the format string.
 */
void fmt_printf(const char* format, ...) {
    if (!format) {
        return;
    }
    va_list args;
    va_start(args, format);

#if defined(_WIN32) || defined(_WIN64)
    char stackBuf[1024];
    va_list copy;
    va_copy(copy, args);
    int needed = vsnprintf(stackBuf, sizeof(stackBuf), format, copy);
    va_end(copy);
    if (needed >= 0) {
        if ((size_t)needed < sizeof(stackBuf)) {
            fmt_write_stdout(stackBuf, (size_t)needed);
        }
        else {
            char* heapBuf = (char*)malloc((size_t)needed + 1);
            if (heapBuf) {
                vsnprintf(heapBuf, (size_t)needed + 1, format, args);
                fmt_write_stdout(heapBuf, (size_t)needed);
                free(heapBuf);
            }
        }
    }
#else
    vprintf(format, args);
#endif

    va_end(args);
}


/**
 * @brief Constructs a formatted string with spaces between arguments and a newline at the end.
 * Returns a dynamically allocated UTF-8 string. The arguments are UTF-8 already, so they are
 * joined byte for byte on every platform (the former UTF-16 round trip on Windows only
 * corrupted invalid UTF-8 and cost an extra allocation per argument).
 */
char* fmt_sprintln_impl(const char* first_arg, ...) {
    va_list args;
    va_start(args, first_arg);

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
    return result;
}


/**
 * @brief Constructs a formatted string with spaces between arguments but without a newline at the end.
 * Returns a dynamically allocated UTF-8 string (arguments are joined byte for byte on every platform).
 */
char* fmt_sprint_impl(const char* first_arg, ...) {
    va_list args;
    va_start(args, first_arg);

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


/* Reads one token from stdin into a heap string. Leading blanks are skipped
 * (and leading newlines too when `stopAtNewline` is false); the token ends at
 * whitespace or EOF. With `stopAtNewline` the rest of the line after the
 * token is consumed. No length limit (the old 1023-byte buffer split long
 * tokens). Returns 0 and sets *output, or -1 with *output = NULL on EOF
 * before any token, an empty line (scanln), or out of memory. */
static int fmt_scan_token(char** output, bool stopAtNewline) {
    if (!output) {
        return -1;
    }
    *output = NULL;

    int ch = getchar();
    while (ch == ' ' || ch == '\t' || ch == '\r' || (!stopAtNewline && ch == '\n')) {
        ch = getchar();
    }
    if (ch == EOF || ch == '\n') {
        return -1;   /* EOF (used to return 0 = success with *output unset) or empty line */
    }

    size_t cap = 64, len = 0;
    char* buf = (char*)malloc(cap);
    if (!buf) {
        return -1;
    }
    while (ch != EOF && ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n') {
        if (len + 1 >= cap) {
            char* grown = (char*)realloc(buf, cap * 2);
            if (!grown) {
                free(buf);
                return -1;
            }
            buf = grown;
            cap *= 2;
        }
        buf[len++] = (char)ch;   /* kept byte for byte (no UTF-16 round trip) */
        ch = getchar();
    }
    buf[len] = '\0';

    if (stopAtNewline) {
        while (ch != '\n' && ch != EOF) {
            ch = getchar();   /* drop the rest of the line */
        }
    }
    *output = buf;
    return 0;
}


/**
 * @brief Scans one whitespace-delimited word from standard input.
 *
 * Leading spaces, tabs and newlines are skipped, then characters are read up to the next
 * whitespace or EOF. The word is stored in a dynamically allocated string, which must be freed
 * by the caller.
 *
 * @param output A pointer to a `char*` that receives the allocated string (NULL on failure).
 *
 * @return 0 on success; -1 if EOF is reached before any word (this used to return 0 and leave
 * `*output` unset), on a NULL `output`, or when memory runs out.
 */
int fmt_scan(char** output) {
    return fmt_scan_token(output, false);
}


/**
 * @brief Reads the first word of the next input line from standard input.
 *
 * Leading spaces and tabs are skipped, the word ends at whitespace or EOF, and the rest of the
 * line is consumed. The caller is responsible for freeing the allocated memory.
 *
 * @param output A pointer to a char* that receives the allocated string (NULL on failure).
 * @return 0 on success; -1 for an empty line, EOF before any word, a NULL `output`, or out of memory.
 */
int fmt_scanln(char** output) {
    return fmt_scan_token(output, true);
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
int fmt_fprint_impl(FILE* stream, const char* str, ...) {
    if (!stream) {
        FMT_LOG("[fmt_fprint_impl] Error: Invalid stream object.\n");
        return -1;
    }
    va_list args;
    va_start(args, str);

    size_t bytes_written = 0;

    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        size_t arg_len = strlen(arg);
        size_t written = fwrite(arg, 1, arg_len, stream);
        if (written != arg_len) {
            FMT_LOG("[fmt_fprint_impl] Short write to stream.\n");
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
int fmt_fprintln_impl(FILE* stream, const char* str, ...) {
    if (!stream) {
        FMT_LOG("[fmt_fprintln_impl] Error: Invalid stream object.\n");
        return -1;
    }

    va_list args;
    va_start(args, str);

    size_t bytes_written = 0;
    bool first = true;

    for (const char* arg = str; arg != NULL; arg = va_arg(args, const char*)) {
        if (!first) {
            if (fputc(' ', stream) == EOF) {
                FMT_LOG("[fmt_fprintln_impl] Error writing space.\n");
                va_end(args);
                return -1;
            }
            bytes_written++;
        }
        first = false;

        size_t arg_len = strlen(arg);
        size_t written = fwrite(arg, 1, arg_len, stream);
        if (written != arg_len) {
            FMT_LOG("[fmt_fprintln_impl] Short write to stream.\n");
            va_end(args);
            return -1;
        }
        bytes_written += written;
    }

    if (fputc('\n', stream) == EOF) {
        FMT_LOG("[fmt_fprintln_impl] Error writing newline.\n");
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
 * and parses it according to the provided format string. The whole line is
 * read however long it is (a fixed 1024-byte buffer used to split longer
 * lines, and the next call then parsed the rest as if it were a new line).
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

    size_t cap = 256;
    size_t len = 0;
    char* line = (char*)malloc(cap);
    if (!line) {
        return -1;
    }

    int ch;
    while ((ch = getc(stream)) != EOF) {
        if (len + 2 >= cap) {   /* room for this byte, a '\n' and the '\0' */
            char* grown = (char*)realloc(line, cap * 2);
            if (!grown) {
                free(line);
                return -1;
            }
            line = grown;
            cap *= 2;
        }
        line[len++] = (char)ch;   /* the '\n' is kept, as fgets kept it */
        if (ch == '\n') {
            break;
        }
    }
    if (ferror(stream)) {
        free(line);
        return -1;
    }
    if (len == 0) {
        free(line);
        return 0;   /* EOF */
    }
    line[len] = '\0';

    va_list args;
    va_start(args, format);
    int items_scanned = vsscanf(line, format, args);
    va_end(args);

    free(line);
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
