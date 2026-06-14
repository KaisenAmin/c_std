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

### `void regex_match_free(RegexMatch* match)`

- **Purpose**: Frees the per-match capture-group buffers (`group_starts` and `group_lengths`) that `regex_match` / `regex_search` / `regex_find_all` allocate on every successful match. Call this once you're done with a `RegexMatch`; for arrays of matches (e.g. from `regex_find_all`), call once per entry.
- **Parameters**:
  - `match`: The `RegexMatch` to free. Safe to call with `NULL` or a zeroed/already-freed match.
- **Note**: Without this call, every successful match leaks memory.

---

### `const char* regex_error_message(RegexResult result)`

- **Purpose**: Converts a regex result code to a human-readable error message.
- **Parameters**:
  - `result`: The result code, such as `REGEX_COMPILE_ERROR`.
- **Return**: A string representing the error message.

---

The following seven helpers were added in the production-hardening pass. Each one is pure C built on top of PCRE — no platform-specific code, no global state, no `exit()` calls — and works identically on Windows, Linux, macOS, and BSD.

### `int regex_test(const char* pattern, const char* string, RegexFlags flags)`

- **Purpose**: One-shot "does this match?" check. Compiles, searches, and deallocates internally so the caller doesn't have to manage a `Regex` lifetime.
- **Parameters**:
  - `pattern`: Regex pattern to compile.
  - `string`: Input string to test.
  - `flags`: Regex flags (e.g. `REGEX_CASE_INSENSITIVE`).
- **Return**:
  - `1` if the pattern is found in the string.
  - `0` if it is not found.
  - `-1` on NULL inputs or compile error.

---

### `int regex_full_match(const char* pattern, const char* string, RegexFlags flags)`

- **Purpose**: Whole-string validation — the `std::regex_match` / Python `re.fullmatch` counterpart to `regex_test`. Where `regex_test` asks "does the string *contain* a match?", `regex_full_match` asks "does the pattern match the **entire** string, from the first byte to the last?". This is the primitive to use for input validation (is this a well-formed IPv4 address, UUID, integer, …).
- **Parameters**:
  - `pattern`, `string`, `flags`: Same as `regex_test`.
- **Return**:
  - `1` if the pattern matches the whole string.
  - `0` if it does not.
  - `-1` on NULL inputs, compile error, or OOM.
- **Notes**: The pattern is anchored at both ends internally, so you do **not** write `^`…`$` yourself (any `^`/`$` you do add still works). The end anchor is *absolute*: unlike a bare `$`, a trailing newline does **not** count as a full match. Capture groups inside the pattern are not reported — the function returns only a yes/no answer.

---

### `int regex_count_matches(const char* pattern, const char* string, RegexFlags flags)`

- **Purpose**: Counts every non-overlapping occurrence of `pattern` in `string`.
- **Parameters**:
  - `pattern`, `string`, `flags`: Same as `regex_test`.
- **Return**: Number of matches, or `-1` on error.
- **Notes**: Handles zero-length matches (`\b`, `^$`, …) by advancing one byte per zero-width hit so the walk always terminates.

---

### `char* regex_replace(const char* pattern, const char* string, const char* replacement, RegexFlags flags)`

- **Purpose**: Returns a freshly-allocated copy of `string` with **every** match of `pattern` replaced by the **literal** text `replacement`.
- **Parameters**:
  - `pattern`, `string`, `flags`: As above.
  - `replacement`: Literal text to substitute. Backreferences such as `$1` or `\1` are **not** expanded.
- **Return**: New string (caller must `free()`), or `NULL` on compile failure / OOM / NULL inputs.

---

### `char* regex_replace_first(const char* pattern, const char* string, const char* replacement, RegexFlags flags)`

- **Purpose**: Same as `regex_replace` but only the **first** match is substituted.
- **Return**: New string (caller must `free()`), or `NULL` on error.

---

### `char* regex_replace_groups(const char* pattern, const char* string, const char* replacement, RegexFlags flags)`

- **Purpose**: Like `regex_replace` (replaces **every** match), but the replacement text may contain **backreferences** to captured groups. This is the standard way to *reformat* text — swap fields, restyle a date, wrap matches — without manually splicing strings.
- **Parameters**:
  - `pattern`, `string`, `flags`: As above.
  - `replacement`: Substitution template. Expansion grammar:
    - `$0` → the whole match.
    - `$1`…`$N` → capture group `N` (the maximal run of digits is the group number); a group that does not exist or did not participate expands to the empty string.
    - `$$` → a literal `$`.
    - `$` followed by anything else → a literal `$` then that character.
- **Return**: New string (caller must `free()`), or `NULL` on compile failure / OOM / NULL inputs.
- **Example**: `regex_replace_groups("(\\d{4})-(\\d{2})-(\\d{2})", "2026-06-14", "$3/$2/$1", REGEX_DEFAULT)` → `"14/06/2026"`.

---

### `char** regex_split(const char* pattern, const char* string, RegexFlags flags, int* out_count)`

- **Purpose**: Splits `string` at every non-empty match of `pattern`.
- **Parameters**:
  - `pattern`, `string`, `flags`: As above.
  - `out_count`: Filled with the number of tokens emitted (excluding the trailing `NULL` sentinel).
- **Return**: A heap-allocated, `NULL`-terminated array of heap-allocated C strings, or `NULL` on error. Empty tokens are kept at adjacent separator positions (matches `re.split` in Python).
- **Free with**: `regex_split_free`.

---

### `void regex_split_free(char** tokens, int count)`

- **Purpose**: Frees the token array returned by `regex_split`.
- **Parameters**:
  - `tokens`: Array returned by `regex_split`. `NULL` is safe.
  - `count`: Number of tokens. Pass `-1` to scan for the `NULL` sentinel instead.

---

### `char* regex_match_group(const RegexMatch* match, int group_index)`

- **Purpose**: Copies the requested group out of a `RegexMatch` into a freshly-allocated, NUL-terminated C string.
- **Parameters**:
  - `match`: A successful `RegexMatch` from `regex_match` / `regex_search` / `regex_find_all`.
  - `group_index`:
    - `0` — the whole match.
    - `1..group_count` — the corresponding capturing group.
- **Return**: New string (caller must `free()`), or `NULL` if `match` is `NULL`, the index is out of range, or allocation fails.

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
        fmt_printf("Match found: %.*s\n", (int)match.length, match.start);
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
        fmt_printf("Match %d: %.*s\n", i + 1, (int)matches[i].length, matches[i].start);
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
        fmt_printf("Case-insensitive match found: %.*s\n", (int)match.length, match.start);
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
        fmt_printf("Multiline match found: %.*s\n", (int)match.length, match.start);
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
        fmt_printf("Dotall match found: %.*s\n", (int)match.length, match.start);
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
        fmt_printf("Match %d: %.*s\n", i + 1, (int)matches[i].length, matches[i].start);
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

    Regex* regex = regex_compile(pattern, (RegexFlags)(REGEX_CASE_INSENSITIVE | REGEX_DOTALL));
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Match found: %.*s\n", (int)match.length, match.start);
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
        fmt_printf("Word boundary match found: %.*s\n", (int)match.length, match.start);
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
        fmt_printf("Email found: %.*s\n", (int)match.length, match.start);
    } 
    else {
        fmt_printf("No match found.\n");
    }

    regex_match_free(&match);
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
        fmt_printf("Match %d: %.*s\n", i + 1, (int)matches[i].length, matches[i].start);
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
        fmt_printf("Match %d: %.*s\n", i + 1, (int)matches[i].length, matches[i].start);
        regex_match_free(&matches[i]);
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
        fmt_printf("Date found: %.*s\n", (int)match.length, match.start);
    } 
    else {
        fmt_printf("No valid date found.\n");
    }

    regex_match_free(&match);
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
        fmt_printf("Date found: %.*s\n", (int)match.length, match.start);

        for (int i = 0; i < match.group_count; i++) {
            fmt_printf("Group %d: %.*s\n", i + 1, (int)match.group_lengths[i], match.group_starts[i]);
        }
    } 
    else {
        fmt_printf("No valid date found.\n");
    }

    regex_match_free(&match);
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

    Regex* regex = regex_compile(pattern, (RegexFlags)(REGEX_CASE_INSENSITIVE | REGEX_MULTILINE | REGEX_DOTALL));
    if (!regex) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch match;
    if (regex_search(regex, test_string, &match) == REGEX_SUCCESS) {
        fmt_printf("Match found across lines: %.*s\n", (int)match.length, match.start);
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
            fmt_printf("Group %d: %.*s\n", i + 1, (int)match.group_lengths[i], match.group_starts[i]);
        }
    } 
    else {
        fmt_printf("No match found.\n");
    }

    regex_match_free(&match);
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

### Example 16: One-shot match test with `regex_test`

Use `regex_test` when you just need a yes/no answer and don't want to manage a `Regex` lifetime.

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"

int main() {
    const char* email = "alli.rashiddi@gmail.com";

    if (regex_test("^[^@\\s]+@[^@\\s]+\\.[a-zA-Z]{2,}$", email, REGEX_DEFAULT) == 1) {
        fmt_printf("Looks like an email.\n");
    } 
    else {
        fmt_printf("Not an email.\n");
    }
    return 0;
}
```

**Result:**
```
Looks like an email.
```

---

### Example 17: Counting occurrences with `regex_count_matches`

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"


int main() {
    const char* text = "production-ready, battle-hardened, tested";

    int vowels = regex_count_matches("[aeiou]", text, REGEX_DEFAULT);
    int words  = regex_count_matches("\\b[a-z]+\\b", text, REGEX_DEFAULT);

    fmt_printf("Vowel count: %d\n", vowels);
    fmt_printf("Word count:  %d\n", words);
    
    return 0;
}
```

**Result:**
```
Vowel count: 13
Word count:  5
```

---

### Example 18: Replacing every match with `regex_replace`

`regex_replace` returns a freshly-allocated string the caller owns. Replacement text is **literal** — backreferences such as `$1` are not expanded.

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    char* out = regex_replace("[aeiou]", "production", "*", REGEX_DEFAULT);
    fmt_printf("%s\n", out);  // pr*d*ct**n
    free(out);

    /* Also handy for normalising whitespace. */
    char* trimmed = regex_replace("\\s+", "  hello   world  ", " ", REGEX_DEFAULT);
    fmt_printf("[%s]\n", trimmed);  // [ hello world ]
    free(trimmed);

    return 0;
}
```

**Result:**
```
pr*d*ct**n
[ hello world ]
```

---

### Example 19: Replacing only the first match with `regex_replace_first`

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    char* out = regex_replace_first("[0-9]+", "abc123def456", "###", REGEX_DEFAULT);
    fmt_printf("%s\n", out);  // abc###def456

    free(out);
    return 0;
}
```

**Result:**
```
abc###def456
```

---

### Example 20: Splitting a string with `regex_split` + `regex_split_free`

`regex_split` keeps empty tokens at adjacent separators (matches Python's `re.split`).

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"


int main() {
    int n = 0;
    char** parts = regex_split("\\s*,\\s*",
                               "alpha, beta ,  gamma",
                               REGEX_DEFAULT, &n);

    for (int i = 0; i < n; i++) {
        fmt_printf("[%d] %s\n", i, parts[i]);
    }

    regex_split_free(parts, n);
    return 0;
}
```

**Result:**
```
[0] alpha
[1] beta
[2] gamma
```

---

### Example 21: Extracting groups with `regex_match_group`

`regex_match_group(&m, 0)` returns the whole match; positive indices return capturing groups.

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    Regex* r = regex_compile("(\\d{4})-(\\d{2})-(\\d{2})", REGEX_DEFAULT);
    RegexMatch m;

    if (regex_search(r, "Release on 2026-05-26.", &m) == REGEX_SUCCESS) {
        char* whole = regex_match_group(&m, 0);
        char* year  = regex_match_group(&m, 1);
        char* month = regex_match_group(&m, 2);
        char* day   = regex_match_group(&m, 3);

        fmt_printf("match = %s\n", whole);
        fmt_printf("year  = %s\n", year);
        fmt_printf("month = %s\n", month);
        fmt_printf("day   = %s\n", day);

        free(whole); 
        free(year); 
        free(month); 
        free(day);
        regex_match_free(&m);
    }

    regex_deallocate(r);
    return 0;
}
```

**Result:**
```
match = 2026-05-26
year  = 2026
month = 05
day   = 26
```

---

### Example 22: End-to-end pipeline — split, filter, replace

Combining the new helpers gives a concise "data cleanup" pipeline.

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* csv = "alpha,beta42,gamma,delta7,epsilon";

    /* 1) Split into fields. */
    int n = 0;
    char** fields = regex_split(",", csv, REGEX_DEFAULT, &n);

    /* 2) Count fields that contain a digit. */
    int with_digit = 0;
    for (int i = 0; i < n; i++) {
        if (regex_test("[0-9]", fields[i], REGEX_DEFAULT) == 1) {
            with_digit++;
        }
    }
    fmt_printf("Fields containing a digit: %d / %d\n", with_digit, n);

    /* 3) Anonymise the letter-runs. */
    char* anon = regex_replace("[a-z]+", csv, "X", REGEX_DEFAULT);
    fmt_printf("Anonymised: %s\n", anon);
    free(anon);

    regex_split_free(fields, n);
    return 0;
}
```

**Result:**
```
Fields containing a digit: 2 / 5
Anonymised: X,X42,X,X7,X
```

---

### Example 23: Building a report from matches with the `String` library

Pair `regex_find_all` with the `String` library to extract every lowercase word, upper-case it, and join the results into a single report string.

```c
#include "regex/std_regex.h"
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    const char* text = "the quick brown fox";

    Regex* re = regex_compile("[a-z]+", REGEX_DEFAULT);
    if (!re) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch matches[16];
    int n = regex_find_all(re, text, matches, 16);

    /* Build an UPPER-CASED, comma-joined report with the String library. */
    String* report = string_create("");
    for (int i = 0; i < n; ++i) {
        char* word = regex_match_group(&matches[i], 0);  /* whole match   */
        String* w  = string_create(word);
        char* up   = string_to_upper(w);                 /* heap C-string */

        if (i > 0) {
            string_append(report, ", ");
        }
        string_append(report, up);

        free(up);
        string_deallocate(w);
        free(word);
        regex_match_free(&matches[i]);
    }

    fmt_printf("Found %d words\n", n);
    fmt_printf("Report: %s\n", string_c_str(report));

    string_deallocate(report);
    regex_deallocate(re);
    return 0;
}
```

**Result:**
```
Found 4 words
Report: THE, QUICK, BROWN, FOX
```

---

### Example 24: Collecting matches into a `Vector`

Use `regex_find_all` to pull every HTTP status code out of a log line and store each one as a heap string inside a `Vector`.

```c
#include "regex/std_regex.h"
#include "vector/vector.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    const char* log = "GET /index 200, POST /login 403, GET /data 200, DELETE /x 500";

    Regex* re = regex_compile("\\b[1-5][0-9][0-9]\\b", REGEX_DEFAULT); /* HTTP codes */
    if (!re) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch matches[16];
    int n = regex_find_all(re, log, matches, 16);

    /* Collect every matched status code into a Vector of heap strings. */
    Vector* codes = vector_create(sizeof(char*));
    for (int i = 0; i < n; ++i) {
        char* code = regex_match_group(&matches[i], 0);
        vector_push_back(codes, &code);
        regex_match_free(&matches[i]);
    }

    fmt_printf("Collected %zu status codes:\n", vector_size(codes));
    for (size_t i = 0; i < vector_size(codes); ++i) {
        char* code = *(char**)vector_at(codes, i);
        fmt_printf("  [%zu] %s\n", i, code);
        free(code);
    }

    vector_deallocate(codes);
    regex_deallocate(re);
    return 0;
}
```

**Result:**
```
Collected 4 status codes:
  [0] 200
  [1] 403
  [2] 200
  [3] 500
```

---

### Example 25: A tiny `grep` over a file written with `file_io`

Write a log file with the `FileWriter`, then read it back line-by-line with the `FileReader` and print only the lines that match — a minimal `grep`.

```c
#include "regex/std_regex.h"
#include "file_io/file_writer.h"
#include "file_io/file_reader.h"
#include "fmt/fmt.h"
#include <stdio.h>

int main(void) {
    const char* path = "rgx_grep_demo.txt";

    /* 1) Write a small log file with the FileWriter. */
    FileWriter* w = file_writer_open(path, WRITE_TEXT);
    file_writer_write_fmt(w, "2026-05-30 INFO  service started\n");
    file_writer_write_fmt(w, "2026-05-30 WARN  disk almost full\n");
    file_writer_write_fmt(w, "2026-05-30 ERROR connection refused\n");
    file_writer_write_fmt(w, "2026-05-30 INFO  request handled\n");
    file_writer_write_fmt(w, "2026-05-30 ERROR timeout reached\n");
    file_writer_close(w);

    /* 2) grep: print only the lines that match, using regex_test per line. */
    FileReader* r = file_reader_open(path, READ_TEXT);
    char line[256];
    int hits = 0;
    while (file_reader_read_line(line, sizeof(line), r)) {
        if (regex_test("\\b(WARN|ERROR)\\b", line, REGEX_DEFAULT) == 1) {
            fmt_printf("%s\n", line);
            hits++;
        }
    }
    file_reader_close(r);

    fmt_printf("matched %d line(s)\n", hits);

    remove(path);
    return 0;
}
```

**Result:**
```
2026-05-30 WARN  disk almost full
2026-05-30 ERROR connection refused
2026-05-30 ERROR timeout reached
matched 3 line(s)
```

---

### Example 26: Base64-encoding a captured group with the `encoding` library

Capture a token with a group, then hand the extracted text to the `encoding` library's Base64 encoder.

```c
#include "regex/std_regex.h"
#include "encoding/encoding.h"
#include "fmt/fmt.h"
#include <string.h>
#include <stdlib.h>

int main(void) {
    const char* input = "token=OpenSesame; ttl=60";

    Regex* re = regex_compile("token=([A-Za-z0-9]+)", REGEX_DEFAULT);
    if (!re) {
        fmt_printf("Failed to compile regex.\n");
        return 1;
    }

    RegexMatch m;
    if (regex_search(re, input, &m) == REGEX_SUCCESS) {
        char* secret = regex_match_group(&m, 1);   /* capturing group 1 */

        /* Base64-encode the extracted secret with the encoding library. */
        char* b64 = encoding_base64_encode(secret, strlen(secret));
        fmt_printf("secret    = %s\n", secret);
        fmt_printf("base64    = %s\n", b64);

        free(b64);
        free(secret);
        regex_match_free(&m);
    } 
    else {
        fmt_printf("No token found.\n");
    }

    regex_deallocate(re);
    return 0;
}
```

**Result:**
```
secret    = OpenSesame
base64    = T3BlblNlc2FtZQ==
```

---

### Example 27: Counting categories into a fixed-size `array`

`regex_count_matches` is perfect for tallying. Here the three tallies are stored in a compile-time `array` and summed.

```c
#include "regex/std_regex.h"
#include "array/array.h"
#include "fmt/fmt.h"

array_create(int, 3, Counts3);

int main(void) {
    const char* text = "cat dog cat bird dog cat fish dog bird";

    /* Count three categories with regex_count_matches, store in a fixed array. */
    Counts3 counts;
    array_at(counts, 0) = regex_count_matches("\\bcat\\b",  text, REGEX_DEFAULT);
    array_at(counts, 1) = regex_count_matches("\\bdog\\b",  text, REGEX_DEFAULT);
    array_at(counts, 2) = regex_count_matches("\\bbird\\b", text, REGEX_DEFAULT);

    const char* names[3] = { "cat", "dog", "bird" };
    int total = 0;
    for (size_t i = 0; i < array_size(counts); ++i) {
        fmt_printf("%-4s : %d\n", names[i], array_at(counts, i));
        total += array_at(counts, i);
    }
    fmt_printf("total: %d\n", total);
    return 0;
}
```

**Result:**
```
cat  : 3
dog  : 3
bird : 2
total: 8
```

---

### Example 28: Validating input with `regex_full_match` and reformatting with `regex_replace_groups`

`regex_full_match` is the validation primitive — it returns true only when the
**entire** string matches (contrast with `regex_test`, which only checks for a
*contained* match). `regex_replace_groups` expands `$1`, `$2`, … backreferences
in the replacement, so you can reformat text — swap fields, restyle a date —
in a single call.

```c
#include "regex/std_regex.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    /* --- Validation with regex_full_match (the whole string must match) --- */
    const char* ipv4 = "(\\d{1,3}\\.){3}\\d{1,3}";
    const char* inputs[] = { "192.168.0.1", "192.168.0.1x", "10.0.0" };

    fmt_printf("IPv4 validation (full match vs. contains):\n");
    for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
        int full = regex_full_match(ipv4, inputs[i], REGEX_DEFAULT);
        int has  = regex_test(ipv4, inputs[i], REGEX_DEFAULT);
        fmt_printf("  %-14s full=%d contains=%d\n", inputs[i], full, has);
    }

    /* --- Reformatting with regex_replace_groups (backreferences) --- */
    char* date = regex_replace_groups("(\\d{4})-(\\d{2})-(\\d{2})",
                                      "release on 2026-06-14, patch 2026-07-01",
                                      "$3/$2/$1", REGEX_DEFAULT);
    fmt_printf("\nReformatted dates: %s\n", date ? date : "(error)");
    free(date);

    char* name = regex_replace_groups("(\\w+)\\s+(\\w+)", "Ada Lovelace",
                                      "$2, $1", REGEX_DEFAULT);
    fmt_printf("Swapped name:      %s\n", name ? name : "(error)");
    free(name);

    return 0;
}
```

**Result:**
```
IPv4 validation (full match vs. contains):
  192.168.0.1    full=1 contains=1
  192.168.0.1x   full=0 contains=1
  10.0.0         full=0 contains=0

Reformatted dates: release on 14/06/2026, patch 01/07/2026
Swapped name:      Lovelace, Ada
```

---

## License

This library is licensed under the ISC License.