# EvalExpr Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2025  
**License:** ISC License

## Overview

The EvalExpr library in C provides a simple yet powerful tool for evaluating arithmetic expressions. Using the shunting-yard algorithm, this library converts infix expressions (such as `3 + 4 * 2 / (1 - 5) ^ 2 ^ 3`) into Reverse Polish Notation (RPN) and evaluates the result. It supports basic arithmetic operators (addition, subtraction, multiplication, division, exponentiation) and proper handling of parentheses, making it ideal for parsing and evaluating mathematical expressions in C projects.

## Key Features

- **Expression Evaluation:**  
  Parse and evaluate expressions containing integers, floating point numbers, operators (`+`, `-`, `*`, `/`, `^`), and parentheses.
  
- **Reverse Polish Notation Conversion:**  
  Convert an infix expression into its RPN form for debugging or further processing.

- **Error Reporting:**  
  Provides error codes and human-readable error messages for issues during tokenization, conversion, or evaluation.

- **Tokenization:**  
  Exposes the internal tokenization of an expression as an array of strings, which is useful for debugging or advanced parsing tasks.

- **Syntax Validation:**  
  Check if an expression is syntactically valid before attempting evaluation.

## Installation and Compilation

To use the EvalExpr library in your project:

1. **Include the Library:**
   - Place the `evalexpr.h` header and `evalexpr.c` source file in your project (for example, in an `evalexpr` directory).
   - Include the header in your source files:
     ```c
     #include "evalexpr/evalexpr.h"
     ```

2. **Compile Your Code:**
   Use a command similar to the following (adjust paths as needed):
   ```bash
   gcc -std=c11 -O2 -o evalexpr_example evalexpr.c main.c -lm
   ```
   The `-lm` flag is required to link the math library for operations such as `pow()`.

---

## Library Functions

### `double eval_expr(const char* expr)`
**Purpose**: Evaluates an arithmetic infix expression using the shunting-yard algorithm, supporting `+`, `-`, `*`, `/`, `^` and parentheses.
**Parameters**:
- `expr`: The null-terminated expression string.
**Return Value**: The computed result as a `double`, or `NAN` if the expression cannot be parsed or evaluated.
**Usage Case**: Use when you want a quick evaluation and don't need detailed error information.

---

### `double eval_expr_strict(const char* expr, int* error)`
**Purpose**: Evaluates an arithmetic expression and reports detailed error information.
**Parameters**:
- `expr`: The null-terminated expression string.
- `error`: A pointer to an `int` that receives the status code: `EVAL_EXPR_SUCCESS` (0) on success, or one of `EVAL_EXPR_ERROR_TOKENIZE` (-1), `EVAL_EXPR_ERROR_SHUNTING` (-2), `EVAL_EXPR_ERROR_EVAL_RPN` (-3) on failure.
**Return Value**: The computed result as a `double`.
**Usage Case**: Use when you need to distinguish between different failure modes and provide the user with a meaningful error message.

---

### `char* eval_expr_to_rpn_string(const char* expr)`
**Purpose**: Converts an infix arithmetic expression to its Reverse Polish Notation (RPN) form and returns it as a heap-allocated string.
**Parameters**:
- `expr`: The null-terminated expression string.
**Return Value**: A dynamically allocated null-terminated string containing the space-separated RPN tokens, or `NULL` on failure. The caller must `free` the returned string.
**Usage Case**: Use for debugging, logging, or when you need the RPN representation for further processing.

---

### `char** eval_expr_tokenize(const char* expr, int* numTokens)`
**Purpose**: Tokenizes an arithmetic expression into its constituent parts (numbers, operators, parentheses) and returns them as a dynamically allocated array of strings.
**Parameters**:
- `expr`: The null-terminated expression string.
- `numTokens`: A pointer to an `int` that receives the number of tokens found.
**Return Value**: A heap-allocated array of heap-allocated strings, one per token, or `NULL` on failure. The caller must `free` each string and then the array itself.
**Usage Case**: Use for debugging or when you need programmatic access to the individual tokens.

---

### `int eval_expr_is_valid(const char* expr)`
**Purpose**: Checks whether an arithmetic expression is syntactically valid by attempting to tokenize it and convert it to RPN.
**Parameters**:
- `expr`: The null-terminated expression string to validate.
**Return Value**: `1` if the expression is valid, or `0` if it is not.
**Usage Case**: Use to validate user-supplied expressions before evaluating them, to provide early error feedback.

---

### `void eval_expr_print_debug(const char* expr)`
**Purpose**: Prints detailed debugging information about an expression to stdout, including its tokenized form, the corresponding RPN, and the final evaluation result.
**Parameters**:
- `expr`: The null-terminated expression string.
**Return Value**: None.
**Usage Case**: Use during development to trace how an expression is parsed and evaluated step by step.

---

### `const char* eval_expr_error_message(int error)`
**Purpose**: Returns a human-readable description of an evaluation error code.
**Parameters**:
- `error`: The error code returned by `eval_expr_strict` (one of the `EVAL_EXPR_ERROR_*` constants or `EVAL_EXPR_SUCCESS`).
**Return Value**: A pointer to a static string describing the error.
**Usage Case**: Use together with `eval_expr_strict` to display meaningful error messages when evaluation fails.

---

## Examples

Below are a few examples that demonstrate how to use the EvalExpr library.

### Example 1: Evaluating an Expression

```c
#include "fmt/fmt.h"
#include "evalexpr/evalexpr.h"
#include <stdlib.h>

int main(void) {
    const char *expr = "3 + 4 * 2 / (1 - 5) ^ 2 ^ 3";
    double result = eval_expr(expr);

    fmt_printf("Expression: %s\nResult: %g\n", expr, result);

    return EXIT_SUCCESS;
}
```

**Result**
```
Expression: 3 + 4 * 2 / (1 - 5) ^ 2 ^ 3
Result: 3.00012
```

---

### Example 2: Using Strict Evaluation and Error Reporting

```c
#include "fmt/fmt.h"
#include "evalexpr/evalexpr.h"
#include <stdlib.h>

int main(void) {
    const char *expr = "3 + 4 * 2 / (1 - 5) ^ 2 ^ 3";
    int error = 0;
    double result = eval_expr_strict(expr, &error);
    
    if (error != EVAL_EXPR_SUCCESS) {
        fmt_printf("Error evaluating expression: %s\n", eval_expr_error_message(error));
    } 
    else {
        fmt_printf("Expression: %s\nResult: %g\n", expr, result);
    }

    return EXIT_SUCCESS;
}
```
**Result**
```
Expression: 3 + 4 * 2 / (1 - 5) ^ 2 ^ 3
Result: 3.00012
```

---

### Example 3: Converting an Expression to RPN

```c
#include "fmt/fmt.h"
#include "evalexpr/evalexpr.h"
#include <stdlib.h>

int main(void) {
    const char *expr = "(2 + 3) * 4";
    char *rpn = eval_expr_to_rpn_string(expr);
    
    if (rpn) {
        fmt_printf("Expression: %s\nRPN: %s\n", expr, rpn);
        free(rpn);
    } 
    else {
        fmt_printf("Failed to convert expression to RPN.\n");
    }

    return EXIT_SUCCESS;
}
```

**Result**
```
Expression: (2 + 3) * 4
RPN: 2 3 + 4 *
```

---

### Example 4: Tokenizing an Expression

```c
#include "fmt/fmt.h"
#include "evalexpr/evalexpr.h"
#include <stdlib.h>

int main(void) {
    const char *expr = "3.5 + 2.5";
    int numTokens = 0;
    char **tokens = eval_expr_tokenize(expr, &numTokens);
    
    if (tokens) {
        for (int i = 0; i < numTokens; i++) {
            fmt_printf("Token %d: %s\n", i, tokens[i]);
            free(tokens[i]);
        }
        free(tokens);
    } 
    else {
        fmt_printf("Failed to tokenize the expression.\n");
    }

    return EXIT_SUCCESS;
}
```

**Result**
```
Token 0: 3.5
Token 1: +
Token 2: 2.5
```

---

### Example 5: Checking Expression Validity

```c
#include "fmt/fmt.h"
#include "evalexpr/evalexpr.h"
#include <stdlib.h>

int main(void) {
    const char *expr = "10 / 2 + 3";

    if (eval_expr_is_valid(expr)) {
        fmt_printf("The expression '%s' is valid.\n", expr);
    } 
    else {
        fmt_printf("The expression '%s' is invalid.\n", expr);
    }

    return EXIT_SUCCESS;
}
```

**Result**
```
The expression '10 / 2 + 3' is valid.
```
---

### Example 6: Comperhensive example 

```c
#include "fmt/fmt.h"
#include "evalexpr/evalexpr.h"
#include <stdlib.h>

int main(void) {
    const char *expr1 = "3 + 4 * 2 / (1 - 5) ^ 2 ^ 3";
    double result1 = eval_expr(expr1);
    fmt_printf("Example 1: Evaluating expression:\n%s\nResult: %g\n\n", expr1, result1);

    const char *expr2 = "3 + 4 * 2 / (1 - 5) ^ 2 ^ 3";
    int error = 0;
    double result2 = eval_expr_strict(expr2, &error);
    if (error != EVAL_EXPR_SUCCESS) {
        fmt_printf("Example 2: Error evaluating expression: %s\n\n", eval_expr_error_message(error));
    } 
    else {
        fmt_printf("Example 2: Expression: %s\nResult: %g\n\n", expr2, result2);
    }

    const char *expr3 = "(2 + 3) * 4";
    char *rpnStr = eval_expr_to_rpn_string(expr3);
    if (rpnStr) {
        fmt_printf("Example 3: Expression: %s\nRPN: %s\n\n", expr3, rpnStr);
        free(rpnStr);
    } 
    else {
        fmt_printf("Example 3: Failed to convert expression to RPN.\n\n");
    }

    const char *expr4 = "3.5 + 2.5";
    int numTokens = 0;
    char **tokens = eval_expr_tokenize(expr4, &numTokens);

    if (tokens) {
        fmt_printf("Example 4: Tokenizing expression: %s\n", expr4);
        for (int i = 0; i < numTokens; i++) {
            fmt_printf("Token %d: %s\n", i, tokens[i]);
            free(tokens[i]);
        }
        free(tokens);
        fmt_printf("\n");
    } 
    else {
        fmt_printf("Example 4: Failed to tokenize the expression.\n\n");
    }

    const char *expr5 = "10 / 2 + 3";
    if (eval_expr_is_valid(expr5)) {
        fmt_printf("Example 5: The expression '%s' is valid.\n\n", expr5);
    } 
    else {
        fmt_printf("Example 5: The expression '%s' is invalid.\n\n", expr5);
    }

    eval_expr_print_debug(expr5);

    return EXIT_SUCCESS;
}
```
**Result**
```
Example 1: Evaluating expression:
3 + 4 * 2 / (1 - 5) ^ 2 ^ 3
Result: 3.00012

Example 2: Expression: 3 + 4 * 2 / (1 - 5) ^ 2 ^ 3
Result: 3.00012

Example 3: Expression: (2 + 3) * 4
RPN: 2 3 + 4 *

Example 4: Tokenizing expression: 3.5 + 2.5
Token 0: 3.5
Token 1: +
Token 2: 2.5

Example 5: The expression '10 / 2 + 3' is valid.

Debug: Tokens:
  Token 0: [NUMBER] 10
  Token 1: [OPERATOR] /
  Token 2: [NUMBER] 2
  Token 3: [OPERATOR] +
  Token 4: [NUMBER] 3
Debug: RPN: 10 2 / 3 +
Debug: Result: 8

```
---

## Contribution

Contributions to the EvalExpr library are welcome. Whether you’d like to extend its capabilities (e.g., to support functions or additional operators), improve error handling, or optimize performance, please fork the repository and submit a pull request.

## License

This project is open-source and available under the [ISC License](LICENSE).
