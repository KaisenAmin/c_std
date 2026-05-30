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
 * variadic macros expand to internal `__fmt_*` functions terminated
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
    #define FMT_LOG(fmt, ...) \
        do { fprintf(stderr, "[FMT LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define FMT_LOG(...) do { } while (0)
#endif


/* ------------------------------------------------------------------ */
/* Variadic-argument list terminator + Go-style wrapper macros        */
/* ------------------------------------------------------------------ */

#define FMT_END_ARGS (NULL)

#define fmt_print(...)             __fmt_print     (__VA_ARGS__, NULL)
#define fmt_println(...)           __fmt_println   (__VA_ARGS__, NULL)
#define fmt_sprint(...)            __fmt_sprint    (__VA_ARGS__, NULL)
#define fmt_sprintln(...)          __fmt_sprintln  (__VA_ARGS__, NULL)
#define fmt_fprint(stream, ...)    __fmt_fprint    (stream, __VA_ARGS__, NULL)
#define fmt_fprintln(stream, ...)  __fmt_fprintln  (stream, __VA_ARGS__, NULL)


/* ------------------------------------------------------------------ */
/* stdout writers — Go-style variadic                                 */
/* ------------------------------------------------------------------ */

void   __fmt_print          (const char* str, ...);
void   __fmt_println        (const char* str, ...);


/* ------------------------------------------------------------------ */
/* stdout writers — C printf-style                                    */
/* ------------------------------------------------------------------ */

void   fmt_printf           (const char* format, ...);


/* ------------------------------------------------------------------ */
/* String builders — Go-style variadic                                */
/* ------------------------------------------------------------------ */

char*  __fmt_sprint         (const char* str, ...);
char*  __fmt_sprintln       (const char* str, ...);


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

int    __fmt_fprint         (FILE* stream, const char* str, ...);
int    __fmt_fprintln       (FILE* stream, const char* str, ...);


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
