/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class fmt
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in fmt.c (file-level convention).
 *
 * Go-style printf/scanf surface. The `fmt_print` / `fmt_println` /
 * `fmt_sprint` / `fmt_sprintln` / `fmt_fprint` / `fmt_fprintln`
 * variadic macros expand to internal `fmt_*_impl` functions terminated
 * with a `NULL` sentinel — the C printf-style `fmt_printf` /
 * `fmt_sprintf` / `fmt_fprintf` are also available.
 */

#ifndef FMT_H_
#define FMT_H_

#include <stdio.h>
#include "../file_io/file_writer.h"
#include "../file_io/file_reader.h"

#ifdef __cplusplus
extern "C" {
#endif


/* #define FMT_LOGGING_ENABLE */

#ifdef FMT_LOGGING_ENABLE
    #define FMT_LOG(...) \
        do { fprintf(stderr, "[FMT LOG] "); fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)
#else
    #define FMT_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Variadic-argument list terminator + Go-style wrapper macros        */
/* ------------------------------------------------------------------ */

#define FMT_END_ARGS (NULL)

#define fmt_print(...)             fmt_print_impl     (__VA_ARGS__, NULL)
#define fmt_println(...)           fmt_println_impl   (__VA_ARGS__, NULL)
#define fmt_sprint(...)            fmt_sprint_impl    (__VA_ARGS__, NULL)
#define fmt_sprintln(...)          fmt_sprintln_impl  (__VA_ARGS__, NULL)
#define fmt_fprint(stream, ...)    fmt_fprint_impl    (stream, __VA_ARGS__, NULL)
#define fmt_fprintln(stream, ...)  fmt_fprintln_impl  (stream, __VA_ARGS__, NULL)


/* ------------------------------------------------------------------ */
/* stdout writers — Go-style variadic                                 */
/* ------------------------------------------------------------------ */

void   fmt_print_impl          (const char* str, ...);
void   fmt_println_impl        (const char* str, ...);


/* ------------------------------------------------------------------ */
/* stdout writers — C printf-style                                    */
/* ------------------------------------------------------------------ */

void   fmt_printf           (const char* format, ...);


/* ------------------------------------------------------------------ */
/* String builders — Go-style variadic                                */
/* ------------------------------------------------------------------ */

char*  fmt_sprint_impl         (const char* str, ...);
char*  fmt_sprintln_impl       (const char* str, ...);


/* ------------------------------------------------------------------ */
/* String builders — C printf-style                                   */
/* ------------------------------------------------------------------ */

char*  fmt_sprintf          (const char* format, ...);


/* ------------------------------------------------------------------ */
/* stdin scanners                                                     */
/* ------------------------------------------------------------------ */

int    fmt_scan             (char** output);
int    fmt_scanln           (char** output);
int    fmt_scanf            (const char* format, ...);


/* ------------------------------------------------------------------ */
/* Stream writers — Go-style variadic                                 */
/* ------------------------------------------------------------------ */

int    fmt_fprint_impl         (FILE* stream, const char* str, ...);
int    fmt_fprintln_impl       (FILE* stream, const char* str, ...);


/* ------------------------------------------------------------------ */
/* Stream writers — C printf-style                                    */
/* ------------------------------------------------------------------ */

int    fmt_fprintf          (FILE* stream, const char* format, ...);


/* ------------------------------------------------------------------ */
/* Stream scanners                                                    */
/* ------------------------------------------------------------------ */

int    fmt_fscan            (FILE* stream, const char* format, ...);
int    fmt_fscanln          (FILE* stream, const char* format, ...);
int    fmt_fscanf           (FILE* stream, const char* format, ...);


#ifdef __cplusplus
}
#endif

#endif 
