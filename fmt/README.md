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

### Function Descriptions

### `void fmt_printf(const char* format, ...)`
**Purpose**: Prints a formatted string to stdout using printf-style format specifiers.
**Parameters**:
- `format`: The printf-style format string controlling output layout.
- `...`: Additional arguments to be formatted according to the format string.
**Return Value**: None.
**Usage Case**: Use as the primary formatted-output function throughout application code; on Windows it converts UTF-8 to UTF-16 for proper Unicode output via `vfwprintf`.

---

### `void fmt_print(const char* str, ...)`
**Purpose**: Prints one or more string arguments to stdout concatenated without spaces or a trailing newline.
**Parameters**:
- `str`: The first string argument to print.
- `...`: Additional string arguments; terminate the list with `NULL` (handled automatically by the `fmt_print(...)` macro).
**Return Value**: None.
**Usage Case**: Use when you need to print multiple strings side by side without separators; on Windows uses `WriteConsoleW` for Unicode output.

---

### `void fmt_println(const char* str, ...)`
**Purpose**: Prints one or more string arguments to stdout with a space between each argument and a newline at the end.
**Parameters**:
- `str`: The first string argument to print.
- `...`: Additional string arguments to print space-separated.
**Return Value**: None.
**Usage Case**: Use for Go-style "println" output where space-separated arguments and an automatic newline are desired; handles Unicode on Windows.

---

### `char* fmt_sprintf(const char* format, ...)`
**Purpose**: Formats a string using printf-style specifiers and returns it as a heap-allocated string.
**Parameters**:
- `format`: The printf-style format string controlling output layout.
- `...`: Additional arguments to be formatted according to the format string.
**Return Value**: A pointer to the newly allocated formatted string, or `NULL` on allocation failure; the caller must `free` the result.
**Usage Case**: Use when you need to build a formatted string in memory before printing or storing it.

---

### `char* fmt_sprint(const char* first_arg, ...)`
**Purpose**: Concatenates one or more string arguments into a heap-allocated result string without a trailing newline.
**Parameters**:
- `first_arg`: The first string argument to include.
- `...`: Additional string arguments to concatenate (with a space between non-adjacent strings).
**Return Value**: A pointer to the newly allocated string, or `NULL` on allocation failure; the caller must `free` the result.
**Usage Case**: Use to assemble multi-part strings in memory without immediately printing them.

---

### `char* fmt_sprintln(const char* first_arg, ...)`
**Purpose**: Like `fmt_sprint` but adds a space between each argument and appends a newline at the end.
**Parameters**:
- `first_arg`: The first string argument to include.
- `...`: Additional string arguments to append space-separated.
**Return Value**: A pointer to the newly allocated string, or `NULL` on allocation failure; the caller must `free` the result.
**Usage Case**: Use when you need a Go-style sprintln result as a heap string for later use.

---

### `int fmt_scan(char** output)`
**Purpose**: Reads one whitespace-delimited token from stdin into a heap-allocated string.
**Parameters**:
- `output`: A pointer to a `char*` that will be set to the allocated buffer containing the token; the caller must `free` it.
**Return Value**: `0` on success, or a negative value on error.
**Usage Case**: Use for simple single-word input reading from standard input.

---

### `int fmt_scanln(char** output)`
**Purpose**: Reads up to the next newline or whitespace from stdin into a heap-allocated string.
**Parameters**:
- `output`: A pointer to a `char*` that will be set to the allocated buffer containing the input; the caller must `free` it.
**Return Value**: `0` on success, or a negative value on error.
**Usage Case**: Use when reading a line or word from interactive input.

---

### `int fmt_scanf(const char* format, ...)`
**Purpose**: Scans stdin using a printf-style format string, storing parsed values in the variadic arguments.
**Parameters**:
- `format`: The format string specifying how to parse the input.
- `...`: Pointers to variables where the parsed values will be stored.
**Return Value**: The number of items successfully matched, or a negative value on error.
**Usage Case**: Use as a formatted replacement for standard `scanf`.

---

### `int fmt_fprintf(FILE* stream, const char* format, ...)`
**Purpose**: Writes formatted output to `stream` using printf-style format specifiers.
**Parameters**:
- `stream`: The `FILE*` to write formatted output to (e.g. `stdout`, `stderr`, or any open file).
- `format`: The printf-style format string controlling output layout.
- `...`: Additional arguments to be formatted according to the format string.
**Return Value**: The number of bytes written, or a negative value on error.
**Usage Case**: Use to write formatted text to `stdout`, `stderr`, or any open `FILE*`.

---

### `int fmt_fprint(FILE* stream, const char* str, ...)`
**Purpose**: Writes one or more string arguments to `stream` concatenated without spaces or a trailing newline.
**Parameters**:
- `stream`: The `FILE*` to write to.
- `str`: The first string argument to write.
- `...`: Additional string arguments to write sequentially.
**Return Value**: The number of bytes written, or `-1` on error.
**Usage Case**: Use to write multi-part strings to a file or stream without separators.

---

### `int fmt_fprintln(FILE* stream, const char* str, ...)`
**Purpose**: Writes one or more string arguments to `stream` with a space between each and a newline at the end.
**Parameters**:
- `stream`: The `FILE*` to write to.
- `str`: The first string argument to write.
- `...`: Additional string arguments to write space-separated.
**Return Value**: The number of bytes written, or `-1` on error.
**Usage Case**: Use for Go-style newline-terminated stream output.

---

### `int fmt_fscan(FILE* stream, const char* format, ...)`
**Purpose**: Reads formatted input from `stream` using a printf-style format string.
**Parameters**:
- `stream`: The `FILE*` to read from.
- `format`: The format string specifying how to parse the input.
- `...`: Pointers to variables where the parsed values will be stored.
**Return Value**: The number of items matched, or a negative value on error.
**Usage Case**: Use to parse structured data from a file or any `FILE*`.

---

### `int fmt_fscanln(FILE* stream, const char* format, ...)`
**Purpose**: Reads a single line from `stream` and parses it according to the format string.
**Parameters**:
- `stream`: The `FILE*` to read one line from.
- `format`: The format string specifying how to parse the line.
- `...`: Pointers to variables where the parsed values will be stored.
**Return Value**: The number of items matched, `0` on EOF, or a negative value on error.
**Usage Case**: Use when you need to parse exactly one line at a time from a file.

---

### `int fmt_fscanf(FILE* stream, const char* format, ...)`
**Purpose**: Reads formatted input from `stream`, identical in behavior to standard `fscanf`.
**Parameters**:
- `stream`: The `FILE*` to read from.
- `format`: The format string specifying how to parse the input.
- `...`: Pointers to variables where the parsed values will be stored.
**Return Value**: The number of items matched, or a negative value on error.
**Usage Case**: Use as a formatted replacement for standard `fscanf` with any file stream.

---

## Example 1 : how to show message with `fmt_println` and `fmt_print`

```c
#include "fmt/fmt.h"
#include "string/std_string.h"
#include <stdlib.h>

void println() {
    for (int index = 0; index < 10; index++) {
        char* num = string_from_int_cstr(index);
        fmt_println(num, ":", "سلام دنیا");
        free(num);
    }
}

// japanese
void print() {
    for (int index = 0; index < 10; index++) {
        char* num = string_from_int_cstr(index);
        fmt_print(num, ":", "ああ、 --");
        free(num);
    }
}

int main() {
    println();
    fmt_println("-----------------");
    print();

    return 0;
}
```
**Result:**
```
0 : سلام دنیا 
1 : سلام دنیا 
2 : سلام دنیا 
3 : سلام دنیا 
4 : سلام دنیا 
5 : سلام دنیا 
6 : سلام دنیا 
7 : سلام دنیا 
8 : سلام دنیا 
9 : سلام دنیا 
----------------- 
0 : ああ、 -- 1 : ああ、 -- 2 : ああ、 -- 3 : ああ、 -- 4 : ああ、 -- 5 : ああ、 -- 6 : ああ、 -- 7 : ああ、 -- 8 : ああ、 -- 9 : ああ、 --
```

---

## Example 2 : write data in console with `fmt_printf`

```c
#include "fmt/fmt.h"
#include "string/std_string.h"
#include <stdlib.h>

int main() {
    char* message = fmt_sprintln("This is a message with Unicode characters:", "سلام دنیا", "ああ、こんにちは");
    if (message) {
        fmt_print(message);
        fmt_println(message);

        free(message);
    }

    char* num = string_from_int_cstr(42);
    message = fmt_sprintln("Another message:", "امین طهماسبی", num);
    free(num);
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
**Result:**
```
This is a message with Unicode characters: سلام دنیا ああ、こんにちは
This is a message with Unicode characters: سلام دنیا ああ、こんにちは
 
Another message: امین طهماسبی 42
and 15 امینThis is a message: Hello, world!
This is a message with Unicode characters: سلام دنیا, ああ、こんにちは
Another message: Hello, world!, 42
```

---

## Example 3 : how to use `fmt_sprintln`

Sprintln formats using the default formats for its operands and returns the resulting string. Spaces are always added between operands and a newline is

```c
#include "fmt/fmt.h"
#include "string/std_string.h"
#include <stdlib.h>

int main() {
    char* message = fmt_sprintln("This is a message with Unicode characters:", "سلام دنیا", "ああ、こんにちは");
    if (message) {
        fmt_print(message);
        fmt_println(message);

        free(message);
    }

    char* num = string_from_int_cstr(42);
    message = fmt_sprintln("Another message:", "Hello, world!", num);
    free(num);
    if (message) {
        fmt_printf("%s", message);
        fmt_printf("and %d %s", 15, "امین"); 

        free(message);
    }

    return 0;
}
```
**Result:**
```
This is a message with Unicode characters: سلام دنیا ああ、こんにちは
 This is a message with Unicode characters: سلام دنیا ああ、こんにちは
 
Another message: Hello, world! 42
```

---

## Example 4 : how to use `fmt_sprint`

Sprint formats using the default formats for its operands and returns the resulting string. Spaces are added between operands when neither is a string.

```c
#include "fmt/fmt.h"
#include "string/std_string.h"
#include <stdlib.h>

int main() {
    char* message = fmt_sprint("This is a message with Unicode characters:", "سلام دنیا", "ああ、こんにちは", "\n");
    if (message) {
        fmt_print(message);
        fmt_println(message);
        
        free(message);
    }

    char* num = string_from_int_cstr(42);
    message = fmt_sprint("Another message:", "Hello, world!", num);
    free(num);
    if (message) {
        fmt_printf("%s", message);
        fmt_printf("and %d", 15);

        free(message);
    }

    return 0;
}
```
**Result:**
```
This is a message with Unicode characters: سلام دنیا ああ、こんにちは 
This is a message with Unicode characters: سلام دنیا ああ、こんにちは 

Another message: Hello, world! 42and 15
```

---

## Example 5 : how to use `fmt_sprintf`

```c
#include "fmt/fmt.h"
#include "string/std_string.h"
#include <stdlib.h>

int main() {
    char* message = fmt_sprintf("Number: %d, String: %s, Unicode: %s\n", 42, "Hello World", "こんにちは");
    if (message) {
        fmt_printf("%s", message); 
        free(message); 
    }

    const char* name = "امین طهماسبی";
    int age = 30;
    message = fmt_sprintf("%s is %d years old. Unicode Test: %s\n", name, age, "سلام دنیا");
    if (message) {
        fmt_printf("%s", message);
        free(message);
    }

    return 0;
}
```
**Result:**
```
Number: 42, String: Hello World, Unicode: こんにちは
امین طهماسبی is 30 years old. Unicode Test: سلام دنیا
```

---

### Example 6: Multi-Language Greeting and Data Display

This example will display greetings in multiple languages and show formatted data including dates, times, and numbers in different formats.

```c
#include "fmt/fmt.h"
#include "string/std_string.h"
#include "time/std_time.h"
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

    const char *format = "%Y-%m-%d";
    Time* current_time = time_current_time();
    Date* current_date = date_current_date(Persian);

    char* time_str = time_to_string(current_time);
    char* date_str = date_to_string(current_date, format);
    char* datetime = fmt_sprintf("Current Date and Time: %s %s\n", time_str, date_str);
    free(time_str);
    free(date_str);
    if (datetime) {
        fmt_printf("%s", datetime);
        free(datetime);
    }

    time_deallocate(current_time);
    date_deallocate(current_date);
    return 0;
}
```
**Result:**
```
Hello in English: Hello, In Arabic: مرحبا, In Japanese: こんにちは
Current Date and Time: (23:45:40:762) 1403-03-31
```

---

### Example 7: International Restaurant Menu Display

This example will simulate displaying a restaurant menu with items in different languages and prices.

```c
#include "fmt/fmt.h"
#include "string/std_string.h"
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
**Result:**
```
International Restaurant Menu:
-----------------------------
1. Dish: Pizza (ピザ), Price: $9.99
2. Dish: فلافل, Price: $5.49
3. Dish: Noodles (麺), Price: $7.25
```

---

## Example 8 : how much `fmt_printf` is fast ? 

```c
#include "fmt/fmt.h"
#include "time/std_time.h"
#include <time.h>


int main() {
    Time* start_time = time_current_time();

    for (int i = 0; i < 10000; i++) {
        // fmt lib
        fmt_printf("%d : %s\n", i, "امین طهماسبی"); 

        // standard lib
        printf("%d : %s\n", i, "امین طهماسبی");
    }

    Time* end_time = time_current_time();
    double diff_time = time_diff_in_seconds(start_time, end_time);

    fmt_printf("Difference in Seconds : %lf", diff_time);

    time_deallocate(start_time);
    time_deallocate(end_time);

    return 0;
}
```
**Result:**
```
test it and see speed of fmt_printf
```

---

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
**Result:**
```
Enter a string: c_std
You entered: c_std
```

---

## Example 10 : Using `fmt_scan`

```c
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    char* input = NULL;
    fmt_print("Enter a string (fmt_scan): ");

    if (fmt_scan(&input) == 0) {
        fmt_print("You entered: ");
        fmt_println(input);

        free(input);
    } 
    else {
        fmt_println("Error reading input.");
    }
    
    return 0;
}
```
**Result:**
```
Enter a string (fmt_scan):  Hello World
You entered:  Hello 
```

---

## Example 11 : Using `fmt_scanln`

```c
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    char* input = NULL;
    fmt_print("Enter a string (fmt_scanln): ");

    if (fmt_scanln(&input) == 0 && input != NULL) {
        char* message = fmt_sprintf("You entered: %s", input);
        if (message) {
            fmt_printf("%s\n", message);
            free(message);
        }
        
        free(input);
    } 
    else {
        fmt_println("Error or end of input.");
    }
    return 0;
}
```
**Result:**
```
Enter a string (fmt_scanln): largest c framwork
You entered: largest
```

---

## Example 12 : how to use `fmt_scanf`

```c
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    int age;
    float salary;
    char name[50];

    fmt_print("Enter your name, age, and salary: ");
    fmt_scanf("%[^\n]s %d %f", name, &age, &salary);

    char* message = fmt_sprintf("Name: %s, Age: %d, Salary: %.2f", name, age, salary);
    if (message) {
        fmt_printf("Scanned data: %s\n", message);
        free(message);
    } 

    return 0;
}
```
**Result:**
```
Enter your name, age, and salary: "amin tahmasebi" 50 20.32
Scanned data: Name: "amin tahmasebi" 50 20.32, Age: 551, Salary: 227470.88
```

---

## Example 13 : how to write stream with `fmt_fprint`

```c
#include "fmt/fmt.h"

int main() {
    FileWriter* writer = file_writer_open("./output.txt", WRITE_TEXT);

    fmt_fprint(writer->file_writer, "This is text in a file");
    fmt_fprint(stdout, "Hello, World! ", "こんにちは ", "سلام دنیا");
    fmt_fprint(stderr, "This is an error message on stderr");

    file_writer_close(writer);
    return 0;
}
```
**Result:**
```
Hello, World! こんにちは سلام دنیاThis is an error message on stderr
```

---

## Example 14 : how to write on stream with `fmt_fprintln`

```c
#include "fmt/fmt.h"

int main() {
    FileWriter* writer = file_writer_open("./sources/output.txt", WRITE_TEXT);

    fmt_fprintln(writer->file_writer, "This is a line in a file");
    fmt_fprintln(stdout, "This is a line on stdout");
    fmt_fprintln(stderr, "This is a line on stderr");

    file_writer_close(writer);
    return 0;
}
```
**Result:**
```
This is a line on stdout 
This is a line on stderr
```

---

## Example 15 : how to write formatted text to file with `fmt_fprintf`

```c
#include "fmt/fmt.h"

int main() {
    FileWriter* writer = file_writer_open("./output.txt", WRITE_TEXT);
    const char* name = "Amin";
    int age = 27;

    int bytes_written = fmt_fprintf(writer->file_writer, "%s is %d years old.\n", name, age);
    if (bytes_written < 0) {
        fmt_fprintf(stderr, "Error occurred during writing with fmt_fprintf.\n");
    }
    else {
        fmt_printf("%d bytes written.\n", bytes_written);
    }

    file_writer_close(writer);
    return 0;
}
```
**Result:**
```
22 bytes written.
```

---

## Example 16 : writing error messatge in stderr using `fmt_fprintf`

```c
#include "fmt/fmt.h"

int main() {
    int errorCode = 404;
    const char* errorMessage = "Not Found";

    // Write the error message to stderr
    int bytes_written = fmt_fprintf(stderr, "Error %d: %s\n", errorCode, errorMessage);
    if (bytes_written < 0) {
        fmt_fprintf(stderr, "Error occurred during writing to stderr.\n");
    } 
    else {
        fmt_printf("%d bytes written to stderr.\n", bytes_written);
    }

    return 0;
}
```
**Result:**
```
Error 404: Not Found
21 bytes written to stderr.
```

---

## Example 17 : Writing message in console using `stdout` and `fmt_fprintf`

```c
#include "fmt/fmt.h"

int main() {
    const char* name = "Amin";
    int age = 27;

    int bytes_written = fmt_fprintf(stdout, "%s is %d years old.\n", name, age); 
    if (bytes_written < 0) {
        fmt_fprintf(stderr, "Error occurred during writing with fmt_fprintf.\n");
    }
    else {
        fmt_printf("%d bytes written.\n", bytes_written);
    }

    return 0;
}
```
**Result:**
```
Amin is 27 years old.
22 bytes written.
```

---

## Example 18 : Show Emoji 

```c
#include "fmt/fmt.h"

int main() {
    fmt_printf("World : %s %s\n", "🌍", "😡");
    fmt_println("Haaaa :", "😊🥴");
    fmt_print("Hello", "World", "🤩");

    return 0;
}
```
**Result:**
```
World : 🌍 😡
Haaaa : 😊🥴
HelloWorld🤩
```

---

## Example 19 : Scan data from file in formated text with `fmt_fscanf`

```c
#include "fmt/fmt.h"

int main() {
    // 25 19.365 hello put these forexample in file 
    FileReader* stream = file_reader_open("./sources/input.txt", READ_TEXT);
    int i;
    double d;
    char s[100];

    int n = fmt_fscanf(stream->file_reader, "%d %lf %[^\n]s", &i, &d, s);
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
**Result** (when `./sources/input.txt` contains `25 19.365 Hello World`)**:**
```
Read data: 25, 19.365000, Hello World
```

---

## Example 20 : Using `fmt_scanln` with standart input

```c
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    char* input = NULL;
    fmt_print("Enter a line of text: ");

    if (fmt_scanln(&input) == 0 && input != NULL) {
        fmt_printf("You entered: %s\n", input);
        free(input);
    } 
    else {
        fmt_println("Error or end of input.");
    }
    
    return 0;
}
```
**Result:**
```
Enter a line of text: Hello World
You entered: Hello
```

---

## Example 21: Using Emojis with `fmt_fprintf` and `fmt_fprint`

```c
#include "fmt/fmt.h"

int main() {
    fmt_fprintf(stdout, "Emoji example: %s %s %s\n", "😀", "🚀", "🌟");
    fmt_fprint(stdout, "More emojis: ", "🌍", "🌈", "🔥");
    
    return 0;
}
```
**Result:**
```
Emoji example: 😀 🚀 🌟
More emojis: 🌍🌈🔥
```

---

## Example 22 : Writing to a file Using `fmt_fprintf` and `fmt_fprint`

```c
#include "fmt/fmt.h"

int main() {
    FileWriter* writer = file_writer_open("./output.txt", WRITE_TEXT);

    fmt_fprintf(writer->file_writer, "Writing to a file: %s\n", "Hello, برنامه نویسان");
    fmt_fprint(writer->file_writer, "More text: ", "This is an example", " using fmt_fprint");

    file_writer_close(writer);
    return 0;
}
```
**Result:**
```
(no console output — both strings are written to ./output.txt)
```

---

## License

This project is open-source and available under [ISC License].