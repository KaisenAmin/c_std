

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

    char* datetime = fmt_sprintf("Current Date and Time: %s %s", time_to_string(current_time), date_to_string(current_date, format));
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

## Example 8 : how much fmt_printf is fast ? 

```c
#include "fmt/fmt.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

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
#include <stdio.h>

int main() {
    char* input = NULL;
    printf("Enter a string: ");

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