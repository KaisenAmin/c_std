/**
 * @author Amin Tahmasebi
 * @date 2023
 * @class fmt
*/

#ifndef FMT_H_
#define FMT_H_

// #include <stddef.h>
// #include <stdarg.h>
#include <stdio.h>
#include "../file_io/file_writer.h"
#include "../file_io/file_reader.h"

#ifdef __cplusplus 
extern "C" {
#endif 

// #define FMT_LOGGING_ENABLE

#ifdef FMT_LOGGING_ENABLE 
    #define FMT_LOG(fmt, ...) \
        do { fprintf(stderr, "[FMT LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define FMT_LOG(fmt, ...) do { } while (0)
#endif


#define FMT_END_ARGS (NULL)

#define fmt_print(...) __fmt_print(__VA_ARGS__, NULL)
#define fmt_println(...) __fmt_println(__VA_ARGS__, NULL)
#define fmt_sprint(...) __fmt_sprint(__VA_ARGS__, NULL)
#define fmt_sprintln(...) __fmt_sprintln(__VA_ARGS__, NULL)
#define fmt_fprint(stream, ...) __fmt_fprint(stream, __VA_ARGS__, NULL)
#define fmt_fprintln(stream, ...) __fmt_fprintln(stream, __VA_ARGS__, NULL)

void __fmt_print(const char* str, ...);
void __fmt_println(const char* str, ...);
void fmt_printf(const char* format, ...);

char* __fmt_sprintln(const char* str, ...);
char* __fmt_sprint(const char* str, ...);
char* fmt_sprintf(const char* format, ...);

int fmt_scan(char** output);
int fmt_scanln(char** output);
int fmt_scanf(const char* format, ...);

int __fmt_fprint(FILE* stream, const char* str, ...);
int __fmt_fprintln(FILE* stream, const char* str, ...);
int fmt_fprintf(FILE* stream, const char* format, ...);

int fmt_fscan(FILE* stream, const char* format, ...);
int fmt_fscanln(FILE* stream, const char* format, ...);
int fmt_fscanf(FILE* stream, const char* format, ...);

#ifdef __cplusplus 
}
#endif 

#endif 