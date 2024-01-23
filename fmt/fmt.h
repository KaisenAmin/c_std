#ifndef FMT_H_
#define FMT_H_

// #include <stddef.h>
// #include <stdarg.h>
// #include <stdio.h>
#include "../file_io/file_writer.h"
#include "../file_io/file_reader.h"

#define FMT_END_ARGS (NULL)

void fmt_print(const char* format, ...);
void fmt_println(const char* format, ...);
void fmt_printf(const char* format, ...);

char* fmt_sprintln(const char* first_arg, ...);
char* fmt_sprint(const char* first_arg, ...);
char* fmt_sprintf(const char* format, ...);

int fmt_scan(char** output);
int fmt_scanln(char** output);
int fmt_scanf(const char* format, ...);

int fmt_fprint(FILE* stream, ...);
int fmt_fprintln(FILE* stream, ...);
int fmt_fprintf(FILE* stream, const char* format, ...);

int fmt_fscan(FILE* stream, const char* format, ...);
int fmt_fscanln(FILE* stream, const char* format, ...);
int fmt_fscanf(FILE* stream, const char* format, ...);

#endif 