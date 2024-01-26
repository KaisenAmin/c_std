# Fmt Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2024
**License:** ISC License

## Overview

The Fmt library, developed in C, is inspired by the formatting capabilities commonly found in higher-level languages like Go and Python. It is designed to enhance the C programming experience by providing a set of functions for formatted string manipulation, input/output operations, and string conversions, making it highly suitable for both simple and complex text processing tasks in C.

## Key Features

- **Flexible Formatting:** Rich set of functions for formatted output to console, strings, and files.
- **Unicode Support:** Handles Unicode characters, enabling internationalization in C applications.
- **Dynamic Strings:** Simplifies working with dynamic strings in C, including automatic memory management.
- **Formatted Scanning:** Offers functions for formatted input parsing from various sources, including files and standard input.
- **Performance:** Optimized for speed and efficiency in both memory and execution time.

## Installation and Compilation

Include `fmt.h` in your project and compile the source files alongside your C project files. For GCC, use:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./fmt/fmt.c ./string/string.c
```

## Documentation

Comprehensive documentation is provided, detailing the functionality and usage of each function in the library. It covers everything from basic string formatting to more advanced features like Unicode processing and file I/O operations.

## Examples

The library includes various examples demonstrating its capabilities. These range from simple formatted console output to complex operations involving dynamic strings and file I/O.

### Function Descriptions

- `void fmt_print(const char* str, ...)`: Prints formatted text to the standard output with variable arguments.
- `void fmt_println(const char* str, ...)`: Prints formatted text followed by a newline to the standard output.
- `void fmt_printf(const char* format, ...)`: Outputs formatted text to the standard output, similar to `printf`.
- `char* fmt_sprintln(const char* first_arg, ...)`: Creates a formatted string with a newline at the end, returning a dynamically allocated string.

- `char* fmt_sprint(const char* first_arg, ...)`: Constructs a formatted string without a newline, returning a dynamically allocated string.

- `char* fmt_sprintf(const char* format, ...)`: Creates a formatted string based on a format string and variable arguments.
- `int fmt_scan(char** output)`: Scans input from standard input into a dynamically allocated string until space or newline.
- `int fmt_scanln(char** output)`: Reads a line of input from standard input into a dynamically allocated string.
- `int fmt_scanf(const char* format, ...)`: Scans formatted input from standard input, similar to `scanf`.
-`int fmt_fprint(FILE* stream, ...)`: Writes formatted text to the specified file stream.
- `int fmt_fprintln(FILE* stream, ...)`: Writes formatted text followed by a newline to the specified file stream.
- `int fmt_fprintf(FILE* stream, const char* format, ...)`: Outputs formatted text to the given file stream, similar to `fprintf`.

- `int fmt_fscan(FILE* stream, const char* format, ...)`: Reads formatted data from the specified file stream.
- `int fmt_fscanln(FILE* stream, const char* format, ...)`: Reads a line of formatted data from the specified file stream.
- `int fmt_fscanf(FILE* stream, const char* format, ...)`: Scans formatted input from a file stream, based on the specified format.

Each of these functions offers a versatile approach to handling formatted text in various contexts, greatly enhancing the ease of performing common text operations in C.

## Example 1 : how to show message with `fmt_println` and `fmt_print`

```c
#include "fmt/fmt.h"
#include "string/string.h"

void println() {
    for (int index = 0; index < 10; index++) {
        fmt_println(string_from_int_cstr(index), ":", "سلام دنیا", FMT_END_ARGS);
    }
}

// japanese
void print() {
    for (int index = 0; index < 10; index++) {
        fmt_print(string_from_int_cstr(index), ":", "ああ、 --", FMT_END_ARGS);
    }
}

int main() {
    println();
    fmt_println("-----------------", FMT_END_ARGS);
    print();

    return 0;
}
```

## Example 2 : write data in console with `fmt_printf`

```c
#include "fmt/fmt.h"
#include "string/string.h"
#include <stdlib.h>

int main() {
    char* message = fmt_sprintln("This is a message with Unicode characters:", "سلام دنیا", "ああ、こんにちは", FMT_END_ARGS);
    if (message) {
        fmt_print(message, FMT_END_ARGS);
        fmt_println(message, FMT_END_ARGS);
        free(message);
    }

    message = fmt_sprintln("Another message:", "امین طهماسبی", string_from_int_cstr(42), FMT_END_ARGS);
    if (message) {
        fmt_printf("%s", message);
        fmt_printf("and %d %s", 15, "امین"); 
        free(message);
    }

    fmt_printf("This is a message: %s\n", "Hello, world!");
    fmt_printf("This is a message with Unicode characters: %s, %s\n", "سلام دنیا", "ああ、こんにちは");
    fmt_printf("Another message: %s, %d\n", "Hello, world!", 42);

    return 0;
}
```

## Example 3 : how to use `fmt_sprintln`

Sprintln formats using the default formats for its operands and returns the resulting string. Spaces are always added between operands and a newline is

```c
#include "fmt/fmt.h"
#include "string/string.h"
#include <stdlib.h>

int main() {
    char* message = fmt_sprintln("This is a message with Unicode characters:", "سلام دنیا", "ああ、こんにちは", FMT_END_ARGS);
    if (message) {
        fmt_print(message, FMT_END_ARGS);
        fmt_println(message, FMT_END_ARGS);
        free(message);
    }

    message = fmt_sprintln("Another message:", "Hello, world!", string_from_int_cstr(42), FMT_END_ARGS);
    if (message) {
        fmt_printf("%s", message);
        fmt_printf("and %d %s", 15, "امین"); 
        free(message);
    }

    return 0;
}
```

## Example 4 : how to use `fmt_sprint`

Sprint formats using the default formats for its operands and returns the resulting string. Spaces are added between operands when neither is a string.

```c
#include "fmt/fmt.h"
#include "string/string.h"
#include <stdlib.h>

int main() {
    char* message = fmt_sprint("This is a message with Unicode characters:", "سلام دنیا", "ああ、こんにちは", "\n", FMT_END_ARGS);
    if (message) {
        fmt_print(message, FMT_END_ARGS);
        fmt_println(message, FMT_END_ARGS);
        free(message);
    }

    message = fmt_sprint("Another message:", "Hello, world!", string_from_int_cstr(42), FMT_END_ARGS);
    if (message) {
        fmt_printf("%s", message);
        fmt_printf("and %d", 15, FMT_END_ARGS); 
        free(message);
    }

    return 0;
}
```

## Example 5 : how to use `fmt_sprintf`

```c
#include "fmt/fmt.h"
#include "string/string.h"
#include <stdlib.h>

int main() {
    char* message = fmt_sprintf("Number: %d, String: %s, Unicode: %s\n", 42, "Hello World", "こんにちは");
    if (message) {
        fmt_printf("%s", message); 
        free(message); 
    }

    char* name = "امین طهماسبی";
    int age = 30;
    message = fmt_sprintf("%s is %d years old. Unicode Test: %s\n", name, age, "سلام دنیا");
    if (message) {
        fmt_printf("%s", message);
        free(message);
    }

    return 0;
}
```

### Example 6: Multi-Language Greeting and Data Display

This example will display greetings in multiple languages and show formatted data including dates, times, and numbers in different formats.

```c
#include "fmt/fmt.h"
#include "string/string.h"
#include "time/time.h"
#include "date/date.h"
#include <stdlib.h>

int main() {
    // Display greetings in different languages
    char* greeting = fmt_sprintf("Hello in English: %s, In Arabic: %s, In Japanese: %s\n",
                                 "Hello", "مرحبا", "こんにちは");
    if (greeting) {
        fmt_printf("%s", greeting);
        free(greeting);
    }

    char *format = "%Y-%m-%d";
    Time* current_time = time_current_time();
    Date* current_date = date_current_date(Persian);

    char* datetime = fmt_sprintf("Current Date and Time: %s %s", time_to_string(current_time), 
                                            date_to_string(current_date, format));
    if (datetime) {
        fmt_printf("%s", datetime);
        free(datetime);
    }

    time_deallocate(current_time);
    date_deallocate(current_date);
    return 0;
}
```

### Example 7: International Restaurant Menu Display

This example will simulate displaying a restaurant menu with items in different languages and prices.

```c
#include "fmt/fmt.h"
#include "string/string.h"
#include <stdlib.h>

int main() {
    fmt_printf("International Restaurant Menu:\n");
    fmt_printf("-----------------------------\n");

    char* item1 = fmt_sprintf("1. Dish: %s, Price: $%0.2f\n", "Pizza (ピザ)", 9.99);
    char* item2 = fmt_sprintf("2. Dish: %s, Price: $%0.2f\n", "فلافل", 5.49);
    char* item3 = fmt_sprintf("3. Dish: %s, Price: $%0.2f\n", "Noodles (麺)", 7.25);

    if (item1) {
        fmt_printf("%s", item1);
        free(item1);
    }
    if (item2) {
        fmt_printf("%s", item2);
        free(item2);
    }
    if (item3) {
        fmt_printf("%s", item3);
        free(item3);
    }
    return 0;
}
```

## Example 8 : how much `fmt_printf` is fast ? 

```c
#include "fmt/fmt.h"
#include <time.h>


int main() {
    struct timespec start, end;
    double time_sum = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 10000; i++) {
        // Average Custom fmt_printf Time: 0.000327 seconds
        fmt_printf("%d : %s\n", i, "امین طهماسبی"); 

        // Average Custom printf Time: 0.001184 seconds
        // printf("%d : %s\n", i, "امین طهماسبی");
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_sum += (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    double average_time = time_sum / 10000;
    printf("Average Custom fmt_printf Time: %f seconds\n", average_time);

    return 0;
}
```

## Example 9 : use `fmt_scan`

```c
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    char* input = NULL;
    fmt_printf("Enter a string: ");

    if (fmt_scan(&input) == 0) {
        fmt_printf("You entered: %s\n", input);
        free(input);
    } 
    else {
        fmt_printf("Error reading input.\n");
    }
    return 0;
}
```

## Example 10 : Using `fmt_scan`

```c
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    char* input = NULL;
    fmt_print("Enter a string (fmt_scan): ", FMT_END_ARGS);

    if (fmt_scan(&input) == 0) {
        fmt_print("You entered: ", FMT_END_ARGS);
        fmt_println(input, FMT_END_ARGS);
        free(input);
    } 
    else {
        fmt_println("Error reading input.", FMT_END_ARGS);
    }
    return 0;
}

```

## Example 11 : Using `fmt_scanln`

```c
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    char* input = NULL;
    fmt_print("Enter a string (fmt_scanln): ", FMT_END_ARGS);

    if (fmt_scanln(&input) == 0 && input != NULL) {
        char* message = fmt_sprintf("You entered: %s", input);
        if (message) {
            fmt_printf("%s\n", message);
            free(message);
        }
        free(input);
    } 
    else {
        fmt_println("Error or end of input.", FMT_END_ARGS);
    }
    return 0;
}

```

## Example 12 : how to use `fmt_scanf`

```c
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    int age;
    float salary;
    char name[50];

    fmt_print("Enter your name, age, and salary: ", FMT_END_ARGS);
    fmt_scanf("%s %d %f", name, &age, &salary);

    char* message = fmt_sprintf("Name: %s, Age: %d, Salary: %.2f", name, age, salary);
    if (message) {
        fmt_printf("Scanned data: %s\n", message);
        free(message);
    } 
    return 0;
}

```

## Example 13 : how to write stream with `fmt_fprint`

```c
#include "fmt/fmt.h"

int main() {
    FileWriter* writer = file_writer_open("./output.txt", WRITE_TEXT);

    fmt_fprint(writer->file_writer, "This is text in a file", FMT_END_ARGS);
    fmt_fprint(stdout, "Hello, World! ", "こんにちは ", "سلام دنیا", FMT_END_ARGS);
    fmt_fprint(stderr, "This is an error message on stderr", FMT_END_ARGS);

    file_writer_close(writer);
    return 0;
}
```

## Example 14 : how to write on stream with `fmt_fprintln`

```c
#include "fmt/fmt.h"

int main() {
    FileWriter* writer = file_writer_open("./sources/output.txt", WRITE_TEXT);

    fmt_fprintln(writer->file_writer, "This is a line in a file", FMT_END_ARGS);
    fmt_fprintln(stdout, "This is a line on stdout", FMT_END_ARGS);
    fmt_fprintln(stderr, "This is a line on stderr", FMT_END_ARGS);

    file_writer_close(writer);
    return 0;
}

```

## Example 15 : how to write formatted text to file with `fmt_fprintf`

```c
#include "fmt/fmt.h"

int main() {
    FileWriter* writer = file_writer_open("./output.txt", WRITE_TEXT);
    const char* name = "Amin";
    int age = 27;

    int bytes_written = fmt_fprintf(writer->file_writer, "%s is %d years old.\n", name, age);
    if (bytes_written < 0) {
        fprintf(stderr, "Error occurred during writing with fmt_fprintf.\n");
    }
    else {
        printf("%d bytes written.\n", bytes_written);
    }

    file_writer_close(writer);
    return 0;
}
```

## Example 16 : writing error messatge in stderr using `fmt_fprintf`

```c
#include "fmt/fmt.h"

int main() {
    int errorCode = 404;
    const char* errorMessage = "Not Found";

    // Write the error message to stderr
    int bytes_written = fmt_fprintf(stderr, "Error %d: %s\n", errorCode, errorMessage);
    if (bytes_written < 0) {
        fprintf(stderr, "Error occurred during writing to stderr.\n");
    } 
    else {
        printf("%d bytes written to stderr.\n", bytes_written);
    }

    return 0;
}
```

## Example 17 : Writing message in console using `stdout` and `fmt_fprintf`

```c
#include "fmt/fmt.h"

int main() {
    const char* name = "Amin";
    int age = 27;

    int bytes_written = fmt_fprintf(stdout, "%s is %d years old.\n", name, age); 
    if (bytes_written < 0) {
        fprintf(stderr, "Error occurred during writing with fmt_fprintf.\n");
    }
    else {
        printf("%d bytes written.\n", bytes_written);
    }

    return 0;
}
```

## Example 18 : Show Emoji 

```c
#include "fmt/fmt.h"

int main() {
    fmt_printf("World : %s %s\n", "🌍", "😡");
    fmt_println("Haaaa :", "😊🥴", FMT_END_ARGS);
    fmt_print("😡", "🥶", "😎", "🤩", FMT_END_ARGS);

    return 0;
}
```

## Example 19 : Scan data from file in formated text with `fmt_fscanf`

```c
#include "fmt/fmt.h"

int main() {
    // 25 19.365 hello put these forexample in file 
    FileReader* stream = file_reader_open("./sources/input.txt", READ_TEXT);
    int i;
    double d;
    char s[100];

    int n = fmt_fscanf(stream->file_reader, "%d %lf %99s", &i, &d, s);
    if (n != 3) {
        fmt_fprintf(stderr, "Error or insufficient data read from file\n");
    } 
    else {
        fmt_printf("Read data: %d, %lf, %s\n", i, d, s);
    }

    file_reader_close(stream);
    return 0;
}
```

## Example 20 : Using `fmt_scanln` with standart input

```c
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    char* input = NULL;
    fmt_print("Enter a line of text: ", FMT_END_ARGS);

    if (fmt_scanln(&input) == 0 && input != NULL) {
        fmt_printf("You entered: %s\n", input);
        free(input);
    } 
    else {
        fmt_println("Error or end of input.", FMT_END_ARGS);
    }
    
    return 0;
}
```

## Example 21: Using Emojis with `fmt_fprintf` and `fmt_fprint`

```c
#include "fmt/fmt.h"

int main() {
    fmt_fprintf(stdout, "Emoji example: %s %s %s\n", "😀", "🚀", "🌟");
    fmt_fprint(stdout, "More emojis: ", "🌍", "🌈", "🔥", FMT_END_ARGS);
    
    return 0;
}
```

## Example 22 : Writing to a file Using `fmt_fprintf` and `fmt_fprint`

```c
#include "fmt/fmt.h"

int main() {
    FileWriter* writer = file_writer_open("./output.txt", WRITE_TEXT);

    fmt_fprintf(writer->file_writer, "Writing to a file: %s\n", "Hello, برنامه نویسان");
    fmt_fprint(writer->file_writer, "More text: ", "This is an example", " using fmt_fprint", FMT_END_ARGS);

    file_writer_close(writer);
    return 0;
}
```