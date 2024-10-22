# Regex Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

This Regex library in C provides functionality to compile, match, and search regular expressions. It is designed to offer flexibility with flags for case-insensitive matching, multiline support, and dotall mode, making it suitable for a wide range of pattern-matching applications. The library is cross-platform, supporting both POSIX and PCRE (for Windows).

## Key Features

- **Regex Compilation:** Compile regex patterns with optional flags for case-insensitive, multiline, and dotall modes.
- **Pattern Matching:** Match compiled regex patterns against strings, returning details of matches (start, end, length).
- **Pattern Search:** Search for patterns in strings, finding all occurrences.
- **Cross-Platform Support:** Uses PCRE on Windows and POSIX regex on other platforms.
- **Error Handling:** Provides detailed error messages for compilation and matching errors.

## Installation and Compilation

To use this library, include `std_regex.h` in your project and compile the source files with your C compiler. On Linux, the following command can be used:

For **Windows** with **MSYS2/MinGW64**, you will need to install **PCRE** (Perl-Compatible Regular Expressions) for regex operations. MSYS2 provides the `mingw-w64-x86_64-pcre` package, which includes PCRE support for MinGW environments.

### Windows Requirements (MSYS2 MinGW64):
To use your `std_regex` library with **MSYS2/MinGW64**, you need to install the PCRE package. You can do this using the following command:

### Steps for Compiling on MSYS2/MinGW64:
1. Install PCRE:
   ```
   pacman -S mingw-w64-x86_64-pcre
   ```

### Linux Requirements:
1. Install `sudo apt-get install libpcre3-dev`

## Documentation

The library includes detailed function descriptions for compiling and matching regex patterns. Below are the key functions provided by the library.

---

### Data Structures and Enums

#### `typedef struct Regex Regex`

- **Purpose**: This is an opaque structure used to represent a compiled regular expression. It is defined as `struct Regex`, but the internal fields are hidden from the user. The purpose of this design is to encapsulate the implementation details of the regular expression, providing a clean API for users of the library.
- **Usage**: This structure is created and used when a pattern is compiled via `regex_compile()` and passed to various matching and searching functions.

---

#### `typedef enum RegexResult`

- **Purpose**: This enumeration defines the possible result codes for regex compilation and matching operations. It is returned by functions such as `regex_compile()` and `regex_match()` to indicate success or failure.
- **Enum Values**:
  - `REGEX_SUCCESS`: Indicates that the operation was successful, whether it is regex compilation or pattern matching.
  - `REGEX_COMPILE_ERROR`: Indicates an error occurred during the compilation of the regular expression pattern.
  - `REGEX_MATCH_ERROR`: Indicates that an error occurred during the pattern matching process.
  - `REGEX_NO_MATCH`: Indicates that no match was found for the given pattern in the input string.

---

#### `typedef enum RegexFlags`

- **Purpose**: This enumeration defines optional flags that can be passed to the `regex_compile()` function to modify the behavior of the regular expression matching process.
- **Enum Values**:
  - `REGEX_DEFAULT`: The default behavior with no special options enabled. Matches are case-sensitive, single-line, and `.` does not match newline characters.
  - `REGEX_CASE_INSENSITIVE`: Enables case-insensitive matching, allowing patterns to match regardless of letter case (e.g., "Hello" will match "hello").
  - `REGEX_MULTILINE`: Multiline matching mode. In this mode, the `^` and `$` symbols match the start and end of each line, rather than the entire string.
  - `REGEX_DOTALL`: Enables dotall mode, where the `.` symbol matches any character, including newline characters. Without this flag, `.` will not match newlines.

---

#### `typedef struct RegexMatch`

- **Purpose**: This structure holds the result of a regex match operation. When a string is matched against a pattern, the `RegexMatch` structure stores the start, end, and length of the matched substring.
- **Fields**:
  - `start`: A pointer to the start of the match in the input string. It points to the position where the first matching character is found.
  - `end`: A pointer to the end of the match in the input string. It points to the character immediately after the last matching character.
  - `length`: The length of the matched substring, calculated as the difference between `end` and `start`.
  - `group_count`: Number of capturing groups.
  - `group_starts`: Array to store starting points of groups.
  - `group_lengths`: Array to store lengths of groups.
---

### Function Descriptions

### `Regex* regex_compile(const char* pattern, RegexFlags flags)`

- **Purpose**: Compiles the given regular expression pattern into a `Regex` object.
- **Parameters**:
  - `pattern`: The regex pattern to compile.
  - `flags`: Optional flags for regex compilation, including case-insensitive and multiline options.
- **Return**: A pointer to a compiled `Regex` object or `NULL` if compilation fails.

---

### `RegexResult regex_match(Regex* regex, const char* string, RegexMatch* match)`

- **Purpose**: Matches a string against the compiled regex.
- **Parameters**:
  - `regex`: The compiled regex object.
  - `string`: The string to match.
  - `match`: A structure to store match details (start, end, and length of the match).
- **Return**: `REGEX_SUCCESS` if a match is found, `REGEX_NO_MATCH` if not, or `REGEX_MATCH_ERROR` on error.

---

### `RegexResult regex_search(Regex* regex, const char* string, RegexMatch* match)`

- **Purpose**: Searches for a regex pattern within a string.
- **Parameters**:
  - `regex`: The compiled regex object.
  - `string`: The string to search.
  - `match`: A structure to store match details for the first occurrence.
- **Return**: Same as `regex_match`.

---

### `int regex_find_all(Regex* regex, const char* string, RegexMatch* matches, int max_matches)`

- **Purpose**: Finds all occurrences of a regex pattern in a string.
- **Parameters**:
  - `regex`: The compiled regex object.
  - `string`: The string to search.
  - `matches`: Pointer to an array of `RegexMatch` objects for storing match results.
  - `max_matches`: Maximum number of matches to find.
- **Return**: The number of matches found or `-1` on error.

---

### `void regex_deallocate(Regex* regex)`

- **Purpose**: Frees resources allocated for the compiled regex object.
- **Parameters**:
  - `regex`: The regex object to free.

---

### `const char* regex_error_message(RegexResult result)`

- **Purpose**: Converts a regex result code to a human-readable error message.
- **Parameters**:
  - `result`: The result code, such as `REGEX_COMPILE_ERROR`.
- **Return**: A string representing the error message.

---

## Example Usage

### Example 1: Basic Regex Matching

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "hello";
    const char* test_string = "hello world!";

    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Match found: %.*s\n", match.length, match.start);
    } 
    else {
        fmt_printf("No match found.\n");
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result**
```
Match found: hello
```

---

### Example 2: Finding All Occurrences

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "hello";
    const char* test_string = "hello world! hello again!";

    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch matches[10];
    int match_count = regex_find_all(regex, test_string, matches, 10);

    fmt_printf("Found %d matches:\n", match_count);
    for (int i = 0; i < match_count; ++i) {
        fmt_printf("Match %d: %.*s\n", i + 1, matches[i].length, matches[i].start);
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result**
```
Found 2 matches:
Match 1: hello
Match 2: hello
```

---

### Example 3: Case-Insensitive Matching

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "HELLO";
    const char* test_string = "hello world!";

    Regex* regex = regex_compile(pattern, REGEX_CASE_INSENSITIVE);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Case-insensitive match found: %.*s\n", match.length, match.start);
    } 
    else {
        fmt_printf("No match found.\n");
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result:**
```
Case-insensitive match found: hello
```

---

### Example 4: Multiline Matching

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "^hello";
    const char* test_string = "hello world!\ngoodbye world!";

    Regex* regex = regex_compile(pattern, REGEX_MULTILINE);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Multiline match found: %.*s\n", match.length, match.start);
    } 
    else {
        fmt_printf("No match found.\n");
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result:**
```
Multiline match found: hello
```

---

### Example 5: Dotall Mode (Dot Matches Newline)

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "hello.*goodbye";
    const char* test_string = "hello world!\ngoodbye world!";

    Regex* regex = regex_compile(pattern, REGEX_DOTALL);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Dotall match found: %.*s\n", match.length, match.start);
    } 
    else {
        fmt_printf("No match found.\n");
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result:**
```
Dotall match found: hello world!
goodbye
```

---

### Example 6: Finding All Matches with Case Sensitivity

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "hello";
    const char* test_string = "Hello world! hello again!";

    // Compile the pattern with case-sensitive
    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch matches[10];
    int match_count = regex_find_all(regex, test_string, matches, 10);

    fmt_printf("Found %d matches:\n", match_count);
    for (int i = 0; i < match_count; ++i) {
        fmt_printf("Match %d: %.*s\n", i + 1, matches[i].length, matches[i].start);
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result:**
```
Found 1 matches:
Match 1: hello
```

---

### Example 7: Using Multiple Flags (Case-Insensitive and Dotall Mode)

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "HELLO.*GOODBYE";
    const char* test_string = "hello world!\ngoodbye world!";

    Regex* regex = regex_compile(pattern, REGEX_CASE_INSENSITIVE | REGEX_DOTALL);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Match found: %.*s\n", match.length, match.start);
    } 
    else {
        fmt_printf("No match found.\n");
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result:**
```
Match found: hello world!
goodbye
```

---

### Example 8: Matching a Word Boundary (Regex Anchors)

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "\\bhello\\b";  // Matches "hello" as a whole word
    const char* test_string = "well, hello there!";

    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Word boundary match found: %.*s\n", match.length, match.start);
    } 
    else {
        fmt_printf("No match found.\n");
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result:**
```
Word boundary match found: hello
```

---

### Example 9: Matching Groups and Extracting Substrings

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "([[:alnum:]_]+)@([[:alnum:]_]+\\.[[:alnum:]_]+)";  // Matches email addresses
    const char* test_string = "Contact: john@example.com";

    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Email found: %.*s\n", match.length, match.start);
    } 
    else {
        fmt_printf("No match found.\n");
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result:**
```
Email found: john@example.com
```

---

### Example 10: Repeated Matching (Finding Repeated Substrings)

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "\\d+";  // Matches sequences of digits
    const char* test_string = "There are 3 apples, 4 oranges, and 123 grapes.";

    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch matches[10];
    int match_count = regex_find_all(regex, test_string, matches, 10);

    fmt_printf("Found %d matches:\n", match_count);
    for (int i = 0; i < match_count; ++i) {
        fmt_printf("Match %d: %.*s\n", i + 1, matches[i].length, matches[i].start);
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result:**
```
Found 3 matches:
Match 1: 3
Match 2: 4
Match 3: 123
```

---

### Example 11: Matching and Capturing Multiple Groups

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "(\\d+)-(\\w+)"; 
    const char* test_string = "ID: 123-apple, 456-banana, 789-cherry";

    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch matches[10];
    int match_count = regex_find_all(regex, test_string, matches, 10);

    fmt_printf("Found %d matches:\n", match_count);
    for (int i = 0; i < match_count; ++i) {
        fmt_printf("Match %d: %.*s\n", i + 1, matches[i].length, matches[i].start);
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result:**
```
Found 3 matches:
Match 1: 123-apple
Match 2: 456-banana
Match 3: 789-cherry
```

---

### Example 12: Validating and Extracting a Date Format (DD/MM/YYYY)

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "(\\d{2})/(\\d{2})/(\\d{4})"; 
    const char* test_string = "Today's date is 21/10/2024.";

    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Date found: %.*s\n", match.length, match.start);
    } 
    else {
        fmt_printf("No valid date found.\n");
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result:**
```
Date found: 21/10/2024
```

---

### Example 13 : How to use group capture 

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "(\\d{2})/(\\d{2})/(\\d{4})"; 
    const char* test_string = "Today's date is 21/10/2024.";

    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Date found: %.*s\n", match.length, match.start);
        for (int i = 0; i < match.group_count; i++) {
            fmt_printf("Group %d: %.*s\n", i + 1, match.group_lengths[i], match.group_starts[i]);
        }
    } 
    else {
        fmt_printf("No valid date found.\n");
    }

    regex_deallocate(regex);
    return 0;
}
```
**Result**
```
Date found: 21/10/2024
Group 1: 21
Group 2: 10
Group 3: 2024
```

---

### Example 14: Using Multiple Flags (Case-Insensitive, Multiline, and Dotall Mode Together)

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "HELLO.*GOODBYE"; 
    const char* test_string = "hello world!\nThis is a test.\nGoodbye world!";

    Regex* regex = regex_compile(pattern, REGEX_CASE_INSENSITIVE | REGEX_MULTILINE | REGEX_DOTALL);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Match found across lines: %.*s\n", match.length, match.start);
    } 
    else {
        fmt_printf("No match found.\n");
    }

    regex_deallocate(regex);
    return 0;
}
```

**Result:**
```
Match found across lines: hello world!
This is a test.
Goodbye
```

---

### Example 15: Capturing Substrings Using Named Groups (Simulated)

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* pattern = "(\\d+)-([a-zA-Z]+)-([a-zA-Z]+)"; // capturing parts of a string
    const char* test_string = "Order ID: 123-apple-orange";

    Regex* regex = regex_compile(pattern, REGEX_DEFAULT);
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Full match: %.*s\n", (int)match.length, match.start);
        for (int i = 0; i < match.group_count; i++) {
            fmt_printf("Group %d: %.*s\n", i + 1, match.group_lengths[i], match.group_starts[i]);
        }
    } 
    else {
        fmt_printf("No match found.\n");
    }

    regex_deallocate(regex);
    return 0;
}
```

**Result:**
```
Full match: 123-apple-orange
Group 1: 123
Group 2: apple
Group 3: orange
```

---

## License

This library is licensed under the ISC License.